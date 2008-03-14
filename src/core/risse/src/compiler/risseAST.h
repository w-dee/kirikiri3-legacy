//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
	#define RISSE_AST_ENUM_DEF(X) enum tAST##X {
	#define RISSE_AST_ENUM_ITEM(PREFIX, X) PREFIX##X,
	#define RISSE_AST_ENUM_END };
#else
	#define RISSE_AST_ENUM_DEF(X) static const char * AST##X##Names[] = {
	#define RISSE_AST_ENUM_ITEM(PREFIX, X) #X,
	#define RISSE_AST_ENUM_END };
#endif

//---------------------------------------------------------------------------
//! @brief	ASTノードのタイプ
//---------------------------------------------------------------------------
RISSE_AST_ENUM_DEF(NodeType)
	RISSE_AST_ENUM_ITEM(ant, Context		)		//!< コンテキスト
	RISSE_AST_ENUM_ITEM(ant, Import			)		//!< import 文
	RISSE_AST_ENUM_ITEM(ant, ImportList		)		//!< import 文のリスト
	RISSE_AST_ENUM_ITEM(ant, ImportAs		)		//!< import のリスト中の as ペア
	RISSE_AST_ENUM_ITEM(ant, ImportLoc		)		//!< import のリスト中の位置指定子
	RISSE_AST_ENUM_ITEM(ant, Assert			)		//!< assert 文
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

class tScriptBlockInstance;
class tSSAForm;
class tASTNode;
class tSSAVariable;
class tSSAVariableAccessMap;
//---------------------------------------------------------------------------
//! @brief	ASTノードの配列
//---------------------------------------------------------------------------
typedef gc_vector<tASTNode *> tASTArray;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	ASTノードの基本クラス
//---------------------------------------------------------------------------
class tASTNode : public tCollectee
{
	risse_size Position; //!< ソースコード上の位置 (コードポイント数オフセット)
	tASTNodeType Type; //!< ノードタイプ
	tASTNode * Parent; //!< 親ノード

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
	tASTNode(risse_size position, tASTNodeType type) :
		Position(position), Type(type), Parent(NULL) {;}

	//! @brief		デストラクタ
	//! @note		このデストラクタは呼ばれない
	virtual ~tASTNode() {;}

public:
	//! @brief		ソースコード上の位置を得る
	//! @return		ソースコード上の位置
	risse_size GetPosition() const { return Position; }

	//! @brief		ソースコード上の位置を設定する
	//! @param		pos		ソースコード上の位置
	void SetPosition(risse_size pos) { Position = pos; }

	//! @brief		親ノードを設定する
	//! @param		parent		親ノード
	void SetParent(tASTNode * parent) { Parent = parent; }

	//! @brief		親ノードを得る
	//! @return		親ノード
	tASTNode * GetParent() const { return Parent; }

	//! @brief		ノードタイプを得る
	//! @return		ノードタイプ
	tASTNodeType GetType() const { return Type; }

public:
	//! @brief		子ノードの個数を得る(下位クラスで実装すること)
	//! @return		子ノードの個数
	virtual risse_size GetChildCount() const = 0;

	//! @brief		指定されたインデックスの子ノードを得る(下位クラスで実装すること)
	//! @param		index		インデックス
	//! @return		子ノード
	virtual tASTNode * GetChildAt(risse_size index) const = 0;

	//! @brief		指定されたインデックスの子ノードの名前を得る(下位クラスで実装すること)
	//! @param		index		インデックス
	//! @return		名前
	virtual tString GetChildNameAt(risse_size index) const = 0;

public:
	//! @brief		ダンプを行う
	//! @param		result		ダンプされた文字列
	//! @param		level		再帰レベル
	void Dump(tString & result, risse_int level = 0);

protected:
	//! @brief		ダンプ時のこのノードのコメントを得る(下位クラスで実装すること)
	//! @return		ダンプ時のこのノードのコメント
	virtual tString GetDumpComment() const = 0;

public:
	//! @brief		式から名前を得る
	//! @return		名前 (名前を特定できない場合は空文字列が返る)
	//! @note		この関数は、たとえば A というノードならば "A"、
	//!				A.B というノードならば "B" を返す。
	tString GetAccessTargetId();

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
	virtual void * PrepareSSA(tSSAForm *form, tPrepareMode mode) const
		{ return NULL; }

	//! @brief		SSA 形式の読み込み用の表現を生成する(必要に応じて下位クラスで実装すること)
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	//! @note		ここでは「読み込み用」と言っているが、書き込みではないすべての操作を意味する。
	//!				(たとえば if 文などもこのメソッドで SSA 形式に変換できる)
	virtual tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const
				{ return NULL; }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	//! @note		ここでは「読み込み用」と言っているが、書き込みではないすべての操作を意味する。
	//!				(たとえば if 文などもこのメソッドで SSA 形式に変換できる)
	tSSAVariable * GenerateReadSSA(tSSAForm *form) const
	{
	 	return DoReadSSA(form, PrepareSSA(form, pmRead));
	}

	//! @brief		SSA 形式の書き込み用の表現を生成する(必要に応じて下位クラスで実装すること)
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	//! @return		このノードが値の書き込みをサポートしているかどうか
	virtual bool DoWriteSSA(tSSAForm *form, void * param,
			tSSAVariable * value) const { return false; }

