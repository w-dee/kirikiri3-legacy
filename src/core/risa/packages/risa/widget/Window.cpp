//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ウィンドウクラス
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/widget/Window.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(48872,40150,1446,18507,33934,18137,53044,18713);
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tWindowList::tWindowList()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tWindowList::~tWindowList()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowList::Add(void * instance)
{
	volatile tCriticalSection::tLocker cs_holder(CS);
	RISSE_ASSERT(std::find(List.begin(), List.end(), instance) == List.end());
	List.push_back(instance);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowList::Remove(void * instance)
{
	volatile tCriticalSection::tLocker cs_holder(CS);
	gc_vector<void *>::iterator i;
	i = std::find(List.begin(), List.end(), instance);
	RISSE_ASSERT(i != List.end());
	List.erase(i);
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
tWindowInstance::tWindowInstance()
{
	WxWindow = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::NotifyDestroy()
{
	fprintf(stderr, "tWindowInstance::NotifyDestroy()\n");
	fflush(stderr);

	WxWindow = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
wxWindow * tWindowInstance::GetWxWindow() const
{
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	return WxWindow;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::construct()
{
	// 特にやること無し
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// TODO: initialize が2回呼ばれたら？
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tWindowInstance::dispose()
{
	// TODO: 呼び出すスレッドのチェックまたはロック

	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	WxWindow->Destroy();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::close(const tMethodArgument &args)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	bool force = args.Get(0, false);

	WxWindow->Close(force);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::onClose(bool force)
{
	// TODO: 呼び出すスレッドのチェックまたはロック

	// デフォルトの動作は dispose メソッドを呼び出すこと
	Operate(ocFuncCall, NULL, tSS<'d','i','s','p','o','s','e'>(),
			0, tMethodArgument::Empty());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tWindowInstance::get_left() const
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	int v = 0;
	WxWindow->GetPosition(&v, NULL);
	return v;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::set_left(int v)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	int z = 0;
	WxWindow->GetPosition(NULL, &z);
	WxWindow->Move(v, z);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tWindowInstance::get_top() const
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	int v = 0;
	WxWindow->GetPosition(NULL, &v);
	return v;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::set_top(int v)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	int z = 0;
	WxWindow->GetPosition(&z, NULL);
	WxWindow->Move(z, v);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::setPosition(int x, int y)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	WxWindow->Move(x, y);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tWindowInstance::get_width() const
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	int v = 0;
	WxWindow->GetSize(&v, NULL);
	return v;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::set_width(int v)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	wxSize size = WxWindow->GetSize();
	size.SetWidth(v);
	WxWindow->SetSize(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tWindowInstance::get_height() const
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	int v = 0;
	WxWindow->GetSize(NULL, &v);
	return v;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::set_height(int v)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	wxSize size = WxWindow->GetSize();
	size.SetHeight(v);
	WxWindow->SetSize(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::setSize(int w, int h)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	wxSize size(w, h);
	WxWindow->SetSize(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::setDimensions(int x, int y, int w, int h)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	wxRect rect(x, y, w, h);
	WxWindow->SetSize(rect);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tWindowInstance::get_clientWidth() const
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	int w = 0, h = 0;
	WxWindow->GetClientSize(&w, &h);
	return w;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::set_clientWidth(int v)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	wxSize size = WxWindow->GetClientSize();
	size.SetWidth(v);
	WxWindow->SetClientSize(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tWindowInstance::get_clientHeight() const
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	int w = 0, h = 0;
	WxWindow->GetClientSize(&w, &h);
	return h;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::set_clientHeight(int v)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	wxSize size = WxWindow->GetClientSize();
	size.SetHeight(v);
	WxWindow->SetClientSize(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::setClientSize(int w, int h)
{
	// TODO: 呼び出すスレッドのチェックまたはロック
	if(!WxWindow) tInaccessibleResourceExceptionClass::Throw();

	WxWindow->SetClientSize(w, h);
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tWindowClass, (tSS<'W','i','n','d','o','w'>()),
		tClassHolder<tEventSourceClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
	BindFunction(this, tSS<'d','i','s','p','o','s','e'>(), &tWindowInstance::dispose);
	BindFunction(this, tSS<'c','l','o','s','e'>(), &tWindowInstance::close);
	BindFunction(this, tSS<'o','n','C','l','o','s','e'>(), &tWindowInstance::onClose);
	BindProperty(this, tSS<'l','e','f','t'>(), &tWindowInstance::get_left, &tWindowInstance::set_left);
	BindProperty(this, tSS<'t','o','p'>(), &tWindowInstance::get_top, &tWindowInstance::set_top);
	BindFunction(this, tSS<'s','e','t','P','o','s','i','t','i','o','n'>(), &tWindowInstance::setPosition);
	BindFunction(this, tSS<'m','o','v','e'>(), &tWindowInstance::move);
	BindProperty(this, tSS<'w','i','d','t','h'>(), &tWindowInstance::get_width, &tWindowInstance::set_width);
	BindProperty(this, tSS<'h','e','i','g','h','t'>(), &tWindowInstance::get_height, &tWindowInstance::set_height);
	BindFunction(this, tSS<'s','e','t','S','i','z','e'>(), &tWindowInstance::setSize);
	BindFunction(this, tSS<'s','e','t','D','i','m','e','n','s','i','o','n','s'>(), &tWindowInstance::setDimensions);
	BindProperty(this, tSS<'c','l','i','e','n','t','W','i','d','t','h'>(), &tWindowInstance::get_clientWidth, &tWindowInstance::set_clientWidth);
	BindProperty(this, tSS<'c','l','i','e','n','t','H','e','i','g','h','t'>(), &tWindowInstance::get_clientHeight, &tWindowInstance::set_clientHeight);
	BindFunction(this, tSS<'s','e','t','C','l','i','e','n','t','S','i','z','e'>(), &tWindowInstance::setClientSize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * Window クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','w','i','d','g','e','t'>,
	tWindowClass>;
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risa


