//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
tRisseScriptBlock::tRisseScriptBlock(const tRisseString & script,
	const tRisseString & name, risse_size lineofs) :
		tRisseScriptBlockBase(script, name, lineofs)
{
	;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlock::Evaluate(tRisseVariant * result, bool is_expression)
{
	// まず、コンパイルを行う
	// (TODO: スクリプトブロックのキャッシュ対策)

	// Lexer を準備する
	tRisseLexer *lexer = new tRisseLexer(GetScript());

	// Parser を準備する
	// パースする
	tRisseParser *parser = new tRisseParser(this, lexer);

	// コンパイルする
	Compile(parser->GetRoot(), result != NULL, is_expression);
}
//---------------------------------------------------------------------------


} // namespace Risse



