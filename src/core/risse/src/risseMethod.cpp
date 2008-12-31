//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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
tMethodArgument::tEmptyMethodArgument
	tMethodArgument::EmptyArgument = {0, 0, {NULL}};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void ThrowBadArgumentCount(risse_size passed, risse_size expected)
{
	tBadArgumentCountExceptionClass::ThrowNormal(passed, expected);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void ThrowBadBlockArgumentCount(risse_size passed, risse_size expected)
{
	tBadArgumentCountExceptionClass::ThrowBlock(passed, expected);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tMethodArgument & tMethodArgument::Allocate(risse_size ac, risse_size bc)
{
	// このメソッドは、必要なストレージのバイト数を計算して
	// 領域を確保し、tMethodArgument & にキャストして返すだけ。

	// tMethodArgument::argv の配列サイズの差はそのまま
	// ポインタサイズであることを確認
	// 構造体のパッキングによっては構造体サイズの計算を誤る可能性があるため
	// 単純に各メンバのサイズの合計はとらずにすこしトリッキーな式を使う。
	RISSE_ASSERT(
		sizeof(tMethodArgumentOf<2>) -
		sizeof(tMethodArgumentOf<1>) == sizeof(tVariant *));

	void * alloc = new (GC) risse_uint8[
		sizeof(tMethodArgumentOf<1>) - sizeof(tVariant *) +
		sizeof(tVariant) * (ac+bc)];

	tMethodArgument & ret = *static_cast<tMethodArgument*>(alloc);

	ret.ArgumentCount = ac;
	ret.BlockArgumentCount = bc;

	return ret;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risse

