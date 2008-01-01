//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief サウンドクラス
//---------------------------------------------------------------------------
#ifndef SoundH
#define SoundH

#include "sound/ALCommon.h"
#include "sound/ALBuffer.h"
#include "sound/ALSource.h"
#include "sound/WaveDecoder.h"
#include "sound/WaveLoopManager.h"
#include "sound/WaveFilter.h"
#include "base/exception/RisaException.h"
#include "base/event/Event.h"


namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	サウンド用例外クラス
/*! @note
	ファイルシステム関連の例外クラスとしては、ここの FileSystemException
	(extends RuntimeException) 以外に、IOException がある(Risseエンジン内で定義)。
*/
//---------------------------------------------------------------------------
RISA_DEFINE_EXCEPTION_SUBCLASS(tSoundExceptionClass,
	(tSS<'S','o','u','n','d','E','x','c','e','p','t','i','o','n'>()),
	tRisseScriptEngine::instance()->GetScriptEngine()->ExceptionClass)
//---------------------------------------------------------------------------




class tSoundInstance;
//---------------------------------------------------------------------------
//! @brief		OpenALイベントの発生先を tSoundInstance に固定した OpenAL ソースクラス
//---------------------------------------------------------------------------
class tSoundALSource : public tALSource
{
	tSoundInstance * Owner;

public:
	//! @brief		コンストラクタ
	tSoundALSource(tSoundInstance * owner, tALBuffer * buffer,
		tWaveLoopManager * loopmanager);

	//! @brief		コンストラクタ
	tSoundALSource(tSoundInstance * owner, const tALSource * ref);

protected:
	//! @brief		ステータスの変更を通知する
	//! @param		status		ステータス
	virtual void OnStatusChanged(tStatus status);

	//! @brief		ステータスの変更を非同期に通知する
	//! @param		status		ステータス
	virtual void OnStatusChangedAsync(tStatus status);

	//! @brief		ラベルイベントの発生を通知する
	//! @param		name		ラベル名
	virtual void OnLabel(const tString & name);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		サウンドクラス
//---------------------------------------------------------------------------
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
	//! @brief		コンストラクタ
	tSoundInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tSoundInstance() {;}

private:
	//! @brief		内部状態の初期化
	void Init();

protected:
	//! @brief		内部状態のクリア
	void Clear();

public:
	//! @brief		メディアを開く
	//! @param		filename	ファイル名
	void Open(const tString & filename);

	//! @brief		メディアを閉じる
	void Close();

	//! @brief		再生を開始する
	void Play();

	//! @brief		再生を停止する
	void Stop();

	//! @brief		再生を一時停止する
	void Pause();

	//! @brief		再生位置をサンプルグラニュール数で取得する
	//! @return		再生位置(サンプルグラニュール単位)
	risse_uint64 GetSamplePosition();

	//! @brief		再生位置を時間で取得する
	//! @return		再生位置(ミリ秒単位)
	double GetTimePosition();

	//! @brief		再生位置をサンプルグラニュール数で設定する
	//! @param		pos		再生位置(サンプルグラニュール単位)
	void SetSamplePosition(risse_uint64 pos);

	//! @brief		再生位置を時間で設定する
	//! @param		pos		再生位置(ミリ秒単位)
	void SetTimePosition(double pos);

	//! @brief		フィルタ配列を得る
	tVariant & GetFilters();

public:
	//! @brief		ステータスが変更された
	virtual void OnStatusChanged(tStatus status);

	//! @brief		ステータスが変更された
	//! @param		このメソッドは非同期に別スレッドから呼ばれることがあるので注意。
	virtual void OnStatusChangedAsync(tStatus status);

	//! @brief		ラベルイベントの発生を通知する
	//! @param		name		ラベル名
	virtual void OnLabel(const tString & name);

protected:
	//! @brief	イベントが配信されるとき
	//! @param	info イベント情報
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
