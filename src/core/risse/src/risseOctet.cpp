//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オクテット列のC++クラス
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseCharUtils.h"
#include "risseOctet.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(22894,62791,44554,18883,55951,15622,1807,62704);

//---------------------------------------------------------------------------
tOctetBlock::tOctetBlock(const risse_uint8 * buf, risse_size length)
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
tOctetBlock::tOctetBlock(risse_size length)
{
	Capacity = Length = length;
	if(length)
		Buffer = AllocateInternalBuffer(length);
	else
		Buffer = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOctetBlock::tOctetBlock(const tOctetBlock & ref,
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
bool tOctetBlock::operator < (const tOctetBlock & ref) const
{
	if(this == &ref) return false; // 同じポインタ

	// まずは、this と ref のどちらか短い方までの範囲で比較を行う
	size_t size_min = Length < ref.Length ? Length : ref.Length;
	int cmp_result = size_min == 0 ? 0 : ::memcmp(Buffer, ref.Buffer, size_min);
	if(cmp_result < 0) return true; // この時点で this<ref だったり this>ref だったら即帰る
	if(cmp_result > 0) return false;

	// この時点で、少なくとも size_min までは this == ref である
	if(Length == ref.Length) return false; // 内容が全く同じ場合
	if(Length > ref.Length) return false; // this の方が長い
	return true; // this の方が短い場合は this < ref である
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOctetBlock::Append(const risse_uint8 * buffer, risse_size length)
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

	Length = newlength;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOctetBlock & tOctetBlock::operator += (const tOctetBlock & ref)
{
	if(Length == 0) return *this = ref; // 単純なコピーでよい
	Append(ref.Buffer, ref.Length);
	return *this;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOctetBlock tOctetBlock::operator + (const tOctetBlock & ref) const
{
	tOctetBlock block;
	Concat(&block, ref);
	return block;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOctetBlock::Concat(tOctetBlock * dest, const tOctetBlock & ref) const
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
risse_uint8 * tOctetBlock::InternalIndepend() const
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
tString tOctetBlock::AsHumanReadable(risse_size maxlen) const
{
	tString ret;

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
		src ++;
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
