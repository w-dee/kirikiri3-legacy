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
#include "risseAST.h"

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

// 名前表の読み込み
#undef risseASTH
#define RISSE_AST_DEFINE_NAMES
#include "risseAST.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(29091,1243,20617,17999,61570,21800,19479,2186);

//---------------------------------------------------------------------------
void tRisseASTNode::Dump(tRisseString & result, risse_int level)
{
	result += RisseASTNodeTypeNames[Type];
	tRisseString comment = GetDumpComment();
	if(!comment.IsEmpty())
		result += tRisseString(RISSE_WS(" (%1)"), comment);
#ifdef RISSE_TEXT_OUT_CRLF
	result += RISSE_WS("\r\n");
#else
	result += RISSE_WS("\n");
#endif
	GetDumpChildren();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseASTNode::AddDumpChild(tRisseString & result, risse_int level,
		const tRisseString & name, tRisseASTNode * child)
{
	level ++;
	result += tRisseString(RISSE_WS(" ")).Times(level) + name + RISSE_WS(" ");
	child->Dump(result, level); // 再帰
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tRisseASTNode_List::GetDumpChildren(tRisseString & result, risse_int level)
{
	risse_size count = 0;
	for(gc_vector<tRisseASTNode *>::iterator i = Array.begin();
		i != Array.end(); i++, count ++)
	{
		risse_char buf[40];
		AddDumpChild(result, level,
			tRisseString(RISSE_WS("node")) + Risse_int64_to_str(count), *i);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Context::GetDumpComment()
{
	return RisseASTContextTypeNames[ContextType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseASTNode_ExprStmt::GetDumpChildren(tRisseString & result, risse_int level)
{
	AddDumpChild(result, level,
		tRisseString(RISSE_WS("expression")), Expression);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Factor::GetDumpComment()
{
	tRisseString ret = RisseASTFactorTypeNames[FactorType];
	if(FactorType == aftConstant)
	{
		ret += RISSE_WS(" ");
		ret += Value.AsHumanReadable();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseASTNode_Unary::GetDumpChildren(tRisseString & result, risse_int level)
{
	AddDumpChild(result, level,
		tRisseString(RISSE_WS("child")), Child);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Unary::GetDumpComment()
{
	return RisseASTUnaryTypeNames[UnaryType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseASTNode_Binary::GetDumpChildren(tRisseString & result, risse_int level)
{
	AddDumpChild(result, level,
		tRisseString(RISSE_WS("child1")), Child1);
	AddDumpChild(result, level,
		tRisseString(RISSE_WS("child2")), Child2);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Binary::GetDumpComment()
{
	return RisseASTBinaryTypeNames[BinaryType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseASTNode_Trinary::GetDumpChildren(tRisseString & result, risse_int level)
{
	AddDumpChild(result, level,
		tRisseString(RISSE_WS("child1")), Child1);
	AddDumpChild(result, level,
		tRisseString(RISSE_WS("child2")), Child2);
	AddDumpChild(result, level,
		tRisseString(RISSE_WS("child3")), Child3);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Trinary::GetDumpComment()
{
	return RisseASTTrinaryTypeNames[TrinaryType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
