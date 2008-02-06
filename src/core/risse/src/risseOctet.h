//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オクテット列のC++クラス
//---------------------------------------------------------------------------

#ifndef risseOctetH
#define risseOctetH

/*! @note
Risse オクテット列について

Risse オクテット列は tOctetBlock クラスで表される。

*/

#include "risseTypes.h"
#include "risseAssert.h"
#include "risseString.h"
#include "risseGC.h"


namespace Risse
{
//---------------------------------------------------------------------------
//! @brief	オクテット列ブロック
//! @note	これ単体に対するスレッド保護はない
//---------------------------------------------------------------------------
class tOctetBlock : public tCollectee
{
	mutable risse_uint8  *	Buffer;	//!< オクテット列バッファ (NULL = 0オクテット長)
	mutable risse_size Capacity; //!< 確保容量 ( 0 = バッファ共有中 )
	risse_size Length; //!< 長さ

public:
	//! @brief デフォルトコンストラクタ
	tOctetBlock()
	{
		Buffer = 0;
		Capacity = Length = 0;
	}


	//! @brief		コンストラクタ(const risse_uint8 *から)
	//! @param		buf		入力バッファ
	//! @param		length	長さ
	//! @note		内容はコピーされる
	tOctetBlock(const risse_uint8 * buf, risse_size length);

	//! @brief		コンストラクタ(長さから)
	//! @param		length	長さ
	//! @note		バッファの内容は不定になる
	explicit tOctetBlock(risse_size length);

	//! @brief 部分オクテット列を作るためのコンストラクタ
	//! @param ref		コピー元オブジェクト
	//! @param offset	切り出す開始位置
	//! @param length	切り出す長さ
	tOctetBlock(const tOctetBlock & ref,
			risse_size offset, risse_size length);

	//! @brief コピーコンストラクタ
	//! @param ref コピー元オブジェクト
	tOctetBlock(const tOctetBlock & ref)
	{
		*this = ref;
	}

	//! @brief	代入演算子
	//! @param	ref	コピー元オブジェクト
	//! @return	このオブジェクトへの参照
	tOctetBlock & operator = (const tOctetBlock & ref)
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

	//! @brief オクテット列が空かどうかを得る @return オクテット列が空かどうか
	bool IsEmpty() const { return Length == 0; }

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

	//! @brief	既存のメモリ領域を「参照」するオクテット列を新規に作成して帰す
	static tOctetBlock MakeReference(const risse_uint8 * ptr, risse_size len)
	{
		tOctetBlock block;
		block.Capacity = 0;
		block.Length = len;
		block.Buffer = const_cast<risse_uint8 *>(ptr);
		return block;
	}

private: // storage
	//! @brief	n バイト分の内部用バッファを確保して返す
	//! @param	n	確保したいバイト数
	//! @param	prevbuf	以前のバッファ (再確保したい場合)
	//! @return	確保したバッファ
	static risse_uint8 * AllocateInternalBuffer(risse_size n, risse_uint8 * prevbuf = NULL)
	{
		return prevbuf ?
			reinterpret_cast<risse_uint8*>(ReallocCollectee(prevbuf, n)):
			reinterpret_cast<risse_uint8*>(MallocAtomicCollectee(n));
	}

public: // comparison

	//! @brief	同一比較
	//! @param	ref		比較するオブジェクト
	//! @return	*this==refかどうか
	bool operator == (const tOctetBlock & ref) const
	{
		if(this == &ref) return true; // 同じポインタ
		if(Length != ref.Length) return false; // 違う長さ
		if(Buffer == ref.Buffer) return true; // 同じバッファ
		if(Length == 0) return true; // 両方とも長さ 0
		/*
			::memcmp が長さが 0 の場合は真となってくれればよいのだが、
			すこし探してみたがそのようなことが保証されているとする
			ドキュメントが見つからなかった。
		*/
		return !::memcmp(Buffer, ref.Buffer, Length);
	}

	//! @brief	不一致判定
	//! @param	ref		比較するオブジェクト
	//! @return	*this!=refかどうか
	bool operator != (const tOctetBlock & ref) const
		{ return ! (*this == ref); }


