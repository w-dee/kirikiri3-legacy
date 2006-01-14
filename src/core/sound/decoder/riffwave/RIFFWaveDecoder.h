//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
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
class tRisaRIFFWaveDecoder : public tRisaWaveDecoder
{
	tRisseBinaryStream * Stream; //!< 入力ストリーム
	tRisaWaveFormat Format; //!< サウンド形式
	risse_uint64 DataStart; //!< ファイル中でのデータの開始位置
	risse_uint64 CurrentPos; //!< ファイル中でのデータの読み込み位置
	risse_uint SampleSize; //!< 1サンプルのサイズ

public:
	tRisaRIFFWaveDecoder(const ttstr & filename);

	// tRisaWaveDecoder をオーバーライドするもの
	virtual ~tRisaRIFFWaveDecoder();
	virtual void GetFormat(tRisaWaveFormat & format);
	virtual bool Render(void *buf, risse_uint bufsamplelen, risse_uint& rendered);
	virtual bool SetPosition(risse_uint64 samplepos);

private:
	void Open();
	static bool FindRIFFChunk(tRisseStream * stream, const risse_uint8 *chunk);
};
//---------------------------------------------------------------------------


#endif


