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
tPropertySet::tPropertySet()
{
	TotalCount = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPropertySet::Add(tPropertyInfo * info)
{
	// info をフリーズ
	info->Freeze();

	// Infos に追加
	risse_size infos_index = Infos.size();
	tInfoAndStartIndex is;
	is.Info = info;
	is.StartIndex = TotalCount;
	Infos.push_back(is);

	// 総数を加算
	risse_size count = info->GetCount();
	TotalCount += count;

	// InfoMap に、インデックスとそれに対応するInfos内インデックスの組を追加
	for(risse_size i = 0; i < count; i++)
		InfoMap.insert(tInfoMap::value_type(i + info->GetBaseIndex(), infos_index));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tPropertySet::tInfoAndStartIndex *
	tPropertySet::GetInfoAndStartIndex(risse_size index) const
{
	tInfoMap::iterator i = InfoMap.find(index);
	if(i == InfoMap.end()) return NULL;
	return &(Infos[i->second]);
}
//---------------------------------------------------------------------------























//---------------------------------------------------------------------------
tProperty::tProperty(PropertySet * prop_set)
{
	PropertySet = prop_set;
	Properties.resize(PropertySet->GetTotalCount());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tProperty::GetValueAt(risse_size index)
{
	tPropertySet::tInfoAndStartIndex * is = PropertySet->GetInfoAndStartIndex(index);
	if(!is) { /* XXX: プロパティが見つからない例外 */ }
	index -= is->Info->GetBaseIndex();
	index += is->StartIndex;
	return Properties[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tProperty::SetValueAt(risse_size index, tVariant & value)
{
	tPropertySet::tInfoAndStartIndex * is = PropertySet->GetInfoAndStartIndex(index);
	if(!is) { /* XXX: プロパティが見つからない例外 */ }
	index -= is->Info->GetBaseIndex();
	index += is->StartIndex;
	Properties[index] = value;
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
}
