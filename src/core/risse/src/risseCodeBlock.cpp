//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コードブロック
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseCodeBlock.h"
#include "risseScriptBlockClass.h"
#include "compiler/risseCodeGen.h"
#include "risseCodeExecutor.h"

namespace Risse
{
//---------------------------------------------------------------------------
RISSE_DEFINE_SOURCE_ID(13739,40903,3219,19310,50086,28697,53693,30185);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tCodeBlock::tCodeBlock(tScriptBlockInstance * sb)
{
	ScriptBlockInstance = sb;
	Code = NULL;
	CodeSize = 0;
	Consts = NULL;
	ConstsSize = 0;
	NumRegs = 0;
	NumSharedVars = 0;
	NestLevel = 0;
	SharedVariableNestCount = risse_size_max;
	CodeBlockRelocationSize = 0;
	TryIdentifierRelocations = NULL;
	TryIdentifierRelocationSize = 0;

	Executor = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * CodeToSourcePosition をソートするための比較関数
 */
struct tCodeToSourcePositionComparator
{
	bool operator () (const std::pair<risse_size, risse_size> & a, const std::pair<risse_size, risse_size> & b)
		{ return a.first < b.first; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeBlock::Assign(const tCodeGenerator *gen)
{
	// gen からいろいろ情報をコピー
	risse_size ind;

	// コードのコピー
	const gc_vector<risse_uint32> & gen_code = gen->GetCode();
	CodeSize = gen_code.size();
	Code = static_cast<risse_uint32*>(
		MallocAtomicCollectee(CodeSize * sizeof(risse_uint32)));
		// 注意
		// MallocAtomicCollectee を使って Code 領域を確保しているため、
		// Code 領域中にポインタを含ませるばあいは相応の対処をすること。
	ind = 0;
	for(gc_vector<risse_uint32>::const_iterator i = gen_code.begin();
		i != gen_code.end(); i++, ind++)
		Code[ind] = *i;

	// 定数領域のコピー
	const gc_vector<tVariant> & gen_consts = gen->GetConsts();
	ConstsSize = gen_consts.size();
	Consts = new tVariant[ConstsSize];
	ind = 0;
	for(gc_vector<tVariant>::const_iterator i = gen_consts.begin();
		i != gen_consts.end(); i++, ind++)
		Consts[ind] = *i;

	// CodeBlockRelocations のコピー
	const gc_vector<std::pair<risse_size, risse_size> > & cb_relocations =
			gen->GetCodeBlockRelocations();
	CodeBlockRelocations = new (GC) tRelocation[CodeBlockRelocationSize = cb_relocations.size()];
	ind = 0;
	for(gc_vector<std::pair<risse_size, risse_size> >::const_iterator i =
		cb_relocations.begin(); i != cb_relocations.end(); i++, ind++)
		CodeBlockRelocations[ind] = *i;

	// TryIdentifierRelocations のコピー
	const gc_vector<std::pair<risse_size, risse_size> > & ti_relocations =
			gen->GetTryIdentifierRelocations();
	TryIdentifierRelocations = new (GC) tRelocation[TryIdentifierRelocationSize = ti_relocations.size()];
	ind = 0;
	for(gc_vector<std::pair<risse_size, risse_size> >::const_iterator i =
		ti_relocations.begin(); i != ti_relocations.end(); i++, ind++)
		TryIdentifierRelocations[ind] = *i;

	// 必要なレジスタ数/共有変数の数のコピー
	NumRegs = gen->GetMaxNumUsedRegs();
	NestLevel = gen->GetNestLevel();
	NumSharedVars = gen->GetSharedRegCount();

	// CodeToSourcePosition のコピー
	const gc_vector<std::pair<risse_size, risse_size> > & cb_code_src = gen->GetCodeToSourcePosition();
	CodeToSourcePosition = new (GC) std::pair<risse_size, risse_size>[cb_code_src.size()];
	ind = 0;
	for(gc_vector<std::pair<risse_size, risse_size> >::const_iterator i = cb_code_src.begin();
		i != cb_code_src.end(); i++, ind++)
		CodeToSourcePosition[ind] = *i;
	CodeToSourcePositionSize = cb_code_src.size();

	//- 一応 CodeToSourcePosition はこの時点でバイトコード順に sort されているハズだが、そういう保証はないので
	//- stable_sort でソートし直す(あとで二分検索を行うため)
	std::sort(CodeToSourcePosition, CodeToSourcePosition + CodeToSourcePositionSize, tCodeToSourcePositionComparator());

	// Executor を作成
	Executor = new tCodeInterpreter(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeBlock::SetSharedVariableNestCount(risse_size level)
{
	SharedVariableNestCount = level;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeBlock::Fixup()
{
	RISSE_ASSERT(CodeBlockRelocations != NULL); // 二度以上このメソッドを呼べない
	RISSE_ASSERT(TryIdentifierRelocations != NULL); // 二度以上このメソッドを呼べない

	for(risse_size i = 0 ; i < CodeBlockRelocationSize; i++)
		Consts[CodeBlockRelocations[i].first] =
			ScriptBlockInstance->GetCodeBlockAt(CodeBlockRelocations[i].second)->GetObject(); // 再配置を行う

	for(risse_size i = 0 ; i < TryIdentifierRelocationSize; i++)
	{
		Consts[TryIdentifierRelocations[i].first].Clear();
		Consts[TryIdentifierRelocations[i].first] =
			static_cast<tObjectInterface *>
			(ScriptBlockInstance->GetTryIdentifierAt(TryIdentifierRelocations[i].second));
				// 再配置を行う。GetTryIdentifierAt の戻りは void * で
				// そのポインタは tObjectInterface ではないが、
				// 実装上の都合 void * を tObjectInterface に
				// static_cast して使う。このポインタは識別子代わりに
				// 使う物で、このポインタの指す先に実際にアクセスを行うような
				// ことは行わないのでこれでよいが、注意のこと。
	}

	CodeBlockRelocations = NULL; // もう再配置は行った
	TryIdentifierRelocations = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tCodeBlock::GetObject()
{
	return tVariant(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tCodeBlock::Dump() const
{
	static const risse_size display_line_back_count = 3; // この行数文前から表示する
	static const risse_size skip_count = 5; // この行数以上、前の行と離れる場合は中間をスキップする

	tString ret;
	tVMCodeIterator iterator(Code, 0);
	risse_size last_line = risse_size_max; // 最後に表示した行番号
	risse_size max_last_line = 0; // 最後に表示した行番号のうち、もっとも大きいもの
	while((const risse_uint32*)iterator != CodeSize + Code)
	{
		risse_size address = iterator.GetAddress();

		// アドレスからスクリプトの該当の行を計算する
		risse_size line = 0;
		ScriptBlockInstance->PositionToLineAndColumn(CodePositionToSourcePosition(address), &line, NULL);

		// 表示すべき行を決定する
		risse_size start; // 表示を開始する行番号
		risse_size count; // 表示する行数

		if(last_line == risse_size_max)
		{
			// ループの初回の場合
			// 直前のdisplay_line_back_count行を表示する
			start = line > (display_line_back_count-1) ? (line - (display_line_back_count-1)) : 0;
		}
		else if(last_line < line)
		{
			// 最後に表示した行よりも現在の行があとの場合
			if(line <= max_last_line)
			{
				// すでにその行は表示したことがあるような場合
				start = line; // 現在の行のみを表示
			}
			else
			{
				// その行はまだ表示していない場合
				if(line - last_line >= skip_count)
				{
					// 最後に表示した行から離れすぎ
					// 直前のdisplay_line_back_count行を表示する
					start = line > (display_line_back_count-1) ? (line - (display_line_back_count-1)) : 0;
				}
				else
				{
					// そうでもなければ最後の行の次の行から表示
					start = last_line + 1;
				}
			}
		}
		else if(last_line == line)
		{
			// 前の行と今の行が一緒
			// 表示しなくていい
			start = risse_size_max;
		}
		else
		{
			// 最後に表示した行よりも現在の行が前
			// 単純に現在の行だけを表示
			start = line;
		}

		count = line - start + 1;
		last_line = line;
		if(max_last_line < last_line) max_last_line = last_line;

		// ソースコードを表示する
		if(start != risse_size_max)
		{
			while(count--)
			{
				char line_string[22];
				sprintf(line_string, "#(%ld) ", (long)(start+1));
				ret += tString(line_string) + ScriptBlockInstance->GetLineAt(start) + RISSE_WS("\n");

				start ++;
			}
		}

		// アドレス部分を文字列化
		char address_string[22];
		sprintf(address_string, "%05ld ", (long)address);

		// VMコードのダンプを組み立てる
		ret += tString(address_string) + iterator.Dump(Consts) + RISSE_WS("\n");
		++iterator;
	}
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tCodeBlock::CodePositionToSourcePosition(risse_size pos) const
{
	risse_size s = 0;
	risse_size e = CodeToSourcePositionSize;
	if(e == 0) return 0;
	// CodeToSourcePosition[].first はすでにソートされているため、
	// 二分検索を行う
	while(true)
	{
		if(e - s <= 1) return CodeToSourcePosition[s].second;
		risse_size m = s + (e - s)/2;
		if(CodeToSourcePosition[m].first > pos)
			e = m;
		else
			s = m;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectInterface::tRetValue tCodeBlock::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// 仮実装
	if(code == ocFuncCall && name.IsEmpty())
	{
		Executor->Execute(args, ScriptBlockInstance->GetGlobal(), This, NULL, NULL, result);
	}
	return rvNoError;
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tSharedVariableFrames::tSharedVariableFrames(const tSharedVariableFramesOverlay & ref) :
	Frames(ref.Frames->Frames), CS(ref.Frames->CS)
{
	if(ref.OverlayedFrame)
	{
		RISSE_ASSERT(ref.OverlayedFrameLevel < Frames.size());
		Frames[ref.OverlayedFrameLevel] = ref.OverlayedFrame;
	}
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tCodeBlockStackAdapter::tRetValue
	tCodeBlockStackAdapter::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// 仮実装
	if(code == ocFuncCall && name.IsEmpty())
	{
		CodeBlock->GetExecutor()->Execute(args, CodeBlock->GetScriptBlockInstance()->GetGlobal(), This, Frame, &Shared, result);
	}
	return rvNoError;
}
//---------------------------------------------------------------------------





} // namespace Risse
