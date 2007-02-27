//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトエンジン管理
//---------------------------------------------------------------------------

#include "prec.h"
#include "risseScriptEngine.h"
#include "risseCoroutine.h"
#include "risseStaticStrings.h"
#include "risse_parser/risseScriptBlock.h"

#include "risseObjectClass.h"
#include "risseModuleClass.h"
#include "risseClassClass.h"
#include "risseFunctionClass.h"
#include "rissePropertyClass.h"
#include "risseArrayClass.h"
#include "risseStringClass.h"
#include "rissePrimitiveClass.h"
#include "risseNumberClass.h"
#include "risseIntegerClass.h"
#include "risseRealClass.h"
#include "risseExceptionClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(4659,21296,13745,17188,45721,42457,47629,47295);


//---------------------------------------------------------------------------
bool tRisseScriptEngine::CommonObjectsInitialized = false;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseScriptEngine::tRisseScriptEngine()
{
	// 共通に初期化しなくてはならない部分は初期化されているか
	if(!CommonObjectsInitialized)
	{
		CommonObjectsInitialized = true;
		// 共通初期化
		GC_init();
		RisseInitCoroutine();
	}

	// グローバルオブジェクトを "Object" クラスから作成する
	GlobalObject = tRisseVariant(tRisseObjectClass::GetPointer()).New();

	// 各クラスをグローバルオブジェクトに登録する
	tRisseObjectClass::RegisterClassInstance(GlobalObject, ss_Object);
	tRisseModuleClass::RegisterClassInstance(GlobalObject, ss_Module);
	tRisseClassClass::RegisterClassInstance(GlobalObject, ss_Class);
	tRisseFunctionClass::RegisterClassInstance(GlobalObject, ss_Function);
	tRissePropertyClass::RegisterClassInstance(GlobalObject, ss_Property);
	tRisseArrayClass::RegisterClassInstance(GlobalObject, ss_Array);
	tRissePrimitiveClass::RegisterClassInstance(GlobalObject, ss_Primitive);
	tRisseStringClass::RegisterClassInstance(GlobalObject, ss_String);
	tRisseNumberClass::RegisterClassInstance(GlobalObject, ss_Number);
	tRisseIntegerClass::RegisterClassInstance(GlobalObject, ss_Integer);
	tRisseRealClass::RegisterClassInstance(GlobalObject, ss_Real);
	tRisseSourcePointClass::RegisterClassInstance(GlobalObject, ss_SourcePoint);
	tRisseThrowableClass::RegisterClassInstance(GlobalObject, ss_Throwable);
	tRisseErrorClass::RegisterClassInstance(GlobalObject, ss_Error);
	tRisseExceptionClass::RegisterClassInstance(GlobalObject, ss_Exception);
	tRisseIOExceptionClass::RegisterClassInstance(GlobalObject, ss_IOException);
	tRisseCharConversionExceptionClass::RegisterClassInstance(GlobalObject, ss_CharConversionException);
	tRisseRuntimeExceptionClass::RegisterClassInstance(GlobalObject, ss_RuntimeException);
	tRisseCompileExceptionClass::RegisterClassInstance(GlobalObject, ss_CompileException);
	tRisseInstantiationExceptionClass::RegisterClassInstance(GlobalObject, ss_InstantiationException);
	tRisseUnsupportedOperationExceptionClass::RegisterClassInstance(GlobalObject, ss_UnsupportedOperationException);
	tRisseBadContextExceptionClass::RegisterClassInstance(GlobalObject, ss_BadContextException);
	tRisseMemberAccessExceptionClass::RegisterClassInstance(GlobalObject, ss_MemberAccessException);
	tRisseNoSuchMemberExceptionClass::RegisterClassInstance(GlobalObject, ss_NoSuchMemberException);
	tRisseArgumentExceptionClass::RegisterClassInstance(GlobalObject, ss_ArgumentException);
	tRisseIllegalArgumentExceptionClass::RegisterClassInstance(GlobalObject, ss_IllegalArgumentException);
	tRisseNullObjectExceptionClass::RegisterClassInstance(GlobalObject, ss_NullObjectException);
	tRisseBadArgumentCountExceptionClass::RegisterClassInstance(GlobalObject, ss_BadArgumentCountException);
	tRisseIllegalMemberAccessExceptionClass::RegisterClassInstance(GlobalObject, ss_IllegalMemberAccessException);

	// 各クラスのメンバを正式な物に登録し直すためにもう一度RegisterMembersを呼ぶ
	// 上記の状態では メンバとして仮のものが登録されている可能性がある
	// (たとえばArray.modulesはArrayクラスの初期化が終了しないと
	//  登録されないし、各メソッドは Function クラスの初期化が終了しないと
	//  登録できない)。
	// このため、各クラスの RegisterMembers メソッドをもう一度呼び、メンバを
	// 登録し治す。上記ですべてクラスの初期化は終了しているため、
	// もう一度このメソッドを呼べば、正しくメソッドが登録されるはずである。
	tRisseObjectClass::GetPointer()->RegisterMembers();
	tRisseModuleClass::GetPointer()->RegisterMembers();
	tRisseClassClass::GetPointer()->RegisterMembers();
	tRisseFunctionClass::GetPointer()->RegisterMembers();
	tRissePropertyClass::GetPointer()->RegisterMembers();
	tRisseArrayClass::GetPointer()->RegisterMembers();
	tRissePrimitiveClass::GetPointer()->RegisterMembers();
	tRisseStringClass::GetPointer()->RegisterMembers();
	tRisseNumberClass::GetPointer()->RegisterMembers();
	tRisseIntegerClass::GetPointer()->RegisterMembers();
	tRisseRealClass::GetPointer()->RegisterMembers();
	tRisseSourcePointClass::GetPointer()->RegisterMembers();
	tRisseThrowableClass::GetPointer()->RegisterMembers();
	tRisseErrorClass::GetPointer()->RegisterMembers();
	tRisseExceptionClass::GetPointer()->RegisterMembers();
	tRisseIOExceptionClass::GetPointer()->RegisterMembers();
	tRisseCharConversionExceptionClass::GetPointer()->RegisterMembers();
	tRisseRuntimeExceptionClass::GetPointer()->RegisterMembers();
	tRisseCompileExceptionClass::GetPointer()->RegisterMembers();
	tRisseInstantiationExceptionClass::GetPointer()->RegisterMembers();
	tRisseUnsupportedOperationExceptionClass::GetPointer()->RegisterMembers();
	tRisseBadContextExceptionClass::GetPointer()->RegisterMembers();
	tRisseMemberAccessExceptionClass::GetPointer()->RegisterMembers();
	tRisseNoSuchMemberExceptionClass::GetPointer()->RegisterMembers();
	tRisseArgumentExceptionClass::GetPointer()->RegisterMembers();
	tRisseIllegalArgumentExceptionClass::GetPointer()->RegisterMembers();
	tRisseNullObjectExceptionClass::GetPointer()->RegisterMembers();
	tRisseBadArgumentCountExceptionClass::GetPointer()->RegisterMembers();
	tRisseIllegalMemberAccessExceptionClass::GetPointer()->RegisterMembers();

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::Evaluate(const tRisseString & script, const tRisseString & name,
					risse_size lineofs,
					tRisseVariant * result, bool is_expression, const tRisseVariant & context)
{
	// 暫定実装
	// スクリプトブロックを作成
	tRisseScriptBlock block(this, script, name);

	// スクリプトをグローバルコンテキストで実行
	block.Evaluate(context.IsNull()?GlobalObject:context);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse



