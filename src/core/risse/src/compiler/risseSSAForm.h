//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
class tRisseSSABlock;
class tRisseSSAStatement;
class tRisseSSALocalNamespace;
class tRisseCodeGenerator;
class tRisseCodeBlock;
class tRisseSSAVariableAccessMap;

//---------------------------------------------------------------------------
//! @brief	break あるいは continue の情報
//! @note	break や continue は goto と同様、ラベルへのジャンプとして扱われている
//---------------------------------------------------------------------------
class tRisseBreakInfo : public tRisseCollectee
{
	tRisseSSAForm * Form; //!< この情報を保持するSSA形式インスタンス

	typedef gc_vector<tRisseSSABlock *> tPendingJumpSourceBlocks;
	tPendingJumpSourceBlocks PendingJumpSourceBlocks;
	bool IsBlock; //!< この break/continue が block に対する物か
				//!< (block に対する breakやcontinueは他と意味が異なるため)
	bool NonValueBreakShouldSetVoidToLastEvalValue;
		//!< 値なし break が _ に void をセットすべきかどうか(デフォルトでtrueなので注意)
	tRisseString JumpTargetLabel; //!< ジャンプ先のラベル名

public:
	//! @brief		コンストラクタ
	//! @param		form			この情報を保持するSSA形式インスタンス
	//! @param		label_prefix	ラベル名プリフィックス
	tRisseBreakInfo(tRisseSSAForm * form,
		const tRisseString & label_prefix = tRisseString::GetEmptyString());

	//! @brief		この break/continue が block に対する物かを設定する
	//! @param		b この break/continue が block に対する物か
	void SetIsBlock(bool b) { IsBlock = b; }

	//! @brief		この break/continue が block に対する物かを取得する
	//! @return		この break/continue が block に対する物か
	bool GetIsBlock() const { return IsBlock; }

	//! @brief		値なし break が _ に void をセットすべきかどうかを設定する
	//! @param		b 値なし break が _ に void をセットすべきかどうか
	void SetNonValueBreakShouldSetVoidToLastEvalValue(bool b)
		{ NonValueBreakShouldSetVoidToLastEvalValue = b; }

	//! @brief		値なし break が _ に void をセットすべきかどうかを得る
	//! @return		値なし break が _ に void をセットすべきかどうか
	bool GetNonValueBreakShouldSetVoidToLastEvalValue() const
		{ return NonValueBreakShouldSetVoidToLastEvalValue; }

	//! @brief		この break のジャンプ先のラベル名を得る
	//! @return		この break のジャンプ先のラベル名
	const tRisseString & GetJumpTargetLabel() const { return JumpTargetLabel; }

	//! @brief		未バインドのジャンプを追加する
	//! @param		jump_block		ジャンプもとの基本ブロック
	void AddJump(tRisseSSABlock * jump_block);

	//! @brief		未バインドのジャンプをすべてバインドする
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


class tRisseCompilerFunction;
class tRisseSSAVariableAccessMap;
//---------------------------------------------------------------------------
//! @brief	SSA形式を表すクラス
//---------------------------------------------------------------------------
class tRisseSSAForm : public tRisseCollectee
{
	tRisseCompilerFunction * Function; //!< この SSA 形式が含まれる関数インスタンス
	tRisseSSAForm * Parent; //!< この SSA 形式インスタンスの親インスタンス
	bool UseParentFrame; //!< 親SSA形式インスタンスのフレームを使うかどうか
	gc_vector<tRisseSSAForm *> Children; //!< この SSA形式インスタンスの子インスタンスの配列

	bool CanReturn; //!< このSSA形式からはreturn文で戻ることが可能
	typedef gc_map<tRisseString, risse_size> tExitTryBranchTargetLabels;
		//!< このSSA形式が受け取る可能性のあるラベルジャンプ先とその分岐インデックス(0～)のtypedef
	tExitTryBranchTargetLabels *ExitTryBranchTargetLabels;
		//!< このSSA形式が受け取る可能性のあるラベルジャンプ先とその分岐インデックス(0～)
	risse_size TryIdentifierIndex; //!< このSSA形式がtryブロックなどの場合、親SSA形式内の該当tryブロックのtry識別子を表す

