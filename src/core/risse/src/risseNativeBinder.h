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


#include "risseCharUtils.h"
#include "risseObjectBase.h"
#include "risseObjectRTTI.h"

namespace Risse
{
class tRisseClassBase;
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
template <typename TT> // target-type, void (tRisseObjectBase::*)() とか
class tRisseNativeBindFunction : public tRisseObjectInterface
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッドのtypedef
	//! @param		_class		クラス
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tCallee)(tRisseClassBase * _class, TT f,
		const tRisseNativeBindFunctionCallingInfo & info);

	//! @brief		Risseクラスインスタンス
	tRisseClassBase * Class; 

	//! @brief		Risseメソッド呼び出し時に本当に呼ばれるネイティブな関数
	TT TargetFunction;

	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド(インスタンス作成時に指定)
	tCallee Callee;

protected:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		class_		Risseクラスインスタンス
	//! @param		target		Risseメソッド呼び出し時に本当に呼ばれるネイティブな関数
	//! @param		callee		Risseメソッド呼び出し時に呼ばれるメソッド
	tRisseNativeBindFunction(tRisseScriptEngine * engine,
		tRisseClassBase * class_, TT target, tCallee callee) :
		tRisseObjectInterface(new tRisseRTTI(engine))
		{ Class = class_; TargetFunction = target; Callee = callee; }

public:
	//! @brief		新しい関数インスタンスを生成して返す(コンストラクタではなくてこちらを呼ぶこと)
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		class_		Risseクラスインスタンス
	//! @param		target		Risseメソッド呼び出し時に本当に呼ばれるネイティブな関数
	//! @param		callee		Risseメソッド呼び出し時に呼ばれるメソッド
	static tRisseObjectInterface * New(tRisseScriptEngine * engine,
		tRisseClassBase * class_, TT target, tCallee callee);

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ネイティブプロパティゲッター呼び出し情報
//---------------------------------------------------------------------------
class tRisseNativeBindPropertyGetterCallingInfo
{
public:
	tRisseScriptEngine * engine;		//!< スクリプトエンジンインスタンス
	tRisseVariant * result;				//!< 結果の格納先 (NULLの場合は結果が要らない場合)
	tRisseOperateFlags flags;			//!< オペレーションフラグ
	const tRisseVariant &This;			//!< メソッドが実行されるべき"Thisオブジェクト"
										//!< (NULL="Thisオブジェクト"を指定しない場合)

	//! @brief		コンストラクタ
	//! @param		engine_	スクリプトエンジンインスタンス
	//! @param		result_	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags_	オペレーションフラグ
	//! @param		This_	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	tRisseNativeBindPropertyGetterCallingInfo(
		tRisseScriptEngine * engine_,
		tRisseVariant * result_,
		tRisseOperateFlags flags_,
		const tRisseVariant &This_):
		engine(engine_), result(result_), flags(flags_), This(This_) {;}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ネイティブプロパティセッター呼び出し情報
//---------------------------------------------------------------------------
class tRisseNativeBindPropertySetterCallingInfo
{
public:
	tRisseScriptEngine * engine;		//!< スクリプトエンジンインスタンス
	const tRisseVariant & value;		//!< 値
	tRisseOperateFlags flags;			//!< オペレーションフラグ
	const tRisseVariant &This;			//!< メソッドが実行されるべき"Thisオブジェクト"
										//!< (NULL="Thisオブジェクト"を指定しない場合)

	//! @brief		コンストラクタ
	//! @param		engine_	スクリプトエンジンインスタンス
	//! @param		value_	値
	//! @param		flags_	オペレーションフラグ
	//! @param		This_	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	tRisseNativeBindPropertySetterCallingInfo(
		tRisseScriptEngine * engine_,
		const tRisseVariant & value_,
		tRisseOperateFlags flags_,
		const tRisseVariant &This_):
		engine(engine_), value(value_), flags(flags_), This(This_) {;}
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		RisseネイティブプロパティのGetterの実装
//---------------------------------------------------------------------------
template <typename TT> // target-type, void (tRisseObjectBase::*)() とか
class tRisseNativeBindPropertyGetter : public tRisseObjectInterface
{
public:
	//! @brief		Risseプロパティが読み込まれる際に呼ばれるメソッドのtypedef
	//! @param		_class		クラス
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tGetter)(tRisseClassBase * _class, TT f,
		const tRisseNativeBindPropertyGetterCallingInfo & info);

