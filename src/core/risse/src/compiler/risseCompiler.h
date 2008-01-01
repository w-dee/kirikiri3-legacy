//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンパイラ
//---------------------------------------------------------------------------
#ifndef risseCompilerH
#define risseCompilerH

#include "../risseGC.h"
#include "../risseCharUtils.h"
#include "../risseTypes.h"
#include "../risseVariant.h"
#include "../risseOpCodes.h"

//---------------------------------------------------------------------------
/*
	risseでは、ASTに対して、一度 SSA 形式ライクな中間形式に変換を行ってから、
	それを VM バイトコードに変換を行う。
*/
//---------------------------------------------------------------------------
namespace Risse
{
class tASTNode;
class tSSAForm;
class tCompilerFunctionGroup;
class tSSAVariable;
class tSSABlock;
class tBindingInfo;
//---------------------------------------------------------------------------
//! @brief		関数クラス
//---------------------------------------------------------------------------
class tCompilerFunction : public tCollectee
{
	tString Name; //!< 関数の名前(表示用)
	tCompilerFunction * Parent; //!< 親関数
	tCompilerFunctionGroup * FunctionGroup; //!< この関数を保持している関数グループクラスのインスタンス
	gc_vector<tSSAForm *> SSAForms; //!< この関数が保持しているSSA形式のリスト
	risse_size NestLevel; //!< 関数のネストレベル
	typedef gc_map<tString, tSSAVariable *> tSharedVariableMap;
		//!< 子関数により共有されている変数のマップのtypedef (tSharedVariableMap::value_type::second は常に null)
	tSharedVariableMap SharedVariableMap; //!< 子関数(あるいはbinding)により共有されている変数のマップ

public:
	typedef gc_map<tString, tSSABlock *> tLabelMap;
		//!< ラベルのマップのtypedef

	//! @brief		バインドがまだされていないラベルへのジャンプ
	struct tPendingLabelJump : public tCollectee
	{
		tSSABlock * SourceBlock; //!< ジャンプもとの基本ブロック
		tString LabelName; //!< ラベル名
		tPendingLabelJump(
			tSSABlock * source_block,
			const tString & labelname)
		{
			SourceBlock = source_block;
			LabelName = labelname;
		}
	};

	typedef gc_vector<tPendingLabelJump> tPendingLabelJumps;
		//!< バインドがまだされていないラベルへのジャンプのリストのtypedef

private:
	tLabelMap LabelMap; //!< ラベルのマップ
	tPendingLabelJumps PendingLabelJumps; //!< バインドがまだされていないラベルへのジャンプのリスト


public:
	//! @brief		コンストラクタ
	//! @param		function_group		関数グループクラスのインスタンス
	//! @param		parent				親関数インスタンス
	//! @param		nestlevel			ネストレベル(risse_size_max = 親関数インスタンス+1にする)
	//! @param		name				関数名(表示用)
	tCompilerFunction(tCompilerFunctionGroup * function_group,
		tCompilerFunction * parent, risse_size nestlevel, const tString name);

	//! @brief		関数グループクラスのインスタンスを得る
	//! @return		関数グループクラスのインスタンス
	tCompilerFunctionGroup * GetFunctionGroup() const { return FunctionGroup; }

	//! @brief		ラベルのマップを得る
	//! @return		ラベルのマップ
	tLabelMap & GetLabelMap() { return LabelMap; }

	//! @brief		バインドがまだされていないラベルへのジャンプのリストを得る
	//! @return		バインドがまだされていないラベルへのジャンプのリスト
	tPendingLabelJumps & GetPendingLabelJumps() { return PendingLabelJumps; }

	//! @brief		SSA形式インスタンスを追加する
	//! @param		form		SSA形式インスタンス
	void AddSSAForm(tSSAForm * form);

	//! @brief		先頭のSSA形式インスタンスを得る
	tSSAForm * GetTopSSAForm() const { return SSAForms.front(); }

	//! @brief		関数のネストレベルを得る @return 関数のネストレベル
	risse_size GetNestLevel() const { return NestLevel; }

	//! @brief		SSA形式を完結させる
	//! @note		このメソッドは、一通りASTからの変換が終わった後に呼ばれる。
	//!				未バインドのラベルなどのバインドを行う。
	void CompleteSSAForm();

	//! @brief		共有変数をコードジェネレータに登録する
	void RegisterSharedVariablesToCodeGenerator();

	//! @brief		VMコード生成を行う
	void GenerateVMCode();

	//! @brief		最大の共有変数のネストカウントを設定する。
	//! @note		トップレベルの関数は、共有変数のネストカウントに応じたバッファを
	//!				確保しなければならないため、ネスト数が最大でどれほどまでに行くの
	//!				かを知っていなくてはならない。関数グループのうち一番最初に
	//!				実行される関数にのみ有効である。このメソッドはコードジェネレータに
	//!				共有変数のネストレベルを問い合わせ、それをSSA形式インスタンスに設定する。
	void SetSharedVariableNestCount();

//--
public:
	//! @brief		未バインドのラベルジャンプを追加する
	//! @param		jump_block		ジャンプもとの基本ブロック
	//! @param		labelname		ジャンプ先のラベル名
	void AddPendingLabelJump(tSSABlock * jump_block,
			const tString & labelname);

	//! @brief		ラベルマップを追加する
	//! @param		labelname		ラベル名
	//! @param		block			基本ブロック
	//! @note		すでに同じ名前のラベルが存在していた場合は例外が発生する
	void AddLabelMap(const tString &labelname, tSSABlock * block);

	//! @param		変数を共有する
	//! @param		name		変数名(番号付き)
	void ShareVariable(const tString & name);

