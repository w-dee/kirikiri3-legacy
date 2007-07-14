//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OpenAL バッファ管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/exception/RisaException.h"
#include "sound/ALCommon.h"
#include "sound/ALBuffer.h"
#include "sound/WaveFormatConverter.h"

RISSE_DEFINE_SOURCE_ID(24518,55437,60218,19380,17845,8848,1743,50558);



//---------------------------------------------------------------------------
tRisaALBuffer::tRisaALBuffer(boost::shared_ptr<tRisaWaveFilter> filter, bool streaming)
{
	// フィールドの初期化
	BufferAllocatedCount  = 0;
	FreeBufferCount = 0;
	RenderBuffer = NULL;
	RenderBufferSize = 0;
	ConvertBuffer = NULL;
	ConvertBufferSize = 0;
	Streaming = streaming;
	Filter = filter;
	ALFormat = 0;

	// filter のチェック
	// tRisaALBuffer は常に 16bit の OpenAL バッファを使う
	// (他の形式の場合は tRisaALBuffer 内部で変換を行うため)
	const tRisaWaveFormat & format = Filter->GetFormat();
	if     (format.Channels == 1)
		ALFormat = AL_FORMAT_MONO16;
	else if(format.Channels == 2)
		ALFormat = AL_FORMAT_STEREO16;
	else
		eRisaException::Throw(RISSE_WS_TR("not acceptable PCM channels (must be stereo or mono)"));

	ALFrequency = format.Frequency;
	ALSampleGranuleBytes = format.Channels * sizeof(risse_uint16);
	ALOneBufferRenderUnit = format.Frequency / STREAMING_BUFFER_HZ;

	try
	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		// バッファの生成
		risse_uint alloc_count = Streaming ? MAX_NUM_BUFFERS : 1;
		alGenBuffers(alloc_count, Buffers);
		depends_on<tRisaOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alGenBuffers"));
		BufferAllocatedCount = alloc_count;

		// 非ストリーミングの場合はここで全てをデコードする
		if(!Streaming)
		{
			Load();
			Filter.reset(); // もうフィルタは要らない
			FreeTempBuffers(); // 一時的に割り当てられたバッファも解放
		}
		else
		{
			// FreeBuffers にコピー
			memcpy(FreeBuffers, Buffers, sizeof(ALuint) * BufferAllocatedCount);
			FreeBufferCount = BufferAllocatedCount;
		}
	}
	catch(...)
	{
		Clear();
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaALBuffer::~tRisaALBuffer()
{
	tRisaCriticalSection::tLocker lock(CS);
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaALBuffer::Clear()
{
	volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

	alDeleteBuffers(BufferAllocatedCount, Buffers);
	depends_on<tRisaOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alDeleteBuffers"));
	BufferAllocatedCount = 0;
	FreeBufferCount = 0;

	FreeTempBuffers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaALBuffer::FreeTempBuffers()
{
	FreeCollectee(RenderBuffer), RenderBuffer = NULL;
	RenderBufferSize = 0;
	FreeCollectee(ConvertBuffer), ConvertBuffer = NULL;
	ConvertBufferSize = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaALBuffer::FillALBuffer(ALuint buffer, risse_uint samples,
	tRisaWaveSegmentQueue & segmentqueue)
{
	// バッファにデータをレンダリングする
	segmentqueue.Clear(); // queue はここでクリアされるので注意

	risse_uint remain = samples;
	if(remain == 0)
		remain = static_cast<risse_uint>(-1); // remain に整数の最大値を入れる

	risse_uint rendered = 0;
	while(remain > 0)
	{
		bool cont = true;
		risse_uint one_rendered = 0;
		risse_uint one_want = remain;

		// フィルタの現在のフォーマット形式をチェックする
		const tRisaWaveFormat & format = Filter->GetFormat();

		if(format.Frequency != ALFrequency)
			cont = false; // 周波数が変わった
		else if(
			ALFormat == AL_FORMAT_STEREO16 && format.Channels != 2 ||
			ALFormat == AL_FORMAT_MONO16   && format.Channels != 1)
			cont = false; // チャンネル数が変わった

		tRisaPCMTypes::tType filter_pcm_type = tRisaPCMTypes::tunknown;
		risse_uint filter_sample_granule_bytes = 0;

		if(cont)
		{
			filter_pcm_type = format.PCMType;
			filter_sample_granule_bytes = format.GetSampleGranuleSize();
		}

		// 一回のこのループ単位で要求するサイズを決定
		if(cont)
		{
			risse_uint max_request_granules = 512*1024 / filter_sample_granule_bytes;
			if(one_want > max_request_granules) one_want = max_request_granules;
		}

		// 形式変換の必要は？
		bool need_convert = filter_pcm_type != tRisaPCMTypes::ti16;

		// ところで書き込み先バッファのサイズは十分？
		if(cont)
		{
			// RenderBuffer のサイズをチェック
			size_t buffer_size_needed = ( rendered + one_want ) * ALSampleGranuleBytes;
			if(RenderBufferSize < buffer_size_needed)
			{
				void * newbuffer;
				if(RenderBuffer == NULL)
					newbuffer = MallocAtomicCollectee(RenderBuffer);
				else
					newbuffer = ReallocCollectee(RenderBuffer, buffer_size_needed);
				if(!newbuffer)
				{
					FreeCollectee(RenderBuffer), RenderBuffer = NULL;
					RenderBufferSize = 0;
					cont = false;
				}
				else
				{
					RenderBuffer = reinterpret_cast<risse_uint8 *>(newbuffer);
					RenderBufferSize = buffer_size_needed;
				}
			}

			if(need_convert)
			{
				// ConvertBuffer のサイズもチェック
				buffer_size_needed = one_want * filter_sample_granule_bytes;
				if(ConvertBufferSize < buffer_size_needed)
				{
					void * newbuffer;
					if(ConvertBuffer == NULL)
						newbuffer = MallocAtomicCollectee(ConvertBuffer);
					else
						newbuffer = ReallocCollectee(ConvertBuffer, buffer_size_needed);
					if(!newbuffer)
					{
						FreeCollectee(ConvertBuffer), ConvertBuffer = NULL;
						ConvertBufferSize = 0;
						cont = false;
					}
					else
					{
						ConvertBuffer = reinterpret_cast<risse_uint8 *>(newbuffer);
						ConvertBufferSize = buffer_size_needed;
					}
				}
			}
		}

		// フィルタのデータの書き込み先を決定
		risse_uint8 * filter_destination = NULL;
		if(cont)
		{
			if(!need_convert)
			{
				// フィルタの出力タイプが 整数 16bit なので直接出力バッファに書き込む
				filter_destination = RenderBuffer + rendered * ALSampleGranuleBytes;
			}
			else
			{
				// フィルタの出力タイプが 整数 16bit ではないのでいったん変換バッファに書き込む
				filter_destination = ConvertBuffer;
			}
		}

		// フィルタにデータを要求する
		if(cont)
			cont = Filter->Render(
				filter_destination,
				one_want, one_rendered, segmentqueue);

		// ループの初回でデコードが終了したか？
		if(rendered == 0 && !cont)
		{
			// このループ初回かつ、デコード終了
			return false;
		}

		// PCM 形式の変換
		if(need_convert)
		{
			tRisaWaveFormatConverter::Convert(tRisaPCMTypes::ti16,
				RenderBuffer + rendered * ALSampleGranuleBytes,
				filter_pcm_type,
				ConvertBuffer,
				ALFormat == AL_FORMAT_STEREO16 ? 2 : ALFormat == AL_FORMAT_MONO16 ? 1 : 0,
				one_rendered);
		}

		// カウンタを進める
		rendered += one_rendered;
		remain -= one_rendered;

		if(!cont)
		{
			// デコードの終了
			// 残りを無音で埋める
			if(samples != 0)
			{
				memset(
					RenderBuffer + rendered * ALSampleGranuleBytes,
					0x00, remain * ALSampleGranuleBytes); // 無音は 0
			}
			break;
		}
	}

	// バッファにデータを割り当てる

	volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

//	wxPrintf(wxT("alBufferData: buffer %u, format %d, size %d, freq %d\n"), buffer,
//			ALFormat, ALSampleGranuleBytes * rendered, ALFrequency);
	alBufferData(buffer, ALFormat, RenderBuffer,
		rendered * ALSampleGranuleBytes, ALFrequency);
	depends_on<tRisaOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alBufferData"));

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaALBuffer::PushFreeBuffer(ALuint buffer)
{
	tRisaCriticalSection::tLocker lock(CS);

	FreeBuffers[FreeBufferCount++] = buffer;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaALBuffer::HasFreeBuffer()
{
	tRisaCriticalSection::tLocker lock(CS);

	return FreeBufferCount > 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaALBuffer::PopFilledBuffer(ALuint & buffer, tRisaWaveSegmentQueue & segmentqueue)
{
	tRisaCriticalSection::tLocker lock(CS);

	// ストリーミングではない場合はそのまま返る
	if(!Streaming) return false;

	// フリーのバッファが無い場合はそのまま返る
	if(!FreeBufferCount) return false;

	// バッファにデータを流し込む
	buffer = FreeBuffers[FreeBufferCount - 1];
	bool filled = FillALBuffer(buffer, ALOneBufferRenderUnit, segmentqueue);

	// FreeBufferCount を減らす
	if(filled) FreeBufferCount --;

	return filled;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaALBuffer::FreeAllBuffers()
{
	tRisaCriticalSection::tLocker lock(CS);

	// すべてのバッファを解放したことにする
	memcpy(FreeBuffers, Buffers, sizeof(ALuint) * BufferAllocatedCount);
	FreeBufferCount = BufferAllocatedCount;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaALBuffer::Load()
{
	tRisaCriticalSection::tLocker lock(CS);

	// OpenAL バッファに Filter からの入力を「すべて」デコードし、入れる

	// TODO: WaveLoopManager のリンク無効化 (そうしないと延々とサウンドを
	//       メモリが無くなるまでデコードし続ける)
	// TODO: segments と events のハンドリング
	tRisaWaveSegmentQueue segumentqueue;
	bool filled = FillALBuffer(Buffers[0], 0, segumentqueue);

	if(!filled)
		eRisaException::Throw(RISSE_WS_TR("no data to play"));
}
//---------------------------------------------------------------------------
