//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OggVorbis デコーダ
//---------------------------------------------------------------------------

#ifndef OGGVORBISDECODERH
#define OGGVORBISDECODERH

#include "sound/WaveDecoder.h"
#include "risse/include/risse.h"
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

//---------------------------------------------------------------------------
//! @brief	 OggVorbis デコーダ
//---------------------------------------------------------------------------
class tRisaOggVorbisDecoder : public tRisaWaveDecoder
{
	tRisseBinaryStream * Stream; //!< 入力ストリーム
	tRisaWaveFileInfo FileInfo; //!< サウンドファイル情報
	OggVorbis_File InputFile; //!< OggVorbis_File instance
	int CurrentSection;

public:
	tRisaOggVorbisDecoder(const ttstr & filename);

	// tRisaWaveDecoder をオーバーライドするもの
	virtual ~tRisaOggVorbisDecoder();
	virtual void SuggestFormat(const tRisaWaveFormat & format);
	virtual void GetFormat(tRisaWaveFileInfo & fileinfo);
	virtual bool Render(void *buf, risse_uint bufsamplelen, risse_uint& rendered);
	virtual bool SetPosition(risse_uint64 samplepos);

private:
	bool Open();

	static size_t read_func(void *ptr,
		size_t size, size_t nmemb, void *datasource);
	static int seek_func(void *datasource, ogg_int64_t offset, int whence);
	static int close_func(void *datasource);
	static long tell_func(void *datasource);

};
//---------------------------------------------------------------------------


#endif


