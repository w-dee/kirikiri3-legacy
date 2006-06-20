//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイトコードジェネレータ
//---------------------------------------------------------------------------
#ifndef risseCodeGenH
#define risseCodeGenH

#include "risseGC.h"
#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAST.h"
#include "risseVariant.h"
#include "risseOpCodes.h"

//---------------------------------------------------------------------------
/*
	risseでは、ASTに対して、一度 SSA 形式ライクな中間形式に変換を行ってから、
	それを VM バイトコードに変換を行う。
*/
//---------------------------------------------------------------------------
namespace Risse
{
class tRisseSSABlock;
class tRisseSSAVariable;
class tRisseSSAForm;
//---------------------------------------------------------------------------
//! @brief	ローカル変数用のフラットな名前空間管理クラス
//---------------------------------------------------------------------------
class tRisseSSAFlatNamespace : public tRisseCollectee
{
	//! @brief		名前に関する情報
	struct tInfo
	{
		tInfo() { Read = false; Write = false; } //!< コンストラクタ
		bool Read;		//!< この変数に対する読み込みが発生したかどうか(使用フラグ)
		bool Write;		//!< この変数に対する書き込みが発生したかどうか(使用フラグ)
	};
	typedef gc_map<tRisseString, tInfo> tMap; //!< 変数名(番号なし)→情報のマップのtypedef

	tMap Map; //!< 変数名(番号なし)→情報のマップ

public:
	//! @brief		コンストラクタ
	tRisseSSAFlatNamespace() {;}

	//! @brief		変数が存在するかどうかを調べる
	//! @param		name		変数名(番号なし)
	//! @return		変数が存在する場合真、存在しない場合偽
	bool Find(const tRisseString & name) const;

	//! @brief		変数が存在するかどうかを調べ、存在すれば使用フラグを真に設定する
	//! @param		name		変数名(番号なし)
	//! @param		write		その変数に対するアクセスが書き込みか(真)、読み込みか(偽)
	//! @return		変数が存在する場合真、存在しない場合偽
	bool FindAndSetUsed(const tRisseString & name, bool write);

	//! @brief		変数を追加する
	//! @param		name		変数名(番号なし)
	void Add(const tRisseString & name);

	//! @param		遅延評価ブロック中で「読み込み」が発生した変数に対して読み込みを行う文を作成する
	//! @param		form		SSA形式インスタンス
	//! @param		pos			スクリプト上の位置
	//! @param		block_var	遅延評価ブロックを表す変数
	void GenerateChildRead(tRisseSSAForm * form, risse_size pos,
		tRisseSSAVariable* block_var);

	//! @param		遅延評価ブロック中で「書き込み」が発生した変数に対して読み込みを行う文を作成する
	//! @param		form		SSA形式インスタンス
	//! @param		pos			スクリプト上の位置
	//! @param		block_var	遅延評価ブロックを表す変数
	void GenerateChildWrite(tRisseSSAForm * form, risse_size pos,
		tRisseSSAVariable* block_var);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	ローカル変数用の階層化された名前空間管理クラス
//---------------------------------------------------------------------------
class tRisseSSALocalNamespace : public tRisseCollectee
{
	tRisseSSABlock * Block; //!< この名前空間に結びつけられている基本ブロック
	tRisseSSAFlatNamespace * Chained; //!< チェーンされた名前空間
	typedef gc_map<tRisseString, tRisseSSAVariable *> tVariableMap;
		//!< 変数名(番号付き)→変数オブジェクトのマップのtypedef
	typedef gc_map<tRisseString, tRisseString> tAliasMap;
		//!< 変数名(番号なし)→変数名(番号付き)のマップのtypedef

	//! @brief		名前空間の一つのスコープを表す構造体
	struct tScope
	{
		tVariableMap VariableMap; //!< 変数名(番号付き)→変数オブジェクトのマップ
		tAliasMap AliasMap; //!< 変数名(番号なし)→変数名(番号付き)のマップ
	};
	typedef gc_vector<tScope *> tScopes; //!< スコープの typedefs
	tScopes Scopes; //!< 名前空間のスコープ

public:
	//! @brief		コンストラクタ
	tRisseSSALocalNamespace();

	//! @brief		コピーコンストラクタ
	tRisseSSALocalNamespace(const tRisseSSALocalNamespace &ref);

	//! @brief		チェーンされた名前空間を設定する
	//! @param		ns		チェーンされた名前空間
	void SetChained(tRisseSSAFlatNamespace * ns) { Chained = ns; }

	//! @brief		番号 付き変数名を得る
	//! @param		name		変数名
	//! @param		num			番号
	//! @return		"name#num" 形式の文字列
	static tRisseString GetNumberedName(const tRisseString & name, risse_int num);

	//! @brief		この名前空間に結びつけられる基本ブロックを設定する
	//! @param		block	この名前空間に結びつけられる基本ブロック
	void SetBlock(tRisseSSABlock * block) { Block = block; }

	//! @brief		名前空間を push する
	void Push();

	//! @brief		名前空間を pop する
	void Pop();

