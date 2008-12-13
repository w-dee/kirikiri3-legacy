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

#include "../../risseObject.h"
#include "../../risseClass.h"
#include "../../risseGC.h"
#include "../risseBuiltinPackageInitializer.h"

namespace Risse
{
class tScriptThread;
//---------------------------------------------------------------------------
/**
 * "Thread" クラスのインスタンス用 C++クラス
 */
class tThreadInstance : public tObjectBase
{
private:
	tScriptThread * Thread; //!< スレッドの実装

	tVariant Method; //!< 実行するメソッド
	tVariant Context; //!< 実行するメソッドのコンテキスト
	tVariant Ret; //!< Execute メソッドの戻り値
	const tVariant *Exception; //!< 例外が発生した場合の例外オブジェクト

public:
	/**
	 * コンストラクタ
	 */
	tThreadInstance();

	/**
	 * ダミーのデストラクタ(おそらく呼ばれない)
	 */
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
/**
 * "Thread" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tThreadClass, tClassBase, tThreadInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * "thread" パッケージイニシャライザ
 */
class tThreadPackageInitializer : public tBuiltinPackageInitializer
{
public:
	tThreadClass * ThreadClass;

	/**
	 * コンストラクタ
	 */
	tThreadPackageInitializer();

	/**
	 * パッケージを初期化する
	 * @param engine	スクリプトエンジンインスタンス
	 * @param name		パッケージ名
	 * @param global	パッケージグローバル
	 */
	virtual void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global);
};
//---------------------------------------------------------------------------




} // namespace Risse


#endif
