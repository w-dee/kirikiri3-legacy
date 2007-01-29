//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "risseVariantData.h"

namespace Risse
{
class tRisseVariantBlock;
typedef tRisseVariantBlock tRisseVariant;
class tRisseMethodArgument;
//---------------------------------------------------------------------------
//! @brief		メソッドへ渡す引数を表すクラス(可変引数用テンプレートクラス)
//---------------------------------------------------------------------------
template <risse_size AC, risse_size BC = 0>
class tRisseMethodArgumentOf
{
protected:
	risse_size ArgumentCount; //!< 普通の引数の配列の個数
	risse_size BlockArgumentCount; //!< ブロック引数の配列の個数
	const tRisseVariant * Arguments[(AC+BC)<1?1:(AC+BC)]; //!< 普通の引数を表す値へのポインタの配列

public:
	//! @brief		コンストラクタ
	tRisseMethodArgumentOf()
	{
		ArgumentCount = AC;
		BlockArgumentCount = BC;
		// Argumentsはここでは初期化しないので利用する側で代入して使うこと
		// (あまり設計が良くない)
	}


public:
	//! @brief		tRisseMethodArgumentへのキャスト
	//! @return		tRisseMethodArgumentへの参照
	//! @note		バイナリレイアウトが同一なのでtRisseMethodArgumentへは安全に
	//!				キャストできるはず
	operator const tRisseMethodArgument & () const
		{ return * reinterpret_cast<const tRisseMethodArgument *>(this); }

	//! @brief		普通の引数の個数を得る
	//! @return		普通の引数の個数
	risse_size GetArgumentCount() const { return ArgumentCount; }

	//! @brief		ブロック引数の個数を得る
	//! @return		ブロック引数の個数
	risse_size GetBlockArgumentCount() const { return BlockArgumentCount; }

	//! @brief		指定位置にある普通の引数への参照を返す(const版)
	//! @param		n		位置(0～)
	//! @return		引数への参照
	//! @note		n の範囲はチェックしていない
	const tRisseVariant & operator [] (risse_size n) const
	{
		return *Arguments[n];
	}

	//! @brief		指定位置にあるブロック引数への参照を返す(const版)
	//! @param		n		位置(0～)
	//! @return		引数への参照
	//! @note		n の範囲はチェックしていない
	const tRisseVariant & GetBlockArgument (risse_size n) const
	{
		return *Arguments[n];
	}

	//! @brief		普通の引数へ値への参照をセットする
	//! @param		n		パラメータ位置
	//! @param		v		(パラメータの値)
	void SetArgument(risse_size n, const tRisseVariant &v)
	{
		Arguments[n] = &v;
	}

