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

#ifndef __GC_H__
#define __GC_H__

#include "risseAssert.h"

// マクロ NDEBUG が定義されていない場合 (デバッグビルドの場合)
// GC のデバッグ機能を有効にする
// (risseAssert.hにはNDEBUGに関する定義が入っている)
#ifndef NDEBUG
 #define GC_DEBUG
#endif

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
typedef	gc			tCollectee; //!< コレクタの対象となるクラスの基本クラス
typedef	gc_cleanup	tDestructee; //!< コレクタの対象かつデストラクタが呼ばれるクラスの基本クラス
//!< @note tDestructee のデストラクタは、
//!< コンストラクタとは異なるスレッドから呼ばれる可能性があることに注意。
//!< tDestructee 派生クラスはいろいろと注意点が多い。
//!< まず、循環参照を作るとデストラクタが呼ばれなくなるが、これは循環参照が
//!< 無いように、tCollecteeでの管理部分とtDestructeeの管理部分を分離すること
//!< で解決できる。
//!< tCollecteeの直接のメンバになることはできない。
//!< (tCollecteeがこのクラスのポインタを持つことは可能)。
//!< tDestructeeの直接のメンバになることは可能で、この場合はメンバ親と
//!< 生死をともにすることになる。


// http://www.al.cs.kobe-u.ac.jp/~inamoto/unix-tools/useful/programming/gc/x272.html

/**
 * vector の gc 版
 */
template <class T>
class gc_vector : public std::vector<T, gc_allocator<T> >, public gc { };

/**
 * list の gc 板
 */
template <class T>
class gc_list : public std::list<T, gc_allocator<T> >, public gc { };

/**
 * deque の gc 版
 */
template <class T>
class gc_deque : public std::deque<T, gc_allocator<T> >, public gc { };

/**
 * map の gc 版
 */
template <class T1, class T2, class COMP=std::less<T1> >
class gc_map : public std::map<T1, T2, COMP, gc_allocator<std::pair<T1, T2> > >, public gc { };


//---------------------------------------------------------------------------
/**
 * コレクタの対象となることができるメモリ領域を確保する
 * @param size	確保するサイズ
 * @return	確保されたメモリブロックへのポインタ
 * @note	MallocCollecteeAtomic と異なり、メモリ領域中は GC 中にポインタの
 *			スキャンの対象となる
 */
static inline void * MallocCollectee(size_t size)
{
	return GC_MALLOC(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * コレクタの対象となることができるメモリ領域を確保する
 * @param size	確保するサイズ
 * @return	確保されたメモリブロックへのポインタ
 * @note	MallocCollectee と異なり、メモリ領域中にはなんら有効なポインタが
 *			含まれていないと見なされる(atomicなメモリ領域を確保する)。
 *			メモリ領域中にポインタを含まないようなデータ
 *			の確保にはこっちの関数を使うこと。
 */
static inline void * MallocAtomicCollectee(size_t size)
{
	return GC_MALLOC_ATOMIC(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * コレクタの対象となることができるメモリ領域のサイズを変更する
 * @param old_block	変更したいメモリブロック
 * @param size		変更後のサイズ
 * @return	確保されたメモリブロックへのポインタ
 * @note	メモリブロックのサイズを変更しても、MallocCollecteeAtomic や
 *			MallocCollectee で確保したメモリの属性 (atomicかそうでないか)
 *			は保持される。
 */
static inline void * ReallocCollectee(void * old_block, size_t size)
{
	return GC_REALLOC(old_block, size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * コレクタの対象となることができるメモリ領域を強制的に開放する
 * @param block	開放したいメモリブロック
 */
static inline void FreeCollectee(void * block)
{
	GC_FREE(block);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * 指定バイトでアラインメントされた MallocAtomicCollectee
 * @param size		確保するサイズ
 * @param align		アラインメント
 * @param atomic	atomic な (pointer freeな) メモリ領域を確保するかどうか
 */
void * _AlignedMallocCollectee(size_t size, size_t align, bool atomic);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 指定バイトでアラインメントされた MallocCollectee
 * @param size	確保するサイズ
 * @param align	アラインメント
 *				(1<<align バイト境界にアラインメントされる; 4 ならば 1<<4 = 16 = 16バイト境界)
 */
static inline void * AlignedMallocCollectee(size_t size, size_t align)
{
	return _AlignedMallocCollectee(size, align, false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 指定バイトでアラインメントされた MallocAtomicCollectee
 * @param size	確保するサイズ
 * @param align	アラインメント
 *				(1<<align バイト境界にアラインメントされる; 4 ならば 1<<4 = 16 = 16バイト境界)
 */
static inline void * AlignedMallocAtomicCollectee(size_t size, size_t align)
{
	return _AlignedMallocCollectee(size, align, true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * AlignedMallocAtomicCollectee で確保されたメモリを強制的に解放する
 * @param ptr	解放するポインタ
 */
static inline void AlignedFreeCollectee(void * ptr)
{
	GC_FREE((static_cast<void**>(ptr))[-1]);
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * ポインタを含まない構造体など用の基本クラス(デストラクタは呼ばれない)
 */
class tAtomicCollectee
{
public:
	void * operator new(size_t n) { return MallocAtomicCollectee(n); }
	void * operator new [] (size_t n) { return MallocAtomicCollectee(n); }
	void * operator new(size_t n, void * p) { return p; }
	void operator delete(void *obj) { FreeCollectee(obj); }
	void operator delete [] (void *obj) { FreeCollectee(obj); }
	void operator delete(void *obj, void *) { }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * 強制的にガーベジを回収する
 */
static inline void CollectGarbage()
{
	GC_gcollect();
	GC_invoke_finalizers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

