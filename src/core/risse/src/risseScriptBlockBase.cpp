//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトブロック管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseScriptBlockBase.h"
#include "compiler/risseCompiler.h"
#include "risseCodeBlock.h"
#include "risseScriptEngine.h"
#include "risseBindingInfo.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(10462,6972,23868,17748,24487,5141,43296,28534);


//---------------------------------------------------------------------------
tRisseScriptBlockBase::tRisseScriptBlockBase(tRisseScriptEngine * engine,
	const tRisseString & script, const tRisseString & name, risse_size lineofs)
{
	// フィールドの初期化
	ScriptEngine = engine;
	LinesToPosition = NULL;
	Script = script;
	Name = name;
	LineOffset = lineofs;
	RootCodeBlock = NULL;
	CodeBlocks = new gc_vector<tRisseCodeBlock *>();
	TryIdentifiers = new gc_vector<void *>();
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
			LinesToPosition[i++] = ls - script;
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
		LinesToPosition[i++] = ls - script;
	}

	RISSE_ASSERT(LineCount == i);
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
risse_size tRisseScriptBlockBase::PositionToLine(risse_size pos) const
{
	risse_size line;
	PositionToLineAndColumn(pos, &line, NULL);
	return line;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseScriptBlockBase::GetLineAt(risse_size line)
{
	if(line > LineCount) return tRisseString::GetEmptyString(); // 行が範囲外

	// 改行記号か文字列の終端を探し、そこまでを切り取って返す
	risse_size n, start;
	n = start = LinesToPosition[line];
	while(n < Script.GetLength() &&
		Script[n] != RISSE_WC('\n') &&
		Script[n] != RISSE_WC('\r')) n++;

	return tRisseString(Script, start, n - start);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseScriptBlockBase::BuildMessageAt(risse_size pos, const tRisseString & message)
{
	return tRisseString(RISSE_WS("%1 at %2:%3"),
		message, GetName(), tRisseString::AsString((risse_int64)(1 + PositionToLine(pos))));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockBase::OutputWarning(risse_size pos, const tRisseString & message)
{
	ScriptEngine->OutputWarning(BuildMessageAt(pos, message));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockBase::Compile(tRisseASTNode * root, bool need_result, bool is_expression)
{
	// コンパイラオブジェクトを作成してコンパイルを行う
	tRisseCompiler * compiler = new tRisseCompiler(this);
	compiler->Compile(root, need_result, is_expression);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseScriptBlockBase::AddCodeBlock(tRisseCodeBlock * codeblock)
{
	RISSE_ASSERT(CodeBlocks != NULL); // このメソッドが呼ばれるのは Fixup 以前でなければならない
	CodeBlocks->push_back(codeblock);
	return CodeBlocks->size() - 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseScriptBlockBase::AddTryIdentifier()
{
	RISSE_ASSERT(TryIdentifiers != NULL); // このメソッドが呼ばれるのは Fixup 以前でなければならない
	TryIdentifiers->push_back(reinterpret_cast<void*>(new int(0)));
		// 注意 int を new で確保し、そのアドレスを void にキャストして
		// TryIdentifiers に追加している。これにより、TryIdentifiers は独立した
		// ポインタをそれぞれが持つことになり、識別に使える。これは、
		//  * new で確保される値は常に独立したポインタの値になる
		//  * ポインタとしては vtObject のときに tRisseObjectInterface へのポインタを
		//    持つ頃ができる
		// という理由による。
		// tRisseString も新規確保した文字列は独立した値になるが
		// 同じ文字列インスタンスが常に同じポインタを持ち続けるかという保証がないので
		// (とはいっても現時点ではそれは保証されているが将来的に保証があるかわからないので)
		// つかわない。
	return TryIdentifiers->size() - 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseCodeBlock * tRisseScriptBlockBase::GetCodeBlockAt(risse_size index) const
{
	RISSE_ASSERT(CodeBlocks != NULL);
	RISSE_ASSERT(index < CodeBlocks->size());
	return (*CodeBlocks)[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tRisseScriptBlockBase::GetTryIdentifierAt(risse_size index) const
{
	RISSE_ASSERT(TryIdentifiers != NULL);
	RISSE_ASSERT(index < TryIdentifiers->size());
	return (*TryIdentifiers)[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockBase::Fixup()
{
	// すべてのコードブロックの Fixup() を呼ぶ
	for(gc_vector<tRisseCodeBlock *>::iterator i = CodeBlocks->begin();
		i != CodeBlocks->end(); i++)
		(*i)->Fixup();

	// CodeBlocks をクリアする
	CodeBlocks = NULL;
	// TryIdentifiers をクリアする
	TryIdentifiers = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockBase::Evaluate(const tRisseBindingInfo & binding, tRisseVariant * result, bool is_expression)
{
	// まず、コンパイルを行う
	// (TODO: スクリプトブロックのキャッシュ対策)

	// AST ノードを用意する
	tRisseASTNode * root_node = GetASTRootNode(result != NULL);

	// コンパイルする
	Compile(root_node, result != NULL, is_expression);

	// Fixup する
	Fixup();

	// テスト実行
	RisseFPrint(stderr,(RISSE_WS("========== Result ==========\n")));
	fflush(stderr);
	fflush(stdout);
	tRisseVariant ret;
	RISSE_ASSERT(RootCodeBlock != NULL);
	RootCodeBlock->GetObject().FuncCall(
				&ret, 0,
				tRisseMethodArgument::Empty(),
				binding.GetThis());
	RisseFPrint(stdout, ret.AsHumanReadable().c_str());
}
//---------------------------------------------------------------------------


} // namespace Risse



