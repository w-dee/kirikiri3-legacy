//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief SSA形式
//---------------------------------------------------------------------------
#ifndef risseSSAFormH
#define risseSSAFormH

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
class tSSAStatement;
class tSSALocalNamespace;
class tCodeGenerator;
class tCodeBlock;
class tSSAVariableAccessMap;

//---------------------------------------------------------------------------
/**
 * break あるいは continue の情報
 * @note	break や continue は goto と同様、ラベルへのジャンプとして扱われている
 */
class tBreakInfo : public tCollectee
{
	tSSAForm * Form; //!< この情報を保持するSSA形式インスタンス

	typedef gc_vector<tSSABlock *> tPendingJumpSourceBlocks;
	tPendingJumpSourceBlocks PendingJumpSourceBlocks;
	bool IsBlock; //!< この break/continue が block に対する物か
				//!< (block に対する breakやcontinueは他と意味が異なるため)
	bool NonValueBreakShouldSetVoidToLastEvalValue;
		//!< 値なし break が _ に void をセットすべきかどうか(デフォルトでtrueなので注意)
	tString JumpTargetLabel; //!< ジャンプ先のラベル名

public:
	/**
	 * コンストラクタ
	 * @param form			この情報を保持するSSA形式インスタンス
	 * @param label_prefix	ラベル名プリフィックス
	 */
	tBreakInfo(tSSAForm * form,
		const tString & label_prefix = tString::GetEmptyString());

	/**
	 * この break/continue が block に対する物かを設定する
	 * @param b	この break/continue が block に対する物か
	 */
	void SetIsBlock(bool b) { IsBlock = b; }

	/**
	 * この break/continue が block に対する物かを取得する
	 * @return	この break/continue が block に対する物か
	 */
	bool GetIsBlock() const { return IsBlock; }

	/**
	 * 値なし break が _ に void をセットすべきかどうかを設定する
	 * @param b	値なし break が _ に void をセットすべきかどうか
	 */
	void SetNonValueBreakShouldSetVoidToLastEvalValue(bool b)
		{ NonValueBreakShouldSetVoidToLastEvalValue = b; }

	/**
	 * 値なし break が _ に void をセットすべきかどうかを得る
	 * @return	値なし break が _ に void をセットすべきかどうか
	 */
	bool GetNonValueBreakShouldSetVoidToLastEvalValue() const
		{ return NonValueBreakShouldSetVoidToLastEvalValue; }

	/**
	 * この break のジャンプ先のラベル名を得る
	 * @return	この break のジャンプ先のラベル名
	 */
	const tString & GetJumpTargetLabel() const { return JumpTargetLabel; }

	/**
	 * 未バインドのジャンプを追加する
	 * @param jump_block	ジャンプもとの基本ブロック
	 */
	void AddJump(tSSABlock * jump_block);

	/**
	 * 未バインドのジャンプをすべてバインドする
	 * @param target	ターゲットのブロック
	 */
	void BindAll(tSSABlock * target);
};
//---------------------------------------------------------------------------
typedef tBreakInfo tContinueInfo;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * switch 文に関する情報を表すクラス
 */
class tSwitchInfo : public tCollectee
{
	tSSAVariable * Reference; //!< 参照変数 ( switch の次のカッコの中の値 )
	tSSABlock * LastBlock; //!< 最後のジャンプまたは分岐文を含む基本ブロック
	tSSAStatement * LastStatement; //!< 最後のジャンプまたは分岐文
	tSSABlock * DefaultBlock; //!< default 文のあるブロック

public:
	/**
	 * コンストラクタ
	 * @param node		SwitchのASTノード
	 * @param reference	参照変数
	 */
	tSwitchInfo(tSSAVariable * reference)
	{
		Reference = reference;
		LastBlock = NULL;
		LastStatement = NULL;
		DefaultBlock = NULL;
	}

	/**
	 * 参照変数を得る
	 * @return	参照変数
	 */
	tSSAVariable * GetReference() const { return Reference; }

