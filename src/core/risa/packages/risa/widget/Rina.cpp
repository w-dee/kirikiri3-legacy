//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Rina 表示領域クラス
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/widget/Rina.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(58627,32079,6056,17748,10429,30722,59446,14940);
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		Rinaの表示領域を表す wxControl 派生クラス用のイベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRina, wxControl)
END_EVENT_TABLE()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRina::tRina(tRinaInternal * internal, wxWindow * parent) : inherited(parent, -1)
{
	Internal = internal;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRina::~tRina()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRina::Destroy()
{
	// Internal にウィンドウが破棄されたことを通知する
	Internal->NotifyDestroy();

	// Internal を一応切り離す
	Internal = NULL;

	// 親クラスのメソッドを呼び出す
	return inherited::Destroy();
}
//---------------------------------------------------------------------------























//---------------------------------------------------------------------------
tRinaInternal::tRinaInternal(tRinaInstance * instance, tFrameInstance * frame)
{
	Instance = instance;
	Rina = new tRina(this, frame->GetWxWindow());
	tWindowList::instance()->Add(this);

	Rina->Show();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRinaInternal::~tRinaInternal()
{
	if(Instance)
		tWindowList::instance()->Remove(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaInternal::NotifyDestroy()
{
	// tRina からRinaコントロールの破棄が伝えられるとき。
	if(Instance)
	{
		tWindowList::instance()->Remove(this);
		Instance->NotifyDestroy(); // インスタンスにも通知する
		Instance = NULL; // インスタンスへの参照を断ち切る
		Rina.set(NULL); // Rinaコントロールもすでに削除されることになっているので削除キューに登録する必要はない
	}
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tRinaInstance::tRinaInstance()
{
	Internal = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaInstance::NotifyDestroy()
{
	Internal = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tMainThreadAutoPtr<tRina> & tRinaInstance::GetWxWindow() const
{
	volatile tSynchronizer sync(this); // sync
	if(!Internal) tInaccessibleResourceExceptionClass::Throw();

	return Internal->GetRina();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaInstance::construct()
{
	// 特にやること無し
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaInstance::initialize(const tVariant & parent, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// parent から tFrameInstance のインスタンスを取り出す
	tFrameInstance * frame =
		parent.ExpectAndGetObjectInterafce<tFrameInstance>(
			tClassHolder<tFrameClass>::instance()->GetClass());

	// Rinaコントロールを作成
	if(Internal) Internal->GetRina()->Destroy();
	Internal = new tRinaInternal(this, frame);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tRinaInstance::dispose()
{
	// TODO: 呼び出すスレッドのチェックまたはロック

	if(!Internal) tInaccessibleResourceExceptionClass::Throw();

	Internal->GetRina()->Destroy();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaInstance::close(const tMethodArgument &args)
{
	// TODO: 呼び出すスレッドのチェックまたはロック

	if(!Internal) tInaccessibleResourceExceptionClass::Throw();

	bool force = args.HasArgument(0) ? args[0].operator bool() : false;

	Internal->GetRina()->Close(force);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaInstance::onClose(bool force)
{
	// TODO: 呼び出すスレッドのチェックまたはロック

	// デフォルトの動作は dispose メソッドを呼び出すこと
	Operate(ocFuncCall, NULL, tSS<'d','i','s','p','o','s','e'>(),
			0, tMethodArgument::Empty());
}
//---------------------------------------------------------------------------


















//---------------------------------------------------------------------------
tRinaClass::tRinaClass(tScriptEngine * engine) :
	tClassBase(tSS<'R','i','n','a'>(),
		tClassHolder<tEventSourceClass>::instance()->GetClass())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tRinaClass::ovulate);
	BindFunction(this, ss_construct, &tRinaInstance::construct);
	BindFunction(this, ss_initialize, &tRinaInstance::initialize);
	BindFunction(this, tSS<'d','i','s','p','o','s','e'>(), &tRinaInstance::dispose);
	BindFunction(this, tSS<'c','l','o','s','e'>(), &tRinaInstance::close);
	BindFunction(this, tSS<'o','n','C','l','o','s','e'>(), &tRinaInstance::onClose);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tRinaClass::ovulate()
{
	return tVariant(new tRinaInstance());
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		Rina クラスレジストラ
//---------------------------------------------------------------------------
template class tClassRegisterer<
	tSS<'r','i','s','a','.','w','i','d','g','e','t'>,
	tRinaClass>;
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risa


