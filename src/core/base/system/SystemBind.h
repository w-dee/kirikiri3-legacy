//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief System クラス バインディング
//---------------------------------------------------------------------------
#ifndef _SYSTEMBIND_H
#define _SYSTEMBIND_H

#include "base/script/RisseEngine.h"
#include "risse/include/risseNative.h"


//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tRisaSystemRegisterer :
			public singleton_base<tRisaSystemRegisterer>,
			protected depends_on<tRisaRisseScriptEngine>
{
	iRisseDispatch2 * SystemClass;

public:
	//! @brief		コンストラクタ
	tRisaSystemRegisterer();

	//! @brief デストラクタ
	~tRisaSystemRegisterer();

	//! @brief		System.exceptionHandler を得る
	//! @param		dest		exceptionHandler を格納するクロージャ
	//! @return		正常な exceptionHandler を得られれば真、得られなければ偽
	bool GetExceptionHandlerObject(tRisseVariantClosure & dest);
};
//---------------------------------------------------------------------------

#endif

