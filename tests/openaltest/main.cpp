// openal を使って音声をストリーミング再生するテスト

#include "prec.h"
#include "Singleton.h"
#include "RisaException.h"
#include "RisseEngine.h"
#include "WaveDecoder.h"
#include "RIFFWaveDecoder.h"
#include "VorbisDecoder.h"
#include "al.h"
#include "alc.h"


RISSE_DEFINE_SOURCE_ID(1);

/*
	some notes

	OpenAL API を実行するときは基本的に

	tRisaOpenAL::tCriticalSectionHolder cs_holder;

	で OpenAL API を保護し、その後

	tRisaOpenAL::instance()->ThrowIfError();

	でエラーをチェックするという方法をとること。

	OpenAL の API は API 実行後に alGetError でエラーコードを取得するという
	方法をとるが、API 実行とalGetErrorの間に、ほかのスレッドが他の API を
	実行してしまうことによって alGetError が正しい値を得ないという可能性がある。
	
*/

//---------------------------------------------------------------------------
//! @brief		OpenAL管理クラス
//---------------------------------------------------------------------------
class tRisaOpenAL
{
public:
	//! @brief OpenAL APIを保護するためのクリティカルセクションホルダ
	struct tCriticalSectionHolder
	{
		tRisseCriticalSectionHolder holder;
		tCriticalSectionHolder() : holder(tRisaOpenAL::instance()->GetCS())
		{
			// エラー状態をクリアする
			tRisaOpenAL::instance()->ClearErrorState();
		}
	};


private:
	ALCdevice * Device; //!< デバイス
	ALCcontext * Context; //!< コンテキスト

