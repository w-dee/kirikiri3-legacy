/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse で使う型などの宣言  
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"

namespace Risse
{
//RISSE_DEFINE_SOURCE_ID(48985,38068,5574,18363,4752,19370,64892,64833);


//---------------------------------------------------------------------------
/*
 some sanity checking
*/
template<bool x> struct tRisseStaticAsserter;
template<> struct tRisseStaticAsserter<true>  { int hoge; };

// risse_int の有効ビット数は32bit符号付き？
enum { RisseIntSizeAsserter = sizeof (tRisseStaticAsserter<
	std::numeric_limits<risse_int>::is_signed && 
	std::numeric_limits<risse_int>::digits >= 31 > ) };

// char の有効ビット数は8bit符号付き？
enum { RisseCharSizeAsserter = sizeof (tRisseStaticAsserter<
	std::numeric_limits<char>::is_signed && 
	std::numeric_limits<char>::digits == 7 > ) };

// unsigned char の有効ビット数は8bit符号なし？
enum { RisseUCharSizeAsserter = sizeof (tRisseStaticAsserter<
	!std::numeric_limits<unsigned char>::is_signed && 
	std::numeric_limits<unsigned char>::digits == 8 > ) };
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
risse_uint32 RisseGetFPClass(risse_real r)
{
	risse_uint64 *ui64 = (risse_uint64*)&r;

	if(RISSE_IEEE_D_IS_NaN(*ui64))
	{
		if(RISSE_IEEE_D_SIGN_MASK & *ui64)
			return RISSE_FC_CLASS_NAN | RISSE_FC_SIGN_MASK;
		else
			return RISSE_FC_CLASS_NAN;
	}
	if(RISSE_IEEE_D_IS_INF(*ui64))
	{
		if(RISSE_IEEE_D_SIGN_MASK & *ui64)
			return RISSE_FC_CLASS_INF | RISSE_FC_SIGN_MASK;
		else
			return RISSE_FC_CLASS_INF;
	}
	if(RISSE_IEEE_D_SIGN_MASK & *ui64)
		return RISSE_FC_CLASS_NORMAL | RISSE_FC_SIGN_MASK;
	else
		return RISSE_FC_CLASS_NORMAL;
}
//---------------------------------------------------------------------------

} /* namespace Risse */

