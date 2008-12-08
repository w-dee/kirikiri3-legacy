//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RisseのDataクラス関連の処理の実装
//---------------------------------------------------------------------------

#ifndef risseDataClassH
#define risseDataClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "risseNativeBinder.h"
#include "rissePrimitiveClass.h"

namespace Risse
{
//---------------------------------------------------------------------------
RISSE_DEFINE_CLASS_BEGIN(tDataClass, tClassBase/* tPrimitiveClassBase ではないので注意 */)
public: // Risse用メソッドなど
	static void construct();
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * すべての Data 派生クラスの C++ 実装用の基底クラス
 */
typedef tPrimitiveClassBase tDataClassBase;
//---------------------------------------------------------------------------


} // namespace Risse


#endif
