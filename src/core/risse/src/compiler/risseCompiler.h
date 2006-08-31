//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンパイラ
//---------------------------------------------------------------------------
#ifndef risseCompilerH
#define risseCompilerH

#include "../risseGC.h"
#include "../risseCharUtils.h"
#include "../risseTypes.h"
#include "risseAST.h"
#include "../risseVariant.h"
#include "../risseOpCodes.h"

//---------------------------------------------------------------------------
/*
	risseでは、ASTに対して、一度 SSA 形式ライクな中間形式に変換を行ってから、
	それを VM バイトコードに変換を行う。
*/
//---------------------------------------------------------------------------
namespace Risse
{
class tRisseCodeBlock;
//---------------------------------------------------------------------------
//! @brief		コンパイラクラス
//---------------------------------------------------------------------------
class tRisseCompiler : public tRisseCollectee
{
	tRisseScriptBlockBase * ScriptBlock; //!< このコンパイラを保有しているスクリプトブロック
	gc_vector<tRisseSSAForm *> SSAForms; //!< このブロックが保持している SSA 形式インスタンスのリスト

public:
	//! @brief		コンストラクタ
	//! @paramo		scriptblock		スクリプトブロックインスタンス
	tRisseCompiler(tRisseScriptBlockBase * scriptblock) { ScriptBlock = scriptblock; }

	//! @brief		スクリプトブロックインスタンスを得る
	//! @return		スクリプトブロックインスタンス
	tRisseScriptBlockBase * GetScriptBlock() const { return ScriptBlock; }

	//! @brief		ASTを元にコンパイルを行う
	//! @param		root		ルートASTノード
	//! @param		need_result		評価時に結果が必要かどうか
	//! @param		is_expression	式評価モードかどうか
	void Compile(tRisseASTNode * root, bool need_result, bool is_expression);

	//! @brief		SSA形式インスタンスを追加する
	//! @param		ssaform		SSA形式インスタンス
	void AddSSAForm(tRisseSSAForm * ssaform);

	//! @brief		コードブロックを追加する
	//! @param		block		コードブロック
	//! @return		コードブロックのインデックス
	risse_size AddCodeBlock(tRisseCodeBlock * block);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

