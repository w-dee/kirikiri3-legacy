//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
//! @brief	ラベルマップを表すクラス
//---------------------------------------------------------------------------
class tRisseSSALabelMap : public tRisseCollectee
{
	// TODO: 親コンテキストのラベルマップの継承
	tRisseSSAForm * Form; //!< このラベルマップを保持する SSA 形式インスタンス

	//! @brief		バインドがまだされていないラベルへのジャンプ
	struct tPendingLabelJump : public tRisseCollectee
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
	gc_vector<tRisseSSAForm *> Children; //!< この SSA形式インスタンスの子インスタンスの配列
	typedef gc_map<tRisseString, tRisseSSAVariable *> tPinnedVariableMap;
		//!< 「ピン」されている変数のマップのtypedef (tPinnedVariableMap::value_type::second は常に null)
	tPinnedVariableMap PinnedVariableMap; //!< 「ピン」されている変数のマップ
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

	tRisseCodeGenerator * CodeGenerator; //!< バイトコードジェネレータのインスタンス
	tRisseCodeBlock * CodeBlock; //!< コードブロック
	risse_size CodeBlockIndex; //!< コードブロックのスクリプトブロック内でのインデックス

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

	//! @param		変数を「ピン」する
	//! @param		name		変数名(番号付き)
	void PinVariable(const tRisseString & name);

	//! @param		変数が「ピン」されているかを得る
	//! @param		name		変数名(番号付き)
	//! @return		ピンされているかどうか
	bool GetPinned(const tRisseString & name);

protected:
	//! @brief	CreateLazyBlock で返される情報の構造体
	struct tLazyBlockParam : public tRisseCollectee
	{
		tRisseSSAForm * NewForm; //!< 新しいSSA形式インスタンス(遅延評価ブロックを表す)
		risse_size Position; //!< スクリプト上の位置
		tRisseSSAVariable *BlockVariable; //!< 遅延評価ブロックを表す変数
		tRisseSSAVariableAccessMap * AccessMap;
	};
public:
	//! @brief		新しい遅延評価ブロックを作成する
	//! @param		node		遅延評価ブロックのルートノード
	//! @param		pinvars		遅延評価ブロックからブロック外の変数を参照された場合に
	//!							その参照された変数をスタックフレームに固定(pin)するかどうか。
	//!							変数を固定することにより、遅延評価ブロックを、そのブロックが定義
	//!							された位置以外から呼び出しても安全に変数にアクセスできるように
	//!							なる ( function 内 function でレキシカルクロージャを使用するとき
	//!							などに有効 )
	//! @param		block_var	その遅延評価ブロックを表すSSA変数を格納する先
	//! @return		CleanupLazyBlock() に渡すべき情報
	//! @note		このメソッドは遅延評価ブロックを作成してその遅延評価ブロックを
	//!				表す変数を返す。この変数はメソッドオブジェクトなので、呼び出して
	//!				使う。使い終わったら CreateLazyBlock() メソッドの戻り値を
	//!				CleanupLazyBlock() メソッドに渡して呼び出すこと。
	void * CreateLazyBlock(tRisseASTNode * node, bool pinvars, tRisseSSAVariable *& block_var);

	//! @brief		遅延評価ブロックのクリーンアップ処理を行う
	//! @param		param	CreateLazyBlock() の戻り値
	void CleanupLazyBlock(void * param);

	//! @brief		ユニークな番号を得る
	//! @return		ユニークな番号
	risse_int GetUniqueNumber();

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;

//----
public:
	//! @brief		実行ブロックの最後の return を生成する
	void GenerateLastReturn();

	//! @brief		到達しない基本ブロックを削除する
	void LeapDeadBlocks();

	//! @brief		ピンの刺さった変数へのアクセスを別形式の文に変換
	void ConvertPinnedVariableAccess();

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

	//! @brief		 コードブロックのスクリプトブロック内でのインデックスを得る
	//! @return		コードブロックのスクリプトブロック内でのインデックス
	risse_size GetCodeBlockIndex() const { return CodeBlockIndex; }

	//! @brief		バイトコードを生成する
	void GenerateCode() const;
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
#endif

