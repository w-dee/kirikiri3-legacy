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
#include "risseFunctionClass.h"
#include "rissePropertyClass.h"
#include "risseStaticStrings.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(50153,12161,23237,20278,22942,17690,37012,37765);



//---------------------------------------------------------------------------
void tRisseNativeCallInfo::InitializeSuperClass(const tRisseMethodArgument & args) const
{
	Class->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
template <typename TT>
tRisseObjectInterface::tRetValue tRisseNativeBindFunction<TT>::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// このオブジェクトに対する関数呼び出しか？
	if(code == ocFuncCall && name.IsEmpty())
	{
		// このオブジェクトに対する関数呼び出しなので Callee を呼ぶ
		tRisseNativeCallInfo info(
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
tRisseObjectInterface * tRisseNativeBindFunction<TT>::New(tRisseScriptEngine * engine,
	tRisseClassBase * class_, TT target, tCallee callee)
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
template class tRisseNativeBindFunction<void (tRisseObjectBase::*)()>; // メンバ関数用
template class tRisseNativeBindFunction<void (*)()>; // staticメンバ関数用
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
template <typename TT>
tRisseObjectInterface::tRetValue tRisseNativeBindPropertyGetter<TT>::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall)
		{
			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(!Getter) return rvPropertyCannotBeRead;
			tRisseNativePropGetInfo info(
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
tRisseObjectInterface::tRetValue tRisseNativeBindPropertySetter<TT>::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall)
		{
			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			args.ExpectArgumentCount(1);
			if(!Setter) return rvPropertyCannotBeWritten;
			tRisseNativePropSetInfo info(
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
tRisseObjectInterface::tRetValue tRisseNativeBindProperty<TT>::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
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
template <typename TT>
tRisseObjectInterface * tRisseNativeBindProperty<TT>::New(tRisseScriptEngine * engine,
		tRisseClassBase * Class,
		TT gettertarget, typename tRisseNativeBindPropertyGetter<TT>::tGetter getter,
		TT settertarget, typename tRisseNativeBindPropertySetter<TT>::tSetter setter)
{
	// tRissePropertyClass がまだ登録されていない場合は仮のプロパティオブジェクトを
	// 作成して登録する (のちに正式なプロパティオブジェクトに置き換えられる)
	if(engine->PropertyClass)
	{
		// 正式なプロパティオブジェクトを登録する
		tRisseVariant v = tRisseVariant(engine->PropertyClass).New(
				0, tRisseMethodArgument::New(
					getter ?
						tRisseVariant(new tRisseNativeBindPropertyGetter<TT>(
												engine, Class, gettertarget, getter)):
						tRisseVariant::GetNullObject(),
					setter ?
						tRisseVariant(new tRisseNativeBindPropertySetter<TT>(
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
		return (tRissePropertyInstance *)(new tRisseNativeBindProperty<TT>(engine,
			Class, gettertarget, getter, settertarget, setter));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template class tRisseNativeBindPropertyGetter<void (tRisseObjectBase::*)()>; // メンバ関数用
template class tRisseNativeBindPropertyGetter<void (*)()>; // staticメンバ関数用
template class tRisseNativeBindPropertySetter<void (tRisseObjectBase::*)()>; // メンバ関数用
template class tRisseNativeBindPropertySetter<void (*)()>; // staticメンバ関数用
template class tRisseNativeBindProperty<void (tRisseObjectBase::*)()>; // メンバ関数用
template class tRisseNativeBindProperty<void (*)()>; // staticメンバ関数用
//---------------------------------------------------------------------------


} // namespace Risse
