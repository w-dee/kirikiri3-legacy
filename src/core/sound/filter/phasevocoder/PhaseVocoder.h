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

#include "BasicWaveFilter.h"
#include "PhaseVocoderDSP.h"

//---------------------------------------------------------------------------
//! @brief フェーズボコーダフィルタクラス
//---------------------------------------------------------------------------
class tRisaPhaseVocoder : public tRisaBasicWaveFilter
{
	static const int DEFAULT_FRAME_SIZE = 4096; //!< default frame size
	static const int DEFAULT_OVERSAMPLING = 8; //!< default oversampling factor

	tRisaPhaseVocoderDSP * DSP; //!< PhaseVocoder DSP

	int FrameSize; //!< フレームサイズ
	int OverSampling; //!< オーバーサンプリング係数
	float	TimeScale; //!< 時間軸方向のスケール(出力/入力)
	float	FrequencyScale; //!< 周波数方向のスケール(出力/入力)

public:
	tRisaPhaseVocoder();
	~tRisaPhaseVocoder();

private:
	void Clear();

	void InputChanged();
	void Filter();

};
//---------------------------------------------------------------------------


#endif

