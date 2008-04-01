//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA ID管理
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "visual/rina/rinaIdRegistry.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(47046,39674,50984,17886,46749,51427,61988,64154);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tIdRegistry::tIdRegistry()
{
	RenderGeneration = 1;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool tIdRegistry::RegisterEdgeData(const tEdgeData & data)
{
	volatile tCriticalSection::tLocker holder(CS);

	std::pair<tEdgeDataMap::iterator, bool> result =
		EdgeDataMap.insert(tEdgeDataMap::value_type(data.Id, data));
	return result.second;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tIdRegistry::GetNewRenderGeneration()
{
	volatile tCriticalSection::tLocker holder(CS);
	risse_uint32 gen = RenderGeneration++;
	if(RenderGeneration == 0) RenderGeneration = 1; // 0 は使わない
	return gen;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}
