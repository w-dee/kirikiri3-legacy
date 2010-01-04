//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 基本的なWaveFilterの各機能の実装
//---------------------------------------------------------------------------

#ifndef BasicWaveFilterH
#define BasicWaveFilterH

#include "risa/packages/risa/sound/WaveFilter.h"
#include "risseNativeBinder.h"
#include "risseClass.h"
#include "risseObjectBase.h"
#include "risa/common/RisseEngine.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * WaveFilterの基本動作の実装
 */
class tWaveFilterInstance : public tObjectBase, public tWaveFilter
{
protected:
	// フィルタ管理
	tWaveFilter * Input; //!< チェーンの前につながっているフィルタ
	tWaveFormat InputFormat; //!< 入力フォーマット
	tWaveFormat OutputFormat; //!< 出力フォーマット

	// 入力フォーマット変換関連
	risse_uint8 * ConvertBuffer; //!< 変換バッファ
	size_t ConvertBufferSize; //!< 変換バッファのサイズ

	// 出力バッファ(キュー)管理
	tPCMTypes::tType DesiredOutputType; //!< 出力PCMタイプ
	risse_uint8 * QueuedData; //!< キューされた PCM データ
	risse_uint QueuedDataAllocSize; //!< キューに割り当てられたメモリサイズ(バイト単位)
	risse_uint QueuedSampleGranuleCount; //!< キューに入っている全体のサンプルグラニュール数
	risse_uint QueuedSampleGranuleRemain; //!< キューに入っている残りのサンプルグラニュール数
	tWaveSegmentQueue SegmentQueue; //!< キューに入っているセグメント

public:
	/**
	 * コンストラクタ
	 * @param desired_output_type	サブクラスが望む PCM 形式
	 */
	tWaveFilterInstance(tPCMTypes::tType desired_output_type);

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	~tWaveFilterInstance();

	/**
	 * フィルタをリセットする際に呼ばれる
	 */
	void Reset();

	/**
	 * フィルタの入力を設定する
	 * @param input	入力となるフィルタ
	 */
	void SetInput(tWaveFilter * input);

	void SuggestFormat(const tWaveFormat & format) {;}
		//!< @note ここではなにもしない。必要ならばサブクラスで実装すること。

	/**
	 * デコードを行う
	 * @param dest			デコード結果を格納するバッファ
	 * @param samples		デコードを行いたいサンプル数
	 * @param written		実際にデコード出来たサンプル数
	 * @param segmentqueue	再生セグメントキュー情報を書き込む先
	 * @return	まだデコードすべきデータが残っているかどうか
	 */
	bool Render(void *dest, risse_uint samples, risse_uint &written,
		tWaveSegmentQueue & segmentqueue);

	/**
	 * PCM形式を返す
	 * @return	PCM形式への参照
	 */
	const tWaveFormat & GetFormat();

protected:

	/**
	 * 出力キューを準備する
	 * @param numsamplegranules	準備したいサンプルグラニュール数
	 * @return	出力バッファ
	 */
	void * PrepareQueue(risse_uint numsamplegranules);

	/**
	 * 出力キューにデータをおく
	 * @param numsamplegranules	サンプル数
	 * @param segmentqueue		再生セグメントキュー情報
	 */
	void Queue(risse_uint numsamplegranules,
		const tWaveSegmentQueue & segmentqueue);

	/**
	 * 指定されたバッファに入力フィルタから情報を読み出し、書き込む
	 * @param dest				書き込みたいバッファ
	 * @param numsamplegranules	欲しいサンプルグラニュール数
	 * @param desired_type		欲しいPCM形式
	 * @param fill_silence		欲しいサンプルグラニュール数に入力が満たないとき、残りを無音で埋めるかどうか
	 * @param segmentqueue		再生セグメントキュー情報を書き込む先(内容はクリアされずに追加される)
	 * @return	実際に書き込まれたサンプルグラニュール数
	 */
	risse_uint Fill(void * dest, risse_uint numsamplegranules, tPCMTypes::tType desired_type,
		bool fill_silence,
		tWaveSegmentQueue & segmentqueue);



protected:
	// 以下、サブクラスで実装すべきメソッド
	virtual void InputChanged() = 0; // 入力が変更された時やリセットされたときに呼ばれる
	virtual void Filter() = 0; // フィルタ動作が必要な時に呼ばれる


public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * "WaveFilter" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tWaveFilterClass, tClassBase, tWaveFilterInstance, itNoInstance)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
} // namespace Risa


#endif

