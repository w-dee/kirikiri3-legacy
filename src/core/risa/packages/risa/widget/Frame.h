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
/**
 * Risaのフレームを表す wxFrame 派生クラス
 */
class tFrame : public wxFrame, public tRisaWindowBahavior<tFrame, tFrameInstance>
{
	typedef wxFrame inherited;

public:
	/**
	 * コンストラクタ
	 * @param instance	Frameクラスのインスタンスへのポインタ
	 */
	tFrame(tFrameInstance * instance);

	/**
	 * デストラクタ
	 */
	~tFrame();

public:
	/**
	 * フレームが閉じられようとするとき
	 * @param event	イベントオブジェクト
	 */
	void OnClose(wxCloseEvent & event);

	/**
	 * フレームを破棄する
	 * @note	デストラクタをフックしている訳ではないので注意
	 *			(ただしtRisaWindowBahaviorのデストラクタ内も参照)
	 */
	virtual bool Destroy();

private:
	/**
	 * イベントテーブルの定義
	 */
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
/**
 * フレームクラスのインスタンス
 */
class tFrameInstance : public tWindowInstance
{
private:
	tFrame::tInternal * Internal; //!< 内部実装クラスへのポインタ

public:
	/**
	 * コンストラクタ
	 */
	tFrameInstance();

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tFrameInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
/**
 * "Frame" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tFrameClass, tClassBase, tFrameInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risa


#endif
