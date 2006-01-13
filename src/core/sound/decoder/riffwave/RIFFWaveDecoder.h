//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RIFF Wave デコーダ
//---------------------------------------------------------------------------

#ifndef RIFFWAVEDECODERH
#define RIFFWAVEDECODERH

#include "WaveDecoder.h"
#include "tjs.h"

//---------------------------------------------------------------------------
//! @brief	 RIFF Wave デコーダ
//---------------------------------------------------------------------------
class tTVPRIFFWaveDecoder : public tTVPWaveDecoder
{
	tTJSBinaryStream * Stream; //!< 入力ストリーム
	tTVPWaveFormat Format; //!< サウンド形式
	tjs_uint64 DataStart; //!< ファイル中でのデータの開始位置
	tjs_uint64 CurrentPos; //!< ファイル中でのデータの読み込み位置
	tjs_uint SampleSize; //!< 1サンプルのサイズ

public:
	tTVPRIFFWaveDecoder(const ttstr & filename);

	// tTVPWaveDecoder をオーバーライドするもの
	virtual ~tTVPRIFFWaveDecoder();
	virtual void GetFormat(tTVPWaveFormat & format);
	virtual bool Render(void *buf, tjs_uint bufsamplelen, tjs_uint& rendered);
	virtual bool SetPosition(tjs_uint64 samplepos);

private:
	void Open();
	static bool FindRIFFChunk(tTJSStream * stream, const tjs_uint8 *chunk);
};
//---------------------------------------------------------------------------


#endif


