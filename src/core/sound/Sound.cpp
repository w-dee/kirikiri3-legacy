//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief サウンドクラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "sound/Sound.h"

RISSE_DEFINE_SOURCE_ID(47626,3140,27936,19656,12175,17772,57131,58681);




//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaSoundALSource::tRisaSoundALSource(tRisaSound * owner,
	boost::shared_ptr<tRisaALBuffer> buffer, boost::shared_ptr<tRisaWaveLoopManager> loopmanager) :
	tRisaALSource(buffer, loopmanager), Owner(owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaSoundALSource::tRisaSoundALSource(tRisaSound * owner, const tRisaALSource * ref) :
	tRisaALSource(ref), Owner(owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ステータスが変更された
//! @param		status  ステータス
//---------------------------------------------------------------------------
void tRisaSoundALSource::OnStatusChanged(tStatus status)
{
	Owner->CallOnStatusChanged(status);
}
//---------------------------------------------------------------------------







/*
	メモ
	データの流れ

	tRisaALSource <-- tRisaALBuffer (<-- tRisaWaveFilter)* <-- tRisaWaveLoopManager <-- tRisaWaveDecoder

	・複数の tRisaALSource は 一つの tRisaALBuffer を共有できる
	  (ただし、tRisaALBuffer が非ストリーミングバッファの場合)
	・ストリーミング時にバッファを fill するのは tRisaALSource の責任
	・tRisaWaveFilter は あってもなくてもOK、複数個が直列する場合もある
	・tRisaWaveLoopManager は一つ tRisaWaveDecoder を入力にとる
*/


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaSound::tRisaSound()
{
	Init();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル名を指定してのコンストラクタ
//---------------------------------------------------------------------------
tRisaSound::tRisaSound(const ttstr & filename)
{
	Init();
	Open(filename);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaSound::~tRisaSound()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		内部状態の初期化
//---------------------------------------------------------------------------
void tRisaSound::Init()
{
	Status = ssUnload;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		内部状態のクリア
//---------------------------------------------------------------------------
void tRisaSound::Clear()
{
	// 再生を停止
	if(Source) Source->Stop();

	// すべてのフィルタなどをリセット
	Decoder.reset();
	LoopManager.reset();
	Filters.clear();
	Buffer.reset();
	Source.reset();

	// ステータスを unload に
	CallOnStatusChanged(ssUnload);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		OnStatusChanged を呼ぶ
//! @param		status ステータス
//---------------------------------------------------------------------------
void tRisaSound::CallOnStatusChanged(tStatus status)
{
	if(Status != status)
	{
		Status = status;
		OnStatusChanged(status);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メディアを開く
//---------------------------------------------------------------------------
void tRisaSound::Open(const ttstr & filename)
{
	// メディアを開くのに先立って内部状態をクリア
	Clear();

	// 各コンポーネントを作成して接続する
	try
	{
		// デコーダを作成
		Decoder = tRisaWaveDecoderFactoryManager::instance()->Create(filename);

		// LoopManager を作成
		LoopManager =
			boost::shared_ptr<tRisaWaveLoopManager>(new tRisaWaveLoopManager(Decoder));

		// バッファを作成
		Buffer = boost::shared_ptr<tRisaALBuffer>(new tRisaALBuffer(LoopManager, true));

		// ソースを作成
		Source = boost::shared_ptr<tRisaSoundALSource>(new tRisaSoundALSource(this, Buffer, LoopManager));

		// ステータスを更新
		CallOnStatusChanged(ssStop);
	}
	catch(...)
	{
		// 内部状態をクリア
		Clear();
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メディアを閉じる
//---------------------------------------------------------------------------
void tRisaSound::Close()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		再生を開始する
//---------------------------------------------------------------------------
void tRisaSound::Play()
{
	if(!Source) return; // ソースがないので再生を開始できない
	Source->Play(); // 再生を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		再生を停止する
//---------------------------------------------------------------------------
void tRisaSound::Stop()
{
	if(!Source) return; // ソースがないので再生を停止できない
	Source->Stop(); // 再生を停止
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		再生を一時停止する
//---------------------------------------------------------------------------
void tRisaSound::Pause()
{
	if(!Source) return; // ソースがないので再生を一時停止できない
	Source->Pause(); // 再生を一時停止
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		再生位置をサンプルグラニュール数で取得する
//! @return		再生位置(サンプルグラニュール単位)
//---------------------------------------------------------------------------
risse_uint64 tRisaSound::GetSamplePosition()
{
	if(!Source) return 0; // ソースがないので再生位置を取得できない
	return Source->GetPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		再生位置を時間で取得する
//! @return		再生位置(ミリ秒単位)
//---------------------------------------------------------------------------
double tRisaSound::GetTimePosition()
{
	if(!Source) return 0; // ソースがないので再生位置を取得できない
	return Source->GetPosition() * 1000 / LoopManager->GetFormat().Frequency;
}
//---------------------------------------------------------------------------


