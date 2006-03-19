//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseテキストストリーム
//---------------------------------------------------------------------------
#ifndef TEXTSTREAMH
#define TEXTSTREAMH


#include "risse/include/risse.h"
#include "base/utils/Singleton.h"
#include "base/script/RisseEngine.h"


//---------------------------------------------------------------------------
//! @brief		テキストストリームをスクリプトエンジンに登録するクラス
//---------------------------------------------------------------------------
class tRisaTextStreamRegisterer : public singleton_base<tRisaTextStreamRegisterer>, protected depends_on<tRisaRisseScriptEngine>
{
	static iRisseTextReadStream * CreateForRead(const tRisseString &name,
		const tRisseString &modestr);
	static iRisseTextWriteStream * CreateForWrite(const tRisseString &name,
		const tRisseString &modestr);

public:
	tRisaTextStreamRegisterer();
	~tRisaTextStreamRegisterer();
};
//---------------------------------------------------------------------------


#endif