	//! @brief		変数を定義する
	//! @param		name		変数名
	//! @param		where		その変数を表す SSA 変数表現
	void Add(const tRisseString & name, tRisseSSAVariable * where);

	//! @brief		変数を探す
	//! @param		name		変数名
	//! @param		var 		その変数を表す SSA 変数表現 を格納する先(NULL = 要らない)
	//! @return		変数が見つかったかどうか
	//! @note		変数が見つからなかった場合は *var にはなにも書き込まれない.
	//!				name は番号なしの変数名であると見なされる.
	bool Find(const tRisseString & name, tRisseSSAVariable ** var = NULL) const;

	//! @brief		変数が存在するかどうかを調べる
	//! @param		name		変数名
	//! @return		変数が見つかったかどうか
	//!				name は番号なしの変数名であると見なされる.
	//!				このメソッドは「チェーンされた」名前空間も検索し、そこに変数があるならば
	//!				真を返す
	bool IsAvailable(const tRisseString & name) const;

	//! @brief		変数を削除する
	//! @param		name		変数名
	//! @return		変数の削除に成功したかどうか
	//!	@note		name は番号なしの変数名であると見なされる
	bool Delete(const tRisseString & name);

	//! @brief		必要ならばφ関数を作成する
	//! @param		pos		スクリプト上の位置
	//! @param		name	変数名
	//! @param		n_name	番号付き変数名
	//! @return		見つかった変数、あるいはφ関数の戻り値 (NULL=ローカル変数に見つからなかった)
	tRisseSSAVariable * MakePhiFunction(risse_size pos,
		const tRisseString & name, const tRisseString & n_name = tRisseString());

	//! @brief		変数をすべて「φ関数を参照のこと」としてマークする
	//! @note		このメソッドは、Scopes のすべてのマップの値を
	//!				NULL に設定する(NULL=変数は存在しているがφ関数を作成する必要がある
	//!				という意味)
	void MarkToCreatePhi();

	//! @brief		変数に読み込みのためにアクセスをする(必要ならばφ関数などが作成される)
	//! @param		form	SSA形式インスタンス
	//! @param		pos		スクリプト上の位置
	//! @param		name	変数名
	//! @return		見つかった変数、あるいはφ関数の戻り値など (NULL=ローカル変数に見つからなかった)
	//! @note		このメソッドは、「チェーンされた」名前空間も検索し、そこに変数があるならば
	//!				チェーンされた名前空間へアクセスするための文を作成する
	tRisseSSAVariable * Read(tRisseSSAForm * form, risse_size pos, const tRisseString & name);

	//! @brief		変数に書き込みのためにアクセスをする(必要ならばφ関数などが作成される)
	//! @param		form	SSA形式インスタンス
	//! @param		pos		スクリプト上の位置
	//! @param		name	変数名
	//! @param		value	書き込む値を保持しているSSA変数
	//! @return		書き込み成功したか(変数が見つからないなど、書き込みに失敗した場合は偽)
	//! @note		このメソッドは、「チェーンされた」名前空間も検索し、そこに変数があるならば
	//!				チェーンされた名前空間へアクセスするための文を作成する
	bool Write(tRisseSSAForm * form, risse_size pos, const tRisseString & name,
				tRisseSSAVariable * value);

	//! @brief		tRisseSSAFlatNamespace のインスタンスを作成して返す
	//! @return		tRisseSSAFlatNamespace のインスタンス
	//! @note		このメソッドはすべてのメソッドを「フラットな」アクセスが可能
	//!				なように再構成してtRisseSSAFlatNamespaceのインスタンスを作成し
	//!				返す
	tRisseSSAFlatNamespace * CreateFlatNamespace() const;
};
//---------------------------------------------------------------------------






class tRisseSSAStatement;
class tRisseSSAForm;
//---------------------------------------------------------------------------
//! @brief	SSA形式における「変数」
//---------------------------------------------------------------------------
class tRisseSSAVariable : public tRisseCollectee
{
	tRisseSSAForm * Form; //!< この変数が属している SSA 形式インスタンス
	tRisseString Name; //!< 変数名(バージョンなし, 番号なし)
	tRisseString NumberedName; //!< 変数名(バージョンなし、番号つき)
	risse_int Version; //!< 変数のバージョン
	tRisseSSAStatement * Declared; //!< この変数が定義された文
	gc_vector<tRisseSSAStatement*> Used; //!< この変数が使用されている文のリスト

	const tRisseVariant *Value; //!< この変数がとりうる値(NULL=決まった値がない)
	tRisseVariant::tType ValueType; //!< この変数がとりうる型(void = どんな型でも取りうる)

public:
	//! @brief		コンストラクタ
	//! @param		form	この変数が属する SSA 形式インスタンス
	//! @param		stmt	この変数が定義された文
	//! @param		name	変数名
	//!						一時変数については空文字列を渡すこと
	tRisseSSAVariable(tRisseSSAForm * form, tRisseSSAStatement *stmt = NULL,
						const tRisseString & name = tRisseString());

