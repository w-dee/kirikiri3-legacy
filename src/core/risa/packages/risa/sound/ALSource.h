//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OpenAL ソース管理
//---------------------------------------------------------------------------
#ifndef ALSourceH
#define ALSourceH

#include <AL/al.h>
#include <AL/alc.h>
#include "risa/packages/risa/sound/ALCommon.h"
#include "risa/packages/risa/sound/ALBuffer.h"
#include "risa/packages/risa/sound/WaveSegmentQueue.h"
#include "risa/packages/risa/sound/WaveLoopManager.h"
#include "risa/common/RisaThread.h"
#include "risa/common/Singleton.h"
#include "risa/packages/risa/event/Event.h"
#include "risseNativeBinder.h"

namespace Risa {
//---------------------------------------------------------------------------

class tALSource;
//---------------------------------------------------------------------------
/**
 * 監視用スレッド
 * @note	監視用スレッドは、約50ms周期のコールバックをすべての
 *			ソースに発生させる。ソースではいくつかポーリングを
 *			行わなければならない場面でこのコールバックを利用する。
 */
class tWaveWatchThread :
	public singleton_base<tWaveWatchThread>,
	manual_start<tWaveWatchThread>,
	public tThread
{
	tThreadEvent Event; //!< スレッドをたたき起こすため/スレッドを眠らせるためのイベント

public:
	/**
	 * コンストラクタ
	 */
	tWaveWatchThread();

	/**
	 * デストラクタ
	 */
	~tWaveWatchThread();

	/**
	 * 眠っているスレッドを叩き起こす
	 */
	void Wakeup();

protected:
	/**
	 * スレッドのエントリーポイント
	 */
	void Execute(void);
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * tALSourceのステータスを含むクラス
 */
class tALSourceStatus
{
public:
	/**
	 * サウンドソースの状態
	 */
	enum tStatus
	{
		ssUnload /*!< data is not specified (tALSourceではこの状態は存在しない) */,
		ssStop /*!< stopping */,
		ssPlay /*!< playing */,
		ssPause /*!< pausing */,
	};
};
//---------------------------------------------------------------------------


}
namespace Risse {
//---------------------------------------------------------------------------
/**
 * NativeBinder 用の Variant -> tALSourceStatus::tStatus 変換定義
 */
template <>
inline Risa::tALSourceStatus::tStatus FromVariant<Risa::tALSourceStatus::tStatus>(const tVariant & v)
{
	return (Risa::tALSourceStatus::tStatus)(int)(risse_int64)v;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * NativeBinder 用の tALSourceStatus::tStatus -> Variant 変換定義
 */
template <>
inline tVariant ToVariant<Risa::tALSourceStatus::tStatus>(Risa::tALSourceStatus::tStatus s)
{
	return tVariant((risse_int64)(int)s);
}
//---------------------------------------------------------------------------
}
namespace Risa {




class tWaveDecodeThread;
//---------------------------------------------------------------------------
/**
 * OpenALソース
 */
class tALSource :
				public tCollectee,
				protected depends_on<tOpenAL>,
				protected depends_on<tWaveWatchThread>,
				protected depends_on<tEventSystem>,
				public tALSourceStatus
{
	friend class tWaveDecodeThread;
	friend class tWaveWatchThread;
public:
	// 定数など
	static const risse_uint STREAMING_NUM_BUFFERS = tALBuffer::MAX_NUM_BUFFERS; //!< ストリーミング時のバッファの数(調整可)
	static const risse_uint STREAMING_PREPARE_BUFFERS = 4; //!< 再生開始前にソースにキューしておくバッファの数

private:
	struct tInternalSource : public tDestructee
	{
		ALuint Source; //!< OpenAL ソース
		tInternalSource(); //!< コンストラクタ
		~tInternalSource(); //!< デストラクタ
	};

	tCriticalSection * CS; //!< このオブジェクトを保護するクリティカルセクション
	tCriticalSection * BufferCS; //!< バッファへのキューイングを保護するクリティカルセクション
	risse_uint NumBuffersQueued; //!< キューに入っているバッファの数
	tInternalSource * Source;
	tALBuffer * Buffer; //!< バッファ
	tWaveLoopManager * LoopManager; //!< ループマネージャ
	bool NeedRewind; //!< リワインド (巻き戻し) が必要な場合に真
	tWaveDecodeThread * DecodeThread; //!< デコードスレッド
	tStatus Status; //!< サウンドステータス
	tStatus PrevStatus; //!< 直前のサウンドステータス

	/**
	 * 一つの OpenAL バッファに対応するセグメントの情報
	 */
	struct tSegmentInfo
	{
		tWaveSegmentQueue SegmentQueue; //!< セグメントキュー
		risse_uint64 DecodePosition; //!< デコードを開始した任意の原点からの相対デコード位置(サンプルグラニュール単位)
	};
	gc_deque<tSegmentInfo> SegmentQueues; //!< セグメントキューの配列
	gc_deque<tWaveEvent> SegmentEvents; //!< イベントの配列

	risse_uint64 DecodePosition; //!< デコードした総サンプル数

public:
	/**
	 * コンストラクタ
	 * @param buffer	OpenAL バッファを管理する tALBuffer インスタンス
	 */
	tALSource(tALBuffer * buffer,
		tWaveLoopManager * loopmanager = NULL);

	/**
	 * コンストラクタ(ほかのtALSourceとバッファを共有する場合)
	 * @param ref	コピー元ソース
	 */
	tALSource(const tALSource * ref);

	/**
	 * デストラクタ
	 */
	virtual ~tALSource() {;}

private:
	/**
	 * オブジェクトを初期化する
	 * @param buffer	OpenAL バッファを管理する tALBuffer インスタンス
	 */
	void Init(tALBuffer * buffer);

	/**
	 * Source の存在を確実にする
	 */
	void EnsureSource();

	/**
	 * Source を強制的に削除する
	 */
	void DeleteSource();

public:
	ALuint GetSource() const { if(Source) return Source->Source; else return 0; } //!< Source を得る

private: //---- queue/buffer management
	/**
	 * レンダリング(デコード)を行う
	 * @note	残り容量が少ないと偽を返す
	 */
	bool Render();

	/**
	 * バッファのデータを埋める
	 */
	void FillBuffer();

	/**
	 * すべてのバッファをアンキューする
	 */
	void UnqueueAllBuffers();

	/**
	 * バッファをソースにキューする
	 * @note	キューできない場合は何もしない
	 */
	void QueueBuffer();

	/**
	 * 状態をチェックする
	 * @note	OpenAL による実際の再生状況と、このクラス内の管理情報が
	 *			異なる場合がある(特に再生停止時)ため、その状態を再度チェックするためにある。
	 *			クリティカルセクションによる保護は別の場所で行うこと。
	 */
	void RecheckStatus();

public:
	/**
	 * 監視用コールバック(tWaveWatchThreadから約50msごとに呼ばれる)
	 */
	void WatchCallback();

	/**
	 * 現在再生位置までに発生したイベントをすべて発生させる
	 * @return	もっとも近い次のラベルイベントまでの時間を ms で返す
	 */
	

private:
	/**
	 * 前回とステータスが変わっていたら OnStatusChanged を呼ぶ
	 * @param async	非同期イベントかどうか
	 * @note	このメソッド内でロックは行わないので、呼び出し元が
	 *			ちゃんとロックを行っているかどうかを確認すること。
	 */
	void CallStatusChanged(bool async);

public:
	/**
	 * 再生の開始
	 */
	void Play();

protected:
	/**
	 * 再生の停止(内部関数)
	 * @param notify	OnStatusChanged で通知をするかどうか
	 *					0=通知しない 1=同期イベントとして通知 2=非同期イベントとして通知
	 */
	void InternalStop(int notify);

public:
	/**
	 * 再生の停止
	 * @param notify	OnStatusChanged で通知をするかどうか
	 *					0=通知しない 1=同期イベントとして通知 2=非同期イベントとして通知
	 */
	void Stop(int notify = 1);

	/**
	 * 再生の一時停止
	 */
	void Pause();

private:
	/**
	 * 再生中のバッファ内の位置を得る
	 * @return	再生中のバッファ内の位置 (キューの先頭からのサンプルグラニュール数単位)
	 * @note	現在位置を得られなかった場合は risse_size_max が帰る。このメソッドは
	 *			スレッド保護を行わないので注意
	 */
	risse_size GetBufferPlayingPosition();

public:
	/**
	 * 再生位置を得る
	 * @return	再生位置   (デコーダ出力時におけるサンプルグラニュール数単位)
	 * @note	返される値は、デコーダ上(つまり元のメディア上での)サンプルグラニュール数
	 *			単位となる。これは、フィルタとして時間の拡縮を行うようなフィルタが
	 *			挟まっていた場合は、実際に再生されたサンプルグラニュール数とは
	 *			異なる場合があるということである。
	 */
	risse_uint64 GetPosition();

	/**
	 * 再生位置を設定する
	 * @param pos	再生位置 (デコーダ出力におけるサンプルグラニュール数単位)
	 */
	void SetPosition(risse_uint64 pos);

public:
	/**
	 * 現在の再生位置までのラベルイベントを発生させ、次のラベルイベントまでの時間を帰す
	 * @return	次のラベルイベントまでの時間(ms) ラベルイベントが見つからない場合は -1 を帰す
	 */
	risse_int32 FireLabelEvents();

public:
	/**
	 * ステータスの変更を通知する
	 * @param status	ステータス
	 * @note	OnStatusChangedAsync は非同期イベント用。
	 *			OnStatusChanged 同士や OnStatusChangedAsync 同士、
	 *			あるいはそれぞれ同士の呼び出しが重ならないことは
	 *			このクラスが保証している。
	 *			OnStatusChangedAsync は OnStatusChanged を呼んだ
	 *			スレッドとは別のスレッドが呼ぶ可能性があるので注意すること。
	 */
	virtual void OnStatusChanged(tStatus status) {;}

	/**
	 * ステータスの変更を非同期に通知する
	 * @param status	ステータス
	 * @note	OnStatusChangedAsync は非同期イベント用。
	 *			OnStatusChanged 同士や OnStatusChangedAsync 同士、
	 *			あるいはそれぞれ同士の呼び出しが重ならないことは
	 *			このクラスが保証している。
	 *			OnStatusChangedAsync は OnStatusChanged を呼んだ
	 *			スレッドとは別のスレッドが呼ぶ可能性があるので注意すること。
	 */
	virtual void OnStatusChangedAsync(tStatus status) {;}

	/**
	 * ラベルイベントの発生を通知する
	 * @param name	ラベル名
	 * @note	このイベントは常に非同期イベントとなるはず。
	 *			スレッドとは別のスレッドが呼ぶ可能性があるので注意すること。
	 */
	virtual void OnLabel(const tString & name) {;}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
