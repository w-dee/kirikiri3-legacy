//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA ピン管理
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "visual/rina/rinaPin.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(31046,8618,37955,16475,39331,46428,7001,14742);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tPin::tPin()
{
	Node = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tPin::SuggestType(tPin * pin, bool * strong_suggest)
{
	const gc_vector<risse_uint32> & this_types = GetSupportedTypes();
	const gc_vector<risse_uint32> &  his_types = pin->GetSupportedTypes();

	// こちらの対応するタイプを基準にあちらのタイプと一致している物が
	// あるかどうかを見ていく。タイプはそれほど多くならないと考えられるので
	// 非常に原始的なアルゴリズムでマッチングを行う
	for(gc_vector<risse_uint32>::const_iterator ti = this_types.begin();
		ti != this_types.end(); ti++)
	{
		for(gc_vector<risse_uint32>::const_iterator hi = his_types.begin();
			hi != his_types.end(); hi++)
			if(*ti == *hi)
			{
				if(strong_suggest) *strong_suggest = false;
				// このメソッドをオーバーライドしない限り
				// *strong_suggest が真になることはない ...
				return *ti;
			}
	}
	return 0;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tInputPin::tInputPin()
{
	OutputPin = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tInputPin::Negotiate(tOutputPin * output_pin)
{
	bool this_strong_suggestion;
	risse_uint32 this_suggested_type = SuggestType(output_pin, &this_strong_suggestion);
	bool  his_strong_suggestion;
	risse_uint32  his_suggested_type = output_pin->SuggestType(this, & his_strong_suggestion);

	// どちらかが 0 を返した場合は同意に至らなかったと見なす
	if(this_suggested_type == 0 || his_suggested_type == 0) return 0;

	// どちらかが strong suggestion を示した場合はそちらを採用する
	if(this_strong_suggestion && !his_strong_suggestion) return this_suggested_type;
	if(!this_strong_suggestion && his_strong_suggestion) return  his_suggested_type;

	// どちらも strong suggestion では無かった場合や どちらも strong suggestion だった場合は
	// 入力ピン側の結果を優先する
	return this_suggested_type;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tInputPin::InternalConnect(tOutputPin * output_pin)
{
	AgreedType = Negotiate(output_pin);
//	if(AgreedType == 0) { /* TODO: 例外 */ }
	RISSE_ASSERT(AgreedType != 0);
	if(OutputPin)
	{
		OutputPin->Disconnect(this);
		OutputPin->GetNode()->CalcLongestDistance();
	}
	OutputPin = output_pin;
	output_pin->Connect(this);
	OutputPin->GetNode()->CalcLongestDistance();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInputPin::Connect(tOutputPin * output_pin)
{
	volatile tGraphLocker holder(*this);
	InternalConnect(output_pin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tInputPin::GetAgreedType() const
{
	volatile tGraphLocker holder(*this);
	return AgreedType;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tOutputPin::tOutputPin()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tOutputPin::GetLongestDistance() const
{
	risse_size longest = risse_size_max;
	for(tInputPins::const_iterator i = InputPins.begin(); i != InputPins.end(); i++)
	{
		risse_size dist = (*i)->GetNode()->GetLongestDistance();
		if(longest == risse_size_max || longest < dist)
			longest = dist;
	}
	return longest;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOutputPin::Connect(tInputPin * input_pin)
{
	if(std::find(InputPins.begin(), InputPins.end(), input_pin) == InputPins.end())
		InputPins.push_back(input_pin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOutputPin::Disconnect(tInputPin * input_pin)
{
	tInputPins::iterator i = std::find(InputPins.begin(), InputPins.end(), input_pin);
	RISSE_ASSERT(i != InputPins.end());
	InputPins.erase(i);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