	/**
	 * 最後のジャンプまたは分岐文を含む基本ブロックを得る
	 * @return	最後のジャンプまたは分岐文を含む基本ブロック
	 */
	tSSABlock * GetLastBlock() const { return LastBlock; }

	/**
	 * 最後のジャンプまたは分岐文を含む基本ブロックを設定する
	 * @param block	最後のジャンプまたは分岐文を含む基本ブロック
	 */
	void SetLastBlock(tSSABlock * block) { LastBlock = block; }

	/**
	 * 最後のジャンプまたは分岐文を得る
	 * @return	最後のジャンプまたは分岐文
	 */
	tSSAStatement * GetLastStatement() const { return LastStatement; }

	/**
	 * 最後のジャンプまたは分岐文を含む基本ブロックを設定する
	 * @param stmt	最後のジャンプまたは分岐文
	 */
	void SetLastStatement(tSSAStatement * stmt) { LastStatement = stmt; }

	/**
	 * default 文のあるブロックを得る
	 * @return	default 文のあるブロック
	 */
	tSSABlock * GetDefaultBlock() const { return DefaultBlock; }

	/**
	 * default 文のあるブロックを設定する
	 * @param block	default 文のあるブロック
	 */
	void SetDefaultBlock(tSSABlock * block) { DefaultBlock = block; }

};
//---------------------------------------------------------------------------


class tCompilerFunction;
class tSSAVariableAccessMap;
//---------------------------------------------------------------------------
/**
 * SSA形式を表すクラス
 */
class tSSAForm : public tCollectee
{
	tCompilerFunction * Function; //!< この SSA 形式が含まれる関数インスタンス
	tSSAForm * Parent; //!< この SSA 形式インスタンスの親インスタンス
	bool UseParentFrame; //!< 親SSA形式インスタンスのフレームを使うかどうか
	gc_vector<tSSAForm *> Children; //!< この SSA形式インスタンスの子インスタンスの配列

	bool CanReturn; //!< このSSA形式からはreturn文で戻ることが可能
	typedef gc_map<tString, risse_size> tExitTryBranchTargetLabels;
		//!< このSSA形式が受け取る可能性のあるラベルジャンプ先とその分岐インデックス(0～)のtypedef
	tExitTryBranchTargetLabels *ExitTryBranchTargetLabels;
		//!< このSSA形式が受け取る可能性のあるラベルジャンプ先とその分岐インデックス(0～)
	risse_size TryIdentifierIndex; //!< このSSA形式がtryブロックなどの場合、親SSA形式内の該当tryブロックのtry識別子を表す

	/**
	 * このSSA形式が保持しているTryCallしたあとのCatchBranch文と、
	 * そのTryCallの戻り値(例外の値)の情報
	 */
	struct tCatchBranchAndExceptionValue : public tCollectee
	{
		tCatchBranchAndExceptionValue(
			tSSAStatement * catch_branch_stmt,
			tSSAVariable * except_value,
			tExitTryBranchTargetLabels * target_labels)
		{
			CatchBranchStatement = catch_branch_stmt;
			ExceptionValue = except_value;
			ExitTryBranchTargetLabels = target_labels;
		}
		tSSAStatement * CatchBranchStatement; //!< CatchBranch文
		tSSAVariable * ExceptionValue; //!< TryCallの際の例外の値
		tExitTryBranchTargetLabels * ExitTryBranchTargetLabels;
	};
	typedef gc_map<risse_size, tCatchBranchAndExceptionValue *> tCatchBranchAndExceptionMap;
		//!< このSSA形式が保持しているTri識別子と、
		//!< TryCallしたあとのCatchBranch文/そのTryCallの戻り値の情報のリストのtypedef
	tCatchBranchAndExceptionMap CatchBranchAndExceptionMap;
		//!< このSSA形式が保持しているTri識別子と、
		//!< TryCallしたあとのCatchBranch文/そのTryCallの戻り値の情報のリスト

	tString Name; //!< このSSA形式インスタンスの名前
	tSSALocalNamespace * LocalNamespace; //!< ローカル名前空間

	tSSABlock * EntryBlock; //!< エントリーSSA基本ブロック
	tSSABlock * CurrentBlock; //!< 現在変換中の基本ブロック

