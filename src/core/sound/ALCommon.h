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
	( depends_on でインスタンスの存在を確実にできる場合は
	 depends_on<tRisaOpenAL>::locked_instance()->ThrowIfError())
	 の方が効率がよい )

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
	struct tCriticalSectionHolder : protected depends_on<tRisaOpenAL>
	{
		tRisaCriticalSection::tLocker holder;
		tCriticalSectionHolder() : holder(depends_on<tRisaOpenAL>::locked_instance()->GetCS())
		{
			// エラー状態をクリアする
			depends_on<tRisaOpenAL>::locked_instance()->ClearErrorState();
		}
		~tCriticalSectionHolder()
		{
		}
	};


private:
	ALCdevice * Device; //!< デバイス
	ALCcontext * Context; //!< コンテキスト

	tRisaCriticalSection CS; //!< OpenAL API を保護する CS


public:
	//! @brief		コンストラクタ
	tRisaOpenAL();

	//! @brief		デストラクタ
	~tRisaOpenAL();

private:
	//! @brief		クリーンアップ処理
	void Clear();

public:
	//! @brief		現在のエラーに対応する例外を投げる
	//! @param		message メッセージ
	//! @note		エラーが何も発生していない場合は何もしない
	void ThrowIfError(const risse_char * message);

	//! @brief		OpenAL のエラー状態をクリアする
	void ClearErrorState();

	tRisaCriticalSection & GetCS() { return CS; } //!< このオブジェクトを保護しているCSを得る
};
//---------------------------------------------------------------------------
#endif
