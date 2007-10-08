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
public:
	tWindowFrame();
	~tWindowFrame();

public:
	//! @brief		ウィンドウが閉じられようとするとき
	//! @param		event イベントオブジェクト
	void OnClose(wxCloseEvent & event);

private:
	//! @brief		イベントテーブルの定義
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		ウィンドウインスタンス
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

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


#endif
