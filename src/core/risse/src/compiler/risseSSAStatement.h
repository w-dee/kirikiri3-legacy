//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief SSA形式における「文」
//---------------------------------------------------------------------------
#ifndef risseSSAStatementH
#define risseSSAStatementH

#include "../risseGC.h"
#include "../risseCharUtils.h"
#include "../risseTypes.h"
#include "risseAST.h"
#include "../risseVariant.h"
#include "../risseOpCodes.h"

//---------------------------------------------------------------------------
namespace Risse
{
class tSSABlock;
class tSSAVariable;
class tSSAForm;
class tCodeBlock;
class tCodeGenerator;

//---------------------------------------------------------------------------
/**
 * SSA形式における「文」
 */
class tSSAStatement : public tCollectee
{
	tSSAForm * Form; //!< この変数が属している SSA 形式インスタンス
	risse_size Id; //!< Id (ユニークな値)
	risse_size Position; //!< ソースコード上の位置 (コードポイント数オフセット)
	tOpCode Code; //!< オペレーションコード
	tSSABlock * Block; //!< この文が含まれている基本ブロック
	tSSAStatement * Pred; //!< 直前の文
	tSSAStatement * Succ; //!< 直後の文

	tSSAVariable * Declared; //!< この文で定義された変数
	gc_vector<tSSAVariable*> Used; //!< この文で使用されている変数のリスト

	risse_size Order; //!< コード先頭からの通し番号

	void * Mark; //!< マーク (任意の用途に使う)

	bool Effective; //!< この文が副作用を持っているかどうか

	gc_vector<tSSABlock *> Targets; //!< 分岐のターゲット(解釈はCodeによって異なる)

	// ここでは構造体のバイト数の節約のために、いくつか相互に関係のない
	// メンバをunionでくくっている。
	union
	{
		risse_uint32 FuncExpandFlags; //!< ocFuncCall/ocNew; 配列展開のビットマスク(1=配列を展開する)
		tString * Name; //!< 名前
		risse_size Index; //!< インデックス
		tString * Message; //!< Assert時のメッセージ
		tVariant::tType AssertType; //!< AssertType時の型
		const tVariant * Value; //!< 値 (ocAssignConstant等用)
	};
	union
	{
		tSSAForm * DefinedForm;	//!< この文で宣言された遅延評価ブロックの
										//!< SSA形式インスタンス(ocDefineLazyBlock)
		risse_size BlockCount;			//!< 関数呼び出し時のブロックの個数
		risse_size TryIdentifierIndex;	//!< Try識別子のインデックス
		risse_uint32 OperateFlagsValue;	//!< ocDSetF, ocDGetF の操作フラグとocDSetAttribの属性値
	};

public:
	// 型伝播解析・定数伝播解析 で使う警告/エラーの情報(Mark にこれが設定される)
	struct tErrorWarningInfo
	{
		tString Message; //!< メッセージ
		bool Error; //!< エラーかどうか
		tErrorWarningInfo(const tString & message, bool error_) : Message(message), Error(error_) {;}
	};

public:
	/**
	 * コンストラクタ
	 * @param form		この文が属する SSA 形式インスタンス
	 * @param position	ソースコード上の位置
	 * @param code		オペレーションコード
	 */
	tSSAStatement(tSSAForm * form, risse_size position, tOpCode code);

	/**
	 * この変数が属している SSA 形式インスタンスを取得する
	 * @return	この変数が属している SSA 形式インスタンス
	 */
	tSSAForm * GetForm() const { return Form; }

	/**
	 * ソースコード上の位置を得る
	 * @return	ソースコード上の位置
	 */
	risse_size GetPosition() const { return Position; }

	/**
	 * Id を得る
	 */
	risse_size GetId() const { return Id; }

	/**
	 * Mark を得る
	 * @return	Mark
	 */
	void * GetMark() const { return Mark; }

	/**
	 * Mark を設定する
	 * @param mark	マーク
	 */
	void SetMark(void * mark) { Mark = mark; }

