//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief オブジェクトの C++ インターフェースの実装
//---------------------------------------------------------------------------
#ifndef risseCxxObjectH
#define risseCxxObjectH

#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "gc_cpp.h"


namespace Risse
{
typedef void * tRisseObjectImpl;
//---------------------------------------------------------------------------
//! @brief	オブジェクト用ポインタ
//! @note
//! ポインタの最下位の2ビットが常に 01 なのは、このポインタが オブジェクトであることを
//! 表している。ポインタは常に少なくとも 32bit 境界に配置されるため、最下位の２ビットは
//! オブジェクトのタイプを表すのに利用されている。tRisseVariantを参照。
//---------------------------------------------------------------------------
class tRisseObjectPointer : public gc
{
	tRisseObjectImpl * Impl; //!< ブロックへのポインタ (最下位の2ビットは常に10なので注意)
							//!< アクセス時は必ず GetBlock, SetBlock を用いること

protected: // pointer operation
	void SetImpl(tRisseObjectImpl * impl)
		{ Impl = reinterpret_cast<tRisseObjectImpl*>(reinterpret_cast<risse_ptruint>(impl) + 1); }

	tRisseObjectImpl * GetImpl() const
		{ return reinterpret_cast<tRisseObjectImpl*>(reinterpret_cast<risse_ptruint>(Impl) - 1); }

public:
	//! @brief null ポインタ。オブジェクトが null を表す場合は、この値をとる。
	static const tRisseObjectImpl * NullPtr = reinterpret_cast<tRisseObjectImpl*>((risse_ptruint)0x10);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
class tRisseObjectBlock : public tRisseObjectPointer
{
public:
	// デフォルトコンストラクタ
	tRisseObjectPointer()
	{
		SetImpl(NullPtr);
	}
};
//---------------------------------------------------------------------------

typedef tRisseObjectBlock tRisseObject; //!< いまのところ tRisseObject は tRisseObjectBlock と同じ

//---------------------------------------------------------------------------
}
#endif

