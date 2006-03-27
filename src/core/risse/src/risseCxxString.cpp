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
#include "prec.h"

#include "risseCxxString.h"

RISSE_DEFINE_SOURCE_ID(45632,47818,10920,18335,63117,13582,59145,24628);

/*! @note
Risse 文字列について

Risse 文字列は tRisseStringBlock クラスで表される。
このクラスは、risse_char * 型の文字列を保持する。この文字列は \0 を含むこ
とはできない。

以下の説明ではコードポイントを「文字」と表記する。

■ 構造

	tRisseStringBlock は以下の二つのメンバを持っている。

	risse_char  *	Buffer;	// 文字列バッファ
	risse_size		Length;	// 文字列長 (最後の \0 は含めない)

	tRisseStringBlock はこの二つのメンバを用い、「Bufferから始まるLength分の
	長さを持つ文字列」を表す。
	長さのない (空の) 文字列の場合、Buffer には NULL が入り、Length は 0 になる。


■ バッファ

	バッファは 最低でも Length + 2 の長さ(risse_char 単位で)を持つ。
	tRisseStringBlock が任意の文字列を元に作成される場合、Length + 2 の長さ
	のバッファがまず確保され、文字列中の各文字は以下のように配置される。

	\0 文字0 文字1 文字3 .... \0

	このように、各 CP の両端に \0 がついたバッファとなる。

	tRisseStringBlock::Buffer は、最初の \0 ではなく、その次の文字0(つまり
	文字列の先頭) を指すようになる。

	先頭の \0 は Buffer が他の tRisseStringBlock と共有されている場合に \0 
	ではなくなる。最後の \0 は、終端が \0 であることを期待している C 言語系
	の関数に渡す際に NULL終端 を表す。

■ バッファの共有

	tRisseStringBlock のコピーコンストラクタや代入演算子は、バッファの中身は
	コピーせず、Buffer と Length だけをコピーする。この際、バッファがすでに
	共有されたことを表すため、Buffer[-1] が \0 でなければ、Buffer[-1] に
	-1 を代入する。これはバッファが共有されている可能性を表す。

■ 部分文字列の共有

	tRisseStringBlock は、他の文字列の一部を指し示すことができる。

	tRisseStringBlock a が以下のバッファを持っている場合、

	\0 文字0 文字1 文字2 文字3 \0
	    ↑
	   Buffer
	Length = 4

	文字1 ～ 文字2 の２文字を表す tRisseStringBlock b は以下のように表すこ
	とができる。

	-1 文字0 文字1 文字2 文字3 \0
	          ↑
	        Buffer
	Length = 2

	この場合もバッファの共有と同じく、バッファの先頭の \0 が -1 に書き換えら
	れ、バッファが共有されていることを表す。

■ バッファの共有の判定

	tRisseStringBlock が他の tRisseStringBlock とバッファを共有している可能
	性があるかどうかを判断するには、Buffer[-1] が \0 でないかどうかを見れば
	よい。Buffer[-1] が 非 \0 であれはバッファは共有されている可能性がある。

	tRisseStringBlock は文字列を共有する場合、Buffer[-1] が非 \0 の場合に
	 -1 を代入するが、これによりバッファが共有されている可能性を表す。

	tRisseStringBlock は部分文字列を共有する場合、母文字列は Buffer[-1] が
	非 \0の場合に -1 を代入するし、部分文字列は Buffer[-1] は必然的に \0 に
	なる。これは、部分文字列が母文字列の先頭から共有しているならば、Buffer[-1]
	は母文字列と同じ -1 になるし、部分文字列が母文字列の途中から共有している
	ならば、Buffer[-1] は直前の文字を表し、tRisseStringBlock は文字列中に
	\0 を含むことはないから、これは非 \0 となる。

	これらは共有の可能性を表すだけである。可能性があっても実際は共有されてい
	ない場合があり得る。

■ Independ

	Independ メソッドは、文字列がそのバッファを共有している可能性がある場合、
	新たにバッファを確保し、内容をコピーする。これにより、バッファに何か変
	更を書き込んでも、他の文字列に影響が及ばないようにすることができる。

	Independ は新たに確保したバッファの先頭は \0 にするが、元のバッファの
	内容には手を加えない。元のバッファの内容は、さらに他の文字列から共有
	されている可能性があるが、実際に共有されているのか、あるいはされていな
	いのかは、tRisseStringBlock が持っている情報からは判定できないからである。

■ c_str()

	c_str() は、多くの C 言語系 API が期待するような、NULL 終端文字列を返す。
	文字列が他の文字列の部分文字列を表している場合、文字列の最後が \0 である
	保証はないが、そのような場合は、c_str() は新たにバッファを確保し、最後が
	\0 で終了しているバッファを作り、それを返す。

■ バッファの解放

	参照されなくなったバッファは、GC により自動的に回収される。

*/

