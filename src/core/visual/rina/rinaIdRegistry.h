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
	risse_size PropertyIndex; //!< プロパティのインデックス

public:
	static const risse_size PropertyIndexIncrement = 0x100;
		//!< プロパティインデックスの増分 ( = 一度に確保できるプロパティの個数の最大値)

public:
	//! @brief		コンストラクタ
	tIdRegistry();

	//! @brief		新しいプロパティ序数の基数を得る
	risse_size GetNewPropertyIndex();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}

#endif