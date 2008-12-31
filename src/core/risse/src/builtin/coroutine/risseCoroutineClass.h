//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Coroutine" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseCoroutineClassH
#define risseCoroutineClassH

#include "../../risseObject.h"
#include "../../risseClass.h"
#include "../../risseGC.h"
#include "../risseBuiltinPackageInitializer.h"
#include "../../risseExceptionClass.h"

namespace Risse
{
class tCoroutine;
//---------------------------------------------------------------------------
/**
 * "Coroutine" クラスのインスタンス用 C++クラス
 */
class tCoroutineInstance : public tObjectBase
{
private:
	tCoroutine * Coroutine; //!< コルーチンの実装

public:
	/**
	 * コンストラクタ
	 */
	tCoroutineInstance();

	/**
	 * ダミーのデストラクタ(おそらく呼ばれない)
	 */
	virtual ~tCoroutineInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo & info);
	void run() const;
	tVariant resume(const tMethodArgument & args) const;
	tVariant yield(const tMethodArgument & args) const;
	void dispose() const;
	bool get_alive() const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * "Coroutine" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tCoroutineClass, tClassBase, tCoroutineInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * "CoroutineException" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tCoroutineExceptionClass, tClassBase, tObjectBase, itNormal)
public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	/**
	 * 「コルーチンは既に終了している」例外を発生
	 * @param engine	スクリプトエンジンインスタンス
	 */
	static void ThrowCoroutineHasAlreadyExited(tScriptEngine * engine) RISSE_NORETURN;
	/**
	 * 「コルーチンは既に終了している」例外を発生
	 * @param engine	スクリプトエンジンインスタンス
	 */
	static void ThrowCoroutineHasAlreadyExited() RISSE_NORETURN
		{ ThrowCoroutineHasAlreadyExited(NULL); }

	/**
	 * 「コルーチンはまだ開始していない」例外を発生
	 * @param engine	スクリプトエンジンインスタンス
	 */
	static void ThrowCoroutineHasNotStartedYet(tScriptEngine * engine) RISSE_NORETURN;
	/**
	 * 「コルーチンはまだ開始していない」例外を発生
	 * @param engine	スクリプトエンジンインスタンス
	 */
	static void ThrowCoroutineHasNotStartedYet() RISSE_NORETURN
		{ ThrowCoroutineHasNotStartedYet(NULL); }

	/**
	 * 「コルーチンは実行中でない」例外を発生
	 * @param engine	スクリプトエンジンインスタンス
	 */
	static void ThrowCoroutineIsNotRunning(tScriptEngine * engine) RISSE_NORETURN;
	/**
	 * 「コルーチンは実行中でない」例外を発生
	 * @param engine	スクリプトエンジンインスタンス
	 */
	static void ThrowCoroutineIsNotRunning() RISSE_NORETURN
		{ ThrowCoroutineIsNotRunning(NULL); }

	/**
	 * 「コルーチンは実行中」例外を発生
	 * @param engine	スクリプトエンジンインスタンス
	 */
	static void ThrowCoroutineIsRunning(tScriptEngine * engine) RISSE_NORETURN;
	/**
	 * 「コルーチンは実行中」例外を発生
	 * @param engine	スクリプトエンジンインスタンス
	 */
	static void ThrowCoroutineIsRunning() RISSE_NORETURN
		{ ThrowCoroutineIsRunning(NULL); }
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * "coroutine" パッケージイニシャライザ
 */
class tCoroutinePackageInitializer : public tBuiltinPackageInitializer
{
public:
	tCoroutineClass * CoroutineClass;
	tCoroutineExceptionClass * CoroutineExceptionClass;

	/**
	 * コンストラクタ
	 */
	tCoroutinePackageInitializer();

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
