//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 文字列のC++クラス
//---------------------------------------------------------------------------

#ifndef risseCxxStringH
#define risseCxxStringH


#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "gc_cpp.h"


namespace Risse
{
//---------------------------------------------------------------------------
//! @brief	文字列用データ
//---------------------------------------------------------------------------
class tRisseStringData : public gc
{
protected:
	mutable risse_char  *	Buffer;	//!< 文字列バッファ
	risse_size				Length;	//!< 文字列長 (最後の \0 は含めない)

	const static risse_char MightBeShared  = static_cast<risse_char>(-1L);
		//!< 共有可能性フラグとして Buffer[-1] に設定する値

	static risse_char EmptyBuffer[3];
		//!< -1, 0, 0 が入っている配列(空のバッファを表す)
	#define RISSE_STRING_EMPTY_BUFFER (EmptyBuffer+1)

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	文字列ブロック
//---------------------------------------------------------------------------
class tRisseStringBlock : protected tRisseStringData
{
public:
	//! @brief デフォルトコンストラクタ
	tRisseStringBlock()
	{
		Buffer = RISSE_STRING_EMPTY_BUFFER;
		Length = 0;
	}

	//! @brief コピーコンストラクタ
	//! @param ref コピー元オブジェクト
	tRisseStringBlock(const tRisseStringBlock & ref)
	{
		*this = ref;
	}

	tRisseStringBlock(const tRisseStringBlock & ref,
		risse_size offset, risse_size length);

	//! @brief		コンストラクタ(risse_char * から)
	//! @param		ref		元の文字列
	tRisseStringBlock(const risse_char * ref)
	{
		*this = ref;
	}

	tRisseStringBlock(const risse_char * ref, risse_size n);

#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	//! @brief		コンストラクタ(wchar_t * から)
	//! @param		ref		元の文字列
	tRisseStringBlock(const wchar_t *ref)
	{
		*this = ref;
	}
#endif

//#ifdef RISSE_SUPPORT_WX
// TODO: パフォーマンス的に問題になりそうならばこれを実装すること
//	tRisseStringBlock(const wxString & ref);
//#endif

	//! @brief		コンストラクタ(char * から)
	//! @param		ref		元の文字列
	tRisseStringBlock(const char * ref)
	{
		*this = ref;
	}

	tRisseStringBlock(const tRisseStringBlock &msg, const tRisseStringBlock &r1);
	tRisseStringBlock(const tRisseStringBlock &msg, const tRisseStringBlock &r1,
					const tRisseStringBlock &r2);
	tRisseStringBlock(const tRisseStringBlock &msg, const tRisseStringBlock &r1,
					const tRisseStringBlock &r2, const tRisseStringBlock &r3);
	tRisseStringBlock(const tRisseStringBlock &msg, const tRisseStringBlock &r1,
					const tRisseStringBlock &r2, const tRisseStringBlock &r3,
					const tRisseStringBlock &r4);

	//! @brief	代入演算子
	//! @param	ref	コピー元オブジェクト
	//! @return	このオブジェクトへの参照
	tRisseStringBlock & operator = (const tRisseStringBlock & ref)
	{
		if(ref.Buffer[-1] == 0)
			ref.Buffer[-1] = MightBeShared; // 共有可能性フラグをたてる
		Buffer = ref.Buffer;
		Length = ref.Length;
		return *this;
	}

	tRisseStringBlock & operator = (const risse_char * ref);

	tRisseStringBlock & operator = (const risse_char ref);

#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	tRisseStringBlock & operator = (const wchar_t *ref);
#endif

//#ifdef RISSE_SUPPORT_WX
// TODO: パフォーマンス的に問題になりそうならばこれを実装すること
//	tRisseStringBlock & operator = (const wxString & ref);
//#endif

	tRisseStringBlock & operator = (const char * ref);

	//! @brief	バッファをコピーし、新しい tRisseStringBlock を返す
	//! @param	ref	コピー元オブジェクト
	//! @return	新しい tRisseStringBlock オブジェクト
	//! @note	このメソッドは、必ずバッファをコピーして返し、
	//!			元の文字列 (この文字列) の共有状態などはいっさい
	//!			変更しない。破壊を前提とした文字列を他の文字列
	//!			から作成する場合などに効率的。
	tRisseStringBlock MakeBufferCopy(const tRisseStringBlock & ref) const
	{
		return tRisseStringBlock(Buffer, Length);
	}


private: // buffer management
	static risse_char * AllocateInternalBuffer(risse_size n, risse_char *prevbuf = NULL);


