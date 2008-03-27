//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コルーチンの実装
//---------------------------------------------------------------------------
#ifndef risseCoroutineH
#define risseCoroutineH

#include "../../risseConfig.h"
#include "../../risseThread.h"
#include "../../risseGC.h"
#include "../../risseVariant.h"

// #define RISSE_CORO_DEBUG
// #define RISSE_TRACK_FIBERS



namespace Risse
{


//---------------------------------------------------------------------------
//! @brief		コルーチンの初期化
//! @note		GC_init の直後に呼ばれる必要がある
//---------------------------------------------------------------------------
void InitCoroutine();
//---------------------------------------------------------------------------


class tCoroutineImpl;
class tCoroutinePtr;
//---------------------------------------------------------------------------
//! @brief		コルーチンの実装クラス
//! @note		コルーチンに関する実装はすべてこのクラス内で隠蔽することにする
//---------------------------------------------------------------------------
class tCoroutine : public tCollectee
{
	friend class tCoroutineImpl;

	tScriptEngine * Engine; //!< スクリプトエンジンインスタンス
	tCoroutinePtr * Ptr; //!< 実装クラス
	tVariant Function; //!< 呼び出し先の関数
	tVariant FunctionArg; //!< 呼び出し先関数の2番目の引数
	const tVariant * ExceptionValue; //!< コルーチン中で例外が発生した場合、その値

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		function	呼び出される関数
	//! @param		arg			呼び出される関数の2番目の引数
	//! @note		関数の引数は、1番目に初回の Resume 呼び出しの引数、2番目にargが
	//! 			渡される。
	tCoroutine(tScriptEngine * engine, const tVariant & function, const tVariant arg);

	//! @brief		呼び出し先の関数を設定する
	//! @param		function	呼び出し先の関数
	void SetFunction(const tVariant & function) { Function = function; }

	//! @brief		コルーチンを実行する
	//! @param		arg		呼び出される関数の２番目の引数、あるいは yield メソッドの
	//!						戻り値となる値
	//! @return		DoYield() メソッドの戻り値 (実行が終了した場合は void が帰る)
	tVariant Resume(const tVariant &arg);

	//! @brief		コルーチンからyieldする
	//! @param		arg		Resume() メソッドの戻り値となる値
	//! @return		Resume() メソッドの引数
	//! @note		なんか Yield がどこかでマクロとして定義されているらしいので DoYield にする
	tVariant DoYield(const tVariant &arg);

	//! @brief		コルーチンが生存しているかどうかを得る
	//! @param		コルーチンが生存しているかどうか
	bool GetAlive() const;

	//! @brief		コルーチンを破棄する
	void Dispose();

};
//---------------------------------------------------------------------------


} // namespace Risse

#endif