	tSwitchInfo * CurrentSwitchInfo; //!< 現在の switch に関する情報
	tBreakInfo * CurrentBreakInfo; //!< 現在の break に関する情報
	tContinueInfo * CurrentContinueInfo; //!< 現在の continue に関する情報

	tSSAVariable * FunctionCollapseArgumentVariable; //!< 関数引数の無名の * を保持している変数

	tCodeGenerator * CodeGenerator; //!< バイトコードジェネレータのインスタンス
	tCodeBlock * CodeBlock; //!< コードブロック
	risse_size CodeBlockIndex; //!< コードブロックのスクリプトブロック内でのインデックス

public:
	/**
	 * このSSA形式の状態を表す列挙型
	 */
	enum tState {
		ssSSA /*!< 有効な SSA 形式である */,
		ssNonSSA /*!< phi関数の除去などでSSA性は失われた */,
		ssCodeGenerated /*!< すでにコードジェネレータにたいしてコードを生成した後である */
	};

private:
	tState State; //!< ステート

public:
	/**
	 * コンストラクタ
	 * @param pos				ソースコード上の位置
	 * @param function			この SSA 形式が含まれる関数インスタンス
	 * @param name				このSSA形式インスタンスの名前
	 * @param parent			親SSA形式インスタンス
	 * @param useparentframe	親SSA形式インスタンスのフレームを使うかどうか
	 */
	tSSAForm(risse_size pos, tCompilerFunction * function, const tString & name,
		tSSAForm * parent, bool useparentframe);

	/**
	 * AST を SSA 形式に変換する
	 * @param roots	ASTのルートノードの配列
	 */
	void Generate(const gc_vector<tASTNode *> & roots);

	/**
	 * AST を SSA 形式に変換する
	 * @param root	ASTのルートノード
	 */
	void Generate(const tASTNode * root);

	/**
	 * SSA形式の最適化と通常形式への変換
	 */
	void OptimizeAndUnSSA();

	/**
	 * 関数インスタンスを得る
	 * @return	関数インスタンス
	 */
	tCompilerFunction * GetFunction() const { return Function; }

	/**
	 * 親SSA形式インスタンスを得る
	 */
	tSSAForm * GetParent() const { return Parent; }

	/**
	 * 親SSA形式インスタンスのフレームを使うかどうかを得る
	 */
	bool GetUseParentFrame() const { return UseParentFrame; }

	/**
	 * スクリプトブロックインスタンスを得る
	 * @return	スクリプトブロックインスタンス
	 */
	tScriptBlockInstance * GetScriptBlockInstance() const;

	/**
	 * このSSA形式インスタンスの名前を設定する
	 * @param name	このSSA形式インスタンスの名前
	 */
	void SetName(const tString & name) { Name = name; }

	/**
	 * このSSA形式インスタンスの名前を得る
	 * @return	このSSA形式インスタンスの名前
	 */
	const tString & GetName() const { return Name; }

	/**
	 * ローカル名前空間を得る
	 * @return	ローカル名前空間
	 */
	tSSALocalNamespace * GetLocalNamespace() const { return LocalNamespace; }

	/**
	 * this-proxyを得る
	 * @return	this-proxy
	 */
	tSSAVariable * GetThisProxy(risse_size pos);

	/**
	 * 現在変換中の基本ブロックを得る
	 * @return	現在変換中の基本ブロック
	 */
	tSSABlock * GetCurrentBlock() const { return CurrentBlock; }

	/**
	 * 関数引数の無名の * を保持している変数を得る
	 * @return	関数引数の無名の * を保持している変数
	 */
	tSSAVariable * GetFunctionCollapseArgumentVariable() const
		{ return FunctionCollapseArgumentVariable; }

	/**
	 * Try識別子のインデックスを設定する
	 * @param idx	Try識別子のインデックス
	 */
	void SetTryIdentifierIndex(risse_size idx) { TryIdentifierIndex = idx; }

