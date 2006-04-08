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

#include "sound/WaveFilter.h"


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
	tRisaWaveSegmentQueue SegmentQueue; //!< キューに入っているセグメント

public:
	//! @brief		コンストラクタ
	//! @param		desired_output_type    サブクラスが望む PCM 形式
	tRisaBasicWaveFilter(tRisaPCMTypes::tType desired_output_type);

	//! @brief		デストラクタ
	~tRisaBasicWaveFilter();

	//! @brief		フィルタをリセットする際に呼ばれる
	void Reset();

	//! @brief		フィルタの入力を設定する
	//! @param		input  入力となるフィルタ
	void SetInput(boost::shared_ptr<tRisaWaveFilter> input);

	void SuggestFormat(const tRisaWaveFormat & format) {;}
		//!< @note ここではなにもしない。必要ならばサブクラスで実装すること。

	//! @brief		デコードを行う
	//! @param		dest		デコード結果を格納するバッファ
	//! @param		samples		デコードを行いたいサンプル数
	//! @param		written		実際にデコード出来たサンプル数
	//! @param		segmentqueue	再生セグメントキュー情報を書き込む先
	//! @return		まだデコードすべきデータが残っているかどうか
	bool Render(void *dest, risse_uint samples, risse_uint &written,
		tRisaWaveSegmentQueue & segmentqueue);

	//! @brief		PCM形式を返す
	//! @return		PCM形式への参照
	const tRisaWaveFormat & GetFormat();

protected:

	//! @brief		出力キューを準備する
	//! @param		numsamplegranules  準備したいサンプルグラニュール数
	//! @return		出力バッファ
	void * PrepareQueue(risse_uint numsamplegranules);

	//! @brief		出力キューにデータをおく
	//! @param		numsamplegranules	サンプル数
	//! @param		segmentqueue	再生セグメントキュー情報
	void Queue(risse_uint numsamplegranules,
		const tRisaWaveSegmentQueue & segmentqueue);

	//! @brief		指定されたバッファに入力フィルタから情報を読み出し、書き込む
	//! @param		dest				書き込みたいバッファ
	//! @param		numsamplegranules	欲しいサンプルグラニュール数
	//! @param		desired_type		欲しいPCM形式
	//! @param		fill_silence		欲しいサンプルグラニュール数に入力が満たないとき、残りを無音で埋めるかどうか
	//! @param		segmentqueue	再生セグメントキュー情報を書き込む先
	//! @return		実際に書き込まれたサンプルグラニュール数
	risse_uint Fill(void * dest, risse_uint numsamplegranules, tRisaPCMTypes::tType desired_type,
		bool fill_silence,
		tRisaWaveSegmentQueue & segmentqueue);



protected:
	// 以下、サブクラスで実装すべきメソッド
	virtual void InputChanged() = 0; // 入力が変更された時やリセットされたときに呼ばれる
	virtual void Filter() = 0; // フィルタ動作が必要な時に呼ばれる

};
//---------------------------------------------------------------------------

#endif

