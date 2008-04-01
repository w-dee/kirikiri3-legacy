//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OpenAL 共通
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/sound/WaveDecoder.h"
#include "risa/packages/risa/sound/ALCommon.h"
#include "risa/packages/risa/sound/Sound.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(9828,18466,5910,19138,44164,25475,18182,51394);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOpenAL::tOpenAL()
{
	// フィールドの初期化
	Device = NULL;
	Context = NULL;

	// デバイスを開く
	Device = alcOpenDevice(NULL);
		// TODO: デバイスの選択と null デバイスへのフォールバック

	if(!Device)
		tSoundExceptionClass::Throw(RISSE_WS_TR("failed to create OpenAL device"));

	try
	{
		// コンテキストを作成する
		Context = alcCreateContext(Device, NULL);
		ThrowIfError(RISSE_WS("alcCreateContext"));

		// コンテキストを選択する
		if(!alcMakeContextCurrent(Context))
			tSoundExceptionClass::Throw(RISSE_WS_TR("failed to select OpenAL context"));
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
tOpenAL::~tOpenAL()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOpenAL::Clear()
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
void tOpenAL::ThrowIfError(const risse_char * message)
{
	ALCenum err = alGetError();
	if(err == AL_NO_ERROR) return ; // エラーはなにも起きていない
	const ALCchar *msg = alcGetString(Device, err);
	tSoundExceptionClass::Throw(tString(RISSE_WS_TR("OpenAL error in %1 : %2"),
		tString(message),
		tString(wxString(msg, wxConvUTF8))));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOpenAL::ClearErrorState()
{
	alGetError();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