	//! @brief	このSSA形式が保持しているTryCallしたあとのCatchBranch文と、
	//!			そのTryCallの戻り値(例外の値)の情報
	struct tCatchBranchAndExceptionValue : public tRisseCollectee
	{
		tCatchBranchAndExceptionValue(
			tRisseSSAStatement * catch_branch_stmt,
			tRisseSSAVariable * except_value,
			tExitTryBranchTargetLabels * target_labels)
		{
			CatchBranchStatement = catch_branch_stmt;
			ExceptionValue = except_value;
			ExitTryBranchTargetLabels = target_labels;
		}
		tRisseSSAStatement * CatchBranchStatement; //!< CatchBranch文
		tRisseSSAVariable * ExceptionValue; //!< TryCallの際の例外の値
		tExitTryBranchTargetLabels * ExitTryBranchTargetLabels;
	};
	typedef gc_map<risse_size, tCatchBranchAndExceptionValue *> tCatchBranchAndExceptionMap;
		//!< このSSA形式が保持しているTri識別子と、
		//!< TryCallしたあとのCatchBranch文/そのTryCallの戻り値の情報のリストのtypedef
	tCatchBranchAndExceptionMap CatchBranchAndExceptionMap;
		//!< このSSA形式が保持しているTri識別子と、
		//!< TryCallしたあとのCatchBranch文/そのTryCallの戻り値の情報のリスト

	tRisseString Name; //!< このSSA形式インスタンスの名前
	risse_int UniqueNumber; //!< ユニークな番号 (変数のバージョン付けに用いる)
	tRisseSSALocalNamespace * LocalNamespace; //!< ローカル名前空間

	tRisseSSAVariable * ThisProxy; //!< this-proxy (thisにメンバがなければglobalを見に行くオブジェクト)が入っているSSA変数

	tRisseSSABlock * EntryBlock; //!< エントリーSSA基本ブロック
	tRisseSSABlock * CurrentBlock; //!< 現在変換中の基本ブロック

	tRisseSwitchInfo * CurrentSwitchInfo; //!< 現在の switch に関する情報
	tRisseBreakInfo * CurrentBreakInfo; //!< 現在の break に関する情報
	tRisseContinueInfo * CurrentContinueInfo; //!< 現在の continue に関する情報

	tRisseSSAVariable * FunctionCollapseArgumentVariable; //!< 関数引数の無名の * を保持している変数

	tRisseCodeGenerator * CodeGenerator; //!< バイトコードジェネレータのインスタンス
	tRisseCodeBlock * CodeBlock; //!< コードブロック
	risse_size CodeBlockIndex; //!< コードブロックのスクリプトブロック内でのインデックス

public:
	//! @brief		コンストラクタ
	//! @param		pos				ソースコード上の位置
	//! @param		function		この SSA 形式が含まれる関数インスタンス
	//! @param		name			このSSA形式インスタンスの名前
	//! @param		parent			親SSA形式インスタンス
	//! @param		useparentframe	親SSA形式インスタンスのフレームを使うかどうか
	tRisseSSAForm(risse_size pos, tRisseCompilerFunction * function, const tRisseString & name,
		tRisseSSAForm * parent, bool useparentframe);

	//! @brief		AST を SSA 形式に変換する
	//! @param		roots	ASTのルートノードの配列
	void Generate(const gc_vector<tRisseASTNode *> & roots);

	//! @brief		AST を SSA 形式に変換する
	//! @param		root	ASTのルートノード
	void Generate(const tRisseASTNode * root);

	//! @brief		SSA形式の最適化と通常形式への変換
	void OptimizeAndUnSSA();

	//! @brief		関数インスタンスを得る
	//! @return		関数インスタンス
	tRisseCompilerFunction * GetFunction() const { return Function; }

	//! @brief		親SSA形式インスタンスを得る
	//! @param		親SSA形式インスタンス
	tRisseSSAForm * GetParent() const { return Parent; }

	//! @brief		親SSA形式インスタンスのフレームを使うかどうかを得る
	//! @param		親SSA形式インスタンスのフレームを使うかどうか
	bool GetUseParentFrame() const { return UseParentFrame; }

