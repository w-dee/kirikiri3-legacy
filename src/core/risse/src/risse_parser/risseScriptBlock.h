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

	//! @brief		AST のルートノードを取得する(下位クラスで実装すること)
	//! @param		need_result		結果が必要かどうか
	//! @param		is_expression	式モードかどうか
	//! @return		AST ルートノード
	tRisseASTNode * GetASTRootNode(bool need_result = false, bool is_expression = false);
};
//---------------------------------------------------------------------------

} // namespace Risse

#endif

