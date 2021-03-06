//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
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

/**
 * risse_char 版 atoi
 */
risse_int atoi(const risse_char *s);

/**
 * risse_int を risse_char 文字列に変換する
 * @param value		変換したい値
 * @param string	出力先文字列バッファ
 * @return	string パラメータで指定した出力先文字列バッファがそのまま帰る
 * @note	string には十分な余裕があることが前提
 */
risse_char * int_to_str(risse_int value, risse_char *string);

/**
 * risse_int64 を risse_char 文字列に変換する
 * @param value		変換したい値
 * @param string	出力先文字列バッファ
 * @return	string パラメータで指定した出力先文字列バッファがそのまま帰る
 * @note	string には十分な余裕があることが前提
 */
risse_char * int64_to_str(risse_int64 value, risse_char *string);

/**
 * risse_real を risse_char 文字列に変換する
 * @param value		変換したい値
 * @param string	出力先文字列バッファ
 * @return	string パラメータで指定した出力先文字列バッファがそのまま帰る
 * @note	string には十分な余裕があることが前提
 */
risse_char * real_to_str(risse_real value, risse_char *string);

/**
 * ポインタを risse_char 文字列に変換する
 * @param value		変換したい値
 * @param string	出力先文字列バッファ
 * @return	string パラメータで指定した出力先文字列バッファがそのまま帰る
 * @note	string には十分な余裕があることが前提
 */
risse_char * pointer_to_str(const void *value, risse_char *string);

/**
 * 文字列のASCII英字の大文字・小文字同一視比較
 * @param s1	値1
 * @param s2	値2
 * @return	s1 < s2 ? -1 : s1 > s2 ? 1 : 0
 * @note	この関数は ASCII 英字のみを同一視比較する
 */
risse_int straicmp(const risse_char *s1, const risse_char *s2);

/**
 * 最大で指定コードポイント数分だけ文字列をコピー
 * @param d		格納先バッファ
 * @param s		コピー元バッファ
 * @param len	コピーする最大コードポイント数
 * @note	この関数は strncpy と違い、指定コードポイント数分コピーした後、
 *			最大文字数に達していてもいなくても、d の最後に \0 を追加する。
 *			つまり、d は最低、len + 1 コードポイントの領域が確保されて
 *			いなければならない。また、s が len に達していない場合、
 *			d の残りは null では埋められない。
 */
void strcpy_maxlen(risse_char *d, const risse_char *s, risse_size len);

/**
 * 文字列コピー
 * @param d	格納先バッファ
 * @param s	コピー元バッファ
 */
void strcpy(risse_char *d, const risse_char *s);

/**
 * 文字列の長さを得る
 * @param d	文字列
 * @return	文字列長
 */
risse_size strlen(const risse_char *d);

/**
 * UTF16文字列をrisse_char(UTF-32文字列) に変換
 * @param out	出力文字列 (最低でもinと同じ要素数を持つこと) null終端は書き込まれる
 * @param in	入力文字列
 * @return	出力バッファのサイズ (null終端を含まず, risse_char単位)
 */
risse_size ConvertUTF16ToRisseCharString(risse_char * out, const risse_uint16 * in);

/**
 * risse_char(UTF-32文字列) をUTF16文字列に変換
 * @param out		出力文字列(NULL可)  null終端は書き込まれる
 * @param in		入力文字列
 * @param in_len	入力文字列のサイズ (risse_size_max の場合は自動判定)
 * @return	出力バッファのサイズ (null終端を含まず, risse_uint16単位)
 */
risse_size ConvertRisseCharToUTF16String(risse_uint16 * out, const risse_char * in,
						risse_size in_len = risse_size_max);

/**
 * UTF-8 文字列を risse_char 文字列に変換する
 * @param out	出力 risse_char 文字列 (NULL可)
 * @param in	入力 UTF-8 文字列
 * @return	変換後の risse_char のコードポイント数 (null-terminatorを含まず)。
 *			変換に失敗した場合は risse_size_max が帰る
 */
risse_size ConvertUtf8ToRisseCharString(risse_char *out, const char * in);

/**
 * risse_char 文字列を UTF-8 文字列に変換する
 * @param out		出力 UTF-8 文字列 (NULL可)
 * @param in		入力 risse_char 文字列
 * @param in_len	入力文字列のサイズ (risse_size_max の場合は自動判定)
 * @return	変換後の UTF-8 のバイト数 (null-terminatorを含まず)
 */
