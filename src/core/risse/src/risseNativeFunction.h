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
// この基底クラスを派生し、下位クラスで Operate をオーバーライドして各メソッド
// やプロパティを実装する方法もあるが、それだと各メソッドやプロパティごとに
// クラスが作成され、RTTIを浪費することになる (RTTIを作成しないpragmaticな
// 方法もあるだろうが)。ここでは、実際に呼び出す先を関数ポインタにし、
// クラスはいちいち作らないことにする。ちなみにTJS2では当初はクラスを
// それぞれのメソッドやプロパティごとに持っていたが、このような方法を
// とったことにより大幅に実行バイナリのサイズが減った。

//! @brief		tRisseNativeFunctionBase::tCallee の引数
#define RISSE_NATIVEFUNCTION_CALLEE_ARGS \
		tRisseVariant * result,               \
		tRisseOperateFlags flags,             \
		const tRisseMethodArgument & args,    \
		const tRisseVariant &This,            \
		const tRisseStackFrameContext &stack

//---------------------------------------------------------------------------
//! @brief		Risseネイティブ関数の基底クラス
//---------------------------------------------------------------------------
class tRisseNativeFunctionBase : public tRisseObjectBase
{
	//! @param		親クラスのtypedef
	typedef tRisseObjectBase inherited;

protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッドのtypedef
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	typedef void (*tCallee)(RISSE_NATIVEFUNCTION_CALLEE_ARGS);

	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド(インスタンス作成時に指定)
	tCallee Callee;

public:
	//! @brief		コンストラクタ
	//! @param		callee		Risseメソッド呼び出し時に呼ばれるメソッド
	tRisseNativeFunctionBase(tCallee callee) { Callee = callee; }

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------

}
#endif

