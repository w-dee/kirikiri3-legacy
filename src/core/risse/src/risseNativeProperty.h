//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ネイティブプロパティを記述するためのサポート
//---------------------------------------------------------------------------
#ifndef risseNativePropertyH
#define risseNativePropertyH

#include "risseCharUtils.h"
#include "risseObjectBase.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		Risseネイティブプロパティの基底クラス
//---------------------------------------------------------------------------
class tRisseNativePropertyBase : public tRisseObjectBase
{
	//! @param		親クラスのtypedef
	typedef tRisseObjectBase inherited;

protected:
	//! @brief		Risseプロパティが読み込まれる際に呼ばれるメソッド(下位クラスでオーバーライドすること.
	//!				オーバライドしなかった場合は読み込み禁止のプロパティになる)
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags	オペレーションフラグ
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	virtual void Read(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack);

	//! @brief		Risseプロパティが書き込まれる際に呼ばれるメソッド(下位クラスでオーバーライドすること.
	//!				オーバーライドしなかった場合は書き込み禁止のプロパティになる)
	//! @param		value	書き込む値
	//! @param		flags	オペレーションフラグ
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	virtual void Write(
		const tRisseVariant & value,
		tRisseOperateFlags flags,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack);

public:
	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------

}
#endif

