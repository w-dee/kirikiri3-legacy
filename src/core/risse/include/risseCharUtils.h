//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief risse_char 操作関数群
//---------------------------------------------------------------------------

#ifndef risseCharUtilsH
#define risseCharUtilsH

#include "risseTypes.h"

namespace Risse
{
//---------------------------------------------------------------------------
/*
	Risse では文字を表すプリミティブ型である risse_char では UTF-32 を規定している。
	システムの wchar_t が UTF-32 であることを期待できるシステムでは既存の w 系
	関数を使用するが、そうでない場合は互換の関数を用いる。
*/

risse_int Risse_atoi(const risse_char *s);
risse_char * Risse_int_to_str(risse_int value, risse_char *string);
risse_char * Risse_tTVInt_to_str(risse_int64 value, risse_char *string);
risse_char * Risse_tTVReal_to_str(risse_real value, risse_char *string);
risse_char * Risse_pointer_to_str(const void *value, risse_char *string);
risse_int Risse_straicmp(const risse_char *s1, const risse_char *s2);
void Risse_strcpy_maxlen(risse_char *d, const risse_char *s, size_t len);
void Risse_strcpy(risse_char *d, const risse_char *s);
size_t Risse_strlen(const risse_char *d);


void RisseConvertUTF16ToRisseCharString(risse_char * out, const risse_uint16 * in);


#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	int Risse_strcmp(const risse_char *s1, const risse_char *s2);
	int Risse_strncmp(const risse_char *s1, const risse_char *s2, size_t n);
	risse_char *Risse_strncpy(risse_char *d, const risse_char *s, size_t len);
	risse_char *Risse_strcat(risse_char *d, const risse_char *s);
	risse_char *Risse_strstr(const risse_char *s1, const risse_char *s2);
	risse_char *Risse_strchr(const risse_char *s, int c);
	double Risse_strtod(const risse_char *nptr, risse_char **endptr);
#else
	#define Risse_strcmp			wcscmp
	#define Risse_strncmp			wcsncmp
	#define Risse_strncpy			wcsncpy
	#define Risse_strcat			wcscat
	#define Risse_strstr			wcsstr
	#define Risse_strchr			wcschr
	#define Risse_strtod			wcstod
#endif

#ifdef RISSE_SUPPORT_WX
wxString RisseCharToWxString(const risse_char * str);
#endif

//---------------------------------------------------------------------------
// RISSE_WS 実装(UTF-32リテラル)

#if defined(DEBUG) || defined(_DEBUG) || defined(__WXDEBUG__)
	#define RISSE_CHARUTILS_DEBUG
#endif

#ifdef RISSE_CHARUTILS_DEBUG
	#define RISSE_DEFINE_SOURCE_ID(x) \
		bool RISSE_DEFINE_SOURCE_ID_CHECK_##x = false; \
		static const int RisseUniqueSourceNumber = x
#else
	#define RISSE_DEFINE_SOURCE_ID(x) \
		static const int RisseUniqueSourceNumber = x
#endif


#ifndef RISSE_WCHAR_T_SIZE_IS_16BIT
	// wchar_t がもともと32bitの環境では L"文字列" で UTF32 を生成できることを
	// 期待できるのでこのマクロをそのまま使う
	#define RISSE_WS(X)  (L ## X)
	#define RISSE_WS1(X) (L ## X)
	#define RISSE_WS2(X) (L ## X)
	#define RISSE_WS3(X) (L ## X)
	#define RISSE_WS4(X) (L ## X)
	#define RISSE_WS5(X) (L ## X)
	#define RISSE_WS6(X) (L ## X)
	#define RISSE_WS7(X) (L ## X)
	#define RISSE_WS8(X) (L ## X)
	#define RISSE_WS9(X) (L ## X)
#else
	// wchar_t が 16bit の環境では UTF32 リテラルをそのままは生成できないため、
	// 下記のようなトリッキーなテンプレートを使うことになる。

	// VC++ で
	// error C2975: 'UNIQ2' : 'tRisseUtf16ToUtf32' に対するテンプレート引数が無効です。
	//                        コンパイル時に評価可能な定数式が必要です。
	// のようなエラーが出る場合は、オプション /ZI (エディット・コンティニュー用のプログラムデータベース)
	// を /Zi (プログラムデータベースを使用) に変えること。

	// このテンプレートは UTF32 文字列を static な領域に保持することを目的としている。
	// 各文字列は、文字列長、ソースファイルごとにユニークなID、ソースコード行、連番
	// で識別される。

