/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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

	ネイティブ実装の関数あるいは tCodeBlock クラスは実際に tObjectInterface
	を実装しており、関数として動作できるのだが、そのままでは Function クラスのインスタンス
	としての振る舞いをしない。そこで、この tFunctionClass および tFunctionInstance
	でそれらの「裸の」関数インスタンスをラップすることとなる。
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(40031,39054,14544,17698,39309,64830,49090,60837);


//---------------------------------------------------------------------------
tFunctionInstance::tFunctionInstance()
{
	Body.Nullize();
	Synchronized = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tFunctionInstance::tRetValue tFunctionInstance::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するメソッド呼び出しか？
		{
			// synchronized メソッドの場合はロックを行う
			tVariant::tSynchronizer sync(Synchronized ? This : tVariant::GetNullObject());

			// Body を呼び出す
			Body.FuncCall_Object(result, tString::GetEmptyString(), flags, args, This);
			return rvNoError;
		}
	}

	// そのほかの場合は親クラスのメソッドを呼ぶ
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFunctionInstance::construct()
{
	// 何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFunctionInstance::initialize(const tNativeCallInfo & info)
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
RISSE_IMPL_CLASS_BEGIN(tFunctionClass, ss_Function, engine->ObjectClass)
	// 注: この時点では Function クラスの ovulate は inherited::RegisterMembers で
	// 登録された、親クラスの ovulate となっている。
	// この状態で function クラスのメソッドを登録しようとしても(間違ったovulateが
	// 呼ばれるため)うまくいかないので、
	// たとえ仮であろうと ovulate メソッドを登録する。
	// いったん ScriptEngine インスタンスの FunctionClass を NULL に設定すると
	// BindFunction は仮のメソッドインスタンスを用いるようになる。
	tFunctionClass * f_save = GetRTTI()->GetScriptEngine()->FunctionClass;
	GetRTTI()->GetScriptEngine()->FunctionClass = NULL;
	BindFunction(this, ss_ovulate, &tFunctionClass::ovulate);
	GetRTTI()->GetScriptEngine()->FunctionClass = f_save;

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。
	RISSE_BIND_CONSTRUCTORS
	BindProperty(this, ss_synchronized,
		&tFunctionInstance::get_synchronized, &tFunctionInstance::set_synchronized);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


} /* namespace Risse */

