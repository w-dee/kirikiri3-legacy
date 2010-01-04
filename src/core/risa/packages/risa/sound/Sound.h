//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief サウンドクラス
//---------------------------------------------------------------------------
#ifndef SoundH
#define SoundH

#include "risa/packages/risa/sound/ALCommon.h"
#include "risa/packages/risa/sound/ALBuffer.h"
#include "risa/packages/risa/sound/ALSource.h"
#include "risa/packages/risa/sound/WaveDecoder.h"
#include "risa/packages/risa/sound/WaveLoopManager.h"
#include "risa/packages/risa/sound/WaveFilter.h"
#include "risa/common/RisaException.h"
#include "risa/packages/risa/event/Event.h"


namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * サウンド用例外クラス
 */
/*! @note
	ファイルシステム関連の例外クラスとしては、ここの FileSystemException
	(extends RuntimeException) 以外に、IOException がある(Risseエンジン内で定義)。
*/
//---------------------------------------------------------------------------
RISA_DEFINE_EXCEPTION_SUBCLASS(tSoundExceptionClass,
	(tSS<'m','a','i','n'>()),
	(tSS<'S','o','u','n','d','E','x','c','e','p','t','i','o','n'>()),
	tRisseScriptEngine::instance()->GetScriptEngine()->ExceptionClass)
//---------------------------------------------------------------------------




class tSoundInstance;
//---------------------------------------------------------------------------
/**
 * OpenALイベントの発生先を tSoundInstance に固定した OpenAL ソースクラス
 */
class tSoundALSource : public tALSource
{
	tSoundInstance * Owner;

public:
	/**
	 * コンストラクタ
	 */
	tSoundALSource(tSoundInstance * owner, tALBuffer * buffer,
		tWaveLoopManager * loopmanager);

	/**
	 * コンストラクタ
	 */
	tSoundALSource(tSoundInstance * owner, const tALSource * ref);

protected:
	/**
	 * ステータスの変更を通知する
	 * @param status	ステータス
	 */
	virtual void OnStatusChanged(tStatus status);

	/**
	 * ステータスの変更を非同期に通知する
	 * @param status	ステータス
	 */
	virtual void OnStatusChangedAsync(tStatus status);

	/**
	 * ラベルイベントの発生を通知する
	 * @param name	ラベル名
	 */
	virtual void OnLabel(const tString & name);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * サウンドクラス
 */
class tSoundInstance :
	public tEventSourceInstance,
	public tEventDestination,
	protected depends_on<tOpenAL>,
	protected depends_on<tWaveDecoderFactoryManager>,
	public tALSourceStatus
{
	friend class tSoundALSource;

	tStatus Status; //!< 直前のステータス

	tSoundALSource * Source; //!< OpenAL ソース
	tALBuffer * Buffer; //!< OpenAL バッファ
	tVariant Filters; //!< フィルタ配列
	tWaveLoopManager * LoopManager; //!< ループマネージャ
	tWaveDecoder * Decoder; //!< デコーダ
	tEventQueueInstance::tQueue PendingLabelQueue; //!< ペンディングとなったラベルイベントのキュー

	char StatusEventSource; //!< ステータス変更イベントを識別するためのマーカー
	char LabelEventSource; //!< ラベルイベントを識別するためのマーカー
		// ↑ いずれも、値の内容は関係なく、そのアドレスが異なることで互いを識別するために用いる

	class tLabelEventInfo : public tEventInfo
	{
	public:
		tString Name;
		tLabelEventInfo(tSoundInstance * instance, const tString & name)
			: tEventInfo(100 /*←イベントID*/,
				&instance->LabelEventSource, instance), Name(name)
		{
		}
	};

	friend class tLabelEventInfo;

public:
	/**
	 * コンストラクタ
	 */
	tSoundInstance();

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tSoundInstance() {;}

private:
	/**
	 * 内部状態の初期化
	 */
	void Init();

protected:
	/**
	 * 内部状態のクリア
	 */
	void Clear();

public:
	/**
	 * メディアを開く
	 * @param filename	ファイル名
	 */
	void Open(const tString & filename);

	/**
	 * メディアを閉じる
	 */
	void Close();

	/**
	 * 再生を開始する
	 */
	void Play();

	/**
	 * 再生を停止する
	 */
	void Stop();

	/**
	 * 再生を一時停止する
	 */
	void Pause();

	/**
	 * 再生位置をサンプルグラニュール数で取得する
	 * @return	再生位置(サンプルグラニュール単位)
	 */
	risse_uint64 GetSamplePosition();

	/**
	 * 再生位置を時間で取得する
	 * @return	再生位置(ミリ秒単位)
	 */
	double GetTimePosition();

	/**
	 * 再生位置をサンプルグラニュール数で設定する
	 * @param pos	再生位置(サンプルグラニュール単位)
	 */
	void SetSamplePosition(risse_uint64 pos);

	/**
	 * 再生位置を時間で設定する
	 * @param pos	再生位置(ミリ秒単位)
	 */
	void SetTimePosition(double pos);

	/**
	 * フィルタ配列を得る
	 */
	tVariant & GetFilters();

public:
	/**
	 * ステータスが変更された
	 */
	virtual void OnStatusChanged(tStatus status);

	/**
	 * ステータスが変更された
	 */
	virtual void OnStatusChangedAsync(tStatus status);

	/**
	 * ラベルイベントの発生を通知する
	 * @param name	ラベル名
	 */
	virtual void OnLabel(const tString & name);

protected:
	/**
	 * イベントが配信されるとき
	 * @param info	イベント情報
	 */
	virtual void OnEvent(tEventInfo * info); // from tEventDestination

public:
	tStatus GetStatus() const; //!< ステータスを返す

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

	void open(const tString & filename) { Open(filename); }
	void close() { Close(); }
	void play() { Play(); }
	void stop() { Stop(); }
	void pause() { Pause(); }
	risse_uint64 get_samplePosition() { return GetSamplePosition(); }
	void set_samplePosition(risse_uint64 pos) { SetSamplePosition(pos); }
	double get_position() { return GetTimePosition(); }
	void set_position(double pos) { SetTimePosition(pos); }
	const tVariant & get_filters() { return GetFilters(); }
	tStatus get_status() { return GetStatus(); }
	void onStatusChanged(tStatus status) {;}
	void onLabel(const tString & name) {;}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