	/**
	 * Try識別子のインデックスを得る
	 * @return	Try識別子のインデックス(risse_size_max が帰った場合は無効)
	 */
	risse_size GetTryIdentifierIndex() const { return TryIdentifierIndex; }

public:
	/**
	 * _ 変数に値を書き込む
	 * @param pos	スクリプト上の位置
	 * @param value	値(NULLの場合は何もしない)
	 */
	void WriteLastEvalResult(risse_size pos, tSSAVariable * value);

	/**
	 * 新しい基本ブロックを作成する
	 * @param name	基本ブロック名プリフィックス
	 * @param ns	引き継ぐローカル名前空間(nullを指定すると現在の名前空間を引き継ぐ)
	 * @return	新しく作成された基本ブロック
	 */
	tSSABlock * CreateNewBlock(const tString & name, const tSSALocalNamespace * ns = NULL);

private:
	/**
	 * 任意の ExitTryBranchTargetLabels にマップを追加する(AddExitTryBranchTargetLabel()から呼ばれる)
	 * @param target_label	ExitTryBranchTargetLabels
	 * @param label			ラベル名または "@return" または "@break" のような特別なジャンプ先の名前
	 * @return	そのラベルを表すインデックス
	 */
	static risse_size InternalAddExitTryBranchTargetLabel(
				tExitTryBranchTargetLabels * target_label, 
				const tString & label);

public:
	/**
	 * このSSA形式インスタンスが保持している任意の ExitTryBranchTargetLabels にマップを追加する
	 * @param try_id		try識別子
	 * @param target_label	ExitTryBranchTargetLabels
	 * @param label			ラベル名または "@return" または "@break" のような特別なジャンプ先の名前
	 * @return	そのラベルを表すインデックス
	 */
	risse_size AddExitTryBranchTargetLabel(
				risse_size try_id,
				const tString & label);

	/**
	 * ExitTryBranchTargetLabels にマップを追加する
	 * @param label	ラベル名または "@return" または "@break" のような特別なジャンプ先の名前
	 * @return	そのラベルを表すインデックス
	 * @note	AddExitTryBranchTargetLabelのtry_id指定無し版と違い、
	 *			このメソッドは現在の ExitTryBranchTargetLabels を対象に用いる。
	 */
	risse_size AddExitTryBranchTargetLabel(const tString & label);

	/**
	 * 現在の基本ブロックに定数値を得る文を追加する
	 * @param pos	スクリプト上の位置
	 * @param val	定数
	 * @return	定数値を表す一時変数
	 * @note	このメソッドは、定数値を一時変数に代入する
	 *			文を生成し、その一時変数を返す
	 */
	tSSAVariable * AddConstantValueStatement(risse_size pos, const tVariant & val);

	/**
	 * 現在の基本ブロックに文を追加する
	 * @param pos		スクリプト上の位置
	 * @param code		オペレーションコード
	 * @param ret_var	この文で変数を定義する場合はtSSAVariable *へのポインタを渡す
	 *					(変数を定義したくない場合はNULLを渡す)
	 * @param using1	この文で使用する変数その1(NULL=使用しない)
	 * @param using2	この文で使用する変数その2(NULL=使用しない)
	 * @param using3	この文で使用する変数その3(NULL=使用しない)
	 * @param using4	この文で使用する変数その4(NULL=使用しない)
	 * @return	新しく追加された文
	 */
	tSSAStatement * AddStatement(risse_size pos, tOpCode code,
		tSSAVariable ** ret_var,
			tSSAVariable *using1 = NULL,
			tSSAVariable *using2 = NULL,
			tSSAVariable *using3 = NULL,
			tSSAVariable *using4 = NULL
			);

