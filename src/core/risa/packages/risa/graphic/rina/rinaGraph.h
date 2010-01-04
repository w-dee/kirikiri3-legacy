//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA 接続グラフ管理
//---------------------------------------------------------------------------
#ifndef RINAGRAPH_H
#define RINAGRAPH_H

#include "risa/common/RisaThread.h"
#include "risseClass.h"

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 接続グラフ管理
 */
class tGraphInstance : public tObjectBase
{
public:
	typedef tObjectBase inherited;

public:

	/**
	 * コンストラクタ
	 */
	tGraphInstance();

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * "Graph" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tGraphClass, tClassBase, tGraphInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}

#endif
