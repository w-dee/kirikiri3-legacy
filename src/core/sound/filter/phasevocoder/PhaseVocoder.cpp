//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief フェーズボコーダフィルタ
//---------------------------------------------------------------------------
#include "prec.h"
#include "sound/filter/phasevocoder/Phasevocoder.h"

RISSE_DEFINE_SOURCE_ID(38521,252,49793,17297,63880,47889,47025,34954);

//---------------------------------------------------------------------------
tRisaPhaseVocoder::tRisaPhaseVocoder() :
	tRisaBasicWaveFilter(tRisaPCMTypes::tf32)
{
	DSP = NULL;
	FrameSize = DEFAULT_FRAME_SIZE;
	OverSampling = DEFAULT_OVERSAMPLING;
	TimeScale = 1.0;
	FrequencyScale = 1.0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaPhaseVocoder::~tRisaPhaseVocoder()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tRisaPhaseVocoder::GetFrameSize() const
{
	return FrameSize;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoder::SetFrameSize(int v)
{
	FrameSize = v;
	if(DSP) RebuildDSP(); // DSP は作り直す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tRisaPhaseVocoder::GetOverSampling() const
{
	return OverSampling;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoder::SetOverSampling(int v)
{
	OverSampling = v;
	if(DSP) RebuildDSP(); // DSP は作り直す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
float tRisaPhaseVocoder::GetTimeScale() const
{
	return TimeScale;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoder::SetTimeScale(float v)
{
	TimeScale = v;
	if(DSP) DSP->SetTimeScale(v);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
float tRisaPhaseVocoder::GetFrequencyScale() const
{
	return FrequencyScale;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoder::SetFrequencyScale(float v)
{
	FrequencyScale = v;
	if(DSP) DSP->SetFrequencyScale(v);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoder::Clear()
{
	delete DSP, DSP = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoder::RebuildDSP()
{
	if(DSP) delete DSP, DSP = NULL;
	DSP = new tRisaPhaseVocoderDSP(FrameSize,
		InputFormat.Frequency, InputFormat.Channels);
	DSP->SetTimeScale(TimeScale);
	DSP->SetFrequencyScale(FrequencyScale);
	DSP->SetOverSampling(OverSampling);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoder::InputChanged()
{
	Clear();
	RebuildDSP();
	SegmentQueue.Clear();

	// Input に PCM 形式を提案する
	tRisaWaveFormat format;
	format.Reset();
	format.PCMType = tRisaPCMTypes::tf32; // float がいい！できれば float にして！！
	Input->SuggestFormat(format);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoder::Filter()
{
	tRisaWaveSegmentQueue newqueue;

	// DSP の入力空きを調べる
	size_t inputfree = DSP->GetInputFreeSize();
	if(inputfree > 0)
	{
		// 入力にデータを流し込む
		float *p1, *p2;
		size_t p1len, p2len;
		DSP->GetInputBuffer(inputfree, p1, p1len, p2, p2len);
		size_t filled = 0;
		filled += Fill(p1, p1len, tRisaPCMTypes::tf32, true, SegmentQueue);
		if(p2) filled += Fill(p2, p2len, tRisaPCMTypes::tf32, true, SegmentQueue);
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




