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
#include "prec.h"
#include "WaveSegment.h"


RISSE_DEFINE_SOURCE_ID(41279,59678,37773,16849,57737,10358,8969,57779);


//---------------------------------------------------------------------------
//! @brief		内容をクリアする
//---------------------------------------------------------------------------
void tRisaWaveSegmentQueue::Clear()
{
	Events.clear();
	Segments.clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tRisaWaveSegmentQueueをエンキューする
//! @param		queue		エンキューしたいtRisaWaveSegmentQueueオブジェクト
//---------------------------------------------------------------------------
void tRisaWaveSegmentQueue::Enqueue(const tRisaWaveSegmentQueue & queue)
{
	Enqueue(queue.Events); // events をエンキュー(こっちを先にしないとだめ)
	Enqueue(queue.Segments); // segments をキュー(こっちは後)
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tRisaWaveSegmentをエンキューする
//! @param		queue		エンキューしたいtRisaWaveSegmentオブジェクト
//---------------------------------------------------------------------------
void tRisaWaveSegmentQueue::Enqueue(const tRisaWaveSegment & segment)
{
	Segments.push_back(segment);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tRisaWaveEventをエンキューする
//! @note		Offset は修正されないので注意
//! @param		queue		エンキューしたいtRisaWaveEventオブジェクト
//---------------------------------------------------------------------------
void tRisaWaveSegmentQueue::Enqueue(const tRisaWaveEvent & event)
{
	Events.push_back(event);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tRisaWaveSegmentの配列をエンキューする
//! @param		queue		エンキューしたい std::dequeue<tRisaWaveSegment>オブジェクト
//---------------------------------------------------------------------------
void Enqueue(const std::dequeue<tRisaWaveSegment> & segments)
{
	// segment の追加
	for(std::deque<tRisaWaveSegment>::iterator i = segment.begin();
		i != segment.end(); i++)
		Enqueue(*i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tRisaWaveEventの配列をエンキューする
//! @param		queue		エンキューしたい std::dequeue<tRisaWaveEvent>オブジェクト
//---------------------------------------------------------------------------
void Enqueue(const std::dequeue<tRisaWaveEvent> & events)
{
	// オフセットに加算する値を得る
	risse_int64 event_offset = GetLength();

	// event の追加
	for(std::deque<tRisaWaveEvent>::iterator i = event.begin();
		i != event.end(); i++)
	{
		tRisaWaveEvent one_event(*i);
		one_event.Offset += event_offset; // offset の修正
		Enqueue(one_event);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		先頭から指定長さ分をデキューする
//! @param		dest		格納先キュー(内容はクリアされる)
//! @param		length		切り出す長さ(サンプルグラニュール単位)
//---------------------------------------------------------------------------
void tRisaWaveSegmentQueue::Dequeue(tRisaWaveSegmentQueue & dest, risse_int64 length)
{
	risse_int64 remain;
	// dest をクリア
	dest.Clear();

	// Segments を切り出す
	remain = length;
	while(Segments.size() > 0 && remain > 0)
	{
		if(Segments.front().FilteredLength <= remain)
		{
			// i->FilteredLength が remain 以下
			// → この要素を dest にエンキューして this から削除
			remain -= Segments.front().FilteredLength;
			dest.Enqueue(Segments.front());
			Segments.pop_front();
		}
		else
		{
			// i->FilteredLength が remain よりも大きい
			// → 要素を途中でぶったぎって dest にエンキュー
			// FilteredLength を元に切り出しを行ってるので
			// Length は 線形補間を行う
			risse_int64 newlength =
				static_cast<risse_int64>(
					(double)i->Length / (double)i->FilteredLength * remain);
			if(newlength > 0)
				dest.Enqueue(tRisaWaveSegment(i1, newlength, remain));

			// i の Length と FilteredLength を修正
			i->Length -= remain;
			i->FilteredLength -= newlength;
			if(i->Length == 0 || i->FilteredLength == 0)
			{
				// ぶった切った結果 (線形補完した結果の誤差で)
				// 長さが0になってしまった
				Segments.pop_front(); // セグメントを捨てる
			}
			remain = 0; // ループを抜ける
		}
	}

	// Events を切り出す
	size_t events_to_dequeue = 0;
	for(std::deque<tRisaWaveEvent>::iterator i = Events.begin();
		i != Events.end(); i++)
	{
		risse_int64 newoffset = i->Offset - length;
		if(newoffset < 0)
		{
			// newoffset が負 なので dest に入れる
			dest.Enqueue(*i);
			events_to_dequeue ++; // あとで dequeue
		}
		else
		{
			// *i のオフセットを修正
			i->Offset = newoffset;
		}
	}

	while(events_to_dequeue--) Events.pop_front(); // コピーしたEvents を削除
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		このキューの全体の長さを得る
//! @return		このキューの長さ (サンプルグラニュール単位)
//---------------------------------------------------------------------------
risse_int64 tRisaWaveSegmentQueue::GetLength() const
{
	// キューの長さは すべての Segments のFilteredLengthの合計
	risse_int64 length = 0;
	for(std::deque<tRisaWaveSegment>::iterator i = Segments.begin();
		i != Segments.end(); i++)
		length += i->FilteredLength;

	return length;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		このキューの長さを変化させる
//! @param		new_total_length 新しいキューの長さ (サンプルグラニュール単位)
//! @note		キュー中のSegments などの長さや Eventsの位置は線形補間される
//---------------------------------------------------------------------------
void tRisaWaveSegmentQueue::Scale(risse_int64 new_total_length)
{
	// キューの FilteredLength を変化させる
	risse_int64 total_length_was = GetLength(); // 変化前の長さ

	if(total_length_was == 0) return 0; // 元の長さがないのでスケール出来ない

	// Segments の修正
	risse_int64 offset_was = 0; // 変化前のオフセット
	risse_int64 offset_is = 0; // 変化後のオフセット

	for(std::deque<tRisaWaveSegment>::iterator i = Segments.begin();
		i != Segments.end(); i++)
	{
		risse_int64 old_end = offset_was + i->FilteredLength;
		offset_was += i->FilteredLength;

		// old_end は全体から見てどの位置にある？
		double ratio = static_cast<double>(old_end) /
						static_cast<double>(total_length_was);

		// 新しい old_end はどの位置にあるべき？
		risse_int64 new_end = static_cast<risse_int64>(ratio * new_total_length);

		// FilteredLength の修正
		i->FilteredLength = new_end - offset_is;

		offset_is += i->FilteredLength;
	}

	// からっぽのSegments の除去
	for(std::deque<tRisaWaveSegment>::iterator i = Segments.begin();
		i != Segments.end() ; )
	{
		if(i->FilteredLength == 0 || i->Length == 0)
			i = Segments.erase(i);
		else
			i++;
	}

	// Events の修正
	double ratio = (double)new_total_length / (double)total_length_was;
	for(std::deque<tRisaWaveEvent>::iterator i = Events.begin();
		i != Events.end(); i++)
	{
		i->Offset = static_cast<risse_int64>(i->Offset * ratio);
	}
}
//---------------------------------------------------------------------------


