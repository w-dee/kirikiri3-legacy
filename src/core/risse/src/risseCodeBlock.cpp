//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
#include "risseCodeGen.h"


namespace Risse
{
//---------------------------------------------------------------------------
RISSE_DEFINE_SOURCE_ID(13739,40903,3219,19310,50086,28697,53693,30185);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseCodeBlock::tRisseCodeBlock(const tRisseCodeGenerator *gen)
{
	// gen からいろいろ情報をコピー

	// コードのコピー
	const gc_vector<risse_uint32> & gen_code = gen->GetCode();
	CodeSize = gen_code.size();
	Code = reinterpret_cast<risse_uint32*>(
		RisseMallocAtomicCollectee(CodeSize * sizeof(risse_uint32)));
		// 注意
		// RisseMallocAtomicCollectee を使って Code 領域を確保しているため、
		// Code 領域中にポインタを含ませるばあいは相応の対処をすること。
	risse_size ind = 0;
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

	// 必要なレジスタ数のコピー
	NumRegs = gen->GetMaxNumUsedRegs();
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


} // namespace Risse
