//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief AST(抽象構文木) の操作
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseCxxObject.h"

/*
ダンプ出力例

Context (TopLevel)
　ExprStmt
　　Binary (Assign)
　　　Factor (Symbol)
　　　Binary (Add)
　　　　Factor (Symbol)
　　　　Factor (Constant, value=5)
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(29091,1243,20617,17999,61570,21800,19479,2186);

//---------------------------------------------------------------------------
void tRisseASTNode::Dump(risse_int level)
{
	
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
