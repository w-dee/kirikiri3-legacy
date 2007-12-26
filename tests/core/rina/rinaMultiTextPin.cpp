//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief テスト用の複数形式をサポートするテキストピン管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaMultiTextPin.h"
#include "rinaWideTextEdge.h"
#include "rinaNarrowTextEdge.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(44859,59697,32532,20336,19383,41659,12874,42446);
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tMultiTextOutputPin::tMultiTextOutputPin()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<risse_uint32> & tMultiTextOutputPin::GetSupportedTypes()
{
	// 暫定実装、できれば static かシングルトン上の配列への参照を返した方がよい
	gc_vector<risse_uint32> * arr = new gc_vector<risse_uint32>();
	arr->push_back(WideTextEdgeType);
	arr->push_back(NarrowTextEdgeType);
	return *arr;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