	//! @brief		バッファに割り当てられているコードポイント数(容量)を得る
	//! @param		buffer バッファ
	//! @return		コードポイント数
	//! @note		Buffer[-1] が 0 の時のみにこのメソッドを呼ぶこと。
	//!				それ以外の場合は返値は信用してはならない。
	static risse_size GetBufferCapacity(const risse_char * buffer)
	{
		return
			*reinterpret_cast<const risse_size *>(
				reinterpret_cast<const char *>(buffer) -
					(sizeof(risse_char) + sizeof(risse_size)));
	}

public: // pointer
	//! @brief バッファを割り当てる
	//! @param  n バッファに割り当てる文字数 (最後の \0 は含まない)
	//! @return	文字列バッファ
	//! @note このメソッドを使った後、もし n と異なる
	//! 長さを書き込んだ場合は、FixLength あるいは
	//! SetLength を呼ぶこと。
	risse_char * Allocate(risse_size n)
	{
		if(n)
			Buffer = AllocateInternalBuffer(n), Buffer[n] = Buffer[n+1] = 0;
		else
			Buffer = RISSE_STRING_EMPTY_BUFFER;
		Length = n;
		return Buffer;
	}

	//! @brief 内部で持っている文字列の長さを、実際の長さに合わせる
	void FixLength()
	{
		if((Length = Risse_strlen(Buffer)) == 0)
			Buffer = RISSE_STRING_EMPTY_BUFFER;
		Buffer[Length + 1] = 0; // hint をクリア
	}

	//! @brief C 言語スタイルのポインタを得る
	//! @note  tRisseStringBlock は内部に保持しているバッファの最後が \0 で
	//! 終わってない場合は、バッファを新たにコピーして \0 で終わらせ、その
	//! バッファのポインタを返す。また、空文字列の場合は NULL を返さずに
	//! "" へのポインタを返す。
	const risse_char * c_str() const
	{
		if(Buffer[Length]) return Independ();
		return Buffer;
	}

	//! @brief 内部バッファのポインタを返す
	//! @return 内部バッファのポインタ
	//! @note  このメソッドで返されるポインタは、しばしば
	//! 期待した位置に \0 がない (null終結している保証がない)
	const risse_char * Pointer() const { return Buffer; }

	//! @brief 文字列バッファをコピーし、独立させる
	//! @return 内部バッファ
	//! @note tRisseStringBlock は一つのバッファを複数の文字列インスタンスが
	//! 共有する場合があるが、このメソッドは共有を切り、文字列バッファを
	//! 独立する。Risse の GC の特性上、その文字列がすでに独立しているかどうかを
	//! 確実に知るすべはなく、このメソッドはかなりの確率でバッファをコピーするため、
	//! 実行が高価になる場合があることに注意すること。
	//! このメソッドは内部バッファへのポインタを返すが、このバッファに、もしもとの
	//! 長さよりも短い長さの文字列を書き込んだ場合は、FixLength あるいは
	//! SetLength を呼ぶこと。
	//! このメソッドは、内容が空の時は独立を行わなずに NULL を返す
	risse_char * Independ() const
	{
		if(Buffer[-1]) // 共有可能性フラグが立っている？
			return InternalIndepend();
		return Buffer;
	}

	//! @brief	文字列バッファを文字列と同じサイズにする
	//! @note	Append や += 演算子などは、後のサイズの増加に備えて内部バッファを
	//!			余分に取るが、その余分を切り捨てる。すでにぴったりなサイズ
	//!			だった場合はなにもしない。また、共有中の場合は共有を断ち切る。
	void Fit() const
	{
		if(Buffer[-1]) // 共有可能性フラグが立っている？
			InternalIndepend();
		if(GetBufferCapacity(Buffer) != Length) // 長さがぴったりではない
			InternalIndepend();
	}

private:
	risse_char * InternalIndepend() const;

public: // hint/hash
	//! @brief	ヒントへのポインタを得る
	//! @return ヒントへのポインタ
	//! @note
	//! ここで返されるヒントのポインタは、この文字列オブジェクトの他の
	//! 破壊的メソッドを呼ぶと無効になる。<br>
	//! 破壊的メソッドは const メソッドでも内部バッファを破壊する場合がある
	//! ので注意。これにはc_str()も含む。Pointer() や GetLength(),
	//! operator [] は大丈夫。 <br>
	//! 使用可能ならば常に GetHint() か SetHint() を用いること。<br>
	//! このポインタは、ヒントが使用不可の場合は NULL が帰る。
	risse_uint32 * GetHintPointer() const
	{
		if(!Buffer[Length])
		{
			// バッファの期待した位置に \0終端がある。
			// この場合はその次をヒントへのポインタと見なすことができる。
			return reinterpret_cast<risse_uint32*>(Buffer + Length + 1);
		}
		return NULL;
	}

