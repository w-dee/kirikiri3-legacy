//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 文字列操作
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
//! @brief	文字列ブロック
//---------------------------------------------------------------------------
class tRisseStringBlock : public gc
{
	mutable risse_char  *	Buffer;	//!< 文字列バッファ
	risse_size				Length;	//!< 文字列長 (最後の \0 は含めない)

	const static risse_char MightBeShared  = static_cast<risse_char>(-1L);
		//!< 共有可能性フラグとして Buffer[-1] に設定する値

	static risse_char EmptyBuffer[2];
		//!< -1, 0 が入っている配列(空のバッファを表す)
	#define RISSE_STRING_EMPTY_BUFFER (EmptyBuffer+1)

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
			Buffer[n] = 0;
		else
			Buffer = RISSE_STRING_EMPTY_BUFFER; // Buffer を解放
	}

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
	tRisseStringBlock & operator += (const tRisseStringBlock & ref);
	tRisseStringBlock operator +  (const tRisseStringBlock & ref) const;

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
			Buffer = AllocateInternalBuffer(n), Buffer[n] = 0;
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

private:
	risse_char * InternalIndepend() const;

};
//---------------------------------------------------------------------------


typedef tRisseStringBlock tRisseString; //!< いまのところ tRisseString は tRisseStringBlock と同じ

//---------------------------------------------------------------------------
} // namespace Risse


#endif