	//! @brief		この変数が属している SSA 形式インスタンスを取得する
	//! @return		この変数が属している SSA 形式インスタンス
	tRisseSSAForm * GetForm() const { return Form; }

	//! @brief		この変数の名前を設定する
	//! @param		name		変数名
	//! @note		変数名を設定するたびに Version はユニークな値になる
	void SetName(const tRisseString & name);

	//! @brief		この変数の名前を返す
	//! @return		変数の名前
	const tRisseString & GetName() const { return Name; }

	//! @brief		この変数の番号付きの名前を設定する
	//! @param		n_name		この変数の番号付きの名前
	void SetNumberedName(const tRisseString & n_name) { NumberedName = n_name; }

	//! @brief		この変数の番号付きの名前を返す
	//! @return		この変数の番号付きの名前
	const tRisseString & GetNumberedName() const { return NumberedName; }

	//! @brief		この変数の修飾名を返す
	//! @return		変数の修飾名
	tRisseString GetQualifiedName() const;

	//! @brief		この変数が定義された文を設定する
	//! @param		declared	この変数が定義された文
	void SetDeclared(tRisseSSAStatement * declared) { Declared = declared; }

	//! @brief		この変数が定義された文を取得する
	//! @return		この変数が定義された文
	tRisseSSAStatement * GetDeclared() const { return Declared; }

	//! @brief		この変数が使用されている文を登録する
	//! @param		stmt	この変数が使用されている文
	void AddUsed(tRisseSSAStatement * stmt)
	{
		Used.push_back(stmt);
	}

	//! @brief		この変数が使用されている文を削除する
	//! @param		stmt	この変数が使用されている文
	void DeleteUsed(tRisseSSAStatement * stmt);

	//! @brief		この変数がとりうる値を設定する
	//! @param		value		この変数がとりうる値
	//! @note		ValueType も、この value にあわせて設定される
	void SetValue(const tRisseVariant * value)
	{
		Value = value;
		if(value)
			SetValueType(value->GetType());
		else
			SetValueType(tRisseVariant::vtVoid);
	}

	//! @brief		この変数がとりうる値を取得する
	//! @return		この変数がとりうる値
	const tRisseVariant * GetValue() const { return Value; }

	//! @brief		この変数がとりうる型を設定する
	//! @param		type		この変数がとりうる型
	void SetValueType(tRisseVariant::tType type) { ValueType = type; }

	//! @brief		この変数がとりうる型を取得する
	//! @return		この変数がとりうる型
	tRisseVariant::tType GetValueType() const { return ValueType; }

	//! @brief		この変数のメソッド(固定名)を呼び出すSSA形式を生成する
	//! @param		pos		ソースコード上の位置
	//! @param		name	メソッド名
	//! @param		param1	引数1
	//! @param		param2	引数2
	//! @param		param3	引数3
	//! @return		関数の戻り値
	tRisseSSAVariable * GenerateFuncCall(risse_size pos, const tRisseString & name,
				tRisseSSAVariable * param1 = NULL,
				tRisseSSAVariable * param2 = NULL,
				tRisseSSAVariable * param3 = NULL);

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;

	//! @brief		この変数の型や定数に関するコメントを得る
	tRisseString GetTypeComment() const;
};
//---------------------------------------------------------------------------


class tRisseSSABlock;
//---------------------------------------------------------------------------
//! @brief	SSA形式における「文」
//---------------------------------------------------------------------------
class tRisseSSAStatement : public tRisseCollectee
{
	tRisseSSAForm * Form; //!< この変数が属している SSA 形式インスタンス
	risse_size Position; //!< ソースコード上の位置 (コードポイント数オフセット)
	tRisseOpCode Code; //!< オペレーションコード
	tRisseSSABlock * Block; //!< この文が含まれている基本ブロック
	tRisseSSAStatement * Pred; //!< 直前の文
	tRisseSSAStatement * Succ; //!< 直後の文

	tRisseSSAVariable * Declared; //!< この文で定義された変数
	gc_vector<tRisseSSAVariable*> Used; //!< この文で使用されている変数のリスト

	union
	{
		tRisseSSABlock * TrueBranch; //!< 分岐のジャンプ先(条件が真のとき)
		tRisseSSABlock * JumpTarget; //!< 単純ジャンプのジャンプ先
		risse_uint32 FuncExpandFlags; //!< ocFuncCall/ocNew; 配列展開のビットマスク(1=配列を展開する)
		tRisseString * Name; //!< 名前
	};
	union
	{
		tRisseSSABlock * FalseBranch; //!< 分岐のジャンプ先(条件が偽のとき)
		bool FuncArgOmitted; //!< 引数が省略 ( ... ) されたかどうか
	};

public:
	//! @brief		コンストラクタ
	//! @param		form		この文が属する SSA 形式インスタンス
	//! @param		position	ソースコード上の位置
	//! @param		code		オペレーションコード
	tRisseSSAStatement(tRisseSSAForm * form, risse_size position, tRisseOpCode code);

