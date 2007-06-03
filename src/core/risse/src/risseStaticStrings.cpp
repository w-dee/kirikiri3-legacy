//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スタティックな文字列テーブル
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseString.h"
#include "risseStaticStrings.h"

/*
	このモジュールは、スタティックな tRisseString への参照を保持する。
	これには以下の二つの目的がある
	(1) 内容が固定された文字列の tRisseString 生成時のコストをなくす
	(2) 同じ文字列への参照を得ることでメモリ効率の向上や文字列比較の
	    速度向上を図る

	ここに記したい文字列は、参照用の名前とともに risseStaticStrings.txt に
	記述し、変換用スクリプトにより risseStringString.inc に変換される。

	似たような機構は risseOpCodes にもあるが、risseOpCodes の方はオペレーション
	コードに関連する文字列のみとなっている。
*/


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(42208,23419,40632,19449,10161,42357,6322,51208);
//---------------------------------------------------------------------------

#include "risseStaticStringsData.inc"

//---------------------------------------------------------------------------
} // namespace Risse



