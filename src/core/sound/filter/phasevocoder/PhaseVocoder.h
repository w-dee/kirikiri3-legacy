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

#ifndef PhaseVocoderH
#define PhaseVocoderH

#include "sound/filter/BasicWaveFilter.h"
#include "sound/filter/phasevocoder/PhaseVocoderDSP.h"

//---------------------------------------------------------------------------
//! @brief フェーズボコーダフィルタクラス
//---------------------------------------------------------------------------
class tRisaPhaseVocoder : public tRisaBasicWaveFilter
{
	static const int DEFAULT_FRAME_SIZE = 128; //!< default frame size
	static const int DEFAULT_OVERSAMPLING = 4; //!< default oversampling factor

	tRisaPhaseVocoderDSP * DSP; //!< PhaseVocoder DSP

	int FrameSize; //!< フレームサイズ
	int OverSampling; //!< オーバーサンプリング係数
	float	TimeScale; //!< 時間軸方向のスケール(出力/入力)
	float	FrequencyScale; //!< 周波数方向のスケール(出力/入力)

	tRisaWaveSegmentQueue SegmentQueue;

public:
	tRisaPhaseVocoder();
	~tRisaPhaseVocoder();

	int GetFrameSize() const;
	void SetFrameSize(int v);

	int GetOverSampling() const;
	void SetOverSampling(int v);

	float GetTimeScale() const;
	void SetTimeScale(float v);

	float GetFrequencyScale() const;
	void SetFrequencyScale(float v);

private:
	void Clear();

	void RebuildDSP();

	void InputChanged();
	void Filter();

};
//---------------------------------------------------------------------------


#endif