risse_size ConvertCharToUtf8String(char * out, const risse_char *in, risse_size in_len = risse_size_max);


/**
 * memcmp の risse_char 版。risse_char 単位で比較する。途中にある\0は文字列の終端としては扱わない。
 * @param s1	文字列その1
 * @param s2	文字列その2
 * @param n		コードポイント数
 * @return	s1 < s2 ? -1 : s1 > s2 ? 1 : 0
 * @note	n が 0 の場合は 0 が帰ることは保証されている。
 */
int strbufcmp(const risse_char *s1, const risse_char *s2, risse_size n);


#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	/**
	 * 文字列を比較する
	 * @param s1	文字列その1
	 * @param s2	文字列その2
	 * @return	s1 < s2 ? -1 : s1 > s2 ? 1 : 0
	 */
	int strcmp(const risse_char *s1, const risse_char *s2);

	/**
	 * 文字列を最大でnコードポイントまで比較する
	 * @param s1	文字列その1
	 * @param s2	文字列その2
	 * @param n		最大コードポイント数
	 * @return	s1 < s2 ? -1 : s1 > s2 ? 1 : 0
	 */
	int strncmp(const risse_char *s1, const risse_char *s2, risse_size n);

	/**
	 * 最大で指定コードポイント数分だけ文字列をコピー
	 * @param d		格納先バッファ
	 * @param s		コピー元バッファ
	 * @param len	コピーする最大コードポイント数
	 * @return	d が返る
	 * @note	動作については strncpy と同じ
	 */
	risse_char *strncpy(risse_char *d, const risse_char *s, risse_size len);

	/**
	 * 文字列の連結
	 * @param d	格納先バッファ
	 * @param s	コピー元バッファ
	 * @return	d が返る
	 * @note	動作については strcat と同じ
	 */
	risse_char *strcat(risse_char *d, const risse_char *s);

	/**
	 * 文字列中の部分文字列の検索
	 * @param s1	検索対象文字列(target)
	 * @param s2	検索する文字列(substring)
	 * @return	s1 中の見つかった位置。見つからなかった場合は NULL
	 */
	risse_char *strstr(const risse_char *s1, const risse_char *s2);

	/**
	 * 文字列中のコードポイントの検索
	 * @param s	検索対象文字列(target)
	 * @param c	検索するコードポイント
	 * @return	s  中の見つかった位置。見つからなかった場合は NULL
	 */
	risse_char *strchr(const risse_char *s, int c);

	/**
	 * 数値を表す文字列をdoubleに変換
	 * @param nptr		文字列
	 * @param endptr	数値を表す文字列の次の文字を表すポインタがここに格納される (NULL可)
	 * @return	変換された数値
	 */
	double strtod(const risse_char *nptr, risse_char **endptr);
#else
	static inline int strcmp(const risse_char *s1, const risse_char *s2)
		{ return wcscmp(s1, s2); }
	static inline int strncmp(const risse_char *s1, const risse_char *s2, risse_size n)
		{ return wcsncmp(s1, s2, n); }
	static inline risse_char *strncpy(risse_char *d, const risse_char *s, risse_size len)
		{ return wcsncpy(d, s, len); }
	static inline risse_char *strcat(risse_char *d, const risse_char *s)
		{ return wcscat(d, s); }
	static inline const risse_char *strstr(const risse_char *s1, const risse_char *s2)
		{ return wcsstr(s1, s2); }
	static inline const risse_char *strchr(const risse_char *s, int c)
		{ return wcschr(s, c); }
	static inline double strtod(const risse_char *nptr, risse_char **endptr)
		{ return wcstod(nptr, endptr); }
#endif

/**
 * 指定された文字列を FILE ストリームに出力する
 * @param output	出力先のストリーム
 * @param str		文字列
 */
void FPrint(FILE * output, const risse_char * str);


/**
 * risse_char 型の文字列を wchar_t 配列 に変換する
 * @param str	risse_char*型の文字列
 * @param len	文字列の長さ(コードポイント単位) risse_size_max = 自動判別
 * @return	wchar_t 配列 (ヒープ上に確保されるが GC の回収対象)
 */
wchar_t * CharToWCStringBuffer(const risse_char * str, risse_size len = risse_size_max);


#ifdef RISSE_SUPPORT_WX

