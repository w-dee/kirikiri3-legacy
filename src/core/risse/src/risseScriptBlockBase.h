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
//---------------------------------------------------------------------------
//! @brief		スクリプトブロックの基底クラス
//---------------------------------------------------------------------------
class tRisseScriptBlockBase : public tRisseCollectee
{
private:
	tRisseString Script; //!< スクリプトの内容
	risse_size LineOffset; //!< スクリプトの行オフセット (ドキュメント埋め込みスクリプト用)
	mutable risse_size * LinesToPosition; //!< 各行の先頭に対応するコードポイント位置の配列
	mutable risse_size LineCount; //!< スクリプトの行数

protected:
	//! @brief		コンストラクタ
	//! @param		script		スクリプトの内容
	//! @param		lineofs		行オフセット(ドキュメント埋め込みスクリプト用に、
	//!							スクリプトのオフセットを記録できる)
	tRisseScriptBlockBase(const tRisseString & script, risse_size lineofs = 0);

	//! @brief		LinesToPosition の内容を作成する
	void CreateLinesToPositionArary() const;

	//! @brief		LinesToPosition の内容が作成されていなければ作成する
	void EnsureLinesToPositionArary() const
	{ if(!LinesToPosition) CreateLinesToPositionArary(); }

public:
	//! @brief		スクリプトの内容を得る
	const tRisseString & GetScript() const { return Script; }

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

protected:
	//! @brief		ASTを元にコンパイルを行う
	//! @param		root		ルートASTノード
	//! @param		need_result		評価時に結果が必要かどうか
	//! @param		is_expression	式評価モードかどうか
	void Compile(tRisseASTNode * root, bool need_result, bool is_expression);

};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

