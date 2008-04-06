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
#include "risa/prec.h"
#include "risa/packages/risa/graphic/rina/rinaGraph.h"
#include "risa/common/RisaThread.h"
#include "risse/include/risseNativeBinder.h"
#include "risse/include/risseObjectClass.h"
#include "risse/include/risseStaticStrings.h"


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
tGraphClass::tGraphClass(tScriptEngine * engine) :
	inherited(tSS<'G','r','a','p','h'>(), engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGraphClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tGraphClass::ovulate);
	BindFunction(this, ss_construct, &tGraphInstance::construct);
	BindFunction(this, ss_initialize, &tGraphInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tGraphClass::ovulate()
{
	return tVariant(new tGraphInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}
