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
#include "rissePropertyClass.h"
#include "risseArrayClass.h"
#include "risseStringClass.h"
#include "rissePrimitiveClass.h"
#include "risseNumberClass.h"
#include "risseIntegerClass.h"
#include "risseRealClass.h"

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

	// "Array" クラスのシングルトンインスタンスを作成する
	// というのも、クラスの modules は配列であり、クラスを作る前に
	// "Array" クラスのシングルトンインスタンスができあがっていなくてはならない。
	// ちなみに Array.modules も配列なため Array の構築に必要であり、
	// 連鎖的に無限ループに陥るため、tRisseClassBase::RegisterModulesArray() では
	// Array クラスの構築が完了していない場合は modules 配列が登録されないように
	// なっている。つまり、Array.modules はこの時点では生成されない。
	tRisseArrayClass::GetPointer();

	// Array.modules を改めて登録してやる
	tRisseArrayClass::GetPointer()->RegisterModulesArray();

	// グローバルオブジェクトを "Object" クラスから作成する
	GlobalObject = tRisseVariant(tRisseObjectClass::GetPointer()).New();

	// 各クラスをグローバルオブジェクトに登録する
	tRisseObjectClass::RegisterClassInstance(GlobalObject, ss_Object);
	tRisseModuleClass::RegisterClassInstance(GlobalObject, ss_Module);
	tRisseClassClass::RegisterClassInstance(GlobalObject, ss_Class);
	tRissePropertyClass::RegisterClassInstance(GlobalObject, ss_Property);
	tRisseArrayClass::RegisterClassInstance(GlobalObject, ss_Array);
	tRissePrimitiveClass::RegisterClassInstance(GlobalObject, ss_Primitive);
	tRisseStringClass::RegisterClassInstance(GlobalObject, ss_String);
	tRisseNumberClass::RegisterClassInstance(GlobalObject, ss_Number);
	tRisseIntegerClass::RegisterClassInstance(GlobalObject, ss_Integer);
	tRisseRealClass::RegisterClassInstance(GlobalObject, ss_Real);
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



