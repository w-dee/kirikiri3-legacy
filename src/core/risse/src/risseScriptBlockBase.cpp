//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトブロック管理
//---------------------------------------------------------------------------

#include "prec.h"
#include "risseScriptBlockBase.h"
#include "risseException.h"
#include "risseCodeGen.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(10462,6972,23868,17748,24487,5141,43296,28534);


//---------------------------------------------------------------------------
tRisseScriptBlockBase::tRisseScriptBlockBase(const tRisseString & script,
						const tRisseString & name, risse_size lineofs)
{
	// フィールドの初期化
	LinesToPosition = NULL;
	Script = script;
	Name = name;
	LineOffset = lineofs;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockBase::CreateLinesToPositionArary() const
{
	// まず、全体の行数を数える
	LineCount = 0;

	const risse_char *script = Script.c_str();
	const risse_char *ls = script;
	const risse_char *p = ls;
	while(*p)
	{
		if(*p == RISSE_WC('\r') || *p == RISSE_WC('\n'))
		{
			LineCount ++;
			if(*p == RISSE_WC('\r') && p[1] == RISSE_WC('\n')) p++;
			p++;
			ls = p;
		}
		else
		{
			p++;
		}
	}

	if(p != ls)
	{
		LineCount ++;
	}

	// メモリを確保
	LinesToPosition = new (PointerFreeGC) risse_size[LineCount];

	// 各行の先頭のコードポイント位置を配列に入れていく
	ls = script;
	p = ls;
	risse_size i = 0;
	while(*p)
	{
		if(*p == RISSE_WC('\r') || *p == RISSE_WC('\n'))
		{
			LinesToPosition[i++] = p - script;
			if(*p == RISSE_WC('\r') && p[1] == RISSE_WC('\n')) p++;
			p++;
			ls = p;
		}
		else
		{
			p++;
		}
	}

	if(p != ls)
	{
		LinesToPosition[i++] = p - script;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockBase::PositionToLineAndColumn(risse_size pos,
						risse_size *line, risse_size *col) const
{
	// LinesToPosition 配列が作られていなければ作る
	EnsureLinesToPositionArary();

	// 目的の位置を binary search で探す
	risse_size s = 0;
	risse_size e = LineCount;
	while(true)
	{
		if(e-s <= 1)
		{
			if(line) *line = s + LineOffset; // LineOffsetが加算される
			if(col) *col = pos - LinesToPosition[s];
			return;
		}
		risse_uint m = s + (e-s)/2;
		if(LinesToPosition[m] > pos)
			e = m;
		else
			s = m;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockBase::Compile(tRisseASTNode * root, bool need_result, bool is_expression)
{
	// (テスト) ASTのダンプを行う
	RisseFPrint(stdout, RISSE_WS("---------- AST ----------\n"));
	tRisseString str;
	root->Dump(str);
	RisseFPrint(stdout, str.c_str());

	// (テスト)
	tRisseSSAForm * form = new tRisseSSAForm(this, root, RISSE_WS("root"));
	form->Generate();

	// SSA 形式のダンプ
	for(gc_vector<tRisseSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
	{
		RisseFPrint(stdout,(	RISSE_WS("---------- SSA (") + (*i)->GetName() +
								RISSE_WS(") ----------\n")).c_str());
		str = (*i)->Dump();
		RisseFPrint(stdout, str.c_str());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockBase::AddSSAForm(tRisseSSAForm * ssaform)
{
	SSAForms.push_back(ssaform);
}
//---------------------------------------------------------------------------


} // namespace Risse



