//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 圧縮ブロックを展開された状態でメモリ上に保持するクラス
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "base/fs/xp4fs/DecompressedHolder.h"
#include "base/exception/RisaException.h"
#include <zlib.h>


namespace Risa {
RISSE_DEFINE_SOURCE_ID(47081,32049,45767,18248,53175,52305,56890,58573);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tDecompressedHolder::tDecompressedHolder(tDecompressedHolder::tMethod method,
		const risse_uint8 * indata, risse_size insize,
		risse_size uncomp_size)
{
	Size = 0;
	Data = NULL;

	// データを展開
	Decompress(method, indata, insize, uncomp_size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tDecompressedHolder::tDecompressedHolder(tMethod method,
		tBinaryStream * instream, risse_size insize,
		risse_size uncomp_size)
{
	Size = 0;
	Data = NULL;

	// 入力データをメモリに読み込む
	risse_uint8 * indata = new (PointerFreeGC) risse_uint8[insize];

	instream->ReadBuffer(indata, insize);

	// データを展開
	Decompress(method, indata, insize, uncomp_size);

	delete (PointerFreeGC) [] indata;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tDecompressedHolder::~tDecompressedHolder()
{
	delete (PointerFreeGC) [] Data;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDecompressedHolder::Decompress(tDecompressedHolder::tMethod method, const risse_uint8 * indata, risse_size insize,
		risse_size uncomp_size)
{
	// すでにメモリブロックが割り当てられていた場合はメモリブロックを解放
	delete (PointerFreeGC) [] Data, Data = NULL;
	Size = 0;

	// メソッドをチェック (現在はzlibのみ)
	if(method != dhmZLib) ThrowInternalError;

	// 出力メモリブロックを確保
	Size = uncomp_size;
	Data = new (PointerFreeGC) risse_uint8[uncomp_size];

	// uncompress data
	unsigned long destlen = Size;
	int result = uncompress( (unsigned char*)Data, &destlen,
			(unsigned char*)indata, insize);
	if(result != Z_OK || destlen != Size)
		eRisaException::Throw(RISSE_WS_TR("decompression failed, data may be corrupted"));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


