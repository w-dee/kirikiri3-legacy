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
#include "risseNativeFunction.h"
#include "risseArrayClass.h"
#include "risseClassClass.h"
#include "risseScriptBlockBase.h"
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
	tRisseVariant cls = engine->GetGlobalObject().GetPropertyDirect_Object(ExceptionClassName);

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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// filename メンバを追加 (デフォルトでは空文字列)
		This.SetPropertyDirect_Object(ss_filename, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);

		// line メンバを追加 (デフォルトでは-1)
		This.SetPropertyDirect_Object(ss_line, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant((risse_int64)-1), This);

		// function メンバを追加 (デフォルトではnull)
		This.SetPropertyDirect_Object(ss_function, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetNullObject(), This);

		/*
			ほかにメンバを追加するかも
		*/
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数は空)
		engine->SourcePointClass->CallSuperClassMethod(NULL, ss_initialize, 0, tRisseMethodArgument::Empty(), This);

		// 引数 = ファイル名, 行, メソッド
		if(args.HasArgument(0))
			This.SetPropertyDirect_Object(ss_filename, 0, args[0], This);
		if(args.HasArgument(1))
			This.SetPropertyDirect_Object(ss_line, 0, args[1], This);
		if(args.HasArgument(2))
			This.SetPropertyDirect_Object(ss_function, 0, args[2], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(mnString) // toString
	{
		// ファイル名:行番号: in 関数名 を生成して返す

		tRisseVariant filename, line, function;
		filename = This.GetPropertyDirect_Object(ss_filename);
		line =     This.GetPropertyDirect_Object(ss_line);
		function = This.GetPropertyDirect_Object(ss_function);

		tRisseString ret;
		tRisseString fn = filename.operator tRisseString();
		if(!fn.IsEmpty())
			ret = fn + RISSE_WC(':');
		else
			ret = RISSE_WS_TR("<unknown>:");

		if(line != tRisseVariant((risse_int64)-1))
			ret += line.operator tRisseString();
		else
			ret += RISSE_WS_TR("<unknown>");

		if(!function.IsNull())
			ret += tRisseString(RISSE_WS_TR(": in ")) + function.operator tRisseString();

		if(result) *result = ret;
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// message メンバを追加 (デフォルトでは空文字列)
		This.SetPropertyDirect_Object(ss_message, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);

		// trace メンバを追加 (デフォルトでは空配列)
		This.SetPropertyDirect_Object(ss_trace, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseVariant(engine->ArrayClass).New()), This);

		// cause メンバを追加 (デフォルトではnull)
		This.SetPropertyDirect_Object(ss_cause, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetNullObject(), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数は空)
		engine->ThrowableClass->CallSuperClassMethod(NULL, ss_initialize, 0,
										tRisseMethodArgument::Empty(), This);

		// 引数 = メッセージ
		if(args.HasArgument(0))
			This.SetPropertyDirect_Object(ss_message, 0, args[0], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(mnString) // toString
	{
		// message at [発生場所] を返す
		tRisseString message = (tRisseString)This.GetPropertyDirect_Object(ss_message);
		tRisseVariant trace_array = This.GetPropertyDirect_Object(ss_trace);
		tRisseString at = (tRisseString)trace_array.Invoke_Object(mnIGet, risse_int64(0));
			// 先頭の要素を取り出す
			// 先頭の要素が無い場合は void が返り、at は空文字列になるはず
		if(result)
		{
			if(at.IsEmpty())
				*result = message;
			else
				*result = tRisseString(RISSE_WS_TR("%1 at %2"), message, at);
		}
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_addTrace)
	{
		args.ExpectArgumentCount(1);

		// 引数 = SourcePoint クラスのインスタンス
		// TODO: インスタンスが SourcePoint クラスのインスタンスかどうかをチェック
		This.GetPropertyDirect_Object(ss_trace, 0, This).FuncCall_Object(NULL, ss_push, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_toException)
	{
		// 自分自身が Class のサブクラスの場合は
		// 例外クラスを構築して返す
		// そうでない場合は This をそのまま返す
		tRisseVariant ret;
		if(This.InstanceOf(engine, tRisseVariant(engine->ClassClass)))
			ret = This.New(0, tRisseMethodArgument::Empty());
		else
			ret = This;
		if(result) *result = ret;
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->ExceptionClass->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// expression メンバを追加 (デフォルトでは空文字列)
		This.SetPropertyDirect_Object(ss_expression,
			tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はメッセージ)
		if(args.HasArgument(0))
			engine->AssertionErrorClass->CallSuperClassMethod(NULL, ss_initialize, 0,
				tRisseMethodArgument::New(args[0]), This);
		else
			engine->AssertionErrorClass->CallSuperClassMethod(NULL, ss_initialize, 0,
				tRisseMethodArgument::New(RISSE_WS("assertion failed")), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// identifier メンバを追加 (デフォルトではnull)
		This.SetPropertyDirect_Object(ss_identifier, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetNullObject(), This);

		// target メンバを追加 (デフォルトでは-1)
		This.SetPropertyDirect_Object(ss_target, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant((risse_int64)-1), This);

		// value メンバを追加 (デフォルトではnull)
		This.SetPropertyDirect_Object(ss_value, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetNullObject(), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はメッセージ)
		engine->BlockExitExceptionClass->CallSuperClassMethod(NULL, ss_initialize, 0,
			tRisseMethodArgument::New(RISSE_WS("break/return helper exception")), This);

		// メンバを設定する
		if(args.HasArgument(0))
			This.SetPropertyDirect_Object(ss_identifier, 0, args[0], This);
		if(args.HasArgument(1))
			This.SetPropertyDirect_Object(ss_target, 0, args[1], This);
		if(args.HasArgument(2))
			This.SetPropertyDirect_Object(ss_value, 0, args[2], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->ExceptionClass->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->IOExceptionClass->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->CharConversionExceptionClass->CallSuperClassMethod(NULL,
												ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->RuntimeExceptionClass->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->CompileExceptionClass->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompileExceptionClass::Throw(tRisseScriptEngine * engine,
	const tRisseString & reason, const tRisseScriptBlockBase * sb, risse_size pos)
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->ClassDefinitionExceptionClass->CallSuperClassMethod(NULL,
												ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->InstantiationExceptionClass->CallSuperClassMethod(NULL,
												ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->BadContextExceptionClass->CallSuperClassMethod(NULL,
												ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->UnsupportedOperationExceptionClass->CallSuperClassMethod(NULL,
												ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->ArgumentExceptionClass->CallSuperClassMethod(NULL,
												ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->IllegalArgumentExceptionClass->CallSuperClassMethod(NULL,
												ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->NullObjectExceptionClass->CallSuperClassMethod(NULL,
												ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->BadArgumentCountExceptionClass->CallSuperClassMethod(NULL,
												ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// name メンバを追加 (デフォルトでは空文字列)
		This.SetPropertyDirect_Object(ss_name, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はargs[0])
		engine->MemberAccessExceptionClass->CallSuperClassMethod(NULL,
			ss_initialize, 0, tRisseMethodArgument::New(args[0]), This);

		// メンバを設定する
		if(args.HasArgument(1))
			This.SetPropertyDirect_Object(ss_name, 0, args[1], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->NoSuchMemberExceptionClass->CallSuperClassMethod(NULL,
											ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやることはない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はそのまま)
		engine->IllegalMemberAccessExceptionClass->CallSuperClassMethod(NULL,
											ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// name メンバを追加 (デフォルトでは空文字列)
		This.SetPropertyDirect_Object(ss_name, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はargs[0])
		engine->CoroutineExceptionClass->CallSuperClassMethod(NULL,
			ss_initialize, 0, tRisseMethodArgument::New(args[0]), This);

		// メンバを設定する
		if(args.HasArgument(1))
			This.SetPropertyDirect_Object(ss_name, 0, args[1], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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


} // namespace Risse
