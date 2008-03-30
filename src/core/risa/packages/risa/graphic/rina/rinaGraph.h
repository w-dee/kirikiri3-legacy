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
//! @brief RINA 接続グラフ管理
//---------------------------------------------------------------------------
#ifndef RINAGRAPH_H
#define RINAGRAPH_H

#include "base/utils/RisaThread.h"


namespace Rina {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		接続グラフ管理
//---------------------------------------------------------------------------
class tGraph : public tCollectee
{
	Risa::tCriticalSection * CS; //!< このグラフを保護するためのクリティカルセクション

public:
	typedef tCollectee inherited;

public:

	//! @brief		コンストラクタ
	tGraph();

	//! @brief		このグラフを保護するためのクリティカルセクションを得る
	//! @return		このグラフを保護するためのクリティカルセクション
	tCriticalSection & GetCS() const { return *CS; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}

#endif
