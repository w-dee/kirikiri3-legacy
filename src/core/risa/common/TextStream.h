//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseテキストストリーム
//---------------------------------------------------------------------------
#ifndef TEXTSTREAMH
#define TEXTSTREAMH


#include "risseScriptEngine.h"
#include "risa/common/Singleton.h"
#include "risa/common/RisseEngine.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * テキストストリームをスクリプトエンジンに登録するクラス
 */
class tTextStreamRegisterer : public singleton_base<tTextStreamRegisterer>, protected depends_on<tRisseScriptEngine>
{
	/**
	 * 読み込み用テキストストリームを作成して返す
	 */
	static iRisseTextReadStream * CreateForRead(const tString &name,
		const tString &modestr);

	/**
	 * 書き込み用テキストストリームを作成して返す
	 */
	static iRisseTextWriteStream * CreateForWrite(const tString &name,
		const tString &modestr);

public:
	/**
	 * コンストラクタ
	 */
	tTextStreamRegisterer();

	/**
	 * デストラクタ
	 */
	~tTextStreamRegisterer();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif

