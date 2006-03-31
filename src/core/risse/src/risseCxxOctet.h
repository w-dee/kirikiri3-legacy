//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オクテット列のC++クラス
//---------------------------------------------------------------------------

#ifndef risseCxxOctetH
#define risseCxxOctetH


#include "risseTypes.h"
#include "risseAssert.h"
#include "gc_cpp.h"


namespace Risse
{
//---------------------------------------------------------------------------
//! @brief	オクテット列ブロック
//---------------------------------------------------------------------------
class tRisseOctetBlock : public gc
{
	mutable risse_uint8  *	Buffer;	//!< オクテット列バッファ (NULL = 0オクテット長)
	mutable risse_size Capacity; //!< 確保容量 ( 0 = バッファ共有中 )
	risse_size Length; //!< 長さ

public:
	//! @brief デフォルトコンストラクタ
	tRisseOctetBlock()
	{
		Buffer = 0;
		Capacity = Length = 0;
	}


	tRisseOctetBlock(const risse_uint8 * buf, risse_size length);

	tRisseOctetBlock(const tRisseOctetBlock & ref,
			risse_size offset, risse_size length);

	//! @brief コピーコンストラクタ
	//! @param ref コピー元オブジェクト
	tRisseOctetBlock(const tRisseOctetBlock & ref)
	{
		*this = ref;
	}

	//! @brief	代入演算子
	//! @param	ref	コピー元オブジェクト
	//! @return	このオブジェクトへの参照
	tRisseOctetBlock & operator = (const tRisseOctetBlock & ref)
	{
		ref.Capacity = Capacity = 0;
		Buffer = ref.Buffer;
		Length = ref.Length;
		return *this;
	}


public: // object property
	//! @brief オクテット列の長さを得る
	//! @return	オクテット列の長さ(コードポイント単位) (\0 は含まれない)
	risse_size GetLength() const { return Length; }

	//! @brief オクテット列の長さを設定する(切りつめのみ可)
	//! @param	n 新しい長さ(コードポイント単位)
	void SetLength(risse_size n)
	{
		Independ();
		Length = n;
		if(!n) Buffer = NULL, Capacity = 0;
	}

public: // pointer
	//! @brief  n バイト分のバッファを確保する
	//! @param  n 確保したいバイト数
	//! @return 確保した内部バッファ
	//! @note	n よりも短いオクテット列を返した場合は
	//! 		SetLength で正しい長さを設定すること。
	risse_uint8 * Allocate(risse_size n)
	{
		Capacity = Length = n;
		return Buffer = AllocateInternalBuffer(n);
	}

private: // storage
	//! @brief	n バイト分の内部用バッファを確保して返す
	//! @param	n	確保したいバイト数
	//! @param	prevbuf	以前のバッファ (再確保したい場合)
	//! @return	確保したバッファ
	static risse_uint8 * AllocateInternalBuffer(risse_size n, risse_uint8 * prevbuf = NULL)
	{
		return prevbuf ?
			reinterpret_cast<risse_uint8*>(GC_realloc(prevbuf, n)):
			reinterpret_cast<risse_uint8*>(GC_malloc_atomic(n));
	}

public: // comparison

	//! @brief 同一比較
	//! @param	ref		比較するオブジェクト
	//! @return	*this==refかどうか
	bool operator == (const tRisseOctetBlock & ref) const
	{
		if(this == &ref) return true; // 同じポインタ
		if(Length != ref.Length) return false; // 違う長さ
		if(Buffer == ref.Buffer) return true; // 同じバッファ
		return !memcmp(Buffer, ref.Buffer, Length);
	}

	//! @brief 不一致判定
	//! @param	ref		比較するオブジェクト
	//! @return	*this!=refかどうか
	bool operator != (const tRisseOctetBlock & ref) const
		{ return ! (*this == ref); }

public: // operators
	tRisseOctetBlock & operator += (const tRisseOctetBlock & ref);
	tRisseOctetBlock operator + (const tRisseOctetBlock & ref) const;
	void Concat(tRisseOctetBlock * dest, const tRisseOctetBlock & ref) const;

	//! @brief [] 演算子
	//! @param		n		位置
	//! @return		nの位置にあるコード
	risse_uint8 operator [] (risse_size n) const
	{
		RISSE_ASSERT(n < Length);
		return Buffer[n];
	}

public: // pointer

