//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンソールフレーム
//---------------------------------------------------------------------------
#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "risse/include/risse.h"


//---------------------------------------------------------------------------
//! @brief		コンソールのフレーム
//---------------------------------------------------------------------------
class tRisaLogScrollView;
class tRisaConsoleFrame : public wxFrame
{
public:
	tRisaConsoleFrame();

private:
	tRisaLogScrollView *ScrollView;

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


#endif