	//! @brief		この変数が属している SSA 形式インスタンスを取得する
	//! @return		この変数が属している SSA 形式インスタンス
	tRisseSSAForm * GetForm() const { return Form; }

	//! @brief		ソースコード上の位置を得る
	//! @return		ソースコード上の位置
	risse_size GetPosition() const { return Position; }

	//! @brief		この文が含まれている基本ブロックを設定する
	//! @param		block この文が含まれている基本ブロック
	void SetBlock(tRisseSSABlock * block) { Block = block; }

	//! @brief		この文が含まれている基本ブロックを取得する
	//! @return		この文が含まれている基本ブロック
	tRisseSSABlock * GetBlock() const { return Block; }

	//! @brief		直前の文を設定する
	//! @param		stmt	直前の文
	void SetPred(tRisseSSAStatement * stmt) { Pred = stmt; }

	//! @brief		直前の文を取得する
	//! @return		直前の文
	tRisseSSAStatement * GetPred() const { return Pred; }

	//! @brief		直後の文を設定する
	//! @param		stmt	直後の文
	void SetSucc(tRisseSSAStatement * stmt) { Succ = stmt; }

	//! @brief		直後の文を取得する
	//! @return		直後の文
	tRisseSSAStatement * GetSucc() const { return Succ; }

	//! @brief		オペレーションコードを設定する
	//! @param		type	オペレーションコード
	void SetCode(tRisseOpCode code) { Code = code; }

	//! @brief		オペレーションコードを取得する
	//! @return		オペレーションコード
	tRisseOpCode GetCode() const { return Code; }

	//! @brief		この文で定義された変数を設定する
	//! @param		declared	この文で定義された変数
	void SetDeclared(tRisseSSAVariable * declared)
	{
		Declared = declared;
	}

	//! @brief		この文で定義された変数を取得する
	//! @return		この文で定義された変数
	tRisseSSAVariable * GetDeclared() const { return Declared; }

	//! @brief		この文で使用されている変数のリストに変数を追加する
	//! @param		var		変数
	//! @note		SetDeclared() と違い、このメソッドはvar->AddUsed(this)を呼び出す
	void AddUsed(tRisseSSAVariable * var);

	//! @brief		この文で使用されている変数のリストから変数を削除する
	//! @param		index	インデックス
	//! @note		このメソッドは、削除される変数の DeleteUsed(this) を呼び出す
	void DeleteUsed(risse_size index);

	//! @brief		分岐のジャンプ先(条件が真のとき)を設定する
	//! @param		type	分岐のジャンプ先(条件が真のとき)
	//! @note		block の直前基本ブロックとして Block を追加するので注意
	void SetTrueBranch(tRisseSSABlock * block);

	//! @brief		分岐のジャンプ先(条件が真のとき)を取得する
	//! @return		分岐のジャンプ先(条件が真のとき)
	tRisseSSABlock * GetTrueBranch() const { return TrueBranch; }

	//! @brief		単純ジャンプのジャンプ先を設定する
	//! @param		type	単純ジャンプのジャンプ先
	//! @note		block の直前基本ブロックとして Block を追加するので注意
	void SetJumpTarget(tRisseSSABlock * block);

	//! @brief		単純ジャンプのジャンプ先を取得する
	//! @return		単純ジャンプのジャンプ先
	tRisseSSABlock * GetJumpTarget() const { return JumpTarget; }

	//! @brief		分岐のジャンプ先(条件が偽のとき)を設定する
	//! @param		type	分岐のジャンプ先(条件が偽のとき)
	//! @note		block の直前基本ブロックとして Block を追加するので注意
	void SetFalseBranch(tRisseSSABlock * block);

	//! @brief		分岐のジャンプ先(条件が偽のとき)を取得する
	//! @return		分岐のジャンプ先(条件が偽のとき)
	tRisseSSABlock * GetFalseBranch() const { return FalseBranch; }


	//! @brief		配列展開のビットマスクを設定する
	//! @param		flags		配列展開のビットマスク
	void SetFuncExpandFlags(risse_uint32 flags) { FuncExpandFlags = flags; }

	//! @brief		配列展開のビットマスクを取得する
	//! @return		配列展開のビットマスク
	risse_uint32 GetFuncExpandFlags() const { return FuncExpandFlags; }

	//! @brief		引数が省略されたかどうかを設定する
	//! @param		omitted		引数が省略されたかどうか
	void SetFuncArgOmitted(bool omitted) { FuncArgOmitted = omitted; }

	//! @brief		引数が省略されたかどうかを取得する
	//! @return		引数が省略されたかどうか
	bool GetFuncArgOmitted() const { return FuncArgOmitted; }

	//! @brief		名前を設定する
	//! @param		name		名前
	void SetName(const tRisseString & name);

