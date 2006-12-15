/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトクラスの基底実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risseObjectBase.h"


/*
	ここではオブジェクトの実装に必要な基底の機能を実装する
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(45114,31718,49668,18467,56195,41722,1990,5427);

//---------------------------------------------------------------------------
bool tRisseObjectBase::Read(const tRisseString & name, tjs_uint32 flags,
	const tRisseVariant &This, tRisseVariant &result)
{
	tMemberData * member = HashTable.Find(name);

	if(!member) return false; // 見つからなかった

	// TODO: プロパティアクセス、属性チェックなどなど
	result = member->Value;

	return true;
}
//---------------------------------------------------------------------------
bool tRisseObjectBase::Write(const tRisseString & name, tjs_uint32 flags,
	const tRisseVariant &This, const tRisseVariant &value)
{
	tMemberData * member = HashTable.Find(name);

	if(!member) return false; // 見つからなかった

	// TODO: プロパティアクセス、属性チェックなどなど
	member->Value = value;

	return true;
}
//---------------------------------------------------------------------------

} /* namespace Risse */

