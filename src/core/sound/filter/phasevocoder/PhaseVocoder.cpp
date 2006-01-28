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
#include "Phasevocoder.h"

RISSE_DEFINE_SOURCE_ID(38521,252,49793,17297,63880,47889,47025,34954);

//---------------------------------------------------------------------------
//! @brief		コンストラクタ
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
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaPhaseVocoder::~tRisaPhaseVocoder()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		内容のクリア
//---------------------------------------------------------------------------
void tRisaPhaseVocoder::Clear()
{
	delete DSP, DSP = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		入力となるフィルタが変わったとき、あるいはリセットされるとき
//---------------------------------------------------------------------------
void tRisaPhaseVocoder::InputChanged()
{
	Clear();
	DSP = new tRisaPhaseVocoderDSP(FrameSize, OverSampling,
		InputFormat.Frequency, InputFormat.Channels);
	DSP->SetTimeScale(TimeScale);
	DSP->SetFrequencyScale(FrequencyScale);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		フィルタ動作を行うとき
//---------------------------------------------------------------------------
void tRisaPhaseVocoder::Filter()
{
	// TODO: セグメントとeventsの扱い
	std::vector<tRisaWaveSegment> segments;
	std::vector<tRisaWaveEvent> events;

	while(true) // DSP から出力が出てくるまで繰り返す
	{
		// DSP の入力空きを調べる
		size_t inputfree = DSP->GetInputFreeSize();
		if(inputfree > 0)
		{
			// 入力にデータを流し込む
			float *p1, *p2;
			size_t p1len, p2len;
			DSP->GetInputBuffer(inputfree, p1, p1len, p2, p2len);
			size_t filled = 0;
			filled += Fill(p1, p1len, tRisaPCMTypes::tf32, true, segments, events);
			if(p2) filled += Fill(p2, p2len, tRisaPCMTypes::tf32, true, segments, events);
			if(filled == 0) return ; // もうデータがない
		}

		// DSP に処理をさせる
		(void) DSP->Process();

		// DSP の出力バッファの準備済みサンプル数を調べる
		size_t outputready = DSP->GetOutputReadySize();
		if(outputready > 0)
		{
			// 準備済みサンプルがある

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
			Queue(outputready, segments, events);
			break; //---- ループを抜ける
		}
	} // while(true)
}
//---------------------------------------------------------------------------




