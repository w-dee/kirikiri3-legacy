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

#include "sound/filter/BasicWaveFilter.h"
#include "sound/filter/reverb/fv_revmodel.hpp"

//---------------------------------------------------------------------------
//! @brief リバーブフィルタクラス
//---------------------------------------------------------------------------
class tRisaReverb : public tRisaBasicWaveFilter
{
	static const size_t NumBufferSampleGranules = 4096;

	revmodel Model;

	float * Buffer;

public:
	//! @brief		コンストラクタ
	tRisaReverb();

	//! @brief		デストラクタ
	~tRisaReverb();

private:
	//! @brief		入力となるフィルタが変わったとき、あるいはリセットされるとき
	void InputChanged();

	//! @brief		フィルタ動作を行うとき
	void Filter();
};
//---------------------------------------------------------------------------


#endif

