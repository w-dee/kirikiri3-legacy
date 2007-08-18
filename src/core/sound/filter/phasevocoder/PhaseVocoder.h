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
	static const int DEFAULT_FRAME_SIZE = 128; //!< default frame size
	static const int DEFAULT_OVERSAMPLING = 4; //!< default oversampling factor

	tPhaseVocoderDSP * DSP; //!< PhaseVocoder DSP

	int FrameSize; //!< フレームサイズ
	int OverSampling; //!< オーバーサンプリング係数
	float	TimeScale; //!< 時間軸方向のスケール(出力/入力)
	float	FrequencyScale; //!< 周波数方向のスケール(出力/入力)

	tWaveSegmentQueue SegmentQueue;

public:
	//! @brief		コンストラクタ
	tPhaseVocoderInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	~tPhaseVocoderInstance() {;}

	//! @brief		FFTフレームサイズを得る
	//! @return		FFTフレームサイズ
	int GetFrameSize() const;

	//! @brief		FFTフレームサイズを設定する
	//! @param		v FFTフレームサイズ
	void SetFrameSize(int v);

	//! @brief		オーバーサンプリング係数を得る
	//! @return		オーバーサンプリング係数
	int GetOverSampling() const;

	//! @brief		オーバーサンプリング係数を設定する
	//! @param		v オーバーサンプリング係数
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

	//! @brief		入力となるフィルタが変わったとき、あるいはリセットされるとき
	void InputChanged();

	//! @brief		フィルタ動作を行うとき
	void Filter();

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

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