	//! @brief		ブロック引数へ値への参照をセットする
	//! @param		n		パラメータ位置
	//! @param		v		(パラメータの値)
	void SetBlockArgument(risse_size n, const tRisseVariant &v)
	{
		Arguments[n+ArgumentCount] = &v;
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メソッドへ渡す引数を表すクラス
//---------------------------------------------------------------------------
class tRisseMethodArgument
{
	class tEmptyMethodArgument
	{
	public:
		risse_size ArgumentCount; //!< 普通の引数の個数
		risse_size BlockArgumentCount; //!< ブロック引数の個数
		const tRisseVariant * Arguments[1]; //!< 引数を表す値へのポインタの配列
	};
	static tEmptyMethodArgument EmptyArgument;

protected:
	risse_size ArgumentCount; //!< 普通の引数の配列の個数
	risse_size BlockArgumentCount; //!< ブロック引数の配列の個数
	const tRisseVariant * Arguments[1]; //!< 引数を表す値へのポインタの配列

public:
	//! @brief		引数0の引数を表すstaticオブジェクトへの参照を返す
	//! @return		引数0の引数を表すstaticオブジェクトへの参照
	static const tRisseMethodArgument & GetEmptyArgument()
	{
		// バイナリレイアウトが同一なので安全にキャストできるはず
		return *reinterpret_cast<tRisseMethodArgument*>(&EmptyArgument);
	}

public:
	//! @brief		N個の引数分のストレージを持つこのクラスのインスタンスを動的に作成して返す
	//! @param		ac		普通の引数の数
	//! @param		bc		ブロック引数の数
	//! @return		動的に確保されたこのクラスのインスタンス
	static tRisseMethodArgument & Allocate(risse_size ac, risse_size bc = 0);

public:
	//! @brief		引数=0のtRisseMethodArgumentOfオブジェクトを返す(New()のエイリアス)
	static const tRisseMethodArgument Empty()
	{
		return GetEmptyArgument();
	}

	//! @brief		引数=0のtRisseMethodArgumentOfオブジェクトを返す
	static const tRisseMethodArgument New()
	{
		return Empty();
	}

	//! @brief		普通の引数=1のtRisseMethodArgumentOfオブジェクトを返す
	//! @param		a0		パラメータ0
	//! @note		パラメータは参照(ポインタ)で保持されるため、このオブジェクトの存在期間中は
	//!				パラメータの実体が消えないように保証すること
	static const tRisseMethodArgumentOf<1> New(const tRisseVariant &a0)
	{
		tRisseMethodArgumentOf<1> arg;
		arg.SetArgument(0, a0);
		return arg;
	}

	//! @brief		普通の引数=2のtRisseMethodArgumentOfオブジェクトを返す
	//! @param		a0		パラメータ0
	//! @param		a1		パラメータ1
	//! @note		パラメータは参照(ポインタ)で保持されるため、このオブジェクトの存在期間中は
	//!				パラメータの実体が消えないように保証すること
	static const tRisseMethodArgumentOf<2> New(const tRisseVariant &a0,
								const tRisseVariant &a1)
	{
		tRisseMethodArgumentOf<2> arg;
		arg.SetArgument(0, a0);
		arg.SetArgument(1, a1);
		return arg;
	}

	//! @brief		普通の引数=3のtRisseMethodArgumentOfオブジェクトを返す
	//! @param		a0		パラメータ0
	//! @param		a1		パラメータ1
	//! @param		a2		パラメータ2
	//! @note		パラメータは参照(ポインタ)で保持されるため、このオブジェクトの存在期間中は
	//!				パラメータの実体が消えないように保証すること
	static const tRisseMethodArgumentOf<3> New(const tRisseVariant &a0,
								const tRisseVariant &a1, const tRisseVariant &a2)
	{
		tRisseMethodArgumentOf<3> arg;
		arg.SetArgument(0, a0);
		arg.SetArgument(1, a1);
		arg.SetArgument(2, a2);
		return arg;
	}

public:
	//! @brief		tRisseMethodArgumentへのキャスト
	//! @return		tRisseMethodArgumentへの参照
	//! @note		バイナリレイアウトが同一なのでtRisseMethodArgumentへは安全に
	//!				キャストできるはず
	operator const tRisseMethodArgument & () const
		{ return * reinterpret_cast<const tRisseMethodArgument *>(this); }

	//! @brief		普通の引数の個数を得る
	//! @return		普通の引数の個数
	risse_size GetArgumentCount() const { return ArgumentCount; }

	//! @brief		ブロック引数の個数を得る
	//! @return		ブロック引数の個数
	risse_size GetBlockArgumentCount() const { return BlockArgumentCount; }

	//! @brief		指定位置にある普通の引数への参照を返す(const版)
	//! @param		n		位置(0～)
	//! @return		引数への参照
	//! @note		n の範囲はチェックしていない
	const tRisseVariant & operator [] (risse_size n) const
	{
		return *Arguments[n];
	}

	//! @brief		指定位置にあるブロック引数への参照を返す(const版)
	//! @param		n		位置(0～)
	//! @return		引数への参照
	//! @note		n の範囲はチェックしていない
	const tRisseVariant & GetBlockArgument (risse_size n) const
	{
		return *Arguments[n];
	}

	//! @brief		普通の引数へ値への参照をセットする
	//! @param		n		パラメータ位置
	//! @param		v		(パラメータの値)
	void SetArgument(risse_size n, const tRisseVariant &v)
	{
		Arguments[n] = &v;
	}

	//! @brief		ブロック引数へ値への参照をセットする
	//! @param		n		パラメータ位置
	//! @param		v		(パラメータの値)
	void SetBlockArgument(risse_size n, const tRisseVariant &v)
	{
		Arguments[n+ArgumentCount] = &v;
	}

	//! @brief		引数が存在するかどうかを得る
	//! @param		n		パラメータ位置
	//! @return		引数が存在するかどうか
	bool HasArgument(risse_size n) const
	{
		if(GetArgumentCount() <= n) return false;
		// この時点で tRisseVariant と tRisseVariantData の両方が
		// 使用可能である (二つのクラス間の関係が定義できている) とは限らないため
		// やむを得ず reinterpret_cast をつかう。これはダウンキャストなので
		// 正常に動作するはずである。
		return !reinterpret_cast<const tRisseVariantData*>(Arguments[n])->IsVoid();
	}
};
//---------------------------------------------------------------------------


}


#endif

