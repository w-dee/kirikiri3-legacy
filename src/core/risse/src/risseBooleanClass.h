//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Boolean" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseBooleanClassH
#define risseBooleanClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
/**
 * "Boolean" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tBooleanClass, tPrimitiveClassBase, tVariant, itPrimitive)
public: // Risse用メソッドなど
	static void initialize(const tNativeCallInfo & info);
	static tString toString(const tNativeCallInfo & info);
	static tString dump(const tNativeCallInfo & info);
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------
} // namespace Risse


#endif
