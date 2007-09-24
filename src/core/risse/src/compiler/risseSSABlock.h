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
	risse_size Id; //!< Id (ユニークな値)
	tString Name; //!< 基本ブロック名(人間が読みやすい名前)
	gc_vector<tSSABlock *> Pred; //!< 直前の基本ブロックのリスト
	gc_vector<tSSABlock *> Succ; //!< 直後の基本ブロックのリスト
	tSSAStatement * FirstStatement; //!< 文のリストの先頭
	tSSAStatement * LastStatement; //!< 文のリストの最後
	tSSALocalNamespace * LocalNamespace; //!< この基本ブロックの最後における名前空間のスナップショット
	risse_size LastStatementPosition; //!< 最後の文のスクリプト上の位置

public:
	typedef gc_map<const tSSAVariable *, risse_size> tLiveVariableMap; //!< 生存している変数のリスト
private:
	tLiveVariableMap * LiveIn; //!< このブロックの開始時点で生存している変数のリスト
	tLiveVariableMap * LiveOut; //!< このブロックの終了時点で生存している変数のリスト
	mutable void * Mark; //!< マーク
	mutable bool Traversing; //!< トラバース中かどうか
	bool Alive; //!< この基本ブロックが生きているかどうか

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

	//! @brief		Id を得る
	risse_size GetId() const { return Id; }

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
	void SetAlive(bool status = true) { Alive = status; }

	//! @brief		この基本ブロックが生存しているかどうかを返す
	bool GetAlive() const { return Alive; }

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

	//! @brief		ブロックに文を追加する
	//! @param		pos		スクリプト上の位置
	//! @param		code	オペレーションコード
	//! @param		ret_var	この文で変数を定義する場合はtSSAVariable *へのポインタを渡す
	//!						(変数を定義したくない場合はNULLを渡す)
	//! @param		using1	この文で使用する変数その1(NULL=使用しない)
	//! @param		using2	この文で使用する変数その2(NULL=使用しない)
	//! @param		using3	この文で使用する変数その3(NULL=使用しない)
	//! @param		using4	この文で使用する変数その4(NULL=使用しない)
	//! @return		新しく追加された文
	tSSAStatement * AddStatement(risse_size pos, tOpCode code,
		tSSAVariable ** ret_var,
			tSSAVariable *using1 = NULL,
			tSSAVariable *using2 = NULL,
			tSSAVariable *using3 = NULL,
			tSSAVariable *using4 = NULL
			);


	//! @brief		文を挿入する
	//! @param		stmt		挿入する文
	//! @param		point		挿入する場所
	void InsertStatement(tSSAStatement * stmt, tStatementInsertPoint point);

	//! @brief		文を挿入する
	//! @param		stmt		挿入する文
	//! @param		point		挿入する文(この文の次に挿入される)
	void InsertStatement(tSSAStatement * stmt, tSSAStatement * after);

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
	//!				(どうせ削除されて消えるため)
	void DeletePred(risse_size index);

	//! @brief		直後の基本ブロックを削除する
	//! @param		index		基本ブロックのインデックス(0～)
	//! @note		このメソッドは AddSucc() と異なり、削除されようとする
	//!				基本ブロックの直前ブロックの情報は書き換えない
	//!				(どうせ削除されて消えるため)
	void DeleteSucc(risse_size index);

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

	//! @brief		死んでいるSuccがあれば削除する
	void DeleteDeadSucc();

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

	//! @brief		LiveIn/Out の変数の合併を行う
	//! @param		old_var		合併前の変数
	//! @param		new_var		合併後の変数
	//! @note		old_var がもしあれば、それを new_var に変更する。
	//!				new_var がすでに存在していた場合は old_var を消すだけ。
	void CoalesceLiveness(const tSSAVariable * old_var, const tSSAVariable * new_var);

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

	//! @brief		すべての文のマークを解除する
	void ClearStatementMarks();

	//! @brief		共有の刺さった変数へのアクセスを別形式の文に変換
	void ConvertSharedVariableAccess();

	//! @brief		LiveIn と LiveOut を作成する
	void CreateLiveInAndLiveOut();

	//! @brief		変数の生存区間を基本ブロック単位で解析する
	void AnalyzeVariableBlockLiveness();

	//! @brief		すべての文に通し番号を設定する
	//! @param		order		通し番号の開始値 (終了時には文の数が加算されている)
	void SetOrder(risse_size & order);

	//! @brief		変数の干渉グラフを作成する
	void CreateVariableInterferenceGraph();

	//! @brief		すべての文を列挙する
	//! @param		statements		文を格納する先
	void ListAllStatements(gc_map<risse_size, tSSAStatement *> &statements) const;

	//! @brief		すべての文を列挙する
	//! @param		statements		文を格納する先
	void ListAllStatements(gc_vector<tSSAStatement *> &statements) const;

	//! @brief		型伝播解析・定数伝播解析を行う
	//! @param		variables		変数の作業リスト
	//! @param		blocks			ブロックの作業リスト
	void AnalyzeConstantPropagation(
			gc_vector<tSSAVariable *> &variables,
			gc_vector<tSSABlock *> &blocks);

	//! @brief		事前にしらべた型伝播解析・定数伝播解析のエラーを実際に発生させたり、警告を表示したりする
	//! @note		エラー情報は各文の Mark に設定されている
	void RealizeConstantPropagationErrors();

	//! @brief		型 Assertion コードを挿入する
	void InsertTypeAssertion();

	//! @brief		定数代入を置き換える
	void ReplaceConstantAssign();

	//! @brief		変数の合併を行うための、どの変数とどの変数が合併できそうかのリストを作成する
	void TraceCoalescable();

	//! @brief		変数の合併を行う
	void Coalesce();

	//! @brief		φ関数を削除する
	//! @note		このメソッド実行後はSSA形式としての性質は保てなくなる。
	void RemovePhiStatements();

	//! @brief		3番地形式の格納先が他の変数と異なっていることを保証(暫定処置)
	void Check3AddrAssignee();

	//! @brief		変数にレジスタを割り当てる
	void AssignRegisters(gc_vector<void*> & assign_work);

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

