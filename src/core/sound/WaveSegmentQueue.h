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
	void Clear();
	void Enqueue(const tRisaWaveSegmentQueue & queue);
	void Enqueue(const tRisaWaveSegment & segment);
	void Enqueue(const tRisaWaveEvent & event);
	void Enqueue(const std::dequeue<tRisaWaveSegment> & segments);
	void Enqueue(const std::dequeue<tRisaWaveEvent> & events);
	void Dequeue(tRisaWaveSegmentQueue & dest, risse_int64 length);
	risse_int64 GetLength() const;
	void Scale(risse_int64 new_total_length);
};
//---------------------------------------------------------------------------


