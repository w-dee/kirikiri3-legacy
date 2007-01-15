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

#include "risseScriptBlock.h"
#include "risseLexer.h"
#include "risseParser.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(11177,27394,4689,17315,52629,65112,64194,51762);

//---------------------------------------------------------------------------
tRisseScriptBlock::tRisseScriptBlock(tRisseScriptEngine * engine,
	const tRisseString & script, const tRisseString & name, risse_size lineofs) :
		tRisseScriptBlockBase(engine, script, name, lineofs)
{
	;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseASTNode * tRisseScriptBlock::GetASTRootNode(bool need_result, bool is_expression)
{
	// Lexer を準備する
	tRisseLexer *lexer = new tRisseLexer(GetScript());

	// Parser を準備する
	// パースする
	tRisseParser *parser = new tRisseParser(this, lexer);

	return parser->GetRoot();
}
//---------------------------------------------------------------------------


} // namespace Risse



