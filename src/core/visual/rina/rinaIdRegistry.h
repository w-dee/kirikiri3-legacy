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
#ifndef RINAIDREGISTRY_H
#define RINAIDREGISTRY_H

#include "base/utils/Singleton.h"
#include "visual/rina/rinaIdRegistry.h"

namespace Rina {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		IDレジストリ
//---------------------------------------------------------------------------
class tIdRegistry : public Risa::singleton_base<tIdRegistry>, Risa::manual_start<tIdRegistry>
{
	typedef Risa::singleton_base<tIdRegistry> inherited;

	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

public:
	//! @brief		コンストラクタ
	tIdRegistry();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}

#endif
