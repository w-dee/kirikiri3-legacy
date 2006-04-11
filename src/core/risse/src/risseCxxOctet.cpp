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
#include "prec.h"

#include "risseCharUtils.h"
#include "risseCxxOctet.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(22894,62791,44554,18883,55951,15622,1807,62704);

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
void tRisseOctetBlock::Append(const risse_uint8 * buffer, risse_size length)
{
	if(length == 0) return; // 追加するものなし

	risse_size newlength = Length + length;

	if(Capacity == 0)
	{
		// 共有可能性がある
		// 新しく領域を確保し、そこにコピーする
		risse_uint8 * newbuf = AllocateInternalBuffer(newlength);
		Capacity = newlength;
		memcpy(newbuf, Buffer, Length);
		memcpy(newbuf + Length, buffer, length);
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

		// 現在保持しているオクテット列の直後に buffer をコピーする
		memcpy(Buffer + Length, buffer, length);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseOctetBlock & tRisseOctetBlock::operator += (const tRisseOctetBlock & ref)
{
	if(Length == 0) return *this = ref; // 単純なコピーでよい
	Append(ref.Buffer, ref.Length);
	return *this;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseOctetBlock tRisseOctetBlock::operator + (const tRisseOctetBlock & ref) const
{
	tRisseOctetBlock block;
	Concat(&block, ref);
	return block;
}
//---------------------------------------------------------------------------


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


//---------------------------------------------------------------------------
tRisseString tRisseOctetBlock::ToTokenString(risse_size maxlen) const
{
	tRisseString ret;

	// 変換するオクテット長を計算
	risse_size max_octet_len;

	if(risse_size_max < 8)
		max_octet_len = 1; // 最低でも 1 オクテット
	else
		max_octet_len = (maxlen - 5) / 3;

	if(Length < max_octet_len) max_octet_len = Length;

	// 戻りバッファを確保
	risse_char * p = ret.Allocate(max_octet_len * 3 + 5 + 1); // 省略記号を含む場合があるので注意

	// バッファに内容を書き込む
	p[0] = RISSE_WC('<'); p[1] = RISSE_WC('%'); p[2] = RISSE_WC(' ');
	risse_size i;
	i = 3;
	const risse_uint8 * src = Buffer;

	risse_size remain = max_octet_len;
	while(remain --)
	{
		static const char hex[] = "0123456789abcdef";
		p[i]   = hex[*src >> 4];
		p[i+1] = hex[*src & 15];
		p[i+2] = RISSE_WC(' ');
		i += 3;
	}

	if(max_octet_len < Length)
	{
		p[i+0] = RISSE_WC('.'); p[i+1] = RISSE_WC('.'); p[i+2] = RISSE_WC('.');
		i += 3;
	}
	else
	{
		p[i  ] = RISSE_WC('%'); p[i+1] = RISSE_WC('>');
		i += 2;
	}

	// 戻り文字列の長さを調整
	ret.SetLength(i);

	// 帰る
	return ret;
}
//---------------------------------------------------------------------------


} // namespace Risse
