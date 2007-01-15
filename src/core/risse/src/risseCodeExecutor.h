//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
class tRisseScriptEngine;
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
	//! @brief		args	引数
	//! @brief		args	ブロック引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	//! @param		result		戻りの値を格納する先
	virtual void Execute(
		const tRisseMethodArgument & args = tRisseMethodArgument::Empty(),
		const tRisseMethodArgument & bargs = tRisseMethodArgument::Empty(),
		const tRisseVariant & This = tRisseVariant::GetNullObject(),
		const tRisseStackFrameContext &stack = tRisseStackFrameContext::GetNullContext(),
		tRisseVariant * result = NULL) = 0;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		バイトコードインタプリタ
//---------------------------------------------------------------------------
class tRisseCodeInterpreter : public tRisseCodeExecutor
{
public:
	//! @brief		コンストラクタ
	//! @param		cb		コードブロック
	tRisseCodeInterpreter(tRisseCodeBlock *cb);


	void Execute(
		const tRisseMethodArgument & args = tRisseMethodArgument::Empty(),
		const tRisseMethodArgument & bargs = tRisseMethodArgument::Empty(),
		const tRisseVariant & This = tRisseVariant::GetNullObject(),
		const tRisseStackFrameContext &stack = tRisseStackFrameContext::GetNullContext(),
		tRisseVariant * result = NULL);
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

	//! @brief		デストラクタ
	//! @note		このデストラクタは呼ばれないかもしれない
	virtual ~tRisseTryFuncCallReturnObject() {;}

	//! @brief		例外/戻り値の値を得る
	//! @return		例外/戻り値の値
	const tRisseVariant & GetValue() const { return Value; }

	//! @brief		例外が発生したかどうかを得る
	//! @return		例外が発生したかどうか
	bool GetRaised() const { return Raised; }

private:
	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG) {return rvNoError;}
};
//---------------------------------------------------------------------------


} // namespace Risse
#endif

