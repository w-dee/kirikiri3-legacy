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
#ifndef WIDGETFRAMEH
#define WIDGETFRAMEH

#include "risa/common/RisaException.h"
#include "risa/packages/risa/event/Event.h"
#include "risa/common/RisaGC.h"
#include "risa/packages/risa/widget/Window.h"


namespace Risa {
//---------------------------------------------------------------------------


/*
	メモ

	tFrameInstance::Internal -> tFrameInternal::Window -> tFrame

	の構成になっている。

	tFrameInstance < tCollectee
	tFramaInternal < tDestructee

	で、tFrame は GC 管理下のオブジェクトではないので注意。
*/




class tFrameInternal;
//---------------------------------------------------------------------------
//! @brief		Risaのフレームを表す wxFrame 派生クラス
//---------------------------------------------------------------------------
class tFrame : public wxFrame
{
	typedef wxFrame inherited;

	tFrameInternal * Internal; //!< tFrameInternal のインスタンスへのポインタ

public:
	//! @brief		コンストラクタ
	//! @param		internal	tFrameInstance のインスタンスへのポインタ
	tFrame(tFrameInternal * internal);

	//! @brief		デストラクタ
	~tFrame();

public:
	//! @brief		フレームが閉じられようとするとき
	//! @param		event イベントオブジェクト
	void OnClose(wxCloseEvent & event);

	//! @brief		フレームを破棄する
	//! @note		delete オペレータをフックしている訳ではないので注意
	virtual bool Destroy();

private:
	//! @brief		イベントテーブルの定義
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------





class tFrameInstance;
//---------------------------------------------------------------------------
//! @brief		フレームの内部実装クラス
//---------------------------------------------------------------------------
class tFrameInternal : public tDestructee
{
	tFrameInstance * Instance; //!< tFrameInstance へのポインタ

private:
	tMainThreadAutoPtr<tFrame> Frame; //!< フレームへのポインタ

public:
	//! @brief		コンストラクタ
	//! @param		instance		tFrameInstance へのポインタ
	tFrameInternal(tFrameInstance * instance);

	//! @brief		デストラクタ
	~tFrameInternal();

	//! @brief		インスタンスを得る
	tFrameInstance * GetInstance() const { return Instance; }

	//! @brief		フレームへのポインタを獲る
	tMainThreadAutoPtr<tFrame> & GetFrame() { return Frame; }

	//! @brief		フレームが破棄されたことを通知する(tFrameから呼ばれる)
	void NotifyDestroy();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		フレームクラスのインスタンス
//---------------------------------------------------------------------------
class tFrameInstance : public tObjectBase
{
	friend class tFrameInternal;
private:
	tFrameInternal * Internal; //!< 内部実装クラスへのポインタ

public:
	//! @brief		コンストラクタ
	tFrameInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tFrameInstance() {;}

	//! @brief		フレームが破棄されたことを通知する(tFrameInternalから呼ばれる)
	void NotifyDestroy();

	//! @brief		wxWindow 派生クラスのインスタンスを得る
	tMainThreadAutoPtr<tFrame> & GetWxWindow() const;

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

	void dispose(); //!< フレームを破棄する
	void close(const tMethodArgument &args); //!< 「閉じる」ボタンをエミュレートする
	void onClose(bool force); //!< 「閉じる」ボタンが押されたときやclose()メソッドが呼ばれたとき

};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		"Frame" クラス
//---------------------------------------------------------------------------
class tFrameClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tFrameClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risa


#endif