	//! @brief ヒントを得る
	//! @return ヒント (0 = ヒントが無効)
	risse_uint32 GetHint() const
	{
		if(!Buffer[Length])
			return *reinterpret_cast<risse_uint32*>(Buffer + Length + 1);
		return 0;
	}

	//! @brief 現在の文字列のハッシュに従ってヒントを設定する
	//! @note このメソッドはバッファの内容を変更するにもかかわらず const
	//! メソッドである。
	void SetHint() const
	{
		if(!Buffer[Length])
			*reinterpret_cast<risse_uint32*>(Buffer + Length + 1) = GetHash();
	}

	//! @brief ヒントを設定する
	//! @brief hint  ハッシュ値
	//! @note このメソッドはバッファの内容を変更するにもかかわらず const
	//! メソッドである。
	void SetHint(risse_uint32 hint) const
	{
		if(!Buffer[Length])
			*reinterpret_cast<risse_uint32*>(Buffer + Length + 1) = hint;
	}

	risse_uint32 GetHash() const;

public: // object property
	//! @brief 文字列の長さを得る
	//! @return	文字列の長さ(コードポイント単位) (\0 は含まれない)
	risse_size GetLength() const { return Length; }

	//! @brief 文字列の長さを設定する(切りつめのみ可)
	//! @param	n 新しい長さ(コードポイント単位)
	void SetLength(risse_size n)
	{
		RISSE_ASSERT(n <= Length);
		Independ();
		Length = n;
		if(n)
			Buffer[n] = Buffer[n+1] = 0; // null終端と hint をクリア
		else
			Buffer = RISSE_STRING_EMPTY_BUFFER; // Buffer を解放
	}

	//! @brief 文字列が空かどうかを得る @return 文字列が空かどうか
	bool IsEmpty() const { return Length == 0; }

public: // comparison
	//! @brief < 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this<refかどうか
	bool operator <  (const tRisseStringBlock & ref) const
	{
		return Risse_strcmp(Buffer, ref.Buffer) < 0;
	}

	//! @brief > 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this>refかどうか
	bool operator >  (const tRisseStringBlock & ref) const
		{ return ref < *this; }

	//! @brief <= 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this<=refかどうか
	bool operator <= (const tRisseStringBlock & ref) const
		{ return ! (*this > ref); }

	//! @brief >= 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this>=refかどうか
	bool operator >= (const tRisseStringBlock & ref) const
		{ return ! (*this < ref); }

	//! @brief 同一比較
	//! @param	ref		比較するオブジェクト
	//! @return	*this==refかどうか
	bool operator == (const tRisseStringBlock & ref) const
	{
		if(this == &ref) return true; // 同じポインタ
		if(Length != ref.Length) return false; // 違う長さ
		if(Buffer == ref.Buffer) return true; // 同じバッファ
		return !Risse_strcmp(Buffer, ref.Buffer);
	}

	//! @brief 不一致判定
	//! @param	ref		比較するオブジェクト
	//! @return	*this!=refかどうか
	bool operator != (const tRisseStringBlock & ref) const
		{ return ! (*this == ref); }

public: // operators
	void Append(const risse_char * buffer, risse_size length);

	//! @brief		文字列の連結
	//! @param		ref		連結する文字列
	//! @return		このオブジェクトへの参照
	tRisseStringBlock & operator += (const tRisseStringBlock & ref)
	{
		if(Length == 0) { *this = ref; return *this; }
		Append(ref.Buffer, ref.Length);
		return *this;
	}

	//! @brief		文字の連結
	//! @param		ref		連結する文字
	//! @return		このオブジェクトへの参照
	tRisseStringBlock & operator += (risse_char ref)
	{
		if(ref == 0) { return *this; } // やることなし
		if(Length == 0) { *this = ref; return *this; }
		Append(&ref, 1); // 文字を追加
		return *this;
	}

	tRisseStringBlock operator + (const tRisseStringBlock & ref) const;

	//! @brief [] 演算子
	//! @param		n		位置
	//! @return		nの位置にあるコード
	risse_char operator [] (risse_size n) const
	{
		RISSE_ASSERT(n < Length);
		return Buffer[n];
	}

public: // conversion

#ifdef RISSE_SUPPORT_WX
	wxString AsWxString() const
		{ return RisseCharToWxString(Buffer, Length); }
	operator wxString() const
		{ return AsWxString(); }
#endif

public: // other utilities
	tRisseStringBlock Replace(const tRisseStringBlock &old_str,
		const tRisseStringBlock &new_str, bool replace_all = true) const;
};
//---------------------------------------------------------------------------


typedef tRisseStringBlock tRisseString; //!< いまのところ tRisseString は tRisseStringBlock と同じ

//---------------------------------------------------------------------------
} // namespace Risse


#endif
