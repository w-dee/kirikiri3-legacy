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

//---------------------------------------------------------------------------
namespace Risse
{
class tRisseCodeBlock;
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
	//! @param		this		this として渡すオブジェクト
	//! @param		result		戻りの値を格納する先
	virtual void Execute(const tRisseVariant & this_obj, tRisseVariant * result = NULL) = 0;
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


	//! @brief		コードを実行する
	//! @param		this		this として渡すオブジェクト
	//! @param		result		戻りの値を格納する先
	void Execute(const tRisseVariant & this_obj, tRisseVariant * result = NULL);
};
//---------------------------------------------------------------------------

} // namespace Risse
#endif