	//! @param		SSA 形式の書き込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	//! @return		書き込みが成功したかどうか
	bool GenerateWriteSSA(tSSAForm *form,
			tSSAVariable * value) const
	{  return DoWriteSSA(form, PrepareSSA(form, pmWrite), value); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	ノードの配列を伴うASTノードの基本クラス
//---------------------------------------------------------------------------
class tASTNode_List : public tASTNode
{
	tASTArray Array; //!< 配列

protected:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		type			ノードタイプ
	tASTNode_List(risse_size position, tASTNodeType type) :
		tASTNode(position, type) {;}

public:
	//! @brief		配列オブジェクトを得る
	//! @return		配列オブジェクト
	const tASTArray & GetArray() const { return Array; }

	//! @brief		配列オブジェクトを設定する
	//! @param		array	配列オブジェクト(null=配列のクリア)
	void AssignArray(const tASTArray * array)
	{
		if(array)
			Array = *array;
		else
			Array.clear();
	}

	//! @brief		配列に子ノードを追加する
	//! @param		node		追加したいノード
	void AddChild(tASTNode * node) { Array.push_back(node); if(node) node->SetParent(this); }

	//! @brief		最後の子ノードを削除する
	void PopChild()
	{
		Array.pop_back();
	}

	//! @brief		最後の子ノードを返す
	//! @return		最後の子ノード
	tASTNode * GetLastChild() const
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
	tASTNode * GetChildAt(risse_size index) const
	{
		if(index < Array.size())
			return Array[index];
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	式を一つだけ子にとるノードの基本クラス
//---------------------------------------------------------------------------
class tASTNode_OneExpression : public tASTNode
{
protected:
	tASTNode * Expression; //!< 式ノード

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		type			ノードタイプ
	//! @brief		expression		式ノード
	tASTNode_OneExpression(risse_size position, tASTNodeType type,
		tASTNode * expression) :
		tASTNode(position, type), Expression(expression)
	{
		if(Expression) Expression->SetParent(this);
	}

	//! @brief		式ノードを得る
	//! @return		式ノード
	tASTNode * GetExpression() const { return Expression; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 1;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Expression; else return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const
	{
		return tString(); // 空
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	子の無いノードの基本クラス
//---------------------------------------------------------------------------
class tASTNode_NoChildren : public tASTNode
{
public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		type			ノードタイプ
	tASTNode_NoChildren(risse_size position, tASTNodeType type) :
		tASTNode(position, type)
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
	tASTNode * GetChildAt(risse_size index) const
	{
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const { return tString(); }

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const
	{
		return tString(); // 空
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コンテキストのASTノード(type=antContext)
//---------------------------------------------------------------------------
class tASTNode_Context : public tASTNode_List
{
	tASTContextType ContextType; //!< コンテキストタイプ
	tString Name; //!< コンテキストの名前(説明用)
	risse_size EndPosition; //!< このコンテキストが終わる位置

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		context_type	コンテキストタイプ
	//! @param		name			コンテキストの名前
	tASTNode_Context(risse_size position, tASTContextType context_type,
		const tString & name) :
		tASTNode_List(position, antContext), ContextType(context_type),
		Name(name), EndPosition(position) {;}

	//! @brief		コンテキストタイプを得る
	//! @return		コンテキストタイプ
	tASTContextType GetContextType() const { return ContextType; }

	//! @brief		名前(説明用)を得る
	//! @return		名前(説明用)
	const tString & GetName() const { return Name; }

	//! @brief		コンテキストが終わる位置を設定する
	//! @param		pos			コンテキストが終わる位置
	void SetEndPosition(risse_size pos) { EndPosition = pos; }

	//! @brief		コンテキストが終わる位置を取得する
	//! @return		コンテキストが終わる位置
	risse_size GetEndPosition() const { return EndPosition; }

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	import 文(type=antImport)
//---------------------------------------------------------------------------
class tASTNode_Import : public tASTNode
{
	tASTNode * PackageList; //!< パッケージリスト
	tASTNode * IdList; //!< 識別子リスト

public:
	//! @brief		コンストラクタ
	//! @brief		position			ソースコード上の位置
	//! @param		package_list		パッケージリスト
	//! @param		id_list				識別子リスト
	tASTNode_Import(risse_size position,
		tASTNode * package_list,
		tASTNode * id_list) :
		tASTNode(position, antImport),
		PackageList(package_list),
		IdList(id_list)
	{
	}

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	virtual risse_size GetChildCount() const
	{
		return 2; // 子は二つ
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	virtual tASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return PackageList;
		if(index == 1) return IdList;
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	virtual tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	import 文のリスト(type=antImportList)
//---------------------------------------------------------------------------
class tASTNode_ImportList : public tASTNode_List
{
	typedef tASTNode_List inherited;

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tASTNode_ImportList(risse_size position) :
		tASTNode_List(position, antImportList) {;}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	import のリスト中の as ペア(type=antImportAs)
//---------------------------------------------------------------------------
class tASTNode_ImportAs : public tASTNode
{
	tASTNode * Name; //!< 名前を表すノード
	tASTNode * As; //!< 別名を表すノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			名前
	//! @param		as				別名を表すノード
	tASTNode_ImportAs(risse_size position,
		tASTNode * name, tASTNode * as) :
		tASTNode(position, antImportAs),
		Name(name),
		As(as) {;}

	//! @brief		名前を表すノードを得る
	//! @return		名前を表すノード
	tASTNode * GetName() const { return Name; }

	//! @brief		別名を表すノードを得る
	//! @return		別名を表すノード
	tASTNode * GetAs() const { return As; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
	{
		switch(index)
		{
		case 0: return Name;
		case 1: return As;
		}
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	import のリスト中の位置指定子(type=antImportLoc)
//---------------------------------------------------------------------------
class tASTNode_ImportLoc : public tASTNode_List
{
	typedef tASTNode_List inherited;

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tASTNode_ImportLoc(risse_size position) :
		tASTNode_List(position, antImportLoc) {;}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	assert文(type=antAssert)
//---------------------------------------------------------------------------
class tASTNode_Assert : public tASTNode_OneExpression
{
	tString		ExpressionString; //!< assert文の式(文字列)
public:
	//! @brief		コンストラクタ
	//! @brief		position			ソースコード上の位置
	//! @param		expression			式ノード
	//! @param		expression_string	式を表す文字列
	tASTNode_Assert(risse_size position,
		tASTNode * expression, const tString & expression_string) :
		tASTNode_OneExpression(position, antAssert, expression),
		ExpressionString(expression_string)
	{
	}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return ExpressionString; }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	アクセス時の属性キャスト(type=antCastAttr)
//---------------------------------------------------------------------------
class tASTNode_CastAttr : public tASTNode_OneExpression
{
	tDeclAttribute Attribute; //!< 属性
public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		attribute		属性
	//! @param		expression		式ノード
	tASTNode_CastAttr(risse_size position, tDeclAttribute attribute,
		tASTNode * expression) :
		tASTNode_OneExpression(position, antCastAttr, expression)
	{
		Attribute = attribute;
	}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return Attribute.AsString(); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	式ステートメントASTノード(type=antExprStmt)
//---------------------------------------------------------------------------
class tASTNode_ExprStmt : public tASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @brief		expression		式ノード
	tASTNode_ExprStmt(risse_size position, tASTNode * expression) :
		tASTNode_OneExpression(position, antExprStmt, expression) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	項ASTノード(type=antFactor)
//---------------------------------------------------------------------------
class tASTNode_Factor : public tASTNode
{
	tASTFactorType FactorType; //!< 項のタイプ
	tVariant Value; //!< 値

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		type			項のタイプ
	//! @param		value			項の値
	tASTNode_Factor(risse_size position, tASTFactorType factor_type,
		const tVariant & value = tVariant()) :
		tASTNode(position, antFactor), FactorType(factor_type), Value(value) {;}

	//! @brief		項のタイプを得る
	//! @return		項のタイプ
	tASTFactorType GetFactorType() const { return FactorType; }

	//! @brief		値を得る
	//! @return		値
	const tVariant & GetValue() const { return Value; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 0; // 子はない
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
	{
		return NULL; // 子はない
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const
	{
		return tString(); // 子はない
	}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	変数宣言ノード(type=antVarDecl)
//---------------------------------------------------------------------------
class tASTNode_VarDecl : public tASTNode_List
{
	typedef tASTNode_List inherited;

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tASTNode_VarDecl(risse_size position) :
		tASTNode_List(position, antVarDecl) {;}

	//! @brief		属性を設定する
	//! @param		attrib	属性
	//! @note		この操作は、すべての子ノード (tASTNode_VarDeclPair) に対して
	//!				属性を再設定する。これ以降に子に加わったノードに対しては
	//!				属性は自動的には設定されないので注意すること。
	void SetAttribute(tDeclAttribute attrib);

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	変数宣言の名前と初期値のノード(type=antVarDeclPair)
//---------------------------------------------------------------------------
class tASTNode_VarDeclPair : public tASTNode
{
	tASTNode * Name; //!< 名前を表すノード
	tASTNode * Initializer; //!< 初期値ノード
	tDeclAttribute Attribute; //!< 属性

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			名前
	//! @param		initializer		初期値ノード
	tASTNode_VarDeclPair(risse_size position,
		tASTNode * name, tASTNode * initializer);

	//! @brief		名前を得る
	//! @return		名前
	tASTNode * GetName() const { return Name; }

	//! @brief		初期値ノードを得る
	//! @return		初期値ノード
	tASTNode * GetInitializer() const { return Initializer; }

	//! @brief		属性を設定する
	//! @param		attrib	属性
	//! @note		この操作は、Name がメンバ選択演算子だった場合、そのメンバ選択演算子
	//!				にも同じ属性を設定する。
	void SetAttribute(tDeclAttribute attrib);

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return Attribute.AsString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;

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
	static void PrepareVarDecl(tSSAForm * form, const tASTNode * name);

	//! @brief		変数宣言のSSA 形式の表現を生成する
	//! @param		form		SSA 形式インスタンス
	//! @param		position	ソースコード上の位置
	//! @param		name		変数名
	//! @param		init		初期値を表すSSA形式変数
	//! @param		attrib		変数の属性
	//! @note		これに先立って PrepareVarDecl() を実行しておくこと。
	static void GenerateVarDecl(tSSAForm * form, risse_size position, const tASTNode * name,
			tSSAVariable * init,
			tDeclAttribute attrib = tMemberAttribute::GetDefault() );
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	メンバ選択ノード(type=antMemberSel)
//---------------------------------------------------------------------------
class tASTNode_MemberSel : public tASTNode
{
	tASTNode * Object; //!< オブジェクトノード
	tASTNode * MemberName; //!< メンバ名ノード
	tASTMemberAccessType AccessType ; //!< 演算子の種類
	tOperateFlags Flags; //!< メンバの操作フラグ(tObjectInterface::Opeate() に渡す物)
	tDeclAttribute Attribute; //!< メンバの書き込み時の属性設定値 @r
		//!< メンバへの書き込みのついでに属性を設定する場合のメンバの属性

private:
	//! @brief		PrepareSSA() で返す構造体
	struct tPrepareSSA : public tCollectee
	{
		tSSAVariable * ObjectVar; //!< オブジェクトの式の値
		tSSAVariable * MemberNameVar; //!< メンバ名を表す式の値
	};

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		object			オブジェクトノード
	//! @param		membername		メンバ名ノード
	//! @param		access_type		アクセス方法の種類
	//! @param		flags			メンバの操作フラグ
	tASTNode_MemberSel(risse_size position,
			tASTNode * object, tASTNode * membername, tASTMemberAccessType access_type,
				tOperateFlags flags = tOperateFlags()) :
		tASTNode(position, antMemberSel),
			Object(object), MemberName(membername), AccessType(access_type), Flags(flags)
	{
		if(Object) Object->SetParent(this);
		if(MemberName) MemberName->SetParent(this);
	}

	//! @brief		オブジェクトノードを得る
	//! @return		オブジェクトノード
	tASTNode * GetObject() const { return Object; }

	//! @brief		メンバ名ノードを得る
	//! @return		メンバ名ノード
	tASTNode * GetMemberName() const { return MemberName; }

	//! @brief		アクセス方法の種類を得る
	//! @return		アクセス方法の種類
	tASTMemberAccessType GetAccessType() const { return AccessType; }

	//! @brief		メンバの書き込み時の属性設定値を設定する
	//! @param		attrib		メンバの書き込み時の属性設定値
	void SetAttribute(tDeclAttribute attrib) { Attribute.Overwrite(attrib); }

	//! @brief		メンバの書き込み時の属性設定値を得る
	//! @return		メンバの書き込み時の属性設定値
	bool GetAttribute() const { return Attribute; }

	//! @brief		メンバの操作フラグを設定する
	//! @param		flags		メンバの操作フラグ
	void SetFlags(tOperateFlags flags) { Flags = flags; }

	//! @brief		メンバの操作フラグを得る
	//! @return		メンバの操作フラグ
	tOperateFlags GetFlags() const { return Flags; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用/書き込み用の表現の準備を行う
	//! @param		form	SSA 形式インスタンス
	//! @param		mode	読み込み用情報を生成するか、描き込み用情報を生成するか
	//! @return		読み込み/あるいは書き込みを行うための情報が入った構造体へのポインタ
	virtual void * PrepareSSA(tSSAForm *form, tPrepareMode mode) const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;

	//! @brief		SSA 形式の書き込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	bool DoWriteSSA(tSSAForm *form, void * param,
			tSSAVariable * value) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	識別子ASTノード(type=antId)
//---------------------------------------------------------------------------
class tASTNode_Id : public tASTNode
{
	tString Name; //!< 識別子名
	bool IsPrivate; //!< private (@つき) 変数

	//! @brief		PrepareSSA() で返す構造体
	struct tPrepareSSA : public tCollectee
	{
		const tASTNode_MemberSel * MemberSel;
			//!< ローカル変数ではなかったときには生成された tASTNode_MemberSel のインスタンス
			//!< ローカル変数の場合は NULL
		void * MemberSelParam; //!< tASTNode_MemberSel::PrepareSSA() が生成した情報
	};

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			識別子名
	//! @param		is_priv			private (@つき) 変数かどうか
	tASTNode_Id(risse_size position, const tString & name, bool is_priv) :
		tASTNode(position, antId), Name(name), IsPrivate(is_priv) {;}

	//! @brief		識別子名を得る
	//! @return		識別子名
	const tString & GetName() const { return Name; }

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
	tASTNode * GetChildAt(risse_size index) const
	{
		return NULL; // 子はない
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const
	{
		return tString(); // 子はない
	}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用/書き込み用の表現の準備を行う
	//! @param		form	SSA 形式インスタンス
	//! @param		mode	読み込み用情報を生成するか、描き込み用情報を生成するか
	//! @return		読み込み/あるいは書き込みを行うための情報が入った構造体へのポインタ
	void * PrepareSSA(tSSAForm *form, tPrepareMode mode) const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;

	//! @brief		SSA 形式の書き込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	bool DoWriteSSA(tSSAForm *form, void * param,
			tSSAVariable * value) const;

	//! @brief		この識別子がローカル名前空間に存在するかどうかを返す
	//! @param		form	SSA 形式インスタンス
	//! @return		この識別子がローカル名前空間に存在するかどうか
	bool ExistInLocalNamespace(tSSAForm * form) const;

	//! @brief		この識別子を this 内にアクセスする private 変数用のAST ノードを作成して返す
	//! @param		prefix		変数名の前につけるプリフィクス
	//! @param		write		書き込みの場合に真
	//! @return		この識別子を this 内にアクセスする private 変数用のAST ノード
	const tASTNode_MemberSel * CreatePrivateAccessNodeOnThis(const tString & prefix, bool write) const;

	//! @brief		この識別子を this-proxy 内にアクセスする AST ノードを作成して返す
	//! @return		この識別子を this-proxy 内にアクセスする AST ノード
	const tASTNode_MemberSel * CreateAccessNodeOnThisProxy() const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	単項演算子ノード(type=antUnary)
//---------------------------------------------------------------------------
class tASTNode_Unary : public tASTNode
{
	tASTUnaryType UnaryType; //!< 単項演算子のタイプ
	tASTNode * Child; //!< 子ノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		unary_type		単項演算子のタイプ
	//! @param		child			子ノード
	tASTNode_Unary(risse_size position, tASTUnaryType unary_type,
			tASTNode * child) :
		tASTNode(position, antUnary), UnaryType(unary_type), Child(child)
	{
		if(Child) Child->SetParent(this);
	}

	//! @brief		単項演算子のタイプを得る
	//! @return		単項演算子のタイプ
	tASTUnaryType GetUnaryType() const { return UnaryType; }

	//! @brief		子ノードを得る
	//! @return		子ノード
	tASTNode * GetChild() const { return Child; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 1;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Child; else return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	二項演算子ノード(type=antBinary)
//---------------------------------------------------------------------------
class tASTNode_Binary : public tASTNode
{
	tASTBinaryType BinaryType; //!< 二項演算子のタイプ
	tASTNode * Child1; //!< 子ノード1
	tASTNode * Child2; //!< 子ノード2

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		binary_type		二項演算子のタイプ
	//! @param		child1			子ノード1
	//! @param		child2			子ノード2
	tASTNode_Binary(risse_size position, tASTBinaryType binary_type,
			tASTNode * child1, tASTNode * child2) :
		tASTNode(position, antBinary), BinaryType(binary_type),
			Child1(child1), Child2(child2)
	{
		if(Child1) Child1->SetParent(this);
		if(Child2) Child2->SetParent(this);
	}

	//! @brief		二項演算子のタイプを得る
	//! @return		二項演算子のタイプ
	tASTBinaryType GetBinaryType() const { return BinaryType; }

	//! @brief		子ノード1を得る
	//! @return		子ノード1
	tASTNode * GetChild1() const { return Child1; }

	//! @brief		子ノード2を得る
	//! @return		子ノード2
	tASTNode * GetChild2() const { return Child2; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;

	//! @brief		&& 演算子と || 演算子に対する SSA 形式表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * GenerateLogicalAndOr(tSSAForm *form) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	三項演算子ノード(type=antTrinary)
//---------------------------------------------------------------------------
class tASTNode_Trinary : public tASTNode
{
	tASTTrinaryType TrinaryType; //!< 三項演算子のタイプ
	tASTNode * Child1; //!< 子ノード1
	tASTNode * Child2; //!< 子ノード2
	tASTNode * Child3; //!< 子ノード2

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		trinary_type	三項演算子のタイプ
	//! @param		child1			子ノード1
	//! @param		child2			子ノード2
	//! @param		child3			子ノード3
	tASTNode_Trinary(risse_size position, tASTTrinaryType trinary_type,
			tASTNode * child1, tASTNode * child2, tASTNode * child3) :
		tASTNode(position, antTrinary), TrinaryType(trinary_type),
			Child1(child1), Child2(child2), Child3(child3)
	{
		if(Child1) Child1->SetParent(this);
		if(Child2) Child2->SetParent(this);
		if(Child3) Child3->SetParent(this);
	}

	//! @brief		三項演算子のタイプを得る
	//! @return		三項演算子のタイプ
	tASTTrinaryType GetTrinaryType() const { return TrinaryType; }

	//! @brief		子ノード1を得る
	//! @return		子ノード1
	tASTNode * GetChild1() const { return Child1; }

	//! @brief		子ノード2を得る
	//! @return		子ノード2
	tASTNode * GetChild2() const { return Child2; }

	//! @brief		子ノード3を得る
	//! @return		子ノード3
	tASTNode * GetChild3() const { return Child3; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 3;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	InContextOf ノード
//---------------------------------------------------------------------------
class tASTNode_InContextOf : public tASTNode
{
	tASTNode * Instance; //!< インスタンスノード
	tASTNode * Context; //!< コンテキストノード (NULL=dynamicのとき)

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		instance		インスタンス
	//! @param		context			コンテキスト (NULL = dynamicのとき)
	tASTNode_InContextOf(risse_size position,
			tASTNode * instance, tASTNode * context) :
		tASTNode(position, antInContextOf),
			Instance(instance), Context(context)
	{
		if(Instance) Instance->SetParent(this);
		if(Context) Context->SetParent(this);
	}

	//! @brief		インスタンスノードを得る
	//! @return		インスタンスノード
	tASTNode * GetInstance() const { return Instance; }

	//! @brief		コンテキストノードを得る
	//! @return		コンテキストノード
	tASTNode * GetContext() const { return Context; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	インライン配列ノード(type=antArray)
//---------------------------------------------------------------------------
class tASTNode_Array : public tASTNode_List
{
	typedef tASTNode_List inherited;

	//! @brief		PrepareSSA() で返す構造体
	struct tPrepareSSA : public tCollectee
	{
		tPrepareMode Mode;
		gc_vector<void *> Elements; //!< 各要素の準備用データ
		gc_vector<tSSAVariable *> Indices; //!< 各インデックスを表す数値定数
	};

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tASTNode_Array(risse_size position) :
		tASTNode_List(position, antArray) {;}

	//! @brief		配列の最後の null を削除する
	void Strip();

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用/書き込み用の表現の準備を行う
	//! @param		form	SSA 形式インスタンス
	//! @param		mode	読み込み用情報を生成するか、描き込み用情報を生成するか
	//! @return		読み込み/あるいは書き込みを行うための情報が入った構造体へのポインタ
	void * PrepareSSA(tSSAForm *form, tPrepareMode mode) const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;

	//! @brief		SSA 形式の書き込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	bool DoWriteSSA(tSSAForm *form, void * param,
			tSSAVariable * value) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	インライン辞書配列ノード(type=antDict)
//---------------------------------------------------------------------------
class tASTNode_Dict : public tASTNode_List
{
	typedef tASTNode_List inherited;

	//! @brief		PrepareSSA() で返す構造体
	struct tPrepareSSA : public tCollectee
	{
		gc_vector<void *> Names; //!< 各要素の「名前」の準備用データ
		gc_vector<void *> Values; //!< 各要素の「値」の準備用データ
	};

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tASTNode_Dict(risse_size position) :
		tASTNode_List(position, antDict) {;}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用/書き込み用の表現の準備を行う
	//! @param		form	SSA 形式インスタンス
	//! @param		mode	読み込み用情報を生成するか、描き込み用情報を生成するか
	//! @return		読み込み/あるいは書き込みを行うための情報が入った構造体へのポインタ
	void * PrepareSSA(tSSAForm *form, tPrepareMode mode) const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;

	//! @brief		SSA 形式の書き込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @param		var		SSA 形式における変数 (この結果が書き込まれる)
	bool DoWriteSSA(tSSAForm *form, void * param,
			tSSAVariable * value) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	インライン辞書の子ノード(type=antDictPair)
//---------------------------------------------------------------------------
class tASTNode_DictPair : public tASTNode
{
	tASTNode * Name; //!< 名前ノード
	tASTNode * Value; //!< 値ノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			名前ノード
	//! @param		value			値ノード
	tASTNode_DictPair(risse_size position,
		tASTNode * name, tASTNode * value) :
		tASTNode(position, antDictPair),
			Name(name), Value(value)
	{
		if(Name) Name->SetParent(this);
		if(Value) Value->SetParent(this);
	}

	//! @brief		名前ノードを得る
	//! @return		名前ノード
	tASTNode * GetName() const { return Name; }

	//! @brief		値ノードを得る
	//! @return		値ノード
	tASTNode * GetValue() const { return Value; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	正規表現パターンASTノード(type=antRegExp)
//---------------------------------------------------------------------------
class tASTNode_RegExp : public tASTNode_NoChildren
{
	tString Pattern; //!< 正規表現パターン
	tString Flags; //!< 正規表現フラグ

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		pattern			正規表現パターン
	//! @param		flags			正規表現フラグ
	tASTNode_RegExp(risse_size position,
		const tString & pattern, const tString & flags) :
		tASTNode_NoChildren(position, antRegExp), Pattern(pattern), Flags(flags)
	{;}

	//! @brief		正規表現パターンを得る
	//! @return		正規表現パターン
	const tString & GetPattern() const { return Pattern; }

	//! @brief		正規表現フラグを得る
	//! @return		正規表現フラグ
	const tString & GetFlags() const { return Flags; }

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	if (とelse)(type=antIf)
//---------------------------------------------------------------------------
class tASTNode_If : public tASTNode
{
	tASTNode * Condition; //!< 条件式
	tASTNode * True; //!< 条件が真の時に実行するノード
	tASTNode * False; //!< 条件が偽の時に実行するノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		condition		条件式
	//! @param		true			条件が真の時に実行するノード
	tASTNode_If(risse_size position, tASTNode * condition,
		tASTNode * truenode) :
		tASTNode(position, antIf),
			Condition(condition), True(truenode)
	{
		if(Condition) Condition->SetParent(this);
		if(True) True->SetParent(this);
		False = NULL;
	}

	//! @brief		条件式ノードを得る
	//! @return		条件式ノード
	tASTNode * GetCondition() const { return Condition; }

	//! @brief		条件が真の時に実行するノードを得る
	//! @return		条件が真の時に実行するノード
	tASTNode * GetTrue() const { return True; }

	//! @brief		条件が偽の時に実行するノードを得る
	//! @return		条件が偽の時に実行するノード
	tASTNode * GetFalse() const { return False; }

	//! @brief		条件が偽の時に実行するノードを設定する
	//! @param		falsenode	条件が偽の時に実行するノード
	void SetFalse(tASTNode * falsenode)
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
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する(このクラスと ? : 演算子で使われる)
	//! @param		form		SSA 形式インスタンス
	//! @param		pos			ソースコード上の位置
	//! @param		basenamse	ブロックの名前のprefixにする基本名
	//! @param		condition	条件式ノード
	//! @param		truenode	条件が真の時に実行するノード
	//! @param		falsenode	条件が偽の時に実行するノード
	//! @param		needresult	結果が必要かどうか (  ? : 演算子では真を指定する )
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	static tSSAVariable * InternalDoReadSSA(
		tSSAForm *form,
		risse_size pos,
		const tString &basename,
		tASTNode * condition,
		tASTNode * truenode,
		tASTNode * falsenode,
		bool needresult
		);

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	while文のノード(type=antWhile)
//---------------------------------------------------------------------------
class tASTNode_While : public tASTNode
{
	tASTNode * Condition; //!< 条件式ノード
	tASTNode * Body; //!< 条件が真の間実行するノード
	bool SkipFirstCheck; //!< 最初の式チェックを省略するかどうか (do～whileかどうか)

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		condition		条件式ノード
	//! @param		body			条件が真の間実行するノード
	//! @param		skipfirstcheck	最初の式チェックを省略するかどうか (do～whileかどうか)
	tASTNode_While(risse_size position,
		tASTNode * condition, tASTNode * body, bool skipfirstcheck) :
		tASTNode(position, antWhile),
			Condition(condition), Body(body), SkipFirstCheck(skipfirstcheck)
	{
		if(Condition) Condition->SetParent(this);
		if(Body) Body->SetParent(this);
	}

	//! @brief		条件式ノードを得る
	//! @return		条件式ノード
	tASTNode * GetCondition() const { return Condition; }

	//! @brief		条件が真の間実行するノードを得る
	//! @return		条件が真の間実行するノード
	tASTNode * GetBody() const { return Body; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	for文のノード(type=antFor)
//---------------------------------------------------------------------------
class tASTNode_For : public tASTNode
{
	tASTNode * Initializer; //!< 初期化ノード(第1節)
	tASTNode * Condition; //!< 継続条件ノード(第2節)
	tASTNode * Iterator; //!< 増分処理ノード(第3節)
	tASTNode * Body; //!< 条件が真の間実行するノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		initializer		初期化ノード
	//! @param		condition		継続条件ノード
	//! @param		iterator		増分処理ノード
	//! @param		body			条件が真の間実行するノード
	tASTNode_For(risse_size position,
		tASTNode * initializer, tASTNode * condition, tASTNode * iterator,
		tASTNode * body) :
		tASTNode(position, antFor),
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
	tASTNode * GetInitializer() const { return Initializer; }

	//! @brief		継続条件ノードを得る
	//! @return		継続条件ノード
	tASTNode * GetCondition() const { return Condition; }

	//! @brief		増分処理ノードを得る
	//! @return		増分処理ノード
	tASTNode * GetIterator() const { return Iterator; }

	//! @brief		条件が真の間実行するノードを得る
	//! @return		条件が真の間実行するノード
	tASTNode * GetBody() const { return Body; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 4;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	return文ノード(type=antReturn)
//---------------------------------------------------------------------------
class tASTNode_Return : public tASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		expression		式ノード
	tASTNode_Return(risse_size position, tASTNode * expression) :
		tASTNode_OneExpression(position, antReturn, expression) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	throw文ノード(type=antThrow)
//---------------------------------------------------------------------------
class tASTNode_Throw : public tASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		expression		式ノード
	tASTNode_Throw(risse_size position, tASTNode * expression) :
		tASTNode_OneExpression(position, antThrow, expression) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	break文ノード(type=antBreak)
//---------------------------------------------------------------------------
class tASTNode_Break : public tASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		expression		式ノード
	tASTNode_Break(risse_size position, tASTNode * expression) :
		tASTNode_OneExpression(position, antBreak, expression) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	continue文ノード(type=antContinue)
//---------------------------------------------------------------------------
class tASTNode_Continue : public tASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		expression		式ノード
	tASTNode_Continue(risse_size position, tASTNode * expression) :
		tASTNode_OneExpression(position, antContinue, expression) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	debugger文ノード(type=antDebugger)
//---------------------------------------------------------------------------
class tASTNode_Debugger : public tASTNode_NoChildren
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tASTNode_Debugger(risse_size position) :
		tASTNode_NoChildren(position, antDebugger) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	with文などの基本クラス
//---------------------------------------------------------------------------
class tASTNode_ExpressionBlock : public tASTNode
{
protected:
	tASTNode * Object; //!< オブジェクトノード
	tASTNode * Body; //!< ブロックまたは文のノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		type			ノードタイプ
	//! @param		object			オブジェクトノード
	//! @param		body			ブロックまたは文のノード
	tASTNode_ExpressionBlock(risse_size position,
		tASTNodeType type,
		tASTNode * object, tASTNode * body) :
		tASTNode(position, type),
			Object(object), Body(body)
	{
		if(Object) Object->SetParent(this);
		if(Body) Body->SetParent(this);
	}

	//! @brief		オブジェクトノードを得る
	//! @return		オブジェクトノード
	tASTNode * GetObject() const { return Object; }

	//! @brief		ブロックまたは文のノードを得る
	//! @return		ブロックまたは文のノード
	tASTNode * GetBody() const { return Body; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	with文ノード(type=antWith)
//---------------------------------------------------------------------------
class tASTNode_With : public tASTNode_ExpressionBlock
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		object			オブジェクトノード
	//! @param		body			ブロックまたは文のノード
	tASTNode_With(risse_size position,
		tASTNode * object, tASTNode * body) :
		tASTNode_ExpressionBlock(position, antWith, object, body) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	synchronized文ノード(type=antSynchronized)
//---------------------------------------------------------------------------
class tASTNode_Synchronized : public tASTNode_ExpressionBlock
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		object			オブジェクトノード
	//! @param		body			ブロックまたは文のノード
	tASTNode_Synchronized(risse_size position,
		tASTNode * object, tASTNode * body) :
		tASTNode_ExpressionBlock(position, antSynchronized, object, body) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	using文ノード(type=antUsing)
//---------------------------------------------------------------------------
class tASTNode_Using : public tASTNode_ExpressionBlock
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		object			オブジェクトノード
	//! @param		body			ブロックまたは文のノード
	tASTNode_Using(risse_size position,
		tASTNode * object, tASTNode * body) :
		tASTNode_ExpressionBlock(position, antUsing, object, body) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	ラベルノード(type=antLabel)
//---------------------------------------------------------------------------
class tASTNode_Label : public tASTNode_NoChildren
{
	tString Name; //!< ラベル名

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		name			ラベル名
	tASTNode_Label(risse_size position, const tString & name) :
		tASTNode_NoChildren(position, antLabel), Name(name) {;}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const
	{
		return Name.AsHumanReadable();
	}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	goto ノード(type=antGoto)
//---------------------------------------------------------------------------
class tASTNode_Goto : public tASTNode_NoChildren
{
	tString Name; //!< ジャンプ先ラベル名

public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		name			ジャンプ先ラベル名
	tASTNode_Goto(risse_size position, const tString & name) :
		tASTNode_NoChildren(position, antGoto), Name(name) {;}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const
	{
		return Name.AsHumanReadable();
	}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	switch文ノード(type=antSwitch)
//---------------------------------------------------------------------------
class tASTNode_Switch : public tASTNode_ExpressionBlock
{
public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		object			オブジェクトノード
	//! @param		body			ブロックまたは文のノード
	tASTNode_Switch(risse_size position,
		tASTNode * object, tASTNode * body) :
		tASTNode_ExpressionBlock(position, antSwitch, object, body) {;}

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	case  / default ノード(type=antCase)
//---------------------------------------------------------------------------
class tASTNode_Case : public tASTNode_OneExpression
{
public:
	//! @brief		コンストラクタ
	//! @brief		position		ソースコード上の位置
	//! @param		expression		式ノード
	tASTNode_Case(risse_size position, tASTNode * expression) :
		tASTNode_OneExpression(position, antCase, expression) {;}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	tryノード(type=antTry)
//---------------------------------------------------------------------------
class tASTNode_Try : public tASTNode_List
{
	typedef tASTNode_List inherited;
	tASTNode *Body; //!< tryブロック
	tASTNode *Finally; //!< finallyブロック

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tASTNode_Try(risse_size position) :
		tASTNode_List(position, antTry)
	{
		Body = NULL;
		Finally = NULL;
	}

	//! @brief		tryブロックを得る
	//! @return		tryブロック
	bool GetBody() const { return Body; }

	//! @brief		tryブロックを設定する
	//! @param		body	tryブロック
	void SetBody(tASTNode * body)
	{
		Body = body;
		if(Body) Body->SetParent(this);
	}

	//! @brief		finallyブロックを得る
	//! @return		finallyブロック
	bool GetFinally() const { return Finally; }

	//! @brief		finallyブロックを設定する
	//! @param		finallyblock	finallyブロック
	void SetFinally(tASTNode * finallyblock)
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
	tASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Body;
		if(index == inherited::GetChildCount() + 1) return Finally;
		return inherited::GetChildAt(index - 1);
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const { return tString(); }

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;

//--
private:
	//! @brief		try-catchブロックの内容を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		is_finally	finally用の生成を行うか
	//! @return		このブロックの結果を表す SSA 形式変数
	tSSAVariable * GenerateTryCatchOrFinally(tSSAForm *form, bool is_finally) const;

	//! @brief		catchブロックを生成する
	//! @param		form				SSA 形式インスタンス
	//! @param		try_block_ret_var	tryブロックの戻り値
	void GenerateCatchBlock(tSSAForm * form,
		tSSAVariable * try_block_ret_var) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	catch ノード (type=antCatch)
//---------------------------------------------------------------------------
class tASTNode_Catch : public tASTNode
{
	tString Name; //!< 変数名(空 = 変数を受け取らない)
	tASTNode * Condition; //!< 条件ノード (NULL = 無条件)
	tASTNode * Body; //!< catch のbodyノード

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			変数名
	//! @param		condition		条件ノード
	//! @param		body			body ノード
	tASTNode_Catch(risse_size position,
		const tString & name, tASTNode * condition, tASTNode * body) :
		tASTNode(position, antCatch),
			Name(name), Condition(condition), Body(body)
	{
		if(Condition) Condition->SetParent(this);
		if(Body) Body->SetParent(this);
	}

	//! @brief		変数名を得る
	//! @return		変数名
	const tString & GetName() const { return Name; }

	//! @brief		条件ノードを得る
	//! @return		条件ノード
	tASTNode * GetCondition() const { return Condition; }

	//! @brief		body ノードを得る
	//! @return		body ノード
	tASTNode * GetBody() const { return Body; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 2;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	関数呼び出しのASTノード(type=antFuncCall)
//! @note	new 演算子つきの関数呼び出しはオブジェクト生成とみなし、
//!			関数呼び出し用ASTノードと共用する(GetCreateNewで調べる)
//---------------------------------------------------------------------------
class tASTNode_FuncCall : public tASTNode_List
{
	typedef tASTNode_List inherited;
	tASTNode * Expression; //!< 関数を表す式ノード
	bool CreateNew; //!< new による関数呼び出しかどうか
	bool Omit; //!< 引数省略かどうか
	tASTArray Blocks; //!< ブロック引数の配列

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		omit			引数省略かどうか
	tASTNode_FuncCall(risse_size position, bool omit) :
		tASTNode_List(position, antFuncCall), Omit(omit)
	{
		Expression = NULL;
	}

	//! @brief		式ノードを設定する
	//! @param		node		式ノード
	void SetExpression(tASTNode * node)
		{ Expression = node; Expression->SetParent(this); }

	//! @brief		式ノードを得る
	//! @return		式ノード
	tASTNode * GetExpression() const { return Expression; }

	//! @brief		new による関数呼び出しかどうかを設定する
	//! @param		b	new による関数呼び出しかどうか
	void SetCreateNew(bool b = true) { CreateNew = b; }

	//! @brief		new による関数呼び出しかどうかを得る
	//! @return		new による関数呼び出しかどうか
	bool GetCreateNew() const { return CreateNew; }

	//! @brief		ブロック引数を追加する
	//! @param		list		ブロック引数の配列(null=何もしない)
	void AddBlocks(const tASTArray * array)
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
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	関数呼び出しのの引数を表すノード(type=antFuncCallArg)
//---------------------------------------------------------------------------
class tASTNode_FuncCallArg : public tASTNode
{
	tASTNode * Expression; //!< 引数を表す式ノード(NULL = 引数なし)
	bool Expand; //!< 配列の引数への展開を行うかどうか

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		expression		引数を表す式ノード
	//! @param		expand			配列の引数への展開を行うかどうか
	tASTNode_FuncCallArg(risse_size position, tASTNode * expression, bool expand) :
		tASTNode(position, antFuncCallArg), Expression(expression), Expand(expand)
	{
		if(Expression) Expression->SetParent(this);
	}

	//! @brief		式ノードを得る
	//! @return		式ノード
	tASTNode * GetExpression() const { return Expression; }

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
	tASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Expression; else return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	関数宣言のASTノード(type=antFuncDecl)
//---------------------------------------------------------------------------
class tASTNode_FuncDecl : public tASTNode_List
{
	typedef tASTNode_List inherited;
	tASTNode * Body; //!< 関数ボディ
	tString Name; //!< 関数名
	tDeclAttribute Attribute; //!< 属性
	tASTArray Blocks; //!< ブロック引数の配列
	bool IsBlock; //!< 遅延評価ブロックブロックかどうか(真=遅延評価ブロック,偽=普通の関数)

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tASTNode_FuncDecl(risse_size position) :
		tASTNode_List(position, antFuncDecl)
	{
		Body = NULL;
		IsBlock = false;
	}

	//! @brief		関数ボディを設定する
	//! @param		node		関数ボディ
	void SetBody(tASTNode * node)
		{ Body = node; Body->SetParent(this); }

	//! @brief		関数ボディを得る
	//! @return		関数ボディ
	tASTNode * GetBody() const { return Body; }

	//! @brief		関数名を設定する
	//! @param		name	関数名
	void SetName(const tString & name) { Name = name; }

	//! @brief		関数名を得る
	//! @return		関数名
	const tString & GetName() const { return Name; }

	//! @brief		ブロック引数の配列を追加する
	//! @param		array	ブロック引数の配列(NULL=なにもしない)
	void AddBlocks(const tASTArray * array)
	{
		if(array)
			Blocks.insert(Blocks.end(), array->begin(), array->end());
	}

	//! @brief		属性を設定する
	//! @param		attrib	属性
	void SetAttribute(tDeclAttribute attrib) { Attribute.Overwrite(attrib); }

	//! @brief		属性を設定する
	//! @return		属性
	tDeclAttribute GetAttribute() const { return Attribute; }

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
	tASTNode * GetChildAt(risse_size index) const
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
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;

//--

	//! @brief		関数宣言の表現を生成する
	//! @param		form		SSA 形式インスタンス
	//! @param		access_map	アクセスマップ (IsBlock=真の場合非null必須)
	//! @param		try_id		try識別子(risse_size_maxは指定無しの場合)
	//! @return		関数を表す変数
	tSSAVariable * GenerateFuncDecl(tSSAForm *form,
		tSSAVariableAccessMap *access_map = NULL, risse_size try_id = risse_size_max) const;


	//! @brief		属性に従って関数オブジェクトの設定を行う
	//! @param		form			SSA 形式インスタンス
	//! @param		position		ソースコード上の位置
	//! @param		function		関数オブジェクトを表すSSA形式変数
	//! @param		attr			属性
	static void ApplyMethodAttribute(tSSAForm * form, risse_size position,
		tSSAVariable *& function, tDeclAttribute attrib);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	関数宣言の引数を表すノード(type=antFuncDeclArg)
//---------------------------------------------------------------------------
class tASTNode_FuncDeclArg : public tASTNode
{
	tString Name; //!< 引数名
	tASTNode * Initializer; //!< 初期値を表すノード(NULL = 初期値無し)
	bool Collapse; //!< 引数の配列への圧縮を行うかどうか

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			引数名
	//! @param		initializer		初期値を表すノード
	//! @param		collapse		引数の配列への圧縮を行うかどうか
	tASTNode_FuncDeclArg(risse_size position, const tString & name,
		tASTNode * initializer, bool collapse) :
		tASTNode(position, antFuncDeclArg), Name(name),
		Initializer(initializer), Collapse(collapse)
	{
		if(Initializer) Initializer->SetParent(this);
	}

	//! @brief		初期値を表すノードを得る
	//! @return		初期値を表すノード
	tASTNode * GetInitializer() const { return Initializer; }

	//! @brief		引数の配列への圧縮を行うかどうかを得る
	//! @return		引数の配列への圧縮を行うかどうか
	bool GetCollapse() const { return Collapse; }

	//! @brief		引数名を得る
	//! @return		引数名
	tString GetName() const { return Name; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 1;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Initializer; else return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	関数宣言のブロック引数を表すノード(type=antFuncDeclBlock)
//---------------------------------------------------------------------------
class tASTNode_FuncDeclBlock : public tASTNode
{
	tString Name; //!< ブロック名

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		name			ブロック名
	tASTNode_FuncDeclBlock(risse_size position, const tString & name) :
		tASTNode(position, antFuncDeclBlock), Name(name)
	{
		;
	}

	//! @brief		ブロック名を得る
	//! @return		ブロック名
	tString GetName() const { return Name; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 0;
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
	{
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const
	{
		return tString();
	}

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const { return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	プロパティ宣言のASTノード(type=antPropDecl)
//---------------------------------------------------------------------------
class tASTNode_PropDecl : public tASTNode
{
	tASTNode * Setter; //!< セッタ
	tString SetterArgumentName; //!< セッタの引数の名前
	tASTNode * Getter; //!< ゲッタ
	tDeclAttribute Attribute; //!< 属性
	tString Name; //!< プロパティ名

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	tASTNode_PropDecl(risse_size position) :
		tASTNode(position, antPropDecl)
	{
		Setter = Getter = NULL;
	}

	//! @brief		セッタを設定する
	//! @param		node		セッタ
	void SetSetter(tASTNode * node)
	{
		Setter = node; if(Setter) Setter->SetParent(this);
	}

	//! @brief		セッタを得る
	//! @return		セッタ
	tASTNode * GetSetter(void) const
	{
		return Setter;
	}

	//! @brief		セッタの引数の名前を設定するもちろん
	//! @param		name	セッタの引数の名前
	void SetSetterArgumentName(const tString & name) { SetterArgumentName = name; }

	//! @brief		セッタの引数の名前を得る
	//! @return		セッタの引数の名前
	tString GetSetterArgumentName() const { return SetterArgumentName; }

	//! @brief		ゲッタを設定する
	//! @param		node		ゲッタ
	void SetGetter(tASTNode * node)
	{
		Getter = node; if(Getter) Getter->SetParent(this);
	}

	//! @brief		ゲッタを得る
	//! @return		ゲッタ
	tASTNode * GetGetter(void) const
	{
		return Getter;
	}

	//! @brief		プロパティ名を設定する
	//! @param		name	プロパティ名
	void SetName(const tString & name) { Name = name; }

	//! @brief		プロパティ名を得る
	//! @return		プロパティ名
	const tString & GetName() const { return Name; }


	//! @brief		属性を設定する
	//! @param		attrib	属性
	void SetAttribute(tDeclAttribute attrib) { Attribute.Overwrite(attrib); }

	//! @brief		属性を設定する
	//! @return		属性
	tDeclAttribute GetAttribute() const { return Attribute; }

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
	tASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return Setter;
		if(index == 1) return Getter;
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;
//--

	//! @brief		プロパティ宣言の表現を生成する
	//! @param		form		SSA 形式インスタンス
	//! @return		プロパティを表す変数
	tSSAVariable * GeneratePropertyDecl(tSSAForm *form) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	クラス宣言/モジュール宣言のASTノード(type=antClassDecl)
//---------------------------------------------------------------------------
class tASTNode_ClassDecl : public tASTNode
{
	typedef tASTNode inherited;
	bool IsModule; //!< モジュール宣言の場合に真
	tASTNode * SuperClass; //!< 親クラスを表す式
	tASTNode * Body; //!< クラス宣言ボディ
	tString Name; //!< クラス名
	tDeclAttribute Attribute; //!< 属性

public:
	//! @brief		コンストラクタ
	//! @param		position		ソースコード上の位置
	//! @param		is_module		モジュール宣言かどうか
	//! @param		super_class		親クラスを表す式(is_module = false の時だけ)
	tASTNode_ClassDecl(risse_size position, bool is_module, tASTNode * super_class = NULL) :
		tASTNode(position, antClassDecl)
	{
		IsModule = is_module;
		SuperClass = super_class;
		Body = NULL;
	}

	//! @brief		クラスボディを設定する
	//! @param		node		クラスボディ
	void SetBody(tASTNode * node)
		{ Body = node; Body->SetParent(this); }

	//! @brief		クラスボディを得る
	//! @return		クラスボディ
	tASTNode * GetBody() const { return Body; }

	//! @brief		モジュール宣言かどうかを得る
	//! @return		モジュール宣言
	bool GetIsModule() const { return IsModule; }

	//! @brief		クラス名を設定する
	//! @param		name	クラス名
	void SetName(const tString & name) { Name = name; }

	//! @brief		クラス名を得る
	//! @return		クラス名
	const tString & GetName() const { return Name; }

	//! @brief		属性を設定する
	//! @param		attrib	属性
	void SetAttribute(tDeclAttribute attrib) { Attribute.Overwrite(attrib); }

	//! @brief		属性を設定する
	//! @return		属性
	tDeclAttribute GetAttribute() const { return Attribute; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const
	{
		return 1+1; // +2 = Body
	}

	//! @brief		指定されたインデックスの子ノードを得る
	//! @param		index		インデックス
	//! @return		子ノード
	tASTNode * GetChildAt(risse_size index) const
	{
		if(index == 0) return SuperClass;
		if(index == 1) return Body;
		return NULL;
	}

	//! @brief		指定されたインデックスの子ノードの名前を得る
	//! @param		index		インデックス
	//! @return		名前
	tString GetChildNameAt(risse_size index) const;

	//! @brief		ダンプ時のこのノードのコメントを得る
	//! @return		ダンプ時のこのノードのコメント
	tString GetDumpComment() const;

	//! @brief		SSA 形式の読み込み用の表現を生成する
	//! @param		form	SSA 形式インスタンス
	//! @param		param	PrepareSSA() の戻り値
	//! @return		SSA 形式における変数 (このノードの結果が格納される)
	tSSAVariable * DoReadSSA(tSSAForm *form, void * param) const;

//--

	//! @brief		クラス宣言の表現を生成する
	//! @param		form		SSA 形式インスタンス
	//! @return		クラスを表す変数
	tSSAVariable * GenerateClassDecl(tSSAForm *form) const;

	//! @brief		デフォルトの initialize メソッドを表す AST を生成して返す
	//! @param		pos			スクリプト上の位置
	static tASTNode * GenerateDefaultInitializeAST(risse_size pos);

	//! @brief		デフォルトの construct メソッドを表す AST を生成して返す
	//! @param		pos			スクリプト上の位置
	static tASTNode * GenerateDefaultConstructAST(risse_size pos);
};
//---------------------------------------------------------------------------

#endif // #ifndef RISSE_AST_DEFINE_NAMES


//---------------------------------------------------------------------------
} // namespace Risse

#endif

