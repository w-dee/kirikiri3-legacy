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
#ifndef ALBufferH
#define ALBufferH

#include "al.h"
#include "alc.h"

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

public:
	tRisaALBuffer(boost::shared_ptr<tRisaWaveDecoder> decoder, bool streaming);
	~tRisaALBuffer();

private:
	void Clear();

public:
	void PrepareStream(ALuint source);
	bool QueueStream(ALuint source);
	void UnqueueAllBuffers(ALuint source);
	void Load();

	bool GetStreaming() const { return Streaming; }
	ALuint GetBuffer() const { return Buffers[0]; } // 非ストリーミング用
};
//---------------------------------------------------------------------------

#endif
