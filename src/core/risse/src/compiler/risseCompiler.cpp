//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンパイラ
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseCompiler.h"
#include "risseSSAForm.h"
#include "../risseException.h"
#include "../risseScriptBlockBase.h"
#include "../risseCodeBlock.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(7695,16492,63400,17880,52365,22979,50413,3135);


//---------------------------------------------------------------------------
void tRisseCompiler::Compile(tRisseASTNode * root, bool need_result, bool is_expression)
{
	// (テスト) ASTのダンプを行う
	RisseFPrint(stdout, RISSE_WS("========== AST ==========\n"));
	tRisseString str;
	root->Dump(str);
	RisseFPrint(stdout, str.c_str());

	// (テスト)
	tRisseSSAForm * form = new tRisseSSAForm(this, RISSE_WS("root"), NULL, false);
	form->Generate(root);

	// SSA 形式のダンプ
	for(gc_vector<tRisseSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
	{
		RisseFPrint(stdout,(	RISSE_WS("========== SSA (") + (*i)->GetName() +
								RISSE_WS(") ==========\n")).c_str());
		str = (*i)->Dump();
		RisseFPrint(stdout, str.c_str());
	}

	// コードジェネレータの生成
	for(gc_vector<tRisseSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
		(*i)->EnsureCodeGenerator();

	// VMコードの生成
	SSAForms.front()->GenerateCode();

	// VMコードのダンプ
	for(gc_vector<tRisseSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
	{
		RisseFPrint(stdout,(	RISSE_WS("========== VM (") + (*i)->GetName() +
								RISSE_WS(") ==========\n")).c_str());
		tRisseCodeBlock * cb = (*i)->GetCodeBlock();
		str = cb->Dump();
		RisseFPrint(stdout, str.c_str());
	}

	// ルートのコードブロックを設定する
	ScriptBlock->SetRootCodeBlock(SSAForms.front()->GetCodeBlock());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompiler::AddSSAForm(tRisseSSAForm * ssaform)
{
	SSAForms.push_back(ssaform);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseCompiler::AddCodeBlock(tRisseCodeBlock * block)
{
	// コードブロックの管理はスクリプトブロックが行っている
	return ScriptBlock->AddCodeBlock(block);
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risse
