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

#ifndef risseASTH
#define risseASTH

#include "risseGC.h"
#include "risseVariant.h"


namespace Risse
{
//---------------------------------------------------------------------------
//! @brief	ASTノードのタイプ
//---------------------------------------------------------------------------
enum tRisseASTNodeType
{
	antContext,			//!< コンテキスト

	antExprStmt,		//!< 式のみのステートメント

	antFactor,			//!< 項
	antUnary,			//!< 単項演算子
	antBinary,			//!< 二項演算子
	antTrinary,			//!< 三項演算子
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コンテキストのタイプ
//---------------------------------------------------------------------------
enum tRisseASTContextType
{
	actTopLevel,		//!< トップレベル
	actClass,			//!< クラス
	actFunc				//!< 関数
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	項のタイプ
//---------------------------------------------------------------------------
enum tRisseASTFactorType
{
	aftConstant,		//!< 定数
	aftSymbol,			//!< シンボル
	aftThis,			//!< "this"
	aftSuper,			//!< "super"
	aftGlobal,			//!< "global"
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	単項演算子のタイプ
//---------------------------------------------------------------------------
enum tRisseASTUnaryType
{
	autLogNot,			//!< "!" logical not
	autBitNot,			//!< "~" bit not
	autPreDec,			//!< "--" pre-positioned decrement
	autPreInc,			//!< "++" pre-positioned increment
	autPostDec,			//!< "--" post-positioned decrement
	autPostInc,			//!< "++" post-positioned increment
	autNew,				//!< "new" (its child must be a function call node)
	autDelete,			//!< "delete"
	autPlus,			//!< "+"
	autMinus,			//!< "-"
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	二項演算子のタイプ
//---------------------------------------------------------------------------
enum tRisseASTBinaryType
{
	abtIf,				//!< if
	abtComma,			//!< ,
	abtAssign,			//!< =
	abtBitAndAssign,	//!< &=
	abtBitOrAssign,		//!< |=
	abtBitXorAssign,	//!< ^=
	abtSubAssign,		//!< -=
	abtAddAssign,		//!< +=
	abtModAssign,		//!< %=
	abtDivAssign,		//!< /=
	abtIdivAssign,		//!< \=
	abtMulAssign,		//!< *=
	abtLogOrAssign,		//!< ||=
	abtLogAndAssign,	//!< &&=
	abtRBitShiftAssign,	//!< >>>=
	abtLShiftAssign,	//!< <<=
	abtRShiftAssign,	//!< >>=
	abtLogOr,			//!< ||
	abtLogAnd,			//!< &&
	abtBitOr,			//!< |
	abtBitXor,			//!< ^
	abtBitAnd,			//!< &
	abtNotEqual,		//!< !=
	abtEqual,			//!< ==
	abtDiscNotEqual,	//!< !==
	abtDiscEqual,		//!< ===
	abtSwap,			//!< <->
	abtLesser,			//!< <
	abtGreater,			//!< >
	abtLesserOrEqual,	//!< <=
	abtGreaterOrEqual,	//!< >=
	abtRBitShift,		//!< >>>
	abtLShift,			//!< <<
	abtRShift,			//!< >>
	abtMod,				//!< %
	abtDiv,				//!< /
	abtIdiv,			//!< \ (integer div)
	abtMul,				//!< *
	abtAdd,				//!< +
	abtSub,				//!< -
	abtDirectSel,		//!< .
	abtIndirectSel,		//!< [ ]
	abtIncontextOf,		//!< incontextof
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	三項演算子のタイプ
//---------------------------------------------------------------------------
enum tRisseASTTrinaryType
{
	attCondition,		//!< ? :
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	ASTノードの基本クラス
//---------------------------------------------------------------------------
class tRisseASTNode : public tRisseCollectee
{
	risse_size Position; //!< ソースコード上の位置 (コードポイント数オフセット)
	tRisseASTNodeType Type; //!< ノードタイプ
	tRisseASTNode * Parent; //!< 親ノード

protected:
	//! @brief		コンストラクタ
	//! @brief		position	ソースコード上の位置
	//! @param		type		ノードタイプ
	tRisseASTNode(risse_size position, tRisseASTNodeType type) :
		Position(position), Type(type), Parent(NULL) {;}

public:
	//! @brief		親ノードを設定する
	//! @param		parent		親ノード
	void SetParent(tRisseASTNode * parent) { Parent = parent; }

	//! @brief		親ノードを得るする
	//! @return		親ノード
	tRisseASTNode * GetParent() const { return Parent; }

