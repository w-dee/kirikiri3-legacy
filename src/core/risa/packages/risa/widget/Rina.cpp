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
#include "risa/packages/risa/graphic/image/Image.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(58627,32079,6056,17748,10429,30722,59446,14940);
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		Rinaの表示領域を表す wxControl 派生クラス用のイベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRina, wxControl)
	EVT_PAINT(tRina::OnPaint)
END_EVENT_TABLE()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRina::tRina(tRinaInstance * instance, wxWindow * parent) :
	inherited(parent, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
	tBehavior(this, instance)
{
	TestImage =
			tClassHolder<tImageClass>::instance()->GetClass()->Invoke(ss_new).
				ExpectAndGetObjectInterface<tImageInstance>(
				tClassHolder<tImageClass>::instance()->GetClass()
				);
	TestImage->Load(RISSE_WS("/root/media/jpegFullColor.jpg"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRina::~tRina()
{
	// wxFrame や wxDialog 以外はこっちでウィンドウ破棄をハンドリングする

	fprintf(stderr, "tRina::~tRina()\n");
	fflush(stderr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRina::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxRED_BRUSH);

    // Get window dimensions
    wxSize sz = GetClientSize();

    // Our rectangle dimensions
    wxCoord w = 100, h = 50;

    // Center the rectangle on the window, but never
    // draw at a negative position.
    int x = wxMax(0, (sz.x-w)/2);
    int y = wxMax(0, (sz.y-h)/2);

    wxRect rectToDraw(x, y, w, h);

    // For efficiency, do not draw if not exposed
    if (IsExposed(rectToDraw))
        dc.DrawRectangle(rectToDraw);
}
//---------------------------------------------------------------------------



























//---------------------------------------------------------------------------
tRinaInstance::tRinaInstance()
{
	Internal = NULL;
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
		parent.ExpectAndGetObjectInterface<tFrameInstance>(
			tClassHolder<tFrameClass>::instance()->GetClass());

	// Rinaコントロールを作成
	SetWxWindow(new tRina(this, frame->GetWxWindow()));
}
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
tRinaClass::tRinaClass(tScriptEngine * engine) :
	tClassBase(tSS<'R','i','n','a'>(),
		tClassHolder<tWindowClass>::instance()->GetClass())
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


