/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
tRissePropertyInstance::tRissePropertyInstance()
{
	Getter.Nullize();
	Setter.Nullize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRissePropertyInstance::tRetValue tRissePropertyInstance::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するプロパティ読み込みか？
		{
			// synchronized メソッドの場合はロックを行う
			tRisseVariant::tSynchronizer sync(Synchronized ? This : tRisseVariant::GetVoidObject());

			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(Getter.IsNull()) return rvPropertyCannotBeRead;
			Getter.FuncCall_Object(result, tRisseString::GetEmptyString(),
							flags, tRisseMethodArgument::Empty(), This);
			return rvNoError;
		}
		else if(code == ocDSet) // このオブジェクトに対するプロパティ書き込みか？
		{
			// synchronized メソッドの場合はロックを行う
			tRisseVariant::tSynchronizer sync(Synchronized ? This : tRisseVariant::GetVoidObject());

			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			args.ExpectArgumentCount(1);
			if(Setter.IsNull()) return rvPropertyCannotBeWritten;
			Setter.FuncCall_Object(NULL, tRisseString::GetEmptyString(),
							flags, args, This);
			return rvNoError;
		}
	}

	// そのほかの場合は親クラスのメソッドを呼ぶ
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRissePropertyInstance::construct()
{
	// なにもしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRissePropertyInstance::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// 引数 = {getter, setter}
	if(info.args.HasArgument(0)) SetGetter(info.args[0]);
	if(info.args.HasArgument(1)) SetSetter(info.args[1]);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tRissePropertyClass::tRissePropertyClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRissePropertyClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRissePropertyInstance::construct);
	RisseBindFunction(this, ss_initialize, &tRissePropertyInstance::initialize);
	RisseBindProperty(this, ss_getter, &tRissePropertyInstance::GetGetter);
	RisseBindProperty(this, ss_setter, &tRissePropertyInstance::GetSetter);
	RisseBindProperty(this, ss_synchronized,
		&tRissePropertyInstance::get_synchronized, &tRissePropertyInstance::set_synchronized);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRissePropertyClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRissePropertyInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

