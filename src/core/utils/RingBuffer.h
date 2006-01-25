//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief リングバッファを実現する自家製テンプレートクラス
//---------------------------------------------------------------------------
#ifndef RingBufferH
#define RingBufferH

/*
	リングバッファ, ring buffer, circular buffer, 環状バッファ
*/

//---------------------------------------------------------------------------
//! @brief		固定長リングバッファの実装
//---------------------------------------------------------------------------
template <typename T>
class tRisaRingBuffer
{
	T * Buffer; //!< バッファ
	size_t Size; //!< バッファのサイズ
	size_t WritePos; //!< 書き込み位置
	size_t ReadPos; //!< 読み込み位置
	size_t DataSize; //!< バッファに入っているデータのサイズ

public:
	//! @brief コンストラクタ
	tRisaRingBuffer(size_t size)
	{
		Size = size:
		Buffer = new T[Size];
		WritePos = ReadPos = 0;
		DataSize = Size;
	}

	//! @brief デストラクタ
	~tRisaRingBuffer()
	{
		delete [] Buffer;
	}

	//! @brief	サイズを得る
	size_t GetSize() { return Size; }

	//! @brief	書き込み位置を得る
	size_t GetWritePos() { return WritePos; }

	//! @brief	読み込み位置を得る
	size_t GetReadPos() { return ReadPos; }

	//! @brief	バッファに入っているデータのサイズを得る
	size_t GetDataSize() { return DataSize; }

	//! @brief	バッファの空き容量を得る
	size_t GetFreeSize() { return Size - DataSize; }

	//! @brief	バッファから読み込むためのポインタを得る
	//! @param	readsize 読み込みたいデータ数
	//! @param	p1		ブロック1の先頭へのポインタを格納するための変数
	//! @param	p1size	p1の表すブロックのサイズ
	//! @param	p2		ブロック2の先頭へのポインタを格納するための変数(NULLがあり得る)
	//! @param	p2size	p2の表すブロックのサイズ(0があり得る)
	//! @note	環状バッファといっても、実際はリニアな領域にバッファが確保されている。
	//!			そのため、 ReadPos + readsize がバッファの終端を超えている場合、得たい
	//!			ブロックは２つに分断されることになる。
	//!			このメソッドは、readsizeが実際にバッファに入っているデータのサイズ以下であるか
	//!			などのチェックはいっさい行わない。事前に GetDataSize を調べ、読み込みたい
	//!			サイズが実際にバッファにあるかどうかをチェックすること。
	void GetReadPointer(size_t readsize,
						T * const & p1, size_t &p1size,
						T * const & p2, size_t &p2size)
	{
		if(readsize + ReadPos >= Size)
		{
			// readsize + ReadPos がバッファの終端を超えている
			//  → 返されるブロックは2つ
			p1 = ReadPos + Buffer;
			p1size = Size - ReadPos;
			p2 = Buffer;
			p2size = readsize - p1size;
		}
		else
		{
			// readsize + ReadPos がバッファの終端を超えていない
			//  → 返されるブロックは1つ
			p1 = ReadPos + Buffer;
			p1size = readsize;
			p2 = NULL;
			p2size = 0;
		}
	}

	//! @brief	読み込みポインタを進める
	//! @param	advance		進める要素数
	//! @note	このメソッドは実際に advance < GetDataSize() であることを確認しない。
	//!			必要ならば呼び出し側でチェックすること。
	void AdvanceReadPos(size_t advance)
	{
		ReadPos += advance;
		if(ReadPos >= Size) ReadPos -= Size;
		DataSize -= advance;
	}

	//! @brief	バッファに書き込むのポインタを得る
	//! @param	writesize 書き込みたいデータ数
	//! @param	p1		ブロック1の先頭へのポインタを格納するための変数
	//! @param	p1size	p1の表すブロックのサイズ
	//! @param	p2		ブロック2の先頭へのポインタを格納するための変数(NULLがあり得る)
	//! @param	p2size	p2の表すブロックのサイズ(0があり得る)
	//! @note	GetReadPointerの説明も参照のこと
	void GetWritePointer(size_t writesize,
						T * & p1, size_t &p1size,
						T * & p2, size_t &p2size)
	{
		if(writesize + WritePos >= Size)
		{
			// writesize + WritePos がバッファの終端を超えている
			//  → 返されるブロックは2つ
			p1 = WritePos + Buffer;
			p1size = Size - WritePos;
			p2 = Buffer;
			p2size = writesize - p1size;
		}
		else
		{
			// writesize + WritePos がバッファの終端を超えていない
			//  → 返されるブロックは1つ
			p1 = WritePos + Buffer;
			p1size = writesize;
			p2 = NULL;
			p2size = 0;
		}
	}

	//! @brief	書き込みポインタを進める
	//! @param	advance		進める要素数
	//! @note	このメソッドは実際に advance < GetFreeSize() であることを確認しない。
	//!			必要ならば呼び出し側でチェックすること。
	void AdvanceWritePos(size_t advance)
	{
		WritePos += advance;
		if(WritePos >= Size) WritePos -= Size;
		DataSize += advance;
	}


};

#endif
