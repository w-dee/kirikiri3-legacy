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
#include "prec.h"
#include "VorbisDecoder.h"
#include "RisaException.h"
#include "FSManager.h"
#include "vorbis/vorbisfile.h"


RISSE_DEFINE_SOURCE_ID(2301);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		filename   ファイル名
//---------------------------------------------------------------------------
tRisaOggVorbisDecoder::tRisaOggVorbisDecoder(const ttstr & filename)
{
	CurrentSection = -1;
	Stream = tRisaFileSystemManager::instance()->CreateStream(filename, RISSE_BS_READ);

	try
	{
		if(!Open())
			eRisaException::Throw(RISSE_WS_TR("can not open file '%1' : invalid format"),
				filename);
	}
	catch(...)
	{
		delete Stream;
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//! @param		filename   ファイル名
//---------------------------------------------------------------------------
tRisaOggVorbisDecoder::~tRisaOggVorbisDecoder()
{
	ov_clear(&InputFile);
	delete Stream;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サウンド形式を得る
//! @param		format   形式を格納するための構造体
//---------------------------------------------------------------------------
void tRisaOggVorbisDecoder::GetFormat(tRisaWaveFormat & format)
{
	format = Format;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サウンドをレンダリングする
//! @param		buf					データ格納先バッファ
//! @param		bufsamplelen		バッファのサンプル数(サンプルグラニュール単位)
//! @param		rendered			実際にレンダリングが行われたサンプル数
//! @return		サウンドの最後に達すると偽、それ以外は真
//! @note		rendered != bufsamplelen の場合はサウンドの最後に達したことを
//!				示さない。返値が偽になったかどうかでサウンドの最後に達したかどうかを
//!				判断すること。
//---------------------------------------------------------------------------
bool tRisaOggVorbisDecoder::Render(void *buf, risse_uint bufsamplelen, risse_uint& rendered)
{
	// render output PCM
	int pcmsize = Format.BytesPerSample;

	int res;
	risse_uint pos = 0; // decoded PCM (in bytes)
	int remain = bufsamplelen * Format.Channels * pcmsize; // remaining PCM (in bytes)

	while(remain)
	{
		do
		{
			res = ov_read(&InputFile, ((char*)buf + pos), remain,
				0, pcmsize, 1, &CurrentSection); // decode via ov_read
		} while(res<0); // ov_read would return a negative number
						// if the decoding is not ready
		if(res==0) break;
		pos += res;
		remain -= res;
	}

	pos /= (Format.Channels * pcmsize); // convert to PCM position

	rendered = pos; // return renderd PCM samples

	return pos == bufsamplelen;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デコード位置を変更する
//! @param		samplepos		変更したい位置
//! @return		デコード位置の変更に成功すると真
//---------------------------------------------------------------------------
bool tRisaOggVorbisDecoder::SetPosition(risse_uint64 samplepos)
{
	if(0 != ov_pcm_seek(&InputFile, samplepos))
		return false;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サウンドを開く
//! @return		開くことに成功すれば真
//---------------------------------------------------------------------------
bool tRisaOggVorbisDecoder::Open()
{
	// コールバック関数の容易
	ov_callbacks callbacks = {read_func, seek_func, close_func, tell_func};
		// callback functions

	// 開く
	if(ov_open_callbacks(this, &InputFile, NULL, 0, callbacks) < 0)
	{
		// 失敗
		return false;
	}

	// retrieve PCM information
	vorbis_info *vi;
	vi = ov_info(&InputFile, -1);
	if(!vi)
	{
		ov_clear(&InputFile);
		return E_FAIL;
	}

	// set Format up
	Format.Frequency = vi->rate;
	Format.Channels = vi->channels;
	Format.BitsPerSample = 16; // とりあえず 16bit 固定
	Format.BytesPerSample = Format.BitsPerSample / 8;

	bool seekable = true;

	risse_int64 pcmtotal = ov_pcm_total(&InputFile, -1); // PCM total samples
	if(pcmtotal<0) seekable = false, pcmtotal = 0;
	Format.TotalSampleGranules = pcmtotal;

	double timetotal = ov_time_total(&InputFile, -1); // total time in sec.
	if(timetotal<0)
		seekable = false, Format.TotalTime = 0;
	else
		Format.TotalTime = static_cast<risse_uint64>(timetotal * 1000.0);

	Format.IsFloat = false; // とりあえず false 固定
	Format.Seekable = seekable;

	return true; // 成功
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		読み込み関数
//---------------------------------------------------------------------------
size_t tRisaOggVorbisDecoder::read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	// read function (wrapper for tRisseBinaryStream)

	tRisaOggVorbisDecoder * decoder = reinterpret_cast<tRisaOggVorbisDecoder*>(datasource);

	risse_uint bytesread = decoder->Stream->Read(ptr, risse_uint(size * nmemb));

	return bytesread / size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シーク関数
//---------------------------------------------------------------------------
int tRisaOggVorbisDecoder::seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	// seek function (wrapper for tRisseBinaryStream)

	tRisaOggVorbisDecoder * decoder = reinterpret_cast<tRisaOggVorbisDecoder*>(datasource);

	risse_int seek_type = STREAM_SEEK_SET;

	switch(whence)
	{
	case SEEK_SET:
		seek_type = RISSE_BS_SEEK_SET;
		break;
	case SEEK_CUR:
		seek_type = RISSE_BS_SEEK_CUR;
		break;
	case SEEK_END:
		seek_type = RISSE_BS_SEEK_END;
		break;
	}

	if(offset != 0)
	{
		risse_uint64 curpos = decoder->Stream->GetPosition();
		if(decoder->Stream->Seek(offset, seek_type) == curpos)
			return -1; // failed
	}

	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		クローズ関数
//---------------------------------------------------------------------------
int tRisaOggVorbisDecoder::close_func(void *datasource)
{
	// close function (wrapper for tRisseBinaryStream)

	// なにもしない

	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tell関数
//---------------------------------------------------------------------------
long tRisaOggVorbisDecoder::tell_func(void *datasource)
{
	// tell function (wrapper for tRisseBinaryStream)

	tRisaOggVorbisDecoder * decoder = reinterpret_cast<tRisaOggVorbisDecoder*>(datasource);

	return decoder->Stream->GetPosition();
}
//---------------------------------------------------------------------------

