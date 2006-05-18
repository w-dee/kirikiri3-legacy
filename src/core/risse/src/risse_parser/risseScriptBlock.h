//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse スクリプトブロックの実装
//---------------------------------------------------------------------------
#ifndef RisseScriptBlockH
#define RisseScriptBlockH

#include "../risseScriptBlockBase.h"

namespace Risse
{

//---------------------------------------------------------------------------
class tRisseScriptBlock : public tRisseScriptBlockBase
{
public:
	//! @brief		コンストラクタ
	//! @param		script		スクリプトの内容
	//! @param		name		スクリプトブロックの名称
	//! @param		lineofs		行オフセット(ドキュメント埋め込みスクリプト用に、
	//!							スクリプトのオフセットを記録できる)
	tRisseScriptBlock(const tRisseString & script,
						const tRisseString & name, risse_size lineofs = 0);

	//! @brief		スクリプトを評価する
	//! @param		result			実行の結果(NULL可)
	//! @param		is_expression	式評価モードかどうか
	//! @note		もしスクリプトがコンパイルが必要な場合、
	//!				Evaluate は評価に先立って Compile() を呼び、コンパイルを行う。
	virtual void Evaluate(tRisseVariant * result = NULL, bool is_expression = false);
};
//---------------------------------------------------------------------------

} // namespace Risse

#endif

