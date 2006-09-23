//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コルーチンの実装
//---------------------------------------------------------------------------
#ifndef risseCoroutineH
#define risseCoroutineH

#include "risseConfig.h"
#include "risseUtils.h"

namespace Risse
{


//---------------------------------------------------------------------------
//! @brief		コルーチンの初期化
//! @note		GC_init の直後に呼ばれる必要がある
//---------------------------------------------------------------------------
void RisseInitCoroutine();

} // namespace Risse

#endif

