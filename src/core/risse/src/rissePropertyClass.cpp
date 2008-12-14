/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Property" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "rissePropertyClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"Property" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(26307,13927,24599,16690,19605,20552,47176,34884);

//---------------------------------------------------------------------------
tPropertyInstance::tPropertyInstance()
{
	Getter.Nullize();
	Setter.Nullize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tPropertyInstance::tRetValue tPropertyInstance::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するプロパティ読み込みか？
		{
			// synchronized メソッドの場合はロックを行う
			tVariant::tSynchronizer sync(Synchronized ? This : tVariant::GetVoidObject());

			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(Getter.IsNull()) return rvPropertyCannotBeRead;
			Getter.FuncCall_Object(result, tString::GetEmptyString(),
							flags, tMethodArgument::Empty(), This);
			return rvNoError;
		}
		else if(code == ocDSet) // このオブジェクトに対するプロパティ書き込みか？
		{
			// synchronized メソッドの場合はロックを行う
			tVariant::tSynchronizer sync(Synchronized ? This : tVariant::GetVoidObject());

			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			args.ExpectArgumentCount(1);
			if(Setter.IsNull()) return rvPropertyCannotBeWritten;
			Setter.FuncCall_Object(NULL, tString::GetEmptyString(),
							flags, args, This);
			return rvNoError;
		}
	}

	// そのほかの場合は親クラスのメソッドを呼ぶ
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPropertyInstance::construct()
{
	// なにもしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPropertyInstance::initialize(const tNativeCallInfo & info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// 引数 = {getter, setter}
	if(info.args.HasArgument(0)) SetGetter(info.args[0]);
	if(info.args.HasArgument(1)) SetSetter(info.args[1]);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tPropertyClass, ss_Property, engine->ObjectClass)
	// 注: この時点では Property クラスの ovulate は inherited::RegisterMembers で
	// 登録された、親クラスの ovulate となっている。
	// この状態で Property クラスのメソッドを登録しようとしても(間違ったovulateが
	// 呼ばれるため)うまくいかないので、
	// たとえ仮であろうと ovulate メソッドを登録する。
	// いったん ScriptEngine インスタンスの PropertyClass を NULL に設定すると
	// BindProperty は仮のメソッドインスタンスを用いるようになる。
	tPropertyClass * f_save = GetRTTI()->GetScriptEngine()->PropertyClass;
	GetRTTI()->GetScriptEngine()->PropertyClass = NULL;
	BindFunction(this, ss_ovulate, &tPropertyClass::ovulate);
	GetRTTI()->GetScriptEngine()->PropertyClass = f_save;

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RISSE_BIND_CONSTRUCTORS
	BindProperty(this, ss_getter, &tPropertyInstance::GetGetter);
	BindProperty(this, ss_setter, &tPropertyInstance::GetSetter);
	BindProperty(this, ss_synchronized,
		&tPropertyInstance::get_synchronized, &tPropertyInstance::set_synchronized);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------

} /* namespace Risse */

