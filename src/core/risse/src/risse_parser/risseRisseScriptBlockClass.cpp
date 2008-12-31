//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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
tRisseScriptBlockInstance::tRisseScriptBlockInstance()
{
	;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tASTNode * tRisseScriptBlockInstance::GetASTRootNode(bool need_result, bool is_expression)
{
	// Lexer を準備する
	tLexer *lexer = new tLexer(this);

	// Parser を準備する
	// パースする
	tParser *parser = new tParser(this, lexer);

	return parser->GetRoot();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockInstance::construct()
{
	// 特に何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockInstance::initialize(
	const tString &script, const tString & name, risse_size lineofs,
	const tNativeCallInfo &info)
{
	// 親クラスの同名メソッドを呼び出す
	// 引数はそのまま渡す
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tRisseScriptBlockClass, ss_RisseScriptBlock, engine->ScriptBlockClass)
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------

} // namespace Risse



