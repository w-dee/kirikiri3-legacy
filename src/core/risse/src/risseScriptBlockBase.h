//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトブロック管理
//---------------------------------------------------------------------------
#ifndef risseScriptBlockBaseH
#define risseScriptBlockBaseH

#include "risseTypes.h"
#include "risseCxxString.h"
#include "risseVariant.h"
#include "risseAssert.h"
#include "risseGC.h"

namespace Risse
{
class tRisseASTNode;
class tRisseSSAForm;
//---------------------------------------------------------------------------
//! @brief		スクリプトブロックの基底クラス
//---------------------------------------------------------------------------
class tRisseScriptBlockBase : public tRisseCollectee
{
private:
	tRisseString Script; //!< スクリプトの内容
	tRisseString Name; //!< スクリプトブロックの名称(たいていはファイル名)
	risse_size LineOffset; //!< スクリプトの行オフセット (ドキュメント埋め込みスクリプト用)
	mutable risse_size * LinesToPosition; //!< 各行の先頭に対応するコードポイント位置の配列
	mutable risse_size LineCount; //!< スクリプトの行数


protected:
	//! @brief		コンストラクタ
	//! @param		script		スクリプトの内容
	//! @param		name		スクリプトブロックの名称
	//! @param		lineofs		行オフセット(ドキュメント埋め込みスクリプト用に、
	//!							スクリプトのオフセットを記録できる)
	tRisseScriptBlockBase(const tRisseString & script, const tRisseString & name,
							risse_size lineofs = 0);

	//! @brief		デストラクタ
	virtual ~tRisseScriptBlockBase() {;}

	//! @brief		LinesToPosition の内容を作成する
	void CreateLinesToPositionArary() const;

	//! @brief		LinesToPosition の内容が作成されていなければ作成する
	void EnsureLinesToPositionArary() const
	{ if(!LinesToPosition) CreateLinesToPositionArary(); }

public:
	//! @brief		スクリプトの内容を得る	@return スクリプトの内容
	const tRisseString & GetScript() const { return Script; }

	//! @brief		スクリプトブロックの名称を得る	@return スクリプトブロックの名称
	const tRisseString & GetName() const { return Name; }

	//! @brief		スクリプトのコードポイント位置から行/桁位置への変換を行う
	//! @param		pos			コードポイント位置
	//! @param		line		行位置(0～; 興味がない場合はnull可)
	//! @param		col			桁位置(0～; 興味がない場合はnull可)
	void PositionToLineAndColumn(risse_size pos, risse_size *line, risse_size *col) const;

public:
	//! @brief		スクリプトを評価する
	//! @param		result			実行の結果(NULL可)
	//! @param		is_expression	式評価モードかどうか
	//! @note		もしスクリプトがコンパイルが必要な場合、
	//!				Evaluate は評価に先立って Compile() を呼び、コンパイルを行う。
	virtual void Evaluate(tRisseVariant * result = NULL, bool is_expression = false) = 0;


	//=======================================================================
	// ここから下はコンパイルに関連したもの
	// (もしかしたらこのクラスからは分離するかも)
	// (ここのクラスの宣言直前の forward declaration にも注意)
	gc_vector<tRisseSSAForm *> SSAForms; // このブロックが保持している SSA 形式インスタンスのリスト

protected:
	//! @brief		ASTを元にコンパイルを行う
	//! @param		root		ルートASTノード
	//! @param		need_result		評価時に結果が必要かどうか
	//! @param		is_expression	式評価モードかどうか
	void Compile(tRisseASTNode * root, bool need_result, bool is_expression);

public:
	//! @brief		SSA形式インスタンスを追加する
	//! @param		ssaform		SSA形式インスタンス
	void AddSSAForm(tRisseSSAForm * ssaform);
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