	/**
	 * この文が含まれている基本ブロックを設定する
	 * @param block	この文が含まれている基本ブロック
	 */
	void SetBlock(tSSABlock * block) { Block = block; }

	/**
	 * この文が含まれている基本ブロックを取得する
	 * @return	この文が含まれている基本ブロック
	 */
	tSSABlock * GetBlock() const { return Block; }

	/**
	 * 直前の文を設定する
	 * @param stmt	直前の文
	 */
	void SetPred(tSSAStatement * stmt) { Pred = stmt; }

	/**
	 * 直前の文を取得する
	 * @return	直前の文
	 */
	tSSAStatement * GetPred() const { return Pred; }

	/**
	 * 直後の文を設定する
	 * @param stmt	直後の文
	 */
	void SetSucc(tSSAStatement * stmt) { Succ = stmt; }

	/**
	 * 直後の文を取得する
	 * @return	直後の文
	 */
	tSSAStatement * GetSucc() const { return Succ; }

	/**
	 * オペレーションコードを設定する
	 * @param type	オペレーションコード
	 */
	void SetCode(tOpCode code) { Code = code; }

	/**
	 * オペレーションコードを取得する
	 * @return	オペレーションコード
	 */
	tOpCode GetCode() const { return Code; }

	/**
	 * この文が分岐文かどうかを返す
	 * @return	この文が分岐文かどうか
	 */
	bool IsBranchStatement() const {
		return
			Code == ocBranch ||
			Code == ocJump ||
			Code == ocCatchBranch; }

	/**
	 * この文で定義された変数を設定する
	 * @param declared	この文で定義された変数
	 */
	void SetDeclared(tSSAVariable * declared)
	{
		Declared = declared;
	}

	/**
	 * この文で定義された変数を取得する
	 * @return	この文で定義された変数
	 */
	tSSAVariable * GetDeclared() const { return Declared; }

	/**
	 * この文で使用されている変数のリストに変数を追加する
	 * @param var	変数
	 * @note	SetDeclared() と違い、このメソッドはvar->AddUsed(this)を呼び出す
	 */
	void AddUsed(tSSAVariable * var);

	/**
	 * この文で使用されている変数のリストから変数を削除する
	 * @param index	インデックス
	 * @note	このメソッドは、削除される変数の DeleteUsed(this) を呼び出す
	 */
	void DeleteUsed(risse_size index);

	/**
	 * この文の使用変数リストをすべて解放する
	 */
	void DeleteUsed();

	/**
	 * この文で使用されている変数を書き換える
	 * @param old_var	置き換える前の変数
	 * @param new_var	置き換えた後の変数
	 * @note	変数が見つからない場合は ASSERTION ERROR になる。
	 *			置き換える前の変数の Used からは、この文は削除されない。
	 */
	void OverwriteUsed(tSSAVariable * old_var, tSSAVariable * new_var);

	/**
	 * この文で使用されている変数のリストを得る
	 */
	const gc_vector<tSSAVariable *> & GetUsed() const { return Used; }

	/**
	 * 変数の合併を行うための、どの変数とどの変数が合併できそうかのリストを作成する
	 */
	void TraceCoalescable();

	/**
	 * 変数の合併を行う
	 */
	void Coalesce();

	/**
	 * コード先頭からの通し番号を設定する
	 * @param order	コード先頭からの通し番号
	 */
	void SetOrder(risse_size order) { Order = order; }

	/**
	 * コード先頭からの通し番号を取得する @return コード先頭からの通し番号
	 */
	risse_size GetOrder() const { return Order; }

	/**
	 * 分岐のジャンプ先(条件が真のとき)を設定する
	 * @param block	分岐のジャンプ先(条件が真のとき)
	 * @note	block の直前基本ブロックとして Block を追加するので注意
	 */
	void SetTrueBranch(tSSABlock * block);

	/**
	 * 分岐のジャンプ先(条件が真のとき)を取得する
	 * @return	分岐のジャンプ先(条件が真のとき)
	 */
	tSSABlock * GetTrueBranch() const;

