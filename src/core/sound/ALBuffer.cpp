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
#include "base/exception/RisaException.h"
#include "sound/ALCommon.h"
#include "sound/ALBuffer.h"
#include "sound/WaveFormatConverter.h"

RISSE_DEFINE_SOURCE_ID(24518,55437,60218,19380,17845,8848,1743,50558);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		filter 入力フィルタ
//! @param		streaming	ストリーミング再生を行うかどうか
//---------------------------------------------------------------------------
tRisaALBuffer::tRisaALBuffer(boost::shared_ptr<tRisaWaveFilter> filter, bool streaming)
{
	// フィールドの初期化
	BufferAllocatedCount  = 0;
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
		risse_uint alloc_count = Streaming ? STREAMING_NUM_BUFFERS : 1;
		alGenBuffers(alloc_count, Buffers);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alGenBuffers"));
		BufferAllocatedCount = alloc_count;

		// 非ストリーミングの場合はここで全てをデコードする
		if(!Streaming)
		{
			Load();
			Filter.reset(); // もうフィルタは要らない
			FreeTempBuffers(); // 一時的に割り当てられたバッファも解放
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
	tRisaCriticalSection::tLocker lock(CS);
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

	FreeTempBuffers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一時的に割り当てられたバッファの解放
//---------------------------------------------------------------------------
void tRisaALBuffer::FreeTempBuffers()
{
	free(RenderBuffer), RenderBuffer = NULL;
	RenderBufferSize = 0;
	free(ConvertBuffer), ConvertBuffer = NULL;
	ConvertBufferSize = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		OpenALバッファにデータを詰める
//! @param		buffer		対象とする OpenAL バッファ
//! @param		samples		最低でもこのサンプル数分詰めたい (0=デコードが終わるまで詰めたい)
//! @param		segmentqueue	再生セグメントキュー情報を書き込む先
//! @return		バッファにデータが入ったら真
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
				void * newbuffer = realloc(RenderBuffer, buffer_size_needed);
				if(!newbuffer)
				{
					free(RenderBuffer), RenderBuffer = NULL;
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
					void * newbuffer = realloc(ConvertBuffer, buffer_size_needed);
					if(!newbuffer)
					{
						free(ConvertBuffer), ConvertBuffer = NULL;
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
	tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alBufferData"));

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ストリーミング用バッファの準備
//---------------------------------------------------------------------------
void tRisaALBuffer::PrepareStream(ALuint source)
{
	tRisaCriticalSection::tLocker lock(CS);

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
	tRisaCriticalSection::tLocker lock(CS);

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
//		wxPrintf(wxT("buffer %u unqueued\n"), buffer);
	}
	else
	{
		// まだすべてはキューされていない
		buffer = Buffers[queued]; // まだキューされていないバッファ
//		wxPrintf(wxT("buffer %u to be used\n"), buffer);
	}


	// バッファにデータを流し込む
	// TODO: segments と events のハンドリング
	tRisaWaveSegmentQueue segumentqueue;
	bool filled = FillALBuffer(buffer, ALOneBufferRenderUnit, segumentqueue);

	// バッファにデータを割り当て、キューする
	if(filled)
	{

//		wxPrintf(wxT("buffer %u to be queued\n"), buffer);
		alSourceQueueBuffers(source, 1, &buffer);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourceQueueBuffers"));
	}

	return filled;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		全てのバッファをアンキューする
//! @param		全てのバッファをアンキューしたい source
//---------------------------------------------------------------------------
void tRisaALBuffer::UnqueueAllBuffers(ALuint source)
{
	tRisaCriticalSection::tLocker lock(CS);

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