	//! @brief	< 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this < refかどうか
	bool operator < (const tOctetBlock & ref) const;

	//! @brief	> 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this > refかどうか
	bool operator > (const tOctetBlock & ref) const
	{
		return ref.operator < (*this);
	}

	//! @brief	<= 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this <= refかどうか
	bool operator <= (const tOctetBlock & ref) const
	{
		return !(operator >(ref));
	}

	//! @brief	>= 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this >= refかどうか
	bool operator >= (const tOctetBlock & ref) const
	{
		return !(operator <(ref));
	}



public: // operators
	//! @brief		オクテット列の連結(risse_uint8 と length から)
	//! @param		buffer		連結するオクテット列
	//! @param		length 		連結するオクテット列の長さ
	void Append(const risse_uint8 * buffer, risse_size length);

	//! @brief		オクテット列の追加
	//! @param		ref		追加するオクテット列
	//! @return		このオブジェクト
	tOctetBlock & operator += (const tOctetBlock & ref);

	//! @brief		１バイトの追加
	//! @param		one_byte	追加するバイト
	//! @return		このオブジェクト
	tOctetBlock & operator += (risse_uint8 one_byte)
	{
		Append(&one_byte, 1);
		return *this;
	}

	//! @brief		オクテット列の連結
	//! @param		ref		連結するオクテット列
	//! @return		新しく連結されたオクテット列
	tOctetBlock operator + (const tOctetBlock & ref) const;

	//! @brief		オクテット列の連結
	//! @param		dest	連結されたオクテット列が格納される先(*this + ref がここに入る)
	//! @param		ref		連結するオクテット列
	void Concat(tOctetBlock * dest, const tOctetBlock & ref) const;

	//! @brief [] 演算子
	//! @param		n		位置
	//! @return		nの位置にあるコード
	risse_uint8 operator [] (risse_size n) const
	{
		RISSE_ASSERT(n < Length);
		return Buffer[n];
	}

	//! @brief	~ 演算子(ビット反転)
	//! @return		ビット反転されたオクテット列
	tOctetBlock operator ~() const;

	//! @brief	ビット反転
	//! @note	このオクテット列の内容をビット反転する
	void BitNot();

public: // pointer

	//! @brief バッファをコピーし、独立させる
	//! @return 内部バッファ
	//! @note tOctetBlock は一つのバッファを複数のオクテット列インスタンスが
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

	//! @brief 内部バッファへのポインタを獲る
	//! @return 内部バッファ
	//! @note	このメソッドは、内部バッファへの読み込み専用のポインタを獲る。
	//!			まれに ( Stream::read のような用途で) このバッファを書き込みように
	//!			使うことがあるが、アブノーマルな使い方なので通常はそのような
	//!			使い方をしてはならない。
	const risse_uint8 * Pointer() const
	{
		return Buffer;
	}

	//! @brief バッファの長さを実際の長さに合わせる
	//! @note
	//! += 演算子などは、バッファの容量増加に備え、バッファの確保容量を
	//! 実際のサイズよりも多めにとるが、このメソッドはその容量を長さぴったりに
	//! し、メモリを節約する。
	void Fit() const
	{
		if(Capacity != Length)
			InternalIndepend();
	}


private:
	//! @brief		オクテット列バッファをコピーし、独立させる
	//! @return		内部バッファ
	risse_uint8 * InternalIndepend() const;

public: // hint/hash

	//! @brief ヒントを得る
	//! @return ヒント (0 = ヒントが無効)
	risse_uint32 GetHint() const
	{
		// TODO: こいつの設計と実装
		return 0;
	}

	//! @brief 現在のオクテット列のハッシュに従ってヒントを設定する
	//! @note このメソッドはバッファの内容を変更するにもかかわらず const
	//! メソッドである。
	void SetHint() const
	{
		// TODO: こいつの設計と実装
	}

	//! @brief ヒントを設定する
	//! @brief hint  ハッシュ値
	//! @note このメソッドはバッファの内容を変更するにもかかわらず const
	//! メソッドである。
	void SetHint(risse_uint32 hint) const
	{
		// TODO: こいつの設計と実装
	}