/**
 * risse_char 型の文字列を wxString に変換する
 * @param str	risse_char*型の文字列
 * @param len	文字列の長さ(コードポイント単位) risse_size_max = 自動判別
 * @return	wxString型の文字列
 */
wxString CharToWxString(const risse_char * str, risse_size length = risse_size_max);
#endif










//---------------------------------------------------------------------------
// 言語仕様上 コレーションを行わない場面で使用される文字種判定ルーチン
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * ホワイトスペース類かどうかの判定
 * @param ch	文字
 * @return	ホワイトスペース類の時に真
 */
static bool inline iswspace_nc(risse_char ch)
{
	return ch <= 0x20;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 数値類かどうかの判定
 * @param ch	文字
 * @return	数値類の時に真
 */
static bool inline iswdigit_nc(risse_char ch)
{
	return ch >='0' && ch <= '9';
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * アルファベットかどうかの判定
 * @param ch	文字
 * @return	アルファベットの時に真
 * @note	ここでは 0x80 以上の文字はすべてアルファベットと同じ扱いとする
 */
static bool inline iswalpha_nc(risse_char ch)
{
	return
		(ch >= 'a' && ch <= 'z') ||
		(ch >= 'A' && ch <= 'Z') ||
		ch >= 0x80;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * "単語" に使える文字かどうか
 * @param ch	文字
 * @return	アルファベットの時に真
 * @note	ここでは 0x80 以上の文字はすべてアルファベットと同じ扱いとする
 */
static bool inline iswordcha_nc(risse_char ch)
{
	return iswdigit_nc(ch) || iswalpha_nc(ch) || ch == '_';
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
// RISSE_WS 実装(UTF-32リテラル)
//---------------------------------------------------------------------------

#if defined(DEBUG) || defined(_DEBUG) || defined(__WXDEBUG__) || defined(RISSE_DEBUG)
	#define RISSE_CHARUTILS_DEBUG
#endif

#ifdef RISSE_CHARUTILS_DEBUG
	#define RISSE_DEFINE_SOURCE_ID(x1,x2,x3,x4,x5,x6,x7,x8) \
		bool RISSE_SOURCE_ID_DUPLICATION_CHECK_##x1##_##x2##_##x3##_##x4##_##x5##_##x6##_##x7##_##x8 = false; \
		static const int USN1 = x1;\
		static const int USN2 = x2;\
		static const int USN3 = x3;\
		static const int USN4 = x4;\
		static const int USN5 = x5;\
		static const int USN6 = x6;\
		static const int USN7 = x7;\
		static const int USN8 = x8
#else
	#define RISSE_DEFINE_SOURCE_ID(x1,x2,x3,x4,x5,x6,x7,x8) \
		static const int USN1 = x1;\
		static const int USN2 = x2;\
		static const int USN3 = x3;\
		static const int USN4 = x4;\
		static const int USN5 = x5;\
		static const int USN6 = x6;\
		static const int USN7 = x7;\
		static const int USN8 = x8
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
	// error C2975: 'UNIQ2' : 'tUtf16ToUtf32' に対するテンプレート引数が無効です。
	//                        コンパイル時に評価可能な定数式が必要です。
	// のようなエラーが出る場合は、オプション /ZI (エディット・コンティニュー用のプログラムデータベース)
	// を /Zi (プログラムデータベースを使用) に変えること。

	// このテンプレートは UTF32 文字列を static な領域に保持することを目的としている。
	// 各文字列は、文字列長、ソースファイルごとにユニークなID、ソースコード行、連番
	// で識別される。

	// 各ソースファイルには必ず 
	// RISSE_DEFINE_SOURCE_ID(s1,s2,s3,s4,s5,s6,s7,s8);
	// を記述すること。番号s1～s8はソースファイルごとにユニークな値にする。値が重なっていた場合は
	// リンク時に 「"RISSE_DEFINE_SOURCE_ID_CHECK_番号" というシンボルが重複している」
	// と警告される(ただしデバッグビルド時のみ)。
	// Risaの場合、ソースツリーのルートに update_id.sh というユーティリティシェルスクリプトが
	// あり、ソース中に RISSE_DEFINE_SOURCE_ID() と記述してからこのユーティリティを実行すると、
	// 自動的に ユニークな ID を挿入してくれる。

	// 一行中に複数の RISSE_WS を書くことはできない。もし書いた場合は、実行時にそこを通過した
	// 際に例外が発生する(ただしデバッグビルド時のみ)。
	// 一行中に複数のRISSE_WSを書きたい場合は、２個目以降をRISSE_WS2、RISSE_WS3にする。

	// ちなみにこれは UTF32 リテラルを生成するだけに用いる。静的な tString クラスのインスタンス
	// が欲しければ、risseStaticStrings の利用を検討すること。

	#ifdef RISSE_CHARUTILS_DEBUG
	/**
	 * 一行中に複数の RISSE_WS を使用した場合に例外を送出する関数
	 */
	void ThrowWSAssertionFailure(const wchar_t * source, risse_int line);
	#endif

	/**
	 * UTF-32 リテラル文字列を保持するクラス
	 */
	template<risse_size SIZE, int SID1, int SID2, int SID3, int SID4, int SID5, int SID6, int SID7, int SID8, int SOURCELINE, int SEQID>
	class tUtf16ToUtf32
	{
		static risse_char Utf32Array[SIZE];
		static bool Converted;
	#ifdef RISSE_CHARUTILS_DEBUG
		static const wchar_t * SourceString;
	#endif

	public:
	#ifdef RISSE_CHARUTILS_DEBUG
		tUtf16ToUtf32(const wchar_t * utf16, const wchar_t *sourcefile)
	#else
		tUtf16ToUtf32(const wchar_t * utf16)
	#endif
		{
	#ifdef RISSE_CHARUTILS_DEBUG
			if(SourceString && utf16 != SourceString)
				ThrowWSAssertionFailure(sourcefile, SOURCELINE);
	#endif
			if(!Converted)
			{
				Converted = true;
				ConvertUTF16ToRisseCharString(Utf32Array,
					static_cast<const risse_uint16 *>(static_cast<const void *>(utf16)));
	#ifdef RISSE_CHARUTILS_DEBUG
				SourceString = utf16;
	#endif
			}
		}
		operator const risse_char * () const { return Utf32Array; }
	};


	template<risse_size SIZE, int SID1, int SID2, int SID3, int SID4, int SID5, int SID6, int SID7, int SID8, int SOURCELINE, int SEQID>
		bool tUtf16ToUtf32<SIZE, SID1, SID2, SID3, SID4, SID5, SID6, SID7, SID8, SOURCELINE, SEQID>::Converted = false;

	template<risse_size SIZE, int SID1, int SID2, int SID3, int SID4, int SID5, int SID6, int SID7, int SID8, int SOURCELINE, int SEQID>
	risse_char tUtf16ToUtf32<SIZE, SID1, SID2, SID3, SID4, SID5, SID6, SID7, SID8, SOURCELINE, SEQID>::Utf32Array[SIZE] = {0};

	#ifdef RISSE_CHARUTILS_DEBUG
	template<risse_size SIZE, int SID1, int SID2, int SID3, int SID4, int SID5, int SID6, int SID7, int SID8, int SOURCELINE, int SEQID>
	const wchar_t * tUtf16ToUtf32<SIZE, SID1, SID2, SID3, SID4, SID5, SID6, SID7, SID8, SOURCELINE, SEQID>::SourceString = NULL;
	#endif


	#ifdef RISSE_CHARUTILS_DEBUG
		#define _RISSE_TO_WCHAR_STRING(X) (L##X)
		#define RISSE_TO_WCHAR_STRING(X) _RISSE_TO_WCHAR_STRING(X)
		#define RISSE_WS(X)  ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 0>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS1(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 1>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS2(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 2>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS3(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 3>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS4(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 4>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS5(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 5>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS6(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 6>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS7(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 7>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS8(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 8>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
		#define RISSE_WS9(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 9>(L##X, RISSE_TO_WCHAR_STRING(__FILE__)))
	#else
		#define RISSE_WS(X)  ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 0>(L##X))
		#define RISSE_WS1(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 1>(L##X))
		#define RISSE_WS2(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 2>(L##X))
		#define RISSE_WS3(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 3>(L##X))
		#define RISSE_WS4(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 4>(L##X))
		#define RISSE_WS5(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 5>(L##X))
		#define RISSE_WS6(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 6>(L##X))
		#define RISSE_WS7(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 7>(L##X))
		#define RISSE_WS8(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 8>(L##X))
		#define RISSE_WS9(X) ((const risse_char *)tUtf16ToUtf32<sizeof(X), USN1,USN2,USN3,USN4,USN5,USN6,USN7,USN8, __LINE__, 9>(L##X))
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
