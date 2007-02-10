//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コードブロック
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseCodeBlock.h"
#include "risseException.h"
#include "risseScriptBlockBase.h"
#include "compiler/risseCodeGen.h"
#include "risseCodeExecutor.h"

namespace Risse
{
//---------------------------------------------------------------------------
RISSE_DEFINE_SOURCE_ID(13739,40903,3219,19310,50086,28697,53693,30185);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseCodeBlock::tRisseCodeBlock(tRisseScriptBlockBase * sb)
{
	ScriptBlock = sb;
	Code = NULL;
	CodeSize = 0;
	Consts = NULL;
	ConstsSize = 0;
	NumRegs = 0;
	NumSharedVars = 0;
	CodeBlockRelocations = NULL;
	CodeBlockRelocationSize = 0;
	TryIdentifierRelocations = NULL;
	TryIdentifierRelocationSize = 0;

	Executor = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeBlock::Assign(const tRisseCodeGenerator *gen)
{
	// gen からいろいろ情報をコピー
	risse_size ind;

	// コードのコピー
	const gc_vector<risse_uint32> & gen_code = gen->GetCode();
	CodeSize = gen_code.size();
	Code = reinterpret_cast<risse_uint32*>(
		RisseMallocAtomicCollectee(CodeSize * sizeof(risse_uint32)));
		// 注意
		// RisseMallocAtomicCollectee を使って Code 領域を確保しているため、
		// Code 領域中にポインタを含ませるばあいは相応の対処をすること。
	ind = 0;
	for(gc_vector<risse_uint32>::const_iterator i = gen_code.begin();
		i != gen_code.end(); i++, ind++)
		Code[ind] = *i;

	// 定数領域のコピー
	const gc_vector<tRisseVariant> & gen_consts = gen->GetConsts();
	ConstsSize = gen_consts.size();
	Consts = new tRisseVariant[ConstsSize];
	ind = 0;
	for(gc_vector<tRisseVariant>::const_iterator i = gen_consts.begin();
		i != gen_consts.end(); i++, ind++)
		Consts[ind] = *i;

	// CodeBlockRelocations のコピー
	const gc_vector<std::pair<risse_size, risse_size> > & cb_relocations =
			gen->GetCodeBlockRelocations();
	CodeBlockRelocations = new tRelocation[CodeBlockRelocationSize = cb_relocations.size()];
	ind = 0;
	for(gc_vector<std::pair<risse_size, risse_size> >::const_iterator i =
		cb_relocations.begin(); i != cb_relocations.end(); i++, ind++)
		CodeBlockRelocations[ind] = *i;

	// TryIdentifierRelocations のコピー
	const gc_vector<std::pair<risse_size, risse_size> > & ti_relocations =
			gen->GetTryIdentifierRelocations();
	TryIdentifierRelocations = new tRelocation[TryIdentifierRelocationSize = ti_relocations.size()];
	ind = 0;
	for(gc_vector<std::pair<risse_size, risse_size> >::const_iterator i =
		ti_relocations.begin(); i != ti_relocations.end(); i++, ind++)
		TryIdentifierRelocations[ind] = *i;

	// 必要なレジスタ数/共有変数の数のコピー
	NumRegs = gen->GetMaxNumUsedRegs();
	NestLevel = gen->GetNestLevel();
	NumSharedVars = gen->GetSharedRegCount();

	// Executor を作成
	Executor = new tRisseCodeInterpreter(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeBlock::SetMaxNestLevel(risse_size level)
{
	RISSE_ASSERT(NestLevel == 0); // この情報を持つコードブロックのネストレベルは 0 で無くてはならない
	MaxNestLevel = level;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeBlock::Fixup()
{
	RISSE_ASSERT(CodeBlockRelocations != NULL); // 二度以上このメソッドを呼べない
	RISSE_ASSERT(TryIdentifierRelocations != NULL); // 二度以上このメソッドを呼べない

	for(risse_size i = 0 ; i < CodeBlockRelocationSize; i++)
		Consts[CodeBlockRelocations[i].first] =
			ScriptBlock->GetCodeBlockAt(CodeBlockRelocations[i].second)->GetObject(); // 再配置を行う

	for(risse_size i = 0 ; i < TryIdentifierRelocationSize; i++)
	{
		Consts[TryIdentifierRelocations[i].first].Clear();
		Consts[TryIdentifierRelocations[i].first] =
			reinterpret_cast<tRisseObjectInterface *>
			(ScriptBlock->GetTryIdentifierAt(TryIdentifierRelocations[i].second));
				// 再配置を行う。GetTryIdentifierAt の戻りは void * で
				// そのポインタは tRisseObjectInterface ではないが、
				// 実装上の都合 void * を tRisseObjectInterface に
				// reinterpret_cast して使う。このポインタは識別子代わりに
				// 使う物で、このポインタの指す先に実際にアクセスを行うような
				// ことは行わないのでこれでよいが、注意のこと。
	}

	CodeBlockRelocations = NULL; // もう再配置は行った
	TryIdentifierRelocations = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseCodeBlock::GetObject()
{
	return tRisseVariant(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseCodeBlock::Dump() const
{
	tRisseString ret;
	tRisseVMCodeIterator iterator(Code, 0);
	while((const risse_uint32*)iterator != CodeSize + Code)
	{
		char address[22];
		sprintf(address, "%05d ", (int)iterator.GetAddress());
		ret += tRisseString(address) + iterator.Dump(Consts) + RISSE_WS("\n");
		++iterator;
	}
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectInterface::tRetValue tRisseCodeBlock::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// 仮実装
	if(code == ocFuncCall && name.IsEmpty())
	{
		Executor->Execute(args, This, NULL, NULL, result);
	}
	return rvNoError;
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
tRisseCodeBlockStackAdapter::tRetValue
	tRisseCodeBlockStackAdapter::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// 仮実装
	if(code == ocFuncCall && name.IsEmpty())
	{
		CodeBlock->GetExecutor()->Execute(args, This, Frame, Shared, result);
	}
	return rvNoError;
}
//---------------------------------------------------------------------------





} // namespace Risse