	/**
	 * 分岐のジャンプ先(条件が偽のとき)を設定する
	 * @param block	分岐のジャンプ先(条件が偽のとき)
	 * @note	block の直前基本ブロックとして Block を追加するので注意
	 */
	void SetFalseBranch(tSSABlock * block);

	/**
	 * 分岐のジャンプ先(条件が偽のとき)を取得する
	 * @return	分岐のジャンプ先(条件が偽のとき)
	 */
	tSSABlock * GetFalseBranch() const;

	/**
	 * 単純ジャンプのジャンプ先を設定する
	 * @param block	単純ジャンプのジャンプ先
	 * @note	block の直前基本ブロックとして Block を追加するので注意
	 */
	void SetJumpTarget(tSSABlock * block);

	/**
	 * 単純ジャンプのジャンプ先を設定する
	 * @param block	単純ジャンプのジャンプ先
	 * @note	SetJumpTarget と違って直前基本ブロックなどの情報はいじらない
	 */
	void SetJumpTargetNoSetPred(tSSABlock * block);

	/**
	 * 単純ジャンプのジャンプ先を取得する
	 * @return	単純ジャンプのジャンプ先
	 */
	tSSABlock * GetJumpTarget() const;

	/**
	 * 例外保護付き関数呼び出しの何事もなかった場合のジャンプ先を設定する
	 * @param block	例外保護付き関数呼び出しの何事もなかった場合のジャンプ先
	 * @note	block の直前基本ブロックとして Block を追加するので注意
	 */
	void SetTryExitTarget(tSSABlock * block);

	/**
	 * 例外保護付き関数呼び出しの何事もなかった場合のジャンプ先を取得する
	 * @return	例外保護付き関数呼び出しの何事もなかった場合のジャンプ先
	 */
	tSSABlock * GetTryExitTarget() const;

	/**
	 * 例外保護付き関数呼び出しの例外が発生した場合のジャンプ先を設定する
	 * @param type	例外保護付き関数呼び出しの例外が発生した場合のジャンプ先
	 * @note	block の直前基本ブロックとして Block を追加するので注意
	 */
	void SetTryCatchTarget(tSSABlock * block);

	/**
	 * 例外保護付き関数呼び出しの例外が発生した場合のジャンプ先を取得する
	 * @return	例外保護付き関数呼び出しの例外が発生した場合のジャンプ先
	 */
	tSSABlock * GetTryCatchTarget() const;

	/**
	 * この文の分岐先の数を得る
	 * @return	この文の分岐先の数
	 */
	risse_size GetTargetCount() const { return Targets.size(); }

	/**
	 * この文の分岐先を追加する
	 * @param block	この文の分岐先
	 */
	void AddTarget(tSSABlock * block);

	/**
	 * 配列展開のビットマスクを設定する
	 * @param flags	配列展開のビットマスク
	 */
	void SetFuncExpandFlags(risse_uint32 flags) { FuncExpandFlags = flags; }

	/**
	 * 配列展開のビットマスクを取得する
	 * @return	配列展開のビットマスク
	 */
	risse_uint32 GetFuncExpandFlags() const { return FuncExpandFlags; }

	/**
	 * 名前を設定する
	 * @param name	名前
	 */
	void SetName(const tString & name);

	/**
	 * 名前を取得する
	 * @return	名前
	 */
	const tString & GetName() const;

	/**
	 * 文に副作用があるかどうかを得る
	 * @return	文に副作用があるかどうか
	 */
	bool GetEffective() const { return Effective; }

	/**
	 * メッセージを設定する
	 * @param name	メッセージ
	 */
	void SetMessage(const tString & msg) { Message = new tString(msg); }

	/**
	 * 型を取得する(ocAssertType用)
	 * @return	型
	 */
	tVariant::tType GetAssertType() const { return AssertType; }

	/**
	 * 型を設定する
	 * @param type	型
	 */
	void SetAssertType(tVariant::tType type);

	/**
	 * メッセージを取得する
	 * @return	メッセージ
	 */
	const tString & GetMessage() const { return * Message; }