	//! @brief		ノードタイプを得る
	//! @return		ノードタイプ
	tRisseASTNodeType GetType() const { return Type; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	ASTノードの配列
//---------------------------------------------------------------------------
typedef gc_vector<tRisseASTNode *> tRisseASTArray;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	ノードの配列を伴うASTノードの基本クラス
//---------------------------------------------------------------------------
class tRisseASTNode_List : public tRisseASTNode
{
	tRisseASTArray Array; //!< 配列

protected:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		type			ノードタイプ
	tRisseASTNode_List(risse_size position, tRisseASTNodeType type) :
		tRisseASTNode(position, type) {;}

public:
	//! @brief		配列オブジェクトを得る
	//! @return		配列オブジェクト
	const tRisseASTArray & GetArray() const { return Array; }

	//! @brief		配列に子ノードを追加する
	//! @param		node		追加したいノード
	void AddChild(tRisseASTNode * node) { Array.push_back(node); node->SetParent(this); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コンテキストのASTノード
//---------------------------------------------------------------------------
class tRisseASTNode_Context : public tRisseASTNode_List
{
	tRisseASTContextType ContextType; //!< コンテキストタイプ
	tRisseString Name; //!< コンテキストの名前(説明用)

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		context_type	コンテキストタイプ
	//! @param		name			コンテキストの名前
	tRisseASTNode_Context(risse_size position, tRisseASTContextType context_type,
		const tRisseString & name) :
		tRisseASTNode_List(position, antContext), ContextType(context_type), Name(name) {;}

	//! @brief		コンテキストタイプを得る
	//! @return		コンテキストタイプ
	tRisseASTContextType GetContextType() const { return ContextType; }

	//! @brief		名前(説明用)を得る
	//! @return		名前(説明用)
	const tRisseString & GetName() const { return Name; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	項ASTノード(type=antFactor)
//---------------------------------------------------------------------------
class tRisseASTNode_Factor : public tRisseASTNode
{
	tRisseASTFactorType FactorType; //!< 項のタイプ
	tRisseVariant Value; //!< 値

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		type			項のタイプ
	//! @param		value			項の値
	tRisseASTNode_Factor(risse_size position, tRisseASTFactorType factor_type,
		const tRisseVariant & value = tRisseVariant()) :
		tRisseASTNode(position, antFactor), FactorType(factor_type), Value(value) {;}

	//! @brief		項のタイプを得る
	//! @return		項のタイプ
	tRisseASTFactorType GetFactorType() const { return FactorType; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	単項演算子ノード(type=antUnary)
//---------------------------------------------------------------------------
class tRisseASTNode_Unary : public tRisseASTNode
{
	tRisseASTUnaryType UnaryType; //!< 単項演算子のタイプ
	tRisseASTNode * Child; //!< 子ノード

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		unary_type		単項演算子のタイプ
	//! @param		child			子ノード
	tRisseASTNode_Unary(risse_size position, tRisseASTUnaryType unary_type,
			tRisseASTNode * child) :
		tRisseASTNode(position, antUnary), UnaryType(unary_type), Child(child)
	{
		Child->SetParent(this);
	}

	//! @brief		単項演算子のタイプを得る
	//! @return		単項演算子のタイプ
	tRisseASTUnaryType GetUnaryType() const { return UnaryType; }

	//! @brief		子ノードを得る
	//! @return		子ノード
	tRisseASTNode * GetChild() const { return Child; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	二項演算子ノード(type=antBinary)
//---------------------------------------------------------------------------
class tRisseASTNode_Binary : public tRisseASTNode
{
	tRisseASTBinaryType BinaryType; //!< 二項演算子のタイプ
	tRisseASTNode * Child1; //!< 子ノード1
	tRisseASTNode * Child2; //!< 子ノード2

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		binary_type		二項演算子のタイプ
	//! @param		child1			子ノード1
	//! @param		child2			子ノード2
	tRisseASTNode_Binary(risse_size position, tRisseASTBinaryType binary_type,
			tRisseASTNode * child1, tRisseASTNode * child2) :
		tRisseASTNode(position, antBinary), BinaryType(binary_type),
			Child1(child1), Child2(child2)
	{
		Child1->SetParent(this);
		Child2->SetParent(this);
	}

	//! @brief		二項演算子のタイプを得る
	//! @return		二項演算子のタイプ
	tRisseASTBinaryType GetBinaryType() const { return BinaryType; }

	//! @brief		子ノード1を得る
	//! @return		子ノード1
	tRisseASTNode * GetChild1() const { return Child1; }

	//! @brief		子ノード2を得る
	//! @return		子ノード2
	tRisseASTNode * GetChild2() const { return Child2; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	三項演算子ノード(type=antTrinary)
//---------------------------------------------------------------------------
class tRisseASTNode_Trinary : public tRisseASTNode
{
	tRisseASTTrinaryType TrinaryType; //!< 三項演算子のタイプ
	tRisseASTNode * Child1; //!< 子ノード1
	tRisseASTNode * Child2; //!< 子ノード2
	tRisseASTNode * Child3; //!< 子ノード2

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		trinary_type	三項演算子のタイプ
	//! @param		child1			子ノード1
	//! @param		child2			子ノード2
	//! @param		child3			子ノード3
	tRisseASTNode_Trinary(risse_size position, tRisseASTTrinaryType trinary_type,
			tRisseASTNode * child1, tRisseASTNode * child2, tRisseASTNode * child3) :
		tRisseASTNode(position, antTrinary), TrinaryType(trinary_type),
			Child1(child1), Child2(child2), Child3(child3)
	{
		Child1->SetParent(this);
		Child2->SetParent(this);
		Child3->SetParent(this);
	}

	//! @brief		三項演算子のタイプを得る
	//! @return		三項演算子のタイプ
	tRisseASTTrinaryType GetTrinaryType() const { return TrinaryType; }

	//! @brief		子ノード1を得る
	//! @return		子ノード1
	tRisseASTNode * GetChild1() const { return Child1; }

	//! @brief		子ノード2を得る
	//! @return		子ノード2
	tRisseASTNode * GetChild2() const { return Child2; }

	//! @brief		子ノード3を得る
	//! @return		子ノード3
	tRisseASTNode * GetChild3() const { return Child3; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risse

#endif

