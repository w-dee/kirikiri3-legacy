//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
class tRisseCodeBlock;
class tRisseObjectInterface;
class tRisseExecutorContext;
//---------------------------------------------------------------------------
//! @brief		バイトコード実行クラスの基底クラス
//---------------------------------------------------------------------------
class tRisseCodeExecutor : public tRisseCollectee
{
protected:
	tRisseCodeBlock * CodeBlock; //!< コードブロック

public:
	//! @brief		コンストラクタ
	//! @param		cb		コードブロック
	tRisseCodeExecutor(tRisseCodeBlock *cb) { CodeBlock = cb; }

	//! @brief		デストラクタ
	//! @note		このデストラクタは呼ばれないかもしれない
	virtual ~tRisseCodeExecutor() {;}

	//! @brief		コードを実行する
	//! @param		context		実行コンテキスト情報
	virtual void Execute(tRisseExecutorContext * context) = 0;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		バイトコードインタプリタ
//---------------------------------------------------------------------------
class tRisseCodeInterpreter : public tRisseCodeExecutor
{
	//! @brief	実行状態保持用の構造体
	struct tState
	{
		tRisseVariant * Frame; //!< スタックフレーム
		tRisseVariant * Shared; //!< 共有変数
		tRisseVariant This; //!< This
	};

public:
	//! @brief		コンストラクタ
	//! @param		cb		コードブロック
	tRisseCodeInterpreter(tRisseCodeBlock *cb);


	void Execute(tRisseExecutorContext * context);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ocTryFuncCall の戻り用オブジェクト
//! @note		このオブジェクトはocTryFuncCallとocCatchBranchの間のみ存在する
//!				(ocCatchBranchはこのオブジェクトが入ったレジスタをocTryFuncCall
//!				の戻り値で置き換えてしまう)
//---------------------------------------------------------------------------
class tRisseTryFuncCallReturnObject : public tRisseObjectInterface
{
private:
	tRisseVariant Value; //!< 例外/戻り値の値
	bool Raised; // 例外が発生したかどうか

public:
	//! @brief		コンストラクタ
	//! @param		value		例外/戻り値の値
	//! @param		raised		例外が発生したかどうか
	tRisseTryFuncCallReturnObject(const tRisseVariant &value, bool raised)
		{ Value = value; Raised = raised; }

	//! @brief		例外/戻り値の値を得る
	//! @return		例外/戻り値の値
	const tRisseVariant & GetValue() const { return Value; }

	//! @brief		例外が発生したかどうかを得る
	//! @return		例外が発生したかどうか
	bool GetRaised() const { return Raised; }

private:
	void Operate(tRisseExecutorContext * context) {;}
};
//---------------------------------------------------------------------------


} // namespace Risse
#endif

