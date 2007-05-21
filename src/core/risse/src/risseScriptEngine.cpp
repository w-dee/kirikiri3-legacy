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
#include "risseCoroutineClass.h"
#include "risseExceptionClass.h"
#include "risseBindingClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(4659,21296,13745,17188,45721,42457,47629,47295);


//---------------------------------------------------------------------------
bool tRisseScriptEngine::CommonObjectsInitialized = false;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseScriptEngine::tRisseScriptEngine()
{
	// 各クラスのインスタンスをいったん NULL に設定
	// ここでは StartSentinel と EndSentinel の間が連続して各オブジェクトのインスタンス
	// へのポインタの領域として割り当たっていることを仮定する(構造体というかクラスのレイアウト
	// はそうなっている訳だしコンパイラもそう割り当てるはずだが...)
	EndSentinel = NULL;
	for(void ** p = &StartSentinel; p < &EndSentinel; p++) *p = NULL;

	// フィールドの初期化
	WarningOutput = NULL;

	// 共通に初期化しなくてはならない部分は初期化されているか
	if(!CommonObjectsInitialized)
	{
		CommonObjectsInitialized = true;
		// 共通初期化
		GC_init();
		RisseInitCoroutine();
	}

	// 各クラスのインスタンスを作成する
	ObjectClass                            = new tRisseObjectClass                        (this);
	BindingClass                           = new tRisseBindingClass                       (this);
	ModuleClass                            = new tRisseModuleClass                        (this);
	ClassClass                             = new tRisseClassClass                         (this);
	FunctionClass                          = new tRisseFunctionClass                      (this);
	PropertyClass                          = new tRissePropertyClass                      (this);
	ArrayClass                             = new tRisseArrayClass                         (this);
	PrimitiveClass                         = new tRissePrimitiveClass                     (this);
	StringClass                            = new tRisseStringClass                        (this);
	NumberClass                            = new tRisseNumberClass                        (this);
	IntegerClass                           = new tRisseIntegerClass                       (this);
	RealClass                              = new tRisseRealClass                          (this);
	CoroutineClass                         = new tRisseCoroutineClass                     (this);
	SourcePointClass                       = new tRisseSourcePointClass                   (this);
	ThrowableClass                         = new tRisseThrowableClass                     (this);
	ErrorClass                             = new tRisseErrorClass                         (this);
	AssertionErrorClass                    = new tRisseAssertionErrorClass                (this);
	ExceptionClass                         = new tRisseExceptionClass                     (this);
	IOExceptionClass                       = new tRisseIOExceptionClass                   (this);
	CharConversionExceptionClass           = new tRisseCharConversionExceptionClass       (this);
	RuntimeExceptionClass                  = new tRisseRuntimeExceptionClass              (this);
	CompileExceptionClass                  = new tRisseCompileExceptionClass              (this);
	ClassDefinitionExceptionClass          = new tRisseClassDefinitionExceptionClass      (this);
	InstantiationExceptionClass            = new tRisseInstantiationExceptionClass        (this);
	UnsupportedOperationExceptionClass     = new tRisseUnsupportedOperationExceptionClass (this);
	BadContextExceptionClass               = new tRisseBadContextExceptionClass           (this);
	MemberAccessExceptionClass             = new tRisseMemberAccessExceptionClass         (this);
	NoSuchMemberExceptionClass             = new tRisseNoSuchMemberExceptionClass         (this);
	ArgumentExceptionClass                 = new tRisseArgumentExceptionClass             (this);
	IllegalArgumentExceptionClass          = new tRisseIllegalArgumentExceptionClass      (this);
	NullObjectExceptionClass               = new tRisseNullObjectExceptionClass           (this);
	BadArgumentCountExceptionClass         = new tRisseBadArgumentCountExceptionClass     (this);
	IllegalMemberAccessExceptionClass      = new tRisseIllegalMemberAccessExceptionClass  (this);

	BlockExitExceptionClass                = new tRisseBlockExitExceptionClass            (this);


	// グローバルオブジェクトを "Object" クラスから作成する
	GlobalObject = tRisseVariant(ObjectClass).New();

	// 各クラスをグローバルオブジェクトに登録する
	ObjectClass->RegisterClassInstance(GlobalObject, ss_Object);
	BindingClass->RegisterClassInstance(GlobalObject, ss_Binding);
	ModuleClass->RegisterClassInstance(GlobalObject, ss_Module);
	ClassClass->RegisterClassInstance(GlobalObject, ss_Class);
	FunctionClass->RegisterClassInstance(GlobalObject, ss_Function);
	PropertyClass->RegisterClassInstance(GlobalObject, ss_Property);
	ArrayClass->RegisterClassInstance(GlobalObject, ss_Array);
	PrimitiveClass->RegisterClassInstance(GlobalObject, ss_Primitive);
	StringClass->RegisterClassInstance(GlobalObject, ss_String);
	NumberClass->RegisterClassInstance(GlobalObject, ss_Number);
	IntegerClass->RegisterClassInstance(GlobalObject, ss_Integer);
	RealClass->RegisterClassInstance(GlobalObject, ss_Real);
	CoroutineClass->RegisterClassInstance(GlobalObject, ss_Coroutine);
	SourcePointClass->RegisterClassInstance(GlobalObject, ss_SourcePoint);
	ThrowableClass->RegisterClassInstance(GlobalObject, ss_Throwable);
	ErrorClass->RegisterClassInstance(GlobalObject, ss_Error);
	AssertionErrorClass->RegisterClassInstance(GlobalObject, ss_AssertionError);
	ExceptionClass->RegisterClassInstance(GlobalObject, ss_Exception);
	IOExceptionClass->RegisterClassInstance(GlobalObject, ss_IOException);
	CharConversionExceptionClass->RegisterClassInstance(GlobalObject, ss_CharConversionException);
	RuntimeExceptionClass->RegisterClassInstance(GlobalObject, ss_RuntimeException);
	CompileExceptionClass->RegisterClassInstance(GlobalObject, ss_CompileException);
	ClassDefinitionExceptionClass->RegisterClassInstance(GlobalObject, ss_ClassDefinitionException);
	InstantiationExceptionClass->RegisterClassInstance(GlobalObject, ss_InstantiationException);
	UnsupportedOperationExceptionClass->RegisterClassInstance(GlobalObject, ss_UnsupportedOperationException);
	BadContextExceptionClass->RegisterClassInstance(GlobalObject, ss_BadContextException);
	MemberAccessExceptionClass->RegisterClassInstance(GlobalObject, ss_MemberAccessException);
	NoSuchMemberExceptionClass->RegisterClassInstance(GlobalObject, ss_NoSuchMemberException);
	ArgumentExceptionClass->RegisterClassInstance(GlobalObject, ss_ArgumentException);
	IllegalArgumentExceptionClass->RegisterClassInstance(GlobalObject, ss_IllegalArgumentException);
	NullObjectExceptionClass->RegisterClassInstance(GlobalObject, ss_NullObjectException);
	BadArgumentCountExceptionClass->RegisterClassInstance(GlobalObject, ss_BadArgumentCountException);
	IllegalMemberAccessExceptionClass->RegisterClassInstance(GlobalObject, ss_IllegalMemberAccessException);

	// 各クラスのメンバを正式な物に登録し直すためにもう一度RegisterMembersを呼ぶ
	// 上記の状態では メンバとして仮のものが登録されている可能性がある
	// (たとえばArray.modulesはArrayクラスの初期化が終了しないと
	//  登録されないし、各メソッドは Function クラスの初期化が終了しないと
	//  登録できない)。
	// このため、各クラスの RegisterMembers メソッドをもう一度呼び、メンバを
	// 登録し治す。上記ですべてクラスの初期化は終了しているため、
	// もう一度このメソッドを呼べば、正しくメソッドが登録されるはずである。
	ObjectClass->RegisterMembers();
	BindingClass->RegisterMembers();
	ModuleClass->RegisterMembers();
	ClassClass->RegisterMembers();
	FunctionClass->RegisterMembers();
	PropertyClass->RegisterMembers();
	ArrayClass->RegisterMembers();
	PrimitiveClass->RegisterMembers();
	StringClass->RegisterMembers();
	NumberClass->RegisterMembers();
	IntegerClass->RegisterMembers();
	RealClass->RegisterMembers();
	CoroutineClass->RegisterMembers();
	SourcePointClass->RegisterMembers();
	ThrowableClass->RegisterMembers();
	ErrorClass->RegisterMembers();
	AssertionErrorClass->RegisterMembers();
	ExceptionClass->RegisterMembers();
	IOExceptionClass->RegisterMembers();
	CharConversionExceptionClass->RegisterMembers();
	RuntimeExceptionClass->RegisterMembers();
	CompileExceptionClass->RegisterMembers();
	ClassDefinitionExceptionClass->RegisterMembers();
	InstantiationExceptionClass->RegisterMembers();
	UnsupportedOperationExceptionClass->RegisterMembers();
	BadContextExceptionClass->RegisterMembers();
	MemberAccessExceptionClass->RegisterMembers();
	NoSuchMemberExceptionClass->RegisterMembers();
	ArgumentExceptionClass->RegisterMembers();
	IllegalArgumentExceptionClass->RegisterMembers();
	NullObjectExceptionClass->RegisterMembers();
	BadArgumentCountExceptionClass->RegisterMembers();
	IllegalMemberAccessExceptionClass->RegisterMembers();

	BlockExitExceptionClass->RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::Evaluate(const tRisseString & script, const tRisseString & name,
					risse_size lineofs,
					tRisseVariant * result, const tRisseBindingInfo * binding,
					bool is_expresion)
{
	try
	{
		// 暫定実装
		// スクリプトブロックを作成
		tRisseScriptBlock * block = new tRisseScriptBlock(this, script, name);

		// スクリプトをグローバルコンテキストで実行
		block->Evaluate(binding == NULL ? (tRisseBindingInfo(GlobalObject)) : *binding, result, is_expresion);
	}
	catch(const tRisseTemporaryException * te)
	{
		te->ThrowConverted(this);
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse



