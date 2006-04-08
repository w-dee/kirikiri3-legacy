//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OpenAL 共通
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/exception/RisaException.h"
#include "sound/WaveDecoder.h"
#include "sound/ALCommon.h"

RISSE_DEFINE_SOURCE_ID(9828,18466,5910,19138,44164,25475,18182,51394);


//---------------------------------------------------------------------------
tRisaOpenAL::tRisaOpenAL()
{
	// フィールドの初期化
	Device = NULL;
	Context = NULL;

	// デバイスを開く
	Device = alcOpenDevice(NULL);
		// TODO: デバイスの選択と null デバイスへのフォールバック

	if(!Device)
		eRisaException::Throw(RISSE_WS_TR("failed to create OpenAL device"));

	try
	{
		// コンテキストを作成する
		Context = alcCreateContext(Device, NULL);
		ThrowIfError(RISSE_WS("alcCreateContext"));

		// コンテキストを選択する
		alcMakeContextCurrent(Context);
		ThrowIfError(RISSE_WS("alcMakeContextCurrent"));
	}
	catch(...)
	{
		Clear();
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaOpenAL::~tRisaOpenAL()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaOpenAL::Clear()
{
	if(Context)
	{
		alcMakeContextCurrent(NULL);
		alcDestroyContext(Context), Context = NULL;
	}

	if(Device)
	{
		alcCloseDevice(Device), Device = NULL;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaOpenAL::ThrowIfError(const risse_char * message)
{
	ALCenum err = alGetError();
	if(err == AL_NO_ERROR) return ; // エラーはなにも起きていない
	const ALCchar *msg = alcGetString(Device, err);
	eRisaException::Throw(RISSE_WS_TR("OpenAL error in %1 : %2"),
		ttstr(message),
		ttstr(wxString(msg, wxConvUTF8)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaOpenAL::ClearErrorState()
{
	alGetError();
}
//---------------------------------------------------------------------------

