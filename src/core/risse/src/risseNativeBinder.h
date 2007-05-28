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
//! @brief		ネイティブ関数呼び出し情報
//---------------------------------------------------------------------------
class tRisseNativeBindFunctionCallingInfo
{
public:
	tRisseScriptEngine * engine;		//!< スクリプトエンジンインスタンス
	tRisseVariant * result;				//!< 結果の格納先 (NULLの場合は結果が要らない場合)
	tRisseOperateFlags flags;			//!< オペレーションフラグ
	const tRisseMethodArgument & args;	//!< 引数
	const tRisseVariant &This;			//!< メソッドが実行されるべき"Thisオブジェクト"
										//!< (NULL="Thisオブジェクト"を指定しない場合)

	//! @brief		コンストラクタ
	//! @param		engine_	スクリプトエンジンインスタンス
	//! @param		result_	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags_	オペレーションフラグ
	//! @param		args_	引数
	//! @param		This_	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	tRisseNativeBindFunctionCallingInfo(
		tRisseScriptEngine * engine_,
		tRisseVariant * result_,
		tRisseOperateFlags flags_,
		const tRisseMethodArgument & args_,
		const tRisseVariant &This_):
		engine(engine_), result(result_), flags(flags_), args(args_), This(This_) {;}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risseネイティブ関数
//---------------------------------------------------------------------------
class tRisseNativeBindFunction : public tRisseObjectInterface
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッドのtypedef
	//! @param		_class		クラス
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tCallee)(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
		const tRisseNativeBindFunctionCallingInfo & info);

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
	//! @param		_class		クラス
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tCallee)(tRisseClassBase * _class, void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info);

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
inline tRisseVariant RisseToVariant<risse_size>(risse_size s)
{
	return tRisseVariant((risse_int64)s);
}
template <>
inline tRisseVariant RisseToVariant<risse_offset>(risse_offset s)
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
inline risse_size RisseFromVariant<risse_size>(const tRisseVariant & v)
{
	return (risse_size)(risse_int64)v;
}
//---------------------------------------------------------------------------
// 整数系
template <>
inline risse_offset RisseFromVariant<risse_offset>(const tRisseVariant & v)
{
	return (risse_offset)(risse_int64)v;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// 関数オブジェクトをサポートするためのテンプレート群
//---------------------------------------------------------------------------
// 型が同一かどうか
template <typename A, typename B> struct tRisseIsSameType { enum { value = false }; };
template <typename A> struct tRisseIsSameType<A,A> { enum { value = true }; };
//---------------------------------------------------------------------------
// メタ型 (tRisseNativeBindFunctionCallingInfo内の情報つまり関数の呼び出しに関する
// 情報) なのか 実引数なのか
// メタ型でない場合は value が 1 になる
// メタ型の場合は RisseFromVariantOrCallingInfo でメタ型に変換できる
template <typename T> struct tRisseIsFuncCallNonMetaType { enum { value = 1 }; };
template <> struct tRisseIsFuncCallNonMetaType<const tRisseNativeBindFunctionCallingInfo &>
													{ enum { value = 0 }; };
template <> struct tRisseIsFuncCallNonMetaType<tRisseScriptEngine *>
													{ enum { value = 0 }; };
template <> struct tRisseIsFuncCallNonMetaType<const tRisseMethodArgument &>
													{ enum { value = 0 }; };
//---------------------------------------------------------------------------
// T への、メソッドへの引数あるいは呼び出し情報からの変換
template <typename T>
inline T RisseFromVariantOrCallingInfo(const tRisseVariant & v,
				const tRisseNativeBindFunctionCallingInfo & info)
{
	return RisseFromVariant<T>(v);
}
//---------------------------------------------------------------------------
template <>
inline const tRisseNativeBindFunctionCallingInfo &
	RisseFromVariantOrCallingInfo<const tRisseNativeBindFunctionCallingInfo &>
		(const tRisseVariant & v, const tRisseNativeBindFunctionCallingInfo & info)
{
	return info;
}
//---------------------------------------------------------------------------
template <>
inline tRisseScriptEngine *
	RisseFromVariantOrCallingInfo<tRisseScriptEngine *>
		(const tRisseVariant & v, const tRisseNativeBindFunctionCallingInfo & info)
{
	return info.engine;
}
//---------------------------------------------------------------------------
template <>
inline const tRisseMethodArgument &
	RisseFromVariantOrCallingInfo<const tRisseMethodArgument &>
		(const tRisseVariant & v, const tRisseNativeBindFunctionCallingInfo & info)
{
	return info.args;
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

/*
	TODO: 呼び出しの効率化
	RTTI が増えるかもしれないがのう
*/

}
#endif

