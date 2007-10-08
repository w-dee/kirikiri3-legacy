//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief サウンドクラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/window/Window.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(4811,32114,33460,19785,53925,19531,59339,65072);
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		Risaのウィンドウを表す wxFrame 派生クラス用のイベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tWindowFrame, wxFrame)
	EVT_CLOSE(						tWindowFrame::OnClose)
END_EVENT_TABLE()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tWindowFrame::tWindowFrame(tWindowInternal * internal) : wxFrame(NULL, -1, wxT(""))
{
	Internal = internal;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tWindowFrame::~tWindowFrame()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowFrame::OnClose(wxCloseEvent & event)
{
	if(event.CanVeto())
	{
		int result = ::wxMessageBox(wxT("close?"), wxT("close?"), wxYES_NO, this);
		if(result == wxNO)
			event.Veto();
		else
			Destroy();
	}
	else
	{
		Destroy();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tWindowFrame::Destroy()
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
//! @brief		ウィンドウリストを表すクラス
//! @param		ウィンドウは dispose() されない限りこのリストに登録されたままになる。
//!				つまり ウィンドウは明示的に dispose() しないと、
//!				たとえインタプリタからの変数参照が亡くなったとしても永遠にメモリ上に残るので注意。
//---------------------------------------------------------------------------
class tWindowList : public singleton_base<tWindowList>
{
	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	gc_vector<tWindowInternal *> WindowInternals; //!< リスト

public:
	//! @brief		コンストラクタ
	tWindowList()
	{
	}

	//! @brief		デストラクタ
	~tWindowList()
	{
	}

	//! @brief		ウィンドウリストにウィンドウを登録する
	//! @param		window		tWindowInternal のインスタンス
	void Add(tWindowInternal * window)
	{
		volatile tCriticalSection::tLocker cs_holder(CS);
		RISSE_ASSERT(std::find(WindowInternals.begin(), WindowInternals.end(), window) == WindowInternals.end());
		WindowInternals.push_back(window);
	}


	//! @brief		ウィンドウリストからウィンドウを登録削除する
	void Remove(tWindowInternal * window)
	{
		volatile tCriticalSection::tLocker cs_holder(CS);
		gc_vector<tWindowInternal*>::iterator i;
		i = std::find(WindowInternals.begin(), WindowInternals.end(), window);
		RISSE_ASSERT(i != WindowInternals.end());
		WindowInternals.erase(i);
	}
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tWindowInternal::tWindowInternal(tWindowInstance * instance)
{
	Instance = instance;
	Window = new tWindowFrame(this);
	tWindowList::instance()->Add(this);

	Window->Show();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tWindowInternal::~tWindowInternal()
{
	if(Instance)
		tWindowList::instance()->Remove(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInternal::NotifyDestroy()
{
	// tWindowFrame からウィンドウの破棄が伝えられるとき。
	if(Instance)
	{
		tWindowList::instance()->Remove(this);
		Instance->NotifyDestroy(); // インスタンスにも通知する
		Instance = NULL; // インスタンスへの参照を断ち切る
		Window.set(NULL); // ウィンドウもすでに削除されることになっているので削除キューに登録する必要はない
	}
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tWindowInstance::tWindowInstance()
{
	Internal = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::NotifyDestroy()
{
	Internal = NULL;
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

	// ウィンドウを作成
	Internal = new tWindowInternal(this);
}
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
//! @brief		"Window" クラス
//---------------------------------------------------------------------------
class tWindowClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tWindowClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tWindowClass::tWindowClass(tScriptEngine * engine) :
	tClassBase(tSS<'W','i','n','d','o','w'>(),
	tRisseClassRegisterer<tEventSourceClass>::instance()->GetClassInstance())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tWindowClass::ovulate);
	BindFunction(this, ss_construct, &tWindowInstance::construct);
	BindFunction(this, ss_initialize, &tWindowInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tWindowClass::ovulate()
{
	return tVariant(new tWindowInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Window クラスレジストラ
template class tRisseClassRegisterer<tWindowClass>;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risa


