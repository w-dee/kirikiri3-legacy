//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "String" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseStringClassH
#define risseStringClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
/**
 * "String" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tStringClass, tPrimitiveClassBase)
public: // Risse用メソッドなど
	static void initialize(const tNativeCallInfo & info);
	static void charAt(const tNativeCallInfo & info, risse_offset index);
	static void get_length(const tNativePropGetInfo & info);
	static void substr(risse_offset start, const tNativeCallInfo & info);
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------
} // namespace Risse


#endif