	// 各ソースファイルには必ず 
	// RISSE_DEFINE_SOURCE_ID(番号);
	// を記述すること。番号はソースファイルごとにユニークな値にする。値が重なっていた場合は
	// リンク時に 「"RISSE_DEFINE_SOURCE_ID_CHECK_番号" というシンボルが重複している」
	// と警告される(ただしデバッグビルド時のみ)。

	// 一行中に複数の RISSE_WS を書くことはできない。もし書いた場合は、実行時にそこを通過した
	// 際に例外が発生する(ただしデバッグビルド時のみ)。
	// 一行中に複数のRISSE_WSを書きたい場合は、２個目以降をRISSE_WS2、RISSE_WS3にする。

	#ifdef RISSE_CHARUTILS_DEBUG
	void RisseThrowWSAssertionFailure(const wchar_t * source, risse_int line);
	#endif

	//! @brief UTF-32 リテラル文字列を保持するクラス
	template<size_t SIZE, int SOURCEID, int SOURCELINE, int SEQID>
	class tRisseUtf16ToUtf32
	{
		static risse_char Utf32Array[SIZE];
		static bool Converted;
	#ifdef RISSE_CHARUTILS_DEBUG
		static const wchar_t * SourceString;
	#endif

	public:
	#ifdef RISSE_CHARUTILS_DEBUG
		tRisseUtf16ToUtf32(const wchar_t * utf16, const wchar_t *sourcefile)
	#else
		tRisseUtf16ToUtf32(const wchar_t * utf16)
	#endif
		{
	#ifdef RISSE_CHARUTILS_DEBUG
			if(SourceString && utf16 != SourceString)
				RisseThrowWSAssertionFailure(sourcefile, SOURCELINE);
	#endif
			if(!Converted)
			{
				Converted = true;
				RisseConvertUTF16ToRisseCharString(Utf32Array,
					reinterpret_cast<const risse_uint16 *>(utf16));
	#ifdef RISSE_CHARUTILS_DEBUG
				SourceString = utf16;
	#endif
			}
		}
		operator const risse_char * () const { return Utf32Array; }
	};


	template<size_t SIZE, int SOURCEID, int SOURCELINE, int SEQID>
	bool tRisseUtf16ToUtf32<SIZE, SOURCEID, SOURCELINE, SEQID>::Converted = false;

	template<size_t SIZE, int SOURCEID, int SOURCELINE, int SEQID>
	risse_char tRisseUtf16ToUtf32<SIZE, SOURCEID, SOURCELINE, SEQID>::Utf32Array[SIZE] = {0};

	#ifdef RISSE_CHARUTILS_DEBUG
	template<size_t SIZE, int SOURCEID, int SOURCELINE, int SEQID>
	const wchar_t * tRisseUtf16ToUtf32<SIZE, SOURCEID, SOURCELINE, SEQID>::SourceString = NULL;
	#endif


	#ifdef RISSE_CHARUTILS_DEBUG
		#define _RISSE_TO_WCHAR_STRING(X) (L##X)
		#define RISSE_TO_WCHAR_STRING(X) _RISSE_TO_WCHAR_STRING(X)
		#define RISSE_WS(X)  ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 0>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS1(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 1>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS2(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 2>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS3(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 3>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS4(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 4>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS5(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 5>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS6(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 6>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS7(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 7>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS8(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 8>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS9(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 9>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
	#else
		#define RISSE_WS(X)  ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 0>(L##X))
		#define RISSE_WS1(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 1>(L##X))
		#define RISSE_WS2(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 2>(L##X))
		#define RISSE_WS3(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 3>(L##X))
		#define RISSE_WS4(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 4>(L##X))
		#define RISSE_WS5(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 5>(L##X))
		#define RISSE_WS6(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 6>(L##X))
		#define RISSE_WS7(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 7>(L##X))
		#define RISSE_WS8(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 8>(L##X))
		#define RISSE_WS9(X) ((const risse_char *)tRisseUtf16ToUtf32<sizeof(X), RisseUniqueSourceNumber, __LINE__, 9>(L##X))
	#endif

#endif



/* RISSE_WC マクロは ' ' (文字リテラル) にのみ使う。文字列リテラルにはRISSE_WSを使うこと。 */
#define RISSE_WC(X) (static_cast<risse_char>(L##X))

/* 翻訳の必要がある場合は RISSE_WS_TR を用いる。ただし、これは risse_char ではなくて wchar_t
	  として 表現されることに注意すること */
#if  defined(_) && defined(_WX_DEFS_H_)
	#define RISSE_WS_TR(X) _(X)
	/* translation macro */
#else
	#define RISSE_WS_TR(X) RISSE_WS(X)
#endif


//---------------------------------------------------------------------------
} // namespace Risse
#endif
