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

/*
	Phase Vocoder (フェーズ ボコーダ ; 位相ボコーダ)の実装

	参考資料:

		http://www.panix.com/~jens/pvoc-dolson.par
			Phase Vocoder のチュートリアル。「ミュージシャンにもわかるように」
			書かれており、数学音痴フレンドリー。

		http://www.dspdimension.com/
			無料(オープンソースではない)の Time Stretcher/Pitch Shifterの
			DIRACや、各種アルゴリズムの説明、
			Pitch Shifter の説明的なソースコードなど。

		http://soundlab.cs.princeton.edu/software/rt_pvc/
			real-time phase vocoder analysis/synthesis library + visualization
			ソースあり。
*/

#include "prec.h"
#include <math.h>
#include "base/mathlib/RealFFT.h"
#include "base/mathlib/MathAlgorithms.h"
#include "sound/filter/phasevocoder/PhaseVocoderDSP.h"
#include <string.h>
#include "risseUtils.h"

RISSE_DEFINE_SOURCE_ID(46678,10832,40512,19852,21662,48847,10996,40273);

//---------------------------------------------------------------------------
tRisaPhaseVocoderDSP::tRisaPhaseVocoderDSP(
				unsigned int framesize, unsigned int oversamp,
				unsigned int frequency, unsigned int channels) :
					InputBuffer(framesize * 4 * channels),
					OutputBuffer(framesize * 4 * channels)
		// InputBuffer は最低でも
		// channels * (framesize + (framesize/oversamp)) 必要で、
		// OutputBuffer は最低でも
		// channels * (framesize + (framesize/oversamp)*MAX_TIME_SCALE) 必要
{
	// フィールドの初期化
	FFTWorkIp = NULL;
	FFTWorkW = NULL;
	InputWindow = NULL;
	OutputWindow = NULL;
	AnalWork = NULL;
	SynthWork = NULL;
	LastAnalPhase = NULL;
	LastSynthPhase = NULL;

	FrameSize = framesize;
	OverSampling = oversamp;
	Frequency = frequency;
	Channels = channels;
	InputHopSize = OutputHopSize = FrameSize / OverSampling;

	TimeScale = 1.0;
	FrequencyScale = 1.0;
	RebuildParams = true; // 必ず初回にパラメータを再構築するように真

	LastSynthPhaseAdjustCounter = 0;

	try
	{
		// ワークなどの確保
		AnalWork  = (float **)RisseAlignedAlloc(sizeof(float *) * Channels, 4);
		SynthWork = (float **)RisseAlignedAlloc(sizeof(float *) * Channels, 4);
		for(unsigned int ch = 0; ch < Channels; ch++)
			AnalWork[ch] = NULL, SynthWork[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			AnalWork[ch]  = (float *)RisseAlignedAlloc(sizeof(float) * (FrameSize), 4);
			SynthWork[ch] = (float *)RisseAlignedAlloc(sizeof(float) * (FrameSize), 4);
		}

		LastAnalPhase = (float **)RisseAlignedAlloc(sizeof(float *) * Channels, 4);
		for(unsigned int ch = 0; ch < Channels; ch++)
			LastAnalPhase[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			LastAnalPhase[ch] = (float *)RisseAlignedAlloc(sizeof(float) * (FrameSize/2), 4);
			memset(LastAnalPhase[ch], 0, FrameSize/2 * sizeof(float)); // 0 でクリア
		}

		LastSynthPhase = (float **)RisseAlignedAlloc(sizeof(float *) * Channels, 4);
		for(unsigned int ch = 0; ch < Channels; ch++)
			LastSynthPhase[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			LastSynthPhase[ch] = (float *)RisseAlignedAlloc(sizeof(float) * (FrameSize/2), 4);
			memset(LastSynthPhase[ch], 0, FrameSize/2 * sizeof(float)); // 0 でクリア
		}

		FFTWorkIp = (int *)RisseAlignedAlloc(sizeof(int) * (static_cast<int>(2+sqrt((double)FrameSize/4))), 4);
		FFTWorkIp[0] = FFTWorkIp[1] = 0;
		FFTWorkW = (float *)RisseAlignedAlloc(sizeof(float) * (FrameSize/2), 4);
		InputWindow = (float *)RisseAlignedAlloc(sizeof(float) * FrameSize, 4);
		OutputWindow = (float *)RisseAlignedAlloc(sizeof(float) * FrameSize, 4);
	}
	catch(...)
	{
		Clear();
		throw;
	}

	// 入出力バッファの内容をクリア
	float *bufp1;
	size_t buflen1;
	float *bufp2;
	size_t buflen2;

	InputBuffer.GetWritePointer(InputBuffer.GetSize(),
							bufp1, buflen1, bufp2, buflen2);
	if(bufp1) memset(bufp1, 0, sizeof(float)*buflen1);
	if(bufp2) memset(bufp2, 0, sizeof(float)*buflen2);

	OutputBuffer.GetWritePointer(OutputBuffer.GetSize(),
							bufp1, buflen1, bufp2, buflen2);
	if(bufp1) memset(bufp1, 0, sizeof(float)*buflen1);
	if(bufp2) memset(bufp2, 0, sizeof(float)*buflen2);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaPhaseVocoderDSP::~tRisaPhaseVocoderDSP()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::SetTimeScale(float v)
{
	TimeScale = v;
	if(TimeScale < MIN_TIME_SCALE) TimeScale = MIN_TIME_SCALE;
	else if(TimeScale > MAX_TIME_SCALE) TimeScale = MAX_TIME_SCALE;
	RebuildParams = true;
	OutputHopSize = static_cast<unsigned int>(InputHopSize * TimeScale) & ~1;
		// ↑ 偶数にアライン(重要)
		// 複素数 re,im, re,im, ... の配列が逆FFTにより同数の(複素数の個数×2の)
		// PCMサンプルに変換されるため、PCMサンプルも２個ずつで扱わないとならない.
		// この実際の OutputHopSize に従って ExactTimeScale が計算される.
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::SetFrequencyScale(float v)
{
	FrequencyScale = v;
	RebuildParams = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::Clear()
{
	// 全てのバッファなどを解放する
	if(AnalWork)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			RisseAlignedDealloc(AnalWork[ch]), AnalWork[ch] = NULL;
		RisseAlignedDealloc(AnalWork), AnalWork = NULL;
	}
	if(SynthWork)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			RisseAlignedDealloc(SynthWork[ch]), SynthWork[ch] = NULL;
		RisseAlignedDealloc(SynthWork), SynthWork = NULL;
	}
	if(LastAnalPhase)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			RisseAlignedDealloc(LastAnalPhase[ch]), LastAnalPhase[ch] = NULL;
		RisseAlignedDealloc(LastAnalPhase), LastAnalPhase = NULL;
	}
	if(LastSynthPhase)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			RisseAlignedDealloc(LastSynthPhase[ch]), LastSynthPhase[ch] = NULL;
		RisseAlignedDealloc(LastSynthPhase), LastSynthPhase = NULL;
	}
	RisseAlignedDealloc(FFTWorkIp), FFTWorkIp = NULL;
	RisseAlignedDealloc(FFTWorkW), FFTWorkW = NULL;
	RisseAlignedDealloc(InputWindow), InputWindow = NULL;
	RisseAlignedDealloc(OutputWindow), OutputWindow = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisaPhaseVocoderDSP::GetInputFreeSize()
{
	return InputBuffer.GetFreeSize() / Channels;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaPhaseVocoderDSP::GetInputBuffer(
	size_t numsamplegranules,
	float * & p1, size_t & p1size,
	float * & p2, size_t & p2size)
{
	size_t numsamples = numsamplegranules * Channels;

	if(InputBuffer.GetFreeSize() < numsamples) return false; // 十分な空き容量がない

	InputBuffer.GetWritePointer(numsamples, p1, p1size, p2, p2size);

	p1size /= Channels;
	p2size /= Channels;

	InputBuffer.AdvanceWritePos(numsamples);

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisaPhaseVocoderDSP::GetOutputReadySize()
{
	return OutputBuffer.GetDataSize() / Channels;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaPhaseVocoderDSP::GetOutputBuffer(
	size_t numsamplegranules,
	const float * & p1, size_t & p1size,
	const float * & p2, size_t & p2size)
{
	size_t numsamples = numsamplegranules * Channels;

	if(OutputBuffer.GetDataSize() < numsamples) return false; // 十分な準備済みサンプルがない

	OutputBuffer.GetReadPointer(numsamples, p1, p1size, p2, p2size);

	p1size /= Channels;
	p2size /= Channels;

	OutputBuffer.AdvanceReadPos(numsamples);

	return true;
}
//---------------------------------------------------------------------------

static float arctan2(float y, float x)
{
   static const float coeff_1 = M_PI/4;
   static const float coeff_2 = 3*coeff_1;
   float angle;
   float abs_y = fabs(y)+1e-10;     // kludge to prevent 0/0 condition
   if (x>=0)
   {
      float r = (x - abs_y) / (x + abs_y);
      angle = coeff_1 - coeff_1 * r;
   }
   else
   {
      float r = (x + abs_y) / (abs_y - x);
      angle = coeff_2 - coeff_1 * r;
   }
   if (y < 0)
     return(-angle);     // negate if in quad III or IV
   else
     return(angle);
}

//http://arxiv.org/PS_cache/cs/pdf/0406/0406049.pdf
/* define FASTER_SINCOS for the slightly-less-accurate results in slightly less time */
#define FASTER_SINCOS
#if !defined(FASTER_SINCOS) /* these coefficients generate a badly un-normalized sine-cosine pair, but the angle */
#define ss1 1.5707963268
#define ss2 -0.6466386396
#define ss3 0.0679105987
#define ss4 -0.0011573807
#define cc1 -1.2341299769
#define cc2 0.2465220241
#define cc3 -0.0123926179
#else
 /* use 20031003 coefficients for fast, normalized series*/
#define ss1 1.5707963235
#define ss2 -0.645963615
#define ss3 0.0796819754
#define ss4 -0.0046075748
#define cc1 -1.2336977925
#define cc2 0.2536086171
#define cc3 -0.0204391631
#endif

static inline float madd(float a, float b, float c) { return a*b+c; }
//static inline float round(float a) { return a; }
static inline float nmsub(float a, float b, float c) { return -(a*b-c); }

void fastsincos(float v, float &sin, float &cos)
{
	float s1, s2, c1, c2, fixmag1;
	float x1=madd(v, (float)(1.0/(2.0*3.1415926536)), (float)(0.0));
	/* q1=x/2pi reduced onto (-0.5,0.5), q2=q1**2 */
	float q1=nmsub(round(x1), (float)(1.0), x1);
	float q2=madd(q1, q1, (float)(0.0));
	s1= madd(q1,
			madd(q2,
				madd(q2,
					madd(q2, (float)(ss4),
								(float)(ss3)),
									(float)( ss2)),
							(float)(ss1)),
						(float)(0.0));
	c1= madd(q2,
			madd(q2,
				madd(q2, (float)(cc3),
				(float)(cc2)),
			(float)(cc1)),
		(float)(1.0));

	/* now, do one out of two angle-doublings to get sin & cos theta/2 */
	c2=nmsub(s1, s1, madd(c1, c1, (float)(0.0)));
	s2=madd((float)(2.0), madd(s1, c1, (float)(0.0)), (float)(0.0));

	/* now, cheat on the correction for magnitude drift...
	if the pair has drifted to (1+e)*(cos, sin),
	the next iteration will be (1+e)**2*(cos, sin)
	which is, for small e, (1+2e)*(cos,sin).
	However, on the (1+e) error iteration,
	sin**2+cos**2=(1+e)**2=1+2e also,
	so the error in the square of this term
	will be exactly the error in the magnitude of the next term.
	Then, multiply final result by (1-e) to correct */

#if defined(FASTER_SINCOS)
	/* this works with properly normalized sine-cosine functions, but un-normalized is more */
	fixmag1=nmsub(s2,s2, nmsub(c2, c2, (float)(2.0)));
#else /* must use this method with un-normalized series, since magnitude error is large */
	fixmag1=Reciprocal(madd(s2,s2,madd(c2,c2,(float)(0.0))));
#endif

	c1=nmsub(s2, s2, madd(c2, c2, (float)(0.0)));
	s1=madd((float)(2.0), madd(s2, c2, (float)(0.0)), (float)(0.0));
	cos=madd(c1, fixmag1, (float)(0.0));
	sin=madd(s1, fixmag1, (float)(0.0));
}



//---------------------------------------------------------------------------
tRisaPhaseVocoderDSP::tStatus tRisaPhaseVocoderDSP::Process()
{
	// いくつかの値をローカル変数に持っておく
	unsigned int framesize_d2 = FrameSize / 2;

	// パラメータの再計算の必要がある場合は再計算をする
	if(RebuildParams)
	{
		// 窓関数の計算(ここではHamming窓)
		float output_volume = 
			TimeScale / FrameSize  / sqrt(FrequencyScale) / OverSampling * 4;
		for(unsigned int i = 0; i < FrameSize; i++)
		{
			double window = cos(2.0*M_PI*((double)i+0.5)/FrameSize) * -0.46 + 0.54;
			InputWindow[i]  = (float)(window);
			OutputWindow[i] = (float)(window *output_volume);
		}

		// そのほかのパラメータの再計算
		OverSamplingRadian = (float)((2.0*M_PI)/OverSampling);
		OverSamplingRadianRecp = (float)(1.0/OverSamplingRadian);
		FrequencyPerFilterBand = (float)((double)Frequency/FrameSize);
		FrequencyPerFilterBandRecp = (float)(1.0/FrequencyPerFilterBand);
		ExactTimeScale = (float)OutputHopSize / InputHopSize;

		// フラグを倒す
		RebuildParams = false;
	}

	// 入力バッファ内のデータは十分か？
	if(InputBuffer.GetDataSize() < FrameSize * Channels)
		return psInputNotEnough; // 足りない

	// 出力バッファの空きは十分か？
	if(OutputBuffer.GetFreeSize() < FrameSize * Channels)
		return psOutputFull; // 足りない

	// これから書き込もうとする OutputBuffer の領域の最後の OutputHopSize サンプル
	// グラニュールは 0 で埋める (オーバーラップ時にはみ出す部分なので)
	{
		float *p1, *p2;
		size_t p1len, p2len;

		OutputBuffer.GetWritePointer(OutputHopSize*Channels,
				p1, p1len, p2, p2len, (FrameSize - OutputHopSize)*Channels);
		memset(p1, 0, p1len * sizeof(float));
		if(p2) memset(p2, 0, p2len * sizeof(float));
	}

	// 窓関数を適用しつつ、入力バッファから AnalWork に読み込む
	{
		const float *p1, *p2;
		size_t p1len, p2len;
		InputBuffer.GetReadPointer(FrameSize*Channels, p1, p1len, p2, p2len);
		p1len /= Channels;
		p2len /= Channels;
		RisaDeinterleaveApplyingWindow(AnalWork, p1, InputWindow, Channels, 0, p1len);
		if(p2)
			RisaDeinterleaveApplyingWindow(AnalWork, p2, InputWindow + p1len, Channels, p1len, p2len);
	}

	// チャンネルごとに処理
	for(unsigned int ch = 0; ch < Channels; ch++)
	{
		float * analwork = AnalWork[ch];
		float * synthwork = SynthWork[ch];

		//------------------------------------------------
		// 解析
		//------------------------------------------------

		// FFT を実行する
		rdft(FrameSize, 1, analwork, FFTWorkIp, FFTWorkW); // Real DFT


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
				float ang = atan2(im, re); // ang = atan(im/re)

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
				synthwork[i*2  ] = mag * cos(ang);
				synthwork[i*2+1] = mag * sin(ang);
			}
		}
		else
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
				float ang = arctan2(im, re); // ang = atan(im/re)

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
//				tmp -= (2.0*M_PI) * (int)(tmp * (1.0/(2.0*M_PI))); // ←どうもひつようっぽい


				int rad_unit = static_cast<int>(tmp*(1.0/M_PI));
				if (rad_unit >= 0) rad_unit += rad_unit&1;
				else rad_unit -= rad_unit&1;
				tmp -= M_PI*(double)rad_unit;

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
				fastsincos(ang, s, c);
				synthwork[i*2  ] = mag * c;
				synthwork[i*2+1] = mag * s;
			}
		}

		// FFT を実行する
		rdft(FrameSize, -1, synthwork, FFTWorkIp, FFTWorkW); // Inverse Real DFT
	}

	// 窓関数を適用しつつ、SynthWork から出力バッファに書き込む
	{
		float *p1, *p2;
		size_t p1len, p2len;

		OutputBuffer.GetWritePointer(FrameSize*Channels, p1, p1len, p2, p2len);
		p1len /= Channels;
		p2len /= Channels;
		RisaInterleaveOverlappingWindow(p1, SynthWork, OutputWindow, Channels, 0, p1len);
		if(p2)
			RisaInterleaveOverlappingWindow(p2, SynthWork, OutputWindow + p1len, Channels, p1len, p2len);
	}

	// LastSynthPhase を再調整するか
	LastSynthPhaseAdjustCounter += LastSynthPhaseAdjustIncrement;
	if(LastSynthPhaseAdjustCounter >= LastSynthPhaseAdjustInterval)
	{
		// LastSynthPhase を再調整するカウントになった
		LastSynthPhaseAdjustCounter = 0;

		// ここで行う調整は LastSynthPhase の unwrapping である。
		// LastSynthPhase は位相の差が累積されるので大きな数値になっていくが、
		// 適当な間隔でこれを unwrapping しないと、いずれ(数値が大きすぎて)精度
		// 落ちが発生し、正常に合成が出来なくなってしまう。
		// ただし、精度が保たれればよいため、毎回この unwrapping を行う必要はない。
		// ここでは LastSynthPhaseAdjustInterval/LastSynthPhaseAdjustIncrement 回ごとに調整を行う。
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			for(unsigned int i = 0; i < framesize_d2; i++)
			{
				long int n = static_cast<long int>(LastSynthPhase[ch][i] / (2.0*M_PI));
				LastSynthPhase[ch][i] -= n * (2.0*M_PI);
			}
		}
	}

	// 入出力バッファのポインタを進める
	OutputBuffer.AdvanceWritePos(OutputHopSize * Channels);
	InputBuffer.AdvanceReadPos(InputHopSize * Channels);

	// ステータス = no error
	return psNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::Deinterleave(float * dest, const float * src,
					float * win, size_t len)
{
	unsigned int numch = Channels;
	while(len--)
	{
		dest[0] = *src * *win;
		src += numch;
		dest ++;
		win ++;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::Interleave(float * dest, const float * src,
					float * win, size_t len)
{
	unsigned int numch = Channels;
	while(len--)
	{
		*dest += *src * *win;
		src ++;
		dest += numch;
		win ++;
	}
}
//---------------------------------------------------------------------------

