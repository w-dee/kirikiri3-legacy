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
//! @brief RINA ID管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/rina/rinaIdRegistry.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID();
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tIdRegistry::tIdRegistry()
{
	PropertyIndex = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tIdRegistry::GetNewPropertyIndex()
{
	volatile tCriticalSection::tLocker lock(CS);
	PropertyIndex += PropertyIndexIncrement;
	return PropertyIndex;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}