private:
	//! @brief		Risseクラスインスタンス
	tRisseClassBase * Class; 

	//! @brief		Risseプロパティが読み込まれる際に本当に呼ばれるネイティブな関数
	TT TargetFunction;

	//! @brief		Risseプロパティが読み込まれる際に呼ばれるメソッド
	tGetter Getter;

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		class_		Risseクラスインスタンス
	//! @param		target		Risseプロパティ読み込み時に本当に呼ばれるネイティブな関数
	//! @param		getter		Risseプロパティが読み込まれる際に呼ばれるメソッド
	tRisseNativeBindPropertyGetter(tRisseScriptEngine * engine,
		tRisseClassBase * class_, TT target, tGetter getter) :
		tRisseObjectInterface(new tRisseRTTI(engine))
	{ Class = class_; TargetFunction = target; Getter = getter; }

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		RisseネイティブプロパティのSetterの実装
//---------------------------------------------------------------------------
template <typename TT> // target-type, void (tRisseObjectBase::*)() とか
class tRisseNativeBindPropertySetter : public tRisseObjectInterface
{
public:
	//! @brief		Risseプロパティが書き込まれる際に呼ばれるメソッドのtypedef
	//! @param		_class		クラス
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tSetter)(tRisseClassBase * _class, TT f,
		const tRisseNativeBindPropertySetterCallingInfo & info);


private:
	//! @brief		Risseクラスインスタンス
	tRisseClassBase * Class; 

	//! @brief		Risseプロパティが書き込まれる際に本当に呼ばれるネイティブな関数
	TT TargetFunction;

	//! @brief		Risseプロパティが書き込まれる際に呼ばれるメソッド
	tSetter Setter;

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		class_		Risseクラスインスタンス
	//! @param		target		Risseプロパティ書き込み時に本当に呼ばれるネイティブな関数
	//! @param		setter		Risseプロパティが書き込まれる際に呼ばれるメソッド
	tRisseNativeBindPropertySetter(tRisseScriptEngine * engine,
		tRisseClassBase * class_, TT target, tSetter setter) :
		tRisseObjectInterface(new tRisseRTTI(engine))
	{ Class = class_; TargetFunction = target; Setter = setter; }

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Risseネイティブプロパティ
//---------------------------------------------------------------------------
template <typename TT> // target-type, void (tRisseObjectBase::*)() とか
class tRisseNativeBindProperty : public tRisseObjectInterface
{
	//! @param		親クラスのtypedef
	typedef tRisseObjectInterface inherited;

	tRisseObjectInterface * Getter; //!< Risseプロパティが読み込まれる際に呼ばれるメソッド
	tRisseObjectInterface * Setter; //!< Risseプロパティが書き込まれる際に呼ばれるメソッド

protected:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		Class		クラスインスタンス
	//! @param		getter		Risseプロパティが読み込まれる際に呼ばれるメソッド
	//! @param		setter		Risseプロパティが書き込まれる際に呼ばれるメソッド
	tRisseNativeBindProperty(tRisseScriptEngine * engine, tRisseClassBase * Class,
		TT gettertarget, typename tRisseNativeBindPropertyGetter<TT>::tGetter getter,
		TT settertarget, typename tRisseNativeBindPropertySetter<TT>::tSetter setter)
		: tRisseObjectInterface(new tRisseRTTI(engine)),
		Getter(new tRisseNativeBindPropertyGetter<TT>(engine, Class, gettertarget, getter)),
		Setter(new tRisseNativeBindPropertySetter<TT>(engine, Class, settertarget, setter)) {}

public:
	//! @brief		新しいプロパティインスタンスを生成して返す(コンストラクタではなくてこちらを呼ぶこと)
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		Class		クラスインスタンス
	//! @param		getter		Risseプロパティが読み込まれる際に呼ばれるメソッド
	//! @param		setter		Risseプロパティが書き込まれる際に呼ばれるメソッド
	static tRisseObjectInterface * New(tRisseScriptEngine * engine,
		tRisseClassBase * Class,
		TT gettertarget, typename tRisseNativeBindPropertyGetter<TT>::tGetter getter,
		TT settertarget, typename tRisseNativeBindPropertySetter<TT>::tSetter setter);

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
// プロパティ関連
//---------------------------------------------------------------------------
// static getter
template <typename CC, typename GR>
class tRisseBinderPropertyGetterS
{
	typedef GR (*tFunc)();
public:
	static void Call(tRisseClassBase * _class, void (*f)(),
		const tRisseNativeBindPropertyGetterCallingInfo & info)
	{
		if(info.result)
			*info.result = RisseToVariant(((tFunc)f)());
		else
			((tFunc)f)();
	}
};
// non-static getter
template <typename CC, typename IC, typename GR>
class tRisseBinderPropertyGetter
{
	typedef GR (IC::*tFunc)();
public:
	static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
		const tRisseNativeBindPropertyGetterCallingInfo & info)
	{
		IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
		if(info.result)
			*info.result = RisseToVariant((instance->*((tFunc)f))());
		else
			(instance->*((tFunc)f))();
	}
};
// static getter with calling info
template <typename CC>
class tRisseBinderPropertyGetter_InfoS
{
	typedef void (*tFunc)(const tRisseNativeBindPropertyGetterCallingInfo & info);
public:
	static void Call(tRisseClassBase * _class, void (*f)(),
		const tRisseNativeBindPropertyGetterCallingInfo & info)
	{
		((tFunc)f)(info);
	}
};
// non-static getter with calling info
template <typename CC, typename IC>
class tRisseBinderPropertyGetter_Info
{
	typedef void (IC::*tFunc)(const tRisseNativeBindPropertyGetterCallingInfo & info);
public:
	static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
		const tRisseNativeBindPropertyGetterCallingInfo & info)
	{
		IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
		(instance->*((tFunc)f))(info);
	}
};
// static setter
template <typename CC, typename ST>
class tRisseBinderPropertySetterS
{
	typedef void (*tFunc)(ST);
public:
	static void Call(tRisseClassBase * _class, void (*f)(),
		const tRisseNativeBindPropertySetterCallingInfo & info)
	{
		((tFunc)f)(RisseFromVariant<ST>(info.value));
	}
};
// non-static setter
template <typename CC, typename IC, typename ST>
class tRisseBinderPropertySetter
{
	typedef void (IC::*tFunc)(ST);
public:
	static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
		const tRisseNativeBindPropertySetterCallingInfo & info)
	{
		IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
		(instance->*((tFunc)f))(RisseFromVariant<ST>(info.value));
	}
};

