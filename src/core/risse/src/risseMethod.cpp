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
#include "risseExceptionClass.h"

namespace Risse
{
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseMethodArgument::tEmptyMethodArgument
	tRisseMethodArgument::EmptyArgument = {0, 0, {NULL}};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void RisseThrowBadArgumentCount(risse_size passed, risse_size expected)
{
	tRisseBadArgumentCountExceptionClass::ThrowNormal(passed, expected);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void RisseThrowBadBlockArgumentCount(risse_size passed, risse_size expected)
{
	tRisseBadArgumentCountExceptionClass::ThrowBlock(passed, expected);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseMethodArgument & tRisseMethodArgument::Allocate(risse_size ac, risse_size bc)
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

	void * alloc = new (GC) risse_uint8[
		sizeof(tRisseMethodArgumentOf<1>) - sizeof(tRisseVariant *) +
		sizeof(tRisseVariant) * (ac+bc)];

	tRisseMethodArgument & ret = *reinterpret_cast<tRisseMethodArgument*>(alloc);

	ret.ArgumentCount = ac;
	ret.BlockArgumentCount = bc;

	return ret;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risse