namespace risse
{

//---------------------------------------------------------------------------
//! @brief -1, 0 が入っている配列(空のバッファを表す)
//---------------------------------------------------------------------------
risse_char tRisseStringBlock::EmptyBuffer[2] = { tRisseStringBlock::MightBeShared, 0 };
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(risse_char * から)
//! @param		ref		元の文字列
//---------------------------------------------------------------------------
tRisseStringBlock::tRisseStringBlock(const risse_char * ref)
{
	if((Length = Risse_strlen(ref)) == 0)
	{
		Buffer = EmptyBuffer;
	}
	else
	{
		Buffer = AllocateInternalBuffer(Length);
		Buffer[-1] = 0;
		Risse_strcpy(Buffer, ref);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(risse_char * から、コードポイント数制限付き)
//! @param		ref		元の文字列
//! @param		n		コードポイント数
//! @note		[ref, ref+n) の範囲には \0 がないこと
//---------------------------------------------------------------------------
tRisseStringBlock::tRisseStringBlock(const risse_char * ref, risse_size n)
{
	Length = n;
	if(n == 0)
	{
		Buffer = EmptyBuffer;
	}
	else
	{
		Buffer = AllocateInternalBuffer(Length);
		Buffer[-1] = 0;
		Buffer[n] = 0;
		memcpy(Buffer, ref, sizeof(risse_char) * n);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(char * から)
//! @param		ref		元の文字列
//---------------------------------------------------------------------------
tRisseStringBlock::tRisseStringBlock(const char * ref)
{
	Length = RisseUtf8ToRisseCharString(ref, NULL); // コードポイント数を得る
//	if(Length == static_cast<risse_size>(-1L))
//		; /////////////////////////////////////////// TODO: 例外を投げる
	Buffer = AllocateInternalBuffer(Length);
	RisseUtf8ToRisseCharString(ref, Buffer);
	Buffer[Length] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列の連結
//! @param		ref		連結する文字列
//! @return		このオブジェクト
//---------------------------------------------------------------------------
tRisseStringBlock & tRisseStringBlock::opetator += (const tRisseStringBlock & ref)
{
	if(ref.Length == 0) return *this; // 追加するものなし
	if(Length == 0) return *this = ref; // 単純なコピーでよい

	risse_size newlength = Length + ref.Length;

	if(Buffer[-1])
	{
		// 共有可能性フラグが立っている
		// 新しく領域を確保し、そこにコピーする
		risse_char * newbuf = AllocateInternalBuffer(newlength);
		memcpy(newbuf, Buffer, Length * sizeof(risse_char));
		memcpy(newbuf + Length, ref.Buffer, ref.Length * sizeof(risse_char));
	}
	else
	{
		// 共有可能性フラグは立っていない
		// 現在の領域を拡張し、拡張した部分に ref の Buffer をコピーする
		Buffer = reinterpret_cast<risse_char*>(GC_realloc((newlength + 2)*sizeof(risse_char))) + 1;
		memcpy(Buffer + Length, ref.Buffer, ref.Length * sizeof(risse_char));
	}

	// null 終端を設定する
	Length = newlength;
	newbuf[newlength] = 0;

	return *this;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列の連結
//---------------------------------------------------------------------------
tRisseStringBlock tRisseStringBlock::opetator +  (const tRisseStringBlock & ref) const
{
	if(Length == 0) return ref;
	if(ref.Length == 0) return *this;

	tRisseStringBlock newblock;
	risse_size newsize = Length + ref.Length;
	newblock.Allocate(newsize);
	memcpy(newblock.Buffer, Buffer, Length * sizeof(risse_char));
	memcpy(newblock.Buffer + Length, ref.Buffer, ref.Length * sizeof(risse_char));

	return newblock;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列バッファをコピーし、独立させる
//! @return		内部バッファ
//---------------------------------------------------------------------------
risse_char * tRisseStringBlock::InternalIndepend() const
{
	risse_char * newbuf = AllocateInternalBuffer(Length);
	memcpy(newbuf, Buffer, sizeof(risse_char) * Length);
	newbuf[Length] = 0;
	newbuf[-1] = 0;
	return Buffer = newbuf;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace risse
