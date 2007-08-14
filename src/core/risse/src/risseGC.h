//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse GC インターフェース
//---------------------------------------------------------------------------

#ifndef __GC_H__
#define __GC_H__

//#define GC_DEBUG
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
//! @note	MallocCollecteeAtomic と異なり、メモリ領域中は GC 中にポインタの
//!			スキャンの対象となる
//---------------------------------------------------------------------------
static inline void * MallocCollectee(size_t size)
{
	return GC_MALLOC(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コレクタの対象となることができるメモリ領域を確保する
//! @param	size  確保するサイズ
//! @return	確保されたメモリブロックへのポインタ
//! @note	MallocCollectee と異なり、メモリ領域中にはなんら有効なポインタが
//!			含まれていないと見なされる(atomicなメモリ領域を確保する)。
//!			メモリ領域中にポインタを含まないようなデータ
//!			の確保にはこっちの関数を使うこと。
//---------------------------------------------------------------------------
static inline void * MallocAtomicCollectee(size_t size)
{
	return GC_MALLOC_ATOMIC(size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コレクタの対象となることができるメモリ領域のサイズを変更する
//! @param	old_block	変更したいメモリブロック
//! @param	size		変更後のサイズ
//! @return	確保されたメモリブロックへのポインタ
//! @note	メモリブロックのサイズを変更しても、MallocCollecteeAtomic や
//!			MallocCollectee で確保したメモリの属性 (atomicかそうでないか)
//!			は保持される。
//---------------------------------------------------------------------------
static inline void * ReallocCollectee(void * old_block, size_t size)
{
	return GC_REALLOC(old_block, size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	コレクタの対象となることができるメモリ領域を強制的に開放する
//! @param	block	開放したいメモリブロック
//---------------------------------------------------------------------------
static inline void FreeCollectee(void * block)
{
	GC_FREE(block);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief	指定バイトでアラインメントされた MallocAtomicCollectee
//! @param	size	確保するサイズ
//! @param	align	アラインメント
//! @param	atomic	atomic な (pointer freeな) メモリ領域を確保するかどうか
//---------------------------------------------------------------------------
void * _AlignedMallocCollectee(size_t size, size_t align, bool atomic);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	指定バイトでアラインメントされた MallocCollectee
//! @param	size	確保するサイズ
//! @param	align	アラインメント
//!					(1<<align バイト境界にアラインメントされる; 4 ならば 1<<4 = 16 = 16バイト境界)
//---------------------------------------------------------------------------
static inline void * AlignedMallocCollectee(size_t size, size_t align)
{
	return _AlignedMallocCollectee(size, align, false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	指定バイトでアラインメントされた MallocAtomicCollectee
//! @param	size	確保するサイズ
//! @param	align	アラインメント
//!					(1<<align バイト境界にアラインメントされる; 4 ならば 1<<4 = 16 = 16バイト境界)
//---------------------------------------------------------------------------
static inline void * AlignedMallocAtomicCollectee(size_t size, size_t align)
{
	return _AlignedMallocCollectee(size, align, true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	AlignedMallocAtomicCollectee で確保されたメモリを強制的に解放する
//! @param	ptr		解放するポインタ
//---------------------------------------------------------------------------
static inline void AlignedFreeCollectee(void * ptr)
{
	GC_FREE((reinterpret_cast<void**>(ptr))[-1]);
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief	ポインタを含まない構造体など用の基本クラス(デストラクタは呼ばれない)
//---------------------------------------------------------------------------
class tAtomicCollectee
{
public:
	void * operator new(size_t n) { return MallocAtomicCollectee(n); }
	void * operator new [] (size_t n) { return MallocAtomicCollectee(n); }
	void operator delete(void *obj) { FreeCollectee(obj); }
	void operator delete [] (void *obj) { FreeCollectee(obj); }
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
#endif

