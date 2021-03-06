//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Phase Vocoder のCPUごとの最適化
//---------------------------------------------------------------------------

#include "risseTypes.h"
using namespace Risse;
#include <math.h>
#include "risa/common/MathAlgorithms.h"
#include "risa/packages/risa/sound/filter/phasevocoder/PhaseVocoderDSP.h"
#include "risa/common/RealFFT.h"
#include <string.h>



namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void tPhaseVocoderDSP::ProcessCore(int ch)
{
	unsigned int framesize_d2 = FrameSize / 2;
	float * analwork = AnalWork[ch];
	float * synthwork = SynthWork[ch];

	// FFT を実行する
	rdft(FrameSize, 1, analwork, FFTWorkIp, FFTWorkW); // Real DFT
	analwork[1] = 0.0; // analwork[1] = nyquist freq. power (どっちみち使えないので0に)

	if(FrequencyScale != 1.0)
	{
		// 各フィルタバンドごとに変換
		//-- 各フィルタバンドごとの音量と周波数を求める。
		//-- FFT を実行すると各フィルタバンドごとの値が出てくるが、
		//-- フィルタバンドというバンドパスフィルタの幅の中で
		//-- 周波数のピークが本当はどこにあるのかは、前回計算した
		//-- 位相との差をとってみないとわからない。
		for(unsigned int i = 0; i < framesize_d2; i ++)
		{
			// 直交座標系→極座標系
			float re = analwork[i*2  ];
			float im = analwork[i*2+1];

			float mag = sqrt(re*re + im*im); // mag = √(re^2+im^2)
			float ang = VFast_arctan2(im, re); // ang = atan(im/re)

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
			tmp = WrapPi_F1(tmp);

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
			VFast_sincos(ang, s, c);
			synthwork[i*2  ] = mag * c;
			synthwork[i*2+1] = mag * s;
		}
	}
	else
	{
		// 周波数軸方向にシフトがない場合

		// 各フィルタバンドごとに変換
		//-- 各フィルタバンドごとの音量と周波数を求める。
		//-- FFT を実行すると各フィルタバンドごとの値が出てくるが、
		//-- フィルタバンドというバンドパスフィルタの幅の中で
		//-- 周波数のピークが本当はどこにあるのかは、前回計算した
		//-- 位相との差をとってみないとわからない。
		for(unsigned int i = 0; i < framesize_d2; i ++)
		{
			// 直交座標系→極座標系
			float re = analwork[i*2  ];
			float im = analwork[i*2+1];

			float mag = sqrt(re*re + im*im); // mag = √(re^2+im^2)
			float ang = VFast_arctan2(im, re); // ang = atan(im/re)

			// 前回の位相との差をとる
			// --注意: ここで使用しているFFTパッケージは、
			// --      ソース先頭の参考資料などで示しているFFTと
			// --      出力される複素数の虚数部の符号が逆なので
			// --      (共役がでてくるので)注意が必要。ここでも符号を
			// --      逆の物として扱う。
			float tmp = LastAnalPhase[ch][i] - ang;
			LastAnalPhase[ch][i] = ang; // 今回の値を保存

			// phase shift
			float phase_shift = i * OverSamplingRadian;

			// over sampling の影響を考慮する
			// -- 通常、FrameSize で FFT の１周期であるところを、
			// -- 精度を補うため、OverSampling 倍の周期で演算をしている。
			// -- そのために生じる位相のずれを修正する。
			tmp -= phase_shift;

			// unwrapping をする
			// -- tmp が -M_PI ～ +M_PI の範囲に収まるようにする
			tmp = WrapPi_F1(tmp);

//--
			// OverSampling による位相の補正
			tmp += phase_shift;

			// TimeScale による位相の補正
			// TimeScale で出力が時間軸方向にのびれば(あるいは縮めば)、
			// 位相の差分もそれに伴ってのびる(縮む)
			tmp *= ExactTimeScale;

			// 前回の位相と加算する
			// ここでも虚数部の符号が逆になるので注意
			LastSynthPhase[ch][i] -= tmp;
			ang = LastSynthPhase[ch][i];

			// 極座標系→直交座標系
			float c, s;
			VFast_sincos(ang, s, c);
			synthwork[i*2  ] = mag * c;
			synthwork[i*2+1] = mag * s;
		}
	}

	// FFT を実行する
	synthwork[1] = 0.0; // synthwork[1] = nyquist freq. power (どっちみち使えないので0に)
	rdft(FrameSize, -1, SynthWork[ch], FFTWorkIp, FFTWorkW); // Inverse Real DFT
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

