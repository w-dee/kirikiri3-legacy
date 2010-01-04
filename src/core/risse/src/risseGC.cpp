//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse GC インターフェース
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseGC.h"
#include "risseTypes.h"


namespace Risse
{
//---------------------------------------------------------------------------
void * _AlignedMallocCollectee(size_t size, size_t align, bool atomic)
{
	// aligned memory allocation is to be used (or mandatory) to
	// gain performance on some processors.

	// アラインメントを変換する (例: align が 4 ならば 1<<4 = 16 (bytes) )
	align = 1 << align;

	// アラインメントのチェック
	// sizeof(void*) 未満 のアラインメントは行わない。
	if(align < sizeof(void*)) align = sizeof(void*);

	// メモリブロックを確保する。アラインメント分と、
	// オリジナルのメモリブロックのポインタを格納する為の
	// 領域もくわえた分を確保する。
	void *ptr = atomic ?
		GC_MALLOC_ATOMIC(size + align + sizeof(void*)) :
		GC_MALLOC       (size + align + sizeof(void*));
	void *org_ptr = ptr;

	// ptr の直前に、オリジナルのメモリブロックのポインタを格納する
	(reinterpret_cast<void**>(ptr))[-1] = org_ptr;

	// ptr を整数値としてアクセスするために、整数に変換する
	risse_ptruint iptr =
		reinterpret_cast<risse_ptruint>(ptr);

	// iptr にアラインメント分と sizeof(void*) を加算
	iptr += align + sizeof(void*);

	// iptr をアラインメントに合わせる
	// align-1 の not を iptr に and すると言うことはすなわち
	// iptr を超えない、もっとも大きなアラインメントされたアドレスに
	// iptr が調整されるということ
	iptr &= ~(risse_ptruint)(align - 1);

	// iptr をポインタに変換して返す
	return reinterpret_cast<void*>(iptr);
}
//---------------------------------------------------------------------------


} /* namespace Risse */

