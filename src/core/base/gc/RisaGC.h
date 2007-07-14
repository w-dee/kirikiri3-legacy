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
#ifndef _GCH_
#define _GCH_

#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"
#include <wx/fileconf.h>


namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		ガーベジコレクタ用スレッド
//---------------------------------------------------------------------------
class tCollectorThread : public singleton_base<tCollectorThread>
{
	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	//! @brief	コレクタスレッドのクラス
	class tThreadImpl : public tThread
	{
		tThreadEvent Event; //!< イベントオブジェクト
		tCollectorThread & Owner;
	public:

		//! @brief		コンストラクタ
		//! @param		owner		このオブジェクトを所有する tCollectorThread オブジェクト
		tThreadImpl(tCollectorThread & owner);

		//! @brief		デストラクタ
		~tThreadImpl();

		//! @brief		スレッドのエントリーポイント
		void Execute();

		//! @brief		スレッドをたたき起こす
		void Wakeup() { Event.Signal(); }
	};

	tThreadImpl *Thread; //!< コレクタスレッド

public:
	//! @brief		コンストラクタ
	tCollectorThread();

	//! @brief		デストラクタ
	~tCollectorThread();

private:
	//! @brief		ファイナライズすべきオブジェクトがあった場合に
	//!				GC から呼ばれるコールバック
	static void FinalizerNotifier();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


#endif
