//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief "Class" (クラス) の実装
//---------------------------------------------------------------------------
#ifndef risseClassClassH
#define risseClassClassH

#include "risseCharUtils.h"
#include "risseObject.h"
#include "risseClass.h"
#include "risseSingleton.h"
#include "risseObjectBase.h"
#include "risseObjectRTTI.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		クラス用クラスの基底クラス(Risse用)
//! @note		tRisseClassBase と tRisseClassClass を混同しないこと。
//!				前者はC++のクラス階層において各クラスの親クラスになり、
//!				後者はRisseスクリプトで各クラスインスタンスを生成するための
//!				クラスになる。
//---------------------------------------------------------------------------
class tRisseClassClass : public tRisseClassBase, public tRisseSingleton<tRisseClassClass>
{
public:
	// ! @brief		コンストラクタ
	tRisseClassClass();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();
};
//---------------------------------------------------------------------------


}
#endif

