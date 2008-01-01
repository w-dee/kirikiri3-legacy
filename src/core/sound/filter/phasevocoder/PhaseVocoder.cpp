//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief フェーズボコーダフィルタ
//---------------------------------------------------------------------------
#include "prec.h"
#include "sound/filter/phasevocoder/Phasevocoder.h"
#include "sound/Sound.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(38521,252,49793,17297,63880,47889,47025,34954);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tPhaseVocoderInstance::tPhaseVocoderInstance() :
	tWaveFilterInstance(tPCMTypes::tf32)
{
	DSP = NULL;
	FrameSize = DEFAULT_FRAME_SIZE;
	OverSampling = DEFAULT_OVERSAMPLING;
	TimeScale = 1.0;
	FrequencyScale = 1.0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tPhaseVocoderInstance::GetFrameSize() const
{
	volatile tSynchronizer sync(this); // sync

	return FrameSize;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::SetFrameSize(int v)
{
	volatile tSynchronizer sync(this); // sync

	switch(v)
	{
	case 64: case 128: case 256: case 512: case 1024: case 2048: case 4096: case 8192:
	case 16384: case 32768:
		break;
	default:
		tSoundExceptionClass::Throw(RISSE_WS_TR("window size must be power of 2, in 64 to 32768"));
	}
	FrameSize = v;
	if(DSP) RebuildDSP(); // DSP は作り直す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tPhaseVocoderInstance::GetOverSampling() const
{
	volatile tSynchronizer sync(this); // sync

	return OverSampling;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::SetOverSampling(int v)
{
	volatile tSynchronizer sync(this); // sync

	switch(v)
	{
	case 0:
	case 2: case 4: case 8: case 16: case 32:
		break;
	default:
		tSoundExceptionClass::Throw(RISSE_WS_TR("overlap count must be power of 2, in 2 to 32"));
	}
	OverSampling = v;
	if(DSP) DSP->SetOverSampling(v); // DSP は作り直す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
float tPhaseVocoderInstance::GetTimeScale() const
{
	volatile tSynchronizer sync(this); // sync

	return TimeScale;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::SetTimeScale(float v)
{
	volatile tSynchronizer sync(this); // sync

	TimeScale = v;
	if(DSP)
	{
		DSP->SetTimeScale(v);
		DSP->SetOverSampling(OverSampling);
			// TimeScaleにしたがってOverSampling が自動的に決定される場合があるので、
			// TimeScale の設定の際に OverSampling ももう一度設定し直す。
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
float tPhaseVocoderInstance::GetFrequencyScale() const
{
	volatile tSynchronizer sync(this); // sync

	return FrequencyScale;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::SetFrequencyScale(float v)
{
	volatile tSynchronizer sync(this); // sync

	FrequencyScale = v;
	if(DSP) DSP->SetFrequencyScale(v);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::Clear()
{
	volatile tSynchronizer sync(this); // sync

	if(DSP) delete DSP, DSP = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::RebuildDSP()
{
	volatile tSynchronizer sync(this); // sync

	if(DSP) delete DSP, DSP = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::EnsureDSP()
{
	volatile tSynchronizer sync(this); // sync

	if(!DSP)
	{
		DSP = new tPhaseVocoderDSP(FrameSize,
			InputFormat.Frequency, InputFormat.Channels);
		DSP->SetTimeScale(TimeScale);
		DSP->SetFrequencyScale(FrequencyScale);
		DSP->SetOverSampling(OverSampling);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::InputChanged()
{
	volatile tSynchronizer sync(this); // sync

	Clear();
	RebuildDSP();
	SegmentQueue.Clear();

	// Input に PCM 形式を提案する
	tWaveFormat format;
	format.Reset();
	format.PCMType = tPCMTypes::tf32; // float がいい！できれば float にして！！
	Input->SuggestFormat(format);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::Filter()
{
	volatile tSynchronizer sync(this); // sync

	EnsureDSP();

	tWaveSegmentQueue newqueue;

	// DSP の入力空きを調べる
	size_t inputfree = DSP->GetInputFreeSize();
	if(inputfree > 0)
	{
		// 入力にデータを流し込む
		float *p1, *p2;
		size_t p1len, p2len;
		DSP->GetInputBuffer(inputfree, p1, p1len, p2, p2len);
		size_t filled = 0;
		filled += Fill(p1, p1len, tPCMTypes::tf32, true, SegmentQueue);
		if(p2) filled += Fill(p2, p2len, tPCMTypes::tf32, true, SegmentQueue);
		if(filled == 0) return ; // もうデータがない
	}

	// DSP に処理をさせる
	// DSP は入力が十分な場合 (これは上のブロックで入力にデータを流し込んで
	// いるので保証できる)、DSP->GetInputHopSize() 分のサンプルを入力から
	// 消費し、DSP->GetOutputHopSize() 分のサンプルを出力バッファに入れる。
	(void) DSP->Process();

	// DSP の出力バッファの準備済みサンプル数を調べる
	size_t outputready = DSP->GetOutputReadySize();
	// この時点で outputready は GetOutputHopSize() と同じはず
	if(outputready > 0)
	{
		// 準備済みサンプルがある

		// SegmentQueue から DSP->GetInputHopSize() 分をデキューし、
		// DSP->GetOutputHopSize() の長さにスケールし直す
		SegmentQueue.Dequeue(newqueue, DSP->GetInputHopSize());
		newqueue.Scale(DSP->GetOutputHopSize());

		// バッファを確保
		float * dest_buf = reinterpret_cast<float*>(PrepareQueue(outputready));
		if(!dest_buf) return;

		// DSP の出力から dest_buf にコピーする
		const float *p1, *p2;
		size_t p1len, p2len;
		DSP->GetOutputBuffer(outputready, p1, p1len, p2, p2len);
		memcpy(dest_buf, p1, p1len * sizeof(float)*InputFormat.Channels);
		if(p2) memcpy(dest_buf + p1len * InputFormat.Channels, p2,
						p2len * sizeof(float)*InputFormat.Channels);

		// キューする
		Queue(outputready, newqueue);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::construct()
{
	// デフォルトではなにもしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tPhaseVocoderClass::tPhaseVocoderClass(tScriptEngine * engine) :
	tClassBase(tSS<'P','h','a','s','e','V','o','c','o','d','e','r'>(),
		tRisseClassRegisterer<tWaveFilterClass>::instance()->GetClassInstance())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPhaseVocoderClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tPhaseVocoderClass::ovulate);
	BindFunction(this, ss_construct, &tPhaseVocoderInstance::construct);
	BindFunction(this, ss_initialize, &tPhaseVocoderInstance::initialize);

	BindProperty(this, tSS<'w','i','n','d','o','w'>(), &tPhaseVocoderInstance::get_window, &tPhaseVocoderInstance::set_window);
	BindProperty(this, tSS<'o','v','e','r','a','l','p'>(), &tPhaseVocoderInstance::get_overlap, &tPhaseVocoderInstance::set_overlap);
	BindProperty(this, tSS<'t','i','m','e'>(), &tPhaseVocoderInstance::get_time, &tPhaseVocoderInstance::set_time);
	BindProperty(this, tSS<'p','i','t','c','h'>(), &tPhaseVocoderInstance::get_pitch, &tPhaseVocoderInstance::set_pitch);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPhaseVocoderClass::ovulate()
{
	return tVariant(new tPhaseVocoderInstance());
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		PhaseVocoder クラスレジストラ
template class tRisseWFClassRegisterer<tPhaseVocoderClass>;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