	/**
	 * 現在の基本ブロックに文を追加し、定義された変数を返す
	 * @param pos		スクリプト上の位置
	 * @param code		オペレーションコード
	 * @param using1	この文で使用する変数その1(NULL=使用しない)
	 * @param using2	この文で使用する変数その2(NULL=使用しない)
	 * @param using3	この文で使用する変数その3(NULL=使用しない)
	 * @param using4	この文で使用する変数その4(NULL=使用しない)
	 * @return	定義された変数
	 */
	tSSAVariable * AddVariableWithStatement(risse_size pos, tOpCode code,
			tSSAVariable *using1 = NULL, 
			tSSAVariable *using2 = NULL, 
			tSSAVariable *using3 = NULL,
			tSSAVariable *using4 = NULL)
	{
		tSSAVariable * ret_var = NULL;
		AddStatement(pos, code, &ret_var, using1, using2, using3, using4);
		return ret_var;
	}

	/**
	 * 現在の基本ブロックにreturn文を生成する
	 * @param pos	スクリプト上の位置
	 * @param var	returnする値 (NULL=voidを返す)
	 */
	void AddReturnStatement(risse_size pos, tSSAVariable * var);

	/**
	 * 現在の switch に関する情報を得る
	 * @return	現在の switch に関する情報
	 */
	tSwitchInfo * GetCurrentSwitchInfo() const { return CurrentSwitchInfo; }

	/**
	 * 現在の switch に関する情報を設定する
	 * @param info	現在の switch に関する情報
	 * @return	設定前の switch に関する情報
	 */
	tSwitchInfo * SetCurrentSwitchInfo(tSwitchInfo * info)
	{
		tSwitchInfo * prev = CurrentSwitchInfo;
		CurrentSwitchInfo = info;
		return prev;
	}

	/**
	 * 現在の break に関する情報を得る
	 * @return	現在の break に関する情報
	 */
	tBreakInfo * GetCurrentBreakInfo() const { return CurrentBreakInfo; }

	/**
	 * 現在の break に関する情報を設定する
	 * @param info	現在の break に関する情報
	 * @return	設定前の break に関する情報
	 */
	tBreakInfo * SetCurrentBreakInfo(tBreakInfo * info)
	{
		tBreakInfo * prev = CurrentBreakInfo;
		CurrentBreakInfo = info;
		return prev;
	}

	/**
	 * 現在の continue に関する情報を得る
	 * @return	現在の continue に関する情報
	 */
	tContinueInfo * GetCurrentContinueInfo() const { return CurrentContinueInfo; }

	/**
	 * 現在の continue に関する情報を設定する
	 * @param info	現在の continue に関する情報
	 * @return	設定前の continue に関する情報
	 */
	tContinueInfo * SetCurrentContinueInfo(tContinueInfo * info)
	{
		tContinueInfo * prev = CurrentContinueInfo;
		CurrentContinueInfo = info;
		return prev;
	}

	/**
	 * break 文を追加する
	 * @param pos	スクリプト上の位置
	 * @param var	breakに伴った値 (NULL=voidを返す)
	 */
	void AddBreakStatement(risse_size pos, tSSAVariable * var);

	/**
	 * continue 文を追加する
	 * @param pos	スクリプト上の位置
	 * @param var	continueに伴った値 (NULL=voidを返す)
	 */
	void AddContinueStatement(risse_size pos, tSSAVariable * var);

private:
	/**
	 * ocCatchBranch文にreturn例外などを受け取るための分岐先とそのブロックを
	 * 作成する
	 * @param catch_branch_stmt	ocCatchBranch文(すでに2つのused
	 *							それぞれ例外が発生しなかったときと例外が
	 *							発生したときのジャンプ先が登録されていること)
	 * @param except_value		例外オブジェクトを示す変数
	 * @param target_labels		ExitTryBranchTargetLabels
	 * @note	このSSAインスタンス内のCurrentBlock は保存される
	 */
	void AddCatchBranchTargetsForOne(tSSAStatement * catch_branch_stmt,
					tSSAVariable * except_value,
					tExitTryBranchTargetLabels * target_labels);

public:
	/**
	 * ocCatchBranch文にreturn例外などを受け取るための分岐先とそのブロックを
	 * 作成する
	 * @note	このSSAインスタンス内のCurrentBlock は保存される
	 */
	void AddCatchBranchTargets();

