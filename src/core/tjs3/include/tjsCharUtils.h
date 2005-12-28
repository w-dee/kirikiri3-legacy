//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief tjs_char 操作関数群
//---------------------------------------------------------------------------

#ifndef tjsCharUtilsH
#define tjsCharUtilsH

#include "tjsTypes.h"

namespace TJS
{
//---------------------------------------------------------------------------
/*
	TJS3 では文字を表すプリミティブ型である tjs_char では UTF-32 を規定している。
	システムの wchar_t が UTF-32 であることを期待できるシステムでは既存の w 系
	関数を使用するが、そうでない場合は互換の関数を用いる。
*/

tjs_int TJS_atoi(const tjs_char *s);
tjs_char * TJS_int_to_str(tjs_int value, tjs_char *string);
tjs_char * TJS_tTVInt_to_str(tjs_int64 value, tjs_char *string);
tjs_char * TJS_tTVReal_to_str(tjs_real value, tjs_char *string);
tjs_char * TJS_pointer_to_str(const void *value, tjs_char *string);
tjs_int TJS_straicmp(const tjs_char *s1, const tjs_char *s2);
void TJS_strcpy_maxlen(tjs_char *d, const tjs_char *s, size_t len);
void TJS_strcpy(tjs_char *d, const tjs_char *s);
size_t TJS_strlen(const tjs_char *d);


void TJSConvertUTF16ToTJSCharString(tjs_char * out, const tjs_uint16 * in);


#ifdef TJS_WCHAR_T_SIZE_IS_16BIT
	int TJS_strcmp(const tjs_char *s1, const tjs_char *s2);
	int TJS_strncmp(const tjs_char *s1, const tjs_char *s2, size_t n);
	tjs_char *TJS_strncpy(tjs_char *d, const tjs_char *s, size_t len);
	tjs_char *TJS_strcat(tjs_char *d, const tjs_char *s);
	tjs_char *TJS_strstr(const tjs_char *s1, const tjs_char *s2);
	tjs_char *TJS_strchr(const tjs_char *s, int c);
	double TJS_strtod(const tjs_char *nptr, tjs_char **endptr);
#else
	#define TJS_strcmp			wcscmp
	#define TJS_strncmp			wcsncmp
	#define TJS_strncpy			wcsncpy
	#define TJS_strcat			wcscat
	#define TJS_strstr			wcsstr
	#define TJS_strchr			wcschr
	#define TJS_strtod			wcstod
#endif
//---------------------------------------------------------------------------
// TJS_WS 実装(UTF-32リテラル)

#ifndef TJS_WCHAR_T_SIZE_IS_16BIT
	// wchar_t がもともと32bitの環境では L"文字列" で UTF32 を生成できることを
	// 期待できるのでこのマクロをそのまま使う
	#define TJS_WS(X)  (L ## X)
	#define TJS_WS1(X) (L ## X)
	#define TJS_WS2(X) (L ## X)
	#define TJS_WS3(X) (L ## X)
	#define TJS_WS4(X) (L ## X)
	#define TJS_WS5(X) (L ## X)
	#define TJS_WS6(X) (L ## X)
	#define TJS_WS7(X) (L ## X)
	#define TJS_WS8(X) (L ## X)
	#define TJS_WS9(X) (L ## X)
#else
	// wchar_t が 16bit の環境では UTF32 リテラルをそのままは生成できないため、
	// 下記のようなトリッキーなテンプレートを使うことになる。

	// VC++ で
	// error C2975: 'UNIQ2' : 'tTJSUtf16ToUtf32' に対するテンプレート引数が無効です。
	//                        コンパイル時に評価可能な定数式が必要です。
	// のようなエラーが出る場合は、オプション /ZI (エディット・コンティニュー用のプログラムデータベース)
	// を /Zi (プログラムデータベースを使用) に変えること。

	// このテンプレートは UTF32 文字列を static な領域に保持することを目的としている。
	// 各文字列は、文字列長、ソースファイルごとにユニークなID、ソースコード行、連番
	// で識別される。

	// 各ソースファイルには必ず 
	// TJS_DEFINE_SOURCE_ID(番号);
	// を記述すること。番号はソースファイルごとにユニークな値にする。値が重なっていた場合は
	// リンク時に 「"TJS_DEFINE_SOURCE_ID_CHECK_番号" というシンボルが重複している」
	// と警告される(ただしデバッグビルド時のみ)。

	// 一行中に複数の TJS_WS を書くことはできない。もし書いた場合は、実行時にそこを通過した
	// 際に例外が発生する(ただしデバッグビルド時のみ)。
	// 一行中に複数のTJS_WSを書きたい場合は、２個目以降をTJS_WS2、TJS_WS3にする。

