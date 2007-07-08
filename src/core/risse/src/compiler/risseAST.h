//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief AST(抽象構文木) の操作
//---------------------------------------------------------------------------

#ifndef risseASTH
#define risseASTH

#include "../risseGC.h"
#include "../risseVariant.h"
#include "../risseObject.h"
#include "risseDeclAttribute.h"

/*
	このモジュールでは、AST のデータ型を定義するだけではなく、AST から SSA形式
	へ変換を行うための機構も担う。
	SSA 形式のデータ形式などの定義は risseCodeGen.h にある。
*/


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
	RISSE_AST_ENUM_ITEM(ant, VarDecl		)		//!< 変数宣言
	RISSE_AST_ENUM_ITEM(ant, VarDeclPair	)		//!< 変数宣言の変数名と初期値
	RISSE_AST_ENUM_ITEM(ant, MemberSel		)		//!< メンバ選択演算子
	RISSE_AST_ENUM_ITEM(ant, Id				)		//!< 識別子
	RISSE_AST_ENUM_ITEM(ant, Unary			)		//!< 単項演算子
	RISSE_AST_ENUM_ITEM(ant, Binary			)		//!< 二項演算子
	RISSE_AST_ENUM_ITEM(ant, Trinary		)		//!< 三項演算子
	RISSE_AST_ENUM_ITEM(ant, InContextOf	)		//!< incontextof 演算子
	RISSE_AST_ENUM_ITEM(ant, CastAttr		)		//!< 属性のキャスト
	RISSE_AST_ENUM_ITEM(ant, Array			)		//!< インライン配列
	RISSE_AST_ENUM_ITEM(ant, Dict			)		//!< インライン辞書配列
	RISSE_AST_ENUM_ITEM(ant, DictPair		)		//!< インライン辞書配列の名前と値
	RISSE_AST_ENUM_ITEM(ant, RegExp			)		//!< 正規表現パターン
	RISSE_AST_ENUM_ITEM(ant, If				)		//!< if (とelse)
	RISSE_AST_ENUM_ITEM(ant, While			)		//!< while と do ～ while
	RISSE_AST_ENUM_ITEM(ant, For			)		//!< for
	RISSE_AST_ENUM_ITEM(ant, Return			)		//!< return
	RISSE_AST_ENUM_ITEM(ant, Throw			)		//!< throw
	RISSE_AST_ENUM_ITEM(ant, Break			)		//!< break
	RISSE_AST_ENUM_ITEM(ant, Continue		)		//!< continue
	RISSE_AST_ENUM_ITEM(ant, Debugger		)		//!< debugger
	RISSE_AST_ENUM_ITEM(ant, With			)		//!< with
	RISSE_AST_ENUM_ITEM(ant, Synchronized	)		//!< synchronized
	RISSE_AST_ENUM_ITEM(ant, Using			)		//!< using
	RISSE_AST_ENUM_ITEM(ant, Label			)		//!< ラベル
	RISSE_AST_ENUM_ITEM(ant, Goto			)		//!< goto
	RISSE_AST_ENUM_ITEM(ant, Switch			)		//!< switch
	RISSE_AST_ENUM_ITEM(ant, Case			)		//!< case / default
	RISSE_AST_ENUM_ITEM(ant, Try			)		//!< try
	RISSE_AST_ENUM_ITEM(ant, Catch			)		//!< catch
	RISSE_AST_ENUM_ITEM(ant, FuncCall		)		//!< 関数呼び出し
	RISSE_AST_ENUM_ITEM(ant, FuncCallArg	)		//!< 関数呼び出しの引数
	RISSE_AST_ENUM_ITEM(ant, FuncDecl		)		//!< 関数宣言
	RISSE_AST_ENUM_ITEM(ant, FuncDeclArg	)		//!< 関数宣言の引数
	RISSE_AST_ENUM_ITEM(ant, FuncDeclBlock	)		//!< 関数宣言のブロック引数
	RISSE_AST_ENUM_ITEM(ant, PropDecl		)		//!< プロパティ宣言
	RISSE_AST_ENUM_ITEM(ant, ClassDecl		)		//!< クラス宣言/モジュール宣言
RISSE_AST_ENUM_END
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コンテキストのタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(ContextType)
	RISSE_AST_ENUM_ITEM(act, TopLevel		)		//!< トップレベル
	RISSE_AST_ENUM_ITEM(act, Block			)		//!< ブロック
RISSE_AST_ENUM_END
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メンバ選択演算子のアクセス方法のタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(MemberAccessType)
	RISSE_AST_ENUM_ITEM(mat, Direct			)		//!< 直接メンバ選択
	RISSE_AST_ENUM_ITEM(mat, DirectThis		)		//!< 直接メンバ選択(Thisをコンテキストとして使う)
	RISSE_AST_ENUM_ITEM(mat, Indirect		)		//!< 間接メンバ選択
RISSE_AST_ENUM_END
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	項のタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(FactorType)
	RISSE_AST_ENUM_ITEM(aft, Constant		)		//!< 定数
	RISSE_AST_ENUM_ITEM(aft, This			)		//!< "this"
	RISSE_AST_ENUM_ITEM(aft, ThisProxy		)		//!< this-proxy
	RISSE_AST_ENUM_ITEM(aft, Super			)		//!< "super"
	RISSE_AST_ENUM_ITEM(aft, Global			)		//!< "global"
	RISSE_AST_ENUM_ITEM(aft, Binding		)		//!< (@) (binding)
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
	RISSE_AST_ENUM_ITEM(aut, Delete			)		//!< "delete"
	RISSE_AST_ENUM_ITEM(aut, Plus			)		//!< "+"
	RISSE_AST_ENUM_ITEM(aut, Minus			)		//!< "-"
	RISSE_AST_ENUM_ITEM(aut, String			)		//!< "string" cast to string
	RISSE_AST_ENUM_ITEM(aut, Boolean		)		//!< "boolean" cast to boolean
	RISSE_AST_ENUM_ITEM(aut, Real			)		//!< "real" cast to real
	RISSE_AST_ENUM_ITEM(aut, Integer		)		//!< "integer" cast to integer
	RISSE_AST_ENUM_ITEM(aut, Octet			)		//!< "octet" cast to octet
RISSE_AST_ENUM_END
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	二項演算子のタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(BinaryType)
	RISSE_AST_ENUM_ITEM(abt, If				)		//!< if
	RISSE_AST_ENUM_ITEM(abt, Comma			)		//!< ,
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
	RISSE_AST_ENUM_ITEM(abt, InstanceOf		)		//! instanceof
	RISSE_AST_ENUM_ITEM(abt, RBitShift		)		//!< >>>
	RISSE_AST_ENUM_ITEM(abt, LShift			)		//!< <<
	RISSE_AST_ENUM_ITEM(abt, RShift			)		//!< >>
	RISSE_AST_ENUM_ITEM(abt, Mod			)		//!< %
	RISSE_AST_ENUM_ITEM(abt, Div			)		//!< /
	RISSE_AST_ENUM_ITEM(abt, Idiv			)		//!< \ (integer div)
	RISSE_AST_ENUM_ITEM(abt, Mul			)		//!< *
	RISSE_AST_ENUM_ITEM(abt, Add			)		//!< +
	RISSE_AST_ENUM_ITEM(abt, Sub			)		//!< -
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

