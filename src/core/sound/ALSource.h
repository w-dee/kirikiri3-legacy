//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OpenAL ソース管理
//---------------------------------------------------------------------------
#ifndef ALSourceH
#define ALSourceH

#include <al.h>
#include <alc.h>
#include "sound/ALCommon.h"
#include "sound/ALBuffer.h"
#include "sound/WaveSegmentQueue.h"
#include "sound/WaveLoopManager.h"
#include "base/utils/RisaThread.h"
#include "base/utils/Singleton.h"
#include "base/event/Event.h"

namespace Risa {
//---------------------------------------------------------------------------

class tRisaALSource;
//---------------------------------------------------------------------------
//! @brief		監視用スレッド
//! @note		監視用スレッドは、約50ms周期のコールバックをすべての
//!				ソースに発生させる。ソースではいくつかポーリングを
//!				行わなければならない場面でこのコールバックを利用する。
//---------------------------------------------------------------------------
class tRisaWaveWatchThread :
	public singleton_base<tRisaWaveWatchThread>,
	manual_start<tRisaWaveWatchThread>,
	public tRisaThread
{
	tRisaThreadEvent Event; //!< スレッドをたたき起こすため/スレッドを眠らせるためのイベント
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	gc_vector<tRisaALSource*> Sources; //!< Source の配列

public:
	//! @brief		コンストラクタ
	tRisaWaveWatchThread();

	//! @brief		デストラクタ
	~tRisaWaveWatchThread();

	//! @brief		ソースを登録する
	//! @param		source ソース
	void RegisterSource(tRisaALSource * source);

	//! @brief		ソースの登録を解除する
	//! @param		source ソース
	void UnregisterSource(tRisaALSource * source);

protected:
	//! @brief		スレッドのエントリーポイント
	void Execute(void);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		tRisaALSourceのステータスを含むクラス
//---------------------------------------------------------------------------
class tRisaALSourceStatus
{
public:
	//! @brief	サウンドソースの状態
	enum tStatus
	{
		ssUnload, //!< data is not specified (tRisaALSourceではこの状態は存在しない)
		ssStop, //!< stopping
		ssPlay, //!< playing
		ssPause, //!< pausing
	};
};
//---------------------------------------------------------------------------



class tRisaWaveDecodeThread;
//---------------------------------------------------------------------------
//! @brief		OpenALソース
//---------------------------------------------------------------------------
class tRisaALSource :
				protected depends_on<tRisaOpenAL>,
				protected depends_on<tRisaWaveWatchThread>,
				protected depends_on<tRisaEventSystem>,
				protected tRisaEventDestination,
				public tRisaALSourceStatus
{
	friend class tRisaWaveDecodeThread;
	friend class tRisaWaveWatchThread;
public:
	// 定数など
	static const risse_uint STREAMING_NUM_BUFFERS = tRisaALBuffer::MAX_NUM_BUFFERS; //!< ストリーミング時のバッファの数(調整可)
	static const risse_uint STREAMING_PREPARE_BUFFERS = 4; //!< 再生開始前にソースにキューしておくバッファの数

private:
	//! @brief	イベントのID
	enum tEventId
	{
		eiStatusChanged, // ステータスが変更された
	};

	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	risse_uint NumBuffersQueued; //!< キューに入っているバッファの数
	ALuint Source; //!< OpenAL ソース
	bool SourceAllocated; //!< Source がすでに割り当てられているかどうか
	boost::shared_ptr<tRisaALBuffer> Buffer; //!< バッファ
	boost::shared_ptr<tRisaWaveLoopManager> LoopManager; //!< ループマネージャ
	bool NeedRewind; //!< リワインド (巻き戻し) が必要な場合に真
	tRisaWaveDecodeThread * DecodeThread; //!< デコードスレッド
	tStatus Status; //!< サウンドステータス
	tStatus PrevStatus; //!< 直前のサウンドステータス
	gc_deque<tRisaWaveSegmentQueue> SegmentQueues; //!< セグメントキューの配列

public:
	//! @brief		コンストラクタ
	//! @param		buffer		OpenAL バッファを管理する tRisaALBuffer インスタンス
	tRisaALSource(boost::shared_ptr<tRisaALBuffer> buffer,
		boost::shared_ptr<tRisaWaveLoopManager> loopmanager = boost::shared_ptr<tRisaWaveLoopManager>());

	//! @brief		コンストラクタ(ほかのtRisaALSourceとバッファを共有する場合)
	//! @param		ref		コピー元ソース
	tRisaALSource(const tRisaALSource * ref);

	//! @brief		デストラクタ
	virtual ~tRisaALSource();

private:
	//! @brief		オブジェクトを初期化する
	//! @param		buffer		OpenAL バッファを管理する tRisaALBuffer インスタンス
	void Init(boost::shared_ptr<tRisaALBuffer> buffer);

public:
	ALuint GetSource() const { return Source; } //!< Source を得る

private:
	//! @brief		バッファに関するオブジェクトの解放などのクリーンアップ処理
	void Clear();

private: //---- queue/buffer management
	//! @brief		バッファのデータを埋める
	void FillBuffer();

	//! @brief		すべてのバッファをアンキューする
	void UnqueueAllBuffers();

	//! @brief		バッファをソースにキューする
	//! @note		キューできない場合は何もしない
	void QueueBuffer();

private:
	//! @brief		監視用コールバック(tRisaWaveWatchThreadから約50msごとに呼ばれる)
	void WatchCallback();

	//! @brief		前回とステータスが変わっていたら OnStatusChanged を呼ぶ
	//! @note		必ずメインスレッドから呼び出すこと
	void CallStatusChanged();

protected:
	//! @brief		イベントが発生したとき
	//! @param		info  イベント情報
	void OnEvent(tRisaEventInfo * info);

public:
	//! @brief		再生の開始
	void Play();

	//! @brief		再生の停止
	//! @note		このメソッドはメディアの巻き戻しを行わない(ソースはそこら辺を
	//!				管理しているループマネージャがどこにあるかを知らないので)
	//!				巻き戻しの処理は現在tRisaSound内で行われている
	void Stop();

	//! @brief		再生の一時停止
	void Pause();

	//! @brief		再生位置を得る
	//! @return		再生位置   (デコーダ出力時におけるサンプルグラニュール数単位)
	//! @note		返される値は、デコーダ上(つまり元のメディア上での)サンプルグラニュール数
	//!				単位となる。これは、フィルタとして時間の拡縮を行うようなフィルタが
	//!				挟まっていた場合は、実際に再生されたサンプルグラニュール数とは
	//!				異なる場合があるということである。
	risse_uint64 GetPosition();

	//! @brief		再生位置を設定する
	//! @param		pos  再生位置 (デコーダ出力におけるサンプルグラニュール数単位)
	void SetPosition(risse_uint64 pos);

public:
	virtual void OnStatusChanged(tStatus status) {;}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
