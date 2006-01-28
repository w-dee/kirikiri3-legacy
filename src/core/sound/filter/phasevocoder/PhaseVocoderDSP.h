//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Phase Vocoder の実装
//---------------------------------------------------------------------------
#ifndef RisaPhaseVocoderH
#define RisaPhaseVocoderH

#include "RingBuffer.h"

//---------------------------------------------------------------------------
//! @brief Phase Vocoder DSP クラス
//---------------------------------------------------------------------------
class tRisaPhaseVocoderDSP
{
	const static float MAX_TIME_SCALE = 1.95; //!< 最大の time scale 値
	const static float MIN_TIME_SCALE = 0.25; //!< 最小の time scale 値

	float * AnalWork; //!< 解析(Analyze)用バッファ(FrameSize個) 名前で笑わないように
	float * SynthWork; //!< 合成用作業バッファ(FrameSize)
	float ** LastAnalPhase; //!< 前回解析時の各フィルタバンドの位相 (各チャンネルごとにFrameSize/2個)
	float ** LastSynthPhase; //!< 前回合成時の各フィルタバンドの位相 (各チャンネルごとにFrameSize/2個)

	int * FFTWorkIp; //!< rdft に渡す ip パラメータ
	float * FFTWorkW; //!< rdft に渡す w パラメータ
	float * InputWindow; //!< 入力用窓関数
	float * OutputWindow; //!< 出力用窓関数

	unsigned int FrameSize; //!< FFTサイズ
	unsigned int OverSampling; //!< オーバー・サンプリング係数
	unsigned int Frequency; //!< PCM サンプリング周波数
	unsigned int Channels; //!< PCM チャンネル数
	unsigned int InputHopSize; //!< FrameSize/OverSampling

	tRisaRingBuffer<float> InputBuffer; //!< 入力用リングバッファ
	tRisaRingBuffer<float> OutputBuffer; //!< 出力用リングバッファ

	float	TimeScale; //!< 時間軸方向のスケール(出力/入力)
	float	FrequencyScale; //!< 周波数方向のスケール(出力/入力)

	bool	RebuildParams; //!< 内部的なパラメータなどを再構築しなければならないときに真

	// 以下、RebuildParams が真の時に再構築されるパラメータ
	// ここにあるメンバ以外では、InputWindow と OutputWindow も再構築される
	unsigned int OutputHopSize; //!< InputHopSize * TimeScale
	float OverSamplingRadian; //!< (2.0*M_PI)/OverSampling
	float OverSamplingRadianRecp; //!< OverSamplingRadian の逆数
	float FrequencyPerFilterBand; //!< Frequency/FrameSize
	float FrequencyPerFilterBandRecp; //!< FrequencyPerFilterBand の逆数
	float ExactTimeScale; //!< 厳密なTimeScale = OutputHopSize / InputHopSize

public:
	//! @brief Process が返すステータス
	enum tStatus
	{
		psNoError, //!< 問題なし
		psInputNotEnough, //!< 入力がもうない (GetInputBufferで得たポインタに書いてから再試行せよ)
		psOutputFull //!< 出力バッファがいっぱい (GetOutputBufferで得たポインタから読み出してから再試行せよ)
	};

public:
	tRisaPhaseVocoderDSP(unsigned int framesize, unsigned int oversamp,
					unsigned int frequency, unsigned int channels);
	~tRisaPhaseVocoderDSP();

	float GetTimeScale() const { return TimeScale; } //!< 時間軸方向のスケールを得る
	void SetTimeScale(float v);

	float GetFrequencyScale() const { return FrequencyScale; } //!< 周波数軸方向のスケールを得る
	void SetFrequencyScale(float v);

private:
	void Clear();

public:
	size_t GetInputFreeSize();
	bool GetInputBuffer(size_t numsamplegranules,
		float * & p1, size_t & p1size,
		float * & p2, size_t & p2size);

	size_t GetOutputReadySize();
	bool GetOutputBuffer(size_t numsamplegranules,
		const float * & p1, size_t & p1size,
		const float * & p2, size_t & p2size);

	tStatus Process();

private:
	void Deinterleave(float * dest, const float * src,
					float * win, size_t len);

	void Interleave(float * dest, const float * src,
					float * win, size_t len);

};
//---------------------------------------------------------------------------

#endif