	tRisseCriticalSection CS; //!< OpenAL API を保護する CS


public:
	tRisaOpenAL();
	~tRisaOpenAL();

private:
	void Clear();

public:
	void ThrowIfError(const risse_char * message);
	void ClearErrorState();
	tRisseCriticalSection & GetCS() { return CS; }


private:
	tRisaSingletonObjectLifeTracer<tRisaOpenAL> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tRisaOpenAL> & instance() { return
		tRisaSingleton<tRisaOpenAL>::instance();
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaOpenAL::tRisaOpenAL()
{
	// フィールドの初期化
	Device = NULL;
	Context = NULL;

	// デバイスを開く
	Device = alcOpenDevice(NULL);
		// TODO: デバイスの選択と null デバイスへのフォールバック

	if(!Device)
		eRisaException::Throw(RISSE_WS_TR("failed to create OpenAL device"));

	try
	{
		// コンテキストを作成する
		Context = alcCreateContext(Device, NULL);
		ThrowIfError(RISSE_WS("alcCreateContext"));

		// コンテキストを選択する
		alcMakeContextCurrent(Context);
		ThrowIfError(RISSE_WS("alcMakeContextCurrent"));
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
tRisaOpenAL::~tRisaOpenAL()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		クリーンアップ処理
//---------------------------------------------------------------------------
void tRisaOpenAL::Clear()
{
	if(Context)
	{
		alcMakeContextCurrent(NULL);
		alcDestroyContext(Context), Context = NULL;
	}

	if(Device)
	{
		alcCloseDevice(Device), Device = NULL;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		現在のエラーに対応する例外を投げる
//! @param		message メッセージ
//! @note		エラーが何も発生していない場合は何もしない
//---------------------------------------------------------------------------
void tRisaOpenAL::ThrowIfError(const risse_char * message)
{
	ALCenum err = alGetError();
	if(err == AL_NO_ERROR) return ; // エラーはなにも起きていない
	const ALCchar *msg = alcGetString(Device, err);
	eRisaException::Throw(RISSE_WS_TR("OpenAL error in %1 : %2"),
		ttstr(message),
		ttstr(wxString(msg, wxConvUTF8)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		OpenAL のエラー状態をクリアする
//---------------------------------------------------------------------------
void tRisaOpenAL::ClearErrorState()
{
	alGetError();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		OpenALバッファ
//---------------------------------------------------------------------------
class tRisaALBuffer
{
public:
	// 定数など
	static const risse_uint STREAMING_BUFFER_HZ = 8; //!< ストリーミング時の1/(一つのバッファの時間)(調整可)
	static const risse_uint STREAMING_CHECK_SLEEP_MS = 70; //!< ストリーミング時のバッファをチェックする間隔(調整可)
	static const risse_uint STREAMING_NUM_BUFFERS = 16; //!< ストリーミング時のバッファの数(調整可)
	static const risse_uint STREAMING_PREPARE_BUFFERS = 4; //!< 再生開始前にソースにキューしておくバッファの数
	static const risse_uint MAX_NUM_BUFFERS = STREAMING_NUM_BUFFERS; //!< 一つの tRisaALSource が保持する最大のバッファ数

private:
	ALuint Buffers[MAX_NUM_BUFFERS]; //!< OpenAL バッファ
	risse_uint BufferAllocatedCount; //!< OpenAL バッファに実際に割り当てられたバッファ数
	bool Streaming; //!< ストリーミングを行うかどうか
	boost::shared_ptr<tRisaWaveDecoder> Decoder; //!< デコーダ
	ALenum ALFormat; //!< OpenAL Format
	tRisaWaveFormat Format; //!< Risa 形式の Format descripter

	risse_uint SampleGranuleBytes; //!< サンプルグラニュールのバイト数 = Channels * BytesPerSample
	risse_uint OneBufferSampleGranules; //!< 一つのバッファのサイズ (サンプルグラニュール単位)
	risse_uint8 * RenderBuffer; //!< レンダリング用のテンポラリバッファ
	risse_uint BufferQueuedCount; //!< キューに入っているバッファの数

public:
	tRisaALBuffer(boost::shared_ptr<tRisaWaveDecoder> decoder, bool streaming);
	~tRisaALBuffer();

private:
	void Clear();

public:
	void PrepareStream(ALuint source);
	bool QueueStream(ALuint source);
	void Load();

	bool GetStreaming() const { return Streaming; }
	ALuint GetBuffer() const { return Buffers[0]; } // 非ストリーミング用
};
//---------------------------------------------------------------------------


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
	BufferQueuedCount = 0;
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

	// バッファにすべてキューされている？
	if(BufferQueuedCount == STREAMING_NUM_BUFFERS)
	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		// すべてキューされている
		// キューされているバッファをアンキューしないとならない
		ALint processed;
		alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alGetSourcei(AL_BUFFERS_PROCESSED)"));
		if(processed < 1) return false; // アンキュー出来るバッファがない

		// アンキューする
		alSourceUnqueueBuffers(source, 1, &buffer);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourceUnqueueBuffers"));
		wxPrintf(wxT("buffer %u unqueued\n"), buffer);
	}
	else
	{
		// まだすべてはキューされていない
		buffer = Buffers[BufferQueuedCount]; // まだキューされていないバッファ
		BufferQueuedCount ++;
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









//---------------------------------------------------------------------------
//! @brief		OpenALソース
//---------------------------------------------------------------------------
class tRisaALSource
{
	risse_uint NumBuffersQueued; //!< キューに入っているバッファの数
	ALuint Source; //!< OpenAL ソース
	bool SourceAllocated; //!< Source がすでに割り当てられているかどうか
	tRisaSingleton<tRisaOpenAL> ref_tRisaOpenAL; //!< tRisaOpenAL インスタンス
	boost::shared_ptr<tRisaALBuffer> Buffer; // バッファ

public:
	tRisaALSource(boost::shared_ptr<tRisaALBuffer> buffer);
	~tRisaALSource();

	ALuint GetSource() const { return Source; } // Source を得る

private:
	void Clear();

public:
	void Play();

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		decoder		デコーダ
//! @param		streaming	ストリーミング再生を行うかどうか
//---------------------------------------------------------------------------
tRisaALSource::tRisaALSource(boost::shared_ptr<tRisaALBuffer> buffer) :
	Buffer(buffer)
{
	// フィールドの初期化
	SourceAllocated = false;

	try
	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		// ソースの生成
		alGenSources(1, &Source);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alGenSources"));
		SourceAllocated = true;

		alSourcei(Source, AL_LOOPING, AL_FALSE);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcei(AL_LOOPING)"));

		// ストリーミングを行わない場合は、バッファをソースにアタッチ
		if(!Buffer->GetStreaming())
		{
			alSourcei(Source, AL_BUFFER, Buffer->GetBuffer());
			tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcei(AL_BUFFER)"));
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
tRisaALSource::~tRisaALSource()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		バッファに関するオブジェクトの解放などのクリーンアップ処理
//---------------------------------------------------------------------------
void tRisaALSource::Clear()
{
	volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

	if(SourceAllocated) alDeleteSources(1, &Source);
	tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alDeleteSources"));
	SourceAllocated = false;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		再生の開始
//---------------------------------------------------------------------------
void tRisaALSource::Play()
{
	if(Buffer->GetStreaming())
	{
		// 初期サンプルをいくつか queue する
		Buffer->PrepareStream(Source);
	}

	// 再生を開始する
	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		alSourcePlay(Source);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcePlay"));
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		アプリケーションクラス
//---------------------------------------------------------------------------
class Application : public wxAppConsole
{
public:
	virtual bool OnInit();
	virtual int OnRun();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// アプリケーションメインルーチン定義
//---------------------------------------------------------------------------
IMPLEMENT_APP_CONSOLE(Application)
wxLocale locale;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		アプリケーションが開始するとき
//! @return		成功すれば真
//---------------------------------------------------------------------------
bool Application::OnInit()
{
	locale.Init(wxLANGUAGE_DEFAULT);
	locale.AddCatalogLookupPathPrefix(wxT("locales")); 
	locale.AddCatalogLookupPathPrefix(wxT("../locales")); 
	locale.AddCatalog(wxT("openaltest"));
	return true;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		アプリケーションメインルーチン
//! @return		終了コード
//---------------------------------------------------------------------------
int Application::OnRun()
{
	try
	{
		tRisaSingletonManager::InitAll(); // 全てのシングルトンインスタンスを初期化

		tRisaRisseScriptEngine::instance()->GetEngineNoAddRef()->EvalExpression(
			RISSE_WS("FileSystem.mount('/', new FileSystem.OSFS('.'))"),
			NULL, NULL, NULL);

		boost::shared_ptr<tRisaWaveDecoder> decoder(new tRisaRIFFWaveDecoder(RISSE_WS("test.wav")));
//		boost::shared_ptr<tRisaWaveDecoder> decoder(new tRisaOggVorbisDecoder(RISSE_WS("test.ogg")));
		boost::shared_ptr<tRisaALBuffer> buffer(new tRisaALBuffer(decoder, true));
		tRisaALSource source(buffer);

		source.Play();

		while(true)
		{
			buffer->QueueStream(source.GetSource());
			Sleep(100);
			ALint pos;
			alGetSourcei( source.GetSource(), AL_SAMPLE_OFFSET, &pos);
			wxPrintf(wxT("position : %d\n"), pos);
		}
	}
	catch(const eRisse &e)
	{
		wxFprintf(stderr, wxT("error : %s\n"), e.GetMessage().AsWxString().c_str());
	}

	return 0;
}
//---------------------------------------------------------------------------