	//! @brief		スクリプトブロックインスタンスを得る
	//! @return		スクリプトブロックインスタンス
	tRisseScriptBlockBase * GetScriptBlock() const;

	//! @brief		このSSA形式インスタンスの名前を設定する
	//! @param		name		このSSA形式インスタンスの名前
	void SetName(const tRisseString & name) { Name = name; }

	//! @brief		このSSA形式インスタンスの名前を得る
	//! @return		このSSA形式インスタンスの名前
	const tRisseString & GetName() const { return Name; }

	//! @brief		ローカル名前空間を得る
	//! @return		ローカル名前空間
	tRisseSSALocalNamespace * GetLocalNamespace() const { return LocalNamespace; }

	//! @brief		this-proxyを得る
	//! @return		this-proxy
	tRisseSSAVariable * GetThisProxy() const { return ThisProxy; }

	//! @brief		現在変換中の基本ブロックを得る
	//! @return		現在変換中の基本ブロック
	tRisseSSABlock * GetCurrentBlock() const { return CurrentBlock; }

	//! @brief		関数引数の無名の * を保持している変数を得る
	//! @return		関数引数の無名の * を保持している変数
	tRisseSSAVariable * GetFunctionCollapseArgumentVariable() const
		{ return FunctionCollapseArgumentVariable; }

	//! @brief		Try識別子のインデックスを設定する
	//! @param		idx		Try識別子のインデックス
	void SetTryIdentifierIndex(risse_size idx) { TryIdentifierIndex = idx; }

	//! @brief		Try識別子のインデックスを得る
	//! @return		Try識別子のインデックス(risse_size_max が帰った場合は無効)
	risse_size GetTryIdentifierIndex() const { return TryIdentifierIndex; }

public:
	//! @brief		_ 変数に値を書き込む
	//! @param		pos		スクリプト上の位置
	//! @param		value	値(NULLの場合は何もしない)
	void WriteLastEvalResult(risse_size pos, tRisseSSAVariable * value);

	//! @brief		新しい基本ブロックを作成する
	//! @param		name	基本ブロック名プリフィックス
	//! @param		ns		引き継ぐローカル名前空間(nullを指定すると現在の名前空間を引き継ぐ)
	//! @return		新しく作成された基本ブロック
	tRisseSSABlock * CreateNewBlock(const tRisseString & name, const tRisseSSALocalNamespace * ns = NULL);

private:
	//! @brief		任意の ExitTryBranchTargetLabels にマップを追加する(AddExitTryBranchTargetLabel()から呼ばれる)
	//! @param		target_label	ExitTryBranchTargetLabels
	//! @param		label	ラベル名または "@return" または "@break" のような特別なジャンプ先の名前
	//! @return		そのラベルを表すインデックス
	static risse_size InternalAddExitTryBranchTargetLabel(
				tExitTryBranchTargetLabels * target_label, 
				const tRisseString & label);

public:
	//! @brief		このSSA形式インスタンスが保持している任意の ExitTryBranchTargetLabels にマップを追加する
	//! @param		try_id			try識別子
	//! @param		target_label	ExitTryBranchTargetLabels
	//! @param		label	ラベル名または "@return" または "@break" のような特別なジャンプ先の名前
	//! @return		そのラベルを表すインデックス
	risse_size AddExitTryBranchTargetLabel(
				risse_size try_id,
				const tRisseString & label);

	//! @brief		ExitTryBranchTargetLabels にマップを追加する
	//! @param		label	ラベル名または "@return" または "@break" のような特別なジャンプ先の名前
	//! @return		そのラベルを表すインデックス
	//! @note		AddExitTryBranchTargetLabelのtry_id指定無し版と違い、
	//!				このメソッドは現在の ExitTryBranchTargetLabels を対象に用いる。
	risse_size AddExitTryBranchTargetLabel(const tRisseString & label);

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

	//! @brief		現在の基本ブロックにreturn文を生成する
	//! @param		pos		スクリプト上の位置
	//! @param		var		returnする値 (NULL=voidを返す)
	void AddReturnStatement(risse_size pos, tRisseSSAVariable * var);

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

