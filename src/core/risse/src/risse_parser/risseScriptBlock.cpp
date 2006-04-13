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

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(11177,27394,4689,17315,52629,65112,64194,51762);

//---------------------------------------------------------------------------
tRisseScriptBlock::tRisseScriptBlock(const tRisseString & script, risse_size lineofs) :
	tRisseScriptBlockBase(script, lineofs)
{
	;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlock::Evaluate(tRisseVariant * result, bool is_expression)
{
	// まず、コンパイルを行う
	// (TODO: スクリプトブロックのキャッシュ対策)
	Compile(result != NULL, is_expression);
}
//---------------------------------------------------------------------------


}
#define RISSE_DEFINE_TOKEN_STRING
#undef RisseParserH
#include "risseParser.h" // テスト用
namespace Risse {



//---------------------------------------------------------------------------
void tRisseScriptBlock::Compile(bool need_result, bool is_expression)
{
	// Lexer を準備する
	tRisseLexer lexer(GetScript());

	// -- test
	for(;;)
	{
		tRisseVariant val;
		int token = lexer.GetToken(val);
		if(token == 0)
		{
			printf("Unknown token");
			break;
		}
		if(token == -1) break; // トークンの最後
		printf("%s ", RisseLexerTokenString[token]);
		switch(token)
		{
		case T_CONSTVAL:
			tRisseString str = val.AsHumanReadable();
			wxPrintf(wxT("[%s] "), str.AsWxString().c_str());
			break;
		}
	}
}
//---------------------------------------------------------------------------


} // namespace Risse