	/**
	 * CatchBranch文を追加する
	 * @param catch_branch_stmt	ocCatchBranch文(すでに2つのused
	 *							それぞれ例外が発生しなかったときと例外が
	 *							発生したときのジャンプ先が登録されていること)
	 * @param except_value		例外オブジェクトを示す変数
	 */
	void AddCatchBranchAndExceptionValue(tSSAStatement * catch_branch_stmt,
					tSSAVariable * except_value);

protected:
	/**
	 * CreateLazyBlock で返される情報の構造体
	 */
	struct tLazyBlockParam : public tCollectee
	{
		tSSAForm * NewForm; //!< 新しいSSA形式インスタンス(遅延評価ブロックを表す)
		risse_size Position; //!< スクリプト上の位置
		tSSAVariable *BlockVariable; //!< 遅延評価ブロックを表す変数
		tSSAVariableAccessMap * AccessMap; //!< アクセスマップ
	};
public:
	/**
	 * AccessMap を作成する
	 * @param pos	スクリプト上の位置
	 * @return	作成された AccessMap
	 * @note	ここで作成したアクセスマップは CreateLazyBlock() に
	 *			渡し、使い終わったら CleanupAccessMap() を呼ぶこと。
	 *			このメソッドはCreateLazyBlockでsharevars=falseの場合に必須。
	 */
	tSSAVariableAccessMap * CreateAccessMap(risse_size pos);

	/**
	 * 新しい遅延評価ブロックを作成する
	 * @param pos		スクリプト上の位置
	 * @param basename	新しい遅延評価ブロックの名前(実際にはこれにさらに連番がつく)
	 * @param sharevars	遅延評価ブロックからブロック外の変数を参照された場合に
	 *					その参照された変数を共有変数として扱うかどうか。
	 *					変数を共有することにより、遅延評価ブロックを、そのブロックが定義
	 *					された位置以外から呼び出しても安全に変数にアクセスできるように
	 *					なる ( function 内 function でレキシカルクロージャを使用するとき
	 *					などに有効 )
	 * @param accessmap	アクセスマップ (sharevars が false の場合のみ必須, trueの場合は不要)
	 * @param new_form	新しく作成されたSSA形式のインスタンス
	 * @param block_var	その遅延評価ブロックを表すSSA変数を格納する先
	 * @return	CleanupLazyBlock() に渡すべき情報
	 * @note	このメソッドは遅延評価ブロックを作成してその遅延評価ブロックを
	 *			表す変数をblock_varに返す。遅延評価ブロックを表すSSA形式はnew_form
	 *			に返されるが、中身は空っぽなので呼び出しがわで内容を生成すること。
	 *			内容を生成したら、ListVariablesForLazyBlock() を呼ぶこと。
	 *			block_var に返される変数はメソッドオブジェクトなので、呼び出して使う。
	 *			使い終わったらCleanupLazyBlock() メソッドを呼ぶこと。
	 */
	void * CreateLazyBlock(risse_size pos, const tString & basename,
		bool sharevars, tSSAVariableAccessMap * accessmap,
		tSSAForm *& new_form, tSSAVariable *& block_var);

	/**
	 * 遅延評価ブロックで使用されている変数を親から子へコピーする
	 * @param pos		スクリプト上の位置
	 * @param accessmap	アクセスマップ ( CreateAccessMap() で作成された物を指定)
	 */
	void ListVariablesForLazyBlock(risse_size pos, tSSAVariableAccessMap * accessmap);

	/**
	 * 遅延評価ブロックのクリーンアップ処理を行う
	 * @param param	CreateLazyBlock() の戻り値
	 */
	void CleanupLazyBlock(void * param);

	/**
	 * アクセスマップのクリーンアップ処理を行う
	 * @param pos		スクリプト上の位置
	 * @param accessmap	アクセスマップ ( CreateAccessMap() で作成された物を指定)
	 */
	void CleanupAccessMap(risse_size pos, tSSAVariableAccessMap * accessmap);

	/**
	 * バインディングマップを追加する
	 * @param pos		スクリプト上の位置
	 * @param binding	バインディングオブジェクトを表すSSA形式変数
	 * @note	このメソッドを実行すると、現時点で可視な変数がすべて共有変数に
	 *			なる。
	 */
	void AddBindingMap(risse_size pos, tSSAVariable * binding);

