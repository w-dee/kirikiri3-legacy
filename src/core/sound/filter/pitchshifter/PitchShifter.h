//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief FFTを用いたピッチシフタ
//---------------------------------------------------------------------------

#ifndef PitchShifterH
#define PitchShifterH

#include "BasicWaveFilter.h"
#include "smbPitchShift.h"

class tRisaPitchShifter : public tRisaBasicWaveFilter
{
	std::vector<boost::shared_ptr<smbPitchShifter> > Shifters;
	float * Buffer;
	float Pitch;
	risse_uint FrameSize;

public:
	tRisaPitchShifter(float pitch, risse_uint windowsize);
	~tRisaPitchShifter();

private:
	void InputChanged();
	void Filter();

};


#endif

