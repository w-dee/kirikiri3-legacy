//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "../risseString.h"
#include "../risseVariant.h"


//---------------------------------------------------------------------------
namespace Risse
{
class tSSAStatement;
class tSSAForm;
//---------------------------------------------------------------------------
//! @brief	SSA形式における「変数」
//---------------------------------------------------------------------------
class tSSAVariable : public tCollectee
{
	tSSAForm * Form; //!< この変数が属している SSA 形式インスタンス
	tString Name; //!< 変数名(バージョンなし, 番号なし)
	tString NumberedName; //!< 変数名(バージョンなし、番号つき)
	risse_int Version; //!< 変数のバージョン
	tSSAStatement * Declared; //!< この変数が定義された文
	gc_vector<tSSAStatement*> Used; //!< この変数が使用されている文のリスト

	gc_vector<tSSAVariable*> * CoalescableList; //!< 合併可能な場合はそのリストを指す

	typedef gc_map<const tSSAVariable *, risse_size> tInterferenceEdgeMap; //!< 生存している変数のリスト
	tInterferenceEdgeMap * InterferenceEdgeMap; //!< 干渉グラフのエッジを表すマップ

	tVariant Value; //!< この変数がとりうる値/型
public:
	//! @brief	Valueの状態を表す列挙型
	enum tValueState
	{
		vsUnknown, //!< 値は設定されていない
		vsConstant, //!< 値は設定され、Value の通りである
		vsTypeConstant, //!< 値は複数の状態をとりうるが、型は決まっている(Valueの型がそれを表す)
		vsVarying //!< 値も型も複数の状態を取りうる
	};
private:
	tValueState ValueState; //!< Valueの状態
	void * Mark; //!< マーク
	risse_size AssignedRegister; //!< 割り当てられたレジスタ

public:
	static tSSAVariable * GetUninitialized()
		{ return reinterpret_cast<tSSAVariable *>(1); }
		//!< 定義されてはいるが、値が代入されていないということを表すための特殊なインスタンスの値を返す。
		//!< 1 というアドレスを new などが返すわけがないのでこの値は十分に他の値と区別できる

public:
	//! @brief		コンストラクタ
	//! @param		form	この変数が属する SSA 形式インスタンス
	//! @param		stmt	この変数が定義された文
	//! @param		name	変数名
	//!						一時変数については空文字列を渡すこと
	tSSAVariable(tSSAForm * form, tSSAStatement *stmt = NULL,
						const tString & name = tString::GetEmptyString());

	//! @brief		この変数が属している SSA 形式インスタンスを取得する
	//! @return		この変数が属している SSA 形式インスタンス
	tSSAForm * GetForm() const { return Form; }

	//! @brief		この変数の名前を設定する
	//! @param		name		変数名
	//! @note		変数名を設定するたびに Version はユニークな値になる
	void SetName(const tString & name);

	//! @brief		この変数の名前を返す
	//! @return		変数の名前
	const tString & GetName() const { return Name; }

	//! @brief		この変数の番号付きの名前を設定する
	//! @param		n_name		この変数の番号付きの名前
	void SetNumberedName(const tString & n_name) { NumberedName = n_name; }

	//! @brief		この変数の番号付きの名前を返す
	//! @return		この変数の番号付きの名前
	const tString & GetNumberedName() const { return NumberedName; }

	//! @brief		この変数の修飾名を返す
	//! @return		変数の修飾名
	tString GetQualifiedName() const;

	//! @brief		この変数が定義された文を設定する
	//! @param		declared	この変数が定義された文
	void SetDeclared(tSSAStatement * declared) { Declared = declared; }

	//! @brief		この変数が定義された文を取得する
	//! @return		この変数が定義された文
	tSSAStatement * GetDeclared() const { return Declared; }

	//! @brief		この変数が使用されている文を登録する
	//! @param		stmt	この変数が使用されている文
	void AddUsed(tSSAStatement * stmt)
	{
		Used.push_back(stmt);
	}

	//! @brief		この変数が使用されている文を削除する
	//! @param		stmt	この変数が使用されている文
	void DeleteUsed(tSSAStatement * stmt);

	//! @brief		この変数で使用されている文のリストを得る
	const gc_vector<tSSAStatement *> & GetUsed() const { return Used; }

	//! @brief		変数の干渉を設定する
	//! @param		with		干渉する他の変数 (this != with で無ければならない)
	void SetInterferenceWith(tSSAVariable * with);

	//! @brief		変数の干渉を設定する
	//! @param		with		干渉する他の変数 (this != with で無ければならない)
	//! @note		SetInterferenceWith と違い、withだけでなく、withが干渉している
	//!				他の変数すべてに対しても干渉する
	void SetInterferenceWithAll(tSSAVariable * with);

