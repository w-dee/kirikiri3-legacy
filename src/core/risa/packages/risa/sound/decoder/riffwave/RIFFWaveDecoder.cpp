//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RIFF Wave デコーダ
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/sound/decoder/riffwave/RIFFWaveDecoder.h"
#include "risa/packages/risa/fs/FSManager.h"
#include "risa/packages/risa/sound/Sound.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(17161,14775,60981,18892,4009,20341,33502,766);
//---------------------------------------------------------------------------

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
static risse_uint8 RISA_GUID_KSDATAFORMAT_SUBTYPE_PCM[16] =
{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 };
static risse_uint8 RISA_GUID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT[16] =
{ 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 };

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRIFFWaveDecoder::tRIFFWaveDecoder(const tString & filename) :
	Stream(tFileSystemManager::instance()->Open(filename, tFileOpenModes::omRead))
{
	try
	{
		if(!Open())
			tSoundExceptionClass::Throw(tString(RISSE_WS_TR("cannot open file '%1': invalid format"),
				filename));
	}
	catch(...)
	{
		Stream.Dispose();
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRIFFWaveDecoder::GetFormat(tWaveFileInfo & fileinfo)
{
	fileinfo = FileInfo;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRIFFWaveDecoder::Render(void *buf, risse_uint bufsamplelen, risse_uint& rendered)
{
	risse_uint64 remain = FileInfo.TotalSampleGranules - CurrentPos;
	risse_uint writesamples = bufsamplelen < remain ? bufsamplelen : (risse_uint)remain;
	if(writesamples == 0)
	{
		// already finished stream or bufsamplelen is zero
		rendered = 0;
		return false;
	}

	risse_uint readsize = writesamples * FileInfo.GetSampleGranuleSize();
	risse_uint read = Stream.Read(buf, readsize);

#if RISSE_HOST_IS_BIG_ENDIAN
	// endian-ness conversion
	switch(FileInfo.PCMType)
	{
	tPCMTypes::ti16:
		{
			risse_uint16 *p = (risse_uint16 *)buf;
			risse_uint16 *plim = (risse_uint16 *)( (risse_uint8*)buf + read);
			while(p < plim)
			{
				*p = (*p>>8) + (*p<<8);
				p++;
			}
		}
		break;
	tPCMTypes::ti24:
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
		break;
	tPCMTypes::ti32:
	tPCMTypes::tf32:
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
		break;
	default:
		break;
	}
#endif

	rendered = read / FileInfo.GetSampleGranuleSize();
	CurrentPos += rendered;

	if(read < readsize || writesamples < bufsamplelen)
		return false; // read error or end of stream

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRIFFWaveDecoder::SetPosition(risse_uint64 samplepos)
{
	if(FileInfo.TotalSampleGranules <= samplepos) return false;

	risse_uint64 streampos = DataStart + samplepos * FileInfo.GetSampleGranuleSize();
	risse_uint64 possave = Stream.GetPosition();

	if(!Stream.Seek(streampos, tStreamConstants::soSet))
	{
		// seek failed
		Stream.Seek(possave, tStreamConstants::soSet);
		return false;
	}

	CurrentPos = samplepos;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRIFFWaveDecoder::Open()
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
	risse_uint64 next;

	// check RIFF mark
	risse_uint8 buf[4];
	if(4 != Stream.Read(buf, 4)) return false;
	if(memcmp(buf, riff_mark, 4)) return false;

	if(4 != Stream.Read(buf, 4)) return false; // RIFF chunk size; discard

	// check WAVE subid
	if(4 != Stream.Read(buf, 4)) return false;
	if(memcmp(buf, wave_mark, 4)) return false;

	// find fmt chunk
	if(!FindRIFFChunk(Stream, fmt_mark)) return false;

	size = Stream.ReadI32LE();
	next = Stream.GetPosition() + size;

	// read FileInfo
	risse_uint16 format_tag = Stream.ReadI16LE(); // wFormatTag
	if(format_tag != WAVE_FORMAT_PCM &&
		format_tag != WAVE_FORMAT_IEEE_FLOAT &&
		format_tag != WAVE_FORMAT_EXTENSIBLE) return false;


	FileInfo.Channels = Stream.ReadI16LE(); // nChannels
	FileInfo.Frequency = Stream.ReadI32LE(); // nSamplesPerSec

	if(4 != Stream.Read(buf, 4)) return false; // nAvgBytesPerSec; discard

	risse_uint16 block_align = Stream.ReadI16LE(); // nBlockAlign

	int bits_per_sample = Stream.ReadI16LE(); // wBitsPerSample
	bool is_float = false;

	risse_uint16 ext_size = Stream.ReadI16LE(); // cbSize
	if(format_tag == WAVE_FORMAT_EXTENSIBLE)
	{
		if(ext_size != 22) return false; // invalid extension length
		if(bits_per_sample & 0x07) return false;  // not integer multiply by 8
		Stream.ReadI16LE(); // wValidBitsPerSample; discard
		FileInfo.SpeakerConfig = Stream.ReadI32LE(); // dwChannelMask

		risse_uint8 guid[16];
		if(16 != Stream.Read(guid, 16)) return false;
		if(!memcmp(guid, RISA_GUID_KSDATAFORMAT_SUBTYPE_PCM, 16))
			is_float = false;
		else if(!memcmp(guid, RISA_GUID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 16))
			is_float = true;
		else
			return false;
	}
	else
	{
		if(bits_per_sample & 0x07) return false; // not integer multiplied by 8

		if(FileInfo.Channels == 4)
			FileInfo.SpeakerConfig = 0;
		else if(FileInfo.Channels == 6)
			FileInfo.SpeakerConfig = 0;
		else
			FileInfo.SpeakerConfig = 0;

		is_float = format_tag == WAVE_FORMAT_IEEE_FLOAT;
	}

	if(is_float && bits_per_sample == 32)
		FileInfo.PCMType = tPCMTypes::tf32;
	else if(!is_float && bits_per_sample == 32)
		FileInfo.PCMType = tPCMTypes::ti32;
	else if(!is_float && bits_per_sample == 24)
		FileInfo.PCMType = tPCMTypes::ti24;
	else if(!is_float && bits_per_sample == 16)
		FileInfo.PCMType = tPCMTypes::ti16;
	else if(!is_float && bits_per_sample == 8)
		FileInfo.PCMType = tPCMTypes::ti8;
	else
		return false;

	if((risse_int) block_align != (risse_int)((bits_per_sample / 8) * FileInfo.Channels))
		return false; // invalid align

	if(!Stream.Seek(next, tStreamConstants::soSet)) return false;

	// find data chunk
	if(!FindRIFFChunk(Stream, data_mark)) return false;

	size = Stream.ReadI32LE();

	risse_int64 datastart;

	risse_int64 remain_size = Stream.GetSize() -
		(datastart = Stream.GetPosition());
	if(size > remain_size) return false;
		// data ends before "size" described in the header

	// compute total sample count and total length in time
	FileInfo.TotalSampleGranules = size / (FileInfo.Channels * (bits_per_sample / 8));
	FileInfo.TotalTime = FileInfo.TotalSampleGranules * 1000 / FileInfo.Frequency;

	// the stream is seekable
	FileInfo.Seekable = true;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRIFFWaveDecoder::FindRIFFChunk(tStreamAdapter stream, const risse_uint8 *chunk)
{
	risse_uint8 buf[4];
	while(true)
	{
		if(4 != stream.Read(buf, 4)) return false;
		if(memcmp(buf, chunk, 4))
		{
			// skip to next chunk
			risse_uint32 chunksize = stream.ReadI32LE();
			risse_uint64 next = stream.GetPosition() + chunksize;
			if(!stream.Seek(next, tStreamConstants::soSet)) return false;
		}
		else
		{
			return true;
		}
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * デコーダファクトリクラス
 */
class tRIFFWaveWaveDecoderFactory : public tWaveDecoderFactory
{
public:
	/**
	 * デコーダを作成する
	 */
	tWaveDecoder * Create(const tString & filename)
	{
		return new tRIFFWaveDecoder(filename);
	}
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * デコーダファクトリレジストラ
 */
class tRIFFWaveWaveDecoderFactoryRegisterer :
	public singleton_base<tRIFFWaveWaveDecoderFactoryRegisterer>,
	protected depends_on<tWaveDecoderFactoryManager>
{
public:
	/**
	 * コンストラクタ
	 */
	tRIFFWaveWaveDecoderFactoryRegisterer()
	{
		tWaveDecoderFactory * factory = new tRIFFWaveWaveDecoderFactory();
		tWaveDecoderFactoryManager::instance()->Register(RISSE_WS(".wav"), factory);
	}
	/**
	 * デストラクタ
	 */
	~tRIFFWaveWaveDecoderFactoryRegisterer()
	{
		tWaveDecoderFactoryManager::instance()->Unregister(RISSE_WS(".wav"));
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


