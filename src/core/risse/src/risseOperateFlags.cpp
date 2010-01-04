//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tVariantやtObjectInterfaceのOperateメソッドのflags引数の処理
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseOperateFlags.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(3457,57732,10831,20390,36015,12637,53921,25070);
//---------------------------------------------------------------------------
tString tOperateFlags::AsString() const
{
	// まず属性を文字列化
	tString str = tMemberAttribute(Flags).AsString();

	// フラグを文字列化
	if(Flags & ofMemberEnsure)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		str += RISSE_WS("ofMemberEnsure");
	}

	if(Flags & ofInstanceMemberOnly)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		str += RISSE_WS("ofInstanceMemberOnly");
	}

	if(Flags & ofFinalOnly)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		str += RISSE_WS("ofFinalOnly");
	}

	if(Flags & ofUseClassMembersRule)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		str += RISSE_WS("ofUseClassMembersRule");
	}

	// 戻る
	return str;
}
//---------------------------------------------------------------------------
}

