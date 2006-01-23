//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 基本的なWaveFilterの各機能の実装
//---------------------------------------------------------------------------

#ifndef BasicWaveFilterH
#define BasicWaveFilterH

#include "WaveFilter.h"


//---------------------------------------------------------------------------
//! @brief WaveFilterの基本動作の実装
//---------------------------------------------------------------------------
class tRisaBasicWaveFilter : public tRisaWaveFilter
{
protected:
	// フィルタ管理
	boost::shared_ptr<tRisaWaveFilter> Input; //!< チェーンの前につながっているフィルタ
	tRisaWaveFormat InputFormat; //!< 入力フォーマット
	tRisaWaveFormat OutputFormat; //!< 出力フォーマット

	// 入力フォーマット変換関連
	risse_uint8 * ConvertBuffer; //!< 変換バッファ
	size_t ConvertBufferSize; //!< 変換バッファのサイズ

	// 出力バッファ(キュー)管理
	tRisaPCMTypes::tType DesiredOutputType; //!< 出力PCMタイプ
	risse_uint8 * QueuedData; //!< キューされた PCM データ
	risse_uint QueuedDataAllocSize; //!< キューに割り当てられたメモリサイズ(バイト単位)
	risse_uint QueuedSampleGranuleCount; //!< キューに入っている全体のサンプルグラニュール数
	risse_uint QueuedSampleGranuleRemain; //!< キューに入っている残りのサンプルグラニュール数
	std::vector<tRisaWaveSegment> QueuedSegments; //!< キューに入っているセグメント
	std::vector<tRisaWaveEvent> QueuedEvents; //!< キューに入っているイベント

public:
	tRisaBasicWaveFilter(tRisaPCMTypes::tType desired_output_type);
	~tRisaBasicWaveFilter();

	void SetInput(boost::shared_ptr<tRisaWaveFilter> input);
	bool Render(void *dest, risse_uint samples, risse_uint &written,
		std::vector<tRisaWaveSegment> &segments,
		std::vector<tRisaWaveEvent> &events);
	const tRisaWaveFormat & GetFormat();

protected:

	void * PrepareQueue(risse_uint numsamplegranules);
		// キューを準備する
	void Queue(risse_uint numsamplegranules,
		const std::vector<tRisaWaveSegment> &segments, const std::vector<tRisaWaveEvent> &events);
		// 出力キューにデータをおく

	risse_uint Fill(void * dest, risse_uint numsamplegranules, tRisaPCMTypes::tType desired_type,
		std::vector<tRisaWaveSegment> &segments, std::vector<tRisaWaveEvent> &events);
		// dest に最低でも numsamplegranules のサンプルグラニュールを書き込む
		// 実際に書き込まれたサンプル数が返る



protected:
	// 以下、サブクラスで実装すべきメソッド
	virtual void InputChanged() = 0; // 入力が変更された時やリセットされたときに呼ばれる
	virtual void Filter() = 0; // フィルタ動作が必要な時に呼ばれる

};
//---------------------------------------------------------------------------

#endif

