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
#ifndef risseOpeCodesH
#define risseOpeCodesH

#include "risseGC.h"
#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAST.h"
#include "risseVariant.h"

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
	tRisseSSAVariable * const & Find(const tRisseString & name);

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
	tRisseString Name; //!< 変数名
	tRisseSSAStatement * Declared; //!< この変数が定義された文
	gc_vector<tRisseSSAStatement*> Used; //!< この変数が使用されている文のリスト

	tRisseVariant *Value; //!< この変数がとりうる値(NULL=決まった値がない)
	tRisseVariant::tType ValueType; //!< この変数がとりうる型(void = どんな型でも取りうる)

public:
	//! @brief		コンストラクタ
	//! @param		form	この変数が属する SSA 形式インスタンス
	//! @param		name	変数名 (実際にはこれがprefixになり、通し番号が後に続く)
	//						一時変数については空文字列を渡すこと
	tRisseSSAVariable(tRisseSSAForm * form, const tRisseVariant & name = tRisseVariant());

	//! @brief		この変数が属している SSA 形式インスタンスを取得する
	//! @return		この変数が属している SSA 形式インスタンス
	tRisseSSAForm * GetForm() const { return Form; }

	//! @brief		この変数の名前を返す
	//! @return		変数の名前 (バージョン付き)
	const tRisseString GetName() const { return Name; }

	//! @brief		この変数が定義された文を設定する
	//! @param		declared	この変数が定義された文
	void SetDeclared(tRisseSSAForm * declared) { Declared = declared; }

	//! @brief		この変数が定義された文を取得する
	//! @return		この変数が定義された文
	tRisseSSAForm * GetDeclared() const { return Declared; }

	//! @brief		この変数が使用されている文を登録する
	//! @param		stmt	この変数が使用されている文
	void RegisterUsed(tRisseSSAStatement * stmt) { Used.push_back(stmt); }

};
//---------------------------------------------------------------------------


class tRisseSSABlock;
//---------------------------------------------------------------------------
//! @brief	SSA形式における「文」
//---------------------------------------------------------------------------
class tRisseSSAStatement : public tRisseCollectee
{
	risse_size Position; //!< ソースコード上の位置 (コードポイント数オフセット)
	tRisseSSABlock * Block; //!< この文が含まれているブロック
	tRisseSSAStatement * Pred; //!< 直前の文
	tRisseSSAStatement * Succ; //!< 直後の文

	tRisseOpCodeType Type; //!< 文のタイプ
	tRisseSSAVariable * Declared; //!< 定義された変数
	gc_vector<tRisseSSAVariable*> Used; //!< 使用されている変数のリスト
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	SSA形式における「ブロック」
//---------------------------------------------------------------------------
class tRisseSSABlock : public tRisseCollectee
{
	gc_vector<tRisseSSABlock *> Pred; //!< 直前のブロックのリスト
	gc_vector<tRisseSSABlock *> Succ; //!< 直後のブロックのリスト
	tRisseSSAStatement * Statements; //!< 文のリストの先頭

public:
	//! @brief		コンストラクタ
	tRisseSSABlock()
	{
		Statements = NULL;
	}

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	SSA形式を表すクラス
//---------------------------------------------------------------------------
class tRisseSSAForm : public tRisseCollectee
{
	risse_int UniqueNumber; //!< ユニークな番号 (変数のバージョン付けに用いる)
	tRisseASTNode * Root; //!< このコードジェネレータが生成すべきコードのASTルートノード
	tRisseLocalNamespace * LocalNamespace; //!< ローカル名前空間
	tRisseSSABlock * EntryBlock; //!< エントリーSSAブロック
	tRisseSSABlock * CurrentBlock; //!< 現在変換中のブロック

public:
	//! @brief		コンストラクタ
	//! @param		root	ASTのルートノード
	tRisseSSAGenerator(tRisseASTNode * root);

	//! @brief		AST を SSA 形式に変換する
	void Generate();

	//! @brief		ローカル名前空間を得る
	tRisseLocalNamespace * GetLocalNamespace() const { return LocalNamespace; }

	//! @brief		ユニークな番号を得る
	risse_int GetUniqueNumber()
	{
		UniqueNumber++;
		// 2^30 ぐらいで元に戻る
		// もちろんこれはそれほど変数が使われることは無いだろうという推測の元なので
		// 周回が起こったらここで例外を吐いて止まってしまった方がいいかもしれない
		if(UniqueNumber >= 1 << (sizeof(risse_int) * 8 - 2)) UniqueNumber = 0;
		return UniqueNumber;
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