	//! @brief		名前を取得する
	//! @return		名前
	const tRisseString & GetName() const;

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	SSA形式における「基本ブロック」
//---------------------------------------------------------------------------
class tRisseSSABlock : public tRisseCollectee
{
	tRisseSSAForm * Form; //!< この基本ブロックを保持する SSA 形式インスタンス
	tRisseString Name; //!< 基本ブロック名(人間が読みやすい名前)
	gc_vector<tRisseSSABlock *> Pred; //!< 直前の基本ブロックのリスト
	gc_vector<tRisseSSABlock *> Succ; //!< 直後の基本ブロックのリスト
	tRisseSSAStatement * FirstStatement; //!< 文のリストの先頭
	tRisseSSAStatement * LastStatement; //!< 文のリストの最後
	tRisseSSALocalNamespace * LocalNamespace; //!< この基本ブロックの最後における名前空間のスナップショット
	mutable bool Mark; //!< マークが行われたかどうか
	mutable bool Traversing; //!< トラバース中かどうか

public:
	//! @brief		コンストラクタ
	//! @param		form		この基本ブロックを保持する SSA 形式インスタンス
	//! @param		name		この基本ブロックの名前(実際にはこれに _ が続き連番がつく)
	tRisseSSABlock(tRisseSSAForm * form, const tRisseString & name);

	//! @brief		この基本ブロックが属している SSA 形式インスタンスを取得する
	//! @return		この基本ブロックが属している SSA 形式インスタンス
	tRisseSSAForm * GetForm() const { return Form; }

	//! @brief		基本ブロック名を得る
	//! @return		基本ブロック名
	tRisseString GetName() const { return Name; }

	//! @brief		文を追加する
	//! @param		stmt	文
	void AddStatement(tRisseSSAStatement * stmt)
	{
		if(!FirstStatement)
		{
			// 最初の文
			FirstStatement = LastStatement = stmt;
		}
		else
		{
			// ２つ目以降の文
			LastStatement->SetSucc(stmt);
			stmt->SetPred(LastStatement);
			LastStatement = stmt;
		}
		stmt->SetBlock(this);
	}

	//! @brief		φ関数を追加する
	//! @param		pos			スクリプト上の位置
	//! @param		name		変数名(オリジナルの名前)
	//! @param		n_name		変数名(番号付き, バージョン無し)
	//! @param		decl_var	このφ関数の戻り値を表す変数
	void AddPhiFunction(risse_size pos, const tRisseString & name,
		const tRisseString & n_name, tRisseSSAVariable *& decl_var);

	//! @brief		直前の基本ブロックを追加する
	//! @param		block	基本ブロック
	//! @note		このメソッドは、block->AddSucc(this) を呼び出し、
	//!				block の直後ブロックを設定する
	void AddPred(tRisseSSABlock * block);

	//! @brief		直前の基本ブロックを削除する
	//! @param		index		基本ブロックのインデックス(0～)
	//! @note		このメソッドは AddPred() と異なり、削除されようとする
	//!				基本ブロックの直後ブロックの情報は書き換えない
	void DeletePred(risse_size index);

	//! @param		マークの付いていないPredがあれば削除する
	void DeleteUnmarkedPred();

	//! @brief		直後の基本ブロックを追加する
	//! @param		block	基本ブロック
	void AddSucc(tRisseSSABlock * block);

	//! @brief		ローカル名前空間のスナップショットを作成する
	//! @param		ref		参照元ローカル名前空間
	void TakeLocalNamespaceSnapshot(tRisseSSALocalNamespace * ref);

	//! @brief		「マーク」フラグをクリアする
	void ClearMark() const;

	//! @brief		「マーク」フラグを設定する
	//! @param		m		マークを設定するか(真)、クリアするか(偽)
	void SetMark(bool m = true) { Mark = m; }

	//! @brief		「マーク」を取得する
	bool GetMark() const { return Mark; }

	//! @brief		この基本ブロックを起点にして基本ブロックをたどり、そのリストを得る
	//! @param		blocks		基本ブロックのリストの格納先
	void Traverse(gc_vector<tRisseSSABlock *> & blocks) const;

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	ラベルマップを表すクラス
//---------------------------------------------------------------------------
class tRisseSSALabelMap
{
	// TODO: 親コンテキストのラベルマップの継承
	tRisseSSAForm * Form; //!< このラベルマップを保持する SSA 形式インスタンス

	//! @brief		バインドがまだされていないラベルへのジャンプ
	struct tPendingLabelJump
	{
		tRisseSSABlock * Block; //!< そのジャンプを含む基本ブロック
		risse_size Position; //!< ジャンプのあるスクリプト上の位置
		tRisseString LabelName; //!< ラベル名

		tPendingLabelJump(tRisseSSABlock * block, risse_size pos,
			const tRisseString & labelname)
		{
			Block = block;
			Position = pos;
			LabelName = labelname;
		}
	};

	typedef gc_map<tRisseString, tRisseSSABlock *> tLabelMap;
		//!< ラベルのマップのtypedef
	typedef gc_vector<tPendingLabelJump> tPendingLabelJumps;
		//!< バインドがまだされていないラベルへのジャンプのリストのtypedef

	tLabelMap LabelMap; //!< ラベルのマップ
	tPendingLabelJumps PendingLabelJumps; //!< バインドがまだされていないラベルへのジャンプのリスト

public:
	//! @brief		コンストラクタ
	//! @param		form		このラベルマップを保持する SSA 形式インスタンス
	tRisseSSALabelMap(tRisseSSAForm *form) { Form = form;}

