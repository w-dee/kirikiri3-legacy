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
#include "risse.h"

//---------------------------------------------------------------------------
//! @brief	 RIFF Wave デコーダ
//---------------------------------------------------------------------------
class tTVPRIFFWaveDecoder : public tTVPWaveDecoder
{
	tRisseBinaryStream * Stream; //!< 入力ストリーム
	tTVPWaveFormat Format; //!< サウンド形式
	risse_uint64 DataStart; //!< ファイル中でのデータの開始位置
	risse_uint64 CurrentPos; //!< ファイル中でのデータの読み込み位置
	risse_uint SampleSize; //!< 1サンプルのサイズ

public:
	tTVPRIFFWaveDecoder(const ttstr & filename);

	// tTVPWaveDecoder をオーバーライドするもの
	virtual ~tTVPRIFFWaveDecoder();
	virtual void GetFormat(tTVPWaveFormat & format);
	virtual bool Render(void *buf, risse_uint bufsamplelen, risse_uint& rendered);
	virtual bool SetPosition(risse_uint64 samplepos);

private:
	void Open();
	static bool FindRIFFChunk(tRisseStream * stream, const risse_uint8 *chunk);
};
//---------------------------------------------------------------------------


#endif


