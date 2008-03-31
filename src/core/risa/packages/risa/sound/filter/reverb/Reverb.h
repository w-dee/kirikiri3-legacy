//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
//! @brief リバーブフィルタクラス
//---------------------------------------------------------------------------
class tReverbInstance : public tWaveFilterInstance
{
	static const size_t NumBufferSampleGranules = 4096;

	revmodel Model;

	float * Buffer;

public:
	//! @brief		コンストラクタ
	tReverbInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	~tReverbInstance() {;}

private:
	//! @brief		入力となるフィルタが変わったとき、あるいはリセットされるとき
	void InputChanged();

	//! @brief		フィルタ動作を行うとき
	void Filter();

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		"Reverb" クラス
//---------------------------------------------------------------------------
class tReverbClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tReverbClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public: // Risse 用メソッドなど
};
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
} // namespace Risa



#endif

