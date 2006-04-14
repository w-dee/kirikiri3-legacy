//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Risse GC インターフェース
//---------------------------------------------------------------------------

#ifndef __RisseGC_H__
#define __RisseGC_H__

#include <gc_cpp.h>
#include <gc_allocator.h>
#include <vector>
#include <deque>

namespace Risse
{
//---------------------------------------------------------------------------
typedef	gc			tRisseCollectee; //!< コレクタの対象となるクラスの基本クラス
typedef	gc_cleanup	tRisseDestructee; //!< コレクタの対象かつデストラクタが呼ばれるクラスの基本クラス

//! @brief	vector の gc 版
template <class T>
class gc_vector : public std::vector<T, gc_allocator<T> >, public gc { };

//! @brief	deque の gc 版
template <class T>
class gc_deque : public std::deque<T, gc_allocator<T> >, public gc { };
//---------------------------------------------------------------------------
} // namespace Risse
#endif

