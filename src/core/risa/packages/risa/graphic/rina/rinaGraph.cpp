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
#include "risa/prec.h"
#include "risa/packages/risa/graphic/rina/rinaGraph.h"
#include "risa/common/RisaThread.h"
#include "risa/common/RisseEngine.h"
#include "risseNativeBinder.h"
#include "risseObjectClass.h"
#include "risseStaticStrings.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(46760,31859,55967,19351,13713,9702,23339,64673);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tGraphInstance::tGraphInstance()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGraphInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGraphInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tGraphClass, (tSS<'G','r','a','p','h'>()),
		engine->ObjectClass)
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
/**
 * Graph クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tGraphClass>;
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
}
