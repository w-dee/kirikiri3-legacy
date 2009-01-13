//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief フレームクラス
//---------------------------------------------------------------------------
#ifndef WIDGETRINAH
#define WIDGETRINAH

#include "risa/common/RisaException.h"
#include "risa/packages/risa/event/Event.h"
#include "risa/common/RisaGC.h"
#include "risa/packages/risa/widget/Frame.h"
#include "risa/packages/risa/graphic/rina/rinaNode.h"
#include "risa/packages/risa/graphic/rina/rinaImageEdge.h"

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * Rina widget 用のノード
 */
class tRinaWidgetNodeInstance : public tNodeInstance
{
	typedef tNodeInstance inherited; //!< 親クラス

	tInputPinArrayInstance * InputPinArrayInstance; //!< 入力ピン配列インスタンス
	tOutputPinArrayInstance * OutputPinArrayInstance; //!< 出力ピン配列インスタンス
	tImageInputPinInstance * InputPinInstance; //!< 入力ピンインスタンス

public:
	/**
	 * コンストラクタ
	 */
	tRinaWidgetNodeInstance();

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tRinaWidgetNodeInstance() {}

public: // サブクラスで実装すべき物
	/**
	 * 入力ピンの配列を得る
	 * @return	入力ピンの配列
	 */
	virtual tInputPinArrayInstance & GetInputPinArrayInstance();

	/**
	 * 出力ピンの配列を得る
	 * @return	出力ピンの配列
	 */
	virtual tOutputPinArrayInstance & GetOutputPinArrayInstance();

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * "RinaWidgetNode" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tRinaWidgetNodeClass, tClassBase, tRinaWidgetNodeInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------










class tRinaInstance;
class tImageInstance;
//---------------------------------------------------------------------------
/**
 * Rinaコントロールを表す wcControl 派生クラス
 */
class tRina : public wxControl, public tRisaWindowBahavior<tRina, tRinaInstance>
{
	typedef wxControl inherited;

public:
	/**
	 * コンストラクタ
	 * @param instance	Rinaクラスのインスタンスへのポインタ
	 * @param parent	親コントロール
	 */
	tRina(tRinaInstance * internal, wxWindow * parent);

	/**
	 * デストラクタ
	 */
	~tRina();

public:
private:
	/**
	 * イベントテーブルの定義
	 */
	DECLARE_EVENT_TABLE()

protected: // イベント
	/**
	 * 内容をペイントするとき
	 */
	void OnPaint(wxPaintEvent& event);
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
/**
 * Rinaコントロールクラスのインスタンス
 */
class tRinaInstance : public tWindowInstance
{
private:
	tRina::tInternal * Internal; //!< 内部実装クラスへのポインタ
	tRinaWidgetNodeInstance * RinaWidgetNode; //!< RinaWidgetNode インスタンス

public:
	/**
	 * コンストラクタ
	 */
	tRinaInstance();

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tRinaInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tVariant & parent, const tVariant & graph, const tNativeCallInfo &info);
	tVariant get_node();
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * "Rina" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tRinaClass, tClassBase, tRinaInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa


#endif
