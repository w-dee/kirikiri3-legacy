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
/*! @note

native binder について

以下に説明する RisseBindFunction と RisseBindProperty は tRisseClassBase 派生
クラスの RegisterMembers 内に通常は記述する。


■ RisseBindFunction(Class, name, funcptr, attribute, context)

  Class      クラスインスタンスオブジェクト。通常 tRisseClassBase の
             サブクラスのインスタンスなので、tRisseClassBase 派生
             クラスの RegisterMembers 内では this になる。
  name       このメソッドを登録するメンバ名
  funcptr    実際に呼び出されるC++メソッドポインタ
  attribute  tRisseMemberAttribute 型のメンバ属性
             (省略可、省略するとデフォルトの属性になる)
  context    このメソッドが実行されるコンテキスト。省略すると
             this になる。

  funcptr に記述するC++メソッドはstaticでも非staticでもよい。
  非staticの場合は、tRisseClassBase::CreateNewObjectBase() をオーバーライドした
  C++ メソッドで返すRisseインスタンスの、そのC++クラスのメンバ関数である必要がある。
  たとえば Array クラスの場合、

	void tRisseArrayInstance::iset(const tRisseVariant & value, risse_offset ofs_index)

  のようなメソッドが funcptr に渡っている。この tRisseArrayInstance は
  tRisseArrayClass::CreateNewObjectBase() で返されているインスタンスのC++クラスである。

  funcptr に非 static メソッドが渡った場合、関数が呼び出される際、必ず this が
  正しいインスタンスであるかどうかの RTTI によるチェックが入る。その後、funcptr が
  呼ばれる。
  static メソッドが渡った場合は、このようなチェックはいっさい行われない。
  static メソッドが渡された場合は、その static メソッドが属するクラスは問わない。

  funcptr に渡す関数の戻り値の型は void あるいは任意の型だが、tRisseVariant に
  変換可能な型、あるいは RisseToVariant で変換可能な型に限られる。もし標準で変換
  出来ないような型の場合は、RisseToVariant を特殊化することにより独自の変換ルールを
  書くこと。

  funcptr に渡す関数の引数は、任意の型だが、これも tRisseVariant から変換可能な型、
  あるいは RisseFromVariant で変換可能な型に限られる。もし標準で変換
  出来ないような型の場合は、RisseFromVariant を特殊化することにより独自の変換ルールを
  書くこと。

  funcptr がたとえば4つの引数をとるメソッドの場合は、Risseでも4つの引数が必須となる
  (これより少ない場合は例外が発生する)。ただし、引数の型が以下の場合は特殊で、これらは
  必須引数の数には数えられないし、Risse の引数には対応しない。

  const tRisseNativeCallInfo &
     メソッド呼び出しに関する情報を含む構造体

  tRisseScriptEngine *
     スクリプトエンジンインスタンス

  const tRisseMethodArgument &
     引数。特に、可変長の引数を取る場合は、これにより情報を得ることが出来る。


  たとえば

  	void eval( const tRisseNativeCallInfo &info, const tRisseString & script) const;

  というメソッドの場合は、const tRisseNativeCallInfo &info は必須引数に
  数えられないが、const tRisseString & script は必須引数に数えられる。また、info は
  Risse の引数には対応しない。つまり、この Risse メソッドの必須引数は 1 個で最初の引数が
  script に渡ることになる。


■ RisseBindProperty(Class, name, getter, setter, attribute, context)
■ RisseBindProperty(Class, name, getter, attribute, context)

  Class      クラスインスタンスオブジェクト。通常 tRisseClassBase の
             サブクラスのインスタンスなので、tRisseClassBase 派生
             クラスの RegisterMembers 内では this になる。
  name       このプロパティを登録するメンバ名
  getter     ゲッタに対応するC++関数ポインタ
  setter     セッタに対応するC++関数ポインタ
             (省略すると読み出し専用プロパティになる)
  attribute  tRisseMemberAttribute 型のメンバ属性
             (省略可、省略するとデフォルトの属性になる)
  context    このプロパティが実行されるコンテキスト。省略すると
             this になる。

  プロパティを登録する。

  getter は以下の形のC++メソッドを指定する。

  型 (*getter)()
  void (*getter)(const tRisseNativePropGetInfo &)

  前者は単純で、「型」で表される型の値が戻ることを表す。これは RisseToVariant で
  変換可能な型である必要がある。後者の場合は、プロパティ読み出しに関する
  情報が渡ってくる。

  setter は以下の形のC++メソッドを指定する。

  void (*setter)(型)
  void (*setter)(const tRisseNativePropSetInfo &)

  前者は簡単な形で、「型」で表される型の値を設定することを表す。この型は
  RisseFromVariant で変換可能な型である必要がある。
  後者の場合は、プロパティ書き込みに関する情報が渡ってくる。

  getterもsetterも、関数ポインタはメンバポインタでも、staticメンバ関数の
  ポインタでもよい (メソッドの場合と一緒である)。

*/


namespace Risse
{
class tRisseClassBase;
//---------------------------------------------------------------------------
//! @brief		ネイティブ関数呼び出し情報
//---------------------------------------------------------------------------
class tRisseNativeCallInfo
{
public:
	tRisseScriptEngine * engine;		//!< スクリプトエンジンインスタンス
	tRisseVariant * result;				//!< 結果の格納先 (NULLの場合は結果が要らない場合)
	tRisseOperateFlags flags;			//!< オペレーションフラグ
	const tRisseMethodArgument & args;	//!< 引数
	const tRisseVariant &This;			//!< メソッドが実行されるべき"Thisオブジェクト"
										//!< (NULL="Thisオブジェクト"を指定しない場合)
	tRisseClassBase * Class;			//!< クラスインスタンス

	//! @brief		コンストラクタ
	//! @param		engine_	スクリプトエンジンインスタンス
	//! @param		result_	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags_	オペレーションフラグ
	//! @param		args_	引数
	//! @param		This_	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		Class_	クラスインスタンス
	tRisseNativeCallInfo(
		tRisseScriptEngine * engine_,
		tRisseVariant * result_,
		tRisseOperateFlags flags_,
		const tRisseMethodArgument & args_,
		const tRisseVariant &This_,
		tRisseClassBase * Class_):
		engine(engine_), result(result_), flags(flags_), args(args_), This(This_), Class(Class_) {;}

	//! @brief		親クラスの initialize を呼ぶ(ユーティリティメソッド)
	//! @param		args		引数 (空の引数で呼び出したい場合は指定しない.
	//!							与えられた引数をそのまま渡したい場合はこのオブジェクトの args を渡す)
	void InitializeSuperClass(const tRisseMethodArgument & args = tRisseMethodArgument::Empty()) const;
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
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tCallee)(TT f, const tRisseNativeCallInfo & info);

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
class tRisseNativePropGetInfo
{
public:
	tRisseScriptEngine * engine;		//!< スクリプトエンジンインスタンス
	tRisseVariant * result;				//!< 結果の格納先 (NULLの場合は結果が要らない場合)
	tRisseOperateFlags flags;			//!< オペレーションフラグ
	const tRisseVariant &This;			//!< メソッドが実行されるべき"Thisオブジェクト"
										//!< (NULL="Thisオブジェクト"を指定しない場合)
	tRisseClassBase * Class;			//!< クラスインスタンス


	//! @brief		コンストラクタ
	//! @param		engine_	スクリプトエンジンインスタンス
	//! @param		result_	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags_	オペレーションフラグ
	//! @param		This_	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		Class_	クラスインスタンス
	tRisseNativePropGetInfo(
		tRisseScriptEngine * engine_,
		tRisseVariant * result_,
		tRisseOperateFlags flags_,
		const tRisseVariant &This_,
		tRisseClassBase * Class_):
		engine(engine_), result(result_), flags(flags_), This(This_), Class(Class_) {;}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ネイティブプロパティセッター呼び出し情報
//---------------------------------------------------------------------------
class tRisseNativePropSetInfo
{
public:
	tRisseScriptEngine * engine;		//!< スクリプトエンジンインスタンス
	const tRisseVariant & value;		//!< 値
	tRisseOperateFlags flags;			//!< オペレーションフラグ
	const tRisseVariant &This;			//!< メソッドが実行されるべき"Thisオブジェクト"
										//!< (NULL="Thisオブジェクト"を指定しない場合)
	tRisseClassBase * Class;			//!< クラスインスタンス


