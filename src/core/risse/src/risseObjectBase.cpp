/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトクラスの基底実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risseObjectBase.h"
#include "risseOperateFlags.h"

/*
	ここではオブジェクトの実装に必要な基底の機能を実装する
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(45114,31718,49668,18467,56195,41722,1990,5427);

//---------------------------------------------------------------------------
bool tRisseObjectBase::Read(const tRisseString & name, tRisseOperateFlags flags,
	const tRisseVariant &This, tRisseVariant &result)
{
	tMemberData * member = HashTable.Find(name);

	if(!member) return false; // 見つからなかった

	// TODO: プロパティアクセス、属性チェックなどなど
	result = member->Value;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseObjectBase::Write(const tRisseString & name, tRisseOperateFlags flags,
	const tRisseVariant &This, const tRisseVariant &value)
{
	tMemberData * member;

	if(flags & tRisseOperateFlags::ofMemberEnsure)
	{
		// メンバを新規作成する
		// TODO: プロパティアクセス、属性チェックなどなど

		HashTable.Add(name, tMemberData(tMemberData(value, flags)));
	}
	else
	{
		member = HashTable.Find(name);

		if(!member) return false; // 見つからなかった
		// TODO: プロパティアクセス、属性チェックなどなど

		member->Value = value;
	}

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectBase::tRetValue tRisseObjectBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	switch(code)
	{
	case ocDGet:
		// property get
		if(!Read(name, flags, This, *result))
			return rvMemberNotFound;
		return rvNoError;

	case ocDSet:
		// property set
		if(args.GetCount() < 1) { ; /* TODO: raise an error */ }
		if(!Write(name, flags, This, args[0]))
			return rvMemberNotFound;
		return rvNoError;

	default:
		; // TODO: unhandled operation code support
	}
	return rvNoError;
}
//---------------------------------------------------------------------------

} /* namespace Risse */

