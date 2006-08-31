//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief SSA形式における「基本ブロック」
//---------------------------------------------------------------------------
#ifndef risseSSABlockH
#define risseSSABlockH

#include "../risseGC.h"
#include "../risseCharUtils.h"
#include "../risseTypes.h"
#include "risseAST.h"
#include "../risseVariant.h"
#include "../risseOpCodes.h"

//---------------------------------------------------------------------------
namespace Risse
{
class tRisseSSAStatement;
class tRisseSSALocalNamespace;
class tRisseSSABlock;
class tRisseSSAForm;
class tRisseCodeGenerator;
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
	risse_size LastStatementPosition; //!< 最後の文のスクリプト上の位置

public:
	typedef gc_map<const tRisseSSAVariable *, void *> tLiveVariableMap; //!< 生存している変数のリスト
private:
	tLiveVariableMap * LiveIn; //!< このブロックの開始時点で生存している変数のリスト
	tLiveVariableMap * LiveOut; //!< このブロックの終了時点で生存している変数のリスト
	mutable void * Mark; //!< マーク
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

	//! @brief		InsertStatement() メソッドでの関数挿入位置
	enum tStatementInsertPoint
	{
		sipHead,			//!< 先頭
		sipAfterPhi,		//!< φ関数の直後
		sipBeforeBranch,	//!< 分岐/ジャンプ文の直前
		sipTail				//!< 最後
	};

	//! @brief		LastStatementPosition を設定する
	void SetLastStatementPosition();

	//! @brief		LastStatementPosition を任意の値に設定する
	//! @param		pos		最後の文のスクリプト上の位置
	void SetLastStatementPosition(risse_size pos) { LastStatementPosition = pos; }

	//! @brief		LastStatementPosition を取得する
	//! @return		最後の文のスクリプト上の位置
	risse_size GetLastStatementPosition() const  { return LastStatementPosition; }

	//! @brief		文を追加する
	//! @param		stmt	文
	void AddStatement(tRisseSSAStatement * stmt)
	{ InsertStatement(stmt, sipTail); }

	//! @brief		文を挿入する
	//! @param		stmt		挿入する文
	//! @param		point		挿入する場所
	void InsertStatement(tRisseSSAStatement * stmt, tStatementInsertPoint point);

	//! @brief		文を削除する
	//! @param		stmt		削除する文
	//! @note		文で使用されていた変数の使用リストは変更を行わない
	void DeleteStatement(tRisseSSAStatement * stmt);

	//! @brief		文を置き換える
	//! @param		old_stmt		置き換えられる古い文
	//! @param		new_stmt		新しくそこに配置される文
	//! @note		文で使用されていた変数の使用リストは変更を行わない
	void ReplaceStatement(tRisseSSAStatement * old_stmt, tRisseSSAStatement * new_stmt);

	//! @brief		ブロックをまたがってφ関数を追加する
	//! @param		pos			スクリプト上の位置
	//! @param		name		変数名(オリジナルの名前)
	//! @param		n_name		変数名(番号付き, バージョン無し)
	//! @param		var			φ関数の戻り値を書き込む先
	//! @note		var は 名前空間の 名前 - 変数のペアのうちの「変数」部分を
	//!				指す参照を渡すこと。内部で再度名前空間を参照する前に
	//!				名前空間の該当ペアには値が書き込まれなければならないため。
	void AddPhiFunctionToBlocks(risse_size pos, const tRisseString & name,
		const tRisseString & n_name, tRisseSSAVariable *& var);

private:
	//! @brief		φ関数を追加し、Pred を stack に追加する
	//! @param		pos			スクリプト上の位置
	//! @param		name		変数名(オリジナルの名前)
	//! @param		n_name		変数名(番号付き, バージョン無し)
	//! @param		block_stack	ブロックスタック
	//! @param		phi_stmt_stack	φ関数スタック
	//! @return		φ関数の戻り値
	tRisseSSAVariable * AddPhiFunction(risse_size pos, const tRisseString & name,
		const tRisseString & n_name,
			gc_vector<tRisseSSABlock *> & block_stack,
			gc_vector<tRisseSSAStatement *> & phi_stmt_stack);
public:

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