// static without calling info getter
template <typename CC, typename GR, typename ST>
void RisseRegisterBinder(CC * _class, const tRisseString & name,
	GR (*getter)(), void (*setter)(ST),
	tRisseMemberAttribute attribute = tRisseMemberAttribute(),
	const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
{
	attribute.Set(tRisseMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tRisseVariantBlock(
			tRisseNativeBindProperty<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				getter ? reinterpret_cast<void (*)()>(getter) : NULL,
				getter ? &tRisseBinderPropertyGetterS<CC, GR >::Call : NULL,
				setter ? reinterpret_cast<void (*)()>(setter) : NULL,
				setter ? &tRisseBinderPropertySetterS<CC, ST >::Call : NULL
				), context), attribute);
}

// non-static without calling info getter
template <typename CC, typename IC, typename GR, typename ST>
void RisseRegisterBinder(CC * _class, const tRisseString & name,
	GR (IC::*getter)() const, void (IC::*setter)(ST),
	tRisseMemberAttribute attribute = tRisseMemberAttribute(),
	const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
{
	attribute.Set(tRisseMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tRisseVariantBlock(
			tRisseNativeBindProperty<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				getter ? reinterpret_cast<void (tRisseObjectBase::*)()>(getter) : NULL,
				getter ? &tRisseBinderPropertyGetter<CC, IC, GR >::Call : NULL,
				setter ? reinterpret_cast<void (tRisseObjectBase::*)()>(setter) : NULL,
				setter ? &tRisseBinderPropertySetter<CC, IC, ST >::Call : NULL
				), context), attribute);
}

// static with calling info getter
template <typename CC, typename ST>
void RisseRegisterBinder(CC * _class, const tRisseString & name,
	void (*getter)(const tRisseNativeBindPropertyGetterCallingInfo &), void (*setter)(ST),
	tRisseMemberAttribute attribute = tRisseMemberAttribute(),
	const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
{
	attribute.Set(tRisseMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tRisseVariantBlock(
			tRisseNativeBindProperty<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				getter ? reinterpret_cast<void (*)()>(getter) : NULL,
				getter ? &tRisseBinderPropertyGetter_InfoS<CC>::Call : NULL,
				setter ? reinterpret_cast<void (*)()>(setter) : NULL,
				setter ? &tRisseBinderPropertySetterS<CC, ST >::Call : NULL
				), context), attribute);
}

// non-static with calling info getter
template <typename CC, typename IC, typename ST>
void RisseRegisterBinder(CC * _class, const tRisseString & name,
	void (IC::*getter)(const tRisseNativeBindPropertyGetterCallingInfo &), void (IC::*setter)(ST),
	tRisseMemberAttribute attribute = tRisseMemberAttribute(),
	const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
{
	attribute.Set(tRisseMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tRisseVariantBlock(
			tRisseNativeBindProperty<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				getter ? reinterpret_cast<void (tRisseObjectBase::*)()>(getter) : NULL,
				getter ? &tRisseBinderPropertyGetter_Info<CC, IC>::Call : NULL,
				setter ? reinterpret_cast<void (tRisseObjectBase::*)()>(setter) : NULL,
				setter ? &tRisseBinderPropertySetter<CC, IC, ST >::Call : NULL
				), context), attribute);
}



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

