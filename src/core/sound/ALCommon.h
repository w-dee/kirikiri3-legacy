//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "base/utils/Thread.h"


/*
	some notes

	OpenAL API を実行するときは基本的に

	tOpenAL::tCriticalSectionHolder cs_holder;

	で OpenAL API を保護し、その後

	tOpenAL::instance()->ThrowIfError();

	でエラーをチェックするという方法をとること。
	( depends_on でインスタンスの存在を確実にできる場合は
	 depends_on<tOpenAL>::locked_instance()->ThrowIfError())
	 の方が効率がよい )

	OpenAL の API は API 実行後に alGetError でエラーコードを取得するという
	方法をとるが、API 実行とalGetErrorの間に、ほかのスレッドが他の API を
	実行してしまうことによって alGetError が正しい値を得ないという可能性がある。
	
*/

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		OpenAL管理クラス
//---------------------------------------------------------------------------
class tOpenAL : public singleton_base<tOpenAL>, manual_start<tOpenAL>
{
public:
	//! @brief OpenAL APIを保護するためのクリティカルセクションホルダ
	struct tCriticalSectionHolder : protected depends_on<tOpenAL>
	{
		tCriticalSection::tLocker holder;
		tCriticalSectionHolder() : holder(depends_on<tOpenAL>::locked_instance()->GetCS())
		{
			// エラー状態をクリアする
			depends_on<tOpenAL>::locked_instance()->ClearErrorState();
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
	//! @brief		コンストラクタ
	tOpenAL();

	//! @brief		デストラクタ
	~tOpenAL();

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

	tCriticalSection & GetCS() { return CS; } //!< このオブジェクトを保護しているCSを得る
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
