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
#include "risseArrayClass.h"
#include "risseStringClass.h"
#include "risseIntegerClass.h"

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
	tRisseVariant Object_class(tRisseObjectClass::GetPointer());

	// グローバルオブジェクトを "Object" クラスから作成する
	GlobalObject = Object_class.New();

	// グローバルオブジェクトに "Object" クラスを登録する
	GlobalObject.SetPropertyDirect(ss_Object,
		tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcConst)) |
		tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
				Object_class);

	// "Array" クラスを作成する
	tRisseVariant Array_class(tRisseArrayClass::GetPointer());

	// グローバルオブジェクトに "Array" クラスを登録する
	GlobalObject.SetPropertyDirect(ss_Array,
		tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcConst)) |
		tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
				Array_class);

	// "Primitive" クラスを作成する
	tRisseVariant Primitive_class(tRissePrimitiveClass::GetPointer());

	// グローバルオブジェクトに "Primitive" クラスを登録する
	GlobalObject.SetPropertyDirect(ss_Primitive,
		tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcConst)) |
		tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
				Primitive_class);

	// "String" クラスを作成する
	tRisseVariant String_class(tRisseStringClass::GetPointer());

	// グローバルオブジェクトに "String" クラスを登録する
	GlobalObject.SetPropertyDirect(ss_String,
		tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcConst)) |
		tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
				String_class);

	// "Integer" クラスを作成する
	tRisseVariant Integer_class(tRisseIntegerClass::GetPointer());

	// グローバルオブジェクトに "Integer" クラスを登録する
	GlobalObject.SetPropertyDirect(ss_Integer,
		tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcConst)) |
		tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
				Integer_class);

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



