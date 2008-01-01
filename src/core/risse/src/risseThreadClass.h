//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Thread" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseThreadClassH
#define risseThreadClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"

namespace Risse
{
class tScriptThread;
//---------------------------------------------------------------------------
//! @brief		"Thread" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tThreadInstance : public tObjectBase
{
private:
	tScriptThread * Thread; //!< スレッドの実装

	tVariant Method; //!< 実行するメソッド
	tVariant Context; //!< 実行するメソッドのコンテキスト
	tVariant Ret; //!< Execute メソッドの戻り値
	const tVariant *Exception; //!< 例外が発生した場合の例外オブジェクト

public:
	//! @brief		コンストラクタ
	tThreadInstance();

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tThreadInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo & info);
	void run() const;
	void start();
	tVariant join() const;
	bool sleep(risse_int64 timeout);
	void wakeup();

	// friend 指定
	friend class tScriptThread;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Thread" クラス
//---------------------------------------------------------------------------
class tThreadClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tThreadClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
