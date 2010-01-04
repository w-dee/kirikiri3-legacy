//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OpenAL 共通
//---------------------------------------------------------------------------
#ifndef ALCommonH
#define ALCommonH

#include "risa/packages/risa/sound/WaveDecoder.h"
#include <AL/al.h>
#include <AL/alc.h>
#include "risa/common/Singleton.h"
#include "risa/common/RisaThread.h"


/*
	some notes

	OpenAL API を実行するときは基本的に

	tOpenAL::tCriticalSectionHolder cs_holder;

	で OpenAL API を保護し、その後

	tOpenAL::instance()->ThrowIfError();

	でエラーをチェックするという方法をとること。

	OpenAL の API は API 実行後に alGetError でエラーコードを取得するという
	方法をとるが、API 実行とalGetErrorの間に、ほかのスレッドが他の API を
	実行してしまうことによって alGetError が正しい値を得ないという可能性がある。
	
*/

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * OpenAL管理クラス
 */
class tOpenAL : public singleton_base<tOpenAL>, manual_start<tOpenAL>
{
public:
	/**
	 * OpenAL APIを保護するためのクリティカルセクションホルダ
	 */
	struct tCriticalSectionHolder : protected depends_on<tOpenAL>
	{
		tCriticalSection::tLocker holder;
		tCriticalSectionHolder() : holder(tOpenAL::instance()->GetCS())
		{
			// エラー状態をクリアする
			tOpenAL::instance()->ClearErrorState();
		}
		~tCriticalSectionHolder()
		{
		}
	};


private:
	ALCdevice * Device; //!< デバイス
	ALCcontext * Context; //!< コンテキスト

	tCriticalSection CS; //!< OpenAL API を保護する CS


public:
	/**
	 * コンストラクタ
	 */
	tOpenAL();

	/**
	 * デストラクタ
	 */
	~tOpenAL();

private:
	/**
	 * クリーンアップ処理
	 */
	void Clear();

public:
	/**
	 * 現在のエラーに対応する例外を投げる
	 * @param message	メッセージ
	 * @note	エラーが何も発生していない場合は何もしない
	 */
	void ThrowIfError(const risse_char * message);

	/**
	 * OpenAL のエラー状態をクリアする
	 */
	void ClearErrorState();

	tCriticalSection & GetCS() { return CS; } //!< このオブジェクトを保護しているCSを得る
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
