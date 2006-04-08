//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveセグメント/イベントキュー管理
//---------------------------------------------------------------------------
#ifndef WAVESEGMENTH
#define WAVESEGMENTH


#include <deque>


//---------------------------------------------------------------------------
//! @brief 再生セグメント情報
//---------------------------------------------------------------------------
struct tRisaWaveSegment
{
	//! @brief コンストラクタ
	tRisaWaveSegment(risse_int64 start, risse_int64 length)
		{ Start = start; Length = FilteredLength = length; }
	tRisaWaveSegment(risse_int64 start, risse_int64 length, risse_int64 filteredlength)
		{ Start = start; Length = length; FilteredLength = filteredlength; }
	risse_int64 Start; //!< オリジナルデコーダ上でのセグメントのスタート位置 (PCM サンプルグラニュール数単位)
	risse_int64 Length; //!< オリジナルデコーダ上でのセグメントの長さ (PCM サンプルグラニュール数単位)
	risse_int64 FilteredLength; //!< フィルタ後の長さ (PCM サンプルグラニュール数単位)
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief 再生イベント情報
//---------------------------------------------------------------------------
struct tRisaWaveEvent
{
	//! @brief コンストラクタ
	risse_int64 Position; //!< オリジナルデコーダ上でのラベル位置 (PCM サンプルグラニュール数単位)
	ttstr Name; //!< イベント名
	risse_int Offset;
		/*!< オフセット
			@note
			This member will be set in tRisaWaveLoopManager::Render,
			and will contain the sample granule offset from first decoding
			point at call of tRisaWaveLoopManager::Render().
		*/
	//! @brief コンストラクタ
	tRisaWaveEvent()
	{
		Position = 0;
		Offset = 0;
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief Waveのセグメント・イベントのキューを管理するクラス
//---------------------------------------------------------------------------
class tRisaWaveSegmentQueue
{
	// deque による Segments と Events の配列。
	// 実用上は、これらの配列に大量のデータが入ることはまずないので
	// vector で十分なのかもしれないが ...
	std::deque<tRisaWaveSegment> Segments; //!< セグメントの配列
	std::deque<tRisaWaveEvent> Events; //!< イベントの配列

public:
	//! @brief		内容をクリアする
	void Clear();

	//! @brief		tRisaWaveSegmentQueueをエンキューする
	//! @param		queue		エンキューしたいtRisaWaveSegmentQueueオブジェクト
	void Enqueue(const tRisaWaveSegmentQueue & queue);

	//! @brief		tRisaWaveSegmentをエンキューする
	//! @param		queue		エンキューしたいtRisaWaveSegmentオブジェクト
	void Enqueue(const tRisaWaveSegment & segment);

	//! @brief		tRisaWaveEventをエンキューする
	//! @param		queue		エンキューしたいtRisaWaveEventオブジェクト
	//! @note		Offset は修正されないので注意
	void Enqueue(const tRisaWaveEvent & event);

	//! @brief		tRisaWaveSegmentの配列をエンキューする
	//! @param		queue		エンキューしたい std::dequeue<tRisaWaveSegment>オブジェクト
	void Enqueue(const std::deque<tRisaWaveSegment> & segments);

	//! @brief		tRisaWaveEventの配列をエンキューする
	//! @param		queue		エンキューしたい std::dequeue<tRisaWaveEvent>オブジェクト
	void Enqueue(const std::deque<tRisaWaveEvent> & events);

	//! @brief		先頭から指定長さ分をデキューする
	//! @param		dest		格納先キュー(内容はクリアされる)
	//! @param		length		切り出す長さ(サンプルグラニュール単位)
	void Dequeue(tRisaWaveSegmentQueue & dest, risse_int64 length);

	//! @brief		このキューの全体の長さを得る
	//! @return		このキューの長さ (サンプルグラニュール単位)
	risse_int64 GetFilteredLength() const;

	//! @brief		このキューの長さを変化させる
	//! @param		new_total_filtered_length 新しいキューの長さ (サンプルグラニュール単位)
	//! @note		キュー中のSegments などの長さや Eventsの位置は線形補間される
	void Scale(risse_int64 new_total_length);

	//! @brief		フィルタされた位置からデコード位置へ変換を行う
	//! @param		pos フィルタされた位置
	//! @note		デコード位置
	risse_int64 FilteredPositionToDecodePosition(risse_int64 pos) const;

	//! @brief		内容を標準エラー出力にダンプする(デバッグ用)
	void Dump() const;
};
//---------------------------------------------------------------------------

#endif

