//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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

以下に説明する BindFunction と BindProperty は tClassBase 派生
クラスの RegisterMembers 内に通常は記述する。


■ BindFunction(Class, name, funcptr, attribute, context, is_members)

  Class      クラスインスタンスオブジェクト。通常 tClassBase の
             サブクラスのインスタンスなので、tClassBase 派生
             クラスの RegisterMembers 内では this になる。
  name       このメソッドを登録するメンバ名
  funcptr    実際に呼び出されるC++メソッドポインタ
  attribute  tMemberAttribute 型のメンバ属性
             (省略可、省略するとデフォルトの属性になる)
  context    このメソッドが実行されるコンテキスト。省略すると
             this になる。
  is_members members に登録するかどうか。省略すると true になる。

  funcptr に記述するC++メソッドはstaticでも非staticでもよい。
  非staticの場合は、tClassBase::ovulate() をオーバーライドした
  C++ メソッドで返すRisseインスタンスの、そのC++クラスのメンバ関数である必要がある。
  たとえば Array クラスの場合、

	void tArrayInstance::iset(const tVariant & value, risse_offset ofs_index)

  のようなメソッドが funcptr に渡っている。この tArrayInstance は
  tArrayClass::ovulate() で返されているインスタンスのC++クラスである。

  funcptr に非 static メソッドが渡った場合、関数が呼び出される際、必ず this が
  正しいインスタンスであるかどうかの RTTI によるチェックが入る。その後、funcptr が
  呼ばれる。
  static メソッドが渡った場合は、このようなチェックはいっさい行われない。
  static メソッドが渡された場合は、その static メソッドが属するクラスは問わない。

  funcptr に渡す関数の戻り値の型は void あるいは任意の型だが、tVariant に
  変換可能な型、あるいは ToVariant で変換可能な型に限られる。もし標準で変換
  出来ないような型の場合は、ToVariant を特殊化することにより独自の変換ルールを
  書くこと。

  もし funcptr に渡す関数の戻り値の型が void だった場合、Risse では自動的に this が帰る
  関数になる。

  funcptr に渡す関数の引数は、任意の型だが、これも tVariant から変換可能な型、
  あるいは FromVariant で変換可能な型に限られる。もし標準で変換
  出来ないような型の場合は、FromVariant を特殊化することにより独自の変換ルールを
  書くこと。

  funcptr がたとえば4つの引数をとるメソッドの場合は、Risseでも4つの引数が必須となる
  (これより少ない場合は例外が発生する)。ただし、引数の型が以下の場合は特殊で、これらは
  必須引数の数には数えられないし、Risse の引数には対応しない。

  const tNativeCallInfo &
     メソッド呼び出しに関する情報を含む構造体

  tScriptEngine *
     スクリプトエンジンインスタンス

  const tMethodArgument &
     引数。特に、可変長の引数を取る場合は、これにより情報を得ることが出来る。


  たとえば

  	void eval( const tNativeCallInfo &info, const tString & script) const;

  というメソッドの場合は、const tNativeCallInfo &info は必須引数に
  数えられないが、const tString & script は必須引数に数えられる。また、info は
  Risse の引数には対応しない。つまり、この Risse メソッドの必須引数は 1 個で最初の引数が
  script に渡ることになる。


 tNativeCallInfo には便利なメソッド tNativeCallInfo::InitializeSuperClass()
 があり、これを用いると簡単に親クラスの initialize メソッドを呼び出すことが出来る。



