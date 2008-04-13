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


/*
	メモ

	tRinaInstance::Internal -> tRinaInternal::Window -> tRina

	の構成になっている。

	tRinaInstance < tCollectee
	tFramaInternal < tDestructee

	で、tRina は GC 管理下のオブジェクトではないので注意。
*/


class tRinaInternal;
//---------------------------------------------------------------------------
//! @brief		Rinaコントロールを表す wcControl 派生クラス
//---------------------------------------------------------------------------
class tRina : public wxControl
{
	typedef wxControl inherited;

	tRinaInternal * Internal; //!< tRinaInternal のインスタンスへのポインタ

public:
	//! @brief		コンストラクタ
	//! @param		internal	tRinaInstance のインスタンスへのポインタ
	//! @param		parent		親コントロール
	tRina(tRinaInternal * internal, wxWindow * parent);

	//! @brief		デストラクタ
	~tRina();

public:
private:
	//! @brief		イベントテーブルの定義
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------







class tRinaInstance;
class tFrameInstance;
//---------------------------------------------------------------------------
//! @brief		Rinaコントロールの内部実装クラス
//---------------------------------------------------------------------------
class tRinaInternal : public tDestructee
{
	tRinaInstance * Instance; //!< tRinaInstance へのポインタ

private:
	tMainThreadAutoPtr<tRina> Rina; //!< Rinaコントロールへのポインタ

public:
	//! @brief		コンストラクタ
	//! @param		instance		tRinaInstance へのポインタ
	//! @param		frame			このコントロールを含む親フレーム
	tRinaInternal(tRinaInstance * instance, tFrameInstance * frame);

	//! @brief		デストラクタ
	~tRinaInternal();

	//! @brief		インスタンスを得る
	tRinaInstance * GetInstance() const { return Instance; }

	//! @brief		Rinaコントロールへのポインタを獲る
	tMainThreadAutoPtr<tRina> & GetRina() { return Rina; }

	//! @brief		Rinaコントロールが破棄されたことを通知する(tRinaから呼ばれる)
	void NotifyDestroy();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Rinaコントロールクラスのインスタンス
//---------------------------------------------------------------------------
class tRinaInstance : public tObjectBase
{
	friend class tRinaInternal;
private:
	tRinaInternal * Internal; //!< 内部実装クラスへのポインタ

public:
	//! @brief		コンストラクタ
	tRinaInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tRinaInstance() {;}

	//! @brief		Rinaコントロールが破棄されたことを通知する(tRinaInternalから呼ばれる)
	void NotifyDestroy();

	//! @brief		wxWindow 派生クラスのインスタンスを得る
	tMainThreadAutoPtr<tRina> & GetWxWindow() const;

public: // Risse用メソッドなど
	void construct();
	void initialize(const tVariant & parent, const tNativeCallInfo &info);

	void dispose(); //!< Rinaコントロールを破棄する
	void close(const tMethodArgument &args); //!< 「閉じる」ボタンをエミュレートする
	void onClose(bool force); //!< 「閉じる」ボタンが押されたときやclose()メソッドが呼ばれたとき

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
