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

// この基底クラスを派生し、下位クラスで Operate をオーバーライドして各メソッド
// やプロパティを実装する方法もあるが、それだと各メソッドやプロパティごとに
// クラスが作成され、RTTIを浪費することになる (RTTIを作成しないpragmaticな
// 方法もあるだろうが)。ここでは、実際に呼び出す先を関数ポインタにし、
// クラスはいちいち作らないことにする。ちなみにTJS2では当初はクラスを
// それぞれのメソッドやプロパティごとに持っていたが、このような方法を
// とったことにより大幅に実行バイナリのサイズが減った。

//! @brief		tRisseNativePropertyBase::tGetter の引数
#define RISSE_NATIVEPROPERTY_GETTER_ARGS \
		tRisseVariant * result,               \
		tRisseOperateFlags flags,             \
		const tRisseVariant &This

//! @brief		tRisseNativePropertyBase::tSetter の引数
#define RISSE_NATIVEPROPERTY_SETTER_ARGS \
		const tRisseVariant & value,          \
		tRisseOperateFlags flags,             \
		const tRisseVariant &This

//! @brief		ネイティブプロパティ宣言の開始(コンテキスト指定)
//! @note		prop_name は一つの単語であること。RISSE_WS( ) などでの文字列は受け付けない。
#define RISSE_BEGIN_NATIVE_PROPERTY_CONTEXT(prop_name, prop_context) \
{ \
	const tRisseString & name = (prop_name); \
	const tRisseVariantBlock * context = (prop_context); \
	struct tNCP_##prop_name { \
		void (*getter)(RISSE_NATIVEPROPERTY_GETTER_ARGS); \
		void (*setter)(RISSE_NATIVEPROPERTY_SETTER_ARGS); \
		tNCP_##prop_name() { \
		getter = NULL; setter = NULL;


//! @brief		ネイティブプロパティ宣言の開始
//! @note		prop_name は一つの単語であること。RISSE_WS( ) などでの文字列は受け付けない。
#define RISSE_BEGIN_NATIVE_PROPERTY(prop_name) \
	RISSE_BEGIN_NATIVE_PROPERTY_CONTEXT(prop_name, tRisseVariant::GetDynamicContext())

//! @brief		ネイティブプロパティ宣言の終了
#define RISSE_END_NATIVE_PROPERTY \
		} \
	} static instance; \
	RegisterNormalMember(name, tRisseVariant(tRisseNativeProperty::New(instance.getter, instance.setter), context), \
		tRisseMemberAttribute(tRisseMemberAttribute::pcProperty)); \
}

//! @brief		ゲッター宣言の開始
#define RISSE_BEGINE_NATIVE_PROPERTY_GETTER \
	struct tNCP_GETTER {\
		static void Do(RISSE_NATIVEPROPERTY_GETTER_ARGS) {

//! @brief		ゲッター宣言の終了
#define RISSE_END_NATIVE_PROPERTY_GETTER \
		} \
	} static getter_instance; \
	getter = getter_instance.Do;

//! @brief		セッター宣言の開始
#define RISSE_BEGINE_NATIVE_PROPERTY_SETTER \
	struct tNCP_SETTER {\
		static void Do(RISSE_NATIVEPROPERTY_SETTER_ARGS) {

//! @brief		セッター宣言の終了
#define RISSE_END_NATIVE_PROPERTY_SETTER \
		} \
	} static setter_instance; \
	setter = setter_instance.Do;



//---------------------------------------------------------------------------
//! @brief		RisseネイティブプロパティのGetterの実装
//---------------------------------------------------------------------------
class tRisseNativePropertyGetter : public tRisseObjectInterface
{
public:
	//! @brief		Risseプロパティが読み込まれる際に呼ばれるメソッドのtypedef
	//!				オーバライドしなかった場合は読み込み禁止のプロパティになる)
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags	オペレーションフラグ
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	typedef void (*tGetter)(RISSE_NATIVEPROPERTY_GETTER_ARGS);

private:
	//! @brief		Risseプロパティが読み込まれる際に呼ばれるメソッド
	tGetter Getter;

public:
	//! @brief		コンストラクタ
	//! @param		getter		Risseプロパティが読み込まれる際に呼ばれるメソッド
	tRisseNativePropertyGetter(tGetter getter) { Getter = getter; }

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		RisseネイティブプロパティのSetterの実装
//---------------------------------------------------------------------------
class tRisseNativePropertySetter : public tRisseObjectInterface
{
public:
	//! @brief		Risseプロパティが書き込まれる際に呼ばれるメソッドのtypedef
	//!				オーバーライドしなかった場合は書き込み禁止のプロパティになる)
	//! @param		value	書き込む値
	//! @param		flags	オペレーションフラグ
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	typedef void (*tSetter)(RISSE_NATIVEPROPERTY_SETTER_ARGS);

private:
	//! @brief		Risseプロパティが書き込まれる際に呼ばれるメソッド
	tSetter Setter;

public:
	//! @brief		コンストラクタ
	//! @param		setter		Risseプロパティが書き込まれる際に呼ばれるメソッド
	tRisseNativePropertySetter(tSetter setter) { Setter = setter; }

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


class tRissePropertyInstance;
//---------------------------------------------------------------------------
//! @brief		Risseネイティブプロパティ
//---------------------------------------------------------------------------
class tRisseNativeProperty : public tRisseObjectInterface
{
	//! @param		親クラスのtypedef
	typedef tRisseObjectInterface inherited;

	tRisseNativePropertyGetter::tGetter Getter; //!< Risseプロパティが読み込まれる際に呼ばれるメソッド
	tRisseNativePropertySetter::tSetter Setter; //!< Risseプロパティが書き込まれる際に呼ばれるメソッド

protected:
	//! @brief		コンストラクタ
	//! @param		getter		Risseプロパティが読み込まれる際に呼ばれるメソッド
	//! @param		setter		Risseプロパティが書き込まれる際に呼ばれるメソッド
	tRisseNativeProperty(tRisseNativePropertyGetter::tGetter getter, tRisseNativePropertySetter::tSetter setter = NULL);

public:
	//! @brief		新しいプロパティインスタンスを生成して返す(コンストラクタではなくてこちらを呼ぶこと)
	//! @param		getter		Risseプロパティが読み込まれる際に呼ばれるメソッド
	//! @param		setter		Risseプロパティが書き込まれる際に呼ばれるメソッド
	static tRisseObjectInterface * New(tRisseNativePropertyGetter::tGetter getter, tRisseNativePropertySetter::tSetter setter = NULL);

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------

}
#endif

