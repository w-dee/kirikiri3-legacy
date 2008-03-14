//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
//---------------------------------------------------------------------------
//! @brief		「不正なパラメータの個数」例外を発生(暫定)
//! @param		passed		実際に渡された個数
//! @param		expected	期待した数
void ThrowBadArgumentCount(risse_size passed, risse_size expected);
//! @brief		「不正なブロックパラメータの個数」例外を発生(暫定)
//! @param		passed		実際に渡された個数
//! @param		expected	期待した数
void ThrowBadBlockArgumentCount(risse_size passed, risse_size expected);
//---------------------------------------------------------------------------


class tVariantBlock;
typedef tVariantBlock tVariant;
class tMethodArgument;

//---------------------------------------------------------------------------
//! @brief		メソッドへ渡す引数を表すクラス(可変引数用テンプレートクラス)
//! @note		スレッド保護はない
//---------------------------------------------------------------------------
template <risse_size AC, risse_size BC = 0>
class tMethodArgumentOf : public tCollectee
{
protected:
	risse_size ArgumentCount; //!< 普通の引数の配列の個数
	risse_size BlockArgumentCount; //!< ブロック引数の配列の個数
	const tVariant * Arguments[(AC+BC)<1?1:(AC+BC)]; //!< 普通の引数を表す値へのポインタの配列

public:
	//! @brief		コンストラクタ
	tMethodArgumentOf()
	{
		ArgumentCount = AC;
		BlockArgumentCount = BC;
		// Argumentsはここでは初期化しないので利用する側で代入して使うこと
		// (あまり設計が良くない)
	}


public:
	//! @brief		tMethodArgumentへのキャスト
	//! @return		tMethodArgumentへの参照
	//! @note		バイナリレイアウトが同一なのでtMethodArgumentへは安全に
	//!				キャストできるはず
	operator const tMethodArgument & () const
		{ return * reinterpret_cast<const tMethodArgument *>(this); }

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
	const tVariant & operator [] (risse_size n) const
	{
		return *Arguments[n];
	}

	//! @brief		指定位置にあるブロック引数への参照を返す(const版)
	//! @param		n		位置(0～)
	//! @return		引数への参照
	//! @note		n の範囲はチェックしていない
	const tVariant & GetBlockArgument (risse_size n) const
	{
		return *Arguments[n];
	}

	//! @brief		普通の引数へ値への参照をセットする
	//! @param		n		パラメータ位置
	//! @param		v		(パラメータの値)
	//! @note		tMethodArgument とその仲間は、値へのポインタしか保持しない。このため、
	//!				値が関数呼び出しの間存在し、不変であることを呼び出し側で保証すること。
	void SetArgument(risse_size n, const tVariant *v)
	{
		Arguments[n] = v;
	}

	//! @brief		ブロック引数へ値への参照をセットする
	//! @param		n		パラメータ位置
	//! @param		v		(パラメータの値)
	//! @note		tMethodArgument とその仲間は、値へのポインタしか保持しない。このため、
	//!				値が関数呼び出しの間存在し、不変であることを呼び出し側で保証すること。
	void SetBlockArgument(risse_size n, const tVariant *v)
	{
		Arguments[n+ArgumentCount] = v;
	}

	//! @brief		普通の引数が想定した数未満の場合に例外を発生させる
	//! @param		n		想定したパラメータの数
	void ExpectArgumentCount(risse_size n) const
	{
		if(AC < n) ThrowBadArgumentCount(AC, n);
	}

