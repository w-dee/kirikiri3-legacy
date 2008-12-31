//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ウィンドウクラス
//---------------------------------------------------------------------------
#ifndef WIDGETWINDOWH
#define WIDGETWINDOWH

#include "risa/common/RisaException.h"
#include "risa/packages/risa/event/Event.h"
#include "risa/common/RisaGC.h"


namespace Risa {
//---------------------------------------------------------------------------


/*
	メモ

	tWindowInstance::Internal -> tRisaWindowBahavior::tInternal::Window ->
		wxWindow派生クラス (tRisaWindowBahavior<> のサブクラス)

	の構成になっている。

	tWindowInstance < tCollectee
	tRisaWindowBahavior::tInternal < tDestructee

	で、wxWindow派生クラス は GC 管理下のオブジェクトではないので注意。
*/


//---------------------------------------------------------------------------
/**
 * ウィンドウリストを表すクラス
 */
class tWindowList : public singleton_base<tWindowList>
{
	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	gc_vector<void *> List; //!< リスト

public:
	/**
	 * コンストラクタ
	 */
	tWindowList();

	/**
	 * デストラクタ
	 */
	~tWindowList();

	/**
	 * ウィンドウリストにウィンドウを登録する
	 * @param instance	なんらかのインスタンス
	 */
	void Add(void * instance);

	/**
	 * ウィンドウリストからウィンドウを登録削除する
	 * @param instance	なんらかのインスタンス
	 */
	void Remove(void * instance);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * wxWindow 派生クラスは delete ではなくて Destroy メソッドを呼ばないとならない
 */
template <>
class tDestructorCaller_Impl<wxWindow *> : public tMainThreadDestructorQueue::tDestructorCaller
{
	wxWindow * Ptr;
public:
	tDestructorCaller_Impl(wxWindow *  ptr) { Ptr = ptr; }
	virtual ~tDestructorCaller_Impl() { Ptr->Destroy(); }
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
/**
 * Risaのウィンドウを表す wxWindow のサブクラスを作成するときに
 * 一緒に継承させるクラス
 */
template <typename WX_WINDOW_CLASS, typename RISSE_INSTANCE_CLASS>
class tRisaWindowBahavior
{
public:
	typedef tRisaWindowBahavior<WX_WINDOW_CLASS, RISSE_INSTANCE_CLASS> tBehavior;
		//!< 自分自身のクラスのエイリアス

	//-----------------------------------------------------------------------
	/**
	 * ウィンドウの内部実装クラス
	 */
	class tInternal : public tDestructee
	{
		RISSE_INSTANCE_CLASS * Instance; //!< tWindowInstance へのポインタ

	private:
		tMainThreadAutoPtr<tRisaWindowBahavior> Window; //!< ウィンドウへのポインタ

	public:
		/**
		 * コンストラクタ
		 * @param wx_isntance	wxWindow 派生クラスのインスタンス
		 * @param instance		RISSE_INSTANCE_CLASS へのポインタ
		 */
		tInternal(WX_WINDOW_CLASS * wx_instance, RISSE_INSTANCE_CLASS * instance)
		{
			Window = wx_instance;
			Instance = instance;
			tWindowList::instance()->Add(this);
		}

		/**
		 * デストラクタ
		 */
		~tInternal()
		{
			if(Instance)
				tWindowList::instance()->Remove(this);
		}

		/**
		 * インスタンスを得る
		 */
		RISSE_INSTANCE_CLASS * GetInstance() const { return Instance; }

		/**
		 * ウィンドウへのポインタを得る
		 */
		tMainThreadAutoPtr<tRisaWindowBahavior> & GetWindow() { return Window; }

		/**
		 * ウィンドウが破棄されたことを通知する(tRisaWindowBahaviorから呼ばれる)
		 */
		void NotifyDestroy()
		{
			if(Instance)
			{
				tWindowList::instance()->Remove(this);
				Instance->NotifyDestroy(); // インスタンスにも通知する
				Instance = NULL; // インスタンスへの参照を断ち切る
				Window.set(NULL); // ウィンドウもすでに削除されることになっ
								// ているので削除キューに登録する必要はない
			}
		}
	};
	//-----------------------------------------------------------------------

private:
	tInternal * Internal; //!< 内部実装クラス

public:
	/**
	 * コンストラクタ
	 * @param wx_isntance	wxWindow 派生クラスのインスタンス
	 * @param internal		RISSE_INSTANCE_CLASS のインスタンスへのポインタ
	 */
	tRisaWindowBahavior(WX_WINDOW_CLASS * wx_instance, RISSE_INSTANCE_CLASS * instance)
	{
		Internal = new tInternal(wx_instance, instance);
	}

	/**
	 * デストラクタ
	 */
	~tRisaWindowBahavior()
	{
		NotifyDestroy();
	}

	/**
	 * 内容が破棄されたことを通知する
	 */
	void NotifyDestroy()
	{
		if(Internal)
		{
			Internal->NotifyDestroy();
			Internal = NULL;
		}
	}

	/**
	 * ウィンドウの内部実装クラスを得る
	 * @return	ウィンドウの内部実装クラス
	 */
	tInternal * GetInternal() const { return Internal; }

protected:
	/**
	 * Risseインスタンスを得る
	 * @return	Risseインスタンス
	 */
	RISSE_INSTANCE_CLASS * GetInstance() const
	{
		RISSE_ASSERT(Internal != NULL);
		return Internal->GetInstance();
	}

};
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
/**
 * ウィンドウクラスのインスタンス
 */
class tWindowInstance : public tObjectBase
{
private:
	wxWindow * WxWindow; //!< wxWindow へのポインタ

public:
	/**
	 * コンストラクタ
	 */
	tWindowInstance();

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tWindowInstance() {;}

	/**
	 * ウィンドウが破棄されたことを通知する(tWindowInternalから呼ばれる)
	 */
	void NotifyDestroy();

protected:
	/**
	 * wxWindow へのポインタを設定する
	 * @param window	wxWindow へのポインタ
	 * @note	サブクラスで wxWindow へのポインタを作成したらこれを設定すること
	 */
	void SetWxWindow(wxWindow * window) { WxWindow = window; }

public:
	/**
	 * wxWindow へのポインタを取得する
	 * @return	wxWindow へのポインタ
	 */
	wxWindow * GetWxWindow() const;

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

	void dispose(); //!< ウィンドウを破棄する
	void close(const tMethodArgument &args); //!< 「閉じる」ボタンをエミュレートする
	void onClose(bool force); //!< 「閉じる」ボタンが押されたときやclose()メソッドが呼ばれたとき

	int get_left() const;
	void set_left(int v);
	int get_top() const;
	void set_top(int v);
	void setPosition(int x, int y);
	void move(int x, int y){ setPosition(x, y); } // alias to setPosition
	int get_width() const;
	void set_width(int v);
	int get_height() const;
	void set_height(int v);
	void setSize(int w, int h);
	void setDimensions(int x, int y, int w, int h);
	int get_clientWidth() const;
	void set_clientWidth(int v);
	int get_clientHeight() const;
	void set_clientHeight(int v);
	void setClientSize(int w, int h);
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
/**
 * "Window" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tWindowClass, tClassBase, tWindowInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risa


#endif