	//! @brief		直後の基本ブロックを追加する
	//! @param		block	基本ブロック
	void AddSucc(tRisseSSABlock * block);

	//! @brief		直前の基本ブロックのリストを取得する
	//! @return		直前の基本ブロックのリスト
	const gc_vector<tRisseSSABlock *> & GetPred() const { return Pred; }

	//! @brief		直後の基本ブロックのリストを取得する
	//! @return		直後の基本ブロックのリスト
	const gc_vector<tRisseSSABlock *> & GetSucc() const { return Succ; }

	//! @param		マークの付いていないPredがあれば削除する
	void DeleteUnmarkedPred();

	//! @brief		ローカル名前空間のスナップショットを作成する
	//! @param		ref		参照元ローカル名前空間
	void TakeLocalNamespaceSnapshot(tRisseSSALocalNamespace * ref);

	//! @brief		ローカル名前空間のスナップショットを得る
	//! @return		ローカル名前空間のスナップショット
	tRisseSSALocalNamespace * GetLocalNamespace() const { return LocalNamespace; }

	//! @brief		LiveIn/Outに変数を追加する
	//! @param		var		追加する変数
	//! @param		out		真の場合 LiveOut を対象にし、偽の場合 LiveIn を対象にする
	void AddLiveness(const tRisseSSAVariable * var, bool out = true);

	//! @brief		LiveIn/Outに変数があるかどうかを得る
	//! @param		var		探す変数
	//! @param		out		真の場合 LiveOut を対象にし、偽の場合 LiveIn を対象にする
	//! @return		変数が見つかれば真
	bool GetLiveness(const tRisseSSAVariable * var, bool out = true) const;

	//! @brief		「マーク」フラグをクリアする
	void ClearMark() const;

	//! @brief		「マーク」フラグを設定する
	//! @param		m		マーク
	//! @note		マークはポインタならば何でもつけることができる。
	//!				ただし、オブジェクトが開放されるとそのマークも無効になると
	//!				考えた方がよい。もしそのオブジェクトのポインタが開放されたあとに
	//!				再度他のオブジェクトに同じポインタの値が割り当てられた場合、
	//!				検証が難しいバグを産む可能性がある。
	//!				マークとして用いる値は確実にこのマークを使用中あるいは使用後
	//!				にも存在が確実なものとすること。
	void SetMark(void * m = reinterpret_cast<void*>(-1)) { Mark = m; }

	//! @brief		「マーク」を取得する
	void * GetMark() const { return Mark; }

	//! @brief		この基本ブロックを起点にして基本ブロックをたどり、そのリストを得る
	//! @param		blocks		基本ブロックのリストの格納先
	void Traverse(gc_vector<tRisseSSABlock *> & blocks) const;

	//! @brief		共有の刺さった変数へのアクセスを別形式の文に変換
	void ConvertSharedVariableAccess();

	//! @brief		LiveIn と LiveOut を作成する
	void CreateLiveInAndLiveOut();

	//! @brief		変数の生存区間を基本ブロック単位で解析する
	void AnalyzeVariableBlockLiveness();

	//! @brief		変数の生存区間を文単位で解析する
	void AnalyzeVariableStatementLiveness();

	//! @brief		φ関数を削除する
	//! @note		このメソッド実行後はSSA形式としての性質は保てなくなる。
	void RemovePhiStatements();

	//! @brief		すべての文に通し番号を設定する
	//! @param		order		通し番号の開始値 (終了時には文の数が加算されている)
	void SetOrder(risse_size & order);

	//! @brief		バイトコードを生成する
	//! @param		gen		バイトコードジェネレータ
	void GenerateCode(tRisseCodeGenerator * gen) const;

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse
#endif

