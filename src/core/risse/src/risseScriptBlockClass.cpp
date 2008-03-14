//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
tScriptBlockInstance::tScriptBlockInstance()
{
	// フィールドの初期化
	LinesToPosition = NULL;
	LineOffset = 0;
	RootCodeBlock = NULL;
	CodeBlocks = new gc_vector<tCodeBlock *>();
	TryIdentifiers = new gc_vector<void *>();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptBlockInstance::SetScriptAndName(const tString & script, const tString & name, int lineofs)
{
	Script = script;
	Name = name;
	LineOffset = lineofs;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptBlockInstance::CreateLinesToPositionArary() const
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
void tScriptBlockInstance::PositionToLineAndColumn(risse_size pos,
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
risse_size tScriptBlockInstance::PositionToLine(risse_size pos) const
{
	risse_size line;
	PositionToLineAndColumn(pos, &line, NULL);
	return line;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tScriptBlockInstance::GetLineAt(risse_size line)
{
	volatile tSynchronizer sync(this); // sync

	if(line > LineCount) return tString::GetEmptyString(); // 行が範囲外

	// 改行記号か文字列の終端を探し、そこまでを切り取って返す
	risse_size n, start;
	n = start = LinesToPosition[line];
	while(n < Script.GetLength() &&
		Script[n] != RISSE_WC('\n') &&
		Script[n] != RISSE_WC('\r')) n++;

	return tString(Script, start, n - start);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tScriptBlockInstance::BuildMessageAt(risse_size pos, const tString & message)
{
	return tString(RISSE_WS("%1 at %2:%3"),
		message, GetName(), tString::AsString((risse_int64)(1 + PositionToLine(pos))));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptBlockInstance::OutputWarning(risse_size pos, const tString & message)
{
	GetRTTI()->GetScriptEngine()->OutputWarning(BuildMessageAt(pos, message));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptBlockInstance::Compile(tASTNode * root, const tBindingInfo & binding, bool need_result, bool is_expression)
{
	volatile tSynchronizer sync(this); // sync

	// コンパイラオブジェクトを作成してコンパイルを行う
	tCompiler * compiler = new tCompiler(this);
	compiler->Compile(root, binding, need_result, is_expression);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tScriptBlockInstance::AddCodeBlock(tCodeBlock * codeblock)
{
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(CodeBlocks != NULL); // このメソッドが呼ばれるのは Fixup 以前でなければならない
	CodeBlocks->push_back(codeblock);
	return CodeBlocks->size() - 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tScriptBlockInstance::AddTryIdentifier()
{
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(TryIdentifiers != NULL); // このメソッドが呼ばれるのは Fixup 以前でなければならない
	TryIdentifiers->push_back(static_cast<void*>(new (GC) int(0)));
		// 注意 int を new で確保し、そのアドレスを void にキャストして
		// TryIdentifiers に追加している。これにより、TryIdentifiers は独立した
		// ポインタをそれぞれが持つことになり、識別に使える。これは、
		//  * new で確保される値は常に独立したポインタの値になる
		//  * ポインタとしては vtObject のときに tObjectInterface へのポインタを
		//    持つ頃ができる
		// という理由による。
		// tString も新規確保した文字列は独立した値になるが
		// 同じ文字列インスタンスが常に同じポインタを持ち続けるかという保証がないので
		// (とはいっても現時点ではそれは保証されているが将来的に保証があるかわからないので)
		// つかわない。
	return TryIdentifiers->size() - 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tCodeBlock * tScriptBlockInstance::GetCodeBlockAt(risse_size index) const
{
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(CodeBlocks != NULL);
	RISSE_ASSERT(index < CodeBlocks->size());
	return (*CodeBlocks)[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tScriptBlockInstance::GetTryIdentifierAt(risse_size index) const
{
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(TryIdentifiers != NULL);
	RISSE_ASSERT(index < TryIdentifiers->size());
	return (*TryIdentifiers)[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptBlockInstance::Fixup()
{
	volatile tSynchronizer sync(this); // sync

	// すべてのコードブロックの Fixup() を呼ぶ
	for(gc_vector<tCodeBlock *>::iterator i = CodeBlocks->begin();
		i != CodeBlocks->end(); i++)
		(*i)->Fixup();

	// CodeBlocks をクリアする
	CodeBlocks = NULL;
	// TryIdentifiers をクリアする
	TryIdentifiers = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptBlockInstance::Evaluate(const tBindingInfo & binding, tVariant * result, bool is_expression)
{
	tCodeExecutor *executor;

	{
		volatile tSynchronizer sync(this); // sync

		// まず、コンパイルを行う
		// (TODO: スクリプトブロックのキャッシュ対策)

		// AST ノードを用意する
		tASTNode * root_node = GetASTRootNode(result != NULL);

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
				tMethodArgument::Empty(),
				binding.GetThis(),
				NULL,
				binding.GetFrames(),
				result);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptBlockInstance::construct()
{
	// 特に何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptBlockInstance::initialize(
	const tString &script, const tString & name, risse_size lineofs,
	const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// 引数を元に設定を行う
	SetScriptAndName(script, name, lineofs);
}
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
tScriptBlockClass::tScriptBlockClass(tScriptEngine * engine) :
	tClassBase(ss_ScriptBlock, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptBlockClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tScriptBlockClass::ovulate);
	BindFunction(this, ss_construct, &tScriptBlockInstance::construct);
	BindFunction(this, ss_initialize, &tScriptBlockInstance::initialize);
	BindFunction(this, mnString, &tScriptBlockInstance::mnString);
	BindProperty(this, ss_script, &tScriptBlockInstance::get_script);
	BindProperty(this, ss_name, &tScriptBlockInstance::get_name);
	BindFunction(this, ss_getLineAt, &tScriptBlockInstance::getLineAt);
	BindFunction(this, ss_positionToLine, &tScriptBlockInstance::positionToLine);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tScriptBlockClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------


} // namespace Risse



