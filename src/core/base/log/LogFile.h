//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログファイル
//---------------------------------------------------------------------------
#ifndef _LOGFILE_H
#define _LOGFILE_H


/*!
@brief
	tLogFile はシングルトンオブジェクトとしてシステム内に常駐するが、
	実際にファイルにログを採るようになるのは Begin メソッドを呼んだあとだけ
	となる。
*/



#include "base/utils/Singleton.h"
#include "base/log/Log.h"
#include "base/utils/RisaThread.h"
#include <deque>
#include <wx/datetime.h>
#include <wx/file.h>

namespace Risa {
//---------------------------------------------------------------------------

class tLogReceiver;

//---------------------------------------------------------------------------
//! @brief		ログファイルクラス(シングルトン)
//---------------------------------------------------------------------------
class tLogFile : public singleton_base<tLogFile>, protected depends_on<tLogger>
{
	static const size_t NumLastLog  = 50; //!< ファイルへのログ記録を開始した際に出力する LastLog の行数

	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	//! @brief ログを受け取るためのレシーバークラス
	class tReceiver : public tLogReceiver
	{
		tLogFile & Owner; //!< tLogFile へのポインタ
	public:
		tReceiver(tLogFile & owner) : Owner(owner) {;} //!< コンストラクタ
		//! @brief ログアイテムを記録するとき
		void OnLog(const tLogger::tItem & item)
		{
			Owner.OnLog(item);
		}
	};
	tReceiver Receiver; //!< レシーバオブジェクト

public:
	wxFile LogFile; //!< ログファイル

public:
	//! @brief		コンストラクタ
	tLogFile();

	//! @brief		デストラクタ
	~tLogFile();

private:
	//! @brief		行を一行出力する
	//! @param		str  行 (EOLはこのメソッドが自動的に出力する)
	void OutputOneLine(const tString & str);

	//! @brief		ログが追加されるとき
	//! @param		item  ログアイテム
	void OnLog(const tLogger::tItem & item);

public:
	//! @brief		ログのファイルへの記録を開始する
	void Begin();
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa



#endif
