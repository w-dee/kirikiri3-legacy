//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 圧縮ブロックを展開された状態でメモリ上に保持するクラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/fs/xp4fs/DecompressedHolder.h"
#include "base/exception/RisaException.h"
#include <zlib.h>

RISSE_DEFINE_SOURCE_ID(47081,32049,45767,18248,53175,52305,56890,58573);


//---------------------------------------------------------------------------
tRisaDecompressedHolder::tRisaDecompressedHolder(tRisaDecompressedHolder::tMethod method,
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
tRisaDecompressedHolder::tRisaDecompressedHolder(tMethod method,
		tRisseBinaryStream * instream, risse_size insize, 
		risse_size uncomp_size)
{
	Size = 0;
	Data = NULL;

	// 入力データをメモリに読み込む
	risse_uint8 * indata = new risse_uint8[insize];
	try
	{
		instream->ReadBuffer(indata, insize);

		// データを展開
		Decompress(method, indata, insize, uncomp_size);
	}
	catch(...)
	{
		delete [] indata;
		throw;
	}
	delete [] indata;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaDecompressedHolder::~tRisaDecompressedHolder()
{
	delete [] Data;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaDecompressedHolder::Decompress(tRisaDecompressedHolder::tMethod method, const risse_uint8 * indata, risse_size insize, 
		risse_size uncomp_size)
{
	// すでにメモリブロックが割り当てられていた場合はメモリブロックを解放
	delete [] Data, Data = NULL;
	Size = 0;

	// メソッドをチェック (現在はzlibのみ)
	if(method != dhmZLib) RisaThrowInternalError;

	// 出力メモリブロックを確保
	Size = uncomp_size;
	Data = new risse_uint8[uncomp_size];
	try
	{
		// uncompress data
		unsigned long destlen = Size;
		int result = uncompress( (unsigned char*)indata, &destlen,
				(unsigned char*)indata, insize);
		if(result != Z_OK || destlen != Size)
			eRisaException::Throw(RISSE_WS_TR("Decompression failed. Data may be corrupted."));
	}
	catch(...)
	{
		delete [] Data;
		throw;
	}
}
//---------------------------------------------------------------------------