	//! @brief		break 文を追加する
	//! @param		pos			スクリプト上の位置
	//! @param		var			breakに伴った値 (NULL=voidを返す)
	void AddBreakStatement(risse_size pos, tRisseSSAVariable * var);

	//! @brief		continue 文を追加する
	//! @param		pos			スクリプト上の位置
	//! @param		var			continueに伴った値 (NULL=voidを返す)
	void AddContinueStatement(risse_size pos, tRisseSSAVariable * var);

private:
	//! @brief		ocCatchBranch文にreturn例外などを受け取るための分岐先とそのブロックを
	//!				作成する
	//! @param		catch_branch_stmt	ocCatchBranch文(すでに2つのused
	//!									それぞれ例外が発生しなかったときと例外が
	//!									発生したときのジャンプ先が登録されていること)
	//! @param		except_value		例外オブジェクトを示す変数
	//! @param		target_labels		ExitTryBranchTargetLabels
	//! @note		このSSAインスタンス内のCurrentBlock は保存される
	void AddCatchBranchTargetsForOne(tRisseSSAStatement * catch_branch_stmt,
					tRisseSSAVariable * except_value,
					tExitTryBranchTargetLabels * target_labels);

public:
	//! @brief		ocCatchBranch文にreturn例外などを受け取るための分岐先とそのブロックを
	//!				作成する
	//! @note		このSSAインスタンス内のCurrentBlock は保存される
	void AddCatchBranchTargets();

	//! @brief		CatchBranch文を追加する
	//! @param		catch_branch_stmt	ocCatchBranch文(すでに2つのused
	//!									それぞれ例外が発生しなかったときと例外が
	//!									発生したときのジャンプ先が登録されていること)
	//! @param		except_value		例外オブジェクトを示す変数
	void AddCatchBranchAndExceptionValue(tRisseSSAStatement * catch_branch_stmt,
					tRisseSSAVariable * except_value);

protected:
	//! @brief	CreateLazyBlock で返される情報の構造体
	struct tLazyBlockParam : public tRisseCollectee
	{
		tRisseSSAForm * NewForm; //!< 新しいSSA形式インスタンス(遅延評価ブロックを表す)
		risse_size Position; //!< スクリプト上の位置
		tRisseSSAVariable *BlockVariable; //!< 遅延評価ブロックを表す変数
		tRisseSSAVariableAccessMap * AccessMap; //!< アクセスマップ
	};
public:
	//! @brief		AccessMap を作成する
	//! @param		pos			スクリプト上の位置
	//! @return		作成された AccessMap
	//! @note		ここで作成したアクセスマップは CreateLazyBlock() に
	//!				渡し、使い終わったら CleanupAccessMap() を呼ぶこと。
	//!				このメソッドはCreateLazyBlockでsharevars=falseの場合に必須。
	tRisseSSAVariableAccessMap * CreateAccessMap(risse_size pos);

	//! @brief		新しい遅延評価ブロックを作成する
	//! @param		pos			スクリプト上の位置
	//! @param		basename	新しい遅延評価ブロックの名前(実際にはこれにさらに連番がつく)
	//! @param		sharevars	遅延評価ブロックからブロック外の変数を参照された場合に
	//!							その参照された変数を共有変数として扱うかどうか。
	//!							変数を共有することにより、遅延評価ブロックを、そのブロックが定義
	//!							された位置以外から呼び出しても安全に変数にアクセスできるように
	//!							なる ( function 内 function でレキシカルクロージャを使用するとき
	//!							などに有効 )
	//! @param		accessmap	アクセスマップ (sharevars が false の場合のみ必須, trueの場合は不要)
	//! @param		new_form	新しく作成されたSSA形式のインスタンス
	//! @param		block_var	その遅延評価ブロックを表すSSA変数を格納する先
	//! @return		CleanupLazyBlock() に渡すべき情報
	//! @note		このメソッドは遅延評価ブロックを作成してその遅延評価ブロックを
	//!				表す変数をblock_varに返す。遅延評価ブロックを表すSSA形式はnew_form
	//!				に返されるが、中身は空っぽなので呼び出しがわで内容を生成すること。
	//!				内容を生成したら、ListVariablesForLazyBlock() を呼ぶこと。
	//!				block_var に返される変数はメソッドオブジェクトなので、呼び出して使う。
	//! 			使い終わったらCleanupLazyBlock() メソッドを呼ぶこと。
	void * CreateLazyBlock(risse_size pos, const tRisseString & basename,
		bool sharevars, tRisseSSAVariableAccessMap * accessmap,
		tRisseSSAForm *& new_form, tRisseSSAVariable *& block_var);

