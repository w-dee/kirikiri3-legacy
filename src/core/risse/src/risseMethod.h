//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief メソッドに関する処理
//---------------------------------------------------------------------------
#ifndef risseMethodH
#define risseMethodH

#include "risseTypes.h"
#include "risseAssert.h"


namespace Risse
{
class tRisseVariantBlock;
typedef tRisseVariantBlock tRisseVariant;
class tRisseMethodArgument;
//---------------------------------------------------------------------------
//! @brief		メソッドへ渡す引数を表すクラス(可変引数用テンプレートクラス)
//---------------------------------------------------------------------------
template <risse_size N>
class tRisseMethodArgumentOf
{
public:
	risse_size argc; //!< 配列の個数
	const tRisseVariant * argv[N<1?1:N]; //!< 引数を表す値へのポインタの配列

public:
	//! @brief		コンストラクタ
	tRisseMethodArgumentOf()
	{
		argc = N;
		// argv はここでは初期化しないので利用する側で代入して使うこと
		// (あまり設計が良くない)
	}


public:
	//! @brief		tRisseMethodArgumentへのキャスト
	//! @return		tRisseMethodArgumentへの参照
	//! @note		バイナリレイアウトが同一なのでtRisseMethodArgumentへは安全に
	//!				キャストできるはず
	operator const tRisseMethodArgument & () const
		{ return * reinterpret_cast<const tRisseMethodArgument *>(this); }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		メソッドへ渡す引数を表すクラス
//---------------------------------------------------------------------------
class tRisseMethodArgument
{
public:
	risse_size argc; //!< 引数の個数
	const tRisseVariant * argv[1]; //!< 引数を表す値へのポインタの配列

private:
	static tRisseMethodArgument EmptyArgument; //!< 引数0を表すstaticオブジェクト

public:
	//! @brief		引数0の引数を表すstaticオブジェクトへの参照を返す
	//! @return		引数0の引数を表すstaticオブジェクトへの参照
	static const tRisseMethodArgument & GetEmptyArgument() { return EmptyArgument; }


public:
	//! @brief		N個の引数分のストレージを持つこのクラスのインスタンスを動的に作成して返す
	//! @param		N		引数の数
	//! @return		動的に確保されたこのクラスのインスタンス
	static tRisseMethodArgument & Allocate(risse_size n);

public:

	//! @brief		引数=0のtRisseMethodArgumentOfオブジェクトを返す
	static const tRisseMethodArgument New()
	{
		return GetEmptyArgument();
	}

	//! @brief		引数=1のtRisseMethodArgumentOfオブジェクトを返す
	static const tRisseMethodArgumentOf<1> New(const tRisseVariant *a0)
	{
		tRisseMethodArgumentOf<1> arg;
		arg.argv[0] = a0;
		return arg;
	}

	//! @brief		引数=2のtRisseMethodArgumentOfオブジェクトを返す
	static const tRisseMethodArgumentOf<2> New(const tRisseVariant *a0,
								const tRisseVariant *a1)
	{
		tRisseMethodArgumentOf<2> arg;
		arg.argv[0] = a0;
		arg.argv[1] = a1;
		return arg;
	}

	//! @brief		引数=3のtRisseMethodArgumentOfオブジェクトを返す
	static const tRisseMethodArgumentOf<3> New(const tRisseVariant *a0,
								const tRisseVariant *a1, const tRisseVariant *a2)
	{
		tRisseMethodArgumentOf<3> arg;
		arg.argv[0] = a0;
		arg.argv[1] = a1;
		arg.argv[2] = a2;
		return arg;
	}


};
//---------------------------------------------------------------------------


}


#endif

