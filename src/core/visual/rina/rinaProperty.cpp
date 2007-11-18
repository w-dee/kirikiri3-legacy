//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA プロパティ管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/rina/rinaProperty.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID();
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tPropertyInfo::tPropertyInfo()
{
	Freezed = false;
	BaseIndex = 0; // XXX: 正しい基数の取得
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tPropertyInfo::tItemInfo & tPropertyInfo::Add(const tString & name, const tString & short_desc)
{
	RISSE_ASSERT(!Freezed);
	risse_size newindex = Vector.size();
	tItemInfo * newinfo = new tItemInfo();
	newinfo->Index = newindex;
	newinfo->Name = name;
	newinfo->ShortDesc = short_desc;
	Map.insert(tMap::value_type(name, newinfo));
	Vector.push_back(newinfo);
	return newinfo;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tPropertyInfo::tItemInfo * tPropertyInfo::At(risse_size index) const
{
	if(index >= Vector.size()) return NULL;
	return Vector[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tPropertyInfo::tItemInfo * tPropertyInfo::Find(const tString & name) const
{
	tMap::iterator i = Map.find(name);
	if(i == Map.end()) return NULL;
	return i->second;
}
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
tProperty::tProperty(tPropertyInfo * info)
{
	Info = info;
	info->Freeze();
	Properties.resize(Info->GetCount());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tProperty::GetValueAt(risse_size index)
{
	// XXX 範囲チェック
	index -= Info->GetBaseIndex();
	return Properties[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tProperty::SetValueAt(risse_size index, tVariant & value)
{
	// XXX 範囲チェック
	index -= Info->GetBaseIndex();
	Properties[index] = value;
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
}
