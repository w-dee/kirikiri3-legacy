//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveループマネージャ
//---------------------------------------------------------------------------
/*
	This module will be shared between Risa and
	Loop Tuner 2 (a GUI loop-point editor)
*/

#include "prec.h"
#include <algorithm>
#include "WaveLoopManager.h"

RISSE_DEFINE_SOURCE_ID(2302);


#ifdef RISA__IN_LOOP_TUNER
	#include "WaveReader.h"
#else
	#include "WaveDecoder.h"
#endif

#ifndef RISSE_HOST_IS_BIG_ENDIAN
	#define RISSE_HOST_IS_BIG_ENDIAN 0
#endif

//---------------------------------------------------------------------------
// PCMサンプル型の定義
//---------------------------------------------------------------------------
#ifdef __WIN32__
	// for assembler compatibility
	#pragma pack(push,1)
#endif
//! @brief  8bit PCM型
struct tRisaPCM8
{
	risse_uint8 value;
	tRisaPCM8(risse_int v) { value = (risse_uint8)(v + 0x80); }
	void operator = (risse_int v) { value = (risse_uint8)(v + 0x80); }
	operator risse_int () const { return (risse_int)value - 0x80; }
};
//! @brief  24bit PCM型
struct tRisaPCM24
{
	risse_uint8 value[3];
	tRisaPCM24(risse_int v)
	{
		operator = (v);
	}
	void operator =(risse_int v)
	{
#if RISSE_HOST_IS_BIG_ENDIAN
		value[0] = (v & 0xff0000) >> 16;
		value[1] = (v & 0x00ff00) >> 8;
		value[2] = (v & 0x0000ff);
#else
		value[0] = (v & 0x0000ff);
		value[1] = (v & 0x00ff00) >> 8;
		value[2] = (v & 0xff0000) >> 16;
#endif
	}
	operator risse_int () const
	{
		risse_int t;
#if RISSE_HOST_IS_BIG_ENDIAN
		t = ((risse_int)value[0] << 16) + ((risse_int)value[1] << 8) + ((risse_int)value[2]);
#else
		t = ((risse_int)value[2] << 16) + ((risse_int)value[1] << 8) + ((risse_int)value[0]);
#endif
		t |= -(t&0x800000); // extend sign
		return t;
	}
};
#ifdef __WIN32__
	#pragma pack(pop)
