//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 文字列のC++クラス
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseString.h"
#include "risseExceptionClass.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(45632,47818,10920,18335,63117,13582,59145,24628);


//---------------------------------------------------------------------------
risse_char tStringData::EmptyBuffer[3] = { tStringBlock::MightBeShared, 0, 0 };
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock::tStringBlock(const tStringBlock & ref,
	risse_size offset, risse_size length)
{
	if(length != 0 && offset < ref.Length)
	{
		// length には risse_size_max がわたってくることがあるので注意。
		// その場合は length+offset は折り返して小さな数になる可能性がある
		// ので、length + offset > ref.Length だけではなくて
		// length > ref.Length の条件もチェックする
		if(length > ref.Length || length + offset > ref.Length)
			length = ref.Length - offset;
		if(ref.Buffer[-1] == 0)
			ref.Buffer[-1] = MightBeShared; // 共有可能性フラグをたてる
		Buffer = ref.Buffer + offset;
		Length = length;
	}
	else
	{
		Buffer = RISSE_STRING_EMPTY_BUFFER;
		Length = 0;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock::tStringBlock(const risse_char * ref, risse_size n)
{
	Length = n;
	if(n == 0)
	{
		Buffer = RISSE_STRING_EMPTY_BUFFER;
	}
	else
	{
		Buffer = AllocateInternalBuffer(Length);
		Buffer[n] = Buffer[n+1] = 0; // null終端と hint をクリア
		memcpy(Buffer, ref, sizeof(risse_char) * n);
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tStringBlock::tStringBlock(const tStringBlock &msg, const tStringBlock &r1)
{
	*this = msg.Replace(RISSE_WS("%1"), r1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock::tStringBlock(const tStringBlock &msg, const tStringBlock &r1,
				const tStringBlock &r2)
{
	*this = msg.
			Replace(RISSE_WS("%1"), r1).
			Replace(RISSE_WS("%2"), r2);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock::tStringBlock(const tStringBlock &msg, const tStringBlock &r1,
				const tStringBlock &r2, const tStringBlock &r3)
{
	*this = msg.
			Replace(RISSE_WS("%1"), r1).
			Replace(RISSE_WS("%2"), r2).
			Replace(RISSE_WS("%3"), r3);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock::tStringBlock(const tStringBlock &msg, const tStringBlock &r1,
					const tStringBlock &r2, const tStringBlock &r3,
					const tStringBlock &r4)
{
	*this = msg.
			Replace(RISSE_WS("%1"), r1).
			Replace(RISSE_WS("%2"), r2).
			Replace(RISSE_WS("%3"), r3).
			Replace(RISSE_WS("%4"), r4);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock & tStringBlock::operator = (const risse_char * ref)
{
	if((Length = ::Risse::strlen(ref)) == 0)
	{
		Buffer = RISSE_STRING_EMPTY_BUFFER;
	}
	else
	{
		Buffer = AllocateInternalBuffer(Length);
		memcpy(Buffer, ref, Length * sizeof(risse_char));
			// サイズがわかっているならば memcpy の方が若干早い
//		::Risse::strcpy(Buffer, ref);
		Buffer[Length] = Buffer[Length+1] = 0; // null終端と hint をクリア
	}
	return *this;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock & tStringBlock::operator = (const risse_char ref)
{
	if(ref == 0)
	{
		Length = 0;
		Buffer = RISSE_STRING_EMPTY_BUFFER;
	}
	else
	{
		Length = 1;
		Buffer = AllocateInternalBuffer(1);
		Buffer[0] = ref;
		Buffer[1] = Buffer[1+1] = 0; // null終端と hint をクリア
	}
	return *this;
}
//---------------------------------------------------------------------------


#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
//---------------------------------------------------------------------------
tStringBlock & tStringBlock::operator = (const wchar_t *str)
{
	risse_size org_len = wcslen(str);
	Buffer = AllocateInternalBuffer(org_len);
	risse_size new_len = ConvertUTF16ToRisseCharString(Buffer,
		reinterpret_cast<const risse_uint16 *>(str)); // UTF16 を UTF32 に変換
	if(new_len)
		Buffer[new_len] = Buffer[new_len+1] = 0; // null終端と hint をクリア
	Length = new_len;
	return *this;
}
//---------------------------------------------------------------------------
#endif


//---------------------------------------------------------------------------
tStringBlock & tStringBlock::operator = (const char * ref)
{
	Length = Utf8ToRisseCharString(ref, NULL); // コードポイント数を得る
	if(Length == risse_size_max) tCharConversionExceptionClass::ThrowInvalidUTF8String();
	Buffer = AllocateInternalBuffer(Length);
	Utf8ToRisseCharString(ref, Buffer);
	Buffer[Length] = Buffer[Length + 1] = 0; // null終端と hint をクリア
	return *this;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_char * tStringBlock::AllocateInternalBuffer(
	risse_size n, risse_char *prevbuf)
{
	// バッファを確保
	size_t newbytes = sizeof(risse_size) + (n + 3)*sizeof(risse_char);
	void *ptr;
	if(!prevbuf)
	{
		ptr = MallocAtomicCollectee(newbytes);
	}
	else
	{
		char * buffer_head = reinterpret_cast<char *>(prevbuf) -
			 ( sizeof(risse_char) + sizeof(risse_size) );
		ptr = ReallocCollectee(buffer_head, newbytes);
	}

	// ２番目の文字を指すポインタを獲る
	risse_char *  buffer = reinterpret_cast<risse_char*>(
		reinterpret_cast<char*>(ptr) +
				( sizeof(risse_char) + sizeof(risse_size) ) );

	// 共有可能性フラグを 0 に
	buffer[-1] = 0;

	// 確保容量を書き込む
	*reinterpret_cast<risse_size *>(ptr) = n;

	// もどる
	return buffer;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_char * tStringBlock::InternalIndepend() const
{
	risse_char * newbuf = AllocateInternalBuffer(Length);
	memcpy(newbuf, Buffer, sizeof(risse_char) * Length);
	newbuf[Length] = newbuf[Length + 1] = 0; // null終端とhintをクリア
	return Buffer = newbuf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStringBlock::Reserve(risse_size capacity) const
{
	if(capacity < Length) return; // 長さが容量より長い

	if(Buffer[-1])
	{
		// 共有可能性フラグが立っている
		// 新しく領域を確保し、そこにコピーする
		risse_char * newbuf = AllocateInternalBuffer(capacity);
		memcpy(newbuf, Buffer, Length * sizeof(risse_char));
		Buffer = newbuf;

		// null 終端と hint=0 を設定する
		Buffer[Length] = Buffer[Length + 1] = 0;
	}
	else
	{
		// 共有可能性フラグは立っていない
		// 現在の領域を拡張する必要がある？
		if(GetBufferCapacity(Buffer) < capacity)
		{
			// 容量が足りないので拡張する必要あり
			// 適当に新規確保の容量を計算
			// バッファを再確保
			Buffer = AllocateInternalBuffer(capacity, Buffer);

			// null 終端と hint=0 を設定する
			Buffer[Length] = Buffer[Length + 1] = 0;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tStringBlock::GetHash() const
{
	// the hash function used here is similar to one which used in perl 5.8,
	// see also http://burtleburtle.net/bob/hash/doobs.html (One-at-a-Time Hash)

	const risse_char *p = Buffer;
	const risse_char *plim = Buffer + Length;
	risse_uint32 ret = 0;
	while(p<plim)
	{
		ret += *p;
		ret += (ret << 10);
		ret ^= (ret >> 6);
		p++;
	}
	ret += (ret << 3);
	ret ^= (ret >> 11);
	ret += (ret << 15);
	if(!ret) ret = (risse_uint32)-1L;
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tStringBlock::operator <  (const tStringBlock & ref) const
{
	// どちらのコードポイント数が小さい？
	if(Length < ref.Length)
	{
		// this の方が短い
		// Length まで比較したとき、this の方が小さい、あるいは
		// 同一だったばあいは、this の方が「小さい」と見なすことができる
		return strbufcmp(Buffer, ref.Buffer, Length) <= 0;
	}
	else if(Length > ref.Length)
	{
		// ref の方が短い
		// ref.Length まで比較したとき、this の方が小さい場合のみ「小さい」と見なすことが
		// できる(そこまで比較して同一だった場合は、ref の方が長いのでrefが「大きい」)
		return strbufcmp(Buffer, ref.Buffer, ref.Length) < 0;
	}
	else
	{
		// this の長さと ref の長さが一緒の場合
		// ref.Length まで比較したとき、this の方が小さい場合のみ「小さい」と見なすことが
		// できる(同一だった場合は両方とも同一の文字列である)
		return strbufcmp(Buffer, ref.Buffer, Length) < 0;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tStringBlock::operator == ( const risse_char *ptr ) const
{
	if(Length == 0) return ptr[0] == 0; // 空文字列 ?

	// ptr は null 終結していることを仮定できるが、
	// Buffer は null 終結している保証はない。
	// そのため、一応先頭から順に文字が一致しているかを一つ一つ見ていく。
	// memcmpでもいいのかもしれないがmemcmp(a, b, l) で [a, a+l) と [b, b+l)
	// のどちらかのその一部がアクセスできなかった場合の動作は未定義な
	// 気がするので使用を避ける。
	risse_size i;
	for(i = 0; i < Length; i++)
	{
		if(Buffer[i] != ptr[i])
			return false; // 異なる(Length分検査しないうちに*ptr_p==0に達した場合もここに来るはず)
	}

	// この時点では少なくとも Length 分は ptr と Buffer の中身が一致している
	// 最後に ptr[Length] が \0 かをチェック (そこが \0 ならば完全一致となる)
	return ptr[Length] == 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStringBlock::Append(const risse_char * buffer, risse_size length)
{
	if(length == 0) return; // 追加するものなし

	risse_size newlength = Length + length;

	if(Buffer[-1])
	{
		// 共有可能性フラグが立っている
		// 新しく領域を確保し、そこにコピーする
		risse_char * newbuf = AllocateInternalBuffer(newlength);
		memcpy(newbuf, Buffer, Length * sizeof(risse_char));
		memcpy(newbuf + Length, buffer, length * sizeof(risse_char));
		Buffer = newbuf;
	}
	else
	{
		// 共有可能性フラグは立っていない
		// 現在の領域を拡張する必要がある？
		if(GetBufferCapacity(Buffer) < newlength)
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
		}

		// 現在保持している文字列の直後に buffer をコピーする
		memcpy(Buffer + Length, buffer, length * sizeof(risse_char));
	}

	// null 終端と hint=0 を設定する
	Length = newlength;
	Buffer[newlength] = Buffer[newlength + 1] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock tStringBlock::operator + (const tStringBlock & ref) const
{
	if(Length == 0) return ref;
	if(ref.Length == 0) return *this;

	tStringBlock newblock;
	risse_size newsize = Length + ref.Length;
	newblock.Allocate(newsize);
	memcpy(newblock.Buffer, Buffer, Length * sizeof(risse_char));
	memcpy(newblock.Buffer + Length, ref.Buffer, ref.Length * sizeof(risse_char));

	return newblock;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock tStringBlock::operator + (const risse_char * ref) const
{
	if(Length == 0) return ref;
	if(ref == NULL) return *this;
	risse_size ref_length = ::Risse::strlen(ref);
	if(ref_length == 0) return *this;

	tStringBlock newblock;
	risse_size newsize = Length + ref_length;
	newblock.Allocate(newsize);
	memcpy(newblock.Buffer, Buffer, Length * sizeof(risse_char));
	memcpy(newblock.Buffer + Length, ref, ref_length * sizeof(risse_char));

	return newblock;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock tStringBlock::AsString(risse_int v)
{
	risse_char num_str[40];
	::Risse::int_to_str(v, num_str);
	return tStringBlock(num_str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock tStringBlock::AsString(risse_int64 v)
{
	risse_char num_str[40];
	::Risse::int64_to_str(v, num_str);
	return tStringBlock(num_str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock tStringBlock::Replace(const tStringBlock &old_str,
		const tStringBlock &new_str, bool replace_all) const
{
	// 長さチェック
	if(GetLength() < old_str.GetLength()) return *this; // 置き換えられない

	// old_str が最低1個分 new_str に変わると期待してバッファをあらかじめ確保
	tStringBlock ret;
	ret.Reserve(GetLength() - old_str.GetLength() + new_str.GetLength());

	// 置き換え
	const risse_char * this_c_str = c_str();
	const risse_char * old_c_str = old_str.c_str();
	const risse_char * new_c_str = new_str.c_str();
	const risse_char * lp = this_c_str;
	for(;;)
	{
		const risse_char *p;
		p = ::Risse::strstr(lp, old_c_str);
		if(p)
		{
			ret.Append(lp, p - lp);
			ret.Append(new_c_str, new_str.GetLength());
			if(!replace_all) break;
			lp = p + old_str.GetLength();
		}
		else
		{
			break;
		}
	}

	ret.Append(lp, GetLength() - (lp - this_c_str));

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock tStringBlock::Times(risse_size count)
{
	tStringBlock ret;

	// バッファを確保
	risse_size target_length = count * Length;
	risse_char * buffer = ret.Allocate(target_length);

	// 繰り返しを生成する
	if(target_length > 0)
	{
		for(risse_size i = 0; i < count; i++)
		{
			memcpy(buffer, Buffer, sizeof(risse_char) * Length);
			buffer += Length;
		}
	}

	// 戻る
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringBlock tStringBlock::Escape(risse_size maxlen, bool quote) const
{
	const risse_char * hexchars = RISSE_WS("0123456789ABCDEF");

	// 返値用のバッファを確保
	tStringBlock ret;
	ret.Reserve((maxlen > Length ? Length : maxlen) + 4 + (quote?2:0));
		// 最低でも今の文字列長以上にはなる (+4=余裕)

	// エスケープを行う。
	// \x01 のようなエスケープの後に 'a' のような、16進数の一部として間違
	// われるような文字が続く場合は、その 'a' も \x の形でエスケープする。
	// (その状態の制御を行っている変数が hexflag)
	const risse_char * p = Buffer;
	bool hexflag = false;
	if(quote) ret += RISSE_WC('"');
	for(risse_size i = 0; i < Length; i++)
	{
		if(ret.GetLength() >= maxlen)
		{
			// 最大長に達した
			if(quote) ret.Append(RISSE_WS(" ..."), 4);
			return ret;  //--------------------------------------------- return
		}

		switch(p[i])
		{
		case 0x07: ret.Append(RISSE_WS("\\a"), 2); hexflag = false; continue;
		case 0x08: ret.Append(RISSE_WS("\\b"), 2); hexflag = false; continue;
		case 0x0c: ret.Append(RISSE_WS("\\f"), 2); hexflag = false; continue;
		case 0x0a: ret.Append(RISSE_WS("\\n"), 2); hexflag = false; continue;
		case 0x0d: ret.Append(RISSE_WS("\\r"), 2); hexflag = false; continue;
		case 0x09: ret.Append(RISSE_WS("\\t"), 2); hexflag = false; continue;
		case 0x0b: ret.Append(RISSE_WS("\\v"), 2); hexflag = false; continue;
		case RISSE_WC('\\'): ret.Append(RISSE_WS("\\\\"), 2); hexflag = false; continue;
		case RISSE_WC('\''): ret.Append(RISSE_WS("\\\'"), 2); hexflag = false; continue;
		case RISSE_WC('\"'): ret.Append(RISSE_WS("\\\""), 2); hexflag = false; continue;
		default:
			if(hexflag)
			{
				if(p[i] >= RISSE_WC('a') && p[i] <= RISSE_WC('f') ||
					p[i] >= RISSE_WC('A') && p[i] <= RISSE_WC('F') ||
						p[i] >= RISSE_WC('0') && p[i] <= RISSE_WC('9') )
				{
					risse_char buf[4];
					buf[0] = RISSE_WC('\\');
					buf[1] = RISSE_WC('x');
					buf[2] = hexchars[ (p[i] >> 4)  & 0x0f];
					buf[3] = hexchars[ (p[i]     )  & 0x0f];
					hexflag = true;
					ret.Append(buf, 4);
					continue;
				}
			}

			if(p[i] < 0x20)
			{
				risse_char buf[4];
				buf[0] = RISSE_WC('\\');
				buf[1] = RISSE_WC('x');
				buf[2] = hexchars[ (p[i] >> 4)  & 0x0f];
				buf[3] = hexchars[ (p[i]     )  & 0x0f];
				buf[4] = 0;
				hexflag = true;
				ret.Append(buf, 4);
			}
			else
			{
				ret += p[i];
				hexflag = false;
			}
		}
	}
	if(quote) ret += RISSE_WC('"');
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStringData tStringBlock::EmptyStringData = { RISSE_STRING_EMPTY_BUFFER, 0 };
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tStringBlock operator +(const risse_char *lhs, const tStringBlock &rhs)
{
	risse_size lhs_length = ::Risse::strlen(lhs);
	if(lhs == NULL) return rhs;
	if(lhs_length == 0) return rhs;

	tStringBlock newblock;
	risse_size newsize = lhs_length + rhs.Length;
	newblock.Allocate(newsize);
	memcpy(newblock.Buffer, lhs, lhs_length * sizeof(risse_char));
	memcpy(newblock.Buffer + lhs_length, rhs.Buffer, rhs.Length * sizeof(risse_char));

	return newblock;
}
//---------------------------------------------------------------------------



} // namespace Risse
