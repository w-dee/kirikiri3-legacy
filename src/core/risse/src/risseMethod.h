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
};
//---------------------------------------------------------------------------


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
	//! @brief		引数=0のコンストラクタ
	tRisseMethodArgumentOf()
	{
		argc = 0;
		RISSE_ASSERT(N == 0);
	}

	//! @brief		引数=1のコンストラクタ
	tRisseMethodArgumentOf(const tRisseVariant *a0)
	{
		RISSE_ASSERT(N == 1);
		argc = 1;
		argv[0] = a0;
	}

	//! @brief		引数=2のコンストラクタ
	tRisseMethodArgumentOf(const tRisseVariant *a0, const tRisseVariant *a1)
	{
		RISSE_ASSERT(N == 2);
		argc = 2;
		argv[0] = a0;
		argv[1] = a1;
	}

	//! @brief		引数=3のコンストラクタ
	tRisseMethodArgumentOf(const tRisseVariant *a0, const tRisseVariant *a1,
						const tRisseVariant *a2)
	{
		RISSE_ASSERT(N == 3);
		argc = 3;
		argv[0] = a0;
		argv[1] = a1;
		argv[2] = a2;
	}

public:
	//! @brief		tRisseMethodArgumentへのキャスト
	//! @return		tRisseMethodArgumentへの参照
	//! @note		バイナリレイアウトが同一なのでtRisseMethodArgumentへは安全に
	//!				キャストできるはず
	operator const tRisseMethodArgument & () const
		{ return * reinterpret_cast<tRisseMethodArgument *>(this); }
};
//---------------------------------------------------------------------------




}


#endif

