//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイトコード実行クラスのインターフェースとインタプリタの実装
//---------------------------------------------------------------------------
#ifndef risseCodeExecutorH
#define risseCodeExecutorH

#include "risseGC.h"
#include "risseCodeBlock.h"
#include "risseObject.h"

//---------------------------------------------------------------------------
namespace Risse
{
class tCodeBlock;
class tScriptEngine;
//---------------------------------------------------------------------------
/**
 * バイトコード実行クラスの基底クラス
 */
class tCodeExecutor : public tCollectee
{
protected:
	tCodeBlock * CodeBlock; //!< コードブロック

public:
	/**
	 * コンストラクタ
	 * @param cb	コードブロック
	 */
	tCodeExecutor(tCodeBlock *cb) { CodeBlock = cb; }

	/**
	 * デストラクタ
	 * @note	このデストラクタは呼ばれないかもしれない
	 */
	virtual ~tCodeExecutor() {;}

	/**
	 * コードを実行する
	 * args	引数
	 * @param global	メソッドが実行されるべきパッケージグローバル
	 *					(NULL=パッケージグローバルを指定しない場合)
	 * @param This		メソッドが実行されるべき"Thisオブジェクト"
	 *					(NULL="Thisオブジェクト"を指定しない場合)
	 * @param frame		メソッドが実行されるべきスタックフレーム
	 *					(NULL=スタックフレームを指定しない場合)
	 * @param shared	メソッドが実行されるべき共有フレーム
	 *					(NULL=共有フレームを指定しない場合)
	 * @param result	戻りの値を格納する先
	 */
	virtual void Execute(
		const tMethodArgument & args = tMethodArgument::Empty(),
		const tVariant & global = tVariant::GetNullObject(),
		const tVariant & This = tVariant::GetNullObject(),
		tVariant * frame = NULL, tSharedVariableFrames * shared = NULL,
		tVariant * result = NULL) = 0;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * バイトコードインタプリタ
 */
class tCodeInterpreter : public tCodeExecutor
{
public:
	/**
	 * コンストラクタ
	 * @param cb	コードブロック
	 */
	tCodeInterpreter(tCodeBlock *cb);


	void Execute(
		const tMethodArgument & args = tMethodArgument::Empty(),
		const tVariant & global = tVariant::GetNullObject(),
		const tVariant & This = tVariant::GetNullObject(),
		tVariant * frame = NULL, tSharedVariableFrames * shared = NULL,
		tVariant * result = NULL);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * ocTryFuncCall の戻り用オブジェクト
 * @note	このオブジェクトはocTryFuncCallとocCatchBranchの間のみ存在する
 *			(ocCatchBranchはこのオブジェクトが入ったレジスタをocTryFuncCall
 *			の戻り値で置き換えてしまう)
 */
class tTryFuncCallReturnObject : public tObjectInterface
{
private:
	tVariant Value; //!< 例外/戻り値の値
	bool Raised; // 例外が発生したかどうか

public:
	/**
	 * コンストラクタ
	 * @param value		例外/戻り値の値
	 * @param raised	例外が発生したかどうか
	 */
	tTryFuncCallReturnObject(const tVariant &value, bool raised)
		{ Value = value; Raised = raised; }

	/**
	 * デストラクタ
	 * @note	このデストラクタは呼ばれないかもしれない
	 */
	virtual ~tTryFuncCallReturnObject() {;}

	/**
	 * 例外/戻り値の値を得る
	 * @return	例外/戻り値の値
	 */
	const tVariant & GetValue() const { return Value; }

	/**
	 * 例外が発生したかどうかを得る
	 * @return	例外が発生したかどうか
	 */
	bool GetRaised() const { return Raised; }

private:
	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG) {return rvNoError;}
};
//---------------------------------------------------------------------------


} // namespace Risse
#endif

