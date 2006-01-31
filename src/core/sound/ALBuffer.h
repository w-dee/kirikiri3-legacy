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
#include "WaveFilter.h"

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
	boost::shared_ptr<tRisaWaveFilter> Filter; //!< 入力フィルタ
	ALenum ALFormat; //!< OpenAL バッファの Format
	risse_uint ALFrequency; //!< OpenAL バッファのサンプリングレート
	risse_uint ALSampleGranuleBytes; //!< OpenAL バッファのbytes/sg
	risse_uint ALOneBufferRenderUnit; //!< ストリーミング時の一つのバッファのサンプル数

	risse_uint8 * RenderBuffer; //!< レンダリング用のテンポラリバッファ
	size_t RenderBufferSize; //!< RenderBuffer に割り当てられたサイズ(バイト単位)

	risse_uint8 * ConvertBuffer; //!< レンダリング用のテンポラリバッファ
	size_t ConvertBufferSize; //!< RenderBuffer に割り当てられたサイズ(バイト単位)

public:
	tRisaALBuffer(boost::shared_ptr<tRisaWaveFilter> Filter, bool streaming);
	~tRisaALBuffer();

private:
	void Clear();
	void FreeTempBuffers();

	bool FillALBuffer(ALuint buffer, risse_uint samples,
		tRisaWaveSegmentQueue & segmentqueue);

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