	//! @brief		ブロック引数が想定した数未満の場合に例外を発生させる
	//! @param		n		想定したパラメータの数
	void ExpectBlockArgumentCount(risse_size n) const
	{
		if(BC < n) ThrowBadBlockArgumentCount(BC, n);
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メソッドへ渡す引数を表すクラス
//! @note		スレッド保護はない
//---------------------------------------------------------------------------
class tMethodArgument : public tCollectee
{
	class tEmptyMethodArgument
	{
	public:
		risse_size ArgumentCount; //!< 普通の引数の個数
		risse_size BlockArgumentCount; //!< ブロック引数の個数
		const tVariant * Arguments[1]; //!< 引数を表す値へのポインタの配列
	};
	static tEmptyMethodArgument EmptyArgument;

protected:
	risse_size ArgumentCount; //!< 普通の引数の配列の個数
	risse_size BlockArgumentCount; //!< ブロック引数の配列の個数
	const tVariant * Arguments[1]; //!< 引数を表す値へのポインタの配列

private:
	tMethodArgument(const tMethodArgument &); // non copy-able
	void operator = (const tMethodArgument &); // non copy-able

public:
	//! @brief		引数0の引数を表すstaticオブジェクトへの参照を返す
	//! @return		引数0の引数を表すstaticオブジェクトへの参照
	static const tMethodArgument & GetEmptyArgument()
	{
		// バイナリレイアウトが同一なので安全にキャストできるはず
		return *reinterpret_cast<tMethodArgument*>(&EmptyArgument);
	}


private:
	tMethodArgument(); //!< コンストラクタ (このクラスのインスタンスを作成するには以下のstatic関数をつかってね)

public:
	//! @brief		N個の引数分のストレージを持つこのクラスのインスタンスを動的に作成して返す
	//! @param		ac		普通の引数の数
	//! @param		bc		ブロック引数の数
	//! @return		動的に確保されたこのクラスのインスタンス
	static tMethodArgument & Allocate(risse_size ac, risse_size bc = 0);

public:
	//! @brief		引数=0のtMethodArgumentOfオブジェクトを返す(New()のエイリアス)
	static const tMethodArgument & Empty()
	{
		return GetEmptyArgument();
	}

	//! @brief		引数=0のtMethodArgumentOfオブジェクトを返す
	static const tMethodArgument & New()
	{
		return Empty();
	}

	//! @brief		普通の引数=1のtMethodArgumentOfオブジェクトを返す
	//! @param		a0		パラメータ0
	//! @note		パラメータは参照(ポインタ)で保持されるため、このオブジェクトの存在期間中は
	//!				パラメータの実体が消えないように保証すること
	static const tMethodArgumentOf<1> New(const tVariant &a0)
	{
		tMethodArgumentOf<1> arg;
		arg.SetArgument(0, &a0);
		return arg;
	}

	//! @brief		普通の引数=2のtMethodArgumentOfオブジェクトを返す
	//! @param		a0		パラメータ0
	//! @param		a1		パラメータ1
	//! @note		パラメータは参照(ポインタ)で保持されるため、このオブジェクトの存在期間中は
	//!				パラメータの実体が消えないように保証すること
	static const tMethodArgumentOf<2> New(const tVariant &a0,
								const tVariant &a1)
	{
		tMethodArgumentOf<2> arg;
		arg.SetArgument(0, &a0);
		arg.SetArgument(1, &a1);
		return arg;
	}

	//! @brief		普通の引数=3のtMethodArgumentOfオブジェクトを返す
	//! @param		a0		パラメータ0
	//! @param		a1		パラメータ1
	//! @param		a2		パラメータ2
	//! @note		パラメータは参照(ポインタ)で保持されるため、このオブジェクトの存在期間中は
	//!				パラメータの実体が消えないように保証すること
	static const tMethodArgumentOf<3> New(const tVariant &a0,
								const tVariant &a1, const tVariant &a2)
	{
		tMethodArgumentOf<3> arg;
		arg.SetArgument(0, &a0);
		arg.SetArgument(1, &a1);
		arg.SetArgument(2, &a2);
		return arg;
	}

public:
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
	const tVariant & operator [] (risse_size n) const
	{
		return *Arguments[n];
	}

	//! @brief		指定位置にあるブロック引数への参照を返す(const版)
	//! @param		n		位置(0～)
	//! @return		引数への参照
	//! @note		n の範囲はチェックしていない
	const tVariant & GetBlockArgument (risse_size n) const
	{
		return *Arguments[n+ArgumentCount];
	}

	//! @brief		普通の引数へ値への参照をセットする
	//! @param		n		パラメータ位置
	//! @param		v		(パラメータの値)
	//! @note		tMethodArgument とその仲間は、値へのポインタしか保持しない。このため、
	//!				値が関数呼び出しの間存在し、不変であることを呼び出し側で保証すること。
	void SetArgument(risse_size n, const tVariant *v)
	{
		Arguments[n] = v;
	}

	//! @brief		ブロック引数へ値への参照をセットする
	//! @param		n		パラメータ位置
	//! @param		v		(パラメータの値)
	//! @note		tMethodArgument とその仲間は、値へのポインタしか保持しない。このため、
	//!				値が関数呼び出しの間存在し、不変であることを呼び出し側で保証すること。
	void SetBlockArgument(risse_size n, const tVariant *v)
	{
		Arguments[n+ArgumentCount] = v;
	}

	//! @brief		引数が存在するかどうかを得る
	//! @param		n		パラメータ位置
	//! @return		引数が存在するかどうか
	bool HasArgument(risse_size n) const
	{
		if(GetArgumentCount() <= n) return false;
		// この時点で tVariant と tVariantData の両方が
		// 使用可能である (二つのクラス間の関係が定義できている) とは限らないため
		// やむを得ず static_cast をつかう。これはダウンキャストなので
		// 正常に動作するはずである。
		return !reinterpret_cast<const tVariantData*>(Arguments[n])->IsVoid();
	}

	//! @brief		ブロック引数が存在するかどうかを得る
	//! @param		n		パラメータ位置
	//! @return		ブロック引数が存在するかどうか
	bool HasBlockArgument(risse_size n) const
	{
		if(GetBlockArgumentCount() <= n) return false;
		// HasArgument の説明を参照のこと
		return !reinterpret_cast<const tVariantData*>(Arguments[n+ArgumentCount])->IsVoid();
	}

	//! @brief		普通の引数が想定した数未満の場合に例外を発生させる
	//! @param		n		想定したパラメータの数
	void ExpectArgumentCount(risse_size n) const
	{
		if(ArgumentCount < n) ThrowBadArgumentCount(ArgumentCount, n);
	}

	//! @brief		ブロック引数が想定した数未満の場合に例外を発生させる
	//! @param		n		想定したパラメータの数
	void ExpectBlockArgumentCount(risse_size n) const
	{
		if(BlockArgumentCount < n) ThrowBadBlockArgumentCount(BlockArgumentCount, n);
	}
};
//---------------------------------------------------------------------------


}


#endif

