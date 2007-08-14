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



#include "sound/filter/phasevocoder/PhaseVocoder.h"
#include "sound/filter/reverb/Reverb.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(47626,3140,27936,19656,12175,17772,57131,58681);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSoundALSource::tSoundALSource(tSoundInstance * owner,
	tALBuffer * buffer, tWaveLoopManager * loopmanager) :
	tALSource(buffer, loopmanager), Owner(owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSoundALSource::tSoundALSource(tSoundInstance * owner, const tALSource * ref) :
	tALSource(ref), Owner(owner)
{
}
//---------------------------------------------------------------------------

/*
//---------------------------------------------------------------------------
void tSoundALSource::OnStatusChanged(tStatus status)
{
	Owner->OnStatusChanged(status);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundALSource::OnStatusChangedAsync(tStatus status)
{
	Owner->OnStatusChangedAsync(status);
}
//---------------------------------------------------------------------------
*/






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
tSoundInstance::tSoundInstance()
{
	Init();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Init()
{
	Status = ssUnload;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Clear()
{
	// 再生を停止
	if(Source) Source->Stop();

	// すべてのフィルタなどをリセット
	// TODO: 各インスタンスの正しいリセット
	Decoder = NULL; //.Dispose();
	LoopManager = NULL; //.Dispose();
//	Filters.clear();
	Buffer = NULL;
	Source = NULL; //reset();

	// ステータスを unload に
	OnStatusChanged(ssUnload);
}
//---------------------------------------------------------------------------

/*
//---------------------------------------------------------------------------
void tSoundInstance::CallOnStatusChanged(tStatus status)
{
	if(Status != status)
	{
		Status = status;
		OnStatusChanged(status);
	}
}
//---------------------------------------------------------------------------
*/

//---------------------------------------------------------------------------
void tSoundInstance::Open(const tString & filename)
{
	// メディアを開くのに先立って内部状態をクリア
	Clear();

	// 各コンポーネントを作成して接続する
	try
	{
		// デコーダを作成
		Decoder = tWaveDecoderFactoryManager::instance()->Create(filename);

		// LoopManager を作成
		LoopManager = new tWaveLoopManager(Decoder);

		// pv
		tPhaseVocoder * filter = new tPhaseVocoder();
		filter->SetOverSampling(16);
		filter->SetFrameSize(4096);
		filter->SetTimeScale(1.6);
		filter->SetFrequencyScale(1.0);
		filter->SetInput(LoopManager);
/*
		// rev
		tReverb * filter = new tReverb();
		filter->SetInput(LoopManager);
*/
		// バッファを作成
		Buffer = new tALBuffer(filter, true);

		// ソースを作成
		Source = new tSoundALSource(this, Buffer, LoopManager);

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
void tSoundInstance::Close()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Play()
{
	if(!Source) return; // ソースがないので再生を開始できない
	Source->Play(); // 再生を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Stop()
{
	if(!Source) return; // ソースがないので再生を停止できない
	Source->Stop(); // 再生を停止
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Pause()
{
	if(!Source) return; // ソースがないので再生を一時停止できない
	Source->Pause(); // 再生を一時停止
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tSoundInstance::GetSamplePosition()
{
	if(!Source) return 0; // ソースがないので再生位置を取得できない
	return Source->GetPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
double tSoundInstance::GetTimePosition()
{
	if(!Source) return 0; // ソースがないので再生位置を取得できない
	return Source->GetPosition() * 1000 / LoopManager->GetFormat().Frequency;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::SetSamplePosition(risse_uint64 pos)
{
	if(!Source) return; // ソースがないので再生位置を変更できない
	Source->SetPosition(pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::SetTimePosition(double pos)
{
	if(!Source) return; // ソースがないので再生位置を変更できない
	Source->SetPosition(static_cast<risse_uint64>(pos *  LoopManager->GetFormat().Frequency / 1000));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::construct()
{
	// 特にやること無し
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::initialize(const tNativeCallInfo &info)
{
	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		"Sound" クラス
//---------------------------------------------------------------------------
class tSoundClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tSoundClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tSoundClass::tSoundClass(tScriptEngine * engine) :
	tClassBase(tSS<'S','o','u','n','d'>(),
	tRisseClassRegisterer<tEventSourceClass>::instance()->GetClassInstance())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tSoundClass::ovulate);
	BindFunction(this, ss_construct, &tSoundInstance::construct);
	BindFunction(this, ss_initialize, &tSoundInstance::initialize);
	BindFunction(this, tSS<'o','p','e','n'>(), &tSoundInstance::open);
	BindFunction(this, tSS<'p','l','a','y'>(), &tSoundInstance::play);
	BindFunction(this, tSS<'s','t','o','p'>(), &tSoundInstance::stop);
	BindFunction(this, tSS<'p','a','u','s','e'>(), &tSoundInstance::pause);
	BindProperty(this, tSS<'s','a','m','p','l','e','P','o','s','i','t','i','o','n'>(),
			&tSoundInstance::get_samplePosition, &tSoundInstance::set_samplePosition);
	BindProperty(this, tSS<'p','o','s','i','t','i','o','n'>(),
			&tSoundInstance::get_position, &tSoundInstance::set_position);
	BindFunction(this, tSS<'o','n','S','t','a','t','u','s','C','h','a','n','g','e','d'>(),
			&tSoundInstance::onStatusChanged);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tSoundClass::ovulate()
{
	return tVariant(new tSoundInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Sound クラスレジストラ
template class tRisseClassRegisterer<tSoundClass>;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risa


