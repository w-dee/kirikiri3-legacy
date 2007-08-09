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
#include "prec.h"

#include <gc.h>
#include "risseTypes.h"


namespace Risse
{
//---------------------------------------------------------------------------
void * AlignedMallocAtomicCollectee(size_t size, int align)
{
	// aligned memory allocation is to be used to gain performance on some processors.
	align = 1 << align;
	void *ptr = GC_malloc_atomic(size + align + sizeof(void*));
	void *org_ptr = ptr;
	risse_ptruint *iptr =
		reinterpret_cast<risse_ptruint *>(&ptr);
	*iptr += align + sizeof(void*);
	*iptr &= ~(risse_ptruint)(align - 1);
	(reinterpret_cast<void**>(ptr))[-1] = org_ptr;
	return ptr;
}
//---------------------------------------------------------------------------

} /* namespace Risse */

