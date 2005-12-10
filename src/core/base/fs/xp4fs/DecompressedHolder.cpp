//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 圧縮ブロックを展開された状態でメモリ上に保持するクラス
//---------------------------------------------------------------------------

/*
	XP4 アーカイブ中の圧縮されたセグメントは、このソースファイルに実装された
	機構によりメモリ上にキャッシュされる
*/

#include "DecompressedHolder.h"

#include <zlib.h>



//---------------------------------------------------------------------------
//! @brief		コンストラクタ(メモリ上のデータを展開する場合)
//! @param		method: 圧縮メソッド
//! @param		indata: 入力データ
//! @param		insize: 入力データのサイズ
//! @param		uncomp_size: 入力データを展開した際のサイズ
//---------------------------------------------------------------------------
tTVPDecompressedHolder::tTVPDecompressedHolder(tTVPDecompressedHolder::tMethod method,
		const tjs_uint8 * indata, tjs_size insize, 
		tjs_size uncomp_size)
{
	Size = 0;
	Data = NULL;

	// データを展開
	Decompress(method, indata, insize, uncomp_size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(ストリーム上のデータを展開する場合)
//! @param		method: 圧縮メソッド
//! @param		indata: 入力ストリーム
//! @param		insize: 入力データのサイズ
//! @param		uncomp_size: 入力データを展開した際のサイズ
//---------------------------------------------------------------------------
tTVPDecompressedHolder::tTVPDecompressedHolder(tMethod method,
		tTJSBinaryStream * instream, tjs_size insize, 
		tjs_size uncomp_size)
{
	Size = 0;
	Data = NULL;

	// 入力データをメモリに読み込む
	tjs_uint8 * indata = new tjs_uint8[insize];
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
//!@brief		デストラクタ
//---------------------------------------------------------------------------
tTVPDecompressedHolder::~tTVPDecompressedHolder()
{
	delete [] Data;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メモリ上のデータを展開する
//! @param		method: 圧縮メソッド
//! @param		indata: 入力データ
//! @param		insize: 入力データのサイズ
//! @param		uncomp_size: 入力データを展開した際のサイズ
//---------------------------------------------------------------------------
void Decompress(tTVPDecompressedHolder::tMethod method, const tjs_uint8 * indata, tjs_size insize, 
		tjs_size uncomp_size)
{
	// すでにメモリブロックが割り当てられていた場合はメモリブロックを解放
	delete [] Data, Data = NULL;
	Size = 0;

	// メソッドをチェック (現在はzlibのみ)
	if(method != dhmZLib) TVPThrowInternalError;

	// 出力メモリブロックを確保
	Size = uncomp_size;
	Data = new tjs_uint8[uncomp_size];
	try
	{
		// uncompress data
		unsigned long destlen = Size;
		int result = uncompress( (unsigned char*)indata, &destlen,
				(unsigned char*)indata, insize);
		if(result != Z_OK || destlen != Size)
			TVPThrowExceptionMessage(_("Decompression failed. Data may be corrupted."));
	}
	catch(...)
	{
		delete [] Data;
		throw;
	}
}
//---------------------------------------------------------------------------

