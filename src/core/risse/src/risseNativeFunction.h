//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ネイティブ関数(ネイティブメソッド)を記述するためのサポート
//---------------------------------------------------------------------------
#ifndef risseNativeFunctionH
#define risseNativeFunctionH

#include "risseCharUtils.h"
#include "risseObjectBase.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		Risseネイティブ関数の基底クラス
//---------------------------------------------------------------------------
class tRisseNativeFunctionBase : public tRisseObjectBase
{
	//! @param		親クラスのtypedef
	typedef tRisseObjectBase inherited;

protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド(下位クラスでオーバーライドすること)
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	virtual void Call(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseMethodArgument & args,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack) = 0;

public:
	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------

}
#endif

