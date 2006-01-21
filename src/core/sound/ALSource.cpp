//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OpenAL ソース管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "Singleton.h"
#include "ALSource.h"



RISSE_DEFINE_SOURCE_ID(2305);

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
//! @brief		再生の停止
//---------------------------------------------------------------------------
void tRisaALSource::Stop()
{
	// 再生を停止する
	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		alSourceStop(Source);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcePlay"));
	}

	if(Buffer->GetStreaming())
	{
		// 全てのバッファを unqueueする
		Buffer->UnqueueAllBuffers(Source);
	}
}
//---------------------------------------------------------------------------
