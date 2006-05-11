//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse GC インターフェース
//---------------------------------------------------------------------------

#ifndef __RisseGC_H__
#define __RisseGC_H__

#include <gc.h>
#include <gc_cpp.h>
#include <gc_allocator.h>
#include <vector>
#include <deque>
#include <list>
#include <map>

/*! @note
	現状、Risse は Boehm GC を用いている。
*/


namespace Risse
{
//---------------------------------------------------------------------------
typedef	gc			tRisseCollectee; //!< コレクタの対象となるクラスの基本クラス
typedef	gc_cleanup	tRisseDestructee; //!< コレクタの対象かつデストラクタが呼ばれるクラスの基本クラス

// http://www.al.cs.kobe-u.ac.jp/~inamoto/unix-tools/useful/programming/gc/x272.html

//! @brief	vector の gc 版
template <class T>
class gc_vector : public std::vector<T, gc_allocator<T> >, public gc { };

//! @brief	list の gc 板
template <class T>
class gc_list : public std::list<T, gc_allocator<T> >, public gc { };

//! @brief	deque の gc 版
template <class T>
class gc_deque : public std::deque<T, gc_allocator<T> >, public gc { };

//! @brief	map の gc 版
template <class T1, class T2>
class gc_map : public std::map<T1, T2, std::less<T1>, gc_allocator<std::pair<T1, T2> > >, public gc { };


//---------------------------------------------------------------------------
//! @brief	コレクタの対象となることができるメモリ領域を確保する
//! @param	size  確保するサイズ
//! @return	確保されたメモリブロックへのポインタ
//! @note	RisseMallocCollecteeAtomic と異なり、メモリ領域中は GC 中にポインタの
//!			スキャンの対象となる
//---------------------------------------------------------------------------
static inline void * RisseMallocCollectee(size_t size)
{
	return GC_malloc_atomic(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コレクタの対象となることができるメモリ領域を確保する
//! @param	size  確保するサイズ
//! @return	確保されたメモリブロックへのポインタ
//! @note	RisseMallocCollectee と異なり、メモリ領域中にはなんら有効なポインタが
//!			含まれていないと見なされる(atomicなメモリ領域を確保する)。
//!			メモリ領域中にポインタを含まないようなデータ
//!			の確保にはこっちの関数を使うこと。
//---------------------------------------------------------------------------
static inline void * RisseMallocAtomicCollectee(size_t size)
{
	return GC_malloc(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コレクタの対象となることができるメモリ領域のサイズを変更する
//! @param	old_block	変更したいメモリブロック
//! @param	size		変更後のサイズ
//! @return	確保されたメモリブロックへのポインタ
//! @note	メモリブロックのサイズを変更しても、RisseMallocCollecteeAtomic や
//!			RisseMallocCollectee で確保したメモリの属性 (atomicかそうでないか)
//!			は保持される。
//---------------------------------------------------------------------------
static inline void * RisseReallocCollectee(void * old_block, size_t size)
{
	return GC_realloc(old_block, size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コレクタの対象となることができるメモリ領域を強制的に開放する
//! @param	block	開放したいメモリブロック
//---------------------------------------------------------------------------
static inline void RisseFreeCollectee(void * block)
{
	GC_free(block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

