//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 更新を受け付けることができるクラスの基底クラス
//---------------------------------------------------------------------------
#ifndef RINA1DUPDATERECEIVER_H
#define RINA1DUPDATERECEIVER_H

#include "rina1DRegion.h"

namespace Rina {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		テキストの継承可能プロパティ
//---------------------------------------------------------------------------
class t1DUpdateReceiver : public tProcessNode, public Risa::tSubmorph<t1DUpdateReceiver>
{
public:
	typedef tProcessNode inherited;

public:
	//! @brief		コンストラクタ
	t1DUpdateReceiver(tGraph * graph) : inherited(graph) {;}

	//! @brief		内容の更新があったことを伝える(子ノードから呼ばれる)
	//! @param		area		範囲
	virtual void NotifyUpdate(const t1DArea & area) {;}


};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
