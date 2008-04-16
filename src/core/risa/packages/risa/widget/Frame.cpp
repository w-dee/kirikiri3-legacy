//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief フレームクラス
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/widget/Frame.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(4811,32114,33460,19785,53925,19531,59339,65072);
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		Risaのフレームを表す wxFrame 派生クラス用のイベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tFrame, wxFrame)
	EVT_CLOSE(						tFrame::OnClose)
END_EVENT_TABLE()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tFrame::tFrame(tFrameInstance * instance) :
	inherited(NULL, -1, wxT("")),
	tBehavior(this, instance)
{
	Show();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tFrame::~tFrame()
{
	fprintf(stderr, "tFrame::~tFrame()\n");
	fflush(stderr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFrame::OnClose(wxCloseEvent & event)
{
	// onClose を呼び出す
	GetInstance()->Operate(ocFuncCall, NULL, tSS<'o','n','C','l','o','s','e'>(),
			0, tMethodArgument::New(!event.CanVeto()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tFrame::Destroy()
{
	// Internal にウィンドウが破棄されたことを通知する
	NotifyDestroy();

	// 親クラスのメソッドを呼び出す
	return inherited::Destroy();
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
tFrameInstance::tFrameInstance()
{
	Internal = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFrameInstance::construct()
{
	// 特にやること無し
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFrameInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// フレームを作成
	SetWxWindow(new tFrame(this));
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
tFrameClass::tFrameClass(tScriptEngine * engine) :
	tClassBase(tSS<'F','r','a','m','e'>(),
		tClassHolder<tWindowClass>::instance()->GetClass())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFrameClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tFrameClass::ovulate);
	BindFunction(this, ss_construct, &tFrameInstance::construct);
	BindFunction(this, ss_initialize, &tFrameInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tFrameClass::ovulate()
{
	return tVariant(new tFrameInstance());
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		Frame クラスレジストラ
//---------------------------------------------------------------------------
template class tClassRegisterer<
	tSS<'r','i','s','a','.','w','i','d','g','e','t'>,
	tFrameClass>;
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risa