■ BindProperty(Class, name, getter, setter, attribute, context, is_members)
■ BindProperty(Class, name, getter, attribute, context, is_members)

  Class      クラスインスタンスオブジェクト。通常 tClassBase の
             サブクラスのインスタンスなので、tClassBase 派生
             クラスの RegisterMembers 内では this になる。
  name       このプロパティを登録するメンバ名
  getter     ゲッタに対応するC++関数ポインタ
  setter     セッタに対応するC++関数ポインタ
             (省略すると読み出し専用プロパティになる)
  attribute  tMemberAttribute 型のメンバ属性
             (省略可、省略するとデフォルトの属性になる)
  context    このプロパティが実行されるコンテキスト。省略すると
             this になる。
  is_members members に登録するかどうか。省略すると true になる。

  プロパティを登録する。

  getter は以下の形のC++メソッドを指定する。

  型 (*getter)()
  void (*getter)(const tNativePropGetInfo &)

  前者は単純で、「型」で表される型の値が戻ることを表す。これは ToVariant で
  変換可能な型である必要がある。後者の場合は、プロパティ読み出しに関する
  情報が渡ってくる。

  setter は以下の形のC++メソッドを指定する。

  void (*setter)(型)
  void (*setter)(const tNativePropSetInfo &)

  前者は簡単な形で、「型」で表される型の値を設定することを表す。この型は
  FromVariant で変換可能な型である必要がある。
  後者の場合は、プロパティ書き込みに関する情報が渡ってくる。

  getterもsetterも、関数ポインタはメンバポインタでも、staticメンバ関数の
  ポインタでもよい (メソッドの場合と一緒である)。

*/


namespace Risse
{
class tClassBase;
//---------------------------------------------------------------------------
//! @brief		ネイティブ関数呼び出し情報
//---------------------------------------------------------------------------
class tNativeCallInfo : public tCollectee
{
public:
	tScriptEngine * engine;		//!< スクリプトエンジンインスタンス
	tVariant * result;				//!< 結果の格納先 (NULLの場合は結果が要らない場合)
	tOperateFlags flags;			//!< オペレーションフラグ
	const tMethodArgument & args;	//!< 引数
	const tVariant &This;			//!< メソッドが実行されるべき"Thisオブジェクト"
										//!< (NULL="Thisオブジェクト"を指定しない場合)
	tClassBase * Class;			//!< クラスインスタンス

	//! @brief		コンストラクタ
	//! @param		engine_	スクリプトエンジンインスタンス
	//! @param		result_	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags_	オペレーションフラグ
	//! @param		args_	引数
	//! @param		This_	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		Class_	クラスインスタンス
	tNativeCallInfo(
		tScriptEngine * engine_,
		tVariant * result_,
		tOperateFlags flags_,
		const tMethodArgument & args_,
		const tVariant &This_,
		tClassBase * Class_):
		engine(engine_), result(result_), flags(flags_), args(args_), This(This_), Class(Class_) {;}

	//! @brief		親クラスの initialize を呼ぶ(ユーティリティメソッド)
	//! @param		args		引数 (空の引数で呼び出したい場合は指定しない.
	//!							与えられた引数をそのまま渡したい場合はこのオブジェクトの args を渡す)
	void InitializeSuperClass(const tMethodArgument & args = tMethodArgument::Empty()) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risseネイティブ関数
//---------------------------------------------------------------------------
template <typename TT> // target-type, void (tObjectBase::*)() とか
class tNativeBindFunction : public tObjectInterface
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッドのtypedef
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tCallee)(TT f, const tNativeCallInfo & info);

	//! @brief		Risseクラスインスタンス
	tClassBase * Class; 

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
	tNativeBindFunction(tScriptEngine * engine,
		tClassBase * class_, TT target, tCallee callee) :
		tObjectInterface(new tRTTI(engine))
		{ Class = class_; TargetFunction = target; Callee = callee; }

public:
	//! @brief		新しい関数インスタンスを生成して返す(コンストラクタではなくてこちらを呼ぶこと)
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		class_		Risseクラスインスタンス
	//! @param		target		Risseメソッド呼び出し時に本当に呼ばれるネイティブな関数
	//! @param		callee		Risseメソッド呼び出し時に呼ばれるメソッド
	static tObjectInterface * New(tScriptEngine * engine,
		tClassBase * class_, TT target, tCallee callee);

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ネイティブプロパティゲッター呼び出し情報
//---------------------------------------------------------------------------
class tNativePropGetInfo : public tCollectee
{
public:
	tScriptEngine * engine;		//!< スクリプトエンジンインスタンス
	tVariant * result;				//!< 結果の格納先 (NULLの場合は結果が要らない場合)
	tOperateFlags flags;			//!< オペレーションフラグ
	const tVariant &This;			//!< メソッドが実行されるべき"Thisオブジェクト"
										//!< (NULL="Thisオブジェクト"を指定しない場合)
	tClassBase * Class;			//!< クラスインスタンス