	//! @brief		ラベルマップを追加する
	//! @param		labelname		ラベル名
	//! @param		block			基本ブロック
	//! @param		pos				基本ブロックのあるスクリプト上の位置
	//! @note		すでに同じ名前のラベルが存在していた場合は例外が発生する
	void AddMap(const tRisseString &labelname, tRisseSSABlock * block, risse_size pos);

	//! @brief		未バインドのラベルジャンプを追加する
	//! @param		block			そのジャンプを含む基本ブロック
	//! @param		pos				ジャンプのあるスクリプト上の位置
	//! @param		labelname		ラベル名
	void AddPendingLabelJump(tRisseSSABlock * block, risse_size pos,
			const tRisseString & labelname);

	//! @brief		未バインドのラベルジャンプをすべて解決する
	void BindAll();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	break あるいは continue の情報
//---------------------------------------------------------------------------
class tRisseBreakInfo : public tRisseCollectee
{
	typedef gc_vector<tRisseSSAStatement *> tPendingJumps;
	tPendingJumps PendingJumps;

public:
	//! @brief		コンストラクタ
	tRisseBreakInfo() {;}

	//! @brief		未バインドのジャンプを追加する
	//! @param		jump_stmt		ジャンプ文
	void AddJump(tRisseSSAStatement * jump_stmt);

	//! @brief		未バインドのジャンプをすべて解決する
	//! @param		target		ターゲットのブロック
	void BindAll(tRisseSSABlock * target);
};
//---------------------------------------------------------------------------
typedef tRisseBreakInfo tRisseContinueInfo;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	switch 文に関する情報を表すクラス
//---------------------------------------------------------------------------
class tRisseSwitchInfo : public tRisseCollectee
{
	tRisseSSAVariable * Reference; //!< 参照変数 ( switch の次のカッコの中の値 )
	tRisseSSABlock * LastBlock; //!< 最後のジャンプまたは分岐文を含む基本ブロック
	tRisseSSAStatement * LastStatement; //!< 最後のジャンプまたは分岐文
	tRisseSSABlock * DefaultBlock; //!< default 文のあるブロック

public:
	//! @brief		コンストラクタ
	//! @param		node			SwitchのASTノード
	//! @param		reference		参照変数
	tRisseSwitchInfo(tRisseSSAVariable * reference)
	{
		Reference = reference;
		LastBlock = NULL;
		LastStatement = NULL;
		DefaultBlock = NULL;
	}

	//! @brief		参照変数を得る
	//! @return		参照変数
	tRisseSSAVariable * GetReference() const { return Reference; }

	//! @brief		最後のジャンプまたは分岐文を含む基本ブロックを得る
	//! @return		最後のジャンプまたは分岐文を含む基本ブロック
	tRisseSSABlock * GetLastBlock() const { return LastBlock; }

	//! @brief		最後のジャンプまたは分岐文を含む基本ブロックを設定する
	//! @param		block		最後のジャンプまたは分岐文を含む基本ブロック
	void SetLastBlock(tRisseSSABlock * block) { LastBlock = block; }

	//! @brief		最後のジャンプまたは分岐文を得る
	//! @return		最後のジャンプまたは分岐文
	tRisseSSAStatement * GetLastStatement() const { return LastStatement; }

	//! @brief		最後のジャンプまたは分岐文を含む基本ブロックを設定する
	//! @param		stmt		最後のジャンプまたは分岐文
	void SetLastStatement(tRisseSSAStatement * stmt) { LastStatement = stmt; }

	//! @brief		default 文のあるブロックを得る
	//! @return		default 文のあるブロック
	tRisseSSABlock * GetDefaultBlock() const { return DefaultBlock; }

	//! @brief		default 文のあるブロックを設定する
	//! @param		block		default 文のあるブロック
	void SetDefaultBlock(tRisseSSABlock * block) { DefaultBlock = block; }

};
//---------------------------------------------------------------------------


class tRisseCompiler;
//---------------------------------------------------------------------------
//! @brief	SSA形式を表すクラス
//---------------------------------------------------------------------------
class tRisseSSAForm : public tRisseCollectee
{
	tRisseCompiler * Compiler; //!< この SSA 形式が含まれるコンパイラインスタンス
	tRisseSSAForm * Parent; //!< この SSA 形式インスタンスの親インスタンス
	tRisseASTNode * Root; //!< このコードジェネレータが生成すべきコードのASTルートノード
	tRisseString Name; //!< このSSA形式インスタンスの名前
	risse_int UniqueNumber; //!< ユニークな番号 (変数のバージョン付けに用いる)
	tRisseSSALocalNamespace * LocalNamespace; //!< ローカル名前空間
	tRisseSSALabelMap * LabelMap; //!< ラベルマップ
	tRisseSSABlock * EntryBlock; //!< エントリーSSA基本ブロック
	tRisseSSABlock * CurrentBlock; //!< 現在変換中の基本ブロック