	//! @brief オクテット列のハッシュを計算して返す
	//! @return ハッシュ値
	//! @note	戻り値は必ず0以外の値になる
	risse_uint32 GetHash() const;

public: // utilities

	//! @brief		値を再パース可能な文字列に変換する
	//! @return		再パース可能な文字列
	tString AsTokenString() const
	{ return AsHumanReadable(risse_size_max); }

	//! @brief		値を人間が読み取り可能な文字列に変換する
	//! @param		maxlen		おおよその最大コードポイント数; 収まり切らない場合は 
	//!							省略記号 '...' が付く(risse_size_maxの場合は無制限)
	//! @return		人間が読み取り可能な文字列
	tStringBlock AsHumanReadable(risse_size maxlen = risse_size_max) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	オクテット用データ
//! @note
//! ポインタの最下位の2ビットが常に 01 なのは、このポインタが Octet列であることを
//! 表している。ポインタは常に少なくとも 32bit 境界に配置されるため、最下位の２ビットは
//! オブジェクトのタイプを表すのに利用されている。tVariantを参照。
//---------------------------------------------------------------------------
class tOctetData : public tCollectee
{
	tOctetBlock * Block; //!< ブロックへのポインタ (最下位の2ビットは常に01なので注意)
							//!< アクセス時は必ず GetBlock, SetBlock を用いること

protected: // Block pointer operation
	void SetBlock(tOctetBlock * block)
		{ Block = reinterpret_cast<tOctetBlock*>(reinterpret_cast<risse_ptruint>(block) + 1); }

