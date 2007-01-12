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
#ifndef risseClassH
#define risseClassH

#include "risseCharUtils.h"
#include "risseObject.h"
#include "risseObjectBase.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		Risseクラスの実装
//---------------------------------------------------------------------------
class tRisseClass : public tRisseObjectBase
{
	//! @brief		親クラスのtypedef
	typedef tRisseObjectBase inherited;

public:
	//! @brief		コンストラクタ
	//! @param		super_class		スーパークラス
	tRisseClass(const tRisseVariant & super_class);

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG) = 0;
};
//---------------------------------------------------------------------------

}
#endif