	tRisseSwitchInfo * CurrentSwitchInfo; //!< 現在の switch に関する情報
	tRisseBreakInfo * CurrentBreakInfo; //!< 現在の break に関する情報
	tRisseContinueInfo * CurrentContinueInfo; //!< 現在の continue に関する情報

	tRisseSSAVariable * FunctionCollapseArgumentVariable; //!< 関数引数の無名の * を保持している変数

public:
	//! @brief		コンストラクタ
	//! @param		compiler		この SSA 形式が含まれるコンパイラインスタンス
	//! @param		root			ASTのルートノード
	//! @param		name			このSSA形式インスタンスの名前
	tRisseSSAForm(tRisseCompiler * compiler, tRisseASTNode * root,
		const tRisseString & name);

	//! @brief		この SSA 形式インスタンスの親インスタンスを設定する
	//! @param		form		この SSA 形式インスタンスの親インスタンス
	void SetParent(tRisseSSAForm * form);

	//! @brief		AST を SSA 形式に変換する
	void Generate();

	//! @brief		コンパイラインスタンスを得る
	//! @return		コンパイラインスタンス
	tRisseCompiler * GetCompiler() const { return Compiler; }

	//! @brief		スクリプトブロックインスタンスを得る
	//! @return		スクリプトブロックインスタンス
	tRisseScriptBlockBase * GetScriptBlock() const;

	//! @brief		このSSA形式インスタンスの名前を得る
	//! @return		このSSA形式インスタンスの名前
	const tRisseString & GetName() const { return Name; }

	//! @brief		ローカル名前空間を得る
	//! @return		ローカル名前空間
	tRisseSSALocalNamespace * GetLocalNamespace() const { return LocalNamespace; }

	//! @brief		ラベルマップを得る
	//! @return		ラベルマップ
	tRisseSSALabelMap * GetLabelMap() const { return LabelMap; }

	//! @brief		現在変換中の基本ブロックを得る
	//! @return		現在変換中の基本ブロック
	tRisseSSABlock * GetCurrentBlock() const { return CurrentBlock; }

	//! @brief		現在の switch に関する情報を得る
	//! @return		現在の switch に関する情報
	tRisseSwitchInfo * GetCurrentSwitchInfo() const { return CurrentSwitchInfo; }

	//! @brief		現在の switch に関する情報を設定する
	//! @param		info		現在の switch に関する情報
	//! @return		設定前の switch に関する情報
	tRisseSwitchInfo * SetCurrentSwitchInfo(tRisseSwitchInfo * info)
	{
		tRisseSwitchInfo * prev = CurrentSwitchInfo;
		CurrentSwitchInfo = info;
		return prev;
	}

	//! @brief		現在の break に関する情報を得る
	//! @return		現在の break に関する情報
	tRisseBreakInfo * GetCurrentBreakInfo() const { return CurrentBreakInfo; }

	//! @brief		現在の break に関する情報を設定する
	//! @param		info		現在の break に関する情報
	//! @return		設定前の break に関する情報
	tRisseBreakInfo * SetCurrentBreakInfo(tRisseBreakInfo * info)
	{
		tRisseBreakInfo * prev = CurrentBreakInfo;
		CurrentBreakInfo = info;
		return prev;
	}

	//! @brief		現在の continue に関する情報を得る
	//! @return		現在の continue に関する情報
	tRisseContinueInfo * GetCurrentContinueInfo() const { return CurrentContinueInfo; }

	//! @brief		現在の continue に関する情報を設定する
	//! @param		info		現在の continue に関する情報
	//! @return		設定前の continue に関する情報
	tRisseContinueInfo * SetCurrentContinueInfo(tRisseContinueInfo * info)
	{
		tRisseContinueInfo * prev = CurrentContinueInfo;
		CurrentContinueInfo = info;
		return prev;
	}

	//! @brief		関数引数の無名の * を保持している変数を得る
	//! @return		関数引数の無名の * を保持している変数
	tRisseSSAVariable * GetFunctionCollapseArgumentVariable() const
		{ return FunctionCollapseArgumentVariable; }

	//! @brief		新しい基本ブロックを作成する
	//! @param		name	基本ブロック名プリフィックス
	//! @param		pred	直前の基本ブロック
	//!						(NULLの場合は直前の基本ブロックは設定されない)
	//! @return		新しく作成された基本ブロック
	tRisseSSABlock * CreateNewBlock(const tRisseString & name, tRisseSSABlock * pred = NULL);

	//! @brief		現在の基本ブロックに定数値を得る文を追加する
	//! @param		pos		スクリプト上の位置
	//! @param		val		定数
	//! @return		定数値を表す一時変数
	//! @note		このメソッドは、定数値を一時変数に代入する
	//!				文を生成し、その一時変数を返す
	tRisseSSAVariable * AddConstantValueStatement(risse_size pos, const tRisseVariant & val);

