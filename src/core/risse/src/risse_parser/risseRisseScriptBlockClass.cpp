//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse スクリプトブロックの実装
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseRisseScriptBlockClass.h"
#include "risseLexer.h"
#include "risseParser.h"
#include "../risseScriptEngine.h"
#include "../risseClass.h"
#include "../risseStaticStrings.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(11177,27394,4689,17315,52629,65112,64194,51762);

//---------------------------------------------------------------------------
tRisseRisseScriptBlockInstance::tRisseRisseScriptBlockInstance()
{
	;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseASTNode * tRisseRisseScriptBlockInstance::GetASTRootNode(bool need_result, bool is_expression)
{
	// Lexer を準備する
	tRisseLexer *lexer = new tRisseLexer(this);

	// Parser を準備する
	// パースする
	tRisseParser *parser = new tRisseParser(this, lexer);

	return parser->GetRoot();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseRisseScriptBlockInstance::construct()
{
	// 特に何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseRisseScriptBlockInstance::initialize(
	const tRisseString &script, const tRisseString & name, risse_size lineofs,
	const tRisseNativeCallInfo &info)
{
	// 親クラスの同名メソッドを呼び出す
	// 引数はそのまま渡す
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisseRisseScriptBlockClass::tRisseRisseScriptBlockClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ScriptBlockClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseRisseScriptBlockClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_ovulate, &tRisseRisseScriptBlockClass::ovulate);
	RisseBindFunction(this, ss_construct, &tRisseRisseScriptBlockInstance::construct);
	RisseBindFunction(this, ss_initialize, &tRisseRisseScriptBlockInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseRisseScriptBlockClass::ovulate()
{
	return tRisseVariant(new tRisseRisseScriptBlockInstance());
}
//---------------------------------------------------------------------------


} // namespace Risse



