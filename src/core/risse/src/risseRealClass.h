//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Real" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseRealClassH
#define risseRealClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
/**
 * "Real" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tRealClass, tPrimitiveClassBase, risse_real, itPrimitive)
public: // Risse用メソッドなど
	static void initialize(const tNativeCallInfo & info);
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------
} // namespace Risse


#endif
