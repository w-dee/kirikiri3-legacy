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
#include "risseException.h"
#include "risseExceptionClass.h"
#include "risseObjectClass.h"
#include "risseClass.h"
#include "risseStaticStrings.h"
#include "risseNativeFunction.h"
#include "risseArrayClass.h"

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
		This.SetPropertyDirect(ss_filename, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);

		// line メンバを追加 (デフォルトでは-1)
		This.SetPropertyDirect(ss_line, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant((risse_int64)-1), This);

		// function メンバを追加 (デフォルトではnull)
		This.SetPropertyDirect(ss_function, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
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
		line = This.GetPropertyDirect(ss_line);
		function = This.GetPropertyDirect(ss_function);

		tRisseString ret;
		if(filename.IsEmptyString())
			ret = filename.operator tRisseString() + RISSE_WC(':');
		else
			ret = RISSE_WS_TR("<unknown>");

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
		This.SetPropertyDirect(ss_message, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseString::GetEmptyString()), This);

		// trace メンバを追加 (デフォルトでは空配列)
		This.SetPropertyDirect(ss_trace, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant(tRisseVariant(tRisseArrayClass::GetPointer()).New()), This);

		// cause メンバを追加 (デフォルトではnull)
		This.SetPropertyDirect(ss_cause, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetNullObject(), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数は空)
		tRisseThrowableClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, tRisseMethodArgument::Empty(), This);

		// 引数 = メッセージ, cause
		if(args.HasArgument(0))
			This.SetPropertyDirect(ss_message, 0, args[0], This);
		if(args.HasArgument(1))
			This.SetPropertyDirect(ss_cause, 0, args[1], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_addTrace)
	{
		if(args.GetArgumentCount() < 1) RisseThrowBadArgumentCount(args.GetArgumentCount(), 1);

		// 引数 = SourcePoint クラスのインスタンス
		// TODO: インスタンスが SourcePoint クラスのインスタンスかどうかをチェック
		This.GetPropertyDirect(ss_trace, 0, This).FuncCall(NULL, ss_push, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		This.SetPropertyDirect(ss_identifier, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetNullObject(), This);

		// target メンバを追加 (デフォルトでは-1)
		This.SetPropertyDirect(ss_target, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant((risse_int64)-1), This);

		// value メンバを追加 (デフォルトではnull)
		This.SetPropertyDirect(ss_value, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetNullObject(), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す(引数はメッセージ)
		tRisseBlockExitExceptionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0,
			tRisseMethodArgument::New(RISSE_WS("break/return helper exception")), This);
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




} // namespace Risse
