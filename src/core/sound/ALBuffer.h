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

#include <al.h>
#include <alc.h>
#include "sound/WaveFilter.h"

//---------------------------------------------------------------------------
//! @brief		OpenALバッファ
//---------------------------------------------------------------------------
class tRisaALBuffer : protected depends_on<tRisaOpenAL>
{
public:
	// 定数など
	static const risse_uint STREAMING_BUFFER_HZ = 8; //!< ストリーミング時の1/(一つのバッファの時間)(調整可)
	static const risse_uint STREAMING_CHECK_SLEEP_MS = 70; //!< ストリーミング時のバッファをチェックする間隔(調整可)
	static const risse_uint MAX_NUM_BUFFERS = 16; //!< 一つの tRisaALBuffer が保持する最大のバッファ数

private:
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	ALuint Buffers[MAX_NUM_BUFFERS]; //!< OpenAL バッファ
	risse_uint BufferAllocatedCount; //!< OpenAL バッファに実際に割り当てられたバッファ数
	ALuint FreeBuffers[MAX_NUM_BUFFERS]; //!< フリーのバッファ
	risse_uint FreeBufferCount; //!< フリーのバッファの数
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

	boost::shared_ptr<tRisaWaveFilter> & GetFilter() { return Filter; } //!< 入力フィルタを得る

private:
	void Clear();
	void FreeTempBuffers();

	bool FillALBuffer(ALuint buffer, risse_uint samples,
		tRisaWaveSegmentQueue & segmentqueue);

public:
	void PushFreeBuffer(ALuint buffer);
	bool HasFreeBuffer();
	bool PopFilledBuffer(ALuint & buffer, tRisaWaveSegmentQueue & segmentqueue);
	void FreeAllBuffers();
	void Load();

	bool GetStreaming() const { return Streaming; }
	ALuint GetBuffer() const { return Buffers[0]; } // 非ストリーミング用

	risse_uint GetOneBufferRenderUnit() const { return ALOneBufferRenderUnit; }
};
//---------------------------------------------------------------------------

#endif
