//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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

#include "risa/prec.h"
#include <algorithm>
#include "risa/common/RisaGC.h"
#include "risa/packages/risa/sound/WaveLoopManager.h"



#include "risa/packages/risa/sound/WaveDecoder.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(8592,58083,15398,17259,37009,18155,50172,63359);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * クロスフェードを行うためのテンプレート関数
 * @param dest			結果格納先
 * @param src1			ソース1 (消えていく方)
 * @param src2			ソース2 (入ってくる方)
 * @param ratiostart	ブレンド率の開始値(%)
 * @param ratioend		ブレンド率の終了値(%)
 * @param samples		この関数の呼び出しで処理すべきサンプル数
 * @param channels		対象PCMのチャンネル数
 */
template <typename T>
static void CrossFadeBlend(void *dest, void *src1, void *src2,
	risse_int ratiostart, risse_int ratioend,
	risse_int samples, risse_int channels)
{
	risse_uint blend_step = (risse_int)(
		(
			(ratioend - ratiostart) * ((risse_int64)1<<32) / 100
		) / samples);

	risse_uint8 * d = static_cast<risse_uint8*>(dest);
	const risse_uint8 * s1 = static_cast<const risse_uint8*>(src1);
	const risse_uint8 * s2 = static_cast<const risse_uint8*>(src2);

	risse_uint ratio = (risse_int)(ratiostart * ((risse_int64)1<<32) / 100);
	for(risse_int i = 0; i < samples; i++)
	{
		for(risse_int j = channels - 1; j >= 0; j--)
		{
			risse_int32 si1 = reinterpret_cast<const T*>(s1)->geti32();
			risse_int32 si2 = reinterpret_cast<const T*>(s2)->geti32();
			risse_int o = (risse_int) (
						(((risse_int64)si2 * (risse_uint64)ratio) >> 32) +
						(((risse_int64)si1 * (RISSE_UI64_VAL(0x100000000) - (risse_uint64)ratio) ) >> 32) );
			reinterpret_cast<T*>(d)->seti32(o);
			d += T::size;
			s1 += T::size;
			s2 += T::size;
		}
		ratio += blend_step;
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tWaveLoopManager::tWaveLoopManager(tWaveDecoder * decoder)
{
	FlagsCS = new tCriticalSection();
	DataCS = new tCriticalSection();
	Position = 0;
	IsLinksSorted = false;
	IsLabelsSorted = false;
	CrossFadeSamples = NULL;
	CrossFadeLen = 0;
	CrossFadePosition = 0;
	IgnoreLinks = false;
	Looping = false;
	FileInfo = new tWaveFileInfo;
	memset(FileInfo, 0, sizeof(*FileInfo));
	FirstRendered = false;

	ClearFlags();
	FlagsModifiedByLabelExpression = false;

	SetDecoder(decoder);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::SetDecoder(tWaveDecoder * decoder)
{
	// set decoder
	Decoder = decoder;
	FirstRendered = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tWaveLoopManager::GetFlag(risse_int index)
{
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	return Flags[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::CopyFlags(risse_int *dest)
{
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	// copy flags into dest, and clear FlagsModifiedByLabelExpression
	memcpy(dest, Flags, sizeof(Flags));
	FlagsModifiedByLabelExpression = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tWaveLoopManager::GetFlagsModifiedByLabelExpression()
{
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	return FlagsModifiedByLabelExpression;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::SetFlag(risse_int index, risse_int f)
{
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	if(f < 0) f = 0;
	if(f > MaxFlagValue) f = MaxFlagValue;
	Flags[index] = f;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::ClearFlags()
{
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	for(risse_int i = 0; i < MaxFlags; i++) Flags[i] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::ClearLinksAndLabels()
{
	// clear links and labels
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	Labels.clear();
	Links.clear();
	IsLinksSorted = false;
	IsLabelsSorted = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<tWaveLoopLink> & tWaveLoopManager::GetLinks() const
{
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	return Links;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<tWaveLabel> & tWaveLoopManager::GetLabels() const
{
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	return Labels;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::SetLinks(const gc_vector<tWaveLoopLink> & links)
{
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	Links = links;
	IsLinksSorted = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::SetLabels(const gc_vector<tWaveLabel> & labels)
{
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	Labels = labels;
	IsLabelsSorted = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tWaveLoopManager::GetIgnoreLinks() const
{
	volatile tCriticalSection::tLocker CS(*DataCS);
	return IgnoreLinks;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::SetIgnoreLinks(bool b)
{
	volatile tCriticalSection::tLocker CS(*DataCS);
	IgnoreLinks = b;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int64 tWaveLoopManager::GetPosition() const
{
	// we cannot assume that the 64bit data access is truely atomic on 32bit machines.
	volatile tCriticalSection::tLocker CS(*FlagsCS);
	return Position;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::SetPosition(risse_int64 pos)
{
	volatile tCriticalSection::tLocker CS(*DataCS);
	Position = pos;
	ClearCrossFadeInformation();
	Decoder->SetPosition(pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tWaveLoopManager::Render(void *dest, risse_uint samples, risse_uint &written,
		tWaveSegmentQueue & segmentqueue)
{
	// check if this is the first try to render
	if(!FirstRendered)
	{
		if(Decoder)
			Decoder->GetFormat(*FileInfo);
		else
			memset(FileInfo, 0, sizeof(*FileInfo));
		ShortCrossFadeHalfSamples =
			FileInfo->Frequency * SmoothTimeHalf / 1000;
		FirstRendered = true;
	}

	// decode from current position
	volatile tCriticalSection::tLocker CS(*DataCS);

	written = 0;
	risse_uint8 *d = (risse_uint8*)dest;

	risse_int give_up_count = 0;

	gc_deque<tWaveEvent> events;

	while(written != samples/* && Position < FileInfo->TotalSamples*/)
	{
		// clear events
		events.clear();

		// decide next operation
		risse_int64 next_event_pos;
		bool next_not_found = false;
		risse_int before_count;

		// check nearest link
		tWaveLoopLink link;
		if(!IgnoreLinks && GetNearestLink(Position, link, false))
		{
			// nearest link found ...
			if(link.From == Position)
			{
				// do jump
				give_up_count ++;
				if(give_up_count >= WaveLoopLinkGiveUpCount)
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
					if(FileInfo->TotalSampleGranules - link.From < static_cast<risse_uint64>(after_count))
						after_count =
							(risse_int)(FileInfo->TotalSampleGranules - link.From);
					if(FileInfo->TotalSampleGranules - link.To < static_cast<risse_uint64>(after_count))
						after_count =
							(risse_int)(FileInfo->TotalSampleGranules - link.To);
					tWaveLoopLink over_to_link;
					if(GetNearestLink(link.To, over_to_link, true))
					{
						if(over_to_link.From - link.To < after_count)
							after_count =
								(risse_int)(over_to_link.From - link.To);
					}
					// prepare crossfade
					// allocate memory
					risse_uint8 *src1 = NULL;
					risse_uint8 *src2 = NULL;

					risse_int alloc_size =
						(before_count + after_count) * 
							FileInfo->GetSampleGranuleSize();
					CrossFadeSamples = (risse_uint8*)MallocAtomicCollectee(alloc_size);
					src1 = (risse_uint8*)MallocAtomicCollectee(alloc_size*2);
					src2 = src1 + alloc_size;

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
							before_count * FileInfo->GetSampleGranuleSize();
						DoCrossFade(CrossFadeSamples,
							src1, src2, before_count, 0, 50);
						DoCrossFade(CrossFadeSamples + after_offset,
							src1 + after_offset, src2 + after_offset,
								after_count, 50, 100);
						FreeCollectee(src1);
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

		if(one_unit > 0) give_up_count = 0; // reset give up count

		// evaluate each label
		GetEventAt(Position, Position + one_unit, events);
		for(gc_deque<tWaveEvent>::iterator i = events.begin();
			i != events.end(); i++)
		{
			if(i->Name.c_str()[0] == ':')
			{
				// for each label
				EvalLabelExpression(i->Name);
			}
		}

		// calculate each label offset
		for(gc_deque<tWaveEvent>::iterator i = events.begin();
			i != events.end(); i++)
			i->Offset = (risse_int)(i->Position - Position) + written;

		// enqueue events
		segmentqueue.Enqueue(events);

		// enqueue segment
		segmentqueue.Enqueue(tWaveSegment(Position, one_unit));

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
			d += decoded * FileInfo->GetSampleGranuleSize();
		}
		else
		{
			// in cross fade
			// copy prepared samples
			memcpy((void *)d,
				CrossFadeSamples +
					CrossFadePosition * FileInfo->GetSampleGranuleSize(),
				one_unit * FileInfo->GetSampleGranuleSize());
			CrossFadePosition += one_unit;
			Position += one_unit;
			written += one_unit;
			d += one_unit * FileInfo->GetSampleGranuleSize();
			if(CrossFadePosition == CrossFadeLen)
			{
				// crossfade has finished
				ClearCrossFadeInformation();
			}
		}
	}	// while 
	return written == samples;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::SuggestFormat(const tWaveFormat & format)
{
	// PCM 形式の変更は、最初に Render が呼ばれるよりも前でなければならない
	if(!FirstRendered)
	{
		// そのまま Decoder に要求を流す
		Decoder->SuggestFormat(format);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tWaveFormat & tWaveLoopManager::GetFormat()
{
	if(!FirstRendered)
	{
		// まだ形式を入力から得てない場合はここで得る
		Decoder->GetFormat(*FileInfo);
	}

	return *FileInfo;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tWaveLoopManager::GetNearestLink(risse_int64 current,
		tWaveLoopLink & link, bool ignore_conditions)
{
	// search nearest event in future, from current.
	// this checks conditions unless ignore_conditions is true.
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
				case tWaveLoopLink::llcNone:
					match = true; break;
				case tWaveLoopLink::llcEqual:
					if(Links[s].RefValue == Flags[Links[s].CondVar]) match = true;
					break;
				case tWaveLoopLink::llcNotEqual:
					if(Links[s].RefValue != Flags[Links[s].CondVar]) match = true;
					break;
				case tWaveLoopLink::llcGreater:
					if(Links[s].RefValue <  Flags[Links[s].CondVar]) match = true;
					break;
				case tWaveLoopLink::llcGreaterOrEqual:
					if(Links[s].RefValue <= Flags[Links[s].CondVar]) match = true;
					break;
				case tWaveLoopLink::llcLesser:
					if(Links[s].RefValue >  Flags[Links[s].CondVar]) match = true;
					break;
				case tWaveLoopLink::llcLesserOrEqual:
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
void tWaveLoopManager::GetEventAt(risse_int64 from, risse_int64 to,
		gc_deque<tWaveEvent> & events)
{
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
			events.push_back(*static_cast<tWaveEvent*>(&(Labels[s])));
		else
			break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::DoCrossFade(void *dest, void *src1,
	void *src2, risse_int samples, risse_int ratiostart, risse_int ratioend)
{
	// do on-memory wave crossfade
	// using src1 (fading out) and src2 (fading in).
	if(samples == 0) return; // nothing to do

	switch(FileInfo->PCMType)
	{
	case tPCMTypes::ti8 :			//!< 8bit integer linear PCM type
		CrossFadeBlend<tPCMTypes::i8>(dest, src1, src2,
			ratiostart, ratioend, samples, FileInfo->Channels);
		break;
	case tPCMTypes::ti16:			//!< 16bit integer linear PCM type
		CrossFadeBlend<tPCMTypes::i16>(dest, src1, src2,
			ratiostart, ratioend, samples, FileInfo->Channels);
		break;
	case tPCMTypes::ti24:			//!< 24bit integer linear PCM type
		CrossFadeBlend<tPCMTypes::i24>(dest, src1, src2,
			ratiostart, ratioend, samples, FileInfo->Channels);
		break;
	case tPCMTypes::ti32:			//!< 32bit integer linear PCM type
		CrossFadeBlend<tPCMTypes::i32>(dest, src1, src2,
			ratiostart, ratioend, samples, FileInfo->Channels);
		break;
	case tPCMTypes::tf32:			//!< 32bit float linear PCM type
		CrossFadeBlend<tPCMTypes::f32>(dest, src1, src2,
			ratiostart, ratioend, samples, FileInfo->Channels);
		break;
	default:
		break; // unknown type
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveLoopManager::ClearCrossFadeInformation()
{
	if(CrossFadeSamples) FreeCollectee(CrossFadeSamples), CrossFadeSamples = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tWaveLoopManager::GetLabelExpression(const tLabelStringType &label,
	tWaveLoopManager::tExpressionToken * ope,
	risse_int *lv,
	risse_int *rv, bool *is_rv_indirect)
{
	const tLabelCharType * p = label.c_str();
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
bool tWaveLoopManager::EvalLabelExpression(const tLabelStringType &label)
{
	// eval expression specified by 'label'
	// commit the result when 'commit' is true.
	// returns whether the label syntax is correct.
	volatile tCriticalSection::tLocker CS(*FlagsCS);

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
tWaveLoopManager::tExpressionToken
	tWaveLoopManager::GetExpressionToken(const tLabelCharType *  & p, risse_int * value)
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
bool tWaveLoopManager::GetLabelCharInt(const tLabelCharType *s, risse_int &v)
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
bool tWaveLoopManager::GetInt(char *s, risse_int &v)
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
bool tWaveLoopManager::GetInt64(char *s, risse_int64 &v)
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
bool tWaveLoopManager::GetBool(char *s, bool &v)
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
bool tWaveLoopManager::GetCondition(char *s, tWaveLoopLink::tLinkCondition &v)
{
	// get condition value
	if(!strcasecmp(s, "no")) { v = tWaveLoopLink::llcNone;				return true;	}
	if(!strcasecmp(s, "eq")) { v = tWaveLoopLink::llcEqual;				return true;	}
	if(!strcasecmp(s, "ne")) { v = tWaveLoopLink::llcNotEqual;			return true;	}
	if(!strcasecmp(s, "gt")) { v = tWaveLoopLink::llcGreater;			return true;	}
	if(!strcasecmp(s, "ge")) { v = tWaveLoopLink::llcGreaterOrEqual;	return true;	}
	if(!strcasecmp(s, "lt")) { v = tWaveLoopLink::llcLesser;			return true;	}
	if(!strcasecmp(s, "le")) { v = tWaveLoopLink::llcLesserOrEqual;		return true;	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tWaveLoopManager::GetString(char *s, tLabelStringType &v)
{
	// convert utf-8 string s to v
#ifdef RISA_IN_LOOP_TUNER

	// compute output (unicode) size
	risse_int size = Utf8ToWideCharString(s, NULL);
	if(size == -1) return false; // not able to convert the string

	// allocate output buffer
	risse_char *us = new (PointerFreeGC) risse_char[size + 1];

	Utf8ToWideCharString(s, us);
	us[size] = RISSE_W('\0');

	// convert us (an array of wchar_t) to AnsiString
	v = AnsiString(us);

	return true;
#else
	v = tString(wxString(s, wxConvUTF8));
	return true;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tWaveLoopManager::GetEntityToken(char * & p, char **name, char **value)
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
bool tWaveLoopManager::ReadLinkInformation(char * & p, tWaveLoopLink &link)
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
bool tWaveLoopManager::ReadLabelInformation(char * & p, tWaveLabel &label)
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
bool tWaveLoopManager::ReadInformation(char * p)
{
	// read information from 'p'
	volatile tCriticalSection::tLocker CS(*FlagsCS);

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
		tWaveLoopLink link;
		link.Smooth = false;
		link.Condition = tWaveLoopLink::llcNone;
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
				tWaveLoopLink link;
				if(!ReadLinkInformation(p, link)) return false;
				Links.push_back(link);
			}
			else if(!strncasecmp(p, "Label", 5) && !isalpha(p[5]))
			{
				p += 5;
				while(isspace(*p)) p++;
				if(!*p) return false;
				tWaveLabel label;
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




#ifdef RISA_IN_LOOP_TUNER


// ここはループチューナ内でのみ使われる部分


//---------------------------------------------------------------------------
void tWaveLoopManager::PutInt(AnsiString &s, risse_int v)
{
	s += AnsiString((int)v);
}
//---------------------------------------------------------------------------
void tWaveLoopManager::PutInt64(AnsiString &s, risse_int64 v)
{
	s += AnsiString((__int64)v);
}
//---------------------------------------------------------------------------
void tWaveLoopManager::PutBool(AnsiString &s, bool v)
{
	s += v ? "True" : "False";
}
//---------------------------------------------------------------------------
void tWaveLoopManager::PutCondition(AnsiString &s, tWaveLoopLink::tLinkCondition v)
{
	switch(v)
	{
		case tWaveLoopLink::llcNone:             s += "no" ; break;
		case tWaveLoopLink::llcEqual:            s += "eq" ; break;
		case tWaveLoopLink::llcNotEqual:         s += "ne" ; break;
		case tWaveLoopLink::llcGreater:          s += "gt" ; break;
		case tWaveLoopLink::llcGreaterOrEqual:   s += "ge" ; break;
		case tWaveLoopLink::llcLesser:           s += "lt" ; break;
		case tWaveLoopLink::llcLesserOrEqual:    s += "le" ; break;
	}
}
//---------------------------------------------------------------------------
void tWaveLoopManager::PutString(AnsiString &s, tLabelStringType v)
{
	// convert v to a utf-8 string
	const risse_char *pi;

#ifdef RISA_IN_LOOP_TUNER
	WideString wstr = v;
	pi = wstr.c_bstr();
#else
	pi = v.c_str();
#endif

	// count output bytes
	int size = WideCharToUtf8String(pi, NULL);

	char * out = new (PointerFreeGC) char [size + 1];

	// convert the string
	WideCharToUtf8String(pi, out);
	out[size] = '\0';

	// append the string with quotation
	s += "\'" + AnsiString(out) + "\'";
}
//---------------------------------------------------------------------------
void tWaveLoopManager::DoSpacing(AnsiString &l, int col)
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
void tWaveLoopManager::WriteInformation(AnsiString &s)
{
	// write current link/label information into s
	volatile tCriticalSection::tLocker CS(*FlagsCS);

	// write banner
	s = "#2.00\n# Sound Loop Information (utf-8)\n"
		"# Generated by WaveLoopManager.cpp\n";

	// write links
/*
Link { From=0000000000000000; To=0000000000000000; Smooth=False; Condition=ne; RefValue=444444444; CondVar=99; }
*/
	for(gc_vector<tWaveLoopLink>::iterator i = Links.begin();
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
	for(gc_vector<tWaveLabel>::iterator i = Labels.begin();
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





//---------------------------------------------------------------------------
} // namespace Risa


