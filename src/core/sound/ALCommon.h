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
#ifndef ALCommonH
#define ALCommonH

#include "sound/WaveDecoder.h"
#include <al.h>
#include <alc.h>
#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"


/*
	some notes

	OpenAL API を実行するときは基本的に

	tRisaOpenAL::tCriticalSectionHolder cs_holder;

	で OpenAL API を保護し、その後

	tRisaOpenAL::instance()->ThrowIfError();

	でエラーをチェックするという方法をとること。

	OpenAL の API は API 実行後に alGetError でエラーコードを取得するという
	方法をとるが、API 実行とalGetErrorの間に、ほかのスレッドが他の API を
	実行してしまうことによって alGetError が正しい値を得ないという可能性がある。
	
*/

//---------------------------------------------------------------------------
//! @brief		OpenAL管理クラス
//---------------------------------------------------------------------------
class tRisaOpenAL : public singleton_base<tRisaOpenAL>, manual_start<tRisaOpenAL>
{
public:
	//! @brief OpenAL APIを保護するためのクリティカルセクションホルダ
	struct tCriticalSectionHolder
	{
		tRisaCriticalSection::tLocker holder;
		tCriticalSectionHolder() : holder(tRisaOpenAL::instance()->GetCS())
		{
			// エラー状態をクリアする
			tRisaOpenAL::instance()->ClearErrorState();
		}
	};


private:
	ALCdevice * Device; //!< デバイス
	ALCcontext * Context; //!< コンテキスト

	tRisaCriticalSection CS; //!< OpenAL API を保護する CS


public:
	tRisaOpenAL();
	~tRisaOpenAL();

private:
	void Clear();

public:
	void ThrowIfError(const risse_char * message);
	void ClearErrorState();
	tRisaCriticalSection & GetCS() { return CS; }
};
//---------------------------------------------------------------------------
#endif
