//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログファイル
//---------------------------------------------------------------------------
#ifndef _LOGFILE_H
#define _LOGFILE_H

#include "risse/include/risse.h"
#include "base/utils/Singleton.h"
#include "base/log/Log.h"
#include <deque>
#include <wx/datetime.h>
#include <wx/file.h>


class tRisaLogReceiver;

//---------------------------------------------------------------------------
//! @brief		ログファイルクラス(シングルトン)
//---------------------------------------------------------------------------
class tRisaLogFile
{
	tRisseCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	//! @brief ログを受け取るためのレシーバークラス
	class tReceiver : public tRisaLogReceiver
	{
		tRisaLogFile * Owner; //!< tRisaLogFile へのポインタ
		tReceiver(tRisaLogFile * owner) : Owner(owner) {;} //!< コンストラクタ
		//! @brief ログアイテムを記録するとき
		void OnLog(const tRisaLogger::tItem & item)
		{
			Owner->OnLog(item);
		}
	};

public:
	wxFile LogFile; //!< ログファイル

public:
	tRisaLogFile();
	~tRisaLogFile();

private:
	void OnLog(const tRisaLogger::tItem & item);

private:
	tRisaSingleton<tRisaLogger> ref_tRisaLogger; //!< tRisaLogger に依存

	tRisaSingletonObjectLifeTracer<tRisaLogFile> singleton_object_life_tracer;

public:
	static boost::shared_ptr<tRisaLogFile> & instance() { return
		tRisaSingleton<tRisaLogFile>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------







#endif
//---------------------------------------------------------------------------
