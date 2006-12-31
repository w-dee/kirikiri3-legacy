//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief メソッドに関する処理
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseVariant.h"
#include "risseMethod.h"

namespace Risse
{
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseMethodArgument::tEmptyMethodArgument
	tRisseMethodArgument::EmptyArgument = {0, {NULL}};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tRisseMethodArgument & tRisseMethodArgument::Allocate(risse_size n)
{
	// このメソッドは、必要なストレージのバイト数を計算して
	// 領域を確保し、tRisseMethodArgument & にキャストして返すだけ。

	// tRisseMethodArgument::argv の配列サイズの差はそのまま
	// ポインタサイズであることを確認
	// 構造体のパッキングによっては構造体サイズの計算を誤る可能性があるため
	// 単純に各メンバのサイズの合計はとらずにすこしトリッキーな式を使う。
	RISSE_ASSERT(
		sizeof(tRisseMethodArgumentOf<2>) -
		sizeof(tRisseMethodArgumentOf<1>) == sizeof(tRisseVariant *));

	void * alloc = new risse_uint8[
		sizeof(tRisseMethodArgumentOf<1>) - sizeof(tRisseVariant *) +
		sizeof(tRisseVariant) * n];

	tRisseMethodArgument & ret = *reinterpret_cast<tRisseMethodArgument*>(alloc);

	ret.argc = n;

	return ret;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risse

