//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse 例外クラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseVariant.h"
#include "risseExceptionClass.h"
#include "risseObjectClass.h"
#include "risseClass.h"
#include "risseStaticStrings.h"
#include "risseArrayClass.h"
#include "risseClassClass.h"
#include "risseScriptBlockClass.h"
#include "risseScriptEngine.h"
#include "risseStringTemplate.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(64113,30630,41963,17808,15295,58919,39993,4429);

/*
 例外クラス階層

 Throwable
   Error
     AssertionError
   BlockExitException
   Exception
     InsufficientResourceException
     IOException
       CharConversionException
     RuntimeException
       CompileException
       ImportException
       ClassDefinitionException
       InstantiationException
       BadContextException
       UnsupportedOperationException
       ArithmeticException
       ArgumentException
         IllegalArgumentException
           NullObjectException
           IllegalArgumentClassException
         BadArgumentCountException
       MemberAccessException
         NoSuchMemberException
         IllegalMemberAccessException
       CoroutineException (coroutineパッケージ)
       IllegalStateException
         AlreadyDisposedException
*/




//---------------------------------------------------------------------------
tTemporaryException::tTemporaryException(const tString & package, const tString & classname)
{
	PackageName = package;
	ExceptionClassName = classname;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTemporaryException::tTemporaryException(const tString & package, const tString & classname,
		const tVariant & arg1)
{
	PackageName = package;
	ExceptionClassName = classname;
	Arguments.push_back(arg1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTemporaryException::tTemporaryException(const tString & package, const tString & classname,
		const tVariant & arg1, const tVariant & arg2)
{
	PackageName = package;
	ExceptionClassName = classname;
	Arguments.push_back(arg1);
	Arguments.push_back(arg2);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTemporaryException::tTemporaryException(const tString & package, const tString & classname,
		const tVariant & arg1, const tVariant & arg2, const tVariant & arg3)
{
	PackageName = package;
	ExceptionClassName = classname;
	Arguments.push_back(arg1);
	Arguments.push_back(arg2);
	Arguments.push_back(arg3);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant * tTemporaryException::Convert(tScriptEngine * engine) const
{
	// パッケージグローバルを取得
	tVariant package_global = engine->GetPackageGlobal(PackageName);

	// 例外クラスを取得する
	tVariant cls;
	tObjectInterface::tRetValue ret =
		const_cast<tVariant&>(package_global).Operate(engine, ocDGet, &cls, ExceptionClassName);
	if(ret != tObjectInterface::rvNoError)
	{
		// ExceptionClassNameを取得できなかった。
		// この場合は ss_RuntimeException を取得する
		cls = engine->GetRissePackageGlobal().GetPropertyDirect_Object(ss_RuntimeException);
	}

	// 引数を用意する
	tMethodArgument & new_args = tMethodArgument::Allocate(Arguments.size());

	for(risse_size i = 0; i < Arguments.size(); i++)
		new_args.SetArgument(i, &Arguments[i]);

	// New を呼び出し、それを返す
	return new tVariant(cls.New(0, new_args));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTemporaryException::Dump() const
{
	fflush(stderr);
	fflush(stdout);

	FPrint(stderr, RISSE_WS("tTemporaryException: type "));
	FPrint(stderr, PackageName.c_str());
	FPrint(stderr, RISSE_WS("."));
	FPrint(stderr, ExceptionClassName.c_str());
	FPrint(stderr, RISSE_WS("\n"));

	fflush(stderr);
	fflush(stdout);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tExitTryExceptionClass::tExitTryExceptionClass(tScriptEngine * engine,
	const void * id, risse_uint32 targ_idx, const tVariant * value) :
		tObjectInterface(new tRTTI(engine))
{
	Identifier = id;
	BranchTargetIndex = targ_idx;
	Value = value ? new tVariant(*value) : NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectInterface::tRetValue tExitTryExceptionClass::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// 暫定実装
	if(code == ocDGet && name == RISSE_WS("getExitTryRecord"))
	{
		// tExitTryExceptionClass 型を返す
		if(result)
		{
			*result = static_cast<tObjectInterface*>
				(static_cast<tExitTryExceptionClass*>(this));
		}
	}
	return rvNoError;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void tSourcePointInstance::construct()
{
	// scriptBlock メンバを追加 (デフォルトではnull)
	RegisterNormalMember(ss_scriptBlock, tVariant::GetNullObject());

	// position メンバを追加 (デフォルトでは-1)
	RegisterNormalMember(ss_position, tVariant((risse_int64)-1));

	// function メンバを追加 (デフォルトではnull)
	RegisterNormalMember(ss_function, tVariant::GetNullObject());

	/*
		ほかにメンバを追加するかも
	*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSourcePointInstance::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数は空)
	info.InitializeSuperClass();

	// 引数 = スクリプトブロック, 行, メソッド
	if(info.args.HasArgument(0))
		info.This.SetPropertyDirect_Object(ss_scriptBlock,	0, info.args[0], info.This);
	if(info.args.HasArgument(1))
		info.This.SetPropertyDirect_Object(ss_position,		0, info.args[1], info.This);
	if(info.args.HasArgument(2))
		info.This.SetPropertyDirect_Object(ss_function,		0, info.args[2], info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSourcePointInstance::toString()
{
	// ファイル名:行番号: in 関数名 を生成して返す

	tVariant sb, position, function;
	sb =       GetPropertyDirect(ss_scriptBlock);
	position = GetPropertyDirect(ss_position);
	function = GetPropertyDirect(ss_function);

	tString ret;
	tString fn = sb.GetPropertyDirect(GetRTTI()->GetScriptEngine(), ss_name);
	if(!fn.IsEmpty())
		ret = fn + RISSE_WC(':');
	else
		ret = RISSE_WS_TR("<unknown>:");

	if(position != tVariant((risse_int64)-1))
	{
		tVariant line = sb.Invoke(GetRTTI()->GetScriptEngine(), ss_positionToLine, position);
		ret += (line + tVariant((risse_int64)1)).operator tString();
	}
	else
		ret += RISSE_WS_TR("<unknown>");

	if(!function.IsNull())
		ret += tString(RISSE_WS_TR(": in ")) + function.operator tString();

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tSourcePointClass, ss_SourcePoint, engine->ObjectClass)
	BindFunction(this, ss_ovulate,  &tSourcePointClass::ovulate);
	BindFunction(this, ss_construct,  &tSourcePointInstance::construct);
	BindFunction(this, ss_initialize,  &tSourcePointInstance::initialize);
	BindFunction(this, mnString,  &tSourcePointInstance::toString);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tThrowableClass, ss_Throwable, engine->ObjectClass)
	BindFunction(this, ss_construct, &tThrowableClass::construct);
	BindFunction(this, ss_initialize, &tThrowableClass::initialize);
	BindFunction(this, mnString, &tThrowableClass::toString);
	BindFunction(this, ss_addTrace, &tThrowableClass::addTrace);
	BindFunction(this, ss_toException, &tThrowableClass::toException);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tThrowableClass::construct(const tNativeCallInfo & info)
{
	// message メンバを追加 (デフォルトでは空文字列)
	info.This.SetPropertyDirect_Object(ss_message, tOperateFlags::ofMemberEnsure,
		tVariant(tString::GetEmptyString()), info.This);

	// trace メンバを追加 (デフォルトでは空配列)
	info.This.SetPropertyDirect_Object(ss_trace, tOperateFlags::ofMemberEnsure,
		tVariant(tVariant(info.engine->ArrayClass).New()), info.This);

	// cause メンバを追加 (デフォルトではnull)
	info.This.SetPropertyDirect_Object(ss_cause, tOperateFlags::ofMemberEnsure,
		tVariant::GetNullObject(), info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tThrowableClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数は空)
	info.InitializeSuperClass();

	// 引数 = メッセージ
	if(info.args.HasArgument(0))
		info.This.SetPropertyDirect_Object(ss_message, 0, info.args[0], info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tThrowableClass::toString(const tNativeCallInfo & info)
{
	// message at [発生場所] を返す
	tString message = (tString)info.This.GetPropertyDirect_Object(ss_message);
	tVariant trace_array = info.This.GetPropertyDirect_Object(ss_trace);
	tString at = (tString)trace_array.Invoke_Object(mnIGet, risse_int64(0));
		// 先頭の要素を取り出す
		// 先頭の要素が無い場合は void が返り、at は空文字列になるはず
	if(at.IsEmpty())
		return message;
	else
		return tString(RISSE_WS_TR("%1 at %2"), message, at);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tThrowableClass::addTrace(const tVariant & point,
		const tNativeCallInfo & info)
{
	// 引数 = SourcePoint クラスのインスタンス
	// TODO: インスタンスが SourcePoint クラスのインスタンスかどうかをチェック
	info.This.GetPropertyDirect_Object(ss_trace, 0, info.This).
						FuncCall_Object(NULL, ss_push, 0, info.args, info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tThrowableClass::toException(const tNativeCallInfo & info)
{
	// 自分自身が Class のサブクラスの場合は
	// 例外クラスを構築して返す
	// そうでない場合は This をそのまま返す
	tVariant ret;
	if(info.This.InstanceOf(info.engine, tVariant(info.engine->ClassClass)))
		return info.This.New(0, tMethodArgument::Empty());
	else
		return info.This;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tErrorClass, ss_Error, engine->ThrowableClass)
	BindFunction(this, ss_construct, &tErrorClass::construct);
	BindFunction(this, ss_initialize, &tErrorClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tErrorClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tErrorClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tAssertionErrorClass, ss_AssertionError, engine->ErrorClass)
	BindFunction(this, ss_construct, &tAssertionErrorClass::construct);
	BindFunction(this, ss_initialize, &tAssertionErrorClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tAssertionErrorClass::construct(const tNativeCallInfo & info)
{
	// expression メンバを追加 (デフォルトでは空文字列)
	info.This.SetPropertyDirect_Object(ss_expression,
		tOperateFlags::ofMemberEnsure,
		tVariant(tString::GetEmptyString()), info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tAssertionErrorClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はメッセージ)
	if(info.args.HasArgument(0))
		info.InitializeSuperClass(tMethodArgument::New(info.args[0]));
	else
		info.InitializeSuperClass(tMethodArgument::New(RISSE_WS("assertion failed")));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tAssertionErrorClass::Throw(tScriptEngine * engine, const tString & expression)
{
	tTemporaryException * e = new tTemporaryException(ss_risse, ss_AssertionError, expression);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tBlockExitExceptionClass, ss_BlockExitException, engine->ThrowableClass)
	BindFunction(this, ss_construct, &tBlockExitExceptionClass::construct);
	BindFunction(this, ss_initialize, &tBlockExitExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBlockExitExceptionClass::construct(const tNativeCallInfo & info)
{
	// identifier メンバを追加 (デフォルトではnull)
	info.This.SetPropertyDirect_Object(ss_identifier, tOperateFlags::ofMemberEnsure,
		tVariant::GetNullObject(), info.This);

	// target メンバを追加 (デフォルトでは-1)
	info.This.SetPropertyDirect_Object(ss_target, tOperateFlags::ofMemberEnsure,
		tVariant((risse_int64)-1), info.This);

	// value メンバを追加 (デフォルトではnull)
	info.This.SetPropertyDirect_Object(ss_value, tOperateFlags::ofMemberEnsure,
		tVariant::GetNullObject(), info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBlockExitExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はメッセージ)
	info.InitializeSuperClass(tMethodArgument::New(RISSE_WS("break/return helper exception")));

	// メンバを設定する
	if(info.args.HasArgument(0))
		info.This.SetPropertyDirect_Object(ss_identifier, 0, info.args[0], info.This);
	if(info.args.HasArgument(1))
		info.This.SetPropertyDirect_Object(ss_target, 0, info.args[1], info.This);
	if(info.args.HasArgument(2))
		info.This.SetPropertyDirect_Object(ss_value, 0, info.args[2], info.This);
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tExceptionClass, ss_Exception, engine->ThrowableClass)
	BindFunction(this, ss_construct, &tExceptionClass::construct);
	BindFunction(this, ss_initialize, &tExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tInsufficientResourceExceptionClass, ss_InsufficientResourceException, engine->ExceptionClass)
	BindFunction(this, ss_construct, &tInsufficientResourceExceptionClass::construct);
	BindFunction(this, ss_initialize, &tInsufficientResourceExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInsufficientResourceExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInsufficientResourceExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInsufficientResourceExceptionClass::ThrowCouldNotCreateCoroutine(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_InsufficientResourceException,
			tString(RISSE_WS_TR("could not create coroutine")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tIOExceptionClass, ss_IOException, engine->ExceptionClass)
	BindFunction(this, ss_construct, &tIOExceptionClass::construct);
	BindFunction(this, ss_initialize, &tIOExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIOExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIOExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIOExceptionClass::Throw(tScriptEngine * engine,
							const tString & message)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IOException, message);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIOExceptionClass::ThrowReadError(tScriptEngine * engine,
							const tString & name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IOException,
			name.IsEmpty() ? tString(RISSE_WS_TR("could not read")) :
							 tString(RISSE_WS_TR("could not read at %1"), name)
			);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIOExceptionClass::ThrowWriteError(tScriptEngine * engine,
							const tString & name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IOException,
			name.IsEmpty() ? tString(RISSE_WS_TR("could not write")) :
							 tString(RISSE_WS_TR("could not write at %1"), name)
			);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIOExceptionClass::ThrowSeekError(tScriptEngine * engine,
							const tString & name, risse_size pos)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IOException,
			pos != risse_size_max ?
				(name.IsEmpty() ? tString(RISSE_WS_TR("could not seek to position %1"), tString::AsString((risse_int64)pos)) :
								  tString(RISSE_WS_TR("could not seek to position %1 of %2"), tString::AsString((risse_int64)pos) , name) ) :
				(name.IsEmpty() ? tString(RISSE_WS_TR("could not seek")) :
								  tString(RISSE_WS_TR("could not seek at %1"), name) )
			);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIOExceptionClass::ThrowTruncateError(tScriptEngine * engine,
							const tString & name, risse_size pos)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IOException,
			pos != risse_size_max ?
				(name.IsEmpty() ? tString(RISSE_WS_TR("could not truncate at position %1"), tString::AsString((risse_int64)pos)) :
								  tString(RISSE_WS_TR("could not truncate at position %1 of %2"), tString::AsString((risse_int64)pos) , name) ) :
				(name.IsEmpty() ? tString(RISSE_WS_TR("could not truncate")) :
								  tString(RISSE_WS_TR("could not truncate at %1"), name) )
			);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------

/*
see tInaccessibleResourceExceptionClass::Throw
//---------------------------------------------------------------------------
void tIOExceptionClass::ThrowStreamIsClosed(tScriptEngine * engine, const tString & name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IOException,
			name.IsEmpty() ? tString(RISSE_WS_TR("stream is closed")) :
							 tString(RISSE_WS_TR("stream %1 is closed"), name)
			);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------
*/







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tCharConversionExceptionClass, ss_CharConversionException, engine->IOExceptionClass)
	BindFunction(this, ss_construct, &tCharConversionExceptionClass::construct);
	BindFunction(this, ss_initialize, &tCharConversionExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCharConversionExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCharConversionExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCharConversionExceptionClass::ThrowInvalidUTF8String(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_CharConversionException,
			tString(RISSE_WS_TR("invalid UTF-8 string")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCharConversionExceptionClass::ThrowUTF32OutOfRange(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_CharConversionException,
			tString(RISSE_WS_TR("UTF-32 code out of range")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tRuntimeExceptionClass, ss_RuntimeException, engine->ExceptionClass)
	BindFunction(this, ss_construct, &tRuntimeExceptionClass::construct);
	BindFunction(this, ss_initialize, &tRuntimeExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRuntimeExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRuntimeExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tCompileExceptionClass, ss_CompileException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tCompileExceptionClass::construct);
	BindFunction(this, ss_initialize, &tCompileExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompileExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompileExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompileExceptionClass::Throw(tScriptEngine * engine,
	const tString & reason, const tScriptBlockInstance * sb, risse_size pos)
{
	// 例外インスタンスを生成
	tTemporaryException * et =
		new tTemporaryException(ss_risse, ss_CompileException,
			tString(RISSE_WS_TR("compile error: %1"), reason));
	if(engine)
	{
		tVariant * e = et->Convert(engine);

		// 例外位置情報を追加してやる
		e->AddTrace(sb, pos);

		throw e;
	}
	else
	{
		throw et;
	}
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tImportExceptionClass, ss_ImportException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tImportExceptionClass::construct);
	BindFunction(this, ss_initialize, &tImportExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImportExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImportExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImportExceptionClass::ThrowCannotImportIntoPrimitiveInstanceContext(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_ImportException,
			tString(RISSE_WS_TR("cannot import into primitive instance context")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImportExceptionClass::ThrowPackageNotFound(tScriptEngine * engine, const tString & package_name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_ImportException,
			tString(RISSE_WS_TR("package \"%1\" not found"), package_name));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImportExceptionClass::ThrowPackageIsBeingInitialized(
	tScriptEngine * engine, const tString & package_name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_ImportException,
			tString(RISSE_WS_TR("package \"%1\" is being initialized, cannot be imported"), package_name));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImportExceptionClass::ThrowCannotImportIds(
	tScriptEngine * engine, const gc_vector<tString> & ids)
{
	// ids をつなげる
	static const risse_size max_list = 3;
	risse_size count = ids.size();
	if(count > max_list) count = max_list;
	tString id_list;
	for(risse_size i = 0; i < count; i++)
	{
		if(i != 0) id_list += tSS<',',' '>();
		id_list += tSS<'"'>().operator const tString &() + ids[i] + tSS<'"'>();
	}
	if(ids.size() > count) id_list += tSS<',',' ','.','.','.'>();

	// 例外送出
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_ImportException,
			tString(RISSE_WS_TR("one or more identifiers could not be imported: %1"), id_list));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImportExceptionClass::ThrowInvalidRelativePckageName(tScriptEngine * engine,
			const tString & package_name, const tString & ref)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_ImportException,
			tString(RISSE_WS_TR("invalid relative package name \"%1\" against \"%2\""), package_name, ref));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tClassDefinitionExceptionClass, ss_ClassDefinitionException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tClassDefinitionExceptionClass::construct);
	BindFunction(this, ss_initialize, &tClassDefinitionExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassDefinitionExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassDefinitionExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassDefinitionExceptionClass::ThrowCannotCreateSubClassOfNonExtensibleClass(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_ClassDefinitionException,
			tString(RISSE_WS_TR("cannot create subclass of non-extensible superclass")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassDefinitionExceptionClass::ThrowSuperClassIsNotAClass(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_ClassDefinitionException,
			tString(RISSE_WS_TR("the superclass is not a class")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tInstantiationExceptionClass, ss_InstantiationException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tInstantiationExceptionClass::construct);
	BindFunction(this, ss_initialize, &tInstantiationExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInstantiationExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInstantiationExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInstantiationExceptionClass::ThrowCannotCreateInstanceFromNonClassObject(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_InstantiationException,
			tString(RISSE_WS_TR("cannot create instance from non-class object")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_InstantiationException,
			tString(RISSE_WS_TR("cannot create instance from this class")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tBadContextExceptionClass, ss_BadContextException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tBadContextExceptionClass::construct);
	BindFunction(this, ss_initialize, &tBadContextExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBadContextExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBadContextExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBadContextExceptionClass::Throw(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_BadContextException,
			tString(RISSE_WS_TR("given context is not compatible with this method/property")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tUnsupportedOperationExceptionClass, ss_UnsupportedOperationException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tUnsupportedOperationExceptionClass::construct);
	BindFunction(this, ss_initialize, &tUnsupportedOperationExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tUnsupportedOperationExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tUnsupportedOperationExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tUnsupportedOperationExceptionClass::ThrowCannotCallNonFunctionObjectException(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_UnsupportedOperationException,
			tString(RISSE_WS_TR("cannot call non-function object")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_UnsupportedOperationException,
			tString(RISSE_WS_TR("operation is not implemented")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tArithmeticExceptionClass, ss_ArithmeticException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tArithmeticExceptionClass::construct);
	BindFunction(this, ss_initialize, &tArithmeticExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArithmeticExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArithmeticExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArithmeticExceptionClass::ThrowDivideByZeroException(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_ArithmeticException,
			tString(RISSE_WS_TR("attempt to divide by zero")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tArgumentExceptionClass, ss_ArgumentException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tArgumentExceptionClass::construct);
	BindFunction(this, ss_initialize, &tArgumentExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArgumentExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArgumentExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tNullObjectExceptionClass, ss_NullObjectException, engine->IllegalArgumentExceptionClass)
	BindFunction(this, ss_construct, &tNullObjectExceptionClass::construct);
	BindFunction(this, ss_initialize, &tNullObjectExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNullObjectExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNullObjectExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNullObjectExceptionClass::Throw(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_NullObjectException,
			tString(RISSE_WS_TR("null object was given")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tIllegalArgumentClassExceptionClass, ss_IllegalArgumentClassException, engine->IllegalArgumentExceptionClass)
	BindFunction(this, ss_construct, &tIllegalArgumentClassExceptionClass::construct);
	BindFunction(this, ss_initialize, &tIllegalArgumentClassExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalArgumentClassExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalArgumentClassExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalArgumentClassExceptionClass::ThrowNonAcceptableClass(tScriptEngine * engine,
		const tString & method_name, const tString & class_name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IllegalArgumentClassException,
			tString(RISSE_WS_TR("cannot accept instance of class %2 as argument for method %1()"),
				method_name, class_name));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalArgumentClassExceptionClass::ThrowIllegalOperationMethod(tScriptEngine * engine,
		const tString & method_name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IllegalArgumentClassException,
			tString(RISSE_WS_TR("method %1 is illegal operation"),
				method_name));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalArgumentClassExceptionClass::ThrowSpecifyInstanceOfClass(tScriptEngine * engine,
	const tString & class_name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IllegalArgumentClassException,
			tString(RISSE_WS_TR("specify instance of class %1"),
				class_name));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tIllegalArgumentExceptionClass, ss_IllegalArgumentException, engine->ArgumentExceptionClass)
	BindFunction(this, ss_construct, &tIllegalArgumentExceptionClass::construct);
	BindFunction(this, ss_initialize, &tIllegalArgumentExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalArgumentExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalArgumentExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalArgumentExceptionClass::Throw(tScriptEngine * engine, const tString & message)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IllegalArgumentException, message);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalArgumentExceptionClass::ThrowInvalidDateString(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IllegalArgumentException,
			tString(RISSE_WS_TR("cannot parse the string; invalid date string")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tBadArgumentCountExceptionClass, ss_BadArgumentCountException, engine->ArgumentExceptionClass)
	BindFunction(this, ss_construct, &tBadArgumentCountExceptionClass::construct);
	BindFunction(this, ss_initialize, &tBadArgumentCountExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBadArgumentCountExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBadArgumentCountExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBadArgumentCountExceptionClass::ThrowNormal(tScriptEngine * engine, risse_size passed, risse_size expected)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_BadArgumentCountException,
			tString(RISSE_WS_TR("bad argument count (%1 given, but %2 expected)"),
					tString::AsString((risse_int64)passed),
					tString::AsString((risse_int64)expected)));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBadArgumentCountExceptionClass::ThrowBlock(tScriptEngine * engine, risse_size passed, risse_size expected)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_BadArgumentCountException,
			tString(RISSE_WS_TR("bad block argument count (%1 given, but %2 expected)"),
					tString::AsString((risse_int64)passed),
					tString::AsString((risse_int64)expected)));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tMemberAccessExceptionClass, ss_MemberAccessException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tMemberAccessExceptionClass::construct);
	BindFunction(this, ss_initialize, &tMemberAccessExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMemberAccessExceptionClass::construct(const tNativeCallInfo & info)
{
	// name メンバを追加 (デフォルトでは空文字列)
	info.This.SetPropertyDirect_Object(ss_name, tOperateFlags::ofMemberEnsure,
		tVariant(tString::GetEmptyString()), info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMemberAccessExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はargs[0])
	info.InitializeSuperClass(tMethodArgument::New(info.args[0]));

	// メンバを設定する
	if(info.args.HasArgument(1))
		info.This.SetPropertyDirect_Object(ss_name, 0, info.args[1], info.This);
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tNoSuchMemberExceptionClass, ss_NoSuchMemberException, engine->MemberAccessExceptionClass)
	BindFunction(this, ss_construct, &tNoSuchMemberExceptionClass::construct);
	BindFunction(this, ss_initialize, &tNoSuchMemberExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNoSuchMemberExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNoSuchMemberExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNoSuchMemberExceptionClass::Throw(tScriptEngine * engine, const tString & name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_NoSuchMemberException,
			name.IsEmpty() ?
					tString(RISSE_WS_TR("member not found"), name):
					tString(RISSE_WS_TR("member \"%1\" not found"), name),
				name);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tIllegalMemberAccessExceptionClass, ss_IllegalMemberAccessException, engine->MemberAccessExceptionClass)
	BindFunction(this, ss_construct, &tIllegalMemberAccessExceptionClass::construct);
	BindFunction(this, ss_initialize, &tIllegalMemberAccessExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalMemberAccessExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalMemberAccessExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalMemberAccessExceptionClass::ThrowMemberIsReadOnly(tScriptEngine * engine, const tString & name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IllegalMemberAccessException,
			name.IsEmpty() ?
					tString(RISSE_WS_TR("member is read-only"), name):
					tString(RISSE_WS_TR("member \"%1\" is read-only"), name),
				name);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalMemberAccessExceptionClass::ThrowMemberIsFinal(tScriptEngine * engine, const tString & name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IllegalMemberAccessException,
			name.IsEmpty() ?
					tString(RISSE_WS_TR("member is final, cannot be overridden"), name):
					tString(RISSE_WS_TR("member \"%1\" is final, cannot be overridden"), name),
				name);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalMemberAccessExceptionClass::ThrowPropertyCannotBeRead(tScriptEngine * engine, const tString & name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IllegalMemberAccessException,
			name.IsEmpty() ?
					tString(RISSE_WS_TR("property cannot be read"), name):
					tString(RISSE_WS_TR("property \"%1\" cannot be read"), name),
				name);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalMemberAccessExceptionClass::ThrowPropertyCannotBeWritten(tScriptEngine * engine, const tString & name)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_IllegalMemberAccessException,
			name.IsEmpty() ?
					tString(RISSE_WS_TR("property cannot be written"), name):
					tString(RISSE_WS_TR("property \"%1\" cannot be written"), name),
				name);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tIllegalStateExceptionClass, ss_IllegalStateException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tIllegalStateExceptionClass::construct);
	BindFunction(this, ss_initialize, &tIllegalStateExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalStateExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIllegalStateExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tInaccessibleResourceExceptionClass, ss_InaccessibleResourceException, engine->IllegalStateExceptionClass)
	BindFunction(this, ss_construct, &tInaccessibleResourceExceptionClass::construct);
	BindFunction(this, ss_initialize, &tInaccessibleResourceExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInaccessibleResourceExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInaccessibleResourceExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInaccessibleResourceExceptionClass::Throw(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_risse, ss_InaccessibleResourceException,
					tString(RISSE_WS_TR("the resource is not accessible")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------









} // namespace Risse
