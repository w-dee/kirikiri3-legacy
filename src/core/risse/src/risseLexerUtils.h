//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse レキシカル・アナライザ(字句解析器)向けのユーティリティ関数群
//---------------------------------------------------------------------------
#ifndef risseLexerUtilsH
#define risseLexerUtilsH

#include "risseTypes.h"
#include "risseString.h"
#include "risseVariant.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseLexerUtility
{
public: // public static members
	//! @brief		ホワイトスペースのスキップ
	//! @param		ptr		解析ポインタ (実行後、ホワイトスペースの直後にまで移動する)
	//! @return		スクリプトが継続するかどうか
	static bool SkipSpace(const risse_char * & ptr);

	//! @brief		ホワイトスペースのスキップ(ただし改行はスキップしない)
	//! @param		ptr		解析ポインタ (実行後、改行以外のホワイトスペースの直後にまで移動する)
	//! @return		スクリプトが継続するかどうか
	static bool SkipSpaceExceptForNewLine(const risse_char * & ptr);

	//! @brief		chが改行文字の場合に真を返す
	//! @return		chが改行文字かどうか
	static bool IsNewLineChar(const risse_char ch)
	{
		return ch == RISSE_WC('\r') || ch == RISSE_WC('\n');
	}

	//! @brief		改行文字を一つ読み飛ばす
	//! @param		ptr		解析ポインタ (実行後、改行文字をの直後にまで移動する)
	//! @return		スクリプトが継続するかどうか
	//! @note		ptr は改行文字を指していなければならない
	static bool StepNewLineChar(const risse_char * & ptr);

	//! @brief		改行までスキップ
	//! @param		ptr		解析ポインタ (実行後、改行の直後にまで移動する)
	//! @return		スクリプトが継続するかどうか
	static bool SkipToLineEnd(const risse_char * & ptr);

	//! @brief		16進数文字1桁を数値に
	//! @param		ch	文字
	//! @return		文字の表す数値 (-1:解析不能)
	static risse_int HexNum(risse_char ch);

	//! @brief		8進数文字1桁を数値に
	//! @param		ch	文字
	//! @return		文字の表す数値 (-1:解析不能)
	static risse_int OctNum(risse_char ch);

	//! @brief		10進数文字1桁を数値に
	//! @param		ch	文字
	//! @return		文字の表す数値 (-1:解析不能)
	static risse_int DecNum(risse_char ch);

	//! @brief		2進数文字1桁を数値に
	//! @param		ch	文字
	//! @return		文字の表す数値 (-1:解析不能)
	static risse_int BinNum(risse_char ch);

	//! @brief		バックスラッシュエスケープの値を得る
	//! @param		ch	文字
	//! @return		文字の表すコード
	static risse_int UnescapeBackSlash(risse_char ch);

	//! @brief	コメントをスキップしようとした際の状態
	enum tSkipCommentResult
	{
		scrContinue,	//!< スクリプトはまだ続く
		scrEnded,		//!< スクリプトは終わった
		scrNotComment	//!< 実はコメントではなかった
	};

	//! @brief		コメントのスキップ
	//! @param		ptr		解析ポインタ
	//! @return		スキップした結果どうなったか
	static tSkipCommentResult SkipComment(const risse_char * & ptr);

	//! @brief		ptr にある文字列を別の文字列と比較する
	//! @param		ptr		比較する文字列
	//! @param		wrd		比較する別の文字列
	//! @param		isword	単語単位の比較を行う場合に真
	//! @return		単語が一致したかどうか
	static bool StringMatch(const risse_char * & ptr, const risse_char *wrd, bool isword);

	//! @brief InternalParseString の戻り値
	enum tParseStringResult
	{
		psrNone, //!< (エラー)
		psrDelimiter, //!< デリミタにぶつかった
		psrAmpersand, //!< & にぶつかった ( &式; 形式の埋め込み )
		psrDollar  //!< $ にぶつかった ( ${ } 形式の埋め込み )
	};

	//! @brief		文字列を解析する
	//! @param		ptr		解析開始位置 (通常、開始デリミタの直後)
	//! @param		val		解析した文字列を格納する先
	//! @param		delim	デリミタ ( '\'' か '"' )
	//! @param		embexpmode	埋め込み式モードかどうか (@つき文字列リテラルかどうか)
	//! @return		内部ステータス
	static tParseStringResult
		ParseString(const risse_char * & ptr, tRisseString &val,
			risse_char delim, bool embexpmode);

	//! @brief		現在の解析位置にある文字列リテラルを解析する
	//! @param		ptr		解析開始位置 ('\'' or '"' を指していないとならない)
	//! @param		val		値の格納先
	//! @return		値の解析に成功したかどうか
	static bool ParseString(const risse_char * & ptr, tRisseString &val);

	//! @brief		数値として認識できるだけの文字列を ptr から切り出す
	//! @param		ptr		解析開始位置(切り出し後は切り出した後まで移動している)
	//! @param		validdigits	数値に使う文字集合を判定する関数
	//! @param		expmark	指数表記に使われる文字(2文字まで)
	//! @param		isreal	実数を切り出したときに真に設定される(整数の場合は偽)
	//! @return		切り出した文字列(切り出しに失敗した場合、空文字列が帰る)
	static tRisseString ExtractNumber(
		const risse_char * & ptr,
		risse_int (*validdigits)(risse_char ch),
		const risse_char *expmark,  bool &isreal);

	//! @brief		10進以外の実数表現(0xabfp3, 0b10001p2など)を数値に変換する
	//! @param		ptr		解析開始位置
	//! @param		val		結果の格納先
	//! @param		validdigits	数値に使う文字集合を判定する関数
	//! @param		basebits	基数
	//! @return		変換に成功したかどうか
	static bool ParseNonDecimalReal(const risse_char * ptr, risse_real &val,
		risse_int (*validdigits)(risse_char ch), risse_int basebits);

	//! @brief		10進以外の整数表現(0xabf, 0b10001など)を数値に変換する
	//! @param		ptr		解析開始位置
	//! @param		val		結果の格納先
	//! @param		validdigits	数値に使う文字集合を判定する関数
	//! @param		basebits	基数
	//! @return		変換に成功したかどうか
	static bool ParseNonDecimalInteger(const risse_char * ptr, risse_int64 &val, 
		risse_int (*validdigits)(risse_char ch), risse_int basebits);

	//! @brief		10進以外の数値表現を数値に変換する
	//! @param		ptr		解析開始位置(解析終了後は終了点にまで移動している)
	//! @param		val		結果の格納先
	//! @param		validdigits	数値に使う文字集合を判定する関数
	//! @param		basebits	基数
	//! @return		変換に成功したかどうか
	static bool ParseNonDecimalNumber(const risse_char * & ptr, tRisseVariant &val,
		risse_int (*validdigits)(risse_char ch), risse_int base);

	//! @brief		10進実数を数値に変換する
	//! @param		ptr		解析開始位置
	//! @param		val		結果の格納先
	//! @return		変換に成功したかどうか
	static bool ParseDecimalReal(const risse_char *ptr, risse_real &val);

	//! @brief		10進整数を数値に変換する
	//! @param		ptr		解析開始位置
	//! @param		val		結果の格納先
	//! @return		変換に成功したかどうか
	static bool ParseDecimalInteger(const risse_char * ptr, risse_int64 &val);

private:
	//! @brief		数値変換用内部関数
	//! @param		ptr		解析開始位置(解析終了後は終了点にまで移動している)
	//! @param 		val		結果格納先
	//! @return		解析に成功したかどうか
	static bool ParseNumber2(const risse_char * & ptr, tRisseVariant &val);

public: // public static members
	//! @brief		文字列を数値に変換する
	//! @param		ptr		解析開始位置(解析終了後は終了点にまで移動している)
	//! @param 		val		結果格納先
	//! @return		解析に成功したかどうか
	static bool ParseNumber(const risse_char * & ptr, tRisseVariant &val);

	//! @brief		オクテット列を解釈する
	//! @param		ptr		解析開始位置 '<%' を示していること(解析終了後は終了点にまで移動している)
	//! @param 		val		結果格納先
	//! @return		解析に成功したかどうか
	static bool ParseOctet(const risse_char * & ptr, tRisseOctet &val);

	//! @brief		正規表現リテラルを解析する
	//! @param		ptr		解析開始位置(解析終了後は終了点にまで移動している)
	//! @param		pat		パターンの格納先
	//! @param		flags	フラグの格納先
	//! @return		解析に成功したかどうか
	static bool ParseRegExp(const risse_char * & ptr, tRisseString &pat, tRisseString &flags);

};

//---------------------------------------------------------------------------
} // namespace Risse

#endif

