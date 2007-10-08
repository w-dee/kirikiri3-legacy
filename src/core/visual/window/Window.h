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


//---------------------------------------------------------------------------
//! @brief		ウィンドウインスタンス
//---------------------------------------------------------------------------
class tWindowInstance : public tObjectBase
{
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
