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


#include <math.h>
#include "fftsg.h"
#include "PhaseVocoderDSP.h"
#include <string.h>

//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		framesize		フレームサイズ(2の累乗, 16～)
//! @param		oversamp		オーバーサンプリング係数(2の累乗, 2～)
//! @param		frequency		入力PCMのサンプリングレート
//! @param		channels		入力PCMのチャンネル数
//! @note		音楽用ではframesize=4096,oversamp=16ぐらいがよく、
//! @note		ボイス用ではframesize=128,oversamp=4ぐらいがよい。
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
	InputHopSize = FrameSize / OverSampling;

	TimeScale = 1.0;
	FrequencyScale = 1.0;
	RebuildParams = true; // 必ず初回にパラメータを再構築するように真

	LastSynthPhaseAdjustConter = 0;

	try
	{
		// ワークなどの確保
		AnalWork = new float [FrameSize];
		SynthWork = new float [FrameSize];

		LastAnalPhase = new float * [Channels];
		for(unsigned int ch = 0; ch < Channels; ch++)
			LastAnalPhase[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			LastAnalPhase[ch] = new float [FrameSize/2];
			memset(LastAnalPhase[ch], 0, FrameSize/2 * sizeof(float)); // 0 でクリア
		}

		LastSynthPhase = new float * [Channels];
		for(unsigned int ch = 0; ch < Channels; ch++)
			LastSynthPhase[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			LastSynthPhase[ch] = new float [FrameSize/2];
			memset(LastSynthPhase[ch], 0, FrameSize/2 * sizeof(float)); // 0 でクリア
		}

		FFTWorkIp = new int[static_cast<int>(2+sqrt((double)FrameSize/4))];
		FFTWorkIp[0] = FFTWorkIp[1] = 0;
		FFTWorkW = new float[FrameSize/2];
		InputWindow = new float[FrameSize];
		OutputWindow = new float[FrameSize];
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
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaPhaseVocoderDSP::~tRisaPhaseVocoderDSP()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		時間軸方向のスケールを設定する
//! @param		v     スケール
//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::SetTimeScale(float v)
{
	TimeScale = v;
	if(TimeScale < MIN_TIME_SCALE) TimeScale = MIN_TIME_SCALE;
	else if(TimeScale > MAX_TIME_SCALE) TimeScale = MAX_TIME_SCALE;
	RebuildParams = true;
} 
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		周波数軸方向のスケールを設定する
//! @param		v     スケール
//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::SetFrequencyScale(float v)
{
	FrequencyScale = v;
	RebuildParams = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		クリア
//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::Clear()
{
	// 全てのバッファなどを解放する
	delete [] AnalWork, AnalWork = NULL;
	delete [] SynthWork, SynthWork = NULL;
	if(LastAnalPhase)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			delete [] LastAnalPhase[ch], LastAnalPhase[ch] = NULL;
		delete [] LastAnalPhase, LastAnalPhase = NULL;
	}
	if(LastSynthPhase)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			delete [] LastSynthPhase[ch], LastSynthPhase[ch] = NULL;
		delete [] LastSynthPhase, LastSynthPhase = NULL;
	}
	delete [] LastSynthPhase, LastSynthPhase = NULL;
	delete [] FFTWorkIp, FFTWorkIp = NULL;
	delete [] FFTWorkW, FFTWorkW = NULL;
	delete [] InputWindow, InputWindow = NULL;
	delete [] OutputWindow, OutputWindow = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		入力バッファの空きサンプルグラニュール数を得る
//! @return		入力バッファの空きサンプルグラニュール数
//---------------------------------------------------------------------------
size_t tRisaPhaseVocoderDSP::GetInputFreeSize()
{
	return InputBuffer.GetFreeSize() / Channels;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		入力バッファの書き込みポインタを得る
//! @param		numsamplegranules 書き込みたいサンプルグラニュール数
//! @param		p1		ブロック1の先頭へのポインタを格納するための変数
//! @param		p1size	p1の表すブロックのサンプルグラニュール数
//! @param		p2		ブロック2の先頭へのポインタを格納するための変数(NULLがあり得る)
//! @param		p2size	p2の表すブロックのサンプルグラニュール数(0があり得る)
//! @return		空き容量が足りなければ偽、空き容量が足り、ポインタが返されれば真
//! @note		p1 と p2 のように２つのポインタとそのサイズが返されるのは、
//!				このバッファが実際はリングバッファで、リングバッファ内部のリニアなバッファ
//!				の終端をまたぐ可能性があるため。またがない場合はp2はNULLになるが、またぐ
//!				場合は p1 のあとに p2 に続けて書き込まなければならない。
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
//! @brief		出力バッファの準備済みサンプルグラニュール数を得る
//! @return		出力バッファの準備済みサンプルグラニュール数
//---------------------------------------------------------------------------
size_t tRisaPhaseVocoderDSP::GetOutputReadySize()
{
	return OutputBuffer.GetDataSize() / Channels;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		出力バッファの読み込みポインタを得る
//! @param		numsamplegranules 読み込みたいサンプルグラニュール数
//! @param		p1		ブロック1の先頭へのポインタを格納するための変数
//! @param		p1size	p1の表すブロックのサンプルグラニュール数
//! @param		p2		ブロック2の先頭へのポインタを格納するための変数(NULLがあり得る)
//! @param		p2size	p2の表すブロックのサンプルグラニュール数(0があり得る)
//! @return		準備されたサンプルが足りなければ偽、サンプルが足り、ポインタが返されれば真
//! @note		p1 と p2 のように２つのポインタとそのサイズが返されるのは、
//!				このバッファが実際はリングバッファで、リングバッファ内部のリニアなバッファ
//!				の終端をまたぐ可能性があるため。またがない場合はp2はNULLになるが、またぐ
//!				場合は p1 のあとに p2 を続けて読み出さなければならない。
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

//---------------------------------------------------------------------------
//! @brief		処理を1ステップ行う
//! @return		処理結果を表すenum
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
		OutputHopSize = static_cast<unsigned int>(InputHopSize * TimeScale) & ~1;
			// ↑ 偶数にアライン(重要)
			// 複素数 re,im, re,im, ... の配列が逆FFTにより同数の(複素数の個数×2の)
			// PCMサンプルに変換されるため、PCMサンプルも２個ずつで扱わないとならない.
			// この実際の OutputHopSize に従って ExactTimeScale が計算される.
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

	// チャンネルごとに処理
	for(unsigned int ch = 0; ch < Channels; ch++)
	{
		//------------------------------------------------
		// 解析
		//------------------------------------------------

		// 窓関数を適用しつつ、入力バッファから AnalWork に読み込む
		{
			const float *p1, *p2;
			size_t p1len, p2len;
			InputBuffer.GetReadPointer(FrameSize*Channels, p1, p1len, p2, p2len);
			p1len /= Channels;
			p2len /= Channels;
			Deinterleave(AnalWork, p1 + ch, InputWindow, p1len);
			if(p2)
				Deinterleave(AnalWork + p1len, p2 + ch,
							InputWindow + p1len, p2len);
		}

		// FFT を実行する
		rdft(FrameSize, 1, AnalWork, FFTWorkIp, FFTWorkW); // Real DFT

		// 各フィルタバンドごとに変換
		//-- 各フィルタバンドごとの音量と周波数を求める。
		//-- FFT を実行すると各フィルタバンドごとの値が出てくるが、
		//-- フィルタバンドというバンドパスフィルタの幅の中で
		//-- 周波数のピークが本当はどこにあるのかは、前回計算した
		//-- 位相との差をとってみないとわからない。
		for(unsigned int i = 0; i < framesize_d2; i ++)
		{
			// 直交座標系→極座標系
			float re = AnalWork[i*2  ];
			float im = AnalWork[i*2+1];

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

			// AnalWork に値を格納する
			AnalWork[i*2  ] = mag;
			AnalWork[i*2+1] = freq;
		}


		//------------------------------------------------
		// 変換
		//------------------------------------------------

		// 周波数軸方向のリサンプリングを行う
		if(FrequencyScale != 1.0)
		{
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
					SynthWork[i*2  ] =
						AnalWork[index*2  ] +
						frac * (AnalWork[index*2+2]-AnalWork[index*2  ]);
					SynthWork[i*2+1] =
						FrequencyScale * (
						AnalWork[index*2+1] +
						frac * (AnalWork[index*2+3]-AnalWork[index*2+1]) );
				}
				else if(index < framesize_d2)
				{
					SynthWork[i*2  ] = AnalWork[index*2  ];
					SynthWork[i*2+1] = AnalWork[index*2+1] * FrequencyScale;
				}
				else
				{
					SynthWork[i*2  ] = 0.0;
					SynthWork[i*2+1] = 0.0;
				}
			}
		}
		else
		{
			memcpy(SynthWork, AnalWork, FrameSize * sizeof(float));
		}


		//------------------------------------------------
		// 合成
		//------------------------------------------------

		// 各フィルタバンドごとに変換
		// 基本的には解析の逆変換である
		for(unsigned int i = 0; i < framesize_d2; i ++)
		{
			float mag  = SynthWork[i*2  ];
			float freq = SynthWork[i*2+1];

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

			// 直交座標系→極座標系
			SynthWork[i*2  ] = mag * cos(ang);
			SynthWork[i*2+1] = mag * sin(ang);
		}

		// FFT を実行する
		rdft(FrameSize, -1, SynthWork, FFTWorkIp, FFTWorkW); // Inverse Real DFT

		// 窓関数を適用しつつ、SynthWork から出力バッファに書き込む
		{
			float *p1, *p2;
			size_t p1len, p2len;

			OutputBuffer.GetWritePointer(FrameSize*Channels, p1, p1len, p2, p2len);
			p1len /= Channels;
			p2len /= Channels;
			Interleave(p1 + ch, SynthWork, OutputWindow, p1len);
			if(p2)
				Interleave(p2 + ch, SynthWork + p1len,
							OutputWindow + p1len, p2len);
		}
	}

	// LastSynthPhase を再調整するか
	LastSynthPhaseAdjustConter += LastSynthPhaseAdjustIncrement;
	if(LastSynthPhaseAdjustConter >= LastSynthPhaseAdjustInterval)
	{
		// LastSynthPhase を再調整するカウントになった
		LastSynthPhaseAdjustConter = 0;

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
//! @brief		インターリーブを解除し、窓関数を掛けながら転送
//! @param		dest 転送先
//! @param		src  転送元(チャンネルごとにインターリーブされている)
//! @param		win  窓関数
//! @param		len  転送するサンプルグラニュール数
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
//! @brief		窓関数を掛けながら、インターリーブをしながら加算転送
//! @param		dest 転送先(チャンネルごとにインターリーブする)
//! @param		src  転送元
//! @param		win  窓関数
//! @param		len  転送するサンプルグラニュール数
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