	//! @brief		コンストラクタ
	//! @param		engine_	スクリプトエンジンインスタンス
	//! @param		value_	値
	//! @param		flags_	オペレーションフラグ
	//! @param		This_	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		Class_	クラスインスタンス
	tRisseNativePropSetInfo(
		tRisseScriptEngine * engine_,
		const tRisseVariant & value_,
		tRisseOperateFlags flags_,
		const tRisseVariant &This_,
		tRisseClassBase * Class_):
		engine(engine_), value(value_), flags(flags_), This(This_), Class(Class_) {;}
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
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tGetter)(TT f, const tRisseNativePropGetInfo & info);

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
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tSetter)(TT f, const tRisseNativePropSetInfo & info);


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
// メタ型 (tRisseNativeCallInfo内の情報つまり関数の呼び出しに関する
// 情報) なのか 実引数なのか
// メタ型でない場合は value が 1 になる
// メタ型の場合は RisseFromVariantOrCallingInfo でメタ型に変換できる
template <typename T> struct tRisseIsFuncCallNonMetaType { enum { value = 1 }; };
template <> struct tRisseIsFuncCallNonMetaType<const tRisseNativeCallInfo &>
													{ enum { value = 0 }; };
template <> struct tRisseIsFuncCallNonMetaType<tRisseScriptEngine *>
													{ enum { value = 0 }; };
template <> struct tRisseIsFuncCallNonMetaType<const tRisseMethodArgument &>
													{ enum { value = 0 }; };
//---------------------------------------------------------------------------
// T への、メソッドへの引数あるいは呼び出し情報からの変換
// RisseFVoC = RisseFromVariantOrCallingInfo
template <typename T, int N>
struct tRisseFVoC
{
	static T Cnv(const tRisseNativeCallInfo & info)
	{
		return RisseFromVariant<T>(info.args[N]);
	}
};
//---------------------------------------------------------------------------
template <int N>
struct tRisseFVoC<const tRisseNativeCallInfo &, N>
{
	static const tRisseNativeCallInfo & Cnv(
					const tRisseNativeCallInfo & info)
	{
		return info;
	}
};
//---------------------------------------------------------------------------
template <int N>
struct tRisseFVoC<tRisseScriptEngine *, N>
{
	static tRisseScriptEngine * Cnv(
					const tRisseNativeCallInfo & info)
	{
		return info.engine;
	}
};
//---------------------------------------------------------------------------
template <int N>
struct tRisseFVoC<const tRisseMethodArgument &, N>
{
	static const tRisseMethodArgument & Cnv(
					const tRisseNativeCallInfo & info)
	{
		return info.args;
	}
};
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
	static void Call(void (*f)(),
		const tRisseNativePropGetInfo & info)
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
	static void Call(void (tRisseObjectBase::*f)(),
		const tRisseNativePropGetInfo & info)
	{
		IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
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
	typedef void (*tFunc)(const tRisseNativePropGetInfo & info);
public:
	static void Call(void (*f)(),
		const tRisseNativePropGetInfo & info)
	{
		((tFunc)f)(info);
	}
};
// non-static getter with calling info
template <typename CC, typename IC>
class tRisseBinderPropertyGetter_Info
{
	typedef void (IC::*tFunc)(const tRisseNativePropGetInfo & info);
public:
	static void Call(void (tRisseObjectBase::*f)(),
		const tRisseNativePropGetInfo & info)
	{
		IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
		(instance->*((tFunc)f))(info);
	}
};
// static setter
template <typename CC, typename ST>
class tRisseBinderPropertySetterS
{
	typedef void (*tFunc)(ST);
public:
	static void Call(void (*f)(),
		const tRisseNativePropSetInfo & info)
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
	static void Call(void (tRisseObjectBase::*f)(),
		const tRisseNativePropSetInfo & info)
	{
		IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
		(instance->*((tFunc)f))(RisseFromVariant<ST>(info.value));
	}
};

// static without calling info getter
template <typename CC, typename GR, typename ST>
void RisseBindProperty(CC * _class, const tRisseString & name,
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
// static without calling info getter, read-only
template <typename CC, typename GR/*, typename ST*/>
void RisseBindProperty(CC * _class, const tRisseString & name,
	GR (*getter)()/*, void (*setter)(ST)*/,
	tRisseMemberAttribute attribute = tRisseMemberAttribute(),
	const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
{
	RisseBindProperty<CC, GR, const tRisseVariant &>(
		_class, name, getter, NULL, attribute, context);
}

// non-static without calling info getter, non-const getter
template <typename CC, typename GIC, typename SIC, typename GR, typename ST>
void RisseBindProperty(CC * _class, const tRisseString & name,
	GR (GIC::*getter)(), void (SIC::*setter)(ST),
	tRisseMemberAttribute attribute = tRisseMemberAttribute(),
	const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
{
	attribute.Set(tRisseMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tRisseVariantBlock(
			tRisseNativeBindProperty<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				getter ? reinterpret_cast<void (tRisseObjectBase::*)()>(getter) : NULL,
				getter ? &tRisseBinderPropertyGetter<CC, GIC, GR >::Call : NULL,
				setter ? reinterpret_cast<void (tRisseObjectBase::*)()>(setter) : NULL,
				setter ? &tRisseBinderPropertySetter<CC, SIC, ST >::Call : NULL
				), context), attribute);
}
// non-static without calling info getter, non-const getter, read-only
template <typename CC, typename GIC/*, typename SIC*/, typename GR/*, typename ST*/>
void RisseBindProperty(CC * _class, const tRisseString & name,
	GR (GIC::*getter)()/*, void (SIC::*setter)(ST)*/,
	tRisseMemberAttribute attribute = tRisseMemberAttribute(),
	const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
{
	RisseBindProperty<CC, GIC, tRisseObjectInterface, GR, const tRisseVariant &>(
		_class, name, getter, NULL, attribute, context);
}


// static with calling info getter
template <typename CC, typename ST>
void RisseBindProperty(CC * _class, const tRisseString & name,
	void (*getter)(const tRisseNativePropGetInfo &), void (*setter)(ST),
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
// static with calling info getter, read-only
template <typename CC/*, typename ST*/>
void RisseBindProperty(CC * _class, const tRisseString & name,
	void (*getter)(const tRisseNativePropGetInfo &)/*, void (*setter)(ST)*/,
	tRisseMemberAttribute attribute = tRisseMemberAttribute(),
	const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
{
	RisseBindProperty<CC, const tRisseVariant &>(
		_class, name, getter, NULL, attribute, context);
}

// non-static with calling info getter, non-const getter
template <typename CC, typename GIC, typename SIC, typename ST>
void RisseBindProperty(CC * _class, const tRisseString & name,
	void (GIC::*getter)(const tRisseNativePropGetInfo &), void (SIC::*setter)(ST),
	tRisseMemberAttribute attribute = tRisseMemberAttribute(),
	const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
{
	attribute.Set(tRisseMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tRisseVariantBlock(
			tRisseNativeBindProperty<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				getter ? reinterpret_cast<void (tRisseObjectBase::*)()>(getter) : NULL,
				getter ? &tRisseBinderPropertyGetter_Info<CC, GIC>::Call : NULL,
				setter ? reinterpret_cast<void (tRisseObjectBase::*)()>(setter) : NULL,
				setter ? &tRisseBinderPropertySetter<CC, SIC, ST >::Call : NULL
				), context), attribute);
}
// non-static with calling info getter, non-const getter, read-only
template <typename CC, typename GIC/*, typename SIC, typename ST*/>
void RisseBindProperty(CC * _class, const tRisseString & name,
	void (GIC::*getter)(const tRisseNativePropGetInfo &)/*, void (SIC::*setter)(ST)*/,
	tRisseMemberAttribute attribute = tRisseMemberAttribute(),
	const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
{
	RisseBindProperty<CC, GIC, tRisseObjectInterface, const tRisseVariant &>(
		_class, name, getter, NULL, attribute, context);
}

//---------------------------------------------------------------------------
// 関数オブジェクトなど
// いまのところ RisseBindFunction や RisseBindProperty はどのクラスにも属していない。
// ネイティブ関数の定義が必要ないところに、かなり大きいテンプレートライブラリを
// 読み込みたくなく、RisseBindFunction や RisseBindProperty をそのほかの実装/定義と分離するためである。
//---------------------------------------------------------------------------
#include "risseNativeBinderTemplates.inc"
//---------------------------------------------------------------------------

/*
	TODO: 呼び出しの効率化
	RTTI が増えるかもしれないがのう
*/

}
#endif

