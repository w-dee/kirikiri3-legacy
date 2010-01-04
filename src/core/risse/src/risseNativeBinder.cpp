//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ネイティブ関数/プロパティを簡単に記述するためのC++テンプレートサポート
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseNativeBinder.h"
#include "risseScriptEngine.h"
#include "risseFunctionClass.h"
#include "rissePropertyClass.h"
#include "risseStaticStrings.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(50153,12161,23237,20278,22942,17690,37012,37765);



//---------------------------------------------------------------------------
void tNativeCallInfo::InitializeSuperClass(const tMethodArgument & args) const
{
	Class->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
template <typename TT>
tObjectInterface::tRetValue tNativeBindFunction<TT>::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// このオブジェクトに対する関数呼び出しか？
	if(code == ocFuncCall && name.IsEmpty())
	{
		// このオブジェクトに対する関数呼び出しなので Callee を呼ぶ
		tNativeCallInfo info(
			GetRTTI()->GetScriptEngine(), result, flags, args, This, Class);
		Callee(TargetFunction, info);
		return rvNoError;
	}

	// それ以外の機能はこのインターフェースにはない
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template <typename TT>
tObjectInterface * tNativeBindFunction<TT>::New(tScriptEngine * engine,
	tClassBase * class_, TT target, tCallee callee)
{
	// tFunctionClass がまだ登録されていない場合は仮のメソッドを
	// 作成して登録する (のちに正式なメソッドオブジェクトに置き換えられる)
	if(engine->FunctionClass)
	{
		tVariant v = tVariant(engine->FunctionClass).New(
				0, tMethodArgument::New(new tNativeBindFunction(engine, class_, target, callee)));

		RISSE_ASSERT(v.GetType() == tVariant::vtObject);
		RISSE_ASSERT(dynamic_cast<tFunctionInstance*>(v.GetObjectInterface()) != NULL);

		return v.GetObjectInterface();
	}
	else
	{
		// 仮実装
		return (tFunctionInstance *)(new tNativeBindFunction(engine, class_, target, callee));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template class tNativeBindFunction<void (tObjectBase::*)()>; // メンバ関数用
template class tNativeBindFunction<void (*)()>; // staticメンバ関数用
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
template <typename TT>
tObjectInterface::tRetValue tNativeBindPropertyGetter<TT>::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall)
		{
			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(!Getter) return rvPropertyCannotBeRead;
			tNativePropGetInfo info(
				GetRTTI()->GetScriptEngine(), result, flags, This, Class);
			Getter(TargetFunction, info);
			return rvNoError;
		}
	}

	// そのほかの場合はメンバが無い物として扱う
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template <typename TT>
tObjectInterface::tRetValue tNativeBindPropertySetter<TT>::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall)
		{
			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			args.ExpectArgumentCount(1);
			if(!Setter) return rvPropertyCannotBeWritten;
			tNativePropSetInfo info(
				GetRTTI()->GetScriptEngine(), args[0], flags, This, Class);
			Setter(TargetFunction, info);
			return rvNoError;
		}
	}

	// そのほかの場合はメンバが無い物として扱う
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
template <typename TT>
tObjectInterface::tRetValue tNativeBindProperty<TT>::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するプロパティ読み込みか？
		{
			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(!Getter) return rvPropertyCannotBeRead;
			return Getter->Operate(ocFuncCall, result, tString::GetEmptyString(), flags, args, This);
		}
		else if(code == ocDSet) // このオブジェクトに対するプロパティ書き込みか？
		{
			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			args.ExpectArgumentCount(1);
			if(!Setter) return rvPropertyCannotBeWritten;
			return Setter->Operate(ocFuncCall, result, tString::GetEmptyString(), flags, args, This);
		}
	}

	// そのほかの場合はメンバが無い物として扱う
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template <typename TT>
tObjectInterface * tNativeBindProperty<TT>::New(tScriptEngine * engine,
		tClassBase * Class,
		TT gettertarget, typename tNativeBindPropertyGetter<TT>::tGetter getter,
		TT settertarget, typename tNativeBindPropertySetter<TT>::tSetter setter)
{
	// tPropertyClass がまだ登録されていない場合は仮のプロパティオブジェクトを
	// 作成して登録する (のちに正式なプロパティオブジェクトに置き換えられる)
	if(engine->PropertyClass)
	{
		// 正式なプロパティオブジェクトを登録する
		tVariant v = tVariant(engine->PropertyClass).New(
				0, tMethodArgument::New(
					getter ?
						tVariant(new tNativeBindPropertyGetter<TT>(
												engine, Class, gettertarget, getter)):
						tVariant::GetNullObject(),
					setter ?
						tVariant(new tNativeBindPropertySetter<TT>(
												engine, Class, settertarget, setter)):
						tVariant::GetNullObject()
						));

		RISSE_ASSERT(v.GetType() == tVariant::vtObject);
		RISSE_ASSERT(dynamic_cast<tPropertyInstance*>(v.GetObjectInterface()) != NULL);

		return v.GetObjectInterface();
	}
	else
	{
		// 仮実装
		return (tPropertyInstance *)(new tNativeBindProperty<TT>(engine,
			Class, gettertarget, getter, settertarget, setter));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template class tNativeBindPropertyGetter<void (tObjectBase::*)()>; // メンバ関数用
template class tNativeBindPropertyGetter<void (*)()>; // staticメンバ関数用
template class tNativeBindPropertySetter<void (tObjectBase::*)()>; // メンバ関数用
template class tNativeBindPropertySetter<void (*)()>; // staticメンバ関数用
template class tNativeBindProperty<void (tObjectBase::*)()>; // メンバ関数用
template class tNativeBindProperty<void (*)()>; // staticメンバ関数用
//---------------------------------------------------------------------------


} // namespace Risse
