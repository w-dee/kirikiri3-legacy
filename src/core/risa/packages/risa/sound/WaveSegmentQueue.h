//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveセグメント/イベントキュー管理
//---------------------------------------------------------------------------
#ifndef WAVESEGMENTH
#define WAVESEGMENTH


#include <deque>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * 再生セグメント情報
 */
struct tWaveSegment : public tAtomicCollectee
{
	/**
	 * コンストラクタ
	 */
	tWaveSegment(risse_int64 start, risse_int64 length)
		{ Start = start; Length = FilteredLength = length; }
	tWaveSegment(risse_int64 start, risse_int64 length, risse_int64 filteredlength)
		{ Start = start; Length = length; FilteredLength = filteredlength; }
	risse_int64 Start; //!< オリジナルデコーダ上でのセグメントのスタート位置 (PCM サンプルグラニュール数単位)
	risse_int64 Length; //!< オリジナルデコーダ上でのセグメントの長さ (PCM サンプルグラニュール数単位)
	risse_int64 FilteredLength; //!< フィルタ後の長さ (PCM サンプルグラニュール数単位)
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 再生イベント情報
 */
struct tWaveEvent : public tCollectee
{
	/**
	 * コンストラクタ
	 */
	risse_int64 Position; //!< オリジナルデコーダ上でのラベル位置 (PCM サンプルグラニュール数単位)
	tString Name; //!< イベント名
	risse_int Offset;
		/*!< オフセット
			@note
			This member will be set in tWaveLoopManager::Render,
			and will contain the sample granule offset from first decoding
			point at call of tWaveLoopManager::Render().
		*/
	/**
	 * コンストラクタ
	 */
	tWaveEvent()
	{
		Position = 0;
		Offset = 0;
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * Waveのセグメント・イベントのキューを管理するクラス
 */
class tWaveSegmentQueue : public tCollectee
{
	// deque による Segments と Events の配列。
	// 実用上は、これらの配列に大量のデータが入ることはまずないので
	// vector で十分なのかもしれないが ...
	gc_deque<tWaveSegment> Segments; //!< セグメントの配列
	gc_deque<tWaveEvent> Events; //!< イベントの配列

public:
	/**
	 * 内容をクリアする
	 */
	void Clear();

	/**
	 * tWaveSegmentQueueをエンキューする
	 * @param queue	エンキューしたいtWaveSegmentQueueオブジェクト
	 */
	void Enqueue(const tWaveSegmentQueue & queue);

	/**
	 * tWaveSegmentをエンキューする
	 * @param queue	エンキューしたいtWaveSegmentオブジェクト
	 */
	void Enqueue(const tWaveSegment & segment);

	/**
	 * tWaveEventをエンキューする
	 * @param queue	エンキューしたいtWaveEventオブジェクト
	 * @note	Offset は修正されないので注意
	 */
	void Enqueue(const tWaveEvent & event);

	/**
	 * tWaveSegmentの配列をエンキューする
	 * @param queue	エンキューしたい std::dequeue<tWaveSegment>オブジェクト
	 */
	void Enqueue(const gc_deque<tWaveSegment> & segments);

	/**
	 * tWaveEventの配列をエンキューする
	 * @param queue	エンキューしたい std::dequeue<tWaveEvent>オブジェクト
	 */
	void Enqueue(const gc_deque<tWaveEvent> & events);

	/**
	 * 先頭から指定長さ分をデキューする
	 * @param dest		格納先キュー(内容はクリアされる)
	 * @param length	切り出す長さ(サンプルグラニュール単位)
	 */
	void Dequeue(tWaveSegmentQueue & dest, risse_int64 length);

	/**
	 * このキューの全体の長さを得る
	 * @return	このキューの長さ (サンプルグラニュール単位)
	 */
	risse_int64 GetFilteredLength() const;

	/**
	 * このキューの長さを変化させる
	 * @param new_total_filtered_length	新しいキューの長さ (サンプルグラニュール単位)
	 * @note	キュー中のSegments などの長さや Eventsの位置は線形補間される
	 */
	void Scale(risse_int64 new_total_length);

	/**
	 * フィルタされた位置からデコード位置へ変換を行う
	 * @param pos	フィルタされた位置
	 * @note	デコード位置
	 */
	risse_int64 FilteredPositionToDecodePosition(risse_int64 pos) const;

	/**
	 * イベント配列を得る
	 * @return	イベント配列
	 */
	gc_deque<tWaveEvent> & GetEvents() { return Events; }

	/**
	 * 内容を標準エラー出力にダンプする(デバッグ用)
	 */
	void Dump() const;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif

