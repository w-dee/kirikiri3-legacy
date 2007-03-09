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

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(64113,30630,41963,17808,15295,58919,39993,4429);


//---------------------------------------------------------------------------
tRisseExitTryExceptionClass::tRisseExitTryExceptionClass(
	const void * id, risse_uint32 targ_idx, const tRisseVariant * value)
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
tRisseSourcePointClass::tRisseSourcePointClass() :
	tRisseClassBase(tRisseObjectClass::GetPointer())
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
		This.SetPropertyDirect(ss_filename, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);

		// line メンバを追加 (デフォルトでは-1)
		This.SetPropertyDirect(ss_line, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant((risse_int64)-1), This);

		// function メンバを追加 (デフォルトではnull)
		This.SetPropertyDirect(ss_function, tRisseOperateFlags::ofMemberEnsure,
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
		tRisseSourcePointClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, tRisseMethodArgument::Empty(), This);

		// 引数 = ファイル名, 行, メソッド
		if(args.HasArgument(0))
			This.SetPropertyDirect(ss_filename, 0, args[0], This);
		if(args.HasArgument(1))
			This.SetPropertyDirect(ss_line, 0, args[1], This);
		if(args.HasArgument(2))
			This.SetPropertyDirect(ss_function, 0, args[2], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(mnString) // toString
	{
		// ファイル名:行番号: in 関数名 を生成して返す

		tRisseVariant filename, line, function;
		filename = This.GetPropertyDirect(ss_filename);
		line =     This.GetPropertyDirect(ss_line);
		function = This.GetPropertyDirect(ss_function);

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
tRisseThrowableClass::tRisseThrowableClass() :
	tRisseClassBase(tRisseObjectClass::GetPointer())
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
		This.SetPropertyDirect(ss_message, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);

		// trace メンバを追加 (デフォルトでは空配列)
		This.SetPropertyDirect(ss_trace, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseVariant(tRisseArrayClass::GetPointer()).New()), This);

		// cause メンバを追加 (デフォルトではnull)
		This.SetPropertyDirect(ss_cause, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetNullObject(), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数は空)
		tRisseThrowableClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, tRisseMethodArgument::Empty(), This);

		// 引数 = メッセージ
		if(args.HasArgument(0))
			This.SetPropertyDirect(ss_message, 0, args[0], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(mnString) // toString
	{
		// message at [発生場所] を返す
		tRisseString message = (tRisseString)This.GetPropertyDirect(ss_message);
		tRisseVariant trace_array = This.GetPropertyDirect(ss_trace);
		tRisseString at = (tRisseString)trace_array.Invoke(mnIGet, risse_int64(0));
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
		This.GetPropertyDirect(ss_trace, 0, This).FuncCall(NULL, ss_push, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_toException)
	{
		// 自分自身が Class のサブクラスの場合は
		// 例外クラスを構築して返す
		// そうでない場合は This をそのまま返す
		tRisseVariant ret;
		if(This.InstanceOf(tRisseVariant(tRisseClassClass::GetPointer())))
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
tRisseErrorClass::tRisseErrorClass() :
	tRisseClassBase(tRisseThrowableClass::GetPointer())
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
		tRisseExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseAssertionErrorClass::tRisseAssertionErrorClass() :
	tRisseClassBase(tRisseThrowableClass::GetPointer())
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
		This.SetPropertyDirect(ss_expression,
			tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はメッセージ)
		if(args.HasArgument(0))
			tRisseAssertionErrorClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0,
				tRisseMethodArgument::New(args[0]), This);
		else
			tRisseAssertionErrorClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0,
				tRisseMethodArgument::New(RISSE_WS("assertion failed")), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseAssertionErrorClass::Throw(const tRisseString & expression)
{
	throw new tRisseVariant(
		tRisseVariant(tRisseAssertionErrorClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(expression)
					));
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
tRisseBlockExitExceptionClass::tRisseBlockExitExceptionClass() :
	tRisseClassBase(tRisseThrowableClass::GetPointer())
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
		This.SetPropertyDirect(ss_identifier, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetNullObject(), This);

		// target メンバを追加 (デフォルトでは-1)
		This.SetPropertyDirect(ss_target, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant((risse_int64)-1), This);

		// value メンバを追加 (デフォルトではnull)
		This.SetPropertyDirect(ss_value, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetNullObject(), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はメッセージ)
		tRisseBlockExitExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0,
			tRisseMethodArgument::New(RISSE_WS("break/return helper exception")), This);

		// メンバを設定する
		if(args.HasArgument(0))
			This.SetPropertyDirect(ss_identifier, 0, args[0], This);
		if(args.HasArgument(1))
			This.SetPropertyDirect(ss_target, 0, args[1], This);
		if(args.HasArgument(2))
			This.SetPropertyDirect(ss_value, 0, args[2], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
tRisseExceptionClass::tRisseExceptionClass() :
	tRisseClassBase(tRisseThrowableClass::GetPointer())
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
		tRisseExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
tRisseIOExceptionClass::tRisseIOExceptionClass() :
	tRisseClassBase(tRisseExceptionClass::GetPointer())
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
		tRisseIOExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseCharConversionExceptionClass::tRisseCharConversionExceptionClass() :
	tRisseClassBase(tRisseIOExceptionClass::GetPointer())
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
		tRisseCharConversionExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCharConversionExceptionClass::ThrowInvalidUTF8String()
{
	throw new tRisseVariant(
		tRisseVariant(tRisseCharConversionExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				tRisseString(RISSE_WS_TR("invalid UTF-8 string"))
					)));
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseRuntimeExceptionClass::tRisseRuntimeExceptionClass() :
	tRisseClassBase(tRisseExceptionClass::GetPointer())
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
		tRisseRuntimeExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tRisseCompileExceptionClass::tRisseCompileExceptionClass() :
	tRisseClassBase(tRisseRuntimeExceptionClass::GetPointer())
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
		tRisseCompileExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompileExceptionClass::Throw(const tRisseString & reason, const tRisseScriptBlockBase * sb, risse_size pos)
{
	// 例外インスタンスを生成
	tRisseVariant * e = new tRisseVariant(
		tRisseVariant(tRisseCompileExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				tRisseString(RISSE_WS_TR("compile error: %1"), reason)
					)));

	// 例外位置情報を追加してやる
	e->AddTrace(sb, pos);

	// 例外を投げる
	throw e;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseInstantiationExceptionClass::tRisseInstantiationExceptionClass() :
	tRisseClassBase(tRisseRuntimeExceptionClass::GetPointer())
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
		tRisseInstantiationExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromNonClassObject()
{
	throw new tRisseVariant(
		tRisseVariant(tRisseInstantiationExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				tRisseString(RISSE_WS_TR("cannot create instance from non-class object"))
					)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass()
{
	throw new tRisseVariant(
		tRisseVariant(tRisseInstantiationExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				tRisseString(RISSE_WS_TR("cannot create instance from this class"))
					)));
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tRisseBadContextExceptionClass::tRisseBadContextExceptionClass() :
	tRisseClassBase(tRisseRuntimeExceptionClass::GetPointer())
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
		tRisseBadContextExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadContextExceptionClass::Throw()
{
	throw new tRisseVariant(
		tRisseVariant(tRisseBadContextExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				tRisseString(RISSE_WS_TR("given context is not compatible with this method/property"))
					)));
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseUnsupportedOperationExceptionClass::tRisseUnsupportedOperationExceptionClass() :
	tRisseClassBase(tRisseRuntimeExceptionClass::GetPointer())
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
		tRisseUnsupportedOperationExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseUnsupportedOperationExceptionClass::ThrowCannotCallNonFunctionObjectException()
{
	throw new tRisseVariant(
		tRisseVariant(tRisseUnsupportedOperationExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				tRisseString(RISSE_WS_TR("cannot call non-function object"))
					)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented()
{
	throw new tRisseVariant(
		tRisseVariant(tRisseUnsupportedOperationExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				tRisseString(RISSE_WS_TR("operation is not implemented"))
					)));
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseArgumentExceptionClass::tRisseArgumentExceptionClass() :
	tRisseClassBase(tRisseExceptionClass::GetPointer())
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
		tRisseArgumentExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseIllegalArgumentExceptionClass::tRisseIllegalArgumentExceptionClass() :
	tRisseClassBase(tRisseArgumentExceptionClass::GetPointer())
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
		tRisseIllegalArgumentExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tRisseNullObjectExceptionClass::tRisseNullObjectExceptionClass() :
	tRisseClassBase(tRisseIllegalArgumentExceptionClass::GetPointer())
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
		tRisseNullObjectExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNullObjectExceptionClass::Throw()
{
	throw new tRisseVariant(
		tRisseVariant(tRisseNullObjectExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				tRisseString(RISSE_WS_TR("null object was given"))
					)));
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tRisseBadArgumentCountExceptionClass::tRisseBadArgumentCountExceptionClass() :
	tRisseClassBase(tRisseArgumentExceptionClass::GetPointer())
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
		tRisseBadArgumentCountExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadArgumentCountExceptionClass::ThrowNormal(risse_size passed, risse_size expected)
{
	throw new tRisseVariant(
		tRisseVariant(tRisseBadArgumentCountExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				tRisseString(RISSE_WS_TR("bad argument count (%1 given, but %2 expected)"),
					tRisseString::AsString((risse_int64)passed),
					tRisseString::AsString((risse_int64)expected))
					)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBadArgumentCountExceptionClass::ThrowBlock(risse_size passed, risse_size expected)
{
	throw new tRisseVariant(
		tRisseVariant(tRisseBadArgumentCountExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				tRisseString(RISSE_WS_TR("bad block argument count (%1 given, but %2 expected)"),
					tRisseString::AsString((risse_int64)passed),
					tRisseString::AsString((risse_int64)expected))
					)));
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisseMemberAccessExceptionClass::tRisseMemberAccessExceptionClass() :
	tRisseClassBase(tRisseRuntimeExceptionClass::GetPointer())
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
		This.SetPropertyDirect(ss_name, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はargs[0])
		tRisseMemberAccessExceptionClass::GetPointer()->CallSuperClassMethod(NULL,
			ss_initialize, 0, tRisseMethodArgument::New(args[0]), This);

		// メンバを設定する
		if(args.HasArgument(1))
			This.SetPropertyDirect(ss_name, 0, args[1], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisseNoSuchMemberExceptionClass::tRisseNoSuchMemberExceptionClass() :
	tRisseClassBase(tRisseMemberAccessExceptionClass::GetPointer())
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
		tRisseNoSuchMemberExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNoSuchMemberExceptionClass::Throw(const tRisseString & name)
{
	throw new tRisseVariant(
		tRisseVariant(tRisseNoSuchMemberExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				name.IsEmpty() ?
					tRisseString(RISSE_WS_TR("member not found"), name):
					tRisseString(RISSE_WS_TR("member \"%1\" not found"), name),
				name)));
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisseIllegalMemberAccessExceptionClass::tRisseIllegalMemberAccessExceptionClass() :
	tRisseClassBase(tRisseMemberAccessExceptionClass::GetPointer())
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
		tRisseIllegalMemberAccessExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalMemberAccessExceptionClass::ThrowMemberIsReadOnly(const tRisseString & name)
{
	throw new tRisseVariant(
		tRisseVariant(tRisseIllegalMemberAccessExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				name.IsEmpty() ?
					tRisseString(RISSE_WS_TR("member is read-only"), name):
					tRisseString(RISSE_WS_TR("member \"%1\" is read-only"), name),
				name)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalMemberAccessExceptionClass::ThrowPropertyCannotBeRead(const tRisseString & name)
{
	throw new tRisseVariant(
		tRisseVariant(tRisseIllegalMemberAccessExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				name.IsEmpty() ?
					tRisseString(RISSE_WS_TR("property cannot be read"), name):
					tRisseString(RISSE_WS_TR("property \"%1\" cannot be read"), name),
				name)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIllegalMemberAccessExceptionClass::ThrowPropertyCannotBeWritten(const tRisseString & name)
{
	throw new tRisseVariant(
		tRisseVariant(tRisseIllegalMemberAccessExceptionClass::GetPointer()).
			New(0,
				tRisseMethodArgument::New(
				name.IsEmpty() ?
					tRisseString(RISSE_WS_TR("property cannot be written"), name):
					tRisseString(RISSE_WS_TR("property \"%1\" cannot be written"), name),
				name)));
}
//---------------------------------------------------------------------------





} // namespace Risse
