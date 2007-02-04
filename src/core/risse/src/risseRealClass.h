//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "risseSingleton.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Real" クラス
//---------------------------------------------------------------------------
class tRisseRealClass : public tRissePrimitiveClassBase, public tRisseClassSingleton<tRisseRealClass>
{
public:
	//! @brief		コンストラクタ
	tRisseRealClass();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
