//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OpenAL ソース管理
//---------------------------------------------------------------------------
#ifndef ALSourceH
#define ALSourceH

#include <al.h>
#include <alc.h>
#include "sound/ALCommon.h"
#include "sound/ALBuffer.h"


//---------------------------------------------------------------------------
//! @brief		OpenALソース
//---------------------------------------------------------------------------
class tRisaALSource
{
	risse_uint NumBuffersQueued; //!< キューに入っているバッファの数
	ALuint Source; //!< OpenAL ソース
	bool SourceAllocated; //!< Source がすでに割り当てられているかどうか
	tRisaSingleton<tRisaOpenAL> ref_tRisaOpenAL; //!< tRisaOpenAL インスタンス
	boost::shared_ptr<tRisaALBuffer> Buffer; // バッファ

public:
	tRisaALSource(boost::shared_ptr<tRisaALBuffer> buffer);
	~tRisaALSource();

	ALuint GetSource() const { return Source; } // Source を得る

private:
	void Clear();

public:
	void Play();
	void Stop();

};
//---------------------------------------------------------------------------

#endif
