/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Function" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseFunctionClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"Function" クラスの実装

	Function クラスは引数を一つ取り、関数の内容(文字列)あるいは関数インスタンスを渡す。

	ネイティブ実装の関数あるいは tRisseCodeBlock クラスは実際に tRisseObjectInterface
	を実装しており、関数として動作できるのだが、そのままでは Function クラスのインスタンス
	としての振る舞いをしない。そこで、この tRisseFunctionClass および tRisseFunctionInstance
	でそれらの「裸の」関数インスタンスをラップすることとなる。
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(40031,39054,14544,17698,39309,64830,49090,60837);


//---------------------------------------------------------------------------
tRisseFunctionInstance::tRisseFunctionInstance()
{
	Body.Nullize();
	Synchronized = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseFunctionInstance::tRetValue tRisseFunctionInstance::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するメソッド呼び出しか？
		{
			// synchronized メソッドの場合はロックを行う
			tRisseVariant::tSynchronizer sync(Synchronized ? This : tRisseVariant::GetNullObject());

			// Body を呼び出す
			Body.FuncCall_Object(result, tRisseString::GetEmptyString(), flags, args, This);
			return rvNoError;
		}
	}

	// そのほかの場合は親クラスのメソッドを呼ぶ
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseFunctionInstance::construct()
{
	// 何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseFunctionInstance::initialize(const tRisseNativeCallInfo & info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// 引数 = {body}
	// TODO: 文字列が渡された場合は内容をコンパイルして関数として動作するようにする
	if(info.args.HasArgument(0)) SetBody(info.args[0]);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseFunctionClass::tRisseFunctionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(ss_Function, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseFunctionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// 注: この時点では Function クラスの ovulate は inherited::RegisterMembers で
	// 登録された、親クラスの ovulate となっている。
	// この状態で function クラスのメソッドを登録しようとしても(間違ったovulateが
	// 呼ばれるため)うまくいかないので、
	// たとえ仮であろうと ovulate メソッドを登録する。
	// いったん ScriptEngine インスタンスの FunctionClass を NULL に設定すると
	// RisseBindFunction は仮のメソッドインスタンスを用いるようになる。
	tRisseFunctionClass * f_save = GetRTTI()->GetScriptEngine()->FunctionClass;
	GetRTTI()->GetScriptEngine()->FunctionClass = NULL;
	RisseBindFunction(this, ss_ovulate, &tRisseFunctionClass::ovulate);
	GetRTTI()->GetScriptEngine()->FunctionClass = f_save;

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_ovulate, &tRisseFunctionClass::ovulate);
	RisseBindFunction(this, ss_construct, &tRisseFunctionInstance::construct);
	RisseBindFunction(this, ss_initialize, &tRisseFunctionInstance::initialize);
	RisseBindProperty(this, ss_synchronized,
		&tRisseFunctionInstance::get_synchronized, &tRisseFunctionInstance::set_synchronized);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseFunctionClass::ovulate()
{
	return tRisseVariant(new tRisseFunctionInstance());
}
//---------------------------------------------------------------------------


} /* namespace Risse */

