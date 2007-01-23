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
	//! @param		super_class		スーパークラスを表すオブジェクト
	tRisseClass(const tRisseVariant & super_class = tRisseVariant::GetNullObject());

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

protected:
	//! @brief		newの際の新しいオブジェクトを作成して返す(必要ならば下位クラスでオーバーライドすること)
	virtual tRisseObjectBase * CreateNewObjectBase();

};
//---------------------------------------------------------------------------

}
#endif