class tRisseScriptBlockInstance;
class tRisseSSAForm;
class tRisseASTNode;
class tRisseSSAVariable;
class tRisseSSAVariableAccessMap;
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

public:
	// PrepareSSA で準備すべき情報のタイプ
	enum tPrepareMode
	{
		pmRead, //!< 読み込み用
		pmWrite, //!< 書き込み用
		pmReadWrite, //!< 読み書き両用
	};

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
	//! @brief		ソースコード上の位置を得る
	//! @return		ソースコード上の位置
	risse_size GetPosition() const { return Position; }

	//! @brief		ソースコード上の位置を設定する
	//! @param		pos		ソースコード上の位置
	void SetPosition(risse_size pos) { Position = pos; }

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

public:
	//! @brief		式から名前を得る
	//! @return		名前 (名前を特定できない場合は空文字列が返る)
	//! @note		この関数は、たとえば A というノードならば "A"、
	//!				A.B というノードならば "B" を返す。
	tRisseString GetAccessTargetId();

public:
	//! @brief		このノードの最後のソースコード上の位置を得る
	//! @note		このノード以下をすべて探し、最後のソースコード上の位置を得る @n
	//!				最後の return 文を書く位置を決定するために使う @n
	//!				このノードが antContext の場合は、子をたどって最後の位置を探すことなく
	//!				即座に EndPosition を返す。
	risse_size SearchEndPosition() const;

	//! @brief		SSA 形式の読み込み用/書き込み用の表現の準備を行う
	//!				(必要に応じて下位クラスで実装すること)
	//! @param		form	SSA 形式インスタンス
	//! @param		mode	読み込み用情報を生成するか、描き込み用情報を生成するか
	//! @return		読み込み/あるいは書き込みを行うための情報が入った構造体へのポインタ
	virtual void * PrepareSSA(tRisseSSAForm *form, tPrepareMode mode) const
		{ return NULL; }

	//! @brief		SSA 形式の読み込み用の表現を生成する(必要に応じて下位クラスで実装すること)
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	//! @note		ここでは「読み込み用」と言っているが、書き込みではないすべての操作を意味する。
	//!				(たとえば if 文などもこのメソッドで SSA 形式に変換できる)
	virtual tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const
				{ return NULL; }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	//! @note		ここでは「読み込み用」と言っているが、書き込みではないすべての操作を意味する。
	//!				(たとえば if 文などもこのメソッドで SSA 形式に変換できる)
	tRisseSSAVariable * GenerateReadSSA(tRisseSSAForm *form) const
	{
	 	return DoReadSSA(form, PrepareSSA(form, pmRead));
	}

	//! @brief		SSA 形式の書き込み用の表現を生成する(必要に応じて下位クラスで実装すること)
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	//! @return		このノードが値の書き込みをサポートしているかどうか
	virtual bool DoWriteSSA(tRisseSSAForm *form, void * param,
			tRisseSSAVariable * value) const { return false; }

	//! @param		SSA 形式の書き込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	//! @return		書き込みが成功したかどうか
	bool GenerateWriteSSA(tRisseSSAForm *form,
			tRisseSSAVariable * value) const
	{  return DoWriteSSA(form, PrepareSSA(form, pmWrite), value); }
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

	//! @brief		配列オブジェクトを設定する
	//! @param		array	配列オブジェクト(null=配列のクリア)
	void AssignArray(const tRisseASTArray * array)
	{
		if(array)
			Array = *array;
		else
			Array.clear();
	}

	//! @brief		配列に子ノードを追加する
	//! @param		node		追加したいノード
	void AddChild(tRisseASTNode * node) { Array.push_back(node); if(node) node->SetParent(this); }

	//! @brief		最後の子ノードを削除する
	void PopChild()
	{
		Array.pop_back();
	}

	//! @brief		最後の子ノードを返す
	//! @return		最後の子ノード
	tRisseASTNode * GetLastChild() const
	{
		return Array.back();
	}

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
//! @brief	式を一つだけ子にとるノードの基本クラス
//---------------------------------------------------------------------------
class tRisseASTNode_OneExpression : public tRisseASTNode
{
protected:
	tRisseASTNode * Expression; //!< 式ノード

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		type			ノードタイプ
	//! @brief		expression		式ノード
	tRisseASTNode_OneExpression(risse_size position, tRisseASTNodeType type,
		tRisseASTNode * expression) :
		tRisseASTNode(position, type), Expression(expression)
	{
		if(Expression) Expression->SetParent(this);
	}

