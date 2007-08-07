//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//  @brief   Risse 日付parser
//---------------------------------------------------------------------------

#ifndef risseDateParserH
#define risseDateParserH

#include "risseGC.h"
#include "risseLexerUtils.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		日付文字列パーサ
//---------------------------------------------------------------------------
class tDateParser : public tLexerUtility
{
	tString Input; //!< 入力文字列
	const risse_char * Ptr; //!< 解析ポインタの現在位置

public:
	int Year; //!< 年
	int Month; //!< 月
	int Date; //!< 日
	int Day; //!< 曜日
	int Hours; //!< 時
	int Minutes; //!< 分
	int Seconds; //!< 秒
	int Milliseconds; //!< ミリ秒
	int Timezone; //!< タイムゾーン -1159 ～ 1159 (普通は)
	int TimezoneOffset; //!< タイムゾーンに対するオフセット
	bool AMPM; // AM(false), PM(true)

	bool YearSet; //!< 年が記述されていたか？
	bool MonthSet; //!< 月が記述されていたか？
	bool DateSet; //!< 日が記述されていたか？
	bool DaySet; //!< 曜日が記述されていたか？
	bool HoursSet; //!< 時が記述されていたか？
	bool MinutesSet; //!< 分が記述されていたか？
	bool SecondsSet; //!< 秒が記述されていたか？
	bool MillisecondsSet; //!< ミリ秒が記述されていたか？
	bool TimezoneSet; //!< タイムゾーンが記述されていたか？
	bool TimezoneOffsetSet; //!< タイムゾーンに対するオフセットが記述されていたか？
	bool AMPMSet; //!< AM/PM が記述されていたか？

	bool HasError; //!< エラーが発生したか？

public:
	//! @brief		コンストラクタ
	//! @param		str		parse 対象文字列
	tDateParser(const tString & str);

	//! @brief		トークンを得る
	//! @param		value		値の格納先
	//! @return		トークンID
	int GetToken(long & val);

};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
