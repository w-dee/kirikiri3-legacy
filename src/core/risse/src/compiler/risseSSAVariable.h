//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief SSA形式における「変数」
//---------------------------------------------------------------------------
#ifndef risseSSAVariableH
#define risseSSAVariableH

#include "../risseGC.h"
#include "../risseCharUtils.h"
#include "../risseTypes.h"
#include "../risseCxxString.h"
#include "../risseVariant.h"


//---------------------------------------------------------------------------
namespace Risse
{
class tRisseSSAStatement;
class tRisseSSAForm;
//---------------------------------------------------------------------------
//! @brief	SSA形式における「変数」
//---------------------------------------------------------------------------
class tRisseSSAVariable : public tRisseCollectee
{
	tRisseSSAForm * Form; //!< この変数が属している SSA 形式インスタンス
	tRisseString Name; //!< 変数名(バージョンなし, 番号なし)
	tRisseString NumberedName; //!< 変数名(バージョンなし、番号つき)
	risse_int Version; //!< 変数のバージョン
	tRisseSSAStatement * Declared; //!< この変数が定義された文
	gc_vector<tRisseSSAStatement*> Used; //!< この変数が使用されている文のリスト

	tRisseSSAStatement * FirstUsedStatement; //!< 文の通し番号順で最初にこの変数が使用された文
	tRisseSSAStatement * LastUsedStatement; //!< 文の通し番号順で最後にこの変数が使用された文

	const tRisseVariant *Value; //!< この変数がとりうる値(NULL=決まった値がない)
	tRisseVariant::tType ValueType; //!< この変数がとりうる型(void = どんな型でも取りうる)
	void * Mark; //!< マーク

public:
	//! @brief		コンストラクタ
	//! @param		form	この変数が属する SSA 形式インスタンス
	//! @param		stmt	この変数が定義された文
	//! @param		name	変数名
	//!						一時変数については空文字列を渡すこと
	tRisseSSAVariable(tRisseSSAForm * form, tRisseSSAStatement *stmt = NULL,
						const tRisseString & name = tRisseString::GetEmptyString());

	//! @brief		この変数が属している SSA 形式インスタンスを取得する
	//! @return		この変数が属している SSA 形式インスタンス
	tRisseSSAForm * GetForm() const { return Form; }

	//! @brief		この変数の名前を設定する
	//! @param		name		変数名
	//! @note		変数名を設定するたびに Version はユニークな値になる
	void SetName(const tRisseString & name);

	//! @brief		この変数の名前を返す
	//! @return		変数の名前
	const tRisseString & GetName() const { return Name; }

	//! @brief		この変数の番号付きの名前を設定する
	//! @param		n_name		この変数の番号付きの名前
	void SetNumberedName(const tRisseString & n_name) { NumberedName = n_name; }

	//! @brief		この変数の番号付きの名前を返す
	//! @return		この変数の番号付きの名前
	const tRisseString & GetNumberedName() const { return NumberedName; }

	//! @brief		この変数の修飾名を返す
	//! @return		変数の修飾名
	tRisseString GetQualifiedName() const;

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

	//! @brief		この変数が使用されている文を削除する
	//! @param		stmt	この変数が使用されている文
	void DeleteUsed(tRisseSSAStatement * stmt);

	//! @brief		この変数で使用されている文のリストを得る
	const gc_vector<tRisseSSAStatement *> & GetUsed() const { return Used; }

	//! @brief		文の通し番号順で最初にこの変数が使用された文を得る
	//! @return		文の通し番号順で最初にこの変数が使用された文
	tRisseSSAStatement * GetFirstUsedStatement() const { return FirstUsedStatement; }

	//! @brief		文の通し番号順で最後にこの変数が使用された文を得る
	//! @return		文の通し番号順で最後にこの変数が使用された文
	tRisseSSAStatement * GetLastUsedStatement() const { return LastUsedStatement; }

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

	//! @brief		この変数のメソッド(固定名)を呼び出すSSA形式を生成する
	//! @param		pos		ソースコード上の位置
	//! @param		name	メソッド名
	//! @param		param1	引数1
	//! @param		param2	引数2
	//! @param		param3	引数3
	//! @return		関数の戻り値
	tRisseSSAVariable * GenerateFuncCall(risse_size pos, const tRisseString & name,
				tRisseSSAVariable * param1 = NULL,
				tRisseSSAVariable * param2 = NULL,
				tRisseSSAVariable * param3 = NULL);

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

	//! @brief		この変数が使用された文が与えられるので、
	//!				FirstUsedStatement と LastUsedStatement を設定する
	//! @param		stmt	この変数が使用/宣言された文
	//! @note		このメソッドを呼ぶ前に、tRisseSSABlock::SetOrder() で文に
	//!				通し番号を設定すること
	void AnalyzeVariableStatementLiveness(tRisseSSAStatement * stmt);

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tRisseString Dump() const;

	//! @brief		この変数の型や定数に関するコメントを得る
	tRisseString GetTypeComment() const;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse
#endif

