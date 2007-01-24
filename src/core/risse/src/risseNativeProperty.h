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
		const tRisseVariant &This,            \
		const tRisseStackFrameContext &stack

//! @brief		tRisseNativePropertyBase::tSetter の引数
#define RISSE_NATIVEPROPERTY_SETTER_ARGS \
		const tRisseVariant & value,          \
		tRisseOperateFlags flags,             \
		const tRisseVariant &This,            \
		const tRisseStackFrameContext &stack


//---------------------------------------------------------------------------
//! @brief		Risseネイティブプロパティの基底クラス
//---------------------------------------------------------------------------
class tRisseNativePropertyBase : public tRisseObjectBase
{
	//! @param		親クラスのtypedef
	typedef tRisseObjectBase inherited;

protected:
	//! @brief		Risseプロパティが読み込まれる際に呼ばれるメソッドのtypedef
	//!				オーバライドしなかった場合は読み込み禁止のプロパティになる)
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		flags	オペレーションフラグ
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	typedef void (*tGetter)(RISSE_NATIVEPROPERTY_GETTER_ARGS);

	//! @brief		Risseプロパティが書き込まれる際に呼ばれるメソッドのtypedef
	//!				オーバーライドしなかった場合は書き込み禁止のプロパティになる)
	//! @param		value	書き込む値
	//! @param		flags	オペレーションフラグ
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	typedef void (*tSetter)(RISSE_NATIVEPROPERTY_SETTER_ARGS);

	//! @brief		Risseプロパティが読み込まれる際に呼ばれるメソッド
	tGetter Getter;

	//! @brief		Risseプロパティが書き込まれる際に呼ばれるメソッド
	tSetter Setter;

public:
	//! @brief		コンストラクタ
	//! @param		getter		Risseプロパティが読み込まれる際に呼ばれるメソッド
	//! @param		setter		Risseプロパティが書き込まれる際に呼ばれるメソッド
	tRisseNativePropertyBase(tGetter getter, tSetter setter = NULL);

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------

}
#endif

