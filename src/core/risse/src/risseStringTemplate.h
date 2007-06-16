//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tRisseString 型リテラル用テンプレート
//---------------------------------------------------------------------------

#ifndef risseStringTemplateH
#define risseStringTemplateH

/*
	使い方
	tRisseSS<'T','e','s','t'>() 等とすると tRisseString に変換可能な
	static な文字列を得ることが出来る。
	ハッシュなども計算済み。

	' ' でいちいち囲まなければならないのが面倒だし、テンプレート定義がやや
	大きいためコンパイラ泣かせだが、

	・RISSE_WS のように実行時のペナルティがない
	・全く同じ文字列はリンカが一つにまとめてくれる
	  (ので文字列同定が高速)
	・文字列を「型」として扱うことができる (テンプレートの引数に渡せる)

	などの利点がある。
*/


namespace Risse
{
//---------------------------------------------------------------------------
#include "risseStringTemplates.inc"
//---------------------------------------------------------------------------
} // namespace Risse


#endif
