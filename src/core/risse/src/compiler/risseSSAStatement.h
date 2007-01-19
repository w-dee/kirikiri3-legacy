//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
class tRisseSSABlock;
class tRisseSSAVariable;
class tRisseSSAForm;
class tRisseCodeBlock;
class tRisseCodeGenerator;

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

	risse_size Order; //!< コード先頭からの通し番号

	gc_vector<tRisseSSABlock *> Targets; //!< 分岐のターゲット(解釈はCodeによって異なる)

	// ここでは構造体のバイト数の節約のために、いくつか相互に関係のない
	// メンバをunionでくくっている。
	union
	{
		risse_uint32 FuncExpandFlags; //!< ocFuncCall/ocNew; 配列展開のビットマスク(1=配列を展開する)
		tRisseString * Name; //!< 名前
		risse_size Index; //!< インデックス
	};
	union
	{
		tRisseSSAForm * DefinedForm;	//!< この文で宣言された遅延評価ブロックの
										//!< SSA形式インスタンス(ocDefineLazyBlock)
		risse_size BlockCount;			//!< 関数呼び出し時のブロックの個数
		risse_size TryIdentifierIndex;	//!< Try識別子のインデックス
		risse_uint32 OperateFlagsValue;	//!< ocDSetF, ocDGetF の操作フラグとocDSetAttribの属性値
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

	//! @brief		この文が分岐文かどうかを返す
	//! @return		この文が分岐文かどうか
	bool IsBranchStatement() const {
		return
			Code == ocBranch ||
			Code == ocJump ||
			Code == ocCatchBranch; }

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

	//! @brief		この文の使用変数リストをすべて解放する
	void DeleteUsed();

	//! @brief		この文で使用されている変数のリストを得る
	const gc_vector<tRisseSSAVariable *> & GetUsed() const { return Used; }

	//! @brief		コード先頭からの通し番号を設定する
	//! @param		order	コード先頭からの通し番号
	void SetOrder(risse_size order) { Order = order; }

	//! @brief		コード先頭からの通し番号を取得する @return コード先頭からの通し番号
	risse_size GetOrder() const { return Order; }

	//! @brief		分岐のジャンプ先(条件が真のとき)を設定する
	//! @param		block	分岐のジャンプ先(条件が真のとき)
	//! @note		block の直前基本ブロックとして Block を追加するので注意
	void SetTrueBranch(tRisseSSABlock * block);

	//! @brief		分岐のジャンプ先(条件が真のとき)を取得する
	//! @return		分岐のジャンプ先(条件が真のとき)
	tRisseSSABlock * GetTrueBranch() const;

	//! @brief		分岐のジャンプ先(条件が偽のとき)を設定する
	//! @param		block	分岐のジャンプ先(条件が偽のとき)
	//! @note		block の直前基本ブロックとして Block を追加するので注意
	void SetFalseBranch(tRisseSSABlock * block);

	//! @brief		分岐のジャンプ先(条件が偽のとき)を取得する
	//! @return		分岐のジャンプ先(条件が偽のとき)
	tRisseSSABlock * GetFalseBranch() const;

	//! @brief		単純ジャンプのジャンプ先を設定する
	//! @param		block	単純ジャンプのジャンプ先
	//! @note		block の直前基本ブロックとして Block を追加するので注意
	void SetJumpTarget(tRisseSSABlock * block);

	//! @brief		単純ジャンプのジャンプ先を取得する
	//! @return		単純ジャンプのジャンプ先
	tRisseSSABlock * GetJumpTarget() const;

	//! @brief		例外保護付き関数呼び出しの何事もなかった場合のジャンプ先を設定する
	//! @param		block	例外保護付き関数呼び出しの何事もなかった場合のジャンプ先
	//! @note		block の直前基本ブロックとして Block を追加するので注意
	void SetTryExitTarget(tRisseSSABlock * block);

	//! @brief		例外保護付き関数呼び出しの何事もなかった場合のジャンプ先を取得する
	//! @return		例外保護付き関数呼び出しの何事もなかった場合のジャンプ先
	tRisseSSABlock * GetTryExitTarget() const;

	//! @brief		例外保護付き関数呼び出しの例外が発生した場合のジャンプ先を設定する
	//! @param		type	例外保護付き関数呼び出しの例外が発生した場合のジャンプ先
	//! @note		block の直前基本ブロックとして Block を追加するので注意
	void SetTryCatchTarget(tRisseSSABlock * block);

	//! @brief		例外保護付き関数呼び出しの例外が発生した場合のジャンプ先を取得する
	//! @return		例外保護付き関数呼び出しの例外が発生した場合のジャンプ先
	tRisseSSABlock * GetTryCatchTarget() const;

	//! @brief		この文の分岐先の数を得る
	//! @return		この文の分岐先の数
	risse_size GetTargetCount() const { return Targets.size(); }

	//! @brief		この文の分岐先を追加する
	//! @param		block		この文の分岐先
	void AddTarget(tRisseSSABlock * block);

	//! @brief		配列展開のビットマスクを設定する
	//! @param		flags		配列展開のビットマスク
	void SetFuncExpandFlags(risse_uint32 flags) { FuncExpandFlags = flags; }

	//! @brief		配列展開のビットマスクを取得する
	//! @return		配列展開のビットマスク
	risse_uint32 GetFuncExpandFlags() const { return FuncExpandFlags; }

	//! @brief		名前を設定する
	//! @param		name		名前
	void SetName(const tRisseString & name);

	//! @brief		名前を取得する
	//! @return		名前
	const tRisseString & GetName() const;

	//! @brief		インデックスを設定する
	//! @param		index		インデックス
	void SetIndex(risse_size index) { Index = index; }

	//! @brief		インデックスを取得する
	//! @return		インデックス
	risse_size GetIndex() const { return Index; }

	//! @brief		この文で宣言された遅延評価ブロックのSSA形式インスタンスを設定する
	//! @param		form		この文で宣言された遅延評価ブロックのSSA形式インスタンス
	void SetDefinedForm(tRisseSSAForm *form) { DefinedForm = form; }

	//! @brief		この文で宣言された遅延評価ブロックのSSA形式インスタンスを取得する
	//! @return		この文で宣言された遅延評価ブロックのSSA形式インスタンス
	tRisseSSAForm * GetDefinedForm() const { return DefinedForm; }

	//! @brief		関数宣言のブロックの個数を設定する
	//! @param		count		ブロックの個数
	void SetBlockCount(risse_size count) { BlockCount = count; }

	//! @brief		関数宣言のブロックの個数を取得する
	//! @return		関数宣言のブロックの個数
	risse_size GetBlockCount() const { return BlockCount; }

	//! @brief		Try識別子のインデックスを設定する
	//! @param		idx		Try識別子のインデックス
	void SetTryIdentifierIndex(risse_size idx) { TryIdentifierIndex = idx; }

	//! @brief		Try識別子のインデックスを取得
	//! @return		Try識別子のインデックス
	risse_size GetTryIdentifierIndex() const { return TryIdentifierIndex; }

	//! @brief		操作フラグを設定する
	//! @param		accessflags		アクセスフラグ
	void SetAccessFlags(tRisseOperateFlags accessflags) { OperateFlagsValue = (risse_uint32)accessflags; }

	//! @brief		メンバ属性を取得する
	//! @return		メンバ属性
	tRisseOperateFlags GetAccessFlags() const { return tRisseOperateFlags(OperateFlagsValue); }

	//! @brief		変数の生存区間を文単位で解析する
	void AnalyzeVariableStatementLiveness();


	//! @brief		バイトコードを生成する
	//! @param		gen		バイトコードジェネレータ
	void GenerateCode(tRisseCodeGenerator * gen) const;

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;

	//! @brief		文単位の変数の使用開始と使用終了についてダンプを行う
	//! @param		is_start		使用開始のダンプを行う際にtrue,使用終了の場合はfalse
	tRisseString DumpVariableStatementLiveness(bool is_start) const;
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
#endif

