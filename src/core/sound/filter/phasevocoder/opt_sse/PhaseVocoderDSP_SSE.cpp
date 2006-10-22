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
	tRisaPhaseVocoderDSP_SSE_Trampoline(unsigned int framesize, unsigned int oversamp,
					unsigned int frequency, unsigned int channels) :
				tRisaPhaseVocoderDSP(framesize, oversamp, frequency, channels) {;}


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

	__m128 exact_time_scale = _mm_load1_ps(&ExactTimeScale);
	__m128 over_sampling_radian_v = _mm_load1_ps(&OverSamplingRadian);

	if(FrequencyScale != 1.0)
	{
		// ここでは 4 複素数 (8実数) ごとに処理を行う。

		for(unsigned int i = 0; i < framesize_d2; i ++)
		{
			// 直交座標系→極座標系
			float re = analwork[i*2  ];
			float im = analwork[i*2+1];

			float mag = sqrt(re*re + im*im); // mag = √(re^2+im^2)
			float ang = RisaVFast_arctan2(im, re); // ang = atan(im/re)

			// 前回の位相との差をとる
			// --注意: ここで使用しているFFTパッケージは、
			// --      ソース先頭の参考資料などで示しているFFTと
			// --      出力される複素数の虚数部の符号が逆なので
			// --      (共役がでてくるので)注意が必要。ここでも符号を
			// --      逆の物として扱う。
			float tmp = LastAnalPhase[ch][i] - ang;
			LastAnalPhase[ch][i] = ang; // 今回の値を保存

			// over sampling の影響を考慮する
			// -- 通常、FrameSize で FFT の１周期であるところを、
			// -- 精度を補うため、OverSampling 倍の周期で演算をしている。
			// -- そのために生じる位相のずれを修正する。
			tmp -= i * OverSamplingRadian;

			// unwrapping をする
			// -- tmp が -M_PI ～ +M_PI の範囲に収まるようにする
			int rad_unit = static_cast<int>(tmp*(1.0/M_PI));
			if (rad_unit >= 0) rad_unit += rad_unit&1;
			else rad_unit -= rad_unit&1;
			tmp -= M_PI*(double)rad_unit;

			// -M_PI～+M_PIを-1.0～+1.0の変位に変換
			tmp =  tmp * OverSamplingRadianRecp;

			// tmp をフィルタバンド中央からの周波数の変位に変換し、
			// それにフィルタバンドの中央周波数を加算する
			// -- i * FrequencyPerFilterBand はフィルタバンドの中央周波数を
			// -- 表し、tmp * FrequencyPerFilterBand は フィルタバンド中央から
			// -- の周波数の変位を表す。これらをあわせた物が、そのフィルタ
			// -- バンド内での「真」の周波数である。
			float freq = (i + tmp) *FrequencyPerFilterBand;

			// analwork に値を格納する
			analwork[i*2  ] = mag;
			analwork[i*2+1] = freq;
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
		for(unsigned int i = 0; i < framesize_d2; i ++)
		{
			float mag  = synthwork[i*2  ];
			float freq = synthwork[i*2+1];

			// 周波数から各フィルタバンドの中央周波数を減算し、
			// フィルタバンドの中央周波数からの-1.0～+1.0の変位
			// に変換する
			float tmp = freq * FrequencyPerFilterBandRecp - (float)i;

			// -1.0～+1.0の変位を-M_PI～+M_PIの位相に変換
			tmp =  tmp * OverSamplingRadian;

			// OverSampling による位相の補正
			tmp += i   * OverSamplingRadian;

			// TimeScale による位相の補正
			// TimeScale で出力が時間軸方向にのびれば(あるいは縮めば)、
			// 位相の差分もそれに伴ってのびる(縮む)
			tmp *= ExactTimeScale;

			// 前回の位相と加算する
			// ここでも虚数部の符号が逆になるので注意
			LastSynthPhase[ch][i] -= tmp;
			float ang = LastSynthPhase[ch][i];

			// 極座標系→直交座標系
			float c, s;
			RisaVFast_sincos(ang, s, c);
			synthwork[i*2  ] = mag * c;
			synthwork[i*2+1] = mag * s;
		}
	}
	else
	{
		// 周波数軸方向にシフトがない場合
		// ここでも 4 複素数 (8実数) ごとに処理を行う。

		static float * tmpv = NULL;
		if(!tmpv) tmpv = (float *)RisseAlignedAlloc(sizeof(float) * (framesize_d2), 4);
		static float * magv = NULL;
		if(!magv) magv = (float *)RisseAlignedAlloc(sizeof(float) * (framesize_d2), 4);
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

			*(__m128*)(tmpv + i) = ang;
			*(__m128*)(magv + i) = mag;
		}

//--
		for(unsigned int i = 0; i < framesize_d2; i ++)
		{
		float ang = tmpv[i];
		float mag = magv[i];


			// 極座標系→直交座標系
			float c, s;
			RisaVFast_sincos(ang, s, c);
			synthwork[i*2  ] = mag * c;
			synthwork[i*2+1] = mag * s;
		}
	}
}
//---------------------------------------------------------------------------

