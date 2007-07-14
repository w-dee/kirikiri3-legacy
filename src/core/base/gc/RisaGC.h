//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ガーベジ・コレクション
//---------------------------------------------------------------------------
#ifndef _RisaGCH_
#define _RisaGCH_

#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"
#include <wx/fileconf.h>


namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		ガーベジコレクタ用スレッド
//---------------------------------------------------------------------------
class tRisaCollectorThread : public singleton_base<tRisaCollectorThread>
{
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	//! @brief	コレクタスレッドのクラス
	class tThread : public tRisaThread
	{
		tRisaThreadEvent Event; //!< イベントオブジェクト
		tRisaCollectorThread & Owner;
	public:

		//! @brief		コンストラクタ
		//! @param		owner		このオブジェクトを所有する tRisaCollectorThread オブジェクト
		tThread(tRisaCollectorThread & owner);

		//! @brief		デストラクタ
		~tThread();

		//! @brief		スレッドのエントリーポイント
		void Execute();

		//! @brief		スレッドをたたき起こす
		void Wakeup() { Event.Signal(); }
	};

	tThread *Thread; //!< コレクタスレッド

public:
	//! @brief		コンストラクタ
	tRisaCollectorThread();

	//! @brief		デストラクタ
	~tRisaCollectorThread();

private:
	//! @brief		ファイナライズすべきオブジェクトがあった場合に
	//!				GC から呼ばれるコールバック
	static void FinalizerNotifier();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


#endif
