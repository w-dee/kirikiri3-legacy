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



namespace Risa {
RISSE_DEFINE_SOURCE_ID(47626,3140,27936,19656,12175,17772,57131,58681);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSoundALSource::tSoundALSource(tSound * owner,
	boost::shared_ptr<tALBuffer> buffer, boost::shared_ptr<tWaveLoopManager> loopmanager) :
	tALSource(buffer, loopmanager), Owner(owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSoundALSource::tSoundALSource(tSound * owner, const tALSource * ref) :
	tALSource(ref), Owner(owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundALSource::OnStatusChanged(tStatus status)
{
	Owner->CallOnStatusChanged(status);
}
//---------------------------------------------------------------------------







/*
	メモ
	データの流れ

	tALSource <-- tALBuffer (<-- tWaveFilter)* <-- tWaveLoopManager <-- tWaveDecoder

	・複数の tALSource は 一つの tALBuffer を共有できる
	  (ただし、tALBuffer が非ストリーミングバッファの場合)
	・ストリーミング時にバッファを fill するのは tALSource の責任
	・tWaveFilter は あってもなくてもOK、複数個が直列する場合もある
	・tWaveLoopManager は一つ tWaveDecoder を入力にとる
*/


//---------------------------------------------------------------------------
tSound::tSound()
{
	Init();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSound::tSound(const tString & filename)
{
	Init();
	Open(filename);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSound::~tSound()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSound::Init()
{
	Status = ssUnload;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSound::Clear()
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
void tSound::CallOnStatusChanged(tStatus status)
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
void tSound::Open(const tString & filename)
{
	// メディアを開くのに先立って内部状態をクリア
	Clear();

	// 各コンポーネントを作成して接続する
	try
	{
		// デコーダを作成
		Decoder = depends_on<tWaveDecoderFactoryManager>::locked_instance()->Create(filename);

		// LoopManager を作成
		LoopManager =
			boost::shared_ptr<tWaveLoopManager>(new tWaveLoopManager(Decoder));

		// pv
		boost::shared_ptr<tPhaseVocoder> filter(new tPhaseVocoder());
		filter->SetOverSampling(16);
		filter->SetFrameSize(4096);
		filter->SetTimeScale(1.6);
		filter->SetFrequencyScale(1.0);
		filter->SetInput(LoopManager);

		// バッファを作成
		Buffer = boost::shared_ptr<tALBuffer>(new tALBuffer(filter, true));

		// ソースを作成
		Source = boost::shared_ptr<tSoundALSource>(new tSoundALSource(this, Buffer, LoopManager));

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
void tSound::Close()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSound::Play()
{
	if(!Source) return; // ソースがないので再生を開始できない
	Source->Play(); // 再生を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSound::Stop()
{
	if(!Source) return; // ソースがないので再生を停止できない
	Source->Stop(); // 再生を停止
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSound::Pause()
{
	if(!Source) return; // ソースがないので再生を一時停止できない
	Source->Pause(); // 再生を一時停止
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tSound::GetSamplePosition()
{
	if(!Source) return 0; // ソースがないので再生位置を取得できない
	return Source->GetPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
double tSound::GetTimePosition()
{
	if(!Source) return 0; // ソースがないので再生位置を取得できない
	return Source->GetPosition() * 1000 / LoopManager->GetFormat().Frequency;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSound::SetSamplePosition(risse_uint64 pos)
{
	if(!Source) return; // ソースがないので再生位置を変更できない
	Source->SetPosition(pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSound::SetTimePosition(double pos)
{
	if(!Source) return; // ソースがないので再生位置を変更できない
	Source->SetPosition(static_cast<risse_uint64>(pos *  LoopManager->GetFormat().Frequency / 1000));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


