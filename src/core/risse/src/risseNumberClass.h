//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseの数値型用クラス関連の処理の実装
//---------------------------------------------------------------------------

#ifndef risseNumberClassH
#define risseNumberClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
/**
 * 数値型用クラスの基底クラス(Risse用)
 */
RISSE_DEFINE_CLASS_BEGIN(tNumberClass, tClassBase)
public: // Risse用メソッドなど
	static void construct();
	static void isNaN(const tNativePropGetInfo & info);
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------
} // namespace Risse


#endif