	tOctetBlock * GetBlock() const
		{ return reinterpret_cast<tOctetBlock*>(reinterpret_cast<risse_ptruint>(Block) - 1); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	オクテット列
//! @note	全体的に高速化の必要がある
//---------------------------------------------------------------------------
class tOctet : protected tOctetData
{
public:
	//! @brief デフォルトコンストラクタ
	tOctet()
	{
		SetBlock(new tOctetBlock());
	}

	//! @brief コンストラクタ(tOctetBlockより)
	explicit tOctet(tOctetBlock * block)
	{
		SetBlock(block);
	}

	//! @brief		コンストラクタ(const risse_uint8 *から)
	//! @param		buf		入力バッファ
	//! @param		length	長さ
	tOctet(const risse_uint8 * buf, risse_size length)
	{
		SetBlock(new tOctetBlock(buf, length));
	}

	//! @brief 部分オクテット列を作るためのコンストラクタ
	//! @param ref		コピー元オブジェクト
	//! @param offset	切り出す開始位置
	//! @param length	切り出す長さ
	tOctet(const tOctet & ref,
			risse_size offset, risse_size length)
	{
		SetBlock(new tOctetBlock(*ref.GetBlock(), offset, length));
	}

	//! @brief コピーコンストラクタ
	//! @param ref コピー元オブジェクト
	tOctet(const tOctet & ref)
	{
		*this = ref;
	}

	//! @brief	代入演算子
	//! @param	ref	コピー元オブジェクト
	//! @return	このオブジェクトへの参照
	tOctet & operator = (const tOctet & ref)
	{
		SetBlock(new tOctetBlock(*ref.GetBlock()));
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

	//! @brief オクテット列が空かどうかを得る @return オクテット列が空かどうか
	bool IsEmpty() const { return GetBlock()->IsEmpty(); }

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
	bool operator == (const tOctet & ref) const
	{
		return *GetBlock() == *ref.GetBlock();
	}

	//! @brief 不一致判定
	//! @param	ref		比較するオブジェクト
	//! @return	*this!=refかどうか
	bool operator != (const tOctet & ref) const
		{ return ! (*this == ref); }

	//! @brief	< 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this < refかどうか
	bool operator < (const tOctet & ref) const
		{ return *GetBlock() < *ref.GetBlock(); }

	//! @brief	> 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this > refかどうか
	bool operator > (const tOctet & ref) const
		{ return *GetBlock() > *ref.GetBlock(); }

	//! @brief	<= 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this <= refかどうか
	bool operator <= (const tOctet & ref) const
		{ return *GetBlock() <= *ref.GetBlock(); }

	//! @brief	>= 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this >= refかどうか
	bool operator >= (const tOctet & ref) const
		{ return *GetBlock() >= *ref.GetBlock(); }


public: // operators
	//! @brief		オクテット列の連結(risse_uint8 と length から)
	//! @param		buffer		連結するオクテット列
	//! @param		length 		連結するオクテット列の長さ
	void Append(const risse_uint8 * buffer, risse_size length)
	{
		GetBlock()->Append(buffer, length);
	}

	//! @brief		オクテット列の追加
	//! @param		ref		追加するオクテット列
	//! @return		このオブジェクト
	tOctet & operator += (const tOctet & ref)
	{
		*GetBlock() += *ref.GetBlock();
		return *this;
	}

	//! @brief		１バイトの追加
	//! @param		one_byte	追加するバイト
	//! @return		このオブジェクト
	tOctet & operator += (risse_uint8 one_byte)
	{
		*GetBlock() += one_byte;
		return *this;
	}

	//! @brief		オクテット列の連結
	//! @param		ref		連結するオクテット列
	//! @return		新しく連結されたオクテット列
	tOctet operator + (const tOctet & ref) const
	{
		tOctet ret;
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

	//! @brief		~ 演算子(ビット反転)
	//! @return		ビット反転されたオクテット列
	tOctet operator ~() const
	{
		tOctet ret(*this);
		ret.GetBlock()->BitNot();
		return ret;
	}

public: // pointer

	//! @brief バッファをコピーし、独立させる
	//! @return 内部バッファ
	//! @note tOctetBlock::Independ() を参照
	risse_uint8 * Independ() const
	{
		return GetBlock()->Independ();
	}

	//! @brief 内部バッファへのポインタを獲る
	//! @return 内部バッファ
	//! @note tOctetBlock::Pointer() を参照
	const risse_uint8 * Pointer() const
	{
		return GetBlock()->Pointer();
	}

	//! @brief バッファの長さを実際の長さに合わせる
	//! @note
	//! += 演算子などは、バッファの容量増加に備え、バッファの確保容量を
	//! 実際のサイズよりも多めにとるが、このメソッドはその容量を長さぴったりに
	//! し、メモリを節約する。
	void Fit() const
	{
		GetBlock()->Fit();
	}

public: // hint/hash

	//! @brief ヒントを得る
	//! @return ヒント (0 = ヒントが無効)
	risse_uint32 GetHint() const
	{ return GetBlock()->GetHint(); }

	//! @brief 現在のオクテット列のハッシュに従ってヒントを設定する
	//! @note このメソッドはバッファの内容を変更するにもかかわらず const
	//! メソッドである。
	void SetHint() const
	{ GetBlock()->SetHint(); }

	//! @brief ヒントを設定する
	//! @brief hint  ハッシュ値
	//! @note このメソッドはバッファの内容を変更するにもかかわらず const
	//! メソッドである。
	void SetHint(risse_uint32 hint) const
	{ GetBlock()->SetHint(hint); }

	//! @brief オクテット列のハッシュを計算して返す
	//! @return ハッシュ値
	//! @note	戻り値は必ず0以外の値になる
	risse_uint32 GetHash() const
	{ return GetBlock()->GetHash(); }

public: // utilities
	//! @brief		値を再パース可能な文字列に変換する
	//! @return		再パース可能な文字列
	tString AsTokenString() const
	{ return GetBlock()->AsTokenString(); }

	//! @brief		値を人間が読み取り可能な文字列に変換する
	//! @param		maxlen		おおよその最大コードポイント数; 収まり切らない場合は 
	//!							省略記号 '...' が付く(risse_size_maxの場合は無制限)
	//! @return		人間が読み取り可能な文字列
	tStringBlock AsHumanReadable(risse_size maxlen = risse_size_max) const
	{ return GetBlock()->AsHumanReadable(maxlen); }

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse


#endif
