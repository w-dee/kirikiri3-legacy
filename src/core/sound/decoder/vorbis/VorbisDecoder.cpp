//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OggVorbis デコーダ
//---------------------------------------------------------------------------
#include "prec.h"
#include "sound/decoder/vorbis/VorbisDecoder.h"
#include "base/exception/RisaException.h"
#include "base/fs/common/FSManager.h"
#include <vorbis/vorbisfile.h>


RISSE_DEFINE_SOURCE_ID(11001,39824,8006,19566,26243,29715,33801,62487);



//---------------------------------------------------------------------------
tRisaOggVorbisDecoder::tRisaOggVorbisDecoder(const tRisseString & filename)
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
tRisaOggVorbisDecoder::~tRisaOggVorbisDecoder()
{
	ov_clear(&InputFile);
	delete Stream;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaOggVorbisDecoder::SuggestFormat(const tRisaWaveFormat & format)
{
	// float が求められている場合は float にする
	if(format.PCMType == tRisaPCMTypes::tf32)
		FileInfo.PCMType = tRisaPCMTypes::tf32;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaOggVorbisDecoder::GetFormat(tRisaWaveFileInfo & fileinfo)
{
	fileinfo = FileInfo;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaOggVorbisDecoder::Render(void *buf, risse_uint bufsamplelen, risse_uint& rendered)
{
	// render output PCM

	long res;
	risse_uint pos = 0; // decoded PCM (in bytes)

	if(FileInfo.PCMType == tRisaPCMTypes::ti16)
	{
		// 16bit 整数 PCM 出力の場合
		int pcmsize = 2;

	#if RISSE_HOST_IS_BIG_ENDIAN
		static const int endianp = 1;
	#else
		static const int endianp = 0;
	#endif

		int remain = bufsamplelen * FileInfo.Channels * pcmsize; // remaining PCM (in bytes)

		while(remain)
		{
			do
			{
				res = ov_read(&InputFile, ((char*)buf + pos), remain,
					endianp, pcmsize, 1, &CurrentSection); // decode via ov_read
			} while(res<0); // ov_read would return a negative number
							// if the decoding is not ready
			if(res==0) break;
			pos += res;
			remain -= res;
		}
		pos /= (FileInfo.Channels * pcmsize); // convert to PCM position
	}
	else if(FileInfo.PCMType == tRisaPCMTypes::tf32)
	{
		// 32bit float PCM 出力の場合
		float **pcm;
		int remain = bufsamplelen; // remaining PCM (in sample granules)
		int channels = FileInfo.Channels;

		while(remain)
		{
			do
			{
				res = ov_read_float(&InputFile, &pcm, remain, &CurrentSection); // decode via ov_read_float
			} while(res<0); // ov_read would return a negative number
							// if the decoding is not ready
			if(res==0) break;

			// インターリーブする
			for(int i = 0; i < channels; i++)
			{
				float * dest = (float*)((char*)buf + pos * sizeof(float) * channels) + i;
				const float * src = pcm[i];
				for(long j = 0; j < res; j++)
				{
					*dest = src[j];
					dest += channels;
				}
			}

			pos += res;
			remain -= res;
		}
	}


	rendered = pos; // return renderd PCM samples

	return pos == bufsamplelen;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaOggVorbisDecoder::SetPosition(risse_uint64 samplepos)
{
	if(0 != ov_pcm_seek(&InputFile, samplepos))
		return false;
	return true;
}
//---------------------------------------------------------------------------


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

	// set FileInfo up
	FileInfo.Frequency = vi->rate;
	FileInfo.Channels = vi->channels;
	FileInfo.PCMType = tRisaPCMTypes::tf32; // とりあえず整数 16bit

	bool seekable = true;

	risse_int64 pcmtotal = ov_pcm_total(&InputFile, -1); // PCM total samples
	if(pcmtotal<0) seekable = false, pcmtotal = 0;
	FileInfo.TotalSampleGranules = pcmtotal;

	double timetotal = ov_time_total(&InputFile, -1); // total time in sec.
	if(timetotal<0)
		seekable = false, FileInfo.TotalTime = 0;
	else
		FileInfo.TotalTime = static_cast<risse_uint64>(timetotal * 1000.0);

	FileInfo.SpeakerConfig = 0; // 何も指定しない
	FileInfo.Seekable = seekable;

	return true; // 成功
}
//---------------------------------------------------------------------------


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
int tRisaOggVorbisDecoder::seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	// seek function (wrapper for tRisseBinaryStream)

	tRisaOggVorbisDecoder * decoder = reinterpret_cast<tRisaOggVorbisDecoder*>(datasource);

	risse_int seek_type = RISSE_BS_SEEK_SET;

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

	try
	{
		decoder->Stream->Seek(offset, seek_type);
	}
	catch(...)
	{
		return -1; // failed
	}

	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tRisaOggVorbisDecoder::close_func(void *datasource)
{
	// close function (wrapper for tRisseBinaryStream)

	// なにもしない

	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
long tRisaOggVorbisDecoder::tell_func(void *datasource)
{
	// tell function (wrapper for tRisseBinaryStream)

	tRisaOggVorbisDecoder * decoder = reinterpret_cast<tRisaOggVorbisDecoder*>(datasource);

	return decoder->Stream->GetPosition();
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//! @brief		デコーダファクトリクラス
//---------------------------------------------------------------------------
class tRisaOggVorbisWaveDecoderFactory : public tRisaWaveDecoderFactory
{
public:
	//! @brief デコーダを作成する
	boost::shared_ptr<tRisaWaveDecoder> Create(const tRisseString & filename)
	{
		boost::shared_ptr<tRisaWaveDecoder>
			decoder(new tRisaOggVorbisDecoder(filename));
		return decoder;
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		デコーダファクトリレジストラ
//---------------------------------------------------------------------------
class tRisaOggVorbisWaveDecoderFactoryRegisterer :
	public singleton_base<tRisaOggVorbisWaveDecoderFactoryRegisterer>,
	protected depends_on<tRisaWaveDecoderFactoryManager>
{
public:
	//! @brief コンストラクタ
	tRisaOggVorbisWaveDecoderFactoryRegisterer()
	{
		boost::shared_ptr<tRisaWaveDecoderFactory>
			factory(new tRisaOggVorbisWaveDecoderFactory());
		depends_on<tRisaWaveDecoderFactoryManager>::locked_instance()->Register(RISSE_WS(".ogg"), factory);
	}
	//! @brief デストラクタ
	~tRisaOggVorbisWaveDecoderFactoryRegisterer()
	{
		depends_on<tRisaWaveDecoderFactoryManager>::locked_instance()->Unregister(RISSE_WS(".ogg"));
	}
};
//---------------------------------------------------------------------------




