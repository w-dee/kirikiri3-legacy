//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オクテット列操作
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseCharUtils.h"
#include "risseCxxOctet.h"

RISSE_DEFINE_SOURCE_ID(22894,62791,44554,18883,55951,15622,1807,62704);

/*! @note
Risse オクテット列について

Risse オクテット列は tRisseOctetBlock クラスで表される。

*/

namespace Risse
{

//---------------------------------------------------------------------------
//! @brief		コンストラクタ(const risse_uint8 *から)
//! @param		buf		入力バッファ
//! @param		length	長さ
//---------------------------------------------------------------------------
tRisseOctetBlock::tRisseOctetBlock(const risse_uint8 * buf, risse_size length)
{
	Capacity = Length = length;
	if(length)
	{
		Buffer = AllocateInternalBuffer(length);
		memcpy(Buffer, buf, length);
	}
	else
	{
		Buffer = NULL;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief 部分オクテット列を作るためのコンストラクタ
//! @param ref		コピー元オブジェクト
//! @param offset	切り出す開始位置
//! @param length	切り出す長さ
//---------------------------------------------------------------------------
tRisseOctetBlock::tRisseOctetBlock(const tRisseOctetBlock & ref,
	risse_size offset, risse_size length)
{
	if(length)
	{
		RISSE_ASSERT(ref.Length - offset >= length);
		ref.Capacity = 0; // 共有可能性を表す
		Buffer = ref.Buffer + offset;
		Length = length;
	}
	else
	{
		Buffer = NULL;
		Length = Capacity = 0;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		オクテット列の連結
//! @param		ref		連結するオクテット列
//! @return		このオブジェクト
//---------------------------------------------------------------------------
tRisseOctetBlock & tRisseOctetBlock::operator += (const tRisseOctetBlock & ref)
{
	if(ref.Length == 0) return *this; // 追加するものなし
	if(Length == 0) return *this = ref; // 単純なコピーでよい

	risse_size newlength = Length + ref.Length;

	if(Capacity == 0)
	{
		// 共有可能性がある
		// 新しく領域を確保し、そこにコピーする
		risse_uint8 * newbuf = AllocateInternalBuffer(newlength);
		Capacity = newlength;
		memcpy(newbuf, Buffer, Length);
		memcpy(newbuf + Length, ref.Buffer, ref.Length);
		Buffer = newbuf;
	}
	else
	{
		// 共有可能性フラグは立っていない
		// 現在の領域を拡張する必要がある？
		if(Capacity < newlength)
		{
			// 容量が足りないので拡張する必要あり
			// 適当に新規確保の容量を計算
			risse_size newcapacity;
			if(newlength < 16*1024)
				newcapacity = newlength * 2;
			else
				newcapacity = newlength + 16*1024;
			// バッファを再確保
			Buffer = AllocateInternalBuffer(newcapacity, Buffer);
			Capacity = newcapacity;
		}

		// 現在保持しているオクテット列の直後に ref の Buffer をコピーする
		memcpy(Buffer + Length, ref.Buffer, ref.Length);
	}

	return *this;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		オクテット列の連結
//! @param		ref		連結するオクテット列
//! @return		新しく連結されたオクテット列
//---------------------------------------------------------------------------
tRisseOctetBlock tRisseOctetBlock::operator + (const tRisseOctetBlock & ref) const
{
	tRisseOctetBlock block;
	Concat(&block, ref);
	return block;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		オクテット列の連結
//! @param		dest	連結されたオクテット列が格納される先(*this + ref がここに入る)
//! @param		ref		連結するオクテット列
//---------------------------------------------------------------------------
void tRisseOctetBlock::Concat(tRisseOctetBlock * dest, const tRisseOctetBlock & ref) const
{
	if(Length == 0) { *dest = ref; return; }
	if(ref.Length == 0) { *dest = *this; return; }

	risse_size newsize = Length + ref.Length;
	dest->Allocate(newsize);
	memcpy(dest->Buffer, Buffer, Length);
	memcpy(dest->Buffer + Length, ref.Buffer, ref.Length);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		オクテット列バッファをコピーし、独立させる
//! @return		内部バッファ
//---------------------------------------------------------------------------
risse_uint8 * tRisseOctetBlock::InternalIndepend() const
{
	if(!Buffer) return NULL;
	risse_uint8 * newbuf = AllocateInternalBuffer(Length);
	memcpy(newbuf, Buffer, Length);
	Buffer = newbuf;
	Capacity = Length;
	return Buffer;
}
//---------------------------------------------------------------------------
} // namespace Risse
