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
class tRisseCompilerFunctionGroup;
//---------------------------------------------------------------------------
//! @brief		関数クラス
//---------------------------------------------------------------------------
class tRisseCompilerFunction : public tRisseCollectee
{
	tRisseCompilerFunctionGroup * FunctionGroup; //!< この関数を保持している関数グループクラスのインスタンス
	gc_vector<tRisseSSAForm *> SSAForms; //!< この関数が保持しているSSA形式のリスト

public:
	//! @brief		コンストラクタ
	//! @param		function_group		関数グループクラスのインスタンス
	tRisseCompilerFunction(tRisseCompilerFunctionGroup * function_group);

	//! @brief		関数グループクラスのインスタンスを得る
	//! @return		関数グループクラスのインスタンス
	tRisseCompilerFunctionGroup * GetFunctionGroup() const { return FunctionGroup; }

	//! @brief		SSA形式インスタンスを追加する
	//! @param		form		SSA形式インスタンス
	void AddSSAForm(tRisseSSAForm * form);

	//! @brief		SSA形式を完結させる
	//! @note		このメソッドは、一通りASTからの変換が終わった後に呼ばれる。
	//!				未バインドのラベルなどのバインドを行う。
	void CompleteSSAForm();

	//! @brief		VMコード生成を行う
	void GenerateVMCode();

};
//---------------------------------------------------------------------------


class tRisseCompiler;
class tRisseCompilerFunction;
//---------------------------------------------------------------------------
//! @brief		関数グループクラス
//---------------------------------------------------------------------------
class tRisseCompilerFunctionGroup : public tRisseCollectee
{
	tRisseCompiler * Compiler; //!< この関数グループを保持しているコンパイラクラスのインスタンス
	gc_vector<tRisseCompilerFunction *> Functions; //!< この関数グループが保持している関数インスタンスのリスト

public:
	//! @brief		コンストラクタ
	//! @param		compiler		コンパイラクラスのインスタンス
	tRisseCompilerFunctionGroup(tRisseCompiler * compiler);

	//! @brief		コンパイラクラスのインスタンスを得る
	//! @param		コンパイラクラスのインスタンス
	tRisseCompiler * GetCompiler() const { return Compiler; }

	//! @brief		関数インスタンスを追加する
	//! @param		function		関数インスタンス
	void AddFunction(tRisseCompilerFunction * function);

	//! @brief		SSA形式を完結させる
	//! @note		このメソッドは、一通りASTからの変換が終わった後に呼ばれる。
	//!				未バインドのラベルなどのバインドを行う。
	void CompleteSSAForm();

	//! @brief		VMコード生成を行う
	void GenerateVMCode();

};
//---------------------------------------------------------------------------


class tRisseCodeBlock;
//---------------------------------------------------------------------------
//! @brief		コンパイラクラス
//---------------------------------------------------------------------------
class tRisseCompiler : public tRisseCollectee
{
	tRisseScriptBlockBase * ScriptBlock; //!< このコンパイラを保有しているスクリプトブロック
	gc_vector<tRisseCompilerFunctionGroup *> FunctionGroups;
		//!< この関数グループが保持している関数グループインスタンスのリスト

public:
	//! @brief		コンストラクタ
	//! @param		scriptblock		スクリプトブロックインスタンス
	tRisseCompiler(tRisseScriptBlockBase * scriptblock) { ScriptBlock = scriptblock; }

	//! @brief		スクリプトブロックインスタンスを得る
	//! @return		スクリプトブロックインスタンス
	tRisseScriptBlockBase * GetScriptBlock() const { return ScriptBlock; }

	//! @brief		ASTを元にコンパイルを行う
	//! @param		root		ルートASTノード
	//! @param		need_result		評価時に結果が必要かどうか
	//! @param		is_expression	式評価モードかどうか
	void Compile(tRisseASTNode * root, bool need_result, bool is_expression);

	//! @brief		関数グループインスタンスを追加する
	//! @param		function_group		関数グループインスタンス
	void AddFunctionGroup(tRisseCompilerFunctionGroup * function_group);

	//! @brief		コードブロックを追加する
	//! @param		block		コードブロック
	//! @return		コードブロックのインデックス
	risse_size AddCodeBlock(tRisseCodeBlock * block);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

