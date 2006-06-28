//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オペレーションコード/VM命令定義
//---------------------------------------------------------------------------
#include "risseOpCodes.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(2759,19905,41807,16730,27783,55397,28774,14847);
//---------------------------------------------------------------------------
} // namespace Risse


// 名前表の読み込み
// (ヘッダファイル中でもう一度 namespace Risse に入って
// しまうので、ここは namespace Risse 外に置く)
#undef risseOpCodesH
#define RISSE_OC_DEFINE_NAMES
#include "risseOpCodes.h"




//---------------------------------------------------------------------------
void tRisseVMCodeIterator::GetInsnSize() const
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVMCodeIterator::Dump() const
{
}
//---------------------------------------------------------------------------
