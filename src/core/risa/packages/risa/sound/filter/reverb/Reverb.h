//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief リバーブフィルタの実装 (Freeverb)
//---------------------------------------------------------------------------

#ifndef ReverbH
#define ReverbH

#include "risa/packages/risa/sound/filter/BasicWaveFilter.h"
#include "risa/packages/risa/sound/filter/reverb/fv_revmodel.hpp"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * リバーブフィルタクラス
 */
class tReverbInstance : public tWaveFilterInstance
{
	static const size_t NumBufferSampleGranules = 4096;

	revmodel Model;

	float * Buffer;

public:
	/**
	 * コンストラクタ
	 */
	tReverbInstance();

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	~tReverbInstance() {;}

private:
	/**
	 * 入力となるフィルタが変わったとき、あるいはリセットされるとき
	 */
	void InputChanged();

	/**
	 * フィルタ動作を行うとき
	 */
	void Filter();

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
/**
 * "Reverb" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tReverbClass, tClassBase, tReverbInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
} // namespace Risa



#endif

