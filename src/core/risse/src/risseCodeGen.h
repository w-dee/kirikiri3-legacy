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
class tRisseSSAVariable;
//---------------------------------------------------------------------------
//! @brief	ローカル変数用の名前空間管理クラス
//---------------------------------------------------------------------------
class tRisseLocalNamespace : public tRisseCollectee
{
	typedef gc_map<tRisseString, tRisseSSAVariable *> tMap; //!< マップの typedef
	typedef gc_vector<tMap *> tScopes; //!< スコープの typedefs
	tScopes Scopes; //!< 名前空間のスコープ

public:
	//! @brief		コンストラクタ
	tRisseLocalNamespace();

	//! @brief		名前空間を push する
	void Push();

	//! @brief		名前空間を pop する
	void Pop();

	//! @brief		変数を定義・更新する
	//! @param		name		変数名
	//! @param		where		その変数を表す SSA 変数表現
	void Add(const tRisseString & name, tRisseSSAVariable * where);

	//! @brief		変数を探す
	//! @param		name		変数名
	//! @return		その変数を表す SSA 変数表現 (NULL = みつからない)
	tRisseSSAVariable * Find(const tRisseString & name) const;

	//! @brief		変数を削除する
	//! @param		name		変数名
	//! @return		変数の削除に成功したかどうか
	bool Delete(const tRisseString & name);
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
	tRisseString Name; //!< 変数名(バージョン付き)
	tRisseString OriginalName; //!< 変数名 (元の名前)
	tRisseSSAStatement * Declared; //!< この変数が定義された文
	gc_vector<tRisseSSAStatement*> Used; //!< この変数が使用されている文のリスト

	const tRisseVariant *Value; //!< この変数がとりうる値(NULL=決まった値がない)
	tRisseVariant::tType ValueType; //!< この変数がとりうる型(void = どんな型でも取りうる)

public:
	//! @brief		コンストラクタ
	//! @param		form	この変数が属する SSA 形式インスタンス
	//! @param		stmt	この変数が定義された文
	//! @param		name	変数名 (実際にはこれがprefixになり、通し番号が後に続のがNameに入る)
	//						一時変数については空文字列を渡すこと
	tRisseSSAVariable(tRisseSSAForm * form, tRisseSSAStatement *stmt = NULL,
						const tRisseString & name = tRisseString());

	//! @brief		この変数が属している SSA 形式インスタンスを取得する
	//! @return		この変数が属している SSA 形式インスタンス
	tRisseSSAForm * GetForm() const { return Form; }

	//! @brief		この変数の名前を返す
	//! @return		変数の名前 (バージョン付き)
	const tRisseString GetName() const { return Name; }

	//! @brief		この変数のオリジナルの名前を返す
	//! @return		変数のオリジナルの名前
	const tRisseString GetOriginalName() const { return OriginalName; }

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
	tRisseSSABlock * Block; //!< この文が含まれているブロック
	tRisseSSAStatement * Pred; //!< 直前の文
	tRisseSSAStatement * Succ; //!< 直後の文

	tRisseSSAVariable * Declared; //!< この文で定義された変数
	gc_vector<tRisseSSAVariable*> Used; //!< この文で使用されている変数のリスト

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

	//! @brief		この文が含まれているブロックを設定する
	//! @param		block この文が含まれているブロック
	void SetBlock(tRisseSSABlock * block) { Block = block; }

	//! @brief		この文が含まれているブロックを取得する
	//! @return		この文が含まれているブロック
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
	void AddUsed(tRisseSSAVariable * var)
	{
		var->AddUsed(this);
		Used.push_back(var);
	}

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	SSA形式における「ブロック」
//---------------------------------------------------------------------------
class tRisseSSABlock : public tRisseCollectee
{
	tRisseSSAForm * Form; //!< このブロックを保持する SSA 形式インスタンス
	gc_vector<tRisseSSABlock *> Pred; //!< 直前のブロックのリスト
	gc_vector<tRisseSSABlock *> Succ; //!< 直後のブロックのリスト
	tRisseSSAStatement * FirstStatement; //!< 文のリストの先頭
	tRisseSSAStatement * LastStatement; //!< 文のリストの最後

public:
	//! @brief		コンストラクタ
	//! @param		form		このブロックを保持する SSA 形式インスタンス
	tRisseSSABlock(tRisseSSAForm * form)
	{
		Form = form;
		FirstStatement = LastStatement = NULL;
	}

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
	}

	//! @brief		定数値を得る文を追加する
	//! @param		pos		スクリプト上の位置
	//! @param		val		定数
	//! @return		定数値を表す一時変数
	//! @note		このメソッドは、定数値を一時変数に代入する
	//!				文を生成し、その一時変数を返す
	tRisseSSAVariable * AddConstantValueStatement(risse_size pos, const tRisseVariant & val);

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	SSA形式を表すクラス
//---------------------------------------------------------------------------
class tRisseSSAForm : public tRisseCollectee
{
	tRisseScriptBlockBase * ScriptBlock; //!< この SSA 形式が含まれるスクリプトブロック
	tRisseASTNode * Root; //!< このコードジェネレータが生成すべきコードのASTルートノード
	risse_int UniqueNumber; //!< ユニークな番号 (変数のバージョン付けに用いる)
	tRisseLocalNamespace * LocalNamespace; //!< ローカル名前空間
	tRisseSSABlock * EntryBlock; //!< エントリーSSAブロック
	tRisseSSABlock * CurrentBlock; //!< 現在変換中のブロック

public:
	//! @brief		コンストラクタ
	//! @param		scriptblock		この SSA 形式が含まれるスクリプトブロック
	//! @param		root			ASTのルートノード
	tRisseSSAForm(tRisseScriptBlockBase * scriptblock, tRisseASTNode * root);

	//! @brief		AST を SSA 形式に変換する
	void Generate();

	//! @brief		ローカル名前空間を得る
	tRisseLocalNamespace * GetLocalNamespace() const { return LocalNamespace; }

	//! @brief		現在変換中のブロックを得る
	tRisseSSABlock * GetCurrentBlock() const { return CurrentBlock; }

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
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

