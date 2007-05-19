//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コルーチンの実装
//---------------------------------------------------------------------------
#ifndef risseCoroutineH
#define risseCoroutineH

#include "risseConfig.h"
#include "risseUtils.h"
#include "risseGC.h"
#include "risseVariant.h"

namespace Risse
{


//---------------------------------------------------------------------------
//! @brief		コルーチンの初期化
//! @note		GC_init の直後に呼ばれる必要がある
//---------------------------------------------------------------------------
void RisseInitCoroutine();
//---------------------------------------------------------------------------


class tRisseCoroutineImpl;
//---------------------------------------------------------------------------
//! @brief		コルーチンの実装クラス
//! @note		コルーチンに関する実装はすべてこのクラス内で隠蔽することにする
//---------------------------------------------------------------------------
class tRisseCoroutine : public tRisseCollectee
{
	friend class tRisseCoroutineImpl;

	tRisseScriptEngine * Engine; //!< スクリプトエンジンインスタンス
	tRisseCoroutineImpl * Impl; //!< 実装クラス
	tRisseVariant Function; //!< 呼び出し先の関数
	tRisseVariant FunctionArg; //!< 呼び出し先関数の先頭の引数

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		function	呼び出される関数
	//! @param		arg			呼び出される関数の先頭の引数
	//! @note		関数の引数は、1番目にarg, 2番目に初回の Run 呼び出しの引数が
	//! 			渡される。
	tRisseCoroutine(tRisseScriptEngine * engine, const tRisseVariant & function, const tRisseVariant arg);

	//! @brief		コルーチンを実行する
	//! @param		arg		呼び出される関数の２番目の引数、あるいは yield メソッドの
	//!						戻り値となる値
	//! @return		DoYield() メソッドの戻り値 (実行が終了した場合は void が帰る)
	tRisseVariant Run(const tRisseVariant &arg);

	//! @brief		コルーチンからyieldする
	//! @param		arg		Run() メソッドの戻り値となる値
	//! @return		Run() メソッドの引数
	//! @note		なんか Yield がどこかでマクロとして定義されているらしいので DoYield にする
	tRisseVariant DoYield(const tRisseVariant &arg);

	//! @brief		コルーチンが生存しているかどうかを得る
	//! @param		コルーチンが生存しているかどうか
	bool GetAlive() const;

	//! @brief		コルーチンを破棄する
	void Exit();

};
//---------------------------------------------------------------------------


} // namespace Risse

#endif

