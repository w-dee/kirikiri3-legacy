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

#include "WaveDecoder.h"
#include "al.h"
#include "alc.h"
#include "Singleton.h"


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
class tRisaOpenAL
{
public:
	//! @brief OpenAL APIを保護するためのクリティカルセクションホルダ
	struct tCriticalSectionHolder
	{
		tRisseCriticalSection::tLocker holder;
		tCriticalSectionHolder() : holder(tRisaOpenAL::instance()->GetCS())
		{
			// エラー状態をクリアする
			tRisaOpenAL::instance()->ClearErrorState();
		}
	};


private:
	ALCdevice * Device; //!< デバイス
	ALCcontext * Context; //!< コンテキスト

	tRisseCriticalSection CS; //!< OpenAL API を保護する CS


public:
	tRisaOpenAL();
	~tRisaOpenAL();

private:
	void Clear();

public:
	void ThrowIfError(const risse_char * message);
	void ClearErrorState();
	tRisseCriticalSection & GetCS() { return CS; }


private:
	tRisaSingletonObjectLifeTracer<tRisaOpenAL> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tRisaOpenAL> & instance() { return
		tRisaSingleton<tRisaOpenAL>::instance();
	}
};
//---------------------------------------------------------------------------
#endif
