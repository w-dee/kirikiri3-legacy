//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
class tSSAStatement;
class tSSALocalNamespace;
class tSSABlock;
class tSSAForm;
class tCodeGenerator;
//---------------------------------------------------------------------------
//! @brief	SSA形式における「基本ブロック」
//---------------------------------------------------------------------------
class tSSABlock : public tCollectee
{
	tSSAForm * Form; //!< この基本ブロックを保持する SSA 形式インスタンス
	tString Name; //!< 基本ブロック名(人間が読みやすい名前)
	gc_vector<tSSABlock *> Pred; //!< 直前の基本ブロックのリスト
	gc_vector<tSSABlock *> Succ; //!< 直後の基本ブロックのリスト
	tSSAStatement * FirstStatement; //!< 文のリストの先頭
	tSSAStatement * LastStatement; //!< 文のリストの最後
	tSSALocalNamespace * LocalNamespace; //!< この基本ブロックの最後における名前空間のスナップショット
	risse_size LastStatementPosition; //!< 最後の文のスクリプト上の位置

public:
	typedef gc_map<const tSSAVariable *, void *> tLiveVariableMap; //!< 生存している変数のリスト
private:
	tLiveVariableMap * LiveIn; //!< このブロックの開始時点で生存している変数のリスト
	tLiveVariableMap * LiveOut; //!< このブロックの終了時点で生存している変数のリスト
	mutable void * Mark; //!< マーク
	mutable bool Traversing; //!< トラバース中かどうか
	bool Alive; //!< この基本ブロックが生きているかどうか
		//!< (tSSAForm::LeapDeadBlocks() 後に有効;それ以前は生存している・していないにかかわらず常に偽なので注意)

public:
	//! @brief		コンストラクタ
	//! @param		form		この基本ブロックを保持する SSA 形式インスタンス
	//! @param		name		この基本ブロックの名前(実際にはこれに _ が続き連番がつく)
	//! @param		ns			この基本ブロックが引き継ぐローカル名前空間
	//!							(内容はコピーされ、コピーされた内容をこのインスタンスが保持する)
	tSSABlock(tSSAForm * form, const tString & name, const tSSALocalNamespace * ns);

	//! @brief		この基本ブロックが属している SSA 形式インスタンスを取得する
	//! @return		この基本ブロックが属している SSA 形式インスタンス
	tSSAForm * GetForm() const { return Form; }

	//! @brief		基本ブロック名を得る
	//! @return		基本ブロック名
	tString GetName() const { return Name; }

	//! @brief		InsertStatement() メソッドでの関数挿入位置
	enum tStatementInsertPoint
	{
		sipHead,			//!< 先頭
		sipAfterPhi,		//!< φ関数の直後
		sipBeforeBranch,	//!< 分岐/ジャンプ文の直前
		sipTail				//!< 最後
	};

	//! @brief		この基本ブロックが生存しているとマークする
	void SetAlive() { Alive = true; }

	//! @brief		この基本ブロックが生存しているかどうかを返す
	bool GetAlive() { return Alive; }

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
	void AddStatement(tSSAStatement * stmt)
	{ InsertStatement(stmt, sipTail); }

	//! @brief		文を挿入する
	//! @param		stmt		挿入する文
	//! @param		point		挿入する場所
	void InsertStatement(tSSAStatement * stmt, tStatementInsertPoint point);

	//! @brief		文を削除する
	//! @param		stmt		削除する文
	//! @note		文で使用されていた変数の使用リストは変更を行わない
	void DeleteStatement(tSSAStatement * stmt);

	//! @brief		文を置き換える
	//! @param		old_stmt		置き換えられる古い文
	//! @param		new_stmt		新しくそこに配置される文
	//! @note		文で使用されていた変数の使用リストは変更を行わない
	void ReplaceStatement(tSSAStatement * old_stmt, tSSAStatement * new_stmt);

	//! @brief		ブロックをまたがってφ関数を追加する
	//! @param		pos			スクリプト上の位置
	//! @param		name		変数名(オリジナルの名前)
	//! @param		n_name		変数名(番号付き, バージョン無し)
	//! @param		var			φ関数の戻り値を書き込む先
	//! @note		var は 名前空間の 名前 - 変数のペアのうちの「変数」部分を
	//!				指す参照を渡すこと。内部で再度名前空間を参照する前に
	//!				名前空間の該当ペアには値が書き込まれなければならないため。
	void AddPhiFunctionToBlocks(risse_size pos, const tString & name,
		const tString & n_name, tSSAVariable *& var);

private:
	//! @brief		φ関数を追加し、Pred を stack に追加する
	//! @param		pos			スクリプト上の位置
	//! @param		name		変数名(オリジナルの名前)
	//! @param		n_name		変数名(番号付き, バージョン無し)
	//! @param		block_stack	ブロックスタック
	//! @param		phi_stmt_stack	φ関数スタック
	//! @return		φ関数の戻り値
	tSSAVariable * AddPhiFunction(risse_size pos, const tString & name,
		const tString & n_name,
			gc_vector<tSSABlock *> & block_stack,
			gc_vector<tSSAStatement *> & phi_stmt_stack);
public:

	//! @brief		直前の基本ブロックを追加する
	//! @param		block	基本ブロック
	//! @note		このメソッドは、block->AddSucc(this) を呼び出し、
	//!				block の直後ブロックを設定する
	void AddPred(tSSABlock * block);

	//! @brief		直前の基本ブロックを削除する
	//! @param		index		基本ブロックのインデックス(0～)
	//! @note		このメソッドは AddPred() と異なり、削除されようとする
	//!				基本ブロックの直後ブロックの情報は書き換えない
	void DeletePred(risse_size index);

	//! @brief		直後の基本ブロックを追加する
	//! @param		block	基本ブロック
	void AddSucc(tSSABlock * block);

	//! @brief		直前の基本ブロックのリストを取得する
	//! @return		直前の基本ブロックのリスト
	const gc_vector<tSSABlock *> & GetPred() const { return Pred; }

	//! @brief		直後の基本ブロックのリストを取得する
	//! @return		直後の基本ブロックのリスト
	const gc_vector<tSSABlock *> & GetSucc() const { return Succ; }

	//! @brief		死んでいるPredがあれば削除する
	void DeleteDeadPred();

	//! @brief		各変数のうち、変数が使用されている文が所属しているブロックが死んでいる場合、
	//!				その文を変数の使用リストから削除する
	void DeleteDeadStatementsFromVariables();

	//! @brief		ローカル名前空間を得る
	//! @return		ローカル名前空間
	tSSALocalNamespace * GetLocalNamespace() const { return LocalNamespace; }

	//! @brief		LiveIn/Outに変数を追加する
	//! @param		var		追加する変数
	//! @param		out		真の場合 LiveOut を対象にし、偽の場合 LiveIn を対象にする
	void AddLiveness(const tSSAVariable * var, bool out = true);

	//! @brief		LiveIn/Outに変数があるかどうかを得る
	//! @param		var		探す変数
	//! @param		out		真の場合 LiveOut を対象にし、偽の場合 LiveIn を対象にする
	//! @return		変数が見つかれば真
	bool GetLiveness(const tSSAVariable * var, bool out = true) const;

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
	void Traverse(gc_vector<tSSABlock *> & blocks) const;

	//! @brief		すべての変数のマークを解除する
	void ClearVariableMarks();

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
	void GenerateCode(tCodeGenerator * gen) const;

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tString Dump() const;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse
#endif