	//! @brief		合併可能な変数のリストがまだ作成されていないようならば作成する
	//! @note		新規作成の際は、自分自身をリストの先頭に追加する
	void EnsureCoalescableList();

	//! @brief		合併可能な変数リスト同士を合併する
	//! @param		with		合併する変数
	void CoalesceCoalescableList(tSSAVariable * with);

	//! @brief		変数の干渉があるかどうかをチェックする
	//! @param		with		合併したい変数(コピー元)
	//! @return		with の生存範囲内に this の定義がある場合は干渉があると見なす
	bool CheckInterferenceWith(tSSAVariable * with);

	//! @brief		合併を行う
	void Coalesce();

	//! @brief		合併可能な変数のリストを得る
	gc_vector<tSSAVariable*> * GetCoalescableList() { return CoalescableList; }

	//! @brief		変数にレジスタを割り当てる
	//! @param		assign_work		割り当て作業用のワーク
	void AssignRegister(gc_vector<void*> & assign_work);

	//! @brief		割り当たっているレジスタを得る
	//! @return		割り当たっているレジスタ (risse_size_max の場合はまだレジスタが割り当たっていない)
	risse_size GetAssignedRegister() const { return AssignedRegister; }

	//! @brief		この変数がとりうる値を設定する
	//! @param		value		この変数がとりうる値
	void SetValue(const tVariant value) { Value = value; }

	//! @brief		この変数がとりうる値を設定する
	//! @param		value		この変数がとりうる値
	//! @note		SetValue と異なり、ValueState > vsConstant な場合は値を設定しない。
	//!				また、値を設定した場合は ValueState を vsConstant に設定する。
	void SuggestValue(const tVariant value)
	{
		if(ValueState <= vsConstant)
		{
			Value = value;
			ValueState = vsConstant;
		}
	}

	//! @brief		この変数がとりうる型を設定する
	//! @param		type		この変数がとりうる型
	//! @note		ステートが降りるような動作はしない。
	//!				また、値を設定した場合は ValueState を vsTypeConstant に設定する。
	void SuggestValue(tVariant::tType type);

	//! @brief		この変数がとりうる値を取得する
	//! @return		この変数がとりうる値
	const tVariant GetValue() const { return Value; }

	//! @brief		この変数がとりうる値の状態を設定する
	//! @param		state		この変数がとりうる値の状態
	void SetValueState(tValueState state) { ValueState = state; }

	//! @brief		この変数がとりうる値の状態をあげる
	//! @param		state		この変数がとりうる値の状態
	//! @note		状態が state 未満だった場合にのみ値が設定される
	void RaiseValueState(tValueState state) { if(ValueState < state) ValueState = state; }

	//! @brief		この変数がとりうる値の状態を取得する
	//! @return		この変数がとりうる値の状態
	tValueState GetValueState() const { return ValueState; }

	//! @brief		この変数がとりうる値の型を tVariant::tGuessType として返す
	//! @return		tVariant::tGuessType タイプ
	//! @note		ValueState が vsUnknown の状態では gtAny が返る
	tVariant::tGuessType GetGuessType() const;

	//! @brief		この変数のメソッド(固定名)を呼び出すSSA形式を生成する
	//! @param		pos		ソースコード上の位置
	//! @param		name	メソッド名
	//! @param		param1	引数1
	//! @param		param2	引数2
	//! @param		param3	引数3
	//! @return		関数の戻り値
	tSSAVariable * GenerateFuncCall(risse_size pos, const tString & name,
				tSSAVariable * param1 = NULL,
				tSSAVariable * param2 = NULL,
				tSSAVariable * param3 = NULL);

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

	//! @brief		この変数が使用されている文のうち、死んでいるブロックに属する文を
	//!				削除する
	void DeleteDeadStatements();

	//! @brief		この変数が使用された文が与えられるので、
	//!				FirstUsedStatement と LastUsedStatement を設定する
	//! @param		stmt	この変数が使用/宣言された文
	//! @note		このメソッドを呼ぶ前に、tSSABlock::SetOrder() で文に
	//!				通し番号を設定すること
	void AnalyzeVariableStatementLiveness(tSSAStatement * stmt);

	//! @brief		ダンプを行う
	//! @return		ダンプ文字列
	tString Dump() const;

	//! @brief		この変数の型や定数に関するコメントを得る
	tString GetTypeComment() const;

	//! @brief		各文のダンプの後ろにつけられるようなコメントを生成して返す
	tString GetComment() const;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse
#endif