	//! @param		変数が共有されているかを得る
	//! @param		name		変数名(番号付き)
	//! @return		変数が共有されているかどうか
	bool GetShared(const tString & name);

	//! @param		この関数のネストレベルが共有変数を持っているかどうかを返す
	//! @return		この関数のネストレベルが共有変数を持っているかどうか
	bool HasSharedVariable() const { return SharedVariableMap.size() > 0; }

private:
	//! @brief		未バインドのラベルジャンプをすべて解決する
	void BindAllLabels();


};
//---------------------------------------------------------------------------


class tCompiler;
class tCompilerFunction;
//---------------------------------------------------------------------------
//! @brief		関数グループクラス
//---------------------------------------------------------------------------
class tCompilerFunctionGroup : public tCollectee
{
	tCompiler * Compiler; //!< この関数グループを保持しているコンパイラクラスのインスタンス
	gc_vector<tCompilerFunction *> Functions; //!< この関数グループが保持している関数インスタンスのリスト
	tString Name; //!< 関数グループの名前(たいていの場合、クラス名+ユニーク番号)
	tString ClassName; //!< この関数グループがクラスかモジュールの場合、そのクラスかモジュール名 (それ以外の場合は空文字列)

public:
	//! @brief		コンストラクタ
	//! @param		compiler		コンパイラクラスのインスタンス
	//! @param		name			関数グループの名前
	tCompilerFunctionGroup(tCompiler * compiler, const tString & name);

	//! @brief		コンパイラクラスのインスタンスを得る
	//! @param		コンパイラクラスのインスタンス
	tCompiler * GetCompiler() const { return Compiler; }

	//! @brief		クラス/モジュール名を設定する
	//! @param		class_name		クラス/モジュール名
	void SetClassName(const tString class_name) { ClassName = class_name; }

	//! @brief		クラス/モジュール名を得る
	//! @return		クラス/モジュール名
	const tString & GetClassName() const { return ClassName; }

	//! @brief		関数インスタンスを追加する
	//! @param		function		関数インスタンス
	void AddFunction(tCompilerFunction * function);

	//! @brief		SSA形式を完結させる
	//! @note		このメソッドは、一通りASTからの変換が終わった後に呼ばれる。
	//!				未バインドのラベルなどのバインドを行う。
	void CompleteSSAForm();

	//! @brief		VMコード生成を行う
	void GenerateVMCode();
};
//---------------------------------------------------------------------------


class tCodeBlock;
class tScriptBlockInstance;
//---------------------------------------------------------------------------
//! @brief		コンパイラクラス
//---------------------------------------------------------------------------
class tCompiler : public tCollectee
{
	tScriptBlockInstance * ScriptBlockInstance; //!< このコンパイラを保有しているスクリプトブロック
	gc_vector<tCompilerFunctionGroup *> FunctionGroups;
		//!< この関数グループが保持している関数グループインスタンスのリスト

	risse_int UniqueNumber; //!< ユニークな番号 (変数などのバージョン付けに用いる)

public:
	//! @brief		コンストラクタ
	//! @param		scriptblock		スクリプトブロックインスタンス
	tCompiler(tScriptBlockInstance * scriptblock)
		{ ScriptBlockInstance = scriptblock; UniqueNumber = 0; }

	//! @brief		スクリプトブロックインスタンスを得る
	//! @return		スクリプトブロックインスタンス
	tScriptBlockInstance * GetScriptBlockInstance() const { return ScriptBlockInstance; }

	//! @brief		ASTを元にコンパイルを行う
	//! @param		root		ルートASTノード
	//! @param		binding		バインディング情報
	//! @param		need_result		評価時に結果が必要かどうか
	//! @param		is_expression	式評価モードかどうか
	void Compile(tASTNode * root, const tBindingInfo & binding,
		bool need_result, bool is_expression);

	//! @brief		ASTを元にクラスのコンパイルを行う
	//! @param		root		クラスのルートASTノード(複数可)
	//! @param		name		クラス名として表示する名前
	//! @param		form		クラスを作成する場所にあるSSA形式インスタンス
	//! @param		new_form	新しく作成された(ルートの)SSA形式インスタンス
	//! @param		block_var	そのクラスのSSA変数を格納する先
	//! @param		reg_super	スーパークラス用の変数を追加するかどうか(moduleの場合は偽にする)
	void CompileClass(const gc_vector<tASTNode *> & roots,
		const tString & name, tSSAForm * form,
		tSSAForm *& new_form, tSSAVariable *& block_var, bool reg_super);

private:
	//! @brief		(内部関数)トップレベルのSSA形式を作成する
	//! @param		pos			ソースコード上の位置
	//! @param		name		名前
	//! @param		binding		バインディング情報(NULLの場合=バインディングが無い場合)
	//! @param		need_result		評価時に結果が必要かどうか
	//! @param		is_expression	式評価モードかどうか
	//! @return		トップレベルのSSA形式インスタンス
	tSSAForm * CreateTopLevelSSAForm(risse_size pos, const tString & name,
		const tBindingInfo * binding,
		bool need_result, bool is_expression);

public:
	//! @brief		関数グループインスタンスを追加する
	//! @param		function_group		関数グループインスタンス
	void AddFunctionGroup(tCompilerFunctionGroup * function_group);

	//! @brief		コードブロックを追加する
	//! @param		block		コードブロック
	//! @return		コードブロックのインデックス
	risse_size AddCodeBlock(tCodeBlock * block);


public:
	//! @brief		ユニークな番号を得る
	//! @return		ユニークな番号(0と負の値は絶対に帰らない)
	//! @note		30bit程度を使い果たすと例外が発生する
	risse_int GetUniqueNumber();

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