	//! @brief バッファをコピーし、独立させる
	//! @return 内部バッファ
	//! @note tRisseOctetBlock は一つのバッファを複数のオクテット列インスタンスが
	//! 共有する場合があるが、このメソッドは共有を切り、オクテット列バッファを
	//! 独立する。Risse の GC の特性上、そのオクテット列がすでに独立しているかどうかを
	//! 確実に知るすべはなく、このメソッドはかなりの確率でバッファをコピーするため、
	//! 実行が高価になる場合があることに注意すること。
	//! このメソッドは内部バッファへのポインタを返すが、このバッファに、もしもとの
	//! 長さよりも短い長さのオクテット列を書き込んだ場合は、SetLength を呼ぶこと。
	//! このメソッドは、内容が空の時は独立を行わなずに NULL を返す
	risse_uint8 * Independ() const
	{
		if(Capacity == 0) // 共有可能性？
			return InternalIndepend();
		return Buffer;
	}

private:
	risse_uint8 * InternalIndepend() const;

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	オクテット用ポインタ
//! @note
//! ポインタの最下位の2ビットが常に 10 なのは、このポインタが Octet列であることを
//! 表している。ポインタは常に少なくとも 32bit 境界に配置されるため、最下位の２ビットは
//! オブジェクトのタイプを表すのに利用されている。tRisseVariantを参照。
//---------------------------------------------------------------------------
class tRisseOctetPointer : public gc
{
	tRisseOctetBlock * Block; //!< ブロックへのポインタ (最下位の2ビットは常に10なので注意)
							//!< アクセス時は必ず GetBlock, SetBlock を用いること

protected: // Block pointer operation
	void SetBlock(tRisseOctetBlock * block)
		{ Block = reinterpret_cast<tRisseOctetBlock*>(reinterpret_cast<risse_ptruint>(block) + 2); }

	tRisseOctetBlock * GetBlock() const
		{ return reinterpret_cast<tRisseOctetBlock*>(reinterpret_cast<risse_ptruint>(Block) - 2); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	オクテット列
//---------------------------------------------------------------------------
class tRisseOctet : protected tRisseOctetPointer
{
public:
	//! @brief デフォルトコンストラクタ
	tRisseOctet()
	{
		SetBlock(new tRisseOctetBlock());
	}


	tRisseOctet(const risse_uint8 * buf, risse_size length)
	{
		SetBlock(new tRisseOctetBlock(buf, length));
	}

	tRisseOctet(const tRisseOctet & ref,
			risse_size offset, risse_size length)
	{
		SetBlock(new tRisseOctetBlock(*ref.GetBlock(), offset, length));
	}

	//! @brief コピーコンストラクタ
	//! @param ref コピー元オブジェクト
	tRisseOctet(const tRisseOctet & ref)
	{
		*this = ref;
	}

	//! @brief	代入演算子
	//! @param	ref	コピー元オブジェクト
	//! @return	このオブジェクトへの参照
	tRisseOctet & operator = (const tRisseOctet & ref)
	{
		*GetBlock() = *ref.GetBlock();
		return *this;
	}


public: // object property
	//! @brief オクテット列の長さを得る
	//! @return	オクテット列の長さ(コードポイント単位) (\0 は含まれない)
	risse_size GetLength() const { return GetBlock()->GetLength(); }

	//! @brief オクテット列の長さを設定する(切りつめのみ可)
	//! @param	n 新しい長さ(コードポイント単位)
	void SetLength(risse_size n)
	{
		GetBlock()->SetLength(n);
	}

public: // pointer
	//! @brief  n バイト分のバッファを確保する
	//! @param  n 確保したいバイト数
	//! @return 確保した内部バッファ
	//! @note	n よりも短いオクテット列を返した場合は
	//! 		SetLength で正しい長さを設定すること。
	risse_uint8 * Allocate(risse_size n)
	{
		return GetBlock()->Allocate(n);
	}

public: // comparison

	//! @brief 同一比較
	//! @param	ref		比較するオブジェクト
	//! @return	*this==refかどうか
	bool operator == (const tRisseOctet & ref) const
	{
		return *GetBlock() == *ref.GetBlock();
	}

	//! @brief 不一致判定
	//! @param	ref		比較するオブジェクト
	//! @return	*this!=refかどうか
	bool operator != (const tRisseOctet & ref) const
		{ return ! (*this == ref); }

public: // operators
	tRisseOctet & operator += (const tRisseOctet & ref)
	{
		*GetBlock() += *ref.GetBlock();
		return *this;
	}
	tRisseOctet operator + (const tRisseOctet & ref) const
	{
		tRisseOctet ret;
		GetBlock()->Concat(ret.GetBlock(), *ref.GetBlock());
		return ret;
	}

	//! @brief [] 演算子
	//! @param		n		位置
	//! @return		nの位置にあるコード
	risse_uint8 operator [] (risse_size n) const
	{
		return (*GetBlock())[n];
	}

public: // pointer

	//! @brief バッファをコピーし、独立させる
	//! @return 内部バッファ
	//! @note tRisseOctetBlock::Independ() を参照
	risse_uint8 * Independ() const
	{
		return GetBlock()->Independ();
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse


#endif
