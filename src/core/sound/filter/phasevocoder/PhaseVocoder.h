//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief フェーズボコーダフィルタクラス
//---------------------------------------------------------------------------
class tPhaseVocoderInstance : public tWaveFilterInstance
{
	static const int DEFAULT_FRAME_SIZE = 4096; //!< default frame size
	static const int DEFAULT_OVERSAMPLING = 0; //!< default oversampling factor

	tPhaseVocoderDSP * DSP; //!< PhaseVocoder DSP

	int FrameSize; //!< フレームサイズ(ウィンドウサイズ)
	int OverSampling; //!< オーバーサンプリング係数(オーバーラップ)
	float	TimeScale; //!< 時間軸方向のスケール(出力/入力)
	float	FrequencyScale; //!< 周波数方向のスケール(出力/入力)

	tWaveSegmentQueue SegmentQueue;

public:
	//! @brief		コンストラクタ
	tPhaseVocoderInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	~tPhaseVocoderInstance() {;}

	//! @brief		FFTフレームサイズ(ウィンドウサイズ)を得る
	//! @return		FFTフレームサイズ(ウィンドウサイズ)
	int GetFrameSize() const;

	//! @brief		FFTフレームサイズ(ウィンドウサイズ)を設定する
	//! @param		v FFTフレームサイズ(ウィンドウサイズ)
	void SetFrameSize(int v);

	//! @brief		オーバーサンプリング係数(オーバーラップ)を得る
	//! @return		オーバーサンプリング係数(オーバーラップ)
	int GetOverSampling() const;

	//! @brief		オーバーサンプリング係数(オーバーラップ)を設定する
	//! @param		v オーバーサンプリング係数(オーバーラップ)
	void SetOverSampling(int v);

	//! @brief		時間軸方向のスケールを得る
	//! @return		時間軸方向のスケール
	float GetTimeScale() const;

	//! @brief		時間軸方向のスケールを設定する
	//! @param		v 時間軸方向のスケール
	void SetTimeScale(float v);

	//! @brief		周波数軸方向のスケールを得る
	//! @return		周波数軸方向のスケール
	float GetFrequencyScale() const;

	//! @brief		周波数軸方向のスケールを設定する
	//! @param		v 周波数軸方向のスケール
	void SetFrequencyScale(float v);

private:
	//! @brief		内容のクリア
	void Clear();

	//! @brief		DSPオブジェクトを作り直す
	void RebuildDSP();

	//! @brief		DSPオブジェクトの存在を確かにする
	void EnsureDSP();

	//! @brief		入力となるフィルタが変わったとき、あるいはリセットされるとき
	void InputChanged();

	//! @brief		フィルタ動作を行うとき
	void Filter();

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

	int get_window() { return GetFrameSize(); }
	void set_window(int v) { SetFrameSize(v); }
	int get_overlap() { return GetOverSampling(); }
	void set_overlap(int v) { SetOverSampling(v); }
	float get_time() { return GetTimeScale(); }
	void set_time(float v) { SetTimeScale(v); }
	float get_pitch() { return GetFrequencyScale(); }
	void set_pitch(float v) { SetFrequencyScale(v); }
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		"PhaseVocoder" クラス
//---------------------------------------------------------------------------
class tPhaseVocoderClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tPhaseVocoderClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public: // Risse 用メソッドなど
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
} // namespace Risa

#endif