#endif
/*
	16bit型と32bit型はそれぞれrisse_int16とrisse_int32のプリミティブ型を用いる
*/
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		クロスフェードを行うためのテンプレート関数
//! @param		dest		結果格納先
//! @param		src1		ソース1 (消えていく方)
//! @param		src2		ソース2 (入ってくる方)
//! @param		ratiostart	ブレンド率の開始値(%)
//! @param		ratioend	ブレンド率の終了値(%)
//! @param		samples		この関数の呼び出しで処理すべきサンプル数
//! @param		channels	対象PCMのチャンネル数
//---------------------------------------------------------------------------
template <typename T>
static void RisaCrossFadeIntegerBlend(void *dest, void *src1, void *src2,
	risse_int ratiostart, risse_int ratioend,
	risse_int samples, risse_int channels)
{
	risse_uint blend_step = (risse_int)(
		(
			(ratioend - ratiostart) * ((risse_int64)1<<32) / 100
		) / samples);
	const T *s1 = (const T *)src1;
	const T *s2 = (const T *)src2;
	T *out = (T *)dest;
	risse_uint ratio = (risse_int)(ratiostart * ((risse_int64)1<<32) / 100);
	for(risse_int i = 0; i < samples; i++)
	{
		for(risse_int j = channels - 1; j >= 0; j--)
		{
			risse_int si1 = (risse_int)*s1;
			risse_int si2 = (risse_int)*s2;
			risse_int o = (risse_int) (
						(((risse_int64)si2 * (risse_uint64)ratio) >> 32) +
						(((risse_int64)si1 * (RISSE_UI64_VAL(0x100000000) - (risse_uint64)ratio) ) >> 32) );
			*out = o;
			s1 ++;
			s2 ++;
			out ++;
		}
		ratio += blend_step;
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaWaveLoopManager::tRisaWaveLoopManager()
{
	Position = 0;
	IsLinksSorted = false;
	IsLabelsSorted = false;
	CrossFadeSamples = NULL;
	CrossFadeLen = 0;
	CrossFadePosition = 0;
	Decoder = NULL;
	IgnoreLinks = false;
	Looping = false;
	Format = new tRisaWaveFormat;
	memset(Format, 0, sizeof(*Format));

	ClearFlags();
	FlagsModifiedByLabelExpression = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaWaveLoopManager::~tRisaWaveLoopManager()
{
	ClearCrossFadeInformation();
	delete Format;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デコーダを設定する
//! @param		decoder		デコーダ
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::SetDecoder(tRisaWaveDecoder * decoder)
{
	// set decoder and compute ShortCrossFadeHalfSamples
	Decoder = decoder;
	if(decoder)
		decoder->GetFormat(*Format);
	else
		memset(Format, 0, sizeof(*Format));
	ShortCrossFadeHalfSamples =
		Format->Frequency * SmoothTimeHalf / 1000;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定インデックスのフラグを得る
//! @param		index    フラグのインデックス
//! @return		そのインデックスに対応するフラグの値
//---------------------------------------------------------------------------
int tRisaWaveLoopManager::GetFlag(risse_int index)
{
	volatile tRisseCriticalSectionHolder CS(FlagsCS);
	return Flags[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		フラグをすべて一気にコピーする
//! @param		dest		コピー先
//! @note		この関数を呼ぶと FlagsModifiedByLabelExpression は偽にリセットされる
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::CopyFlags(risse_int *dest)
{
	volatile tRisseCriticalSectionHolder CS(FlagsCS);
	// copy flags into dest, and clear FlagsModifiedByLabelExpression
	memcpy(dest, Flags, sizeof(Flags));
	FlagsModifiedByLabelExpression = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		フラグがラベル式によって変更されたかどうかが返される
//! @return		フラグがラベル式によって変更されたかどうか
//! @note		このフラグは CopyFlags により偽にリセットされる
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetFlagsModifiedByLabelExpression()
{
	volatile tRisseCriticalSectionHolder CS(FlagsCS);
	return FlagsModifiedByLabelExpression;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定インデックスのフラグを設定する
//! @param		index		フラグのインデックス
//! @param		f			フラグの値
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::SetFlag(risse_int index, risse_int f)
{
	volatile tRisseCriticalSectionHolder CS(FlagsCS);
	if(f < 0) f = 0;
	if(f > MaxFlagValue) f = MaxFlagValue;
	Flags[index] = f;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		フラグをすべて 0 にする
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::ClearFlags()
{
	volatile tRisseCriticalSectionHolder CS(FlagsCS);
	for(risse_int i = 0; i < MaxFlags; i++) Flags[i] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		リンクやラベルをすべてクリアする
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::ClearLinksAndLabels()
{
	// clear links and labels
	volatile tRisseCriticalSectionHolder CS(FlagsCS);
	Labels.clear();
	Links.clear();
	IsLinksSorted = false;
	IsLabelsSorted = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		リンクの配列を得る
//! @return		リンクの配列への参照
//---------------------------------------------------------------------------
const std::vector<tRisaWaveLoopLink> & tRisaWaveLoopManager::GetLinks() const
{
	volatile tRisseCriticalSectionHolder
		CS(const_cast<tRisaWaveLoopManager*>(this)->FlagsCS);
	return Links;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ラベルの配列を得る
//! @return		ラベルの配列への参照
//---------------------------------------------------------------------------
const std::vector<tRisaWaveLabel> & tRisaWaveLoopManager::GetLabels() const
{
	volatile tRisseCriticalSectionHolder
		CS(const_cast<tRisaWaveLoopManager*>(this)->FlagsCS);
	return Labels;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		リンクの配列を設定する
//! @param		links		設定したい配列
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::SetLinks(const std::vector<tRisaWaveLoopLink> & links)
{
	volatile tRisseCriticalSectionHolder CS(FlagsCS);
	Links = links;
	IsLinksSorted = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ラベルの配列を設定する
//! @param		links		設定したい配列
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::SetLabels(const std::vector<tRisaWaveLabel> & labels)
{
	volatile tRisseCriticalSectionHolder CS(FlagsCS);
	Labels = labels;
	IsLabelsSorted = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		リンクを無視しながら再生しているかどうかを返す
//! @return		リンクを無視しながら再生しているかどうか
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetIgnoreLinks() const
{
	volatile tRisseCriticalSectionHolder
		CS(const_cast<tRisaWaveLoopManager*>(this)->DataCS);
	return IgnoreLinks;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		リンクを無視しながら再生しているかどうかを設定する
//! @param		b		リンクを蒸ししながら再生するかどうか
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::SetIgnoreLinks(bool b)
{
	volatile tRisseCriticalSectionHolder CS(DataCS);
	IgnoreLinks = b;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		現在のデコード位置を得る
//! @return		現在のデコード位置
//---------------------------------------------------------------------------
risse_int64 tRisaWaveLoopManager::GetPosition() const
{
	// we cannot assume that the 64bit data access is truely atomic on 32bit machines.
	volatile tRisseCriticalSectionHolder
		CS(const_cast<tRisaWaveLoopManager*>(this)->FlagsCS);
	return Position;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		現在のデコード位置を設定する
//! @param		pos		現在のデコード位置
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::SetPosition(risse_int64 pos)
{
	volatile tRisseCriticalSectionHolder CS(DataCS);
	Position = pos;
	ClearCrossFadeInformation();
	Decoder->SetPosition(pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デコードを行う
//! @param		dest		デコード結果を格納するバッファ
//! @param		samples		デコードを行いたいサンプル数
//! @param		written		実際にデコード出来たサンプル数
//! @param		segments	再生セグメント情報を書き込む配列
//! @param		labels		通過ラベル情報を書き込む配列
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::Decode(void *dest, risse_uint samples, risse_uint &written,
		std::vector<tRisaWaveLoopManager::tSegment> &segments,
		std::vector<tRisaWaveLabel> &labels)
{
	// decode from current position
	volatile tRisseCriticalSectionHolder CS(DataCS);

	segments.clear();
	labels.clear();
	written = 0;
	risse_uint8 *d = (risse_uint8*)dest;

	risse_int give_up_count = 0;

	while(written != samples/* && Position < Format->TotalSamples*/)
	{
		// decide next operation
		risse_int64 next_event_pos;
		bool next_not_found = false;
		risse_int before_count;

		// check nearest link
		tRisaWaveLoopLink link;
		if(!IgnoreLinks && GetNearestEvent(Position, link, false))
		{
			// nearest event found ...
			if(link.From == Position)
			{
				// do jump
				give_up_count ++;
				if(give_up_count >= RisaWaveLoopLinkGiveUpCount)
					break; // give up decoding

				Position = link.To;
				if(!CrossFadeSamples)
					Decoder->SetPosition(Position);
				continue;
			}

			if(link.Smooth)
			{
				// the nearest event is a smooth link
				// bofore_count is sample count before 50%-50% blend
				// after_count  is sample count after  50%-50% blend
				before_count = ShortCrossFadeHalfSamples;
				// adjust before count
				if(link.From - before_count < 0)
					before_count = (risse_int)link.From;
				if(link.To - before_count < 0)
					before_count = (risse_int)link.To;
				if(link.From - before_count > Position)
				{
					// Starting crossfade is the nearest next event,
					// but some samples must be decoded before the event comes.
					next_event_pos = link.From - before_count;
				}
				else if(!CrossFadeSamples)
				{
					// just position to start crossfade
					// or crossfade must already start
					next_event_pos = link.From;
					// adjust before_count
					before_count = link.From - Position;
					// adjust after count
					risse_int after_count = ShortCrossFadeHalfSamples;
					if(Format->TotalSampleGranules - link.From < static_cast<risse_uint64>(after_count))
						after_count =
							(risse_int)(Format->TotalSampleGranules - link.From);
					if(Format->TotalSampleGranules - link.To < static_cast<risse_uint64>(after_count))
						after_count =
							(risse_int)(Format->TotalSampleGranules - link.To);
					tRisaWaveLoopLink over_to_link;
					if(GetNearestEvent(link.To, over_to_link, true))
					{
						if(over_to_link.From - link.To < after_count)
							after_count =
								(risse_int)(over_to_link.From - link.To);
					}
					// prepare crossfade
					// allocate memory
					risse_uint8 *src1 = NULL;
					risse_uint8 *src2 = NULL;
					try
					{
						risse_int alloc_size =
							(before_count + after_count) * 
								Format->BytesPerSample * Format->Channels;
						CrossFadeSamples = new risse_uint8[alloc_size];
						src1 = new risse_uint8[alloc_size];
						src2 = new risse_uint8[alloc_size];
					}
					catch(...)
					{
						// memory allocation failed. perform normal link.
						if(CrossFadeSamples)
							delete [] CrossFadeSamples,
								CrossFadeSamples = NULL;
						if(src1) delete [] src1;
						if(src2) delete [] src2;
						next_event_pos = link.From;
					}
					if(CrossFadeSamples)
					{
						// decode samples
						risse_uint decoded1 = 0, decoded2 = 0;

						Decoder->Render((void*)src1,
							before_count + after_count, decoded1);

						Decoder->SetPosition(
							link.To - before_count);

						Decoder->Render((void*)src2,
							before_count + after_count, decoded2);

						// perform crossfade
						risse_int after_offset =
							before_count * Format->BytesPerSample * Format->Channels;
						DoCrossFade(CrossFadeSamples,
							src1, src2, before_count, 0, 50);
						DoCrossFade(CrossFadeSamples + after_offset,
							src1 + after_offset, src2 + after_offset,
								after_count, 50, 100);
						delete [] src1;
						delete [] src2;
						// reset CrossFadePosition and CrossFadeLen
						CrossFadePosition = 0;
						CrossFadeLen = before_count + after_count;
					}
				}
				else
				{
					next_event_pos = link.From;
				}
			}
			else
			{
				// normal jump
				next_event_pos = link.From;
			}
		}
		else
		{
			// event not found
			next_not_found = true;
		}

		risse_int one_unit;

		if(next_not_found || next_event_pos - Position > (samples - written))
			one_unit = samples - written;
		else
			one_unit = (risse_int) (next_event_pos - Position);

		if(CrossFadeSamples)
		{
			if(one_unit > CrossFadeLen - CrossFadePosition)
				one_unit = CrossFadeLen - CrossFadePosition;
		}
		segments.push_back(tSegment(Position, one_unit));

		if(one_unit > 0) give_up_count = 0; // reset give up count

		// evaluate each label
		risse_uint label_base = labels.size();
		GetLabelAt(Position, Position + one_unit, labels);
		for(std::vector<tRisaWaveLabel>::iterator i = labels.begin() + label_base;
			i != labels.end(); i++)
		{
			if(i->Name.c_str()[0] == ':')
			{
				// for each label
				EvalLabelExpression(i->Name);
			}
		}

		// calculate each label offset
		for(std::vector<tRisaWaveLabel>::iterator i = labels.begin() + label_base;
			i != labels.end(); i++)
			i->Offset = (risse_int)(i->Position - Position) + written;

		// decode or copy
		if(!CrossFadeSamples)
		{
			// not crossfade
			// decode direct into destination buffer
			risse_uint decoded;
			Decoder->Render((void *)d, one_unit, decoded);
			Position += decoded;
			written += decoded;
			if(decoded != (risse_uint)one_unit)
			{
				// must be the end of the decode
				if(!Looping) break; // end decoding
				// rewind and continue
				if(Position == 0)
				{
					// already rewinded; must be an error
					break;
				}
				Position = 0;
				Decoder->SetPosition(0);
			}
			d += decoded * Format->BytesPerSample * Format->Channels;
		}
		else
		{
			// in cross fade
			// copy prepared samples
			memcpy((void *)d,
				CrossFadeSamples +
					CrossFadePosition * Format->BytesPerSample * Format->Channels,
				one_unit * Format->BytesPerSample * Format->Channels);
			CrossFadePosition += one_unit;
			Position += one_unit;
			written += one_unit;
			d += one_unit * Format->BytesPerSample * Format->Channels;
			if(CrossFadePosition == CrossFadeLen)
			{
				// crossfade has finished
				ClearCrossFadeInformation();
			}
		}
	}	// while 
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定位置以降で、指定位置にもっとも近いリンクの始点を探す
//! @param		current		検索開始位置
//! @param		link		見つかったリンクを書き込む先
//! @param		ignore_conditions	リンク条件を無視して検索を行うかどうか
//! @return		リンクが見つかれば真、見つからなければ偽
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetNearestEvent(risse_int64 current,
		tRisaWaveLoopLink & link, bool ignore_conditions)
{
	// search nearest event in future, from current.
	// this checks conditions unless ignore_conditions is true.
	volatile tRisseCriticalSectionHolder CS(FlagsCS);

	if(Links.size() == 0) return false; // there are no event

	if(!IsLinksSorted)
	{
		std::sort(Links.begin(), Links.end());
		IsLinksSorted = true;
	}

	// search nearest next event using binary search
	risse_int s = 0, e = Links.size();
	while(e - s > 1)
	{
		risse_int m = (s+e)/2;
		if(Links[m].From <= current)
			s = m;
		else
			e = m;
	}

	if(s < (int)Links.size()-1 && Links[s].From < current) s++;

	if((risse_uint)s >= Links.size() || Links[s].From < current)
	{
		// no links available
		return false;
	}

	// rewind while the link 'from' is the same
	risse_int64 from = Links[s].From;
	while(true)
	{
		if(s >= 1 && Links[s-1].From == from)
			s--;
		else
			break;
	}

	// check conditions
	if(!ignore_conditions)
	{
		do
		{
			// check condition
			if(Links[s].CondVar != -1)
			{
				bool match = false;
				switch(Links[s].Condition)
				{
				case tRisaWaveLoopLink::llcNone:
					match = true; break;
				case tRisaWaveLoopLink::llcEqual:
					if(Links[s].RefValue == Flags[Links[s].CondVar]) match = true;
					break;
				case tRisaWaveLoopLink::llcNotEqual:
					if(Links[s].RefValue != Flags[Links[s].CondVar]) match = true;
					break;
				case tRisaWaveLoopLink::llcGreater:
					if(Links[s].RefValue <  Flags[Links[s].CondVar]) match = true;
					break;
				case tRisaWaveLoopLink::llcGreaterOrEqual:
					if(Links[s].RefValue <= Flags[Links[s].CondVar]) match = true;
					break;
				case tRisaWaveLoopLink::llcLesser:
					if(Links[s].RefValue >  Flags[Links[s].CondVar]) match = true;
					break;
				case tRisaWaveLoopLink::llcLesserOrEqual:
					if(Links[s].RefValue >= Flags[Links[s].CondVar]) match = true;
					break;
				default:
					match = false;
				}
				if(match) break; // condition matched
			}
			else
			{
				break;
			}
			s++;
		} while((risse_uint)s < Links.size());

		if((risse_uint)s >= Links.size() || Links[s].From < current)
		{
			// no links available
			return false;
		}
	}

	link = Links[s];

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定位置以降で指定位置未満の中のラベルを取得する
//! @param		from		検索開始位置
//! @param		to			検索終了位置
//! @param		labels		結果を格納する配列
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::GetLabelAt(risse_int64 from, risse_int64 to,
		std::vector<tRisaWaveLabel> & labels)
{
	volatile tRisseCriticalSectionHolder CS(FlagsCS);

	if(Labels.size() == 0) return; // no labels found
	if(!IsLabelsSorted)
	{
		std::sort(Labels.begin(), Labels.end());
		IsLabelsSorted = true;
	}

	// search nearest label using binary search
	risse_int s = 0, e = Labels.size();
	while(e - s > 1)
	{
		risse_int m = (s+e)/2;
		if(Labels[m].Position <= from)
			s = m;
		else
			e = m;
	}

	if(s < (int)Labels.size()-1 && Labels[s].Position < from) s++;

	if((risse_uint)s >= Labels.size() || Labels[s].Position < from)
	{
		// no labels available
		return;
	}

	// rewind while the label position is the same
	risse_int64 pos = Labels[s].Position;
	while(true)
	{
		if(s >= 1 && Labels[s-1].Position == pos)
			s--;
		else
			break;
	}

	// search labels
	for(; s < (int)Labels.size(); s++)
	{
		if(Labels[s].Position >= from && Labels[s].Position < to)
			labels.push_back(Labels[s]);
		else
			break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		クロスフェードを行う
//! @param		dest		結果格納先
//! @param		src1		ソース1 (消えていく方)
//! @param		src2		ソース2 (入ってくる方)
//! @param		ratiostart	ブレンド率の開始値(%)
//! @param		ratioend	ブレンド率の終了値(%)
//! @param		samples		この関数の呼び出しで処理すべきサンプル数
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::DoCrossFade(void *dest, void *src1,
	void *src2, risse_int samples, risse_int ratiostart, risse_int ratioend)
{
	// do on-memory wave crossfade
	// using src1 (fading out) and src2 (fading in).
	if(samples == 0) return; // nothing to do

	if(Format->IsFloat)
	{
		float blend_step =
			(float)((ratioend - ratiostart) / 100.0 / samples);
		const float *s1 = (const float *)src1;
		const float *s2 = (const float *)src2;
		float *out = (float *)dest;
		float ratio = ratiostart / 100.0;
		for(risse_int i = 0; i < samples; i++)
		{
			for(risse_int j = Format->Channels - 1; j >= 0; j--)
			{
				*out = *s1 + (*s2 - *s1) * ratio;
				s1 ++;
				s2 ++;
				out ++;
			}
			ratio += blend_step;
		}
	}
	else
	{
		if(Format->BytesPerSample == 1)
		{
			RisaCrossFadeIntegerBlend<tRisaPCM8>(dest, src1, src2,
				ratiostart, ratioend, samples, Format->Channels);
		}
		else if(Format->BytesPerSample == 2)
		{
			RisaCrossFadeIntegerBlend<risse_int16>(dest, src1, src2,
				ratiostart, ratioend, samples, Format->Channels);
		}
		else if(Format->BytesPerSample == 3)
		{
			RisaCrossFadeIntegerBlend<tRisaPCM24>(dest, src1, src2,
				ratiostart, ratioend, samples, Format->Channels);
		}
		else if(Format->BytesPerSample == 4)
		{
			RisaCrossFadeIntegerBlend<risse_int32>(dest, src1, src2,
				ratiostart, ratioend, samples, Format->Channels);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		内部にキャッシュされているクロスフェードの情報をクリアする
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::ClearCrossFadeInformation()
{
	if(CrossFadeSamples) delete [] CrossFadeSamples, CrossFadeSamples = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ラベル式を解析する
//! @param		label		ラベル式
//! @param		ope			演算子
//! @param		lv			左辺値
//! @param		rv			右辺値
//! @param		is_rv_indirect	右辺値が間接参照の場合は真、そうでない場合は偽が格納される
//! @param		解析に成功すれば真、式が間違っているなどで失敗したら偽
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetLabelExpression(const tRisaLabelStringType &label,
	tRisaWaveLoopManager::tExpressionToken * ope,
	risse_int *lv,
	risse_int *rv, bool *is_rv_indirect)
{
	const tRisaLabelCharType * p = label.c_str();
	tExpressionToken token;
	tExpressionToken operation;
	risse_int value  = 0;
	risse_int lvalue = 0;
	risse_int rvalue = 0;
	bool rv_indirect = false;

	if(*p != ':') return false; // not expression
	p++;

	token = GetExpressionToken(p, &value);
	if(token != etLBracket) return false; // lvalue must form of '[' integer ']'
	token = GetExpressionToken(p, &value);
	if(token != etInteger) return false; // lvalue must form of '[' integer ']'
	lvalue = value;
	if(lvalue < 0 || lvalue >= MaxFlags) return false; // out of the range
	token = GetExpressionToken(p, &value);
	if(token != etRBracket) return false; // lvalue must form of '[' integer ']'

	token = GetExpressionToken(p, &value);
	switch(token)
	{
	case etEqual:
	case etPlusEqual:  case etIncrement:
	case etMinusEqual: case etDecrement:
		break;
	default:
		return false; // unknown operation
	}
	operation = token;

	token = GetExpressionToken(p, &value);
	if(token == etLBracket)
	{
		// indirect value
		token = GetExpressionToken(p, &value);
		if(token != etInteger) return false; // rvalue does not have form of '[' integer ']'
		rvalue = value;
		if(rvalue < 0 || rvalue >= MaxFlags) return false; // out of the range
		token = GetExpressionToken(p, &value);
		if(token != etRBracket) return false; // rvalue does not have form of '[' integer ']'
		rv_indirect = true;
	}
	else if(token == etInteger)
	{
		// direct value
		rv_indirect = false;
		rvalue = value;
	}
	else if(token == etEOE)
	{
		if(!(operation == etIncrement || operation == etDecrement))
			return false; // increment or decrement cannot have operand
	}
	else
	{
		return false; // syntax error
	}

	token = GetExpressionToken(p, &value);
	if(token != etEOE) return false; // excess characters

	if(ope) *ope = operation;
	if(lv)  *lv = lvalue;
	if(rv)  *rv = rvalue;
	if(is_rv_indirect) * is_rv_indirect = rv_indirect;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ラベル式を評価する
//! @param		label		ラベル
//! @return		評価に成功すれば真、失敗すれば偽
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::EvalLabelExpression(const tRisaLabelStringType &label)
{
	// eval expression specified by 'label'
	// commit the result when 'commit' is true.
	// returns whether the label syntax is correct.
	volatile tRisseCriticalSectionHolder CS(FlagsCS);

	tExpressionToken operation;
	risse_int lvalue;
	risse_int rvalue;
	bool is_rv_indirect;

	if(!GetLabelExpression(label, &operation, &lvalue, &rvalue, &is_rv_indirect)) return false;

	if(is_rv_indirect) rvalue = Flags[rvalue];

	switch(operation)
	{
	case etEqual:
		FlagsModifiedByLabelExpression = true;
		Flags[lvalue] = rvalue;
		break;
	case etPlusEqual:
		FlagsModifiedByLabelExpression = true;
		Flags[lvalue] += rvalue;
		break;
	case etMinusEqual:
		FlagsModifiedByLabelExpression = true;
		Flags[lvalue] -= rvalue;
		break;
	case etIncrement:
		FlagsModifiedByLabelExpression = true;
		Flags[lvalue] ++;
		break;
	case etDecrement:
		FlagsModifiedByLabelExpression = true;
		Flags[lvalue] --;
		break;
	default:
		;
		break; // nothing to do
	}

	if(Flags[lvalue] < 0) Flags[lvalue] = 0;
	if(Flags[lvalue] > MaxFlagValue) Flags[lvalue] = MaxFlagValue;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ラベル式のトークンを切り出して返す
//! @param		p		切り出し開始位置
//! @param		value	トークンの値を格納する変数へのアドレス
//! @return		トークンのタイプ
//---------------------------------------------------------------------------
tRisaWaveLoopManager::tExpressionToken
	tRisaWaveLoopManager::GetExpressionToken(const tRisaLabelCharType *  & p, risse_int * value)
{
	// get token at pointer 'p'

	while(*p && *p <= 0x20) p++; // skip spaces
	if(!*p) return etEOE;

	switch(*p)
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		// numbers
		if(value) GetLabelCharInt(p, *value);
		while(*p && *p >= '0' && *p <= '9') p++;
		return etInteger;

	case '[':
		p++;
		return etLBracket;
	case ']':
		p++;
		return etRBracket;

	case '=':
		p++;
		return etEqual;

	case '+':
		p++;
		if(*p == '=') { p++; return etPlusEqual; }
		if(*p == '+') { p++; return etIncrement; }
		return etPlus;
	case '-':
		p++;
		if(*p == '=') { p++; return etMinusEqual; }
		if(*p == '-') { p++; return etDecrement; }
		return etPlus;

	default:
		;
	}

	p++;
	return etUnknown;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ラベル式から整数値を得る
//! @param		s		解析開始位置
//! @param		v		値を格納する変数
//! @return		解析に成功すれば真
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetLabelCharInt(const tRisaLabelCharType *s, risse_int &v)
{
	// convert string to integer
	risse_int r = 0;
	bool sign = false;
	while(*s && *s <= 0x20) s++; // skip spaces
	if(!*s) return false;
	if(*s == '-')
	{
		sign = true;
		s++;
		while(*s && *s <= 0x20) s++; // skip spaces
		if(!*s) return false;
	}

	while(*s >= '0' && *s <= '9')
	{
		r *= 10;
		r += *s - '0';
		s++;
	}
	if(sign) r = -r;
	v = r;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列から整数値を得る
//! @param		s		解析開始位置
//! @param		v		値を格納する変数
//! @return		解析に成功すれば真
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetInt(char *s, risse_int &v)
{
	// convert string to integer
	risse_int r = 0;
	bool sign = false;
	while(*s && *s <= 0x20) s++; // skip spaces
	if(!*s) return false;
	if(*s == '-')
	{
		sign = true;
		s++;
		while(*s && *s <= 0x20) s++; // skip spaces
		if(!*s) return false;
	}

	while(*s >= '0' && *s <= '9')
	{
		r *= 10;
		r += *s - '0';
		s++;
	}
	if(sign) r = -r;
	v = r;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列から64bit整数値を得る
//! @param		s		解析開始位置
//! @param		v		値を格納する変数
//! @return		解析に成功すれば真
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetInt64(char *s, risse_int64 &v)
{
	// convert string to integer
	risse_int64 r = 0;
	bool sign = false;
	while(*s && *s <= 0x20) s++; // skip spaces
	if(!*s) return false;
	if(*s == '-')
	{
		sign = true;
		s++;
		while(*s && *s <= 0x20) s++; // skip spaces
		if(!*s) return false;
	}

	while(*s >= '0' && *s <= '9')
	{
		r *= 10;
		r += *s - '0';
		s++;
	}
	if(sign) r = -r;
	v = r;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列から真偽値を得る
//! @param		s		解析開始位置
//! @param		v		値を格納する変数
//! @return		解析に成功すれば真
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetBool(char *s, bool &v)
{
	// convert string to boolean
	if(!strcasecmp(s, "True"))		{	v = true;	return true;	}
	if(!strcasecmp(s, "False"))		{	v = false;	return true;	}
	if(!strcasecmp(s, "Yes"))		{	v = true;	return true;	}
	if(!strcasecmp(s, "No"))		{	v = false;	return true;	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列からリンク条件を得る
//! @param		s		解析開始位置
//! @param		v		値を格納する変数
//! @return		解析に成功すれば真
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetCondition(char *s, tRisaWaveLoopLink::tLinkCondition &v)
{
	// get condition value
	if(!strcasecmp(s, "no")) { v = tRisaWaveLoopLink::llcNone;				return true;	}
	if(!strcasecmp(s, "eq")) { v = tRisaWaveLoopLink::llcEqual;				return true;	}
	if(!strcasecmp(s, "ne")) { v = tRisaWaveLoopLink::llcNotEqual;			return true;	}
	if(!strcasecmp(s, "gt")) { v = tRisaWaveLoopLink::llcGreater;			return true;	}
	if(!strcasecmp(s, "ge")) { v = tRisaWaveLoopLink::llcGreaterOrEqual;	return true;	}
	if(!strcasecmp(s, "lt")) { v = tRisaWaveLoopLink::llcLesser;			return true;	}
	if(!strcasecmp(s, "le")) { v = tRisaWaveLoopLink::llcLesserOrEqual;		return true;	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列リテラルを取得する
//! @param		s		解析開始位置
//! @param		v		値を格納する変数
//! @return		解析に成功すれば真
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetString(char *s, tRisaLabelStringType &v)
{
	// convert utf-8 string s to v
#ifdef RISA__IN_LOOP_TUNER

	// compute output (unicode) size
	risse_int size = RisaUtf8ToWideCharString(s, NULL);
	if(size == -1) return false; // not able to convert the string

	// allocate output buffer
	risse_char *us = new risse_char[size + 1];
	try
	{
		RisaUtf8ToWideCharString(s, us);
		us[size] = RISSE_W('\0');

		// convert us (an array of wchar_t) to AnsiString
		v = AnsiString(us);

	}
	catch(...)
	{
		delete [] us;
		throw;
	}
	delete [] us;
	return true;
#else
	v = ttstr(wxString(s, wxConvUTF8));
	return true;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		name=value の形式になっている name と value を取得する
//! @param		p		解析開始位置
//! @param		name	name の部分の最初を表すポインタ
//! @param		value   value の部分の最初を表すポインタ
//! @return		解析に成功すれば真
//! @note		このメソッドは、name および value が終了する位置に \0 を書き込む。
//!				つまり、p を破壊する。
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::GetEntityToken(char * & p, char **name, char **value)
{
	// get name=value string at 'p'.
	// returns whether the token can be got or not.
	// on success, *id will point start of the name, and value will point
	// start of the value. the buffer given by 'start' will be destroied.

	char * namelast;
	char * valuelast;
	char delimiter = '\0';

	// skip preceeding white spaces
	while(isspace(*p)) p++;
	if(!*p) return false;

	// p will now be a name
	*name = p;

	// find white space or '='
	while(!isspace(*p) && *p != '=' && *p) p++;
	if(!*p) return false;

	namelast = p;

	// skip white space
	while(isspace(*p)) p++;
	if(!*p) return false;

	// is current pointer pointing '='  ?
	if(*p != '=') return false;

	// step pointer
	p ++;
	if(!*p) return false;

	// skip white space
	while(isspace(*p)) p++;
	if(!*p) return false;

	// find delimiter
	if(*p == '\'') delimiter = *p, p++;
	if(!*p) return false;

	// now p will be start of value
	*value = p;

	// find delimiter or white space or ';'
	if(delimiter == '\0')
	{
		while((!isspace(*p) && *p != ';') && *p) p++;
	}
	else
	{
		while((*p != delimiter) && *p) p++;
	}

	// remember value last point
	valuelast = p;

	// skip last delimiter
	if(*p == delimiter) p++;

	// put null terminator
	*namelast = '\0';
	*valuelast = '\0';

	// finish
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		リンク情報を文字列から読み取る
//! @param		p		読み取り開始位置
//! @param		link	情報格納先クラス
//! @return		読み取りに成功すれば真
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::ReadLinkInformation(char * & p, tRisaWaveLoopLink &link)
{
	// read link information from 'p'.
	// p must point '{' , which indicates start of the block.
	if(*p != '{') return false;

	p++;
	if(!*p) return false;

	do
	{
		// get one token from 'p'
		char * name;
		char * value;
		if(!GetEntityToken(p, &name, &value))
			return false;

		if(!strcasecmp(name, "From"))
		{	if(!GetInt64(value, link.From))					return false;}
		else if(!strcasecmp(name, "To"))
		{	if(!GetInt64(value, link.To))					return false;}
		else if(!strcasecmp(name, "Smooth"))
		{	if(!GetBool(value, link.Smooth))				return false;}
		else if(!strcasecmp(name, "Condition"))
		{	if(!GetCondition(value, link.Condition))		return false;}
		else if(!strcasecmp(name, "RefValue"))
		{	if(!GetInt(value, link.RefValue))				return false;}
		else if(!strcasecmp(name, "CondVar"))
		{	if(!GetInt(value, link.CondVar))				return false;}
		else
		{													return false;}

		// skip space
		while(isspace(*p)) p++;

		// check ';'. note that this will also be a null, if no delimiters are used
		if(*p != ';' && *p != '\0') return false;
		p++;
		if(!*p) return false;

		// skip space
		while(isspace(*p)) p++;
		if(!*p) return false;

		// check '}'
		if(*p == '}') break;
	} while(true);

	p++;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ラベル情報を文字列から読み取る
//! @param		p		読み取り開始位置
//! @param		link	情報格納先クラス
//! @return		読み取りに成功すれば真
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::ReadLabelInformation(char * & p, tRisaWaveLabel &label)
{
	// read label information from 'p'.
	// p must point '{' , which indicates start of the block.
	if(*p != '{') return false;

	p++;
	if(!*p) return false;

	do
	{
		// get one token from 'p'
		char * name;
		char * value;
		if(!GetEntityToken(p, &name, &value))
			return false;

		if(!strcasecmp(name, "Position"))
		{	if(!GetInt64(value, label.Position))			return false;}
		else if(!strcasecmp(name, "Name"))
		{	if(!GetString(value, label.Name))				return false;}
		else
		{													return false;}

		// skip space
		while(isspace(*p)) p++;

		// check ';'. note that this will also be a null, if no delimiters are used
		if(*p != ';' && *p != '\0') return false;
		p++;
		if(!*p) return false;

		// skip space
		while(isspace(*p)) p++;
		if(!*p) return false;

		// check '}'
		if(*p == '}') break;
	} while(true);

	p++;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		リンク情報やラベル情報を文字列から読み取る
//! @param		p		読み取り開始位置
//! @return		読み取りに成功すれば真
//---------------------------------------------------------------------------
bool tRisaWaveLoopManager::ReadInformation(char * p)
{
	// read information from 'p'
	volatile tRisseCriticalSectionHolder CS(FlagsCS);

	char *p_org = p;
	Links.clear();
	Labels.clear();

	// check version
	if(*p != '#')
	{
		// old sli format
		char *p_length = strstr(p, "LoopLength=");
		char *p_start  = strstr(p, "LoopStart=");
		if(!p_length || !p_start) return false; // read error
		tRisaWaveLoopLink link;
		link.Smooth = false;
		link.Condition = tRisaWaveLoopLink::llcNone;
		link.RefValue = 0;
		link.CondVar = 0;
		risse_int64 start;
		risse_int64 length;
		if(!GetInt64(p_length + 11, length)) return false;
		if(!GetInt64(p_start  + 10, start )) return false;
		link.From = start + length;
		link.To = start;
		Links.push_back(link);
	}
	else
	{
		// sli v2.0+
		if(strncmp(p, "#2.00", 5) > 0)
			return false; // version mismatch 

		while(true)
		{
			if((p == p_org || p[-1] == '\n') && *p == '#')
			{
				// line starts with '#' is a comment
				// skip the comment
				while(*p != '\n' && *p) p++;
				if(!*p) break;

				p ++;
				continue;
			}

			// skip white space
			while(isspace(*p)) p++;
			if(!*p) break;

			// read id (Link or Label)
			if(!strncasecmp(p, "Link", 4) && !isalpha(p[4]))
			{
				p += 4;
				while(isspace(*p)) p++;
				if(!*p) return false;
				tRisaWaveLoopLink link;
				if(!ReadLinkInformation(p, link)) return false;
				Links.push_back(link);
			}
			else if(!strncasecmp(p, "Label", 5) && !isalpha(p[5]))
			{
				p += 5;
				while(isspace(*p)) p++;
				if(!*p) return false;
				tRisaWaveLabel label;
				if(!ReadLabelInformation(p, label)) return false;
				Labels.push_back(label);
			}
			else
			{
				return false; // read error
			}

			// skip white space
			while(isspace(*p)) p++;
			if(!*p) break;
		}
	}

	return true; // done
}
//---------------------------------------------------------------------------




#ifdef RISA__IN_LOOP_TUNER


// ここはループチューナ内でのみ使われる部分


//---------------------------------------------------------------------------
void tRisaWaveLoopManager::PutInt(AnsiString &s, risse_int v)
{
	s += AnsiString((int)v);
}
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::PutInt64(AnsiString &s, risse_int64 v)
{
	s += AnsiString((__int64)v);
}
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::PutBool(AnsiString &s, bool v)
{
	s += v ? "True" : "False";
}
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::PutCondition(AnsiString &s, tRisaWaveLoopLink::tLinkCondition v)
{
	switch(v)
	{
		case tRisaWaveLoopLink::llcNone:             s += "no" ; break;
		case tRisaWaveLoopLink::llcEqual:            s += "eq" ; break;
		case tRisaWaveLoopLink::llcNotEqual:         s += "ne" ; break;
		case tRisaWaveLoopLink::llcGreater:          s += "gt" ; break;
		case tRisaWaveLoopLink::llcGreaterOrEqual:   s += "ge" ; break;
		case tRisaWaveLoopLink::llcLesser:           s += "lt" ; break;
		case tRisaWaveLoopLink::llcLesserOrEqual:    s += "le" ; break;
	}
}
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::PutString(AnsiString &s, tRisaLabelStringType v)
{
	// convert v to a utf-8 string
	const risse_char *pi;

#ifdef RISA__IN_LOOP_TUNER
	WideString wstr = v;
	pi = wstr.c_bstr();
#else
	pi = v.c_str();
#endif

	// count output bytes
	int size = RisaWideCharToUtf8String(pi, NULL);

	char * out = new char [size + 1];
	try
	{
		// convert the string
		RisaWideCharToUtf8String(pi, out);
		out[size] = '\0';

		// append the string with quotation
		s += "\'" + AnsiString(out) + "\'";
	}
	catch(...)
	{
		delete [] out;
		throw;
	}
	delete [] out;
}
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::DoSpacing(AnsiString &l, int col)
{
	// fill space until the string becomes specified length
	static const char * spaces16 = "                ";
	int length = l.Length();
	if(length < col)
	{
		int remain = col - length;
		while(remain)
		{
			int one_size = remain > 16 ? 16 : remain;
			l += ((16 - one_size) + spaces16);
			remain -= one_size;
		}
	}
}
//---------------------------------------------------------------------------
void tRisaWaveLoopManager::WriteInformation(AnsiString &s)
{
	// write current link/label information into s
	volatile tRisseCriticalSectionHolder CS(FlagsCS);

	// write banner
	s = "#2.00\n# Sound Loop Information (utf-8)\n"
		"# Generated by WaveLoopManager.cpp\n";

	// write links
/*
Link { From=0000000000000000; To=0000000000000000; Smooth=False; Condition=ne; RefValue=444444444; CondVar=99; }
*/
	for(std::vector<tRisaWaveLoopLink>::iterator i = Links.begin();
		i != Links.end(); i++)
	{
		AnsiString l;
		l = "Link { ";

		l += "From=";
		PutInt64(l, i->From);
		l += ";";
		DoSpacing(l, 30);

		l += "To=";
		PutInt64(l, i->To);
		l += ";";
		DoSpacing(l, 51);

		l += "Smooth=";
		PutBool(l, i->Smooth);
		l += ";";
		DoSpacing(l, 65);

		l += "Condition=";
		PutCondition(l, i->Condition);
		l += ";";
		DoSpacing(l, 79);

		l += "RefValue=";
		PutInt(l, i->RefValue);
		l += ";";
		DoSpacing(l, 100);

		l += "CondVar=";
		PutInt(l, i->CondVar);
		l += ";";
		DoSpacing(l, 112);

		l += "}\n";
		s += l;
	}


	// write labels
/*
Label { Position=0000000000000000; name="                                         "; }
*/
	for(std::vector<tRisaWaveLabel>::iterator i = Labels.begin();
		i != Labels.end(); i++)
	{
		AnsiString l;
		l = "Label { ";

		l += "Position=";
		PutInt64(l, i->Position);
		l += ";";
		DoSpacing(l, 35);

		l += "Name=";
		PutString(l, i->Name);
		l += "; ";
		DoSpacing(l, 85);

		l += "}\n";
		s += l;
	}

}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------





