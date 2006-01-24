//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief リバーブフィルタの実装 (Freeverb)
//---------------------------------------------------------------------------

#ifndef ReverbH
#define ReverbH

#include "BasicWaveFilter.h"
#include "fv_revmodel.hpp"

class tRisaReverb : public tRisaBasicWaveFilter
{
	static const size_t NumBufferSampleGranules = 4096;

	revmodel Model;

	float * Buffer;

public:
	tRisaReverb();
	~tRisaReverb();

private:
	void InputChanged();
	void Filter();
};


#endif

