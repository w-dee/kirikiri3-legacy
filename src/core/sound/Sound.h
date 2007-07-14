//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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

namespace Risa {
//---------------------------------------------------------------------------

class tSound;
//---------------------------------------------------------------------------
//! @brief		OpenALイベントの発生先を tSound に固定した OpenAL ソースクラス
//---------------------------------------------------------------------------
class tSoundALSource : public tALSource
{
	tSound * Owner;

public:
	//! @brief		コンストラクタ
	tSoundALSource(tSound * owner, boost::shared_ptr<tALBuffer> buffer,
		boost::shared_ptr<tWaveLoopManager> loopmanager);

	//! @brief		コンストラクタ
	tSoundALSource(tSound * owner, const tALSource * ref);

protected:
	//! @brief		ステータスが変更された
	//! @param		status  ステータス
	void OnStatusChanged(tStatus status);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		サウンドクラス
//---------------------------------------------------------------------------
class tSound :
	protected depends_on<tOpenAL>,
	protected depends_on<tWaveDecoderFactoryManager>,
	public tALSourceStatus
{
	friend class tSoundALSource;

	tStatus Status; //!< 直前のステータス

	boost::shared_ptr<tSoundALSource> Source;
	boost::shared_ptr<tALBuffer> Buffer;
	gc_vector<boost::shared_ptr<tWaveFilter> > Filters;
	boost::shared_ptr<tWaveLoopManager> LoopManager;
	boost::shared_ptr<tWaveDecoder> Decoder;

public:
	//! @brief		コンストラクタ
	tSound();

	//! @brief		ファイル名を指定してのコンストラクタ
	tSound(const tString & filename);

	//! @brief		デストラクタ
	virtual ~tSound();

private:
	//! @brief		内部状態の初期化
	void Init();

protected:
	//! @brief		内部状態のクリア
	void Clear();

	//! @brief		OnStatusChanged を呼ぶ
	//! @param		status ステータス
	void CallOnStatusChanged(tStatus status);

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

	virtual void OnStatusChanged(tStatus status) {;}

	tStatus GetStatus() const { return Status; } //!< ステータスを返す
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