	//! @brief		コンストラクタ
	//! @param		engine_	スクリプトエンジンインスタンス
	//! @param		result_	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags_	オペレーションフラグ
	//! @param		This_	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		Class_	クラスインスタンス
	tNativePropGetInfo(
		tScriptEngine * engine_,
		tVariant * result_,
		tOperateFlags flags_,
		const tVariant &This_,
		tClassBase * Class_):
		engine(engine_), result(result_), flags(flags_), This(This_), Class(Class_) {;}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ネイティブプロパティセッター呼び出し情報
//---------------------------------------------------------------------------
class tNativePropSetInfo : public tCollectee
{
public:
	tScriptEngine * engine;		//!< スクリプトエンジンインスタンス
	const tVariant & value;		//!< 値
	tOperateFlags flags;			//!< オペレーションフラグ
	const tVariant &This;			//!< メソッドが実行されるべき"Thisオブジェクト"
										//!< (NULL="Thisオブジェクト"を指定しない場合)
	tClassBase * Class;			//!< クラスインスタンス


	//! @brief		コンストラクタ
	//! @param		engine_	スクリプトエンジンインスタンス
	//! @param		value_	値
	//! @param		flags_	オペレーションフラグ
	//! @param		This_	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		Class_	クラスインスタンス
	tNativePropSetInfo(
		tScriptEngine * engine_,
		const tVariant & value_,
		tOperateFlags flags_,
		const tVariant &This_,
		tClassBase * Class_):
		engine(engine_), value(value_), flags(flags_), This(This_), Class(Class_) {;}
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		RisseネイティブプロパティのGetterの実装
//---------------------------------------------------------------------------
template <typename TT> // target-type, void (tObjectBase::*)() とか
class tNativeBindPropertyGetter : public tObjectInterface
{
public:
	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tNativeBindPropertyGetter() {}

	//! @brief		Risseプロパティが読み込まれる際に呼ばれるメソッドのtypedef
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tGetter)(TT f, const tNativePropGetInfo & info);

private:
	//! @brief		Risseクラスインスタンス
	tClassBase * Class; 

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
	tNativeBindPropertyGetter(tScriptEngine * engine,
		tClassBase * class_, TT target, tGetter getter) :
		tObjectInterface(new tRTTI(engine))
	{ Class = class_; TargetFunction = target; Getter = getter; }

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		RisseネイティブプロパティのSetterの実装
//---------------------------------------------------------------------------
template <typename TT> // target-type, void (tObjectBase::*)() とか
class tNativeBindPropertySetter : public tObjectInterface
{
public:
	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tNativeBindPropertySetter() {}

	//! @brief		Risseプロパティが書き込まれる際に呼ばれるメソッドのtypedef
	//! @param		f			呼び出し先オブジェクト
	//! @param		info		呼び出し情報
	typedef void (*tSetter)(TT f, const tNativePropSetInfo & info);


private:
	//! @brief		Risseクラスインスタンス
	tClassBase * Class; 

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
	tNativeBindPropertySetter(tScriptEngine * engine,
		tClassBase * class_, TT target, tSetter setter) :
		tObjectInterface(new tRTTI(engine))
	{ Class = class_; TargetFunction = target; Setter = setter; }

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Risseネイティブプロパティ
//---------------------------------------------------------------------------
template <typename TT> // target-type, void (tObjectBase::*)() とか
class tNativeBindProperty : public tObjectInterface
{
	//! @param		親クラスのtypedef
	typedef tObjectInterface inherited;

