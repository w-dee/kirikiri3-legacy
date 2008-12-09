//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Null" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseNullClassH
#define risseNullClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
/**
 * "Null" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tNullClass, tPrimitiveClassBase)
public: // Risse用メソッドなど
	static void initialize(const tNativeCallInfo & info);
	static tString toString();
	static tString dump();
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------
} // namespace Risse


#endif
