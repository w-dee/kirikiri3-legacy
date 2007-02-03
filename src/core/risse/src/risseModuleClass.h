//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief "Module" (モジュール) の実装
//---------------------------------------------------------------------------
#ifndef risseModuleClassH
#define risseModuleClassH

#include "risseCharUtils.h"
#include "risseObject.h"
#include "risseSingleton.h"
#include "risseClass.h"
#include "risseObjectBase.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		モジュール用クラスの基底クラス(Risse用)
//---------------------------------------------------------------------------
class tRisseModuleClass : public tRisseClassBase, public tRisseSingleton<tRisseModuleClass>
{
public:
	// ! @brief		コンストラクタ
	tRisseModuleClass();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();
};
//---------------------------------------------------------------------------
}
#endif

