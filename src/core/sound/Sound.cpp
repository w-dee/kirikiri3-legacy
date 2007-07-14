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
#include "prec.h"
#include "sound/Sound.h"

RISSE_DEFINE_SOURCE_ID(47626,3140,27936,19656,12175,17772,57131,58681);




//---------------------------------------------------------------------------
tRisaSoundALSource::tRisaSoundALSource(tRisaSound * owner,
	boost::shared_ptr<tRisaALBuffer> buffer, boost::shared_ptr<tRisaWaveLoopManager> loopmanager) :
	tRisaALSource(buffer, loopmanager), Owner(owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaSoundALSource::tRisaSoundALSource(tRisaSound * owner, const tRisaALSource * ref) :
	tRisaALSource(ref), Owner(owner)
{
}
//---------------------------------------------------------------------------


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
tRisaSound::tRisaSound()
{
	Init();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaSound::tRisaSound(const tString & filename)
{
	Init();
	Open(filename);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaSound::~tRisaSound()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaSound::Init()
{
	Status = ssUnload;
}
//---------------------------------------------------------------------------


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
void tRisaSound::CallOnStatusChanged(tStatus status)
{
	if(Status != status)
	{
		Status = status;
		OnStatusChanged(status);
	}
}
//---------------------------------------------------------------------------

#include "sound/filter/phasevocoder/PhaseVocoder.h"
//---------------------------------------------------------------------------
void tRisaSound::Open(const tString & filename)
{
	// メディアを開くのに先立って内部状態をクリア
	Clear();

	// 各コンポーネントを作成して接続する
	try
	{
		// デコーダを作成
		Decoder = depends_on<tRisaWaveDecoderFactoryManager>::locked_instance()->Create(filename);

		// LoopManager を作成
		LoopManager =
			boost::shared_ptr<tRisaWaveLoopManager>(new tRisaWaveLoopManager(Decoder));

		// pv
		boost::shared_ptr<tRisaPhaseVocoder> filter(new tRisaPhaseVocoder());
		filter->SetOverSampling(16);
		filter->SetFrameSize(4096);
		filter->SetTimeScale(1.6);
		filter->SetFrequencyScale(1.0);
		filter->SetInput(LoopManager);

		// バッファを作成
		Buffer = boost::shared_ptr<tRisaALBuffer>(new tRisaALBuffer(filter, true));

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
void tRisaSound::Close()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaSound::Play()
{
	if(!Source) return; // ソースがないので再生を開始できない
	Source->Play(); // 再生を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaSound::Stop()
{
	if(!Source) return; // ソースがないので再生を停止できない
	Source->Stop(); // 再生を停止
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaSound::Pause()
{
	if(!Source) return; // ソースがないので再生を一時停止できない
	Source->Pause(); // 再生を一時停止
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tRisaSound::GetSamplePosition()
{
	if(!Source) return 0; // ソースがないので再生位置を取得できない
	return Source->GetPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
double tRisaSound::GetTimePosition()
{
	if(!Source) return 0; // ソースがないので再生位置を取得できない
	return Source->GetPosition() * 1000 / LoopManager->GetFormat().Frequency;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaSound::SetSamplePosition(risse_uint64 pos)
{
	if(!Source) return; // ソースがないので再生位置を変更できない
	Source->SetPosition(pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaSound::SetTimePosition(double pos)
{
	if(!Source) return; // ソースがないので再生位置を変更できない
	Source->SetPosition(static_cast<risse_uint64>(pos *  LoopManager->GetFormat().Frequency / 1000));
}
//---------------------------------------------------------------------------


