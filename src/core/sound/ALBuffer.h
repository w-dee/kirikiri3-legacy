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
#ifndef ALBufferH
#define ALBufferH

#include <AL/al.h>
#include <AL/alc.h>
#include "sound/WaveFilter.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		OpenALバッファ
//---------------------------------------------------------------------------
class tALBuffer : public tCollectee, protected depends_on<tOpenAL>
{
public:
	// 定数など
	static const risse_uint STREAMING_BUFFER_HZ = 8; //!< ストリーミング時の1/(一つのバッファの時間)(調整可)
	static const risse_uint STREAMING_CHECK_SLEEP_MS = 70; //!< ストリーミング時のバッファをチェックする間隔(調整可)
	static const risse_uint MAX_NUM_BUFFERS = 4; //!< 一つの tALBuffer が保持する最大のバッファ数
	static const risse_uint MAX_NUM_RENDERBUFFERS = 16; //!< RenderBuffer の数

private:
	struct tInternalBuffers : public tDestructee
	{
		ALuint Buffers[MAX_NUM_BUFFERS]; //!< OpenAL バッファ
		risse_uint BufferAllocatedCount; //!< OpenAL バッファに実際に割り当てられたバッファ数
		tInternalBuffers(risse_uint alloc_count); //!< コンストラクタ
		~tInternalBuffers(); //!< デストラクタ
	};


	tCriticalSection * CS; //!< このオブジェクトを保護するクリティカルセクション
	tInternalBuffers * Buffers; //!< バッファ
	ALuint FreeBuffers[MAX_NUM_BUFFERS]; //!< フリーのバッファ
	risse_uint FreeBufferCount; //!< フリーのバッファの数
	bool Streaming; //!< ストリーミングを行うかどうか
	tWaveFilter * Filter; //!< 入力フィルタ
	ALenum ALFormat; //!< OpenAL バッファの Format
	risse_uint ALFrequency; //!< OpenAL バッファのサンプリングレート
	risse_uint ALSampleGranuleBytes; //!< OpenAL バッファのbytes/sg
	risse_uint ALOneBufferRenderUnit; //!< ストリーミング時の一つのバッファのサンプル数

	risse_uint8 * RenderBuffer; //!< レンダリング用のテンポラリバッファ
	size_t RenderBufferSize; //!< RenderBuffer に割り当てられたサイズ(バイト単位)

	risse_uint8 * ConvertBuffer; //!< レンダリング用のテンポラリバッファ
	size_t ConvertBufferSize; //!< RenderBuffer に割り当てられたサイズ(バイト単位)

public:
	//! @brief		コンストラクタ
	//! @param		filter 入力フィルタ
	//! @param		streaming	ストリーミング再生を行うかどうか
	tALBuffer(tWaveFilter * Filter, bool streaming);

	tWaveFilter * GetFilter() { return Filter; } //!< 入力フィルタを得る

private:
	//! @brief		バッファに関するオブジェクトの解放などのクリーンアップ処理
	void Clear();

	//! @brief		一時的に割り当てられたバッファの解放
	void FreeTempBuffers();

	//! @brief		レンダリング用のテンポラリバッファにデータを詰める
	//! @param		render_buffer	レンダリング用のテンポラリバッファ
	//! @param		render_buffer_size	レンダリング用のテンポラリバッファのサイズ
	//! @param		samples		最低でもこのサンプル数分詰めたい (0=デコードが終わるまで詰めたい)
	//!							戻り値trueで関数が戻ればここには実際にデコードされたサンプル数が入っている
	//! @param		segmentqueue	再生セグメントキュー情報を書き込む先
	//! @return		バッファにデータが入ったら真
	bool FillRenderBuffer(
		risse_uint8 * & render_buffer, size_t & render_buffer_size,
		risse_uint & samples,
		tWaveSegmentQueue & segmentqueue);

	//! @brief		OpenALバッファにデータを詰める
	//! @param		buffer		対象とする OpenAL バッファ
	//! @param		samples		最低でもこのサンプル数分詰めたい (0=デコードが終わるまで詰めたい)
	//! @param		segmentqueue	再生セグメントキュー情報を書き込む先
	//! @return		バッファにデータが入ったら真
	bool FillALBuffer(ALuint buffer, risse_uint samples,
		tWaveSegmentQueue & segmentqueue);

public:
	//! @brief		フリーになったバッファを FreeBuffers に push する
	void PushFreeBuffer(ALuint buffer);

	//! @brief		フリーのバッファがあるかどうかを返す
	//! @return 	フリーのバッファがあるかどうか
	bool HasFreeBuffer();

	//! @brief		空きバッファにデータをfillして返す
	//! @param		buffer バッファ番号を格納する変数
	//! @param		segmentqueue セグメントキュー
	//! @return		fill に成功したか
	bool PopFilledBuffer(ALuint & buffer, tWaveSegmentQueue & segmentqueue);

	//! @brief		全てのバッファを解放する
	void FreeAllBuffers();

	//! @brief		OpenALバッファにサウンドをデコードしてコピーする
	void Load();

	bool GetStreaming() const { return Streaming; }
	ALuint GetBuffer() const { return Buffers->Buffers[0]; } // 非ストリーミング用

	risse_uint GetOneBufferRenderUnit() const { return ALOneBufferRenderUnit; }
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
