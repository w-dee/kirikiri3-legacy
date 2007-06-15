//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
       ClassDefinitionException
       InstantiationException
       BadContextException
       UnsupportedOperation
       ArgumentException
         IllegalArgumentException
           NullObjectException
         BadArgumentCountException
       MemberAccessException
         NoSuchMemberException
         IllegalMemberAccessException
       CoroutineException
*/




//---------------------------------------------------------------------------
tRisseTemporaryException::tRisseTemporaryException(const tRisseString classname)
{
	ExceptionClassName = classname;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseTemporaryException::tRisseTemporaryException(const tRisseString classname,
		const tRisseVariant & arg1)
{
	ExceptionClassName = classname;
	Arguments.push_back(arg1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseTemporaryException::tRisseTemporaryException(const tRisseString classname,
		const tRisseVariant & arg1, const tRisseVariant & arg2)
{
	ExceptionClassName = classname;
	Arguments.push_back(arg1);
	Arguments.push_back(arg2);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseTemporaryException::tRisseTemporaryException(const tRisseString classname,
		const tRisseVariant & arg1, const tRisseVariant & arg2, const tRisseVariant & arg3)
{
	ExceptionClassName = classname;
	Arguments.push_back(arg1);
	Arguments.push_back(arg2);
	Arguments.push_back(arg3);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant * tRisseTemporaryException::Convert(tRisseScriptEngine * engine) const
{
	// まず、例外クラスを取得する
	tRisseVariant cls;
	tRisseObjectInterface::tRetValue ret =
		engine->GetGlobalObject().Operate(engine, ocDGet, &cls, ExceptionClassName);
	if(ret != tRisseObjectInterface::rvNoError)
	{
		// ExceptionClassNameを取得できなかった。
		// この場合は ss_RuntimeException を取得する
		cls = engine->GetGlobalObject().GetPropertyDirect_Object(ss_RuntimeException);
	}

	// 引数を用意する
	tRisseMethodArgument & new_args = tRisseMethodArgument::Allocate(Arguments.size());

	for(risse_size i = 0; i < Arguments.size(); i++)
		new_args.SetArgument(i, Arguments[i]);

	// New を呼び出し、それを返す
	return new tRisseVariant(cls.New(0, new_args));
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseExitTryExceptionClass::tRisseExitTryExceptionClass(tRisseScriptEngine * engine,
	const void * id, risse_uint32 targ_idx, const tRisseVariant * value) :
		tRisseObjectInterface(new tRisseRTTI(engine))
{
	Identifier = id;
	BranchTargetIndex = targ_idx;
	Value = value ? new tRisseVariant(*value) : NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectInterface::tRetValue tRisseExitTryExceptionClass::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// 暫定実装
	if(code == ocDGet && name == RISSE_WS("getExitTryRecord"))
	{
		// tRisseExitTryExceptionClass 型を返す
		if(result)
		{
			*result = reinterpret_cast<tRisseObjectInterface*>
				((tRisseExitTryExceptionClass*)this);
		}
	}
	return rvNoError;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void tRisseSourcePointInstance::construct()
{
	// scriptBlock メンバを追加 (デフォルトではnull)
	RegisterNormalMember(ss_scriptBlock, tRisseVariant::GetNullObject());

	// position メンバを追加 (デフォルトでは-1)
	RegisterNormalMember(ss_position, tRisseVariant((risse_int64)-1));

	// function メンバを追加 (デフォルトではnull)
	RegisterNormalMember(ss_function, tRisseVariant::GetNullObject());

	/*
		ほかにメンバを追加するかも
	*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSourcePointInstance::initialize(const tRisseNativeCallInfo & info)
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
tRisseString tRisseSourcePointInstance::toString() const
{
	// ファイル名:行番号: in 関数名 を生成して返す

	tRisseVariant sb, position, function;
	sb =       ReadMember(ss_scriptBlock);
	position = ReadMember(ss_position);
	function = ReadMember(ss_function);

	tRisseString ret;
	tRisseString fn = sb.GetPropertyDirect(GetRTTI()->GetScriptEngine(), ss_name);
	if(!fn.IsEmpty())
		ret = fn + RISSE_WC(':');
	else
		ret = RISSE_WS_TR("<unknown>:");

	if(position != tRisseVariant((risse_int64)-1))
	{
		tRisseVariant line = sb.Invoke(GetRTTI()->GetScriptEngine(), ss_positionToLine, position);
		ret += (line + tRisseVariant((risse_int64)1)).operator tRisseString();
	}
	else
		ret += RISSE_WS_TR("<unknown>");

	if(!function.IsNull())
		ret += tRisseString(RISSE_WS_TR(": in ")) + function.operator tRisseString();

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSourcePointClass::tRisseSourcePointClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSourcePointClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct,  &tRisseSourcePointInstance::construct);
	RisseBindFunction(this, ss_initialize,  &tRisseSourcePointInstance::initialize);
	RisseBindFunction(this, mnString,  &tRisseSourcePointInstance::toString);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseSourcePointClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseSourcePointInstance());
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tRisseThrowableClass::tRisseThrowableClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThrowableClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseThrowableClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseThrowableClass::initialize);
	RisseBindFunction(this, mnString, &tRisseThrowableClass::toString);
	RisseBindFunction(this, ss_addTrace, &tRisseThrowableClass::addTrace);
	RisseBindFunction(this, ss_toException, &tRisseThrowableClass::toException);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThrowableClass::construct(const tRisseNativeCallInfo & info)
{
	// message メンバを追加 (デフォルトでは空文字列)
	info.This.SetPropertyDirect_Object(ss_message, tRisseOperateFlags::ofMemberEnsure,
		tRisseVariant(tRisseString::GetEmptyString()), info.This);

	// trace メンバを追加 (デフォルトでは空配列)
	info.This.SetPropertyDirect_Object(ss_trace, tRisseOperateFlags::ofMemberEnsure,
		tRisseVariant(tRisseVariant(info.engine->ArrayClass).New()), info.This);

	// cause メンバを追加 (デフォルトではnull)
	info.This.SetPropertyDirect_Object(ss_cause, tRisseOperateFlags::ofMemberEnsure,
		tRisseVariant::GetNullObject(), info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThrowableClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数は空)
	info.InitializeSuperClass();

	// 引数 = メッセージ
	if(info.args.HasArgument(0))
		info.This.SetPropertyDirect_Object(ss_message, 0, info.args[0], info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseThrowableClass::toString(const tRisseNativeCallInfo & info)
{
	// message at [発生場所] を返す
	tRisseString message = (tRisseString)info.This.GetPropertyDirect_Object(ss_message);
	tRisseVariant trace_array = info.This.GetPropertyDirect_Object(ss_trace);
	tRisseString at = (tRisseString)trace_array.Invoke_Object(mnIGet, risse_int64(0));
		// 先頭の要素を取り出す
		// 先頭の要素が無い場合は void が返り、at は空文字列になるはず
	if(at.IsEmpty())
		return message;
	else
		return tRisseString(RISSE_WS_TR("%1 at %2"), message, at);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThrowableClass::addTrace(const tRisseVariant & point,
		const tRisseNativeCallInfo & info)
{
	// 引数 = SourcePoint クラスのインスタンス
	// TODO: インスタンスが SourcePoint クラスのインスタンスかどうかをチェック
	info.This.GetPropertyDirect_Object(ss_trace, 0, info.This).
						FuncCall_Object(NULL, ss_push, 0, info.args, info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseThrowableClass::toException(const tRisseNativeCallInfo & info)
{
	// 自分自身が Class のサブクラスの場合は
	// 例外クラスを構築して返す
	// そうでない場合は This をそのまま返す
	tRisseVariant ret;
	if(info.This.InstanceOf(info.engine, tRisseVariant(info.engine->ClassClass)))
		return info.This.New(0, tRisseMethodArgument::Empty());
	else
		return info.This;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisseErrorClass::tRisseErrorClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ThrowableClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseErrorClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseErrorClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseErrorClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseErrorClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseErrorClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tRisseAssertionErrorClass::tRisseAssertionErrorClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ErrorClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseAssertionErrorClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseAssertionErrorClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseAssertionErrorClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseAssertionErrorClass::construct(const tRisseNativeCallInfo & info)
{
	// expression メンバを追加 (デフォルトでは空文字列)
	info.This.SetPropertyDirect_Object(ss_expression,
		tRisseOperateFlags::ofMemberEnsure,
		tRisseVariant(tRisseString::GetEmptyString()), info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseAssertionErrorClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はメッセージ)
	if(info.args.HasArgument(0))
		info.InitializeSuperClass(tRisseMethodArgument::New(info.args[0]));
	else
		info.InitializeSuperClass(tRisseMethodArgument::New(RISSE_WS("assertion failed")));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseAssertionErrorClass::Throw(tRisseScriptEngine * engine, const tRisseString & expression)
{
	tRisseTemporaryException * e = new tRisseTemporaryException(ss_AssertionError, expression);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
tRisseBlockExitExceptionClass::tRisseBlockExitExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ThrowableClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBlockExitExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseBlockExitExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseBlockExitExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBlockExitExceptionClass::construct(const tRisseNativeCallInfo & info)
{
	// identifier メンバを追加 (デフォルトではnull)
	info.This.SetPropertyDirect_Object(ss_identifier, tRisseOperateFlags::ofMemberEnsure,
		tRisseVariant::GetNullObject(), info.This);

	// target メンバを追加 (デフォルトでは-1)
	info.This.SetPropertyDirect_Object(ss_target, tRisseOperateFlags::ofMemberEnsure,
		tRisseVariant((risse_int64)-1), info.This);

	// value メンバを追加 (デフォルトではnull)
	info.This.SetPropertyDirect_Object(ss_value, tRisseOperateFlags::ofMemberEnsure,
		tRisseVariant::GetNullObject(), info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBlockExitExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はメッセージ)
	info.InitializeSuperClass(tRisseMethodArgument::New(RISSE_WS("break/return helper exception")));

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
tRisseExceptionClass::tRisseExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ThrowableClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tRisseInsufficientResourceExceptionClass::tRisseInsufficientResourceExceptionClass(
	tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInsufficientResourceExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseInsufficientResourceExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseInsufficientResourceExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInsufficientResourceExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInsufficientResourceExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInsufficientResourceExceptionClass::ThrowCouldNotCreateCoroutine(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_InsufficientResourceException,
			tRisseString(RISSE_WS_TR("could not create coroutine")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseIOExceptionClass::tRisseIOExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIOExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseIOExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseIOExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIOExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIOExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIOExceptionClass::ThrowReadError(tRisseScriptEngine * engine,
							const tRisseString & name)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_IOException,
			name.IsEmpty() ? tRisseString(RISSE_WS_TR("could not read")) :
							 tRisseString(RISSE_WS_TR("could not read at %1"), name)
			);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIOExceptionClass::ThrowWriteError(tRisseScriptEngine * engine,
							const tRisseString & name)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_IOException,
			name.IsEmpty() ? tRisseString(RISSE_WS_TR("could not write")) :
							 tRisseString(RISSE_WS_TR("could not write at %1"), name)
			);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIOExceptionClass::ThrowSeekError(tRisseScriptEngine * engine,
							const tRisseString & name, risse_size pos)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_IOException,
			pos != risse_size_max ?
				(name.IsEmpty() ? tRisseString(RISSE_WS_TR("could not seek to position %1"), tRisseString::AsString((risse_int64)pos)) :
								  tRisseString(RISSE_WS_TR("could not seek to position %1 of %2"), tRisseString::AsString((risse_int64)pos) , name) ) :
				(name.IsEmpty() ? tRisseString(RISSE_WS_TR("could not seek")) :
								  tRisseString(RISSE_WS_TR("could not seek at %1"), name) )
			);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIOExceptionClass::ThrowTruncateError(tRisseScriptEngine * engine,
							const tRisseString & name, risse_size pos)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_IOException,
			pos != risse_size_max ?
				(name.IsEmpty() ? tRisseString(RISSE_WS_TR("could not truncate at position %1"), tRisseString::AsString((risse_int64)pos)) :
								  tRisseString(RISSE_WS_TR("could not truncate at position %1 of %2"), tRisseString::AsString((risse_int64)pos) , name) ) :
				(name.IsEmpty() ? tRisseString(RISSE_WS_TR("could not truncate")) :
								  tRisseString(RISSE_WS_TR("could not truncate at %1"), name) )
			);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseCharConversionExceptionClass::tRisseCharConversionExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->IOExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCharConversionExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseCharConversionExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseCharConversionExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCharConversionExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCharConversionExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCharConversionExceptionClass::ThrowInvalidUTF8String(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_CharConversionException,
			tRisseString(RISSE_WS_TR("invalid UTF-8 string")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseRuntimeExceptionClass::tRisseRuntimeExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseRuntimeExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseRuntimeExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseRuntimeExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseRuntimeExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseRuntimeExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseCompileExceptionClass::tRisseCompileExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->RuntimeExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompileExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseCompileExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseCompileExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompileExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompileExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompileExceptionClass::Throw(tRisseScriptEngine * engine,
	const tRisseString & reason, const tRisseScriptBlockInstance * sb, risse_size pos)
{
	// 例外インスタンスを生成
	tRisseTemporaryException * et =
		new tRisseTemporaryException(ss_CompileException,
			tRisseString(RISSE_WS_TR("compile error: %1"), reason));
	if(engine)
	{
		tRisseVariant * e = et->Convert(engine);

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
tRisseClassDefinitionExceptionClass::tRisseClassDefinitionExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->RuntimeExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassDefinitionExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseClassDefinitionExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseClassDefinitionExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassDefinitionExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassDefinitionExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassDefinitionExceptionClass::ThrowCannotCreateSubClassOfNonExtensibleClass(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_ClassDefinitionException,
			tRisseString(RISSE_WS_TR("cannot create subclass of non-extensible superclass")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassDefinitionExceptionClass::ThrowSuperClassIsNotAClass(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_ClassDefinitionException,
			tRisseString(RISSE_WS_TR("the superclass is not a class")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseInstantiationExceptionClass::tRisseInstantiationExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->RuntimeExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInstantiationExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseInstantiationExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseInstantiationExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInstantiationExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInstantiationExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromNonClassObject(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_InstantiationException,
			tRisseString(RISSE_WS_TR("cannot create instance from non-class object")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_InstantiationException,
			tRisseString(RISSE_WS_TR("cannot create instance from this class")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tRisseBadContextExceptionClass::tRisseBadContextExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->RuntimeExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadContextExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseBadContextExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseBadContextExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadContextExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadContextExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadContextExceptionClass::Throw(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_BadContextException,
			tRisseString(RISSE_WS_TR("given context is not compatible with this method/property")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseUnsupportedOperationExceptionClass::tRisseUnsupportedOperationExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->RuntimeExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseUnsupportedOperationExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseUnsupportedOperationExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseUnsupportedOperationExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseUnsupportedOperationExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseUnsupportedOperationExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseUnsupportedOperationExceptionClass::ThrowCannotCallNonFunctionObjectException(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_UnsupportedOperationException,
			tRisseString(RISSE_WS_TR("cannot call non-function object")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_UnsupportedOperationException,
			tRisseString(RISSE_WS_TR("operation is not implemented")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseArgumentExceptionClass::tRisseArgumentExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->RuntimeExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseArgumentExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseArgumentExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseArgumentExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseArgumentExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseArgumentExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tRisseNullObjectExceptionClass::tRisseNullObjectExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->IllegalArgumentExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNullObjectExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseNullObjectExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseNullObjectExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNullObjectExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNullObjectExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNullObjectExceptionClass::Throw(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_NullObjectException,
			tRisseString(RISSE_WS_TR("null object was given")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseIllegalArgumentExceptionClass::tRisseIllegalArgumentExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ArgumentExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalArgumentExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseIllegalArgumentExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseIllegalArgumentExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalArgumentExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalArgumentExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tRisseBadArgumentCountExceptionClass::tRisseBadArgumentCountExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ArgumentExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadArgumentCountExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseBadArgumentCountExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseBadArgumentCountExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadArgumentCountExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadArgumentCountExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadArgumentCountExceptionClass::ThrowNormal(tRisseScriptEngine * engine, risse_size passed, risse_size expected)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_BadArgumentCountException,
			tRisseString(RISSE_WS_TR("bad argument count (%1 given, but %2 expected)"),
					tRisseString::AsString((risse_int64)passed),
					tRisseString::AsString((risse_int64)expected)));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadArgumentCountExceptionClass::ThrowBlock(tRisseScriptEngine * engine, risse_size passed, risse_size expected)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_BadArgumentCountException,
			tRisseString(RISSE_WS_TR("bad block argument count (%1 given, but %2 expected)"),
					tRisseString::AsString((risse_int64)passed),
					tRisseString::AsString((risse_int64)expected)));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisseMemberAccessExceptionClass::tRisseMemberAccessExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->RuntimeExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseMemberAccessExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseMemberAccessExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseMemberAccessExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseMemberAccessExceptionClass::construct(const tRisseNativeCallInfo & info)
{
	// name メンバを追加 (デフォルトでは空文字列)
	info.This.SetPropertyDirect_Object(ss_name, tRisseOperateFlags::ofMemberEnsure,
		tRisseVariant(tRisseString::GetEmptyString()), info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseMemberAccessExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はargs[0])
	info.InitializeSuperClass(tRisseMethodArgument::New(info.args[0]));

	// メンバを設定する
	if(info.args.HasArgument(1))
		info.This.SetPropertyDirect_Object(ss_name, 0, info.args[1], info.This);
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisseNoSuchMemberExceptionClass::tRisseNoSuchMemberExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->MemberAccessExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNoSuchMemberExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseNoSuchMemberExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseNoSuchMemberExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNoSuchMemberExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNoSuchMemberExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNoSuchMemberExceptionClass::Throw(tRisseScriptEngine * engine, const tRisseString & name)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_NoSuchMemberException,
			name.IsEmpty() ?
					tRisseString(RISSE_WS_TR("member not found"), name):
					tRisseString(RISSE_WS_TR("member \"%1\" not found"), name),
				name);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisseIllegalMemberAccessExceptionClass::tRisseIllegalMemberAccessExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->MemberAccessExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalMemberAccessExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseIllegalMemberAccessExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseIllegalMemberAccessExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalMemberAccessExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalMemberAccessExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalMemberAccessExceptionClass::ThrowMemberIsReadOnly(tRisseScriptEngine * engine, const tRisseString & name)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_IllegalMemberAccessException,
			name.IsEmpty() ?
					tRisseString(RISSE_WS_TR("member is read-only"), name):
					tRisseString(RISSE_WS_TR("member \"%1\" is read-only"), name),
				name);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalMemberAccessExceptionClass::ThrowMemberIsFinal(tRisseScriptEngine * engine, const tRisseString & name)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_IllegalMemberAccessException,
			name.IsEmpty() ?
					tRisseString(RISSE_WS_TR("member is final, cannot be overridden"), name):
					tRisseString(RISSE_WS_TR("member \"%1\" is final, cannot be overridden"), name),
				name);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalMemberAccessExceptionClass::ThrowPropertyCannotBeRead(tRisseScriptEngine * engine, const tRisseString & name)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_IllegalMemberAccessException,
			name.IsEmpty() ?
					tRisseString(RISSE_WS_TR("property cannot be read"), name):
					tRisseString(RISSE_WS_TR("property \"%1\" cannot be read"), name),
				name);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalMemberAccessExceptionClass::ThrowPropertyCannotBeWritten(tRisseScriptEngine * engine, const tRisseString & name)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_IllegalMemberAccessException,
			name.IsEmpty() ?
					tRisseString(RISSE_WS_TR("property cannot be written"), name):
					tRisseString(RISSE_WS_TR("property \"%1\" cannot be written"), name),
				name);
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tRisseCoroutineExceptionClass::tRisseCoroutineExceptionClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->RuntimeExceptionClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineExceptionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseCoroutineExceptionClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseCoroutineExceptionClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineExceptionClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineExceptionClass::ThrowCoroutineHasAlreadyExited(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_CoroutineException,
					tRisseString(RISSE_WS_TR("coroutine has already exited")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineExceptionClass::ThrowCoroutineHasNotStartedYet(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_CoroutineException,
					tRisseString(RISSE_WS_TR("coroutine has not started yet")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineExceptionClass::ThrowCoroutineIsNotRunning(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_CoroutineException,
					tRisseString(RISSE_WS_TR("coroutine is not running")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineExceptionClass::ThrowCoroutineIsRunning(tRisseScriptEngine * engine)
{
	tRisseTemporaryException * e =
		new tRisseTemporaryException(ss_CoroutineException,
					tRisseString(RISSE_WS_TR("coroutine is currently running")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------

} // namespace Risse
