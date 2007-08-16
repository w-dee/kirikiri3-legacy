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

#include <AL/al.h>
#include <AL/alc.h>
#include "sound/ALCommon.h"
#include "sound/ALBuffer.h"
#include "sound/WaveSegmentQueue.h"
#include "sound/WaveLoopManager.h"
#include "base/utils/RisaThread.h"
#include "base/utils/Singleton.h"
#include "base/event/Event.h"
#include "risse/include/risseNativeBinder.h"

namespace Risa {
//---------------------------------------------------------------------------

class tALSource;
//---------------------------------------------------------------------------
//! @brief		監視用スレッド
//! @note		監視用スレッドは、約50ms周期のコールバックをすべての
//!				ソースに発生させる。ソースではいくつかポーリングを
//!				行わなければならない場面でこのコールバックを利用する。
//---------------------------------------------------------------------------
class tWaveWatchThread :
	public singleton_base<tWaveWatchThread>,
	manual_start<tWaveWatchThread>,
	public tThread
{
	tThreadEvent Event; //!< スレッドをたたき起こすため/スレッドを眠らせるためのイベント

public:
	//! @brief		コンストラクタ
	tWaveWatchThread();

	//! @brief		デストラクタ
	~tWaveWatchThread();

	//! @brief		眠っているスレッドを叩き起こす
	void Wakeup();

protected:
	//! @brief		スレッドのエントリーポイント
	void Execute(void);
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		tALSourceのステータスを含むクラス
//---------------------------------------------------------------------------
class tALSourceStatus
{
public:
	//! @brief	サウンドソースの状態
	enum tStatus
	{
		ssUnload, //!< data is not specified (tALSourceではこの状態は存在しない)
		ssStop, //!< stopping
		ssPlay, //!< playing
		ssPause, //!< pausing
	};
};
//---------------------------------------------------------------------------


}
namespace Risse {
//---------------------------------------------------------------------------
//! @brief		NativeBinder 用の Variant -> tALSourceStatus::tStatus 変換定義
//---------------------------------------------------------------------------
template <>
inline Risa::tALSourceStatus::tStatus FromVariant<Risa::tALSourceStatus::tStatus>(const tVariant & v)
{
	return (Risa::tALSourceStatus::tStatus)(int)(risse_int64)v;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeBinder 用の tALSourceStatus::tStatus -> Variant 変換定義
//---------------------------------------------------------------------------
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
//! @brief		OpenALソース
//---------------------------------------------------------------------------
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
	//! OpenAL Source を保持するための構造体
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
	gc_deque<tWaveSegmentQueue> SegmentQueues; //!< セグメントキューの配列

public:
	//! @brief		コンストラクタ
	//! @param		buffer		OpenAL バッファを管理する tALBuffer インスタンス
	tALSource(tALBuffer * buffer,
		tWaveLoopManager * loopmanager = NULL);

	//! @brief		コンストラクタ(ほかのtALSourceとバッファを共有する場合)
	//! @param		ref		コピー元ソース
	tALSource(const tALSource * ref);

	//! @brief		デストラクタ
	virtual ~tALSource() {;}

private:
	//! @brief		オブジェクトを初期化する
	//! @param		buffer		OpenAL バッファを管理する tALBuffer インスタンス
	void Init(tALBuffer * buffer);

public:
	ALuint GetSource() const { if(Source) return Source->Source; else return 0; } //!< Source を得る

private: //---- queue/buffer management
	//! @brief		バッファのデータを埋める
	void FillBuffer();

	//! @brief		すべてのバッファをアンキューする
	void UnqueueAllBuffers();

	//! @brief		バッファをソースにキューする
	//! @note		キューできない場合は何もしない
	void QueueBuffer();

	//! @brief		状態をチェックする
	//! @note		OpenAL による実際の再生状況と、このクラス内の管理情報が
	//!				異なる場合がある(特に再生停止時)ため、その状態を再度チェックするためにある。
	//!				クリティカルセクションによる保護は別の場所で行うこと。
	void RecheckStatus();

public:
	//! @brief		監視用コールバック(tWaveWatchThreadから約50msごとに呼ばれる)
	void WatchCallback();

private:
	//! @brief		前回とステータスが変わっていたら OnStatusChanged を呼ぶ
	//! @param		async		非同期イベントかどうか
	//! @note		このメソッド内でロックは行わないので、呼び出し元が
	//!				ちゃんとロックを行っているかどうかを確認すること。
	void CallStatusChanged(bool async);

public:
	//! @brief		再生の開始
	void Play();

	//! @brief		再生の停止
	//! @param		notify		OnStatusChanged で通知をするかどうか
	//! @note		このメソッドはメディアの巻き戻しを行わない(ソースはそこら辺を
	//!				管理しているループマネージャがどこにあるかを知らないので)
	//!				巻き戻しの処理は現在tSound内で行われている
	void Stop(bool notify = true);

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
	//! @brief		ステータスの変更を通知する
	//! @param		status		ステータス
	//! @note		OnStatusChangedAsync は非同期イベント用。
	//!				OnStatusChanged 同士や OnStatusChangedAsync 同士、
	//!				あるいはそれぞれ同士の呼び出しが重ならないことは
	//!				このクラスが保証している。
	//!				OnStatusChangedAsync は OnStatusChanged を呼んだ
	//!				スレッドとは別のスレッドが呼ぶ可能性があるので注意すること。
	virtual void OnStatusChanged(tStatus status) {;}

	//! @brief		ステータスの変更を非同期に通知する
	//! @param		status		ステータス
	//! @note		OnStatusChangedAsync は非同期イベント用。
	//!				OnStatusChanged 同士や OnStatusChangedAsync 同士、
	//!				あるいはそれぞれ同士の呼び出しが重ならないことは
	//!				このクラスが保証している。
	//!				OnStatusChangedAsync は OnStatusChanged を呼んだ
	//!				スレッドとは別のスレッドが呼ぶ可能性があるので注意すること。
	virtual void OnStatusChangedAsync(tStatus status) {;}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