	//! @brief		式ノードを得る
	//! @return		式ノード
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
//! @brief	子の無いノードの基本クラス
//---------------------------------------------------------------------------
class tRisseASTNode_NoChildren : public tRisseASTNode
{
public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		type			ノードタイプ
	tRisseASTNode_NoChildren(risse_size position, tRisseASTNodeType type) :
		tRisseASTNode(position, type)
	{
	}

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 0;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const { return tRisseString(); }

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const
	{
		return tRisseString(); // 空
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コンテキストのASTノード(type=antContext)
//---------------------------------------------------------------------------
class tRisseASTNode_Context : public tRisseASTNode_List
{
	tRisseASTContextType ContextType; //!< コンテキストタイプ
	tRisseString Name; //!< コンテキストの名前(説明用)
	risse_size EndPosition; //!< このコンテキストが終わる位置

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		context_type	コンテキストタイプ
	//! @param		name			コンテキストの名前
	tRisseASTNode_Context(risse_size position, tRisseASTContextType context_type,
		const tRisseString & name) :
		tRisseASTNode_List(position, antContext), ContextType(context_type),
		Name(name), EndPosition(position) {;}

	//! @brief		コンテキストタイプを得る
	//! @return		コンテキストタイプ
	tRisseASTContextType GetContextType() const { return ContextType; }

	//! @brief		名前(説明用)を得る
	//! @return		名前(説明用)
	const tRisseString & GetName() const { return Name; }

	//! @brief		コンテキストが終わる位置を設定する
	//! @param		pos			コンテキストが終わる位置
	void SetEndPosition(risse_size pos) { EndPosition = pos; }

	//! @brief		コンテキストが終わる位置を取得する
	//! @return		コンテキストが終わる位置
	risse_size GetEndPosition() const { return EndPosition; }

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	アクセス時の属性キャスト(type=antCastAttr)
//---------------------------------------------------------------------------
class tRisseASTNode_CastAttr : public tRisseASTNode_OneExpression
{
	tRisseDeclAttribute Attribute; //!< 属性
public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		attribute		属性
	//! @param		expression		式ノード
	tRisseASTNode_CastAttr(risse_size position, tRisseDeclAttribute attribute,
		tRisseASTNode * expression) :
		tRisseASTNode_OneExpression(position, antCastAttr, expression)
	{
		Attribute = attribute;
	}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const { return Attribute.AsString(); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	式ステートメントASTノード(type=antExprStmt)
//---------------------------------------------------------------------------
class tRisseASTNode_ExprStmt : public tRisseASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @brief		expression		式ノード
	tRisseASTNode_ExprStmt(risse_size position, tRisseASTNode * expression) :
		tRisseASTNode_OneExpression(position, antExprStmt, expression) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
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

	//! @brief		値を得る
	//! @return		値
	const tRisseVariant & GetValue() const { return Value; }

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

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	変数宣言ノード(type=antVarDecl)
//---------------------------------------------------------------------------
class tRisseASTNode_VarDecl : public tRisseASTNode_List
{
	typedef tRisseASTNode_List inherited;

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tRisseASTNode_VarDecl(risse_size position) :
		tRisseASTNode_List(position, antVarDecl) {;}

	//! @brief		属性を設定する
	//! @param		attrib	属性
	//! @note		この操作は、すべての子ノード (tRisseASTNode_VarDeclPair) に対して
	//!				属性を再設定する。これ以降に子に加わったノードに対しては
	//!				属性は自動的には設定されないので注意すること。
	void SetAttribute(tRisseDeclAttribute attrib);

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const { return tRisseString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	変数宣言の名前と初期値のノード(type=antVarDeclPair)
//---------------------------------------------------------------------------
class tRisseASTNode_VarDeclPair : public tRisseASTNode
{
	tRisseASTNode * Name; //!< 名前を表すノード
	tRisseASTNode * Initializer; //!< 初期値ノード
	tRisseDeclAttribute Attribute; //!< 属性

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			名前
	//! @param		initializer		初期値ノード
	tRisseASTNode_VarDeclPair(risse_size position,
		tRisseASTNode * name, tRisseASTNode * initializer);

	//! @brief		名前を得る
	//! @return		名前
	tRisseASTNode * GetName() const { return Name; }

	//! @brief		初期値ノードを得る
	//! @return		初期値ノード
	tRisseASTNode * GetInitializer() const { return Initializer; }

	//! @brief		属性を設定する
	//! @param		attrib	属性
	//! @note		この操作は、Name がメンバ選択演算子だった場合、そのメンバ選択演算子
	//!				にも同じ属性を設定する。
	void SetAttribute(tRisseDeclAttribute attrib);

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
		case 0: return Name;
		case 1: return Initializer;
		}
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const { return Attribute.AsString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;

public:
	//! @brief		変数宣言のSSA 形式の表現を準備する
	//! @param		form		SSA 形式インスタンス
	//! @param		name		変数名
	//! @note		GenerateVarDecl() に先だってこれを実行すること。
	//!				このメソッドはローカル名前空間に変数を登録し、
	//!				PrepareVarDecl() と GenerateVarDecl() の間で生成される
	//!				コードからこのローカル変数が見えるようにする。
	//!				これは、{ function f() { return f(); } } のように
	//!				ローカル関数が再帰することを許すためにある。
	//!				ただし、var t = t + 1; とした場合の = の右側の t は
	//!				不定であると定義されるので注意が必要。前述の
	//!				再帰の場合は、function { ... } 内で f が使用される際には
	//!				実行順により、すでに f には値が代入されていると見なすことが
	//!				できるため安全なのである。
	//!				このメソッドはローカル名前空間に変数が作られるばあいのみに
	//!				機能し、それ以外ではなにもおこなわない。
	static void PrepareVarDecl(tRisseSSAForm * form, const tRisseASTNode * name);

	//! @brief		変数宣言のSSA 形式の表現を生成する
	//! @param		form		SSA 形式インスタンス
	//! @param		position	ソースコード上の位置
	//! @param		name		変数名
	//! @param		init		初期値を表すSSA形式変数
	//! @param		attrib		変数の属性
	//! @note		これに先立って PrepareVarDecl() を実行しておくこと。
	static void GenerateVarDecl(tRisseSSAForm * form, risse_size position, const tRisseASTNode * name,
			tRisseSSAVariable * init,
			tRisseDeclAttribute attrib = tRisseMemberAttribute::GetDefault() );
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	メンバ選択ノード(type=antMemberSel)
//---------------------------------------------------------------------------
class tRisseASTNode_MemberSel : public tRisseASTNode
{
	tRisseASTNode * Object; //!< オブジェクトノード
	tRisseASTNode * MemberName; //!< メンバ名ノード
	tRisseASTMemberAccessType AccessType ; //!< 演算子の種類
	tRisseOperateFlags Flags; //!< メンバの操作フラグ(tRisseObjectInterface::Opeate() に渡す物)
	tRisseDeclAttribute Attribute; //!< メンバの書き込み時の属性設定値 @r
		//!< メンバへの書き込みのついでに属性を設定する場合のメンバの属性

private:
	//! @brief		PrepareSSA() で返す構造体
	struct tPrepareSSA : public tRisseCollectee
	{
		tRisseSSAVariable * ObjectVar; //!< オブジェクトの式の値
		tRisseSSAVariable * MemberNameVar; //!< メンバ名を表す式の値
	};

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		object			オブジェクトノード
	//! @param		membername		メンバ名ノード
	//! @param		access_type		アクセス方法の種類
	//! @param		flags			メンバの操作フラグ
	tRisseASTNode_MemberSel(risse_size position,
			tRisseASTNode * object, tRisseASTNode * membername, tRisseASTMemberAccessType access_type,
				tRisseOperateFlags flags = tRisseOperateFlags()) :
		tRisseASTNode(position, antMemberSel),
			Object(object), MemberName(membername), AccessType(access_type), Flags(flags)
	{
		if(Object) Object->SetParent(this);
		if(MemberName) MemberName->SetParent(this);
	}

	//! @brief		オブジェクトノードを得る
	//! @return		オブジェクトノード
	tRisseASTNode * GetObject() const { return Object; }

	//! @brief		メンバ名ノードを得る
	//! @return		メンバ名ノード
	tRisseASTNode * GetMemberName() const { return MemberName; }

	//! @brief		アクセス方法の種類を得る
	//! @return		アクセス方法の種類
	tRisseASTMemberAccessType GetAccessType() const { return AccessType; }

	//! @brief		メンバの書き込み時の属性設定値を設定する
	//! @param		attrib		メンバの書き込み時の属性設定値
	void SetAttribute(tRisseDeclAttribute attrib) { Attribute.Overwrite(attrib); }

	//! @brief		メンバの書き込み時の属性設定値を得る
	//! @return		メンバの書き込み時の属性設定値
	bool GetAttribute() const { return Attribute; }

	//! @brief		メンバの操作フラグを設定する
	//! @param		flags		メンバの操作フラグ
	void SetFlags(tRisseOperateFlags flags) { Flags = flags; }

	//! @brief		メンバの操作フラグを得る
	//! @return		メンバの操作フラグ
	tRisseOperateFlags GetFlags() const { return Flags; }

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
		case 0: return Object;
		case 1: return MemberName;
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

	//! @brief		SSA 形式の読み込み用/書き込み用の表現の準備を行う
	//! @param		form	SSA 形式インスタンス
	//! @param		mode	読み込み用情報を生成するか、描き込み用情報を生成するか
	//! @return		読み込み/あるいは書き込みを行うための情報が入った構造体へのポインタ
	virtual void * PrepareSSA(tRisseSSAForm *form, tPrepareMode mode) const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;

	//! @brief		SSA 形式の書き込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	bool DoWriteSSA(tRisseSSAForm *form, void * param,
			tRisseSSAVariable * value) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	識別子ASTノード(type=antId)
//---------------------------------------------------------------------------
class tRisseASTNode_Id : public tRisseASTNode
{
	tRisseString Name; //!< 識別子名
	bool IsPrivate; //!< private (@つき) 変数

	//! @brief		PrepareSSA() で返す構造体
	struct tPrepareSSA : public tRisseCollectee
	{
		const tRisseASTNode_MemberSel * MemberSel;
			//!< ローカル変数ではなかったときには生成された tRisseASTNode_MemberSel のインスタンス
			//!< ローカル変数の場合は NULL
		void * MemberSelParam; //!< tRisseASTNode_MemberSel::PrepareSSA() が生成した情報
	};

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			識別子名
	//! @param		is_priv			private (@つき) 変数かどうか
	tRisseASTNode_Id(risse_size position, const tRisseString & name, bool is_priv) :
		tRisseASTNode(position, antId), Name(name), IsPrivate(is_priv) {;}

	//! @brief		識別子名を得る
	//! @return		識別子名
	const tRisseString & GetName() const { return Name; }

	//! @brief		この変数がプライベート (@つき) 変数かどうかを得る
	//! @return		この変数がプライベート (@つき) 変数かどうか
	bool GetIsPrivate() const { return IsPrivate; }

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

	//! @brief		SSA 形式の読み込み用/書き込み用の表現の準備を行う
	//! @param		form	SSA 形式インスタンス
	//! @param		mode	読み込み用情報を生成するか、描き込み用情報を生成するか
	//! @return		読み込み/あるいは書き込みを行うための情報が入った構造体へのポインタ
	void * PrepareSSA(tRisseSSAForm *form, tPrepareMode mode) const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;

	//! @brief		SSA 形式の書き込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	bool DoWriteSSA(tRisseSSAForm *form, void * param,
			tRisseSSAVariable * value) const;

	//! @brief		この識別子がローカル名前空間に存在するかどうかを返す
	//! @param		form	SSA 形式インスタンス
	//! @return		この識別子がローカル名前空間に存在するかどうか
	bool ExistInLocalNamespace(tRisseSSAForm * form) const;

	//! @brief		この識別子を this 内にアクセスする private 変数用のAST ノードを作成して返す
	//! @param		prefix		変数名の前につけるプリフィクス
	//! @param		write		書き込みの場合に真
	//! @return		この識別子を this 内にアクセスする private 変数用のAST ノード
	const tRisseASTNode_MemberSel * CreatePrivateAccessNodeOnThis(const tRisseString & prefix, bool write) const;

	//! @brief		この識別子を this-proxy 内にアクセスする AST ノードを作成して返す
	//! @return		この識別子を this-proxy 内にアクセスする AST ノード
	const tRisseASTNode_MemberSel * CreateAccessNodeOnThisProxy() const;
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
		if(Child) Child->SetParent(this);
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

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
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
		if(Child1) Child1->SetParent(this);
		if(Child2) Child2->SetParent(this);
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

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
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
		if(Child1) Child1->SetParent(this);
		if(Child2) Child2->SetParent(this);
		if(Child3) Child3->SetParent(this);
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

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	InContextOf ノード
//---------------------------------------------------------------------------
class tRisseASTNode_InContextOf : public tRisseASTNode
{
	tRisseASTNode * Instance; //!< インスタンスノード
	tRisseASTNode * Context; //!< コンテキストノード (NULL=dynamicのとき)

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		instance		インスタンス
	//! @param		context			コンテキスト (NULL = dynamicのとき)
	tRisseASTNode_InContextOf(risse_size position,
			tRisseASTNode * instance, tRisseASTNode * context) :
		tRisseASTNode(position, antInContextOf),
			Instance(instance), Context(context)
	{
		if(Instance) Instance->SetParent(this);
		if(Context) Context->SetParent(this);
	}

	//! @brief		インスタンスノードを得る
	//! @return		インスタンスノード
	tRisseASTNode * GetInstance() const { return Instance; }

	//! @brief		コンテキストノードを得る
	//! @return		コンテキストノード
	tRisseASTNode * GetContext() const { return Context; }

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
		case 0: return Instance;
		case 1: return Context;
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

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	インライン配列ノード(type=antArray)
//---------------------------------------------------------------------------
class tRisseASTNode_Array : public tRisseASTNode_List
{
	typedef tRisseASTNode_List inherited;

	//! @brief		PrepareSSA() で返す構造体
	struct tPrepareSSA : public tRisseCollectee
	{
		gc_vector<void *> Elements; //!< 各要素の準備用データ
		gc_vector<tRisseSSAVariable *> Indices; //!< 各インデックスを表す数値定数
	};

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tRisseASTNode_Array(risse_size position) :
		tRisseASTNode_List(position, antArray) {;}

	//! @brief		配列の最後の null を削除する
	void Strip();

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const { return tRisseString(); }

	//! @brief		SSA 形式の読み込み用/書き込み用の表現の準備を行う
	//! @param		form	SSA 形式インスタンス
	//! @param		mode	読み込み用情報を生成するか、描き込み用情報を生成するか
	//! @return		読み込み/あるいは書き込みを行うための情報が入った構造体へのポインタ
	void * PrepareSSA(tRisseSSAForm *form, tPrepareMode mode) const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;

	//! @brief		SSA 形式の書き込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	bool DoWriteSSA(tRisseSSAForm *form, void * param,
			tRisseSSAVariable * value) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	インライン辞書配列ノード(type=antDict)
//---------------------------------------------------------------------------
class tRisseASTNode_Dict : public tRisseASTNode_List
{
	typedef tRisseASTNode_List inherited;

	//! @brief		PrepareSSA() で返す構造体
	struct tPrepareSSA : public tRisseCollectee
	{
		gc_vector<void *> Names; //!< 各要素の「名前」の準備用データ
		gc_vector<void *> Values; //!< 各要素の「値」の準備用データ
	};

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tRisseASTNode_Dict(risse_size position) :
		tRisseASTNode_List(position, antDict) {;}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const { return tRisseString(); }

	//! @brief		SSA 形式の読み込み用/書き込み用の表現の準備を行う
	//! @param		form	SSA 形式インスタンス
	//! @param		mode	読み込み用情報を生成するか、描き込み用情報を生成するか
	//! @return		読み込み/あるいは書き込みを行うための情報が入った構造体へのポインタ
	void * PrepareSSA(tRisseSSAForm *form, tPrepareMode mode) const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;

	//! @brief		SSA 形式の書き込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	bool DoWriteSSA(tRisseSSAForm *form, void * param,
			tRisseSSAVariable * value) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	インライン辞書の子ノード(type=antDictPair)
//---------------------------------------------------------------------------
class tRisseASTNode_DictPair : public tRisseASTNode
{
	tRisseASTNode * Name; //!< 名前ノード
	tRisseASTNode * Value; //!< 値ノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			名前ノード
	//! @param		value			値ノード
	tRisseASTNode_DictPair(risse_size position,
		tRisseASTNode * name, tRisseASTNode * value) :
		tRisseASTNode(position, antDictPair),
			Name(name), Value(value)
	{
		if(Name) Name->SetParent(this);
		if(Value) Value->SetParent(this);
	}

	//! @brief		名前ノードを得る
	//! @return		名前ノード
	tRisseASTNode * GetName() const { return Name; }

	//! @brief		値ノードを得る
	//! @return		値ノード
	tRisseASTNode * GetValue() const { return Value; }

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
		case 0: return Name;
		case 1: return Value;
		}
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const { return tRisseString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	正規表現パターンASTノード(type=antRegExp)
//---------------------------------------------------------------------------
class tRisseASTNode_RegExp : public tRisseASTNode_NoChildren
{
	tRisseString Pattern; //!< 正規表現パターン
	tRisseString Flags; //!< 正規表現フラグ

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		pattern			正規表現パターン
	//! @param		flags			正規表現フラグ
	tRisseASTNode_RegExp(risse_size position,
		const tRisseString & pattern, const tRisseString & flags) :
		tRisseASTNode_NoChildren(position, antRegExp), Pattern(pattern), Flags(flags)
	{;}

	//! @brief		正規表現パターンを得る
	//! @return		正規表現パターン
	const tRisseString & GetPattern() const { return Pattern; }

	//! @brief		正規表現フラグを得る
	//! @return		正規表現フラグ
	const tRisseString & GetFlags() const { return Flags; }

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	if (とelse)(type=antIf)
//---------------------------------------------------------------------------
class tRisseASTNode_If : public tRisseASTNode
{
	tRisseASTNode * Condition; //!< 条件式
	tRisseASTNode * True; //!< 条件が真の時に実行するノード
	tRisseASTNode * False; //!< 条件が偽の時に実行するノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		condition		条件式
	//! @param		true			条件が真の時に実行するノード
	tRisseASTNode_If(risse_size position, tRisseASTNode * condition,
		tRisseASTNode * truenode) :
		tRisseASTNode(position, antIf),
			Condition(condition), True(truenode)
	{
		if(Condition) Condition->SetParent(this);
		if(True) True->SetParent(this);
		False = NULL;
	}

	//! @brief		条件式ノードを得る
	//! @return		条件式ノード
	tRisseASTNode * GetCondition() const { return Condition; }

	//! @brief		条件が真の時に実行するノードを得る
	//! @return		条件が真の時に実行するノード
	tRisseASTNode * GetTrue() const { return True; }

	//! @brief		条件が偽の時に実行するノードを得る
	//! @return		条件が偽の時に実行するノード
	tRisseASTNode * GetFalse() const { return False; }

	//! @brief		条件が偽の時に実行するノードを設定する
	//! @param		falsenode	条件が偽の時に実行するノード
	void SetFalse(tRisseASTNode * falsenode)
	{
		False = falsenode;
		if(False) False->SetParent(this);
	}

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
		case 0: return Condition;
		case 1: return True;
		case 2: return False;
		}
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const { return tRisseString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する(このクラスと ? : 演算子で使われる)
	//! @param		form		SSA 形式インスタンス
	//! @param		pos			ソースコード上の位置
	//! @param		basenamse	ブロックの名前のprefixにする基本名
	//! @param		condition	条件式ノード
	//! @param		truenode	条件が真の時に実行するノード
	//! @param		falsenode	条件が偽の時に実行するノード
	//! @param		needresult	結果が必要かどうか (  ? : 演算子では真を指定する )
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	static tRisseSSAVariable * InternalDoReadSSA(
		tRisseSSAForm *form,
		risse_size pos,
		const tRisseString &basename,
		tRisseASTNode * condition,
		tRisseASTNode * truenode,
		tRisseASTNode * falsenode,
		bool needresult
		);

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	while文のノード(type=antWhile)
//---------------------------------------------------------------------------
class tRisseASTNode_While : public tRisseASTNode
{
	tRisseASTNode * Condition; //!< 条件式ノード
	tRisseASTNode * Body; //!< 条件が真の間実行するノード
	bool SkipFirstCheck; //!< 最初の式チェックを省略するかどうか (do～whileかどうか)

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		condition		条件式ノード
	//! @param		body			条件が真の間実行するノード
	//! @param		skipfirstcheck	最初の式チェックを省略するかどうか (do～whileかどうか)
	tRisseASTNode_While(risse_size position,
		tRisseASTNode * condition, tRisseASTNode * body, bool skipfirstcheck) :
		tRisseASTNode(position, antWhile),
			Condition(condition), Body(body), SkipFirstCheck(skipfirstcheck)
	{
		if(Condition) Condition->SetParent(this);
		if(Body) Body->SetParent(this);
	}

	//! @brief		条件式ノードを得る
	//! @return		条件式ノード
	tRisseASTNode * GetCondition() const { return Condition; }

	//! @brief		条件が真の間実行するノードを得る
	//! @return		条件が真の間実行するノード
	tRisseASTNode * GetBody() const { return Body; }

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
		case 0: return Condition;
		case 1: return Body;
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

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	for文のノード(type=antFor)
//---------------------------------------------------------------------------
class tRisseASTNode_For : public tRisseASTNode
{
	tRisseASTNode * Initializer; //!< 初期化ノード(第1節)
	tRisseASTNode * Condition; //!< 継続条件ノード(第2節)
	tRisseASTNode * Iterator; //!< 増分処理ノード(第3節)
	tRisseASTNode * Body; //!< 条件が真の間実行するノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		initializer		初期化ノード
	//! @param		condition		継続条件ノード
	//! @param		iterator		増分処理ノード
	//! @param		body			条件が真の間実行するノード
	tRisseASTNode_For(risse_size position,
		tRisseASTNode * initializer, tRisseASTNode * condition, tRisseASTNode * iterator,
		tRisseASTNode * body) :
		tRisseASTNode(position, antFor),
			Initializer(initializer), Condition(condition), Iterator(iterator),
			Body(body)
	{
		if(Initializer) Initializer->SetParent(this);
		if(Condition) Condition->SetParent(this);
		if(Iterator) Iterator->SetParent(this);
		if(Body) Body->SetParent(this);
	}

	//! @brief		初期化ノードを得る
	//! @return		初期化ノード
	tRisseASTNode * GetInitializer() const { return Initializer; }

	//! @brief		継続条件ノードを得る
	//! @return		継続条件ノード
	tRisseASTNode * GetCondition() const { return Condition; }

	//! @brief		増分処理ノードを得る
	//! @return		増分処理ノード
	tRisseASTNode * GetIterator() const { return Iterator; }

	//! @brief		条件が真の間実行するノードを得る
	//! @return		条件が真の間実行するノード
	tRisseASTNode * GetBody() const { return Body; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 4;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		switch(index)
		{
		case 0: return Initializer;
		case 1: return Condition;
		case 2: return Iterator;
		case 3: return Body;
		}
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const { return tRisseString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	return文ノード(type=antReturn)
//---------------------------------------------------------------------------
class tRisseASTNode_Return : public tRisseASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		expression		式ノード
	tRisseASTNode_Return(risse_size position, tRisseASTNode * expression) :
		tRisseASTNode_OneExpression(position, antReturn, expression) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	throw文ノード(type=antThrow)
//---------------------------------------------------------------------------
class tRisseASTNode_Throw : public tRisseASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		expression		式ノード
	tRisseASTNode_Throw(risse_size position, tRisseASTNode * expression) :
		tRisseASTNode_OneExpression(position, antThrow, expression) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	break文ノード(type=antBreak)
//---------------------------------------------------------------------------
class tRisseASTNode_Break : public tRisseASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		expression		式ノード
	tRisseASTNode_Break(risse_size position, tRisseASTNode * expression) :
		tRisseASTNode_OneExpression(position, antBreak, expression) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	continue文ノード(type=antContinue)
//---------------------------------------------------------------------------
class tRisseASTNode_Continue : public tRisseASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		expression		式ノード
	tRisseASTNode_Continue(risse_size position, tRisseASTNode * expression) :
		tRisseASTNode_OneExpression(position, antContinue, expression) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	debugger文ノード(type=antDebugger)
//---------------------------------------------------------------------------
class tRisseASTNode_Debugger : public tRisseASTNode_NoChildren
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tRisseASTNode_Debugger(risse_size position) :
		tRisseASTNode_NoChildren(position, antDebugger) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	with文などの基本クラス
//---------------------------------------------------------------------------
class tRisseASTNode_ExpressionBlock : public tRisseASTNode
{
protected:
	tRisseASTNode * Object; //!< オブジェクトノード
	tRisseASTNode * Body; //!< ブロックまたは文のノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		type			ノードタイプ
	//! @param		object			オブジェクトノード
	//! @param		body			ブロックまたは文のノード
	tRisseASTNode_ExpressionBlock(risse_size position,
		tRisseASTNodeType type,
		tRisseASTNode * object, tRisseASTNode * body) :
		tRisseASTNode(position, type),
			Object(object), Body(body)
	{
		if(Object) Object->SetParent(this);
		if(Body) Body->SetParent(this);
	}

	//! @brief		オブジェクトノードを得る
	//! @return		オブジェクトノード
	tRisseASTNode * GetObject() const { return Object; }

	//! @brief		ブロックまたは文のノードを得る
	//! @return		ブロックまたは文のノード
	tRisseASTNode * GetBody() const { return Body; }

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
		case 0: return Object;
		case 1: return Body;
		}
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const { return tRisseString(); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	with文ノード(type=antWith)
//---------------------------------------------------------------------------
class tRisseASTNode_With : public tRisseASTNode_ExpressionBlock
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		object			オブジェクトノード
	//! @param		body			ブロックまたは文のノード
	tRisseASTNode_With(risse_size position,
		tRisseASTNode * object, tRisseASTNode * body) :
		tRisseASTNode_ExpressionBlock(position, antWith, object, body) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	synchronized文ノード(type=antSynchronized)
//---------------------------------------------------------------------------
class tRisseASTNode_Synchronized : public tRisseASTNode_ExpressionBlock
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		object			オブジェクトノード
	//! @param		body			ブロックまたは文のノード
	tRisseASTNode_Synchronized(risse_size position,
		tRisseASTNode * object, tRisseASTNode * body) :
		tRisseASTNode_ExpressionBlock(position, antSynchronized, object, body) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	using文ノード(type=antUsing)
//---------------------------------------------------------------------------
class tRisseASTNode_Using : public tRisseASTNode_ExpressionBlock
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		object			オブジェクトノード
	//! @param		body			ブロックまたは文のノード
	tRisseASTNode_Using(risse_size position,
		tRisseASTNode * object, tRisseASTNode * body) :
		tRisseASTNode_ExpressionBlock(position, antUsing, object, body) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	ラベルノード(type=antLabel)
//---------------------------------------------------------------------------
class tRisseASTNode_Label : public tRisseASTNode_NoChildren
{
	tRisseString Name; //!< ラベル名

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		name			ラベル名
	tRisseASTNode_Label(risse_size position, const tRisseString & name) :
		tRisseASTNode_NoChildren(position, antLabel), Name(name) {;}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const
	{
		return Name.AsHumanReadable();
	}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	goto ノード(type=antGoto)
//---------------------------------------------------------------------------
class tRisseASTNode_Goto : public tRisseASTNode_NoChildren
{
	tRisseString Name; //!< ジャンプ先ラベル名

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		name			ジャンプ先ラベル名
	tRisseASTNode_Goto(risse_size position, const tRisseString & name) :
		tRisseASTNode_NoChildren(position, antGoto), Name(name) {;}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const
	{
		return Name.AsHumanReadable();
	}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	switch文ノード(type=antSwitch)
//---------------------------------------------------------------------------
class tRisseASTNode_Switch : public tRisseASTNode_ExpressionBlock
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		object			オブジェクトノード
	//! @param		body			ブロックまたは文のノード
	tRisseASTNode_Switch(risse_size position,
		tRisseASTNode * object, tRisseASTNode * body) :
		tRisseASTNode_ExpressionBlock(position, antSwitch, object, body) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	case  / default ノード(type=antCase)
//---------------------------------------------------------------------------
class tRisseASTNode_Case : public tRisseASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		expression		式ノード
	tRisseASTNode_Case(risse_size position, tRisseASTNode * expression) :
		tRisseASTNode_OneExpression(position, antCase, expression) {;}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	tryノード(type=antTry)
//---------------------------------------------------------------------------
class tRisseASTNode_Try : public tRisseASTNode_List
{
	typedef tRisseASTNode_List inherited;
	tRisseASTNode *Body; //!< tryブロック
	tRisseASTNode *Finally; //!< finallyブロック

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tRisseASTNode_Try(risse_size position) :
		tRisseASTNode_List(position, antTry)
	{
		Body = NULL;
		Finally = NULL;
	}

	//! @brief		tryブロックを得る
	//! @return		tryブロック
	bool GetBody() const { return Body; }

	//! @brief		tryブロックを設定する
	//! @param		body	tryブロック
	void SetBody(tRisseASTNode * body)
	{
		Body = body;
		if(Body) Body->SetParent(this);
	}

	//! @brief		finallyブロックを得る
	//! @return		finallyブロック
	bool GetFinally() const { return Finally; }

	//! @brief		finallyブロックを設定する
	//! @param		finallyblock	finallyブロック
	void SetFinally(tRisseASTNode * finallyblock)
	{
		Finally = finallyblock;
		if(Finally) Finally->SetParent(this);
	}

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return inherited::GetChildCount() + 2; // +1 = Body+Finally
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Body;
		if(index == inherited::GetChildCount() + 1) return Finally;
		return inherited::GetChildAt(index - 1);
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const { return tRisseString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;

//--
private:
	//! @brief		try-catchブロックの内容を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		is_finally	finally用の生成を行うか
	//! @return		このブロックの結果を表す SSA 形式変数
	tRisseSSAVariable * GenerateTryCatchOrFinally(tRisseSSAForm *form, bool is_finally) const;

	//! @brief		catchブロックを生成する
	//! @param		form				SSA 形式インスタンス
	//! @param		try_block_ret_var	tryブロックの戻り値
	void GenerateCatchBlock(tRisseSSAForm * form,
		tRisseSSAVariable * try_block_ret_var) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	catch ノード (type=antCatch)
//---------------------------------------------------------------------------
class tRisseASTNode_Catch : public tRisseASTNode
{
	tRisseString Name; //!< 変数名(空 = 変数を受け取らない)
	tRisseASTNode * Condition; //!< 条件ノード (NULL = 無条件)
	tRisseASTNode * Body; //!< catch のbodyノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			変数名
	//! @param		condition		条件ノード
	//! @param		body			body ノード
	tRisseASTNode_Catch(risse_size position,
		const tRisseString & name, tRisseASTNode * condition, tRisseASTNode * body) :
		tRisseASTNode(position, antCatch),
			Name(name), Condition(condition), Body(body)
	{
		if(Condition) Condition->SetParent(this);
		if(Body) Body->SetParent(this);
	}

	//! @brief		変数名を得る
	//! @return		変数名
	const tRisseString & GetName() const { return Name; }

	//! @brief		条件ノードを得る
	//! @return		条件ノード
	tRisseASTNode * GetCondition() const { return Condition; }

	//! @brief		body ノードを得る
	//! @return		body ノード
	tRisseASTNode * GetBody() const { return Body; }

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
		case 0: return Condition;
		case 1: return Body;
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

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	関数呼び出しのASTノード(type=antFuncCall)
//! @note	new 演算子つきの関数呼び出しはオブジェクト生成とみなし、
//!			関数呼び出し用ASTノードと共用する(GetCreateNewで調べる)
//---------------------------------------------------------------------------
class tRisseASTNode_FuncCall : public tRisseASTNode_List
{
	typedef tRisseASTNode_List inherited;
	tRisseASTNode * Expression; //!< 関数を表す式ノード
	bool CreateNew; //!< new による関数呼び出しかどうか
	bool Omit; //!< 引数省略かどうか
	tRisseASTArray Blocks; //!< ブロック引数の配列

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		omit			引数省略かどうか
	tRisseASTNode_FuncCall(risse_size position, bool omit) :
		tRisseASTNode_List(position, antFuncCall), Omit(omit)
	{
		Expression = NULL;
	}

	//! @brief		式ノードを設定する
	//! @param		node		式ノード
	void SetExpression(tRisseASTNode * node)
		{ Expression = node; Expression->SetParent(this); }

	//! @brief		式ノードを得る
	//! @return		式ノード
	tRisseASTNode * GetExpression() const { return Expression; }

	//! @brief		new による関数呼び出しかどうかを設定する
	//! @param		b	new による関数呼び出しかどうか
	void SetCreateNew(bool b = true) { CreateNew = b; }

	//! @brief		new による関数呼び出しかどうかを得る
	//! @return		new による関数呼び出しかどうか
	bool GetCreateNew() const { return CreateNew; }

	//! @brief		ブロック引数を追加する
	//! @param		list		ブロック引数の配列(null=何もしない)
	void AddBlocks(const tRisseASTArray * array)
	{
		if(array)
			Blocks.insert(Blocks.end(), array->begin(), array->end());
	}

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return inherited::GetChildCount() + Blocks.size() + 1; // +1 = Expression
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Expression;
		index --;
		if(index < inherited::GetChildCount())
			return inherited::GetChildAt(index);
		index -= inherited::GetChildCount();
		return Blocks[index];
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	関数呼び出しのの引数を表すノード(type=antFuncCallArg)
//---------------------------------------------------------------------------
class tRisseASTNode_FuncCallArg : public tRisseASTNode
{
	tRisseASTNode * Expression; //!< 引数を表す式ノード(NULL = 引数なし)
	bool Expand; //!< 配列の引数への展開を行うかどうか

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		expression		引数を表す式ノード
	//! @param		expand			配列の引数への展開を行うかどうか
	tRisseASTNode_FuncCallArg(risse_size position, tRisseASTNode * expression, bool expand) :
		tRisseASTNode(position, antFuncCallArg), Expression(expression), Expand(expand)
	{
		if(Expression) Expression->SetParent(this);
	}

	//! @brief		式ノードを得る
	//! @return		式ノード
	tRisseASTNode * GetExpression() const { return Expression; }

	//! @brief		配列の引数への展開を行うかどうかを得る
	//! @return		配列の引数への展開を行うかどうか
	bool GetExpand() const { return Expand; }

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
	tRisseString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	関数宣言のASTノード(type=antFuncDecl)
//---------------------------------------------------------------------------
class tRisseASTNode_FuncDecl : public tRisseASTNode_List
{
	typedef tRisseASTNode_List inherited;
	tRisseASTNode * Body; //!< 関数ボディ
	tRisseString Name; //!< 関数名
	tRisseDeclAttribute Attribute; //!< 属性
	tRisseASTArray Blocks; //!< ブロック引数の配列
	bool IsBlock; //!< 遅延評価ブロックブロックかどうか(真=遅延評価ブロック,偽=普通の関数)

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tRisseASTNode_FuncDecl(risse_size position) :
		tRisseASTNode_List(position, antFuncDecl)
	{
		Body = NULL;
		IsBlock = false;
	}

	//! @brief		関数ボディを設定する
	//! @param		node		関数ボディ
	void SetBody(tRisseASTNode * node)
		{ Body = node; Body->SetParent(this); }

	//! @brief		関数ボディを得る
	//! @return		関数ボディ
	tRisseASTNode * GetBody() const { return Body; }

	//! @brief		関数名を設定する
	//! @param		name	関数名
	void SetName(const tRisseString & name) { Name = name; }

	//! @brief		関数名を得る
	//! @return		関数名
	const tRisseString & GetName() const { return Name; }

	//! @brief		ブロック引数の配列を追加する
	//! @param		array	ブロック引数の配列(NULL=なにもしない)
	void AddBlocks(const tRisseASTArray * array)
	{
		if(array)
			Blocks.insert(Blocks.end(), array->begin(), array->end());
	}

	//! @brief		属性を設定する
	//! @param		attrib	属性
	void SetAttribute(tRisseDeclAttribute attrib) { Attribute.Overwrite(attrib); }

	//! @brief		属性を設定する
	//! @return		属性
	tRisseDeclAttribute GetAttribute() const { return Attribute; }

	//! @brief		遅延評価ブロックブロックかどうかを設定する
	//! @param		is_block		遅延評価ブロックブロックかどうか
	void SetIsBlock(bool is_block) { IsBlock = is_block; }

	//! @brief		遅延評価ブロックブロックかどうかを取得する
	//! @return		遅延評価ブロックブロックかどうか
	bool GetIsBlock() const { return IsBlock; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return inherited::GetChildCount() + Blocks.size() + 1; // +1 = Body
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		// 子ノードは
		// 1. 引数 = inherited
		// 2. ブロック引数 = blocks
		// 3. body
		// の順番になる
		if(index == inherited::GetChildCount() + Blocks.size()) return Body;
		if(index < inherited::GetChildCount())
			return inherited::GetChildAt(index);
		index -= inherited::GetChildCount();
		return Blocks[index];
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;

//--

	//! @brief		関数宣言の表現を生成する
	//! @param		form		SSA 形式インスタンス
	//! @param		access_map	アクセスマップ (IsBlock=真の場合非null必須)
	//! @param		try_id		try識別子(risse_size_maxは指定無しの場合)
	//! @return		関数を表す変数
	tRisseSSAVariable * GenerateFuncDecl(tRisseSSAForm *form,
		tRisseSSAVariableAccessMap *access_map = NULL, risse_size try_id = risse_size_max) const;


	//! @brief		属性に従って関数オブジェクトの設定を行う
	//! @param		form			SSA 形式インスタンス
	//! @param		position		ソースコード上の位置
	//! @param		function		関数オブジェクトを表すSSA形式変数
	//! @param		attr			属性
	static void ApplyMethodAttribute(tRisseSSAForm * form, risse_size position,
		tRisseSSAVariable *& function, tRisseDeclAttribute attrib);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	関数宣言の引数を表すノード(type=antFuncDeclArg)
//---------------------------------------------------------------------------
class tRisseASTNode_FuncDeclArg : public tRisseASTNode
{
	tRisseString Name; //!< 引数名
	tRisseASTNode * Initializer; //!< 初期値を表すノード(NULL = 初期値無し)
	bool Collapse; //!< 引数の配列への圧縮を行うかどうか

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			引数名
	//! @param		initializer		初期値を表すノード
	//! @param		collapse		引数の配列への圧縮を行うかどうか
	tRisseASTNode_FuncDeclArg(risse_size position, const tRisseString & name,
		tRisseASTNode * initializer, bool collapse) :
		tRisseASTNode(position, antFuncDeclArg), Name(name),
		Initializer(initializer), Collapse(collapse)
	{
		if(Initializer) Initializer->SetParent(this);
	}

	//! @brief		初期値を表すノードを得る
	//! @return		初期値を表すノード
	tRisseASTNode * GetInitializer() const { return Initializer; }

	//! @brief		引数の配列への圧縮を行うかどうかを得る
	//! @return		引数の配列への圧縮を行うかどうか
	bool GetCollapse() const { return Collapse; }

	//! @brief		引数名を得る
	//! @return		引数名
	tRisseString GetName() const { return Name; }

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
		if(index == 0) return Initializer; else return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	関数宣言のブロック引数を表すノード(type=antFuncDeclBlock)
//---------------------------------------------------------------------------
class tRisseASTNode_FuncDeclBlock : public tRisseASTNode
{
	tRisseString Name; //!< ブロック名

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			ブロック名
	tRisseASTNode_FuncDeclBlock(risse_size position, const tRisseString & name) :
		tRisseASTNode(position, antFuncDeclBlock), Name(name)
	{
		;
	}

	//! @brief		ブロック名を得る
	//! @return		ブロック名
	tRisseString GetName() const { return Name; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 0;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const
	{
		return tRisseString();
	}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	プロパティ宣言のASTノード(type=antPropDecl)
//---------------------------------------------------------------------------
class tRisseASTNode_PropDecl : public tRisseASTNode
{
	tRisseASTNode * Setter; //!< セッタ
	tRisseString SetterArgumentName; //!< セッタの引数の名前
	tRisseASTNode * Getter; //!< ゲッタ
	tRisseDeclAttribute Attribute; //!< 属性
	tRisseString Name; //!< プロパティ名

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tRisseASTNode_PropDecl(risse_size position) :
		tRisseASTNode(position, antPropDecl)
	{
		Setter = Getter = NULL;
	}

	//! @brief		セッタを設定する
	//! @param		node		セッタ
	void SetSetter(tRisseASTNode * node)
	{
		Setter = node; if(Setter) Setter->SetParent(this);
	}

	//! @brief		セッタを得る
	//! @return		セッタ
	tRisseASTNode * GetSetter(void) const
	{
		return Setter;
	}

	//! @brief		セッタの引数の名前を設定するもちろん
	//! @param		name	セッタの引数の名前
	void SetSetterArgumentName(const tRisseString & name) { SetterArgumentName = name; }

	//! @brief		セッタの引数の名前を得る
	//! @return		セッタの引数の名前
	tRisseString GetSetterArgumentName() const { return SetterArgumentName; }

	//! @brief		ゲッタを設定する
	//! @param		node		ゲッタ
	void SetGetter(tRisseASTNode * node)
	{
		Getter = node; if(Getter) Getter->SetParent(this);
	}

	//! @brief		ゲッタを得る
	//! @return		ゲッタ
	tRisseASTNode * GetGetter(void) const
	{
		return Getter;
	}

	//! @brief		プロパティ名を設定する
	//! @param		name	プロパティ名
	void SetName(const tRisseString & name) { Name = name; }

	//! @brief		プロパティ名を得る
	//! @return		プロパティ名
	const tRisseString & GetName() const { return Name; }


	//! @brief		属性を設定する
	//! @param		attrib	属性
	void SetAttribute(tRisseDeclAttribute attrib) { Attribute.Overwrite(attrib); }

	//! @brief		属性を設定する
	//! @return		属性
	tRisseDeclAttribute GetAttribute() const { return Attribute; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		// 子ノード =
		// 0. setter
		// 1. getter
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Setter;
		if(index == 1) return Getter;
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;
//--

	//! @brief		プロパティ宣言の表現を生成する
	//! @param		form		SSA 形式インスタンス
	//! @return		プロパティを表す変数
	tRisseSSAVariable * GeneratePropertyDecl(tRisseSSAForm *form) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	クラス宣言/モジュール宣言のASTノード(type=antClassDecl)
//---------------------------------------------------------------------------
class tRisseASTNode_ClassDecl : public tRisseASTNode
{
	typedef tRisseASTNode inherited;
	bool IsModule; //!< モジュール宣言の場合に真
	tRisseASTNode * SuperClass; //!< 親クラスを表す式
	tRisseASTNode * Body; //!< クラス宣言ボディ
	tRisseString Name; //!< クラス名
	tRisseDeclAttribute Attribute; //!< 属性

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		is_module		モジュール宣言かどうか
	//! @param		super_class		親クラスを表す式(is_module = false の時だけ)
	tRisseASTNode_ClassDecl(risse_size position, bool is_module, tRisseASTNode * super_class = NULL) :
		tRisseASTNode(position, antClassDecl)
	{
		IsModule = is_module;
		SuperClass = super_class;
		Body = NULL;
	}

	//! @brief		クラスボディを設定する
	//! @param		node		クラスボディ
	void SetBody(tRisseASTNode * node)
		{ Body = node; Body->SetParent(this); }

	//! @brief		クラスボディを得る
	//! @return		クラスボディ
	tRisseASTNode * GetBody() const { return Body; }

	//! @brief		モジュール宣言かどうかを得る
	//! @return		モジュール宣言
	bool GetIsModule() const { return IsModule; }

	//! @brief		クラス名を設定する
	//! @param		name	クラス名
	void SetName(const tRisseString & name) { Name = name; }

	//! @brief		クラス名を得る
	//! @return		クラス名
	const tRisseString & GetName() const { return Name; }

	//! @brief		属性を設定する
	//! @param		attrib	属性
	void SetAttribute(tRisseDeclAttribute attrib) { Attribute.Overwrite(attrib); }

	//! @brief		属性を設定する
	//! @return		属性
	tRisseDeclAttribute GetAttribute() const { return Attribute; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 1+1; // +2 = Body
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tRisseASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return SuperClass;
		if(index == 1) return Body;
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tRisseString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tRisseString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tRisseSSAVariable * DoReadSSA(tRisseSSAForm *form, void * param) const;

//--

	//! @brief		クラス宣言の表現を生成する
	//! @param		form		SSA 形式インスタンス
	//! @return		クラスを表す変数
	tRisseSSAVariable * GenerateClassDecl(tRisseSSAForm *form) const;

	//! @brief		デフォルトの initialize メソッドを表す AST を生成して返す
	//! @param		pos			スクリプト上の位置
	static tRisseASTNode * GenerateDefaultInitializeAST(risse_size pos);

	//! @brief		デフォルトの construct メソッドを表す AST を生成して返す
	//! @param		pos			スクリプト上の位置
	static tRisseASTNode * GenerateDefaultConstructAST(risse_size pos);
};
//---------------------------------------------------------------------------

#endif // #ifndef RISSE_AST_DEFINE_NAMES


//---------------------------------------------------------------------------
} // namespace Risse

#endif