	/**
	 * ダンプを行う
	 * @return	ダンプ文字列
	 */
	tString Dump() const;

//----
public:
	/**
	 * 実行ブロックの最後の return を生成する
	 * @param root	実行ブロックを表すASTノード
	 */
	void GenerateLastReturn(const tASTNode * root);

	/**
	 * 到達しない直前ブロックを削除する
	 * @note	この時点では条件付き定数伝播解析を行っていない
	 */
	void LeapDeadPredBlocks();

	/**
	 * 到達しないブロックを削除する
	 * @note	この処理は条件付き定数伝播解析の結果を利用する
	 */
	void LeapDeadBlocks();

	/**
	 * 共有された変数へのアクセスを別形式の文に変換
	 */
	void ConvertSharedVariableAccess();

	/**
	 * 変数の生存区間を基本ブロック単位で解析する(すべての変数に対して)
	 */
	void AnalyzeVariableBlockLiveness();

	/**
	 * 変数の生存区間を基本ブロック単位で解析する(個別の変数に対して)
	 * @param var	変数
	 */
	void AnalyzeVariableBlockLiveness(tSSAVariable * var);

	/**
	 * 文の前後関係を高速に判断するため、文に通し番号を振る
	 */
	void SetStatementOrder();

	// @brief		変数の干渉グラフを作成する
	void CreateVariableInterferenceGraph();

	// @brief		文レベルでの最適化を行う
	void OptimizeStatement();

	/**
	 * 型 Assertion コードを挿入する
	 */
	void InsertTypeAssertion();

	/**
	 * 定数代入を置き換える
	 */
	void ReplaceConstantAssign();

	/**
	 * 死んでる変数を除去する
	 */
	void DeleteDeadVariables();

	/**
	 * 変数の合併を行うために、どの変数が合併できそうかどうかを調査する
	 */
	void TraceCoalescable();

	/**
	 * TraceCoalescable() の結果に基づいて変数の合併を行う
	 * @note	これによって SSA性が破壊される
	 */
	void Coalesce();

	/**
	 * φ関数を削除する
	 * @note	SSA形式->通常形式の変換過程においてφ関数を削除する処理がこれ
	 */
	void RemovePhiStatements();

	/**
	 * 3番地形式の格納先が他の変数と異なっていることを保証(暫定処置)
	 */
	void Check3AddrAssignee();

	/**
	 * 変数にレジスタを割り当てる
	 */
	void AssignRegisters();

	/**
	 * バイトコードジェネレータのインスタンスを生成する
	 */
	void EnsureCodeGenerator();

	/**
	 * バイトコードジェネレータを得る
	 * @return	バイトコードジェネレータ
	 */
	tCodeGenerator * GetCodeGenerator() const { return CodeGenerator; }

	/**
	 * コードブロックを得る
	 * @return	コードブロック
	 */
	tCodeBlock * GetCodeBlock() const { return CodeBlock; }

	/**
	 * コードブロックのスクリプトブロック内でのインデックスを得る
	 * @return	コードブロックのスクリプトブロック内でのインデックス
	 */
	risse_size GetCodeBlockIndex() const { return CodeBlockIndex; }

	/**
	 * バイトコードを生成する
	 */
	void GenerateCode();

	/**
	 * 最大の共有変数のネストカウントを設定する
	 * @note	トップレベルの関数は、共有変数のネストカウントに応じたバッファを
	 *			確保しなければならないため、ネスト数が最大でどれほどまでに行くの
	 *			かを知っていなくてはならない。このメソッドはネストレベルが0の
	 *			関数(つまり、関数グループのうち一番最初に実行される関数)の先頭のSSA形式
	 *			にのみ有効である。このメソッドはコードジェネレータに
	 *			共有変数のネストレベルを問い合わせ、それをコードブロックに設定する。
	 */
	void SetSharedVariableNestCount();

	/**
	 * 状態を得る
	 * @return	状態
	 */
	tState GetState() const { return State; }
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
#endif

