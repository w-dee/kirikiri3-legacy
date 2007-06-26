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
#include "risseScriptBlockClass.h"
#include "compiler/risseCompiler.h"
#include "risseCodeBlock.h"
#include "risseScriptEngine.h"
#include "risseBindingInfo.h"
#include "risseCodeExecutor.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseExceptionClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(10462,6972,23868,17748,24487,5141,43296,28534);


//---------------------------------------------------------------------------
tRisseScriptBlockInstance::tRisseScriptBlockInstance()
{
	// フィールドの初期化
	LinesToPosition = NULL;
	LineOffset = 0;
	RootCodeBlock = NULL;
	CodeBlocks = new gc_vector<tRisseCodeBlock *>();
	TryIdentifiers = new gc_vector<void *>();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockInstance::SetScriptAndName(const tRisseString & script, const tRisseString & name, int lineofs)
{
	Script = script;
	Name = name;
	LineOffset = lineofs;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockInstance::CreateLinesToPositionArary() const
{
	volatile tSynchronizer sync(this); // sync

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
void tRisseScriptBlockInstance::PositionToLineAndColumn(risse_size pos,
						risse_size *line, risse_size *col) const
{
	volatile tSynchronizer sync(this); // sync

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
risse_size tRisseScriptBlockInstance::PositionToLine(risse_size pos) const
{
	risse_size line;
	PositionToLineAndColumn(pos, &line, NULL);
	return line;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseScriptBlockInstance::GetLineAt(risse_size line)
{
	volatile tSynchronizer sync(this); // sync

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
tRisseString tRisseScriptBlockInstance::BuildMessageAt(risse_size pos, const tRisseString & message)
{
	return tRisseString(RISSE_WS("%1 at %2:%3"),
		message, GetName(), tRisseString::AsString((risse_int64)(1 + PositionToLine(pos))));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockInstance::OutputWarning(risse_size pos, const tRisseString & message)
{
	GetRTTI()->GetScriptEngine()->OutputWarning(BuildMessageAt(pos, message));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockInstance::Compile(tRisseASTNode * root, const tRisseBindingInfo & binding, bool need_result, bool is_expression)
{
	volatile tSynchronizer sync(this); // sync

	// コンパイラオブジェクトを作成してコンパイルを行う
	tRisseCompiler * compiler = new tRisseCompiler(this);
	compiler->Compile(root, binding, need_result, is_expression);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseScriptBlockInstance::AddCodeBlock(tRisseCodeBlock * codeblock)
{
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(CodeBlocks != NULL); // このメソッドが呼ばれるのは Fixup 以前でなければならない
	CodeBlocks->push_back(codeblock);
	return CodeBlocks->size() - 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseScriptBlockInstance::AddTryIdentifier()
{
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(TryIdentifiers != NULL); // このメソッドが呼ばれるのは Fixup 以前でなければならない
	TryIdentifiers->push_back(reinterpret_cast<void*>(new (GC) int(0)));
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
tRisseCodeBlock * tRisseScriptBlockInstance::GetCodeBlockAt(risse_size index) const
{
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(CodeBlocks != NULL);
	RISSE_ASSERT(index < CodeBlocks->size());
	return (*CodeBlocks)[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tRisseScriptBlockInstance::GetTryIdentifierAt(risse_size index) const
{
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(TryIdentifiers != NULL);
	RISSE_ASSERT(index < TryIdentifiers->size());
	return (*TryIdentifiers)[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockInstance::Fixup()
{
	volatile tSynchronizer sync(this); // sync

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
void tRisseScriptBlockInstance::Evaluate(const tRisseBindingInfo & binding, tRisseVariant * result, bool is_expression)
{
	tRisseCodeExecutor *executor;

	{
		volatile tSynchronizer sync(this); // sync

		// まず、コンパイルを行う
		// (TODO: スクリプトブロックのキャッシュ対策)

		// AST ノードを用意する
		tRisseASTNode * root_node = GetASTRootNode(result != NULL);

		// コンパイルする
		Compile(root_node, binding, result != NULL, is_expression);

		// Fixup する
		Fixup();

		// executor を得る
		executor = RootCodeBlock->GetExecutor();
	}

	// テスト実行
	RISSE_ASSERT(RootCodeBlock != NULL);
	executor->Execute(
				tRisseMethodArgument::Empty(),
				binding.GetThis(),
				NULL,
				binding.GetFrames(),
				result);
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
	const tRisseString &script, const tRisseString & name, risse_size lineofs,
	const tRisseNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// 引数を元に設定を行う
	SetScriptAndName(script, name, lineofs);
}
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
tRisseScriptBlockClass::tRisseScriptBlockClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_ovulate, &tRisseScriptBlockClass::ovulate);
	RisseBindFunction(this, ss_construct, &tRisseScriptBlockInstance::construct);
	RisseBindFunction(this, ss_initialize, &tRisseScriptBlockInstance::initialize);
	RisseBindFunction(this, mnString, &tRisseScriptBlockInstance::mnString);
	RisseBindProperty(this, ss_script, &tRisseScriptBlockInstance::get_script);
	RisseBindProperty(this, ss_name, &tRisseScriptBlockInstance::get_name);
	RisseBindFunction(this, ss_getLineAt, &tRisseScriptBlockInstance::getLineAt);
	RisseBindFunction(this, ss_positionToLine, &tRisseScriptBlockInstance::positionToLine);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseScriptBlockClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tRisseVariant();
}
//---------------------------------------------------------------------------


} // namespace Risse



