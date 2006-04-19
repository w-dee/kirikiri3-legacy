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

/*
	#undef risseASTH
	してから #define RISSE_AST_DEFINE_NAMES
	してこのファイルをもう一度 include すると、各 enum に
	対応した文字列の表を得ることができる。
*/

#ifdef RISSE_AST_ENUM_DEF
	#undef RISSE_AST_ENUM_DEF
#endif

#ifdef RISSE_AST_ENUM_ITEM
	#undef RISSE_AST_ENUM_ITEM
#endif

#ifdef RISSE_AST_ENUM_END
	#undef RISSE_AST_ENUM_END
#endif


#ifndef RISSE_AST_DEFINE_NAMES
	#define RISSE_AST_ENUM_DEF(X) enum tRisseAST##X {
	#define RISSE_AST_ENUM_ITEM(PREFIX, X) PREFIX##X,
	#define RISSE_AST_ENUM_END };
#else
	#define RISSE_AST_ENUM_DEF(X) static const char * RisseAST##X##Names[] = {
	#define RISSE_AST_ENUM_ITEM(PREFIX, X) #X,
	#define RISSE_AST_ENUM_END };
#endif

//---------------------------------------------------------------------------
//! @brief	ASTノードのタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(NodeType)
	RISSE_AST_ENUM_ITEM(ant, Context		)		//!< コンテキスト
	RISSE_AST_ENUM_ITEM(ant, ExprStmt		)		//!< 式のみのステートメント
	RISSE_AST_ENUM_ITEM(ant, Factor			)		//!< 項
	RISSE_AST_ENUM_ITEM(ant, Unary			)		//!< 単項演算子
	RISSE_AST_ENUM_ITEM(ant, Binary			)		//!< 二項演算子
	RISSE_AST_ENUM_ITEM(ant, Trinary		)		//!< 三項演算子
RISSE_AST_ENUM_END
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コンテキストのタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(ContextType)
	RISSE_AST_ENUM_ITEM(act, TopLevel		)		//!< トップレベル
	RISSE_AST_ENUM_ITEM(act, Class			)		//!< クラス
	RISSE_AST_ENUM_ITEM(act, Func			)		//!< 関数
RISSE_AST_ENUM_END
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	項のタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(FactorType)
	RISSE_AST_ENUM_ITEM(aft, Constant		)		//!< 定数
	RISSE_AST_ENUM_ITEM(aft, Symbol			)		//!< シンボル
	RISSE_AST_ENUM_ITEM(aft, This			)		//!< "this"
	RISSE_AST_ENUM_ITEM(aft, Super			)		//!< "super"
	RISSE_AST_ENUM_ITEM(aft, Global			)		//!< "global"
RISSE_AST_ENUM_END
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	単項演算子のタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(UnaryType)
	RISSE_AST_ENUM_ITEM(aut, LogNot			)		//!< "!" logical not
	RISSE_AST_ENUM_ITEM(aut, BitNot			)		//!< "~" bit not
	RISSE_AST_ENUM_ITEM(aut, PreDec			)		//!< "--" pre-positioned decrement
	RISSE_AST_ENUM_ITEM(aut, PreInc			)		//!< "++" pre-positioned increment
	RISSE_AST_ENUM_ITEM(aut, PostDec		)		//!< "--" post-positioned decrement
	RISSE_AST_ENUM_ITEM(aut, PostInc		)		//!< "++" post-positioned increment
	RISSE_AST_ENUM_ITEM(aut, New			)		//!< "new" (its child must be a function call node)
	RISSE_AST_ENUM_ITEM(aut, Delete			)		//!< "delete"
	RISSE_AST_ENUM_ITEM(aut, Plus			)		//!< "+"
	RISSE_AST_ENUM_ITEM(aut, Minus			)		//!< "-"
