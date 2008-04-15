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
#ifndef WIDGETRINAH
#define WIDGETRINAH

#include "risa/common/RisaException.h"
#include "risa/packages/risa/event/Event.h"
#include "risa/common/RisaGC.h"
#include "risa/packages/risa/widget/Frame.h"


namespace Risa {
//---------------------------------------------------------------------------



class tRinaInstance;
//---------------------------------------------------------------------------
//! @brief		Rinaコントロールを表す wcControl 派生クラス
//---------------------------------------------------------------------------
class tRina : public wxControl, public tRisaWindowBahavior<tRina, tRinaInstance>
{
	typedef wxControl inherited;

public:
	//! @brief		コンストラクタ
	//! @param		instance	Rinaクラスのインスタンスへのポインタ
	//! @param		parent		親コントロール
	tRina(tRinaInstance * internal, wxWindow * parent);

	//! @brief		デストラクタ
	~tRina();

public:
private:
	//! @brief		イベントテーブルの定義
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		Rinaコントロールクラスのインスタンス
//---------------------------------------------------------------------------
class tRinaInstance : public tWindowInstance
{
private:
	tRina::tInternal * Internal; //!< 内部実装クラスへのポインタ

public:
	//! @brief		コンストラクタ
	tRinaInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tRinaInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tVariant & parent, const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"Rina" クラス
//---------------------------------------------------------------------------
class tRinaClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRinaClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa


#endif
