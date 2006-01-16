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
#include "prec.h"
#include "RIFFWaveDecoder.h"
#include "RisaException.h"
#include "FSManager.h"

RISSE_DEFINE_SOURCE_ID(2300);


//---------------------------------------------------------------------------
// 定数など
//---------------------------------------------------------------------------


// フォーマット定数
#ifndef WAVE_FORMAT_PCM
	#define WAVE_FORMAT_PCM 0x0001
#endif
#ifndef WAVE_FORMAT_IEEE_FLOAT
	#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif
#ifndef WAVE_FORMAT_EXTENSIBLE
	#define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#endif


// WAVEFORMATEXTENSIBLE で使用されている GUID
static risse_uint8 RISA__GUID_KSDATAFORMAT_SUBTYPE_PCM[16] =
{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 };
static risse_uint8 RISA__GUID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT[16] =
{ 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 };

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		filename   ファイル名
//---------------------------------------------------------------------------
tRisaRIFFWaveDecoder::tRisaRIFFWaveDecoder(const ttstr & filename)
{
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
//---------------------------------------------------------------------------
tRisaRIFFWaveDecoder::~tRisaRIFFWaveDecoder()
{
	delete Stream;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サウンド形式を得る
//! @param		format   形式を格納するための構造体
//---------------------------------------------------------------------------
void tRisaRIFFWaveDecoder::GetFormat(tRisaWaveFormat & format)
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
bool tRisaRIFFWaveDecoder::Render(void *buf, risse_uint bufsamplelen, risse_uint& rendered)
{
	risse_uint64 remain = Format.TotalSampleGranules - CurrentPos;
	risse_uint writesamples = bufsamplelen < remain ? bufsamplelen : (risse_uint)remain;
	if(writesamples == 0)
	{
		// already finished stream or bufsamplelen is zero
		rendered = 0;
		return false;
	}

	risse_uint readsize = writesamples * SampleSize;
	risse_uint read = Stream->Read(buf, readsize);

#if RISSE_HOST_IS_BIG_ENDIAN
	// endian-ness conversion
	if(Format.BytesPerSample == 2)
	{
		risse_uint16 *p = (risse_uint16 *)buf;
		risse_uint16 *plim = (risse_uint16 *)( (risse_uint8*)buf + read);
		while(p < plim)
		{
			*p = (*p>>8) + (*p<<8);
			p++;
		}
	}
	else if(Format.BytesPerSample == 3)
	{
		risse_uint8 *p = (risse_uint8 *)buf;
		risse_uint8 *plim = (risse_uint8 *)( (risse_uint8*)buf + read);
		while(p < plim)
		{
			risse_uint8 tmp = p[0];
			p[0] = p[2];
			p[2] = tmp;
			p += 3;
		}
	}
	else if(Format.BytesPerSample == 4)
	{
		risse_uint32 *p = (risse_uint32 *)buf;
		risse_uint32 *plim = (risse_uint32 *)( (risse_uint8*)buf + read);
		while(p < plim)
		{
			*p =
				(*p &0xff000000) >> 24 +
				(*p &0x00ff0000) >> 8 +
				(*p &0x0000ff00) << 8 +
				(*p &0x000000ff) << 24;
			p ++;
		}
	}
#endif


	rendered = read / SampleSize;
	CurrentPos += rendered;

	if(read < readsize || writesamples < bufsamplelen)
		return false; // read error or end of stream

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デコード位置を変更する
//! @param		samplepos		変更したい位置
//! @return		デコード位置の変更に成功すると真
//---------------------------------------------------------------------------
bool tRisaRIFFWaveDecoder::SetPosition(risse_uint64 samplepos)
{
	if(Format.TotalSampleGranules <= samplepos) return false;

	risse_uint64 streampos = DataStart + samplepos * SampleSize;
	risse_uint64 possave = Stream->GetPosition();

	if(streampos != Stream->Seek(streampos, RISSE_BS_SEEK_SET))
	{
		// seek failed
		Stream->Seek(possave, RISSE_BS_SEEK_SET);
		return false;
	}

	CurrentPos = samplepos;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サウンドを開く
//! @return		開くことに成功すれば真
//---------------------------------------------------------------------------
bool tRisaRIFFWaveDecoder::Open()
{
	// Stream を RIFF Wave サウンドとして開く

	static const risse_uint8 riff_mark[] =
		{ /*R*/0x52, /*I*/0x49, /*F*/0x46, /*F*/0x46 };
	static const risse_uint8 wave_mark[] =
		{ /*W*/0x57, /*A*/0x41, /*V*/0x56, /*E*/0x45 };
	static const risse_uint8 fmt_mark[] =
		{ /*f*/0x66, /*m*/0x6d, /*t*/0x74, /* */0x20 };
	static const risse_uint8 data_mark[] =
		{ /*d*/0x64, /*a*/0x61, /*t*/0x74, /*a*/0x61 };

	risse_uint32 size;
	risse_int64 next;

	// check RIFF mark
	risse_uint8 buf[4];
	if(4 != Stream->Read(buf, 4)) return false;
	if(memcmp(buf, riff_mark, 4)) return false;

	if(4 != Stream->Read(buf, 4)) return false; // RIFF chunk size; discard

	// check WAVE subid
	if(4 != Stream->Read(buf, 4)) return false;
	if(memcmp(buf, wave_mark, 4)) return false;

	// find fmt chunk
	if(!FindRIFFChunk(Stream, fmt_mark)) return false;

	size = Stream->ReadI32LE();
	next = Stream->GetPosition() + size;

	// read Format
	tRisaWaveFormat Format;

	risse_uint16 format_tag = Stream->ReadI16LE(); // wFormatTag
	if(format_tag != WAVE_FORMAT_PCM &&
		format_tag != WAVE_FORMAT_IEEE_FLOAT &&
		format_tag != WAVE_FORMAT_EXTENSIBLE) return false;


	Format.Channels = Stream->ReadI16LE(); // nChannels
	Format.Frequency = Stream->ReadI32LE(); // nSamplesPerSec

	if(4 != Stream->Read(buf, 4)) return false; // nAvgBytesPerSec; discard

	risse_uint16 block_align = Stream->ReadI16LE(); // nBlockAlign

	Format.BitsPerSample = Stream->ReadI16LE(); // wBitsPerSample

	risse_uint16 ext_size = Stream->ReadI16LE(); // cbSize
	if(format_tag == WAVE_FORMAT_EXTENSIBLE)
	{
		if(ext_size != 22) return false; // invalid extension length
		if(Format.BitsPerSample & 0x07) return false;  // not integer multiply by 8
		Format.BytesPerSample = Format.BitsPerSample / 8;
		Format.BitsPerSample = Stream->ReadI16LE(); // wValidBitsPerSample
		Format.SpeakerConfig = Stream->ReadI32LE(); // dwChannelMask

		risse_uint8 guid[16];
		if(16 != Stream->Read(guid, 16)) return false;
		if(!memcmp(guid, RISA__GUID_KSDATAFORMAT_SUBTYPE_PCM, 16))
			Format.IsFloat = false;
		else if(!memcmp(guid, RISA__GUID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 16))
			Format.IsFloat = true;
		else
			return false;
	}
	else
	{
		if(Format.BitsPerSample & 0x07) return false; // not integer multiplyed by 8
		Format.BytesPerSample = Format.BitsPerSample / 8;

		if(Format.Channels == 4)
			Format.SpeakerConfig = 0;
		else if(Format.Channels == 6)
			Format.SpeakerConfig = 0;
		else
			Format.SpeakerConfig = 0;

		Format.IsFloat = format_tag == WAVE_FORMAT_IEEE_FLOAT;
	}


	if(Format.BitsPerSample > 32) return false; // too large bits
	if(Format.BitsPerSample < 8) return false; // too less bits
	if(Format.BitsPerSample > Format.BytesPerSample * 8)
		return false; // bits per sample is larger than bytes per sample
	if(Format.IsFloat)
	{
		if(Format.BitsPerSample != 32) return false; // not a 32-bit IEEE float
		if(Format.BytesPerSample != 4) return false;
	}

	if((risse_int) block_align != (risse_int)(Format.BytesPerSample * Format.Channels))
		return false; // invalid align

	if(next != Stream->Seek(next, RISSE_BS_SEEK_SET)) return false;

	// find data chunk
	if(!FindRIFFChunk(Stream, data_mark)) return false;

	size = Stream->ReadI32LE();

	risse_int64 datastart;

	risse_int64 remain_size = Stream->GetSize() -
		(datastart = Stream->GetPosition());
	if(size > remain_size) return false;
		// data ends before "size" described in the header

	// compute total sample count and total length in time
	Format.TotalSampleGranules = size / (Format.Channels * Format.BytesPerSample);
	Format.TotalTime = Format.TotalSampleGranules * 1000 / Format.Frequency;

	// the stream is seekable
	Format.Seekable = true;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		RIFFチャンクを探す
//! @param		stream		ストリーム(このストリームの現在位置から検索が始まる)
//! @param		chunk		探したいチャンク
//! @return		指定された RIFF チャンクが見つかれば真
//---------------------------------------------------------------------------
bool tRisaRIFFWaveDecoder::FindRIFFChunk(tRisseBinaryStream * stream, const risse_uint8 *chunk)
{
	risse_uint8 buf[4];
	while(true)
	{
		if(4 != stream->Read(buf, 4)) return false;
		if(memcmp(buf, chunk, 4))
		{
			// skip to next chunk
			risse_uint32 chunksize = stream->ReadI32LE();
			risse_int64 next = stream->GetPosition() + chunksize;
			if(next != stream->Seek(next, RISSE_BS_SEEK_SET)) return false;
		}
		else
		{
			return true;
		}
	}
}
//---------------------------------------------------------------------------