RISSE_AST_ENUM_END
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	二項演算子のタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(BinaryType)
	RISSE_AST_ENUM_ITEM(abt, If				)		//!< if
	RISSE_AST_ENUM_ITEM(abt, Comma			)		//!< 
	RISSE_AST_ENUM_ITEM(abt, Assign			)		//!< =
	RISSE_AST_ENUM_ITEM(abt, BitAndAssign	)		//!< &=
	RISSE_AST_ENUM_ITEM(abt, BitOrAssign	)		//!< |=
	RISSE_AST_ENUM_ITEM(abt, BitXorAssign	)		//!< ^=
	RISSE_AST_ENUM_ITEM(abt, SubAssign		)		//!< -=
	RISSE_AST_ENUM_ITEM(abt, AddAssign		)		//!< +=
	RISSE_AST_ENUM_ITEM(abt, ModAssign		)		//!< %=
	RISSE_AST_ENUM_ITEM(abt, DivAssign		)		//!< /=
	RISSE_AST_ENUM_ITEM(abt, IdivAssign		)		//!< \=
	RISSE_AST_ENUM_ITEM(abt, MulAssign		)		//!< *=
	RISSE_AST_ENUM_ITEM(abt, LogOrAssign	)		//!< ||=
	RISSE_AST_ENUM_ITEM(abt, LogAndAssign	)		//!< &&=
	RISSE_AST_ENUM_ITEM(abt, RBitShiftAssign)		//!< >>>=
	RISSE_AST_ENUM_ITEM(abt, LShiftAssign	)		//!< <<=
	RISSE_AST_ENUM_ITEM(abt, RShiftAssign	)		//!< >>=
	RISSE_AST_ENUM_ITEM(abt, LogOr			)		//!< ||
	RISSE_AST_ENUM_ITEM(abt, LogAnd			)		//!< &&
	RISSE_AST_ENUM_ITEM(abt, BitOr			)		//!< |
	RISSE_AST_ENUM_ITEM(abt, BitXor			)		//!< ^
	RISSE_AST_ENUM_ITEM(abt, BitAnd			)		//!< &
	RISSE_AST_ENUM_ITEM(abt, NotEqual		)		//!< !=
	RISSE_AST_ENUM_ITEM(abt, Equal			)		//!< ==
	RISSE_AST_ENUM_ITEM(abt, DiscNotEqual	)		//!< !==
	RISSE_AST_ENUM_ITEM(abt, DiscEqual		)		//!< ===
	RISSE_AST_ENUM_ITEM(abt, Swap			)		//!< <->
	RISSE_AST_ENUM_ITEM(abt, Lesser			)		//!< <
	RISSE_AST_ENUM_ITEM(abt, Greater		)		//!< >
	RISSE_AST_ENUM_ITEM(abt, LesserOrEqual	)		//!< <=
	RISSE_AST_ENUM_ITEM(abt, GreaterOrEqual	)		//!< >=
	RISSE_AST_ENUM_ITEM(abt, RBitShift		)		//!< >>>
	RISSE_AST_ENUM_ITEM(abt, LShift			)		//!< <<
	RISSE_AST_ENUM_ITEM(abt, RShift			)		//!< >>
	RISSE_AST_ENUM_ITEM(abt, Mod			)		//!< %
	RISSE_AST_ENUM_ITEM(abt, Div			)		//!< /
	RISSE_AST_ENUM_ITEM(abt, Idiv			)		//!< \ (integer div)
	RISSE_AST_ENUM_ITEM(abt, Mul			)		//!< *
	RISSE_AST_ENUM_ITEM(abt, Add			)		//!< +
	RISSE_AST_ENUM_ITEM(abt, Sub			)		//!< -
	RISSE_AST_ENUM_ITEM(abt, DirectSel		)		//!< .
	RISSE_AST_ENUM_ITEM(abt, IndirectSel	)		//!< [ ]
	RISSE_AST_ENUM_ITEM(abt, IncontextOf	)		//!< incontextof
RISSE_AST_ENUM_END
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	三項演算子のタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(TrinaryType)
	RISSE_AST_ENUM_ITEM(att, Condition		)		//!< ? :
