//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OpenAL バッファ管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "ALCommon.h"
#include "ALBuffer.h"

RISSE_DEFINE_SOURCE_ID(2304);


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		decoder		デコーダ
//! @param		streaming	ストリーミング再生を行うかどうか
//---------------------------------------------------------------------------
tRisaALBuffer::tRisaALBuffer(boost::shared_ptr<tRisaWaveDecoder> decoder, bool streaming)
{
	// フィールドの初期化
	BufferAllocatedCount  = 0;
	RenderBuffer = NULL;
	Streaming = streaming;
	Decoder = decoder;
	ALFormat = 0;

	// decoder のチェック
	// TODO: 正しいすべての形式のチェックと基本形式へのフォールバック
	decoder->GetFormat(Format);
	if     (!Format.IsFloat && Format.Channels == 1 && Format.BytesPerSample == 1)
		ALFormat = AL_FORMAT_MONO8;
	else if(!Format.IsFloat && Format.Channels == 2 && Format.BytesPerSample == 1)
		ALFormat = AL_FORMAT_STEREO8;
	else if(!Format.IsFloat && Format.Channels == 1 && Format.BytesPerSample == 2)
		ALFormat = AL_FORMAT_MONO16;
	else if(!Format.IsFloat && Format.Channels == 2 && Format.BytesPerSample == 2)
		ALFormat = AL_FORMAT_STEREO16;
	else
		eRisaException::Throw(RISSE_WS_TR("not acceptable PCM format"));

	try
	{
		// 一つのバッファサイズを計算し、RenderBuffer を確保する
		SampleGranuleBytes = Format.Channels * Format.BytesPerSample;
		if(Streaming)
		{
			OneBufferSampleGranules = Format.Frequency / STREAMING_BUFFER_HZ;
			RenderBuffer = new risse_uint8 [OneBufferSampleGranules * SampleGranuleBytes];
		}
		else
		{
			OneBufferSampleGranules = 0;
			RenderBuffer = NULL;
		}

		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		// バッファの生成
		risse_uint alloc_count = Streaming ? STREAMING_NUM_BUFFERS : 1;
		alGenBuffers(alloc_count, Buffers);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alGenBuffers"));
		BufferAllocatedCount = alloc_count;

		// 非ストリーミングの場合はここで全てをデコードする
		if(!Streaming)
		{
			Load();
			Decoder.reset(); // もうデコーダは要らない
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
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaALBuffer::~tRisaALBuffer()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		バッファに関するオブジェクトの解放などのクリーンアップ処理
//---------------------------------------------------------------------------
void tRisaALBuffer::Clear()
{
	volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

	alDeleteBuffers(BufferAllocatedCount, Buffers);
	tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alDeleteBuffers"));
	BufferAllocatedCount = 0;

	delete [] RenderBuffer, RenderBuffer = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ストリーミング用バッファの準備
//---------------------------------------------------------------------------
void tRisaALBuffer::PrepareStream(ALuint source)
{
	if(!Streaming) return;

	for(risse_uint i = 0; i < STREAMING_PREPARE_BUFFERS; i++)
		QueueStream(source);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ストリーミング用バッファのキューイング
//! @param		キュー先の source
//! @return		バッファに実際にキューされたか
//! @note		バッファがいっぱいの時は何もせずに返る
//---------------------------------------------------------------------------
bool tRisaALBuffer::QueueStream(ALuint source)
{
	ALuint  buffer = 0;

	// ストリーミングではない場合はそのまま返る
	if(!Streaming) return false;

	// ソースにキューされているバッファの数を得る
	ALint queued;

	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;
		alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
		tRisaOpenAL::instance()->ThrowIfError(
			RISSE_WS("alGetSourcei(AL_BUFFERS_QUEUED) at tRisaALBuffer::QueueStream"));
	}

	// バッファにすべてキューされている？
	if((risse_uint)queued == STREAMING_NUM_BUFFERS)
	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		// すべてキューされている
		// キューされているバッファをアンキューしないとならない
		ALint processed;
		alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
		tRisaOpenAL::instance()->ThrowIfError(
			RISSE_WS("alGetSourcei(AL_BUFFERS_PROCESSED)"));
		if(processed < 1) return false; // アンキュー出来るバッファがない

		// アンキューする
		alSourceUnqueueBuffers(source, 1, &buffer);
		tRisaOpenAL::instance()->ThrowIfError(
			RISSE_WS("alSourceUnqueueBuffers at tRisaALBuffer::QueueStream"));
		wxPrintf(wxT("buffer %u unqueued\n"), buffer);
	}
	else
	{
		// まだすべてはキューされていない
		buffer = Buffers[queued]; // まだキューされていないバッファ
		wxPrintf(wxT("buffer %u to be used\n"), buffer);
	}

	// バッファにデータをレンダリングする
	risse_uint remain = OneBufferSampleGranules;
	risse_uint rendered = 0;
	while(remain > 0)
	{
		risse_uint one_rendered;
		bool cont = Decoder->Render(
			RenderBuffer + rendered * SampleGranuleBytes,
			remain, one_rendered);
		if(remain == OneBufferSampleGranules && !cont)
		{
			// このループ初回かつ、デコード終了
			// もう完全にデコードが終わっているため、
			// バッファはキューしない
			return false;
		}

		rendered += one_rendered;
		remain -= one_rendered;
		if(!cont)
		{
			// デコードの終了
			// 残りを無音で埋める
			if(Format.BytesPerSample == 1)
				memset(
					RenderBuffer + rendered * SampleGranuleBytes,
					0x80, remain * SampleGranuleBytes); // 無音は 0x80
			else
				memset(
					RenderBuffer + rendered * SampleGranuleBytes,
					0x00, remain * SampleGranuleBytes); // 無音は 0
			break;
		}
	}

	// バッファにデータを割り当て、キューする
	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		wxPrintf(wxT("alBufferData: buffer %u, format %d, size %d, freq %d\n"), buffer,
			ALFormat, OneBufferSampleGranules * SampleGranuleBytes, Format.Frequency);
		alBufferData(buffer, ALFormat, RenderBuffer,
			OneBufferSampleGranules * SampleGranuleBytes, Format.Frequency);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("streaming alBufferData"));

		wxPrintf(wxT("buffer %u to be queued\n"), buffer);
		alSourceQueueBuffers(source, 1, &buffer);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourceQueueBuffers"));
	}

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		全てのバッファをアンキューする
//! @param		全てのバッファをアンキューしたい source
//---------------------------------------------------------------------------
void tRisaALBuffer::UnqueueAllBuffers(ALuint source)
{
	// ソースにキューされているバッファの数を得る
	volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

	ALint queued;
	alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
	tRisaOpenAL::instance()->ThrowIfError(
		RISSE_WS("alGetSourcei(AL_BUFFERS_QUEUED) at tRisaALBuffer::UnqueueAllBuffers"));

	if(queued > 0)
	{
		ALuint dummy_buffers[MAX_NUM_BUFFERS];

		// アンキューする
		alSourceUnqueueBuffers(source, queued, dummy_buffers);
		tRisaOpenAL::instance()->ThrowIfError(
			RISSE_WS("alSourceUnqueueBuffers at tRisaALBuffer::UnqueueAllBuffers"));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		OpenALバッファにサウンドをデコードしてコピーする
//---------------------------------------------------------------------------
void tRisaALBuffer::Load()
{
	// OpenAL バッファに Decoder からの入力を「すべて」デコードし、入れる

	// TODO: WaveLoopManager のリンク無効化 (そうしないと延々とサウンドを
	//       メモリが無くなるまでデコードし続ける)

	risse_uint8 * temp = NULL;
	risse_size temp_sample_granules = 0;

	int increase_size = 1024*512 / SampleGranuleBytes;

	try
	{
		// 初期の temp のサイズを決定
		if(static_cast<risse_size>(temp_sample_granules) == temp_sample_granules)
		{
			temp_sample_granules = Format.TotalSampleGranules ? Format.TotalSampleGranules : increase_size;
			temp = (risse_uint8*)malloc(temp_sample_granules * SampleGranuleBytes);
		}
		else
		{
			// 大きすぎる
			temp = NULL;
		}
		if(!temp)
			eRisaException::Throw(RISSE_WS_TR("can not play sound; too large to play"));

		risse_size rendered = 0;
		risse_size remain = temp_sample_granules;
		while(true)
		{
			risse_uint one_rendered;
			bool cont = Decoder->Render(
				temp + rendered * SampleGranuleBytes,
				remain, one_rendered);

			rendered += one_rendered;
			remain   -= one_rendered;

			if(!cont) break;

			if(remain == 0)
			{
				// 残りメモリが足りないので増やす
				temp = (risse_uint8*)realloc(temp, (temp_sample_granules + increase_size) * SampleGranuleBytes);
				if(!temp)
					eRisaException::Throw(RISSE_WS_TR("can not play sound; too large to play"));
				temp_sample_granules += increase_size;
				remain = increase_size;
			}
		}

		// バッファにデータを割り当てる
		alBufferData(Buffers[0], ALFormat, temp,
			temp_sample_granules * SampleGranuleBytes, Format.Frequency);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("non streaming alBufferData"));
	}
	catch(...)
	{
		if(temp) free(temp);
		throw;
	}

	if(temp) free(temp);
}
//---------------------------------------------------------------------------
