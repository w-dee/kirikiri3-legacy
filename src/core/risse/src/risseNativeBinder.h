//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ネイティブ関数/プロパティを簡単に記述するためのC++テンプレートサポート
//---------------------------------------------------------------------------
#ifndef risseNativeBinderH
#define risseNativeBinderH

#include "risseNativeFunction.h"

namespace Risse
{


//---------------------------------------------------------------------------
//! @brief		Risseネイティブ関数
//---------------------------------------------------------------------------
class tRisseNativeBindFunction : public tRisseObjectInterface
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッドのtypedef
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	typedef void (*tCallee)(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS);

	//! @brief		Risseクラスインスタンス
	tRisseClassBase * Class; 

	//! @brief		Risseメソッド呼び出し時に本当に呼ばれるネイティブな関数
	void (tRisseObjectBase::*TargetFunction)();

	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド(インスタンス作成時に指定)
	tCallee Callee;

protected:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		class_		Risseクラスインスタンス
	//! @param		target		Risseメソッド呼び出し時に本当に呼ばれるネイティブな関数
	//! @param		callee		Risseメソッド呼び出し時に呼ばれるメソッド
	tRisseNativeBindFunction(tRisseScriptEngine * engine,
		tRisseClassBase * class_, void (tRisseObjectBase::*target)(), tCallee callee) :
		tRisseObjectInterface(new tRisseRTTI(engine))
		{ Class = class_; TargetFunction = target; Callee = callee; }

public:
	//! @brief		新しい関数インスタンスを生成して返す(コンストラクタではなくてこちらを呼ぶこと)
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		class_		Risseクラスインスタンス
	//! @param		target		Risseメソッド呼び出し時に本当に呼ばれるネイティブな関数
	//! @param		callee		Risseメソッド呼び出し時に呼ばれるメソッド
	static tRisseObjectInterface * New(tRisseScriptEngine * engine,
		tRisseClassBase * class_, void (tRisseObjectBase::*target)(), tCallee callee);

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Risseネイティブ関数
//---------------------------------------------------------------------------
class tRisseNativeBindStaticFunction : public tRisseObjectInterface
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッドのtypedef
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	typedef void (*tCallee)(tRisseClassBase * _class, void (*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS);

	//! @brief		Risseクラスインスタンス
	tRisseClassBase * Class; 

	//! @brief		Risseメソッド呼び出し時に本当に呼ばれるネイティブな関数
	void (*TargetFunction)();

	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド(インスタンス作成時に指定)
	tCallee Callee;

protected:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		class_		Risseクラスインスタンス
	//! @param		target		Risseメソッド呼び出し時に本当に呼ばれるネイティブな関数
	//! @param		callee		Risseメソッド呼び出し時に呼ばれるメソッド
	tRisseNativeBindStaticFunction(tRisseScriptEngine * engine,
		tRisseClassBase * class_, void (*target)(), tCallee callee) :
		tRisseObjectInterface(new tRisseRTTI(engine))
		{ Class = class_; TargetFunction = target; Callee = callee; }

public:
	//! @brief		新しい関数インスタンスを生成して返す(コンストラクタではなくてこちらを呼ぶこと)
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		class_		Risseクラスインスタンス
	//! @param		target		Risseメソッド呼び出し時に本当に呼ばれるネイティブな関数
	//! @param		callee		Risseメソッド呼び出し時に呼ばれるメソッド
	static tRisseObjectInterface * New(tRisseScriptEngine * engine,
		tRisseClassBase * class_, void (*target)(), tCallee callee);

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// 各種型 -> tRisseVariant 変換
// 自前の型がある場合は RisseToVariant のテンプレート特殊化を行うこと。
// tRisseVariant のメンバ関数の方がよいのでは？という考えもあるかもしれないが
// 新しい型を定義し、その型の定義にも tRisseVariant の定義にも手を加えずに
// 変換規則を書きたい場合などはこっちの方が都合がよい。
//---------------------------------------------------------------------------
template <typename T>
inline tRisseVariant RisseToVariant(T s)
{
	return tRisseVariant(s);
}

//---------------------------------------------------------------------------
// 整数系
template <>
inline tRisseVariant RisseToVariant<size_t>(size_t s)
{
	return tRisseVariant((risse_int64)s);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tRisseVariant -> 各種型 変換
// 自前の型がある場合は RisseFromVariant のテンプレート特殊化を行うこと。
//---------------------------------------------------------------------------
template <typename T>
inline T RisseFromVariant(const tRisseVariant & v)
{
	return (T)v;
}

//---------------------------------------------------------------------------
// 整数系
template <>
inline size_t RisseFromVariant<size_t>(const tRisseVariant & v)
{
	return (size_t)(risse_int64)v;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// 関数オブジェクトなど
// いまのところ RisseRegisterBinder はどのクラスにも属していない。
// ネイティブ関数の定義が必要ないところに、かなり大きいテンプレートライブラリを
// 読み込みたくなく、RisseRegisterBinder をそのほかの実装/定義と分離するためである。
//---------------------------------------------------------------------------
#include "risseNativeBinderTemplates.inc"
//---------------------------------------------------------------------------

}
#endif