RISSE_AST_ENUM_END
//---------------------------------------------------------------------------


#ifndef RISSE_AST_DEFINE_NAMES


class tRisseASTNode;
//---------------------------------------------------------------------------
//! @brief	ASTノードの配列
//---------------------------------------------------------------------------
typedef gc_vector<tRisseASTNode *> tRisseASTArray;
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

	//! @brief		デストラクタ
	//! @note		このデストラクタは呼ばれない
	virtual ~tRisseASTNode() {;}

public:
	//! @brief		親ノードを設定する
	//! @param		parent		親ノード
	void SetParent(tRisseASTNode * parent) { Parent = parent; }

	//! @brief		親ノードを得る
	//! @return		親ノード
	tRisseASTNode * GetParent() const { return Parent; }

	//! @brief		ノードタイプを得る
	//! @return		ノードタイプ
	tRisseASTNodeType GetType() const { return Type; }

public:
	//! @brief		子ノードの個数を得る(下位クラスで実装すること)
	//! @return		子ノードの個数
	virtual risse_size GetChildCount() const = 0;

	//! @brief		指定されたインデックスの子ノードを得る(下位クラスで実装すること)
	//! @param		index		インデックス
	//! @return		子ノード
	virtual tRisseASTNode * GetChildAt(risse_size index) const = 0;

	//! @brief		指定されたインデックスの子ノードの名前を得る(下位クラスで実装すること)
	//! @param		index		インデックス
	//! @return		名前
	virtual tRisseString GetChildNameAt(risse_size index) const = 0;

public:

	//! @brief		ダンプを行う
	//! @param		result		ダンプされた文字列
	//! @param		level		再帰レベル
	void Dump(tRisseString & result, risse_int level = 0);

protected:

	//! @brief		ダンプ時のこのノードのコメントを得る(下位クラスで実装すること)
	//! @return		ダンプ時のこのノードのコメント
	virtual tRisseString GetDumpComment() const = 0;
};
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

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return Array.size();
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		if(index < Array.size())
			return Array[index];
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コンテキストのASTノード(type=antContext)
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

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	式ステートメントASTノード(type=antExprStmt)
//---------------------------------------------------------------------------
class tRisseASTNode_ExprStmt : public tRisseASTNode
{
	tRisseASTNode * Expression; //!< 式ノード

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @brief		expression		式ノード
	tRisseASTNode_ExprStmt(risse_size position, tRisseASTNode * expression) :
		tRisseASTNode(position, antExprStmt) {;}

	//! @brief		式ノードを得る
	tRisseASTNode * GetExpression() const { return Expression; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 1;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Expression; else return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const
	{
		return tRisseString(); // 空
	}
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
	//! @param		position		ソースコード上の位置
	//! @param		type			項のタイプ
	//! @param		value			項の値
	tRisseASTNode_Factor(risse_size position, tRisseASTFactorType factor_type,
		const tRisseVariant & value = tRisseVariant()) :
		tRisseASTNode(position, antFactor), FactorType(factor_type), Value(value) {;}

	//! @brief		項のタイプを得る
	//! @return		項のタイプ
	tRisseASTFactorType GetFactorType() const { return FactorType; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 0; // 子はない
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		return NULL; // 子はない
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const
	{
		return tRisseString(); // 子はない
	}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;
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
	//! @param		position		ソースコード上の位置
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

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 1;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Child; else return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;
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
	//! @param		position		ソースコード上の位置
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

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		switch(index)
		{
		case 0: return Child1;
		case 1: return Child2;
		}
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;
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
	//! @param		position		ソースコード上の位置
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

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 3;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		switch(index)
		{
		case 0: return Child1;
		case 1: return Child2;
		case 2: return Child3;
		}
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;
};
//---------------------------------------------------------------------------

#endif // #ifndef RISSE_AST_DEFINE_NAMES


//---------------------------------------------------------------------------
} // namespace Risse

#endif

