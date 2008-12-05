//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
#include "risa/packages/risa/sound/WaveFilter.h"
#include "risa/packages/risa/sound/WaveSegmentQueue.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * OpenALバッファ
 */
class tALBuffer : public tCollectee, protected depends_on<tOpenAL>
{
public:
	// 定数など
	static const risse_uint STREAMING_BUFFER_HZ = 8; //!< ストリーミング時の1/(一つのバッファの時間)(調整可)
	static const risse_uint STREAMING_CHECK_SLEEP_MS = 70; //!< ストリーミング時のバッファをチェックする間隔(調整可)
	static const risse_uint MAX_NUM_BUFFERS = 4; //!< 一つの tALBuffer が保持する最大のバッファ数
	static const risse_uint MAX_NUM_RENDERBUFFERS = 16; //!< RenderBuffer の数(2の累乗である必要がある)

private:
	struct tInternalBuffers : public tDestructee
	{
		ALuint Buffers[MAX_NUM_BUFFERS]; //!< OpenAL バッファ
		risse_uint BufferAllocatedCount; //!< OpenAL バッファに実際に割り当てられたバッファ数
		tInternalBuffers(risse_uint alloc_count); //!< コンストラクタ
		~tInternalBuffers(); //!< デストラクタ
	};


	tCriticalSection * CS; //!< このオブジェクトを保護するクリティカルセクション
	tCriticalSection * RenderCS; //!< レンダリング(デコード)を保護するためのクリティカルセクション
	tInternalBuffers * Buffers; //!< バッファ
	ALuint FreeBuffers[MAX_NUM_BUFFERS]; //!< フリーのバッファ
	risse_uint FreeBufferCount; //!< フリーのバッファの数
	bool Streaming; //!< ストリーミングを行うかどうか
	tWaveFilter * Filter; //!< 入力フィルタ
	ALenum ALFormat; //!< OpenAL バッファの Format
	risse_uint ALFrequency; //!< OpenAL バッファのサンプリングレート
	risse_uint ALSampleGranuleBytes; //!< OpenAL バッファのbytes/sg
	risse_uint ALOneBufferRenderUnit; //!< ストリーミング時の一つのバッファのサンプル数

	/**
	 * デコードしたPCMを一時的に格納するための構造体
	 */
	struct tRenderBuffer
	{
		risse_uint8 * Buffer; //!< レンダリング用のテンポラリバッファ
		size_t Size; //!< RenderBuffer に割り当てられたサイズ(バイト単位)
		risse_uint Samples; //!< バッファに入っているサンプルグラニュール数
		tWaveSegmentQueue SegmentQueue; //!< セグメントキュー
	};

	tRenderBuffer RenderBuffers[MAX_NUM_RENDERBUFFERS]; //!< デコードしたPCMを一時的に格納するためのバッファ
	tAtomicCounter RenderBufferReadIndex; //!< RenderBuffer の読み込み用インデックス
	tAtomicCounter RenderBufferWriteIndex; //!< RenderBuffer の書き込み用インデックス
	tAtomicCounter RenderBufferRemain; //!< RenderBuffer の残りサイズ

	risse_uint8 * ConvertBuffer; //!< レンダリング用のPCM形式変換用のテンポラリバッファ
	size_t ConvertBufferSize; //!< ConvertBuffer に割り当てられたサイズ(バイト単位)

public:
	/**
	 * コンストラクタ
	 * @param filter	入力フィルタ
	 * @param streaming	ストリーミング再生を行うかどうか
	 */
	tALBuffer(tWaveFilter * Filter, bool streaming);

	tWaveFilter * GetFilter() { return Filter; } //!< 入力フィルタを得る

private:
	/**
	 * バッファに関するオブジェクトの解放などのクリーンアップ処理
	 */
	void Clear();

	/**
	 * 一時的に割り当てられたバッファの解放
	 */
	void FreeTempBuffers();

	/**
	 * レンダリング(デコード)を行う
	 * @param render_buffer			レンダリング用のテンポラリバッファ
	 * @param render_buffer_size	レンダリング用のテンポラリバッファのサイズ
	 * @param samples				最低でもこのサンプル数分詰めたい (0=デコードが終わるまで詰めたい)
	 *								戻り値trueで関数が戻ればここには実際にデコードされたサンプル数が入っている
	 * @param segmentqueue			再生セグメントキュー情報を書き込む先
	 * @return	バッファにデータが入ったら真
	 */
	bool Render(
		risse_uint8 * & render_buffer, size_t & render_buffer_size,
		risse_uint & samples,
		tWaveSegmentQueue & segmentqueue);

public:
	/**
	 * RenderBuffer の現在の残りバッファ個数を得る
	 * @note	まれに負の数が帰ることがあるので注意。この数値は参考値程度にみるべき。
	 */
	long GetRenderBufferRemain() const { return (long) RenderBufferRemain; }

	/**
	 * RenderBuffers を一つ埋める
	 * @return	バッファがいっぱいで埋まらなかった、あるいはデコードする物がないなどの理由で
	 *			デコードに失敗した場合は偽、埋まった場合は真
	 */
	bool FillRenderBuffer();

private:
	/**
	 * RenderBuffers からバッファを一つ盗ってくる
	 * @return	バッファが埋まればそのバッファへのポインタ、
	 *			埋まらなければリトライして、それでも駄目ならば NULL を返す
	 */
	tRenderBuffer * GetRenderBuffer();

	/**
	 * OpenALバッファにデータを詰める
	 * @param buffer		対象とする OpenAL バッファ
	 * @param segmentqueue	再生セグメントキュー情報を書き込む先
	 * @param samples		書き込まれたサンプルグラニュール数
	 * @return	バッファにデータが入ったら真
	 */
	bool FillALBuffer(ALuint buffer,
		tWaveSegmentQueue & segmentqueue, risse_uint & samples);

public:
	/**
	 * フリーになったバッファを FreeBuffers に push する
	 */
	void PushFreeBuffer(ALuint buffer);

	/**
	 * フリーのバッファがあるかどうかを返す
	 * @return	フリーのバッファがあるかどうか
	 */
	bool HasFreeBuffer();

	/**
	 * 空きバッファにデータをfillして返す
	 * @param buffer		バッファ番号を格納する変数
	 * @param segmentqueue	セグメントキュー
	 * @param samples		書き込まれたサンプルグラニュール数
	 * @return	fill に成功したか
	 */
	bool PopFilledBuffer(ALuint & buffer, tWaveSegmentQueue & segmentqueue, risse_uint & samples);

	/**
	 * 全てのバッファを解放する
	 */
	void FreeAllBuffers();

	/**
	 * OpenALバッファにサウンドをデコードしてコピーする
	 */
	void Load();

	bool GetStreaming() const { return Streaming; }
	ALuint GetBuffer() const { return Buffers->Buffers[0]; } // 非ストリーミング用

	risse_uint GetOneBufferRenderUnit() const { return ALOneBufferRenderUnit; }
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
