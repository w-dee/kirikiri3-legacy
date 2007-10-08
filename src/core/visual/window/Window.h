//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ウィンドウクラス
//---------------------------------------------------------------------------
#ifndef WindowH
#define WindowH

#include "base/exception/RisaException.h"
#include "base/event/Event.h"
#include "base/gc/RisaGC.h"


namespace Risa {
//---------------------------------------------------------------------------


/*
	メモ

	tWindowInstance::Internal -> tWindowInternal::Window -> tWindowFrame

	の構成になっている。

	tWindowInstance < tCollectee
	tWindowInternal < tDestructee

	で、tWindowFrame は GC 管理下のオブジェクトではないので注意。
*/


class tWindowInternal;
//---------------------------------------------------------------------------
//! @brief		Risaのウィンドウを表す wxFrame 派生クラス
//---------------------------------------------------------------------------
class tWindowFrame : public wxFrame
{
	typedef wxFrame inherited;

	tWindowInternal * Internal; //!< tWindowInternal のインスタンスへのポインタ

public:
	//! @brief		コンストラクタ
	//! @param		internal	tWindowInstance のインスタンスへのポインタ
	tWindowFrame(tWindowInternal * internal);

	//! @brief		デストラクタ
	~tWindowFrame();

public:
	//! @brief		ウィンドウが閉じられようとするとき
	//! @param		event イベントオブジェクト
	void OnClose(wxCloseEvent & event);

	//! @brief		ウィンドウを破棄する
	//! @note		delete オペレータをフックしている訳ではないので注意
	virtual bool Destroy();

private:
	//! @brief		イベントテーブルの定義
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief	wxWindow 派生クラスは delete ではなくて Destroy メソッドを呼ばないとならない
//---------------------------------------------------------------------------
template <>
class tDestructorCaller_Impl<wxWindow *> : public tMainThreadDestructorQueue::tDestructorCaller
{
	wxWindow * Ptr;
public:
	tDestructorCaller_Impl(wxWindow *  ptr) { Ptr = ptr; }
	virtual ~tDestructorCaller_Impl() { Ptr->Destroy(); }
};
//---------------------------------------------------------------------------




class tWindowInstance;
//---------------------------------------------------------------------------
//! @brief		ウィンドウの内部実装クラス
//---------------------------------------------------------------------------
class tWindowInternal : public tDestructee
{
	tWindowInstance * Instance; //!< tWindowInstance へのポインタ

private:
	tMainThreadAutoPtr<tWindowFrame> Window; //!< ウィンドウへのポインタ

public:
	//! @brief		コンストラクタ
	//! @param		instance		tWindowInstance へのポインタ
	tWindowInternal(tWindowInstance * instance);

	//! @brief		デストラクタ
	~tWindowInternal();

	//! @brief		ウィンドウが破棄されたことを通知する(tWindowFrameから呼ばれる)
	void NotifyDestroy();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		ウィンドウクラスのインスタンス
//---------------------------------------------------------------------------
class tWindowInstance : public tObjectBase
{
	friend class tWindowInternal;
private:
	tWindowInternal * Internal; //!< 内部実装クラスへのポインタ

public:
	//! @brief		コンストラクタ
	tWindowInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tWindowInstance() {;}

	//! @brief		ウィンドウが破棄されたことを通知する(tWindowInternalから呼ばれる)
	void NotifyDestroy();

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


#endif