	/**
	 * 値を設定する
	 * @param value	値
	 */
	void SetValue(const tVariant * value) { Value = value; }

	/**
	 * インデックスを設定する
	 * @param index	インデックス
	 */
	void SetIndex(risse_size index) { Index = index; }

	/**
	 * インデックスを取得する
	 * @return	インデックス
	 */
	risse_size GetIndex() const { return Index; }

	/**
	 * この文で宣言された遅延評価ブロックのSSA形式インスタンスを設定する
	 * @param form	この文で宣言された遅延評価ブロックのSSA形式インスタンス
	 */
	void SetDefinedForm(tSSAForm *form) { DefinedForm = form; }

	/**
	 * この文で宣言された遅延評価ブロックのSSA形式インスタンスを取得する
	 * @return	この文で宣言された遅延評価ブロックのSSA形式インスタンス
	 */
	tSSAForm * GetDefinedForm() const { return DefinedForm; }

	/**
	 * 関数宣言のブロックの個数を設定する
	 * @param count	ブロックの個数
	 */
	void SetBlockCount(risse_size count) { BlockCount = count; }

	/**
	 * 関数宣言のブロックの個数を取得する
	 * @return	関数宣言のブロックの個数
	 */
	risse_size GetBlockCount() const { return BlockCount; }

	/**
	 * Try識別子のインデックスを設定する
	 * @param idx	Try識別子のインデックス
	 */
	void SetTryIdentifierIndex(risse_size idx) { TryIdentifierIndex = idx; }

	/**
	 * Try識別子のインデックスを取得する
	 * @return	Try識別子のインデックス
	 */
	risse_size GetTryIdentifierIndex() const { return TryIdentifierIndex; }


	/**
	 * 操作フラグを設定する
	 * @param accessflags	アクセスフラグ
	 */
	void SetAccessFlags(tOperateFlags accessflags) { OperateFlagsValue = (risse_uint32)accessflags; }

	/**
	 * メンバ属性を取得する
	 * @return	メンバ属性
	 */
	tOperateFlags GetAccessFlags() const { return tOperateFlags(OperateFlagsValue); }

	/**
	 * 変数の干渉グラフを作成する
	 */
	void CreateVariableInterferenceGraph(gc_map<const tSSAVariable *, risse_size> &livemap);

	/**
	 * 文レベルでの最適化を行う
	 * @param statements	作業リスト
	 */
	void OptimizeAtStatementLevel(gc_map<risse_size, tSSAStatement *> &statements);

	/**
	 * 型伝播解析・定数伝播解析を行う
	 * @param variables	変数の作業リスト
	 * @param blocks	ブロックの作業リスト
	 */
	void AnalyzeConstantPropagation(
			gc_vector<tSSAVariable *> &variables,
			gc_vector<tSSABlock *> &blocks);

	/**
	 * 事前にしらべた型伝播解析・定数伝播解析のエラーを実際に発生させたり、警告を表示したりする
	 * @note	エラー情報は各文の Mark に設定されている
	 */
	void RealizeConstantPropagationErrors();

	/**
	 * 型 Assertion コードを挿入する
	 */
	void InsertTypeAssertion();

	/**
	 * 定数代入を置き換える
	 */
	void ReplaceConstantAssign();

	/**
	 * 3番地形式の格納先が他の変数と異なっていることを保証(暫定処置)
	 */
	void Check3AddrAssignee();

	/**
	 * 変数にレジスタを割り当てる
	 */
	void AssignRegisters(gc_vector<void*> & assign_work);

	/**
	 * バイトコードを生成する
	 * @param gen	バイトコードジェネレータ
	 */
	void GenerateCode(tCodeGenerator * gen) const;

public:
	/**
	 * 文そのもののダンプを行う
	 * @return	ダンプ文字列
	 */
	tString DumpBody() const;

public:
	/**
	 * ダンプを行う
	 * @return	ダンプ文字列
	 */
	tString Dump() const;

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
#endif

