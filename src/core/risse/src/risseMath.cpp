//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Math クラス実装
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include "risseMath.h"
#include "math.h"
#include "time.h"

#ifdef __WIN32__
#ifndef RISSE_NO_MASK_MATHERR
	#include "float.h"
#endif
#endif

//---------------------------------------------------------------------------
// matherr and matherrl function
//---------------------------------------------------------------------------
// these functions invalidate the mathmarical error
// (other exceptions, like divide-by-zero error, are not to be caught here)
#if defined(__WIN32__) && !defined(__GNUC__)
#ifndef RISSE_NO_MASK_MATHERR
int _USERENTRY _matherr(struct _exception *e)
{
	return 1;
}
int _USERENTRY _matherrl(struct _exception *e)
{
	return 1;
}
#endif
#endif
//---------------------------------------------------------------------------



namespace Risse
{
RISSE_DEFINE_SOURCE_ID(1020);
//---------------------------------------------------------------------------
// tRisseNC_Math : risse Native Class : Math
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_Math::ClassID = (risse_uint32)-1;
tRisseNC_Math::tRisseNC_Math() :
	tRisseNativeClass(RISSE_WS("Math"))
{
	// constructor
	time_t time_num;
	time(&time_num);
	srand(time_num);

	/*
		Risse cannot promise that the sequence of generated random numbers are
		unique.
		Math.random uses old-style random generator from stdlib
		since Math.RandomGenerator provides Mersenne Twister high-quality random
		generator.
	*/

	RisseSetFPUE();

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/Math)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*Risse class name*/ Math)
{
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/Math)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/abs)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result=fabs(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/abs)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/acos)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = acos(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/acos)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/asin)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = asin(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/asin)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/atan)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = atan(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/atan)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/atan2)
{
	if(numparams<2) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = atan2(param[0]->AsReal(), param[1]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/atan2)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/ceil)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = ceil(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/ceil)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/exp)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = exp(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/exp)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/floor)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = floor(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/floor)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/log)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = log(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/log)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/pow)
{
	if(numparams<2) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = pow(param[0]->AsReal(), param[1]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/pow)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/max)
{
	if(numparams<2) return RISSE_E_BADPARAMCOUNT;

	RisseSetFPUE();
	tTVReal a = param[0]->AsReal();
	tTVReal b = param[1]->AsReal();

	if(result)
	{
		RisseSetFPUE();
		*result = std::max<tTVReal>(a,b);
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/max)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/min)
{
	if(numparams<2) return RISSE_E_BADPARAMCOUNT;

	RisseSetFPUE();

	tTVReal a = param[0]->AsReal();
	tTVReal b = param[1]->AsReal();

	if(result) *result = std::min<tTVReal>(a,b);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/min)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/random)
{
	if(result)
	{
		RisseSetFPUE();
		*result = ((tTVReal)((tTVReal)Risse_rand()/(tTVReal)(RISSE_RAND_MAX)));
	}
	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/random)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/round)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = ::floor(param[0]->AsReal() + 0.5f);
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/round)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/sin)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = ::sin(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/sin)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/cos)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = ::cos(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/cos)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/sqrt)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = ::sqrt(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/sqrt)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/tan)
{
	if(numparams<1) return RISSE_E_BADPARAMCOUNT;

	if(result)
	{
		RisseSetFPUE();
		*result = ::tan(param[0]->AsReal());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/tan)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(E)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_E;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(E)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(LOG2E)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_LOG2E;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(LOG2E)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(LOG10E)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_LOG10E;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(LOG10E)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(LN10)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_LN10;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(LN10)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(LN2)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_LN2;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(LN2)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(PI)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_PI;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(PI)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(SQRT1_2)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		*result = (M_SQRT2/2);
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(SQRT1_2)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(SQRT2)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_SQRT2;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(SQRT2)
//---------------------------------------------------------------------------

	RISSE_END_NATIVE_MEMBERS
} // tRisseNC_Math::tRisseNC_Math()
//---------------------------------------------------------------------------
}  // namespace Risse


