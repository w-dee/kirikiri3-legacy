//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 進捗コールバック用インターフェース
//---------------------------------------------------------------------------
#ifndef PROGRESSCALLBACK_H
#define PROGRESSCALLBACK_H

#include "risseVariant.h"

namespace Risa
{
//---------------------------------------------------------------------------
/**
 * 進捗コールバック用インターフェース
 */
class tProgressCallback : public tCollectee
{
	int Last; //!< 最後に送った進捗率

public:
	/**
	 * コンストラクタ
	 */
	tProgressCallback() : Last(-1) {}

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tProgressCallback() {}

	/**
	 * 進捗コールバックを呼ぶ
	 * @param total	全体の処理量
	 * @param done	終了した処理量
	 */
	void CallOnProgress(risse_size total, risse_size done)
	{
		int pct = 100 * done / total;
		if(Last != pct) OnProgress(pct);
		Last = pct;
	}

protected:
	/**
	 * 進捗コールバックを行う(下位クラスで実装すること)
	 * @param percent	進捗率(%)
	 * @note	処理を中断したい場合は例外を送出すること。
	 */
	virtual void OnProgress(int percent) = 0;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * risse メソッドを呼び出すようにした tProgressCallback の実装
 */
class tRisseProgressCallback : public tProgressCallback
{
	tScriptEngine * ScriptEngine; //!< Risseスクリプトエンジンインスタンス
	tVariant Method; //!< 呼び出すメソッド

public:
	/**
	 * コンストラクタ
	 * @param engine	Risseスクリプトエンジンインスタンス
	 * @param method	呼び出すメソッド
	 */
	tRisseProgressCallback(tScriptEngine * engine,
		const tVariant & method) :
			ScriptEngine(engine), Method(method)
	{}

protected:
	/**
	 * 進捗コールバックを行う
	 * @param percent	進捗率(%)
	 * @note	処理を中断したい場合は例外を送出すること。
	 */
	virtual void OnProgress(int percent)
	{
		Method.FuncCall(ScriptEngine, NULL, 0, tMethodArgument::New((risse_int64)percent));
	}
};
//---------------------------------------------------------------------------
}
#endif