	tObjectInterface * Getter; //!< Risseプロパティが読み込まれる際に呼ばれるメソッド
	tObjectInterface * Setter; //!< Risseプロパティが書き込まれる際に呼ばれるメソッド

public:
	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tNativeBindProperty() {}

protected:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		Class		クラスインスタンス
	//! @param		getter		Risseプロパティが読み込まれる際に呼ばれるメソッド
	//! @param		setter		Risseプロパティが書き込まれる際に呼ばれるメソッド
	tNativeBindProperty(tScriptEngine * engine, tClassBase * Class,
		TT gettertarget, typename tNativeBindPropertyGetter<TT>::tGetter getter,
		TT settertarget, typename tNativeBindPropertySetter<TT>::tSetter setter)
		: tObjectInterface(new tRTTI(engine)),
		Getter(new tNativeBindPropertyGetter<TT>(engine, Class, gettertarget, getter)),
		Setter(new tNativeBindPropertySetter<TT>(engine, Class, settertarget, setter)) {}

public:
	//! @brief		新しいプロパティインスタンスを生成して返す(コンストラクタではなくてこちらを呼ぶこと)
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		Class		クラスインスタンス
	//! @param		getter		Risseプロパティが読み込まれる際に呼ばれるメソッド
	//! @param		setter		Risseプロパティが書き込まれる際に呼ばれるメソッド
	static tObjectInterface * New(tScriptEngine * engine,
		tClassBase * Class,
		TT gettertarget, typename tNativeBindPropertyGetter<TT>::tGetter getter,
		TT settertarget, typename tNativeBindPropertySetter<TT>::tSetter setter);

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// 参照の付いていない型を得るためのテンプレートクラス
//---------------------------------------------------------------------------
template <typename T> struct tRemoveReference     { typedef T type; };
template <typename T> struct tRemoveReference<T&> { typedef T type; };
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// 各種型 -> tVariant 変換
// 自前の型がある場合は ToVariant のテンプレート特殊化を行うこと。
// tVariant のメンバ関数の方がよいのでは？という考えもあるかもしれないが
// 新しい型を定義し、その型の定義にも tVariant の定義にも手を加えずに
// 変換規則を書きたい場合などはこっちの方が都合がよい。
//---------------------------------------------------------------------------
template <typename T>
inline tVariant ToVariant(T s)
{
	return tVariant(s);
}

//---------------------------------------------------------------------------
// 数値系
/*
template <>
inline tVariant ToVariant<risse_size>(risse_size s)
{
	return tVariant((risse_int64)s);
}
*/
template <>
inline tVariant ToVariant<risse_uint64>(risse_uint64 s)
{
	return tVariant((risse_int64)s);
}
template <>
inline tVariant ToVariant<risse_uint32>(risse_uint32 s)
{
	return tVariant((risse_int64)s);
}
template <>
inline tVariant ToVariant<risse_offset>(risse_offset s)
{
	return tVariant((risse_int64)s);
}
template <>
inline tVariant ToVariant<float>(float s)
{
	return tVariant((double)s);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tVariant -> 各種型 変換
// 自前の型がある場合は FromVariant のテンプレート特殊化を行うこと。
//---------------------------------------------------------------------------
template <typename T>
inline typename tRemoveReference<T>::type FromVariant(const tVariant & v)
{
	return (typename tRemoveReference<T>::type)v;
}

//---------------------------------------------------------------------------
// 数値系
/*
template <>
inline risse_size FromVariant<risse_size>(const tVariant & v)
{
	return (risse_size)(risse_int64)v;
}
*/
template <>
inline risse_uint64 FromVariant<risse_uint64>(const tVariant & v)
{
	return (risse_uint64)(risse_int64)v;
}
template <>
inline risse_uint32 FromVariant<risse_uint32>(const tVariant & v)
{
	return (risse_uint32)(risse_int64)v;
}
template <>
inline risse_offset FromVariant<risse_offset>(const tVariant & v)
{
	return (risse_offset)(risse_int64)v;
}
template <>
inline float FromVariant<float>(const tVariant & v)
{
	return (float)(double)v;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// 関数オブジェクトをサポートするためのテンプレート群
//---------------------------------------------------------------------------
// 型が同一かどうか
template <typename A, typename B> struct tIsSameType { enum { value = false }; };
template <typename A> struct tIsSameType<A,A> { enum { value = true }; };
//---------------------------------------------------------------------------
// メタ型 (tNativeCallInfo内の情報つまり関数の呼び出しに関する
// 情報) なのか 実引数なのか
// メタ型でない場合は value が 1 になる
// メタ型の場合は FromVariantOrCallingInfo でメタ型に変換できる
template <typename T> struct tIsFuncCallNonMetaType { enum { value = 1 }; };
template <> struct tIsFuncCallNonMetaType<const tNativeCallInfo &>
													{ enum { value = 0 }; };
template <> struct tIsFuncCallNonMetaType<tScriptEngine *>
													{ enum { value = 0 }; };
template <> struct tIsFuncCallNonMetaType<const tMethodArgument &>
													{ enum { value = 0 }; };
//---------------------------------------------------------------------------
// T への、メソッドへの引数あるいは呼び出し情報からの変換
// FVoC = FromVariantOrCallingInfo
template <typename T, int N>
struct tFVoC
{
	static typename tRemoveReference<T>::type Cnv(const tNativeCallInfo & info)
	{
		return FromVariant<T>(info.args[N]);
	}
};
//---------------------------------------------------------------------------
template <int N>
struct tFVoC<const tNativeCallInfo &, N>
{
	static const tNativeCallInfo & Cnv(
					const tNativeCallInfo & info)
	{
		return info;
	}
};
//---------------------------------------------------------------------------
template <int N>
struct tFVoC<tScriptEngine *, N>
{
	static tScriptEngine * Cnv(
					const tNativeCallInfo & info)
	{
		return info.engine;
	}
};
//---------------------------------------------------------------------------
template <int N>
struct tFVoC<const tMethodArgument &, N>
{
	static const tMethodArgument & Cnv(
					const tNativeCallInfo & info)
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
class tBinderPropertyGetterS
{
	typedef GR (*tFunc)();
public:
	static void Call(void (*f)(),
		const tNativePropGetInfo & info)
	{
		if(info.result)
			*info.result = ToVariant(((tFunc)f)());
		else
			((tFunc)f)();
	}
};
// non-static getter
template <typename CC, typename IC, typename GR>
class tBinderPropertyGetter
{
	typedef GR (IC::*tFunc)();
public:
	static void Call(void (tObjectBase::*f)(),
		const tNativePropGetInfo & info)
	{
		IC * instance = info.This.AssertAndGetObjectInterafce<IC>((CC*)info.Class);
		if(info.result)
			*info.result = ToVariant((instance->*((tFunc)f))());
		else
			(instance->*((tFunc)f))();
	}
};
// static getter with calling info
template <typename CC>
class tBinderPropertyGetter_InfoS
{
	typedef void (*tFunc)(const tNativePropGetInfo & info);
public:
	static void Call(void (*f)(),
		const tNativePropGetInfo & info)
	{
		((tFunc)f)(info);
	}
};
// static setter with calling info
template <typename CC>
class tBinderPropertySetter_InfoS
{
	typedef void (*tFunc)(const tNativePropSetInfo & info);
public:
	static void Call(void (*f)(),
		const tNativePropSetInfo & info)
	{
		((tFunc)f)(info);
	}
};
// non-static getter with calling info
template <typename CC, typename IC>
class tBinderPropertyGetter_Info
{
	typedef void (IC::*tFunc)(const tNativePropGetInfo & info);
public:
	static void Call(void (tObjectBase::*f)(),
		const tNativePropGetInfo & info)
	{
		IC * instance = info.This.AssertAndGetObjectInterafce<IC>((CC*)info.Class);
		(instance->*((tFunc)f))(info);
	}
};
// static setter
template <typename CC, typename ST>
class tBinderPropertySetterS
{
	typedef void (*tFunc)(ST);
public:
	static void Call(void (*f)(),
		const tNativePropSetInfo & info)
	{
		((tFunc)f)(FromVariant<ST>(info.value));
	}
};
// non-static setter
template <typename CC, typename IC, typename ST>
class tBinderPropertySetter
{
	typedef void (IC::*tFunc)(ST);
public:
	static void Call(void (tObjectBase::*f)(),
		const tNativePropSetInfo & info)
	{
		IC * instance = info.This.AssertAndGetObjectInterafce<IC>((CC*)info.Class);
		(instance->*((tFunc)f))(FromVariant<ST>(info.value));
	}
};

// static without calling info getter
template <typename CC, typename GR, typename ST>
void BindProperty(CC * _class, const tString & name,
	GR (*getter)(), void (*setter)(ST),
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	attribute.Set(tMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tVariantBlock(
			tNativeBindProperty<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				getter ? reinterpret_cast<void (*)()>(getter) : NULL,
				getter ? &tBinderPropertyGetterS<CC, GR >::Call : NULL,
				setter ? reinterpret_cast<void (*)()>(setter) : NULL,
				setter ? &tBinderPropertySetterS<CC, ST >::Call : NULL
				), context), attribute, is_members);
}
// static without calling info getter, read-only
template <typename CC, typename GR/*, typename ST*/>
void BindProperty(CC * _class, const tString & name,
	GR (*getter)()/*, void (*setter)(ST)*/,
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	BindProperty<CC, GR, const tVariant &>(
		_class, name, getter, NULL, attribute, context, is_members);
}

// non-static without calling info getter, non-const getter
template <typename CC, typename GIC, typename SIC, typename GR, typename ST>
void BindProperty(CC * _class, const tString & name,
	GR (GIC::*getter)(), void (SIC::*setter)(ST),
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	attribute.Set(tMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tVariantBlock(
			tNativeBindProperty<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				getter ? reinterpret_cast<void (tObjectBase::*)()>(getter) : NULL,
				getter ? &tBinderPropertyGetter<CC, GIC, GR >::Call : NULL,
				setter ? reinterpret_cast<void (tObjectBase::*)()>(setter) : NULL,
				setter ? &tBinderPropertySetter<CC, SIC, ST >::Call : NULL
				), context), attribute, is_members);
}
// non-static without calling info getter, const getter
template <typename CC, typename GIC, typename SIC, typename GR, typename ST>
void BindProperty(CC * _class, const tString & name,
	GR (GIC::*getter)() const, void (SIC::*setter)(ST),
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	attribute.Set(tMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tVariantBlock(
			tNativeBindProperty<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				getter ? reinterpret_cast<void (tObjectBase::*)()>(getter) : NULL,
				getter ? &tBinderPropertyGetter<CC, GIC, GR >::Call : NULL,
				setter ? reinterpret_cast<void (tObjectBase::*)()>(setter) : NULL,
				setter ? &tBinderPropertySetter<CC, SIC, ST >::Call : NULL
				), context), attribute, is_members);
}
// non-static without calling info getter, non-const getter, read-only
template <typename CC, typename GIC/*, typename SIC*/, typename GR/*, typename ST*/>
void BindProperty(CC * _class, const tString & name,
	GR (GIC::*getter)()/*, void (SIC::*setter)(ST)*/,
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	BindProperty<CC, GIC, tObjectBase, GR, const tVariant &>(
		_class, name, getter, NULL, attribute, context, is_members);
}
// non-static without calling info getter, const getter, read-only
template <typename CC, typename GIC/*, typename SIC*/, typename GR/*, typename ST*/>
void BindProperty(CC * _class, const tString & name,
	GR (GIC::*getter)() const/*, void (SIC::*setter)(ST)*/,
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	BindProperty<CC, GIC, tObjectBase, GR, const tVariant &>(
		_class, name, getter, NULL, attribute, context, is_members);
}


// static with calling info getter
template <typename CC, typename ST>
void BindProperty(CC * _class, const tString & name,
	void (*getter)(const tNativePropGetInfo &), void (*setter)(ST),
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	attribute.Set(tMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tVariantBlock(
			tNativeBindProperty<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				getter ? reinterpret_cast<void (*)()>(getter) : NULL,
				getter ? &tBinderPropertyGetter_InfoS<CC>::Call : NULL,
				setter ? reinterpret_cast<void (*)()>(setter) : NULL,
				setter ? &tBinderPropertySetterS<CC, ST >::Call : NULL
				), context), attribute, is_members);
}


// static with calling info getter, calling info setter
template <typename CC>
void BindProperty(CC * _class, const tString & name,
	void (*getter)(const tNativePropGetInfo &), void (*setter)(const tNativePropSetInfo &),
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	attribute.Set(tMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tVariantBlock(
			tNativeBindProperty<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				getter ? reinterpret_cast<void (*)()>(getter) : NULL,
				getter ? &tBinderPropertyGetter_InfoS<CC>::Call : NULL,
				setter ? reinterpret_cast<void (*)()>(setter) : NULL,
				setter ? &tBinderPropertySetter_InfoS<CC>::Call : NULL
				), context), attribute, is_members);
}

// static with calling info getter, read-only
template <typename CC/*, typename ST*/>
void BindProperty(CC * _class, const tString & name,
	void (*getter)(const tNativePropGetInfo &)/*, void (*setter)(ST)*/,
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	BindProperty<CC, const tVariant &>(
		_class, name, getter, NULL, attribute, context, is_members);
}

// non-static with calling info getter, non-const getter
template <typename CC, typename GIC, typename SIC, typename ST>
void BindProperty(CC * _class, const tString & name,
	void (GIC::*getter)(const tNativePropGetInfo &), void (SIC::*setter)(ST),
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	attribute.Set(tMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tVariantBlock(
			tNativeBindProperty<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				getter ? reinterpret_cast<void (tObjectBase::*)()>(getter) : NULL,
				getter ? &tBinderPropertyGetter_Info<CC, GIC>::Call : NULL,
				setter ? reinterpret_cast<void (tObjectBase::*)()>(setter) : NULL,
				setter ? &tBinderPropertySetter<CC, SIC, ST >::Call : NULL
				), context), attribute, is_members);
}
// non-static with calling info getter, const getter
template <typename CC, typename GIC, typename SIC, typename ST>
void BindProperty(CC * _class, const tString & name,
	void (GIC::*getter)(const tNativePropGetInfo &) const, void (SIC::*setter)(ST),
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	attribute.Set(tMemberAttribute::pcProperty);
	_class->RegisterNormalMember(name,
		tVariantBlock(
			tNativeBindProperty<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				getter ? reinterpret_cast<void (tObjectBase::*)()>(getter) : NULL,
				getter ? &tBinderPropertyGetter_Info<CC, GIC>::Call : NULL,
				setter ? reinterpret_cast<void (tObjectBase::*)()>(setter) : NULL,
				setter ? &tBinderPropertySetter<CC, SIC, ST >::Call : NULL
				), context), attribute, is_members);
}
// non-static with calling info getter, non-const getter, read-only
template <typename CC, typename GIC/*, typename SIC, typename ST*/>
void BindProperty(CC * _class, const tString & name,
	void (GIC::*getter)(const tNativePropGetInfo &)/*, void (SIC::*setter)(ST)*/,
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	BindProperty<CC, GIC, tObjectBase, const tVariant &>(
		_class, name, getter, NULL, attribute, context, is_members);
}
// non-static with calling info getter, const getter, read-only
template <typename CC, typename GIC/*, typename SIC, typename ST*/>
void BindProperty(CC * _class, const tString & name,
	void (GIC::*getter)(const tNativePropGetInfo &) const/*, void (SIC::*setter)(ST)*/,
	tMemberAttribute attribute = tMemberAttribute(),
	const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
{
	BindProperty<CC, GIC, tObjectBase, const tVariant &>(
		_class, name, getter, NULL, attribute, context, is_members);
}

//---------------------------------------------------------------------------
// 関数オブジェクトなど
// いまのところ BindFunction や BindProperty はどのクラスにも属していない。
// ネイティブ関数の定義が必要ないところに、かなり大きいテンプレートライブラリを
// 読み込みたくなく、BindFunction や BindProperty をそのほかの実装/定義と分離するためである。
//---------------------------------------------------------------------------
#include "risseNativeBinderTemplates.inc"
//---------------------------------------------------------------------------

/*
	TODO: 呼び出しの効率化
	RTTI が増えるかもしれないがのう
*/

}
#endif

