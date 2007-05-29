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
#include "prec.h"

#include "risseNativeBinder.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(50153,12161,23237,20278,22942,17690,37012,37765);


//---------------------------------------------------------------------------
tRisseNativeBindFunction::tRetValue tRisseNativeBindFunction::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// このオブジェクトに対する関数呼び出しか？
	if(code == ocFuncCall && name.IsEmpty())
	{
		// このオブジェクトに対する関数呼び出しなので Callee を呼ぶ
		tRisseNativeBindFunctionCallingInfo info(GetRTTI()->GetScriptEngine(), result, flags, args, This);
		Callee(Class, TargetFunction, info);
		return rvNoError;
	}

	// それ以外の機能はこのインターフェースにはない
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectInterface * tRisseNativeBindFunction::New(tRisseScriptEngine * engine,
	tRisseClassBase * class_, void (tRisseObjectBase::*target)(), tCallee callee)
{
	// tRisseFunctionClass がまだ登録されていない場合は仮のメソッドを
	// 作成して登録する (のちに正式なメソッドオブジェクトに置き換えられる)
	if(engine->FunctionClass)
	{
		tRisseVariant v = tRisseVariant(engine->FunctionClass).New(
				0, tRisseMethodArgument::New(new tRisseNativeBindFunction(engine, class_, target, callee)));

		RISSE_ASSERT(v.GetType() == tRisseVariant::vtObject);
		RISSE_ASSERT(dynamic_cast<tRisseFunctionInstance*>(v.GetObjectInterface()) != NULL);

		return v.GetObjectInterface();
	}
	else
	{
		// 仮実装
		return (tRisseFunctionInstance *)(new tRisseNativeBindFunction(engine, class_, target, callee));
	}
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tRisseNativeBindStaticFunction::tRetValue tRisseNativeBindStaticFunction::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// このオブジェクトに対する関数呼び出しか？
	if(code == ocFuncCall && name.IsEmpty())
	{
		// このオブジェクトに対する関数呼び出しなので Callee を呼ぶ
		tRisseNativeBindFunctionCallingInfo info(GetRTTI()->GetScriptEngine(), result, flags, args, This);
		Callee(Class, TargetFunction, info);
		return rvNoError;
	}

	// それ以外の機能はこのインターフェースにはない
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectInterface * tRisseNativeBindStaticFunction::New(tRisseScriptEngine * engine,
	tRisseClassBase * class_, void (*target)(), tCallee callee)
{
	// tRisseFunctionClass がまだ登録されていない場合は仮のメソッドを
	// 作成して登録する (のちに正式なメソッドオブジェクトに置き換えられる)
	if(engine->FunctionClass)
	{
		tRisseVariant v = tRisseVariant(engine->FunctionClass).New(
				0, tRisseMethodArgument::New(new tRisseNativeBindStaticFunction(engine, class_, target, callee)));

		RISSE_ASSERT(v.GetType() == tRisseVariant::vtObject);
		RISSE_ASSERT(dynamic_cast<tRisseFunctionInstance*>(v.GetObjectInterface()) != NULL);

		return v.GetObjectInterface();
	}
	else
	{
		// 仮実装
		return (tRisseFunctionInstance *)(new tRisseNativeBindStaticFunction(engine, class_, target, callee));
	}
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tRisseNativeBindPropertyGetter::tRetValue tRisseNativeBindPropertyGetter::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall)
		{
			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(!Getter) return rvPropertyCannotBeRead;
			tRisseNativeBindPropertyGetterCallingInfo info(GetRTTI()->GetScriptEngine(), result, flags, This);
			Getter(Class, TargetFunction, info);
			return rvNoError;
		}
	}

	// そのほかの場合はメンバが無い物として扱う
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseNativeBindPropertySetter::tRetValue tRisseNativeBindPropertySetter::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall)
		{
			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			args.ExpectArgumentCount(1);
			if(!Setter) return rvPropertyCannotBeWritten;
			tRisseNativeBindPropertySetterCallingInfo info(GetRTTI()->GetScriptEngine(), args[0], flags, This);
			Setter(Class, TargetFunction, info);
			return rvNoError;
		}
	}

	// そのほかの場合はメンバが無い物として扱う
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseNativeBindProperty::tRetValue tRisseNativeBindProperty::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するプロパティ読み込みか？
		{
			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(!Getter) return rvPropertyCannotBeRead;
			return Getter->Operate(ocFuncCall, result, tRisseString::GetEmptyString(), flags, args, This);
		}
		else if(code == ocDSet) // このオブジェクトに対するプロパティ書き込みか？
		{
			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			args.ExpectArgumentCount(1);
			if(!Setter) return rvPropertyCannotBeWritten;
			return Setter->Operate(ocFuncCall, result, tRisseString::GetEmptyString(), flags, args, This);
		}
	}

	// そのほかの場合はメンバが無い物として扱う
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectInterface * tRisseNativeBindProperty::New(tRisseScriptEngine * engine,
		tRisseClassBase * Class,
		void (tRisseObjectBase::*gettertarget)(), tRisseNativeBindPropertyGetter::tGetter getter,
		void (tRisseObjectBase::*settertarget)(), tRisseNativeBindPropertySetter::tSetter setter)
{
	// tRissePropertyClass がまだ登録されていない場合は仮のプロパティオブジェクトを
	// 作成して登録する (のちに正式なプロパティオブジェクトに置き換えられる)
	if(engine->PropertyClass)
	{
		// 正式なプロパティオブジェクトを登録する
		tRisseVariant v = tRisseVariant(engine->PropertyClass).New(
				0, tRisseMethodArgument::New(
					getter ?
						tRisseVariant(new tRisseNativeBindPropertyGetter(
												engine, Class, gettertarget, getter)):
						tRisseVariant::GetNullObject(),
					setter ?
						tRisseVariant(new tRisseNativeBindPropertySetter(
												engine, Class, settertarget, setter)):
						tRisseVariant::GetNullObject()
						));

		RISSE_ASSERT(v.GetType() == tRisseVariant::vtObject);
		RISSE_ASSERT(dynamic_cast<tRissePropertyInstance*>(v.GetObjectInterface()) != NULL);

		return v.GetObjectInterface();
	}
	else
	{
		// 仮実装
		return (tRissePropertyInstance *)(new tRisseNativeBindProperty(engine,
			Class, gettertarget, getter, settertarget, setter));
	}
}
//---------------------------------------------------------------------------




} // namespace Risse