	//! @brief		現在の基本ブロックに文を追加する
	//! @param		pos		スクリプト上の位置
	//! @param		code	オペレーションコード
	//! @param		ret_var	この文で変数を定義する場合はtRisseSSAVariable *へのポインタを渡す
	//!						(変数を定義したくない場合はNULLを渡す)
	//! @param		using1	この文で使用する変数その1(NULL=使用しない)
	//! @param		using2	この文で使用する変数その2(NULL=使用しない)
	//! @param		using3	この文で使用する変数その3(NULL=使用しない)
	//! @param		using4	この文で使用する変数その4(NULL=使用しない)
	//! @return		新しく追加された文
	tRisseSSAStatement * AddStatement(risse_size pos, tRisseOpCode code,
		tRisseSSAVariable ** ret_var,
			tRisseSSAVariable *using1 = NULL,
			tRisseSSAVariable *using2 = NULL,
			tRisseSSAVariable *using3 = NULL,
			tRisseSSAVariable *using4 = NULL
			);

	//! @brief		現在の基本ブロックに文を追加し、定義された変数を返す
	//! @param		pos		スクリプト上の位置
	//! @param		code	オペレーションコード
	//! @param		using1	この文で使用する変数その1(NULL=使用しない)
	//! @param		using2	この文で使用する変数その2(NULL=使用しない)
	//! @param		using3	この文で使用する変数その3(NULL=使用しない)
	//! @param		using4	この文で使用する変数その4(NULL=使用しない)
	//! @return		定義された変数
	tRisseSSAVariable * AddVariableWithStatement(risse_size pos, tRisseOpCode code,
			tRisseSSAVariable *using1 = NULL, 
			tRisseSSAVariable *using2 = NULL, 
			tRisseSSAVariable *using3 = NULL,
			tRisseSSAVariable *using4 = NULL)
	{
		tRisseSSAVariable * ret_var = NULL;
		AddStatement(pos, code, &ret_var, using1, using2, using3, using4);
		return ret_var;
	}


protected:
	//! @brief	CreateLazyBlock で返される情報の構造体
	struct tLazyBlockParam
	{
		tRisseSSAFlatNamespace * Chain; //!< スコープチェーン
		risse_size Position; //!< スクリプト上の位置
		tRisseSSAVariable *BlockVariable; //!< 遅延評価ブロックを表す変数
	};
public:
	//! @brief		新しい遅延評価ブロックを作成する
	//! @param		node		遅延評価ブロックのルートノード
	//! @param		block_var	その遅延評価ブロックを表すSSA変数を格納する先
	//! @return		CleanupLazyBlock() に渡すべき情報
	//! @note		このメソッドは遅延評価ブロックを作成してその遅延評価ブロックを
	//!				表す変数を返す。この変数はメソッドオブジェクトなので、呼び出して
	//!				使う。使い終わったら CreateLazyBlock() メソッドの戻り値を
	//!				CleanupLazyBlock() メソッドに渡して呼び出すこと。
	void * CreateLazyBlock(tRisseASTNode * node, tRisseSSAVariable *& block_var);

	//! @brief		遅延評価ブロックのクリーンアップ処理を行う
	//! @param		param	CreateLazyBlock() の戻り値
	void CleanupLazyBlock(void * param);

	//! @brief		ユニークな番号を得る
	risse_int GetUniqueNumber()
	{
		UniqueNumber++;
		// int のサイズにもよるが、32bit integer では 2^30 ぐらいで元に戻る
		// もちろんこれはそれほど変数が使われることは無いだろうという推測の元なので
		// 周回が起こったらここで例外を吐いて止まってしまった方がいいかもしれない
		if(UniqueNumber >= 1 << (sizeof(risse_int) * 8 - 2)) UniqueNumber = 0;
		return UniqueNumber;
	}

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;

//----
public:
	//! @brief		到達しない基本ブロックを削除する
	void LeapDeadBlocks();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンパイラクラス
//---------------------------------------------------------------------------
class tRisseCompiler : public tRisseCollectee
{
	tRisseScriptBlockBase * ScriptBlock; //!< このコンパイラを保有しているスクリプトブロック
	gc_vector<tRisseSSAForm *> SSAForms; //!< このブロックが保持している SSA 形式インスタンスのリスト

public:
	//! @brief		コンストラクタ
	//! @paramo		scriptblock		スクリプトブロックインスタンス
	tRisseCompiler(tRisseScriptBlockBase * scriptblock) { ScriptBlock = scriptblock; }

	//! @brief		スクリプトブロックインスタンスを得る
	//! @return		スクリプトブロックインスタンス
	tRisseScriptBlockBase * GetScriptBlock() const { return ScriptBlock; }

	//! @brief		ASTを元にコンパイルを行う
	//! @param		root		ルートASTノード
	//! @param		need_result		評価時に結果が必要かどうか
	//! @param		is_expression	式評価モードかどうか
	void Compile(tRisseASTNode * root, bool need_result, bool is_expression);

	//! @brief		SSA形式インスタンスを追加する
	//! @param		ssaform		SSA形式インスタンス
	void AddSSAForm(tRisseSSAForm * ssaform);


};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

