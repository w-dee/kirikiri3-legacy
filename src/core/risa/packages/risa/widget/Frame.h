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




class tFrameInstance;
//---------------------------------------------------------------------------
//! @brief		Risaのフレームを表す wxFrame 派生クラス
//---------------------------------------------------------------------------
class tFrame : public wxFrame, public tRisaWindowBahavior<tFrame, tFrameInstance>
{
	typedef wxFrame inherited;

public:
	//! @brief		コンストラクタ
	//! @param		instance		Frameクラスのインスタンスへのポインタ
	tFrame(tFrameInstance * instance);

	//! @brief		デストラクタ
	~tFrame();

public:
	//! @brief		フレームが閉じられようとするとき
	//! @param		event イベントオブジェクト
	void OnClose(wxCloseEvent & event);

	//! @brief		フレームを破棄する
	//! @note		デストラクタをフックしている訳ではないので注意
	//!				(ただしtRisaWindowBahaviorのデストラクタ内も参照)
	virtual bool Destroy();

private:
	//! @brief		イベントテーブルの定義
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		フレームクラスのインスタンス
//---------------------------------------------------------------------------
class tFrameInstance : public tWindowInstance
{
private:
	tFrame::tInternal * Internal; //!< 内部実装クラスへのポインタ

public:
	//! @brief		コンストラクタ
	tFrameInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tFrameInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
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
