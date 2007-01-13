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
#include "risseObjectClass.h"
#include "risseStaticStrings.h"
#include "risse_parser/risseScriptBlock.h"

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

	// "Object" クラスを作成する
	tRisseVariant Object_class(new tRisseObjectClass());

	// グローバルオブジェクトを "Object" クラスから作成する
	GlobalObject = Object_class.New();

	// グローバルオブジェクトに "Object" クラスを登録する
	GlobalObject.SetPropertyDirect(ss_Object, tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcVar)) |
			tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
				Object_class);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::Evaluate(const tRisseString & script, const tRisseString & name,
					risse_size lineofs,
					tRisseVariant * result, bool is_expression, const tRisseVariant & context)
{
	// 暫定実装
	// スクリプトブロックを作成
	tRisseScriptBlock block(script, name);

	// スクリプトをグローバルコンテキストで実行
	block.Evaluate(context.IsNull()?GlobalObject:context);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse



