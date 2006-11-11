//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Phase Vocoder のCPUごとの最適化
//---------------------------------------------------------------------------

#include "prec.h"
#include <math.h>
#include "base/mathlib/MathAlgorithms.h"
#include "sound/filter/phasevocoder/PhaseVocoderDSP.h"
#include "base/mathlib/RealFFT.h"
#include <string.h>
#include "risseUtils.h"


/*
	このソースコードでは詳しいアルゴリズムの説明は行わない。
	基本的な流れはプレーンなC言語版と変わりないので、
	../opt_default/PhaseVocoderDSP_default.cpp を参照のこと。
*/

//---------------------------------------------------------------------------
//! @brief		スタックアラインメント調整用のダミークラス
//---------------------------------------------------------------------------
class tRisaPhaseVocoderDSP_SSE_Trampoline : public tRisaPhaseVocoderDSP
{
public:
	tRisaPhaseVocoderDSP_SSE_Trampoline(unsigned int framesize,
					unsigned int frequency, unsigned int channels) :
				tRisaPhaseVocoderDSP(framesize, frequency, channels) {;}


	void __ProcessCore(int ch);

	RISA_DEFINE_STACK_ALIGN_128_TRAMPOLINE(void, _ProcessCore, (int ch),
							__ProcessCore, (ch) )
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::ProcessCore(int ch)
{
	// トランポリンを呼ぶ
	// 結果的に tRisaPhaseVocoderDSP_SSE_Trampoline::__ProcessCore() が
	// スタックのアラインメントを調整した上で呼ばれることになる
	((tRisaPhaseVocoderDSP_SSE_Trampoline*)this)->_ProcessCore(ch);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP_SSE_Trampoline::__ProcessCore(int ch)
{
	unsigned int framesize_d2 = FrameSize / 2;
	float * analwork = AnalWork[ch];
	float * synthwork = SynthWork[ch];

	// 丸めモードを設定
	RisaSetRoundingModeToNearest_SSE();

	// FFT を実行する
	rdft(FrameSize, 1, analwork, FFTWorkIp, FFTWorkW); // Real DFT
	analwork[1] = 0.0; // analwork[1] = nyquist freq. power (どっちみち使えないので0に)

	__m128 exact_time_scale = _mm_load1_ps(&ExactTimeScale);
	__m128 over_sampling_radian_v = _mm_load1_ps(&OverSamplingRadian);

	if(FrequencyScale != 1.0)
	{
		// ここでは 4 複素数 (8実数) ごとに処理を行う。
		__m128 over_sampling_radian_recp = _mm_load1_ps(&OverSamplingRadianRecp);
		__m128 frequency_per_filter_band = _mm_load1_ps(&FrequencyPerFilterBand);
		__m128 frequency_per_filter_band_recp = _mm_load1_ps(&FrequencyPerFilterBandRecp);

		for(unsigned int i = 0; i < framesize_d2; i += 4)
		{
			// インターリーブ解除 +  直交座標系→極座標系
			__m128 aw3120 = *(__m128*)(analwork + i*2    );
			__m128 aw7654 = *(__m128*)(analwork + i*2 + 4);

			__m128 re3210 = _mm_shuffle_ps(aw3120, aw7654, _MM_SHUFFLE(2,0,2,0));
			__m128 im3210 = _mm_shuffle_ps(aw3120, aw7654, _MM_SHUFFLE(3,1,3,1));

			__m128 mag = _mm_sqrt_ps(re3210 * re3210 + im3210 * im3210);
			__m128 ang = RisaVFast_arctan2_F4_SSE(im3210, re3210);

			// 前回の位相との差をとる
			__m128 lastp = *(__m128*)(LastAnalPhase[ch] + i);
			*(__m128*)(LastAnalPhase[ch] + i) = ang;
			ang = lastp - ang;

			// over sampling の影響を考慮する
			__m128 i_3210;
			i_3210 = _mm_cvtsi32_ss(i_3210, i);
			i_3210 = _mm_shuffle_ps(i_3210, i_3210, _MM_SHUFFLE(0,0,0,0));
			i_3210 = i_3210 + PM128(PFV_INIT);

			__m128 phase_shift = i_3210 * over_sampling_radian_v;
			ang -= phase_shift;

			// unwrapping をする
			ang = RisaWrap_Pi_F4_SSE(ang);

			// -M_PI～+M_PIを-1.0～+1.0の変位に変換
			ang *= over_sampling_radian_recp;

			// tmp をフィルタバンド中央からの周波数の変位に変換し、
			// それにフィルタバンドの中央周波数を加算する
			__m128 freq = (ang + i_3210) * frequency_per_filter_band;

			// analwork に値を格納する
			re3210 = mag;
			im3210 = freq;
			__m128 im10re10 = _mm_movelh_ps(re3210, im3210);
			__m128 im32re32 = _mm_movehl_ps(im3210, re3210);
			__m128 im1re1im0re0 = _mm_shuffle_ps(im10re10, im10re10, _MM_SHUFFLE(3,1,2,0));
			__m128 im3re3im2re2 = _mm_shuffle_ps(im32re32, im32re32, _MM_SHUFFLE(3,1,2,0));
			*(__m128*)(analwork + i*2    ) = im1re1im0re0;
			*(__m128*)(analwork + i*2 + 4) = im3re3im2re2;
		}


		//------------------------------------------------
		// 変換
		//------------------------------------------------
		// 周波数軸方向のリサンプリングを行う
		float FrequencyScale_rcp = 1.0 / FrequencyScale;
		for(unsigned int i = 0; i < framesize_d2; i ++)
		{
			// i に対応するインデックスを得る
			float fi = i * FrequencyScale_rcp;

			// floor(x) と floor(x) + 1 の間でバイリニア補間を行う
			unsigned int index = static_cast<unsigned int>(fi); // floor
			float frac = fi - index;

			if(index + 1 < framesize_d2)
			{
				synthwork[i*2  ] =
					analwork[index*2  ] +
					frac * (analwork[index*2+2]-analwork[index*2  ]);
				synthwork[i*2+1] =
					FrequencyScale * (
					analwork[index*2+1] +
					frac * (analwork[index*2+3]-analwork[index*2+1]) );
			}
			else if(index < framesize_d2)
			{
				synthwork[i*2  ] = analwork[index*2  ];
				synthwork[i*2+1] = analwork[index*2+1] * FrequencyScale;
			}
			else
			{
				synthwork[i*2  ] = 0.0;
				synthwork[i*2+1] = 0.0;
			}
		}

		//------------------------------------------------
		// 合成
		//------------------------------------------------

		// 各フィルタバンドごとに変換
		// 基本的には解析の逆変換である
		for(unsigned int i = 0; i < framesize_d2; i += 4)
		{
			// インターリーブ解除
			__m128 sw3120 = *(__m128*)(synthwork + i*2    );
			__m128 sw7654 = *(__m128*)(synthwork + i*2 + 4);

			__m128 mag  = _mm_shuffle_ps(sw3120, sw7654, _MM_SHUFFLE(2,0,2,0));
			__m128 freq = _mm_shuffle_ps(sw3120, sw7654, _MM_SHUFFLE(3,1,3,1));

			// i+3 i+2 i+1 i+0 を準備
			__m128 i_3210;
			i_3210 = _mm_cvtsi32_ss(i_3210, i);
			i_3210 = _mm_shuffle_ps(i_3210, i_3210, _MM_SHUFFLE(0,0,0,0));
			i_3210 = i_3210 + PM128(PFV_INIT);

			// 周波数から各フィルタバンドの中央周波数を減算し、
			// フィルタバンドの中央周波数からの-1.0～+1.0の変位
			// に変換する
			__m128 ang = freq * frequency_per_filter_band_recp - i_3210;

			// -1.0～+1.0の変位を-M_PI～+M_PIの位相に変換
			ang *= over_sampling_radian_v;

			// OverSampling による位相の補正
			ang += i_3210 * over_sampling_radian_v;

			// TimeScale による位相の補正
			ang *= exact_time_scale;

			// 前回の位相と加算する
			// ここでも虚数部の符号が逆になるので注意
			ang = *(__m128*)(LastSynthPhase[ch] + i) - ang;
			*(__m128*)(LastSynthPhase[ch] + i) = ang;

			// 極座標系→直交座標系
			__m128 sin, cos;
			RisaVFast_sincos_F4_SSE(ang, sin, cos);
			__m128 re3210 = mag * cos;
			__m128 im3210 = mag * sin;

			// インターリーブ
			__m128 im10re10 = _mm_movelh_ps(re3210, im3210);
			__m128 im32re32 = _mm_movehl_ps(im3210, re3210);
			__m128 im1re1im0re0 = _mm_shuffle_ps(im10re10, im10re10, _MM_SHUFFLE(3,1,2,0));
			__m128 im3re3im2re2 = _mm_shuffle_ps(im32re32, im32re32, _MM_SHUFFLE(3,1,2,0));
			*(__m128*)(synthwork + i*2    ) = im1re1im0re0;
			*(__m128*)(synthwork + i*2 + 4) = im3re3im2re2;
		}
	}
	else
	{
		// 周波数軸方向にシフトがない場合
		// ここでも 4 複素数 (8実数) ごとに処理を行う。
		for(unsigned int i = 0; i < framesize_d2; i += 4)
		{
			// インターリーブ解除 +  直交座標系→極座標系
			__m128 aw3120 = *(__m128*)(analwork + i*2    );
			__m128 aw7654 = *(__m128*)(analwork + i*2 + 4);

			__m128 re3210 = _mm_shuffle_ps(aw3120, aw7654, _MM_SHUFFLE(2,0,2,0));
			__m128 im3210 = _mm_shuffle_ps(aw3120, aw7654, _MM_SHUFFLE(3,1,3,1));

			__m128 mag = _mm_sqrt_ps(re3210 * re3210 + im3210 * im3210);
			__m128 ang = RisaVFast_arctan2_F4_SSE(im3210, re3210);

			// 前回の位相との差をとる
			__m128 lastp = *(__m128*)(LastAnalPhase[ch] + i);
			*(__m128*)(LastAnalPhase[ch] + i) = ang;
			ang = lastp - ang;

			// over sampling の影響を考慮する
			__m128 i_3210;
			i_3210 = _mm_cvtsi32_ss(i_3210, i);
			i_3210 = _mm_shuffle_ps(i_3210, i_3210, _MM_SHUFFLE(0,0,0,0));
			i_3210 = i_3210 + PM128(PFV_INIT);

			__m128 phase_shift = i_3210 * over_sampling_radian_v;
			ang -= phase_shift;

			// unwrapping をする
			ang = RisaWrap_Pi_F4_SSE(ang);

			// OverSampling による位相の補正
			ang += phase_shift;

			// TimeScale による位相の補正
			ang *= exact_time_scale;

			// 前回の位相と加算する
			// ここでも虚数部の符号が逆になるので注意
			ang = *(__m128*)(LastSynthPhase[ch] + i) - ang;
			*(__m128*)(LastSynthPhase[ch] + i) = ang;

			// 極座標系→直交座標系
			__m128 sin, cos;
			RisaVFast_sincos_F4_SSE(ang, sin, cos);
			re3210 = mag * cos;
			im3210 = mag * sin;

			// インターリーブ
			__m128 im10re10 = _mm_movelh_ps(re3210, im3210);
			__m128 im32re32 = _mm_movehl_ps(im3210, re3210);
			__m128 im1re1im0re0 = _mm_shuffle_ps(im10re10, im10re10, _MM_SHUFFLE(3,1,2,0));
			__m128 im3re3im2re2 = _mm_shuffle_ps(im32re32, im32re32, _MM_SHUFFLE(3,1,2,0));
			*(__m128*)(synthwork + i*2    ) = im1re1im0re0;
			*(__m128*)(synthwork + i*2 + 4) = im3re3im2re2;
		}
	}

	// FFT を実行する
	synthwork[1] = 0.0; // synthwork[1] = nyquist freq. power (どっちみち使えないので0に)
	rdft(FrameSize, -1, synthwork, FFTWorkIp, FFTWorkW); // Inverse Real DFT
}
//---------------------------------------------------------------------------

