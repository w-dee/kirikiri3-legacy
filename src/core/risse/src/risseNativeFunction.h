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
#include "risseFunctionClass.h"

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
		const tRisseVariant &This

//! @brief		ネイティブ関数宣言の開始(コンテキスト指定)
//! @note		func_name は一つの単語であること。RISSE_WS( ) などでの文字列は受け付けない。
#define RISSE_BEGIN_NATIVE_METHOD_CONTEXT(func_name, func_context) \
{ \
	const tRisseString & name = (func_name); \
	const tRisseVariantBlock * context = (func_context); \
	struct tNCM_##func_name { \
		static void Do(RISSE_NATIVEFUNCTION_CALLEE_ARGS) {


//! @brief		ネイティブ関数宣言の開始
//! @note		func_name は一つの単語であること。RISSE_WS( ) などでの文字列は受け付けない。
#define RISSE_BEGIN_NATIVE_METHOD(func_name) \
	RISSE_BEGIN_NATIVE_METHOD_CONTEXT(func_name, tRisseVariant::GetAnyContext());

//! @brief		ネイティブ関数宣言の終了
#define RISSE_END_NATIVE_METHOD \
		} \
	} static instance;\
	RegisterNormalMember(name, tRisseVariant(tRisseNativeFunction::New(instance.Do), context)); \
}

//---------------------------------------------------------------------------
//! @brief		Risseネイティブ関数
//---------------------------------------------------------------------------
class tRisseNativeFunction : public tRisseObjectInterface
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッドのtypedef
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	typedef void (*tCallee)(RISSE_NATIVEFUNCTION_CALLEE_ARGS);

	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド(インスタンス作成時に指定)
	tCallee Callee;

protected:
	//! @brief		コンストラクタ
	//! @param		callee		Risseメソッド呼び出し時に呼ばれるメソッド
	tRisseNativeFunction(tCallee callee) { Callee = callee; }

public:
	//! @brief		新しい関数インスタンスを生成して返す(コンストラクタではなくてこちらを呼ぶこと)
	//! @param		callee		Risseメソッド呼び出し時に呼ばれるメソッド
	static tRisseObjectInterface * New(tCallee callee);

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------

}
#endif

