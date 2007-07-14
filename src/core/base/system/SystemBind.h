//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief System クラス バインディング
//---------------------------------------------------------------------------
#ifndef _SYSTEMBIND_H
#define _SYSTEMBIND_H

#if 0
#include "base/script/RisseEngine.h"
#include "risse/include/risseNative.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tSystemRegisterer :
			public singleton_base<tSystemRegisterer>,
			protected depends_on<tRisseScriptEngine>
{
	iRisseDispatch2 * SystemClass;

public:
	//! @brief		コンストラクタ
	tSystemRegisterer();

	//! @brief デストラクタ
	~tSystemRegisterer();

	//! @brief		System.exceptionHandler を得る
	//! @param		dest		exceptionHandler を格納するクロージャ
	//! @return		正常な exceptionHandler を得られれば真、得られなければ偽
	bool GetExceptionHandlerObject(tVariantClosure & dest);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif

#endif