	//! @brief		遅延評価ブロックで使用されている変数を親から子へコピーする
	//! @param		pos			スクリプト上の位置
	//! @param		accessmap	アクセスマップ ( CreateAccessMap() で作成された物を指定)
	void ListVariablesForLazyBlock(risse_size pos, tRisseSSAVariableAccessMap * accessmap);

	//! @brief		遅延評価ブロックのクリーンアップ処理を行う
	//! @param		param	CreateLazyBlock() の戻り値
	void CleanupLazyBlock(void * param);

	//! @brief		アクセスマップのクリーンアップ処理を行う
	//! @param		pos			スクリプト上の位置
	//! @param		accessmap	アクセスマップ ( CreateAccessMap() で作成された物を指定)
	void CleanupAccessMap(risse_size pos, tRisseSSAVariableAccessMap * accessmap);

	//! @brief		バインディングマップを追加する
	//! @param		pos			スクリプト上の位置
	//! @param		binding		バインディングオブジェクトを表すSSA形式変数
	//! @note		このメソッドを実行すると、現時点で可視な変数がすべて共有変数に
	//!				なる。
	void AddBindingMap(risse_size pos, tRisseSSAVariable * binding);

	//! @brief		ユニークな番号を得る
	//! @return		ユニークな番号
	risse_int GetUniqueNumber();

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;

//----
public:
	//! @brief		実行ブロックの最後の return を生成する
	//! @param		root		実行ブロックを表すASTノード
	void GenerateLastReturn(const tRisseASTNode * root);

	//! @brief		到達しない基本ブロックを削除する
	void LeapDeadBlocks();

	//! @brief		共有された変数へのアクセスを別形式の文に変換
	void ConvertSharedVariableAccess();

	//! @brief		変数の生存区間を基本ブロック単位で解析する(すべての変数に対して)
	void AnalyzeVariableBlockLiveness();

	//! @brief		変数の生存区間を基本ブロック単位で解析する(個別の変数に対して)
	//! @param		var		変数
	void AnalyzeVariableBlockLiveness(tRisseSSAVariable * var);

	//! @brief		φ関数を削除する
	//! @note		SSA形式->通常形式の変換過程においてφ関数を削除する処理がこれ
	void RemovePhiStatements();

	//! @brief		変数の生存区間を文単位で解析する
	void AnalyzeVariableStatementLiveness();

	//! @brief		バイトコードジェネレータのインスタンスを生成する
	void EnsureCodeGenerator();

	//! @brief		バイトコードジェネレータを得る
	//! @return		バイトコードジェネレータ
	tRisseCodeGenerator * GetCodeGenerator() const { return CodeGenerator; }

	//! @brief		コードブロックを得る
	//! @return		コードブロック
	tRisseCodeBlock * GetCodeBlock() const { return CodeBlock; }

	//! @brief		コードブロックのスクリプトブロック内でのインデックスを得る
	//! @return		コードブロックのスクリプトブロック内でのインデックス
	risse_size GetCodeBlockIndex() const { return CodeBlockIndex; }

	//! @brief		バイトコードを生成する
	void GenerateCode() const;

	//! @brief		最大の共有変数のネストカウントを設定する
	//! @note		トップレベルの関数は、共有変数のネストカウントに応じたバッファを
	//!				確保しなければならないため、ネスト数が最大でどれほどまでに行くの
	//!				かを知っていなくてはならない。このメソッドはネストレベルが0の
	//!				関数(つまり、関数グループのうち一番最初に実行される関数)の先頭のSSA形式
	//!				にのみ有効である。このメソッドはコードジェネレータに
	//!				共有変数のネストレベルを問い合わせ、それをコードブロックに設定する。
	void SetSharedVariableNestCount();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
#endif

