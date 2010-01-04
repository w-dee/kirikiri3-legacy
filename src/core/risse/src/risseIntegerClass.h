//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Integer" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseIntegerClassH
#define risseIntegerClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"

namespace Risse
{
//---------------------------------------------------------------------------
/**
 * "Integer" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tIntegerClass, tPrimitiveClassBase, risse_int64, itPrimitive)
public: // Risse用メソッドなど
	static void initialize(const tNativeCallInfo & info);
	static void times(const tNativeCallInfo & info);
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------
} // namespace Risse


#endif
