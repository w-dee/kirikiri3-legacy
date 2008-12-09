//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Octet" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseOctetClassH
#define risseOctetClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
/**
 * "Octet" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tOctetClass, tPrimitiveClassBase)
public: // Risse用メソッドなど
	static void initialize(const tNativeCallInfo & info);
	static void get_length(const tNativePropGetInfo & info);
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------
} // namespace Risse


#endif