	#ifdef DEBUG
	void TJSThrowWSAssertionFailure(const wchar_t * source, tjs_int line)
	#endif

	//! @brief UTF-32 リテラル文字列を保持するクラス
	template<size_t SIZE, int SOURCEID, int SOURCELINE, int SEQID>
	class tTJSUtf16ToUtf32
	{
		static tjs_char Utf32Array[SIZE];
		static bool Converted;
	#ifdef DEBUG
		static const wchar_t * SourceString;
	#endif

	public:
		tTJSUtf16ToUtf32(const wchar_t * utf16)
		{
	#ifdef DEBUG
			if(SourceString && utf16 != SourceString)
				TJSThrowWSAssertionFailure();
	#endif
			if(!Converted)
			{
				Converted = true;
				TJSConvertUTF16ToTJSCharString(Utf32Array,
					reinterpret_cast<const tjs_uint16 *>(utf16));
	#ifdef DEBUG
				SourceString = utf16;
	#endif
			}
		}
		operator const tjs_char * () const { return Utf32Array; }
	};


	template<size_t SIZE, int SOURCEID, int SOURCELINE, int SEQID>
	bool tTJSUtf16ToUtf32<SIZE, SOURCEID, SOURCELINE, SEQID>::Converted = false;

	template<size_t SIZE, int SOURCEID, int SOURCELINE, int SEQID>
	tjs_char tTJSUtf16ToUtf32<SIZE, SOURCEID, SOURCELINE, SEQID>::Utf32Array[SIZE] = {0};

	#ifdef DEBUG
	template<size_t SIZE, int SOURCEID, int SOURCELINE, int SEQID>
	const wchar_t * tTJSUtf16ToUtf32<SIZE, SOURCEID, SOURCELINE, SEQID>::SourceString = NULL;
	#endif


	#ifdef DEBUG
		#define TJS_DEFINE_SOURCE_ID(x) \
			bool TJS_DEFINE_SOURCE_ID_CHECK_##x = false; \
			static const int TJSUniqueSourceNumber = x
	#else
		#define TJS_DEFINE_SOURCE_ID(x) \
			static const int TJSUniqueSourceNumber = x
	#endif

	#define TJS_WS(X)  ((const tjs_char *)tTJSUtf16ToUtf32<sizeof(X), TJSUniqueSourceNumber, __LINE__, 0>(L##X))
	#define TJS_WS1(X) ((const tjs_char *)tTJSUtf16ToUtf32<sizeof(X), TJSUniqueSourceNumber, __LINE__, 1>(L##X))
	#define TJS_WS2(X) ((const tjs_char *)tTJSUtf16ToUtf32<sizeof(X), TJSUniqueSourceNumber, __LINE__, 2>(L##X))
	#define TJS_WS3(X) ((const tjs_char *)tTJSUtf16ToUtf32<sizeof(X), TJSUniqueSourceNumber, __LINE__, 3>(L##X))
	#define TJS_WS4(X) ((const tjs_char *)tTJSUtf16ToUtf32<sizeof(X), TJSUniqueSourceNumber, __LINE__, 4>(L##X))
	#define TJS_WS5(X) ((const tjs_char *)tTJSUtf16ToUtf32<sizeof(X), TJSUniqueSourceNumber, __LINE__, 5>(L##X))
	#define TJS_WS6(X) ((const tjs_char *)tTJSUtf16ToUtf32<sizeof(X), TJSUniqueSourceNumber, __LINE__, 6>(L##X))
	#define TJS_WS7(X) ((const tjs_char *)tTJSUtf16ToUtf32<sizeof(X), TJSUniqueSourceNumber, __LINE__, 7>(L##X))
	#define TJS_WS8(X) ((const tjs_char *)tTJSUtf16ToUtf32<sizeof(X), TJSUniqueSourceNumber, __LINE__, 8>(L##X))
	#define TJS_WS9(X) ((const tjs_char *)tTJSUtf16ToUtf32<sizeof(X), TJSUniqueSourceNumber, __LINE__, 9>(L##X))


#endif



/* TJS_WC マクロは ' ' (文字リテラル) にのみ使う。文字列リテラルにはTJS_WSを使うこと。 */
#define TJS_WC(X) (static_cast<tjs_char>(L##X))

/* 翻訳の必要がある場合は TJS_WS_TR を用いる。ただし、これは tjs_char ではなくて wchar_t
	  として 表現されることに注意すること */
#if  defined(_) && defined(_WX_DEFS_H_)
	#define TJS_WS_TR(X) _(X)
	/* translation macro */
#else
	#define TJS_WS_TR(X) TJS_WS(X)
#endif


//---------------------------------------------------------------------------
} // namespace TJS
#endif
