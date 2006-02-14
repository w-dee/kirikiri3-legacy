//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief VMの実装
//---------------------------------------------------------------------------

#include "risseCommHead.h"

#include "risseInterCodeExec.h"
#include "risseInterCodeGen.h"
#include "risseScriptBlock.h"
#include "risseError.h"
#include "risse.h"
#include "risseUtils.h"
#include "risseNative.h"
#include "risseArray.h"
#include "risseDebug.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(11214,39710,29720,19182,10390,2872,46583,57802);
//---------------------------------------------------------------------------
// utility functions
//---------------------------------------------------------------------------
static void ThrowFrom_risse_error_num(risse_error hr, risse_int num)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	RisseThrowFrom_risse_error(hr, buf);
}
//---------------------------------------------------------------------------
static void ThrowInvalidVMCode()
{
	Risse_eRisseError(RisseInvalidOpecode);
}
//---------------------------------------------------------------------------
static void GetStringProperty(tRisseVariant *result, const tRisseVariant *str,
	const tRisseVariant &member)
{
	// processes properties toward strings.
	if(member.Type() != tvtInteger && member.Type() != tvtReal)
	{
		const risse_char *name = member.GetString();
		if(!name) RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, RISSE_WS(""));

		if(!Risse_strcmp(name, RISSE_WS("length")))
		{
			// get string length
			const tRisseVariantString * s = str->AsStringNoAddRef();
#ifdef __CODEGUARD__
			if(!s)
				*result = tTVInteger(0); // tRisseVariantString::GetLength can return zero if 'this' is NULL
			else
#endif
			*result = tTVInteger(s->GetLength());
			return;
		}
		else if(name[0] >= RISSE_WC('0') && name[0] <= RISSE_WC('9'))
		{
			const tRisseVariantString * valstr = str->AsStringNoAddRef();
			const risse_char *s = str->GetString();
			risse_int n = Risse_atoi(name);
			risse_int len = valstr->GetLength();
			if(n == len) { *result = tRisseVariant(RISSE_WS("")); return; }
			if(n<0 || n>len)
				Risse_eRisseError(RisseRangeError);
			risse_char bf[2];
			bf[1] = 0;
			bf[0] = s[n];
			*result = tRisseVariant(bf);
			return;
		}

		RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, name);
	}
	else // member.Type() == tvtInteger || member.Type() == tvtReal
	{
		const tRisseVariantString * valstr = str->AsStringNoAddRef();
		const risse_char *s = str->GetString();
		risse_int n = (risse_int)member.AsInteger();
		risse_int len = valstr->GetLength();
		if(n == len) { *result = tRisseVariant(RISSE_WS("")); return; }
		if(n<0 || n>len)
			Risse_eRisseError(RisseRangeError);
		risse_char bf[2];
		bf[1] = 0;
		bf[0] = s[n];
		*result = tRisseVariant(bf);
		return;
	}
}
//---------------------------------------------------------------------------
static void SetStringProperty(tRisseVariant *param, const tRisseVariant *str,
	const tRisseVariant &member)
{
	// processes properties toward strings.
	if(member.Type() != tvtInteger && member.Type() != tvtReal)
	{
		const risse_char *name = member.GetString();
		if(!name) RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, RISSE_WS(""));

		if(!Risse_strcmp(name, RISSE_WS("length")))
		{
			RisseThrowFrom_risse_error(RISSE_E_ACCESSDENYED, RISSE_WS(""));
		}
		else if(name[0] >= RISSE_WC('0') && name[0] <= RISSE_WC('9'))
		{
			RisseThrowFrom_risse_error(RISSE_E_ACCESSDENYED, RISSE_WS(""));
		}

		RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, name);
	}
	else // member.Type() == tvtInteger || member.Type() == tvtReal
	{
		RisseThrowFrom_risse_error(RISSE_E_ACCESSDENYED, RISSE_WS(""));
	}
}
//---------------------------------------------------------------------------
static void GetOctetProperty(tRisseVariant *result, const tRisseVariant *octet,
	const tRisseVariant &member)
{
	// processes properties toward octets.
	if(member.Type() != tvtInteger && member.Type() != tvtReal)
	{
		const risse_char *name = member.GetString();
		if(!name) RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, RISSE_WS(""));

		if(!Risse_strcmp(name, RISSE_WS("length")))
		{
			// get string length
			tRisseVariantOctet *o = octet->AsOctetNoAddRef();
			if(o)
				*result = tTVInteger(o->GetLength());
			else
				*result = tTVInteger(0);
			return;
		}
		else if(name[0] >= RISSE_WC('0') && name[0] <= RISSE_WC('9'))
		{
			tRisseVariantOctet *o = octet->AsOctetNoAddRef();
			risse_int n = Risse_atoi(name);
			risse_int len = o?o->GetLength():0;
			if(n<0 || n>=len)
				Risse_eRisseError(RisseRangeError);
			*result = tTVInteger(o->GetData()[n]);
			return;
		}

		RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, name);
	}
	else // member.Type() == tvtInteger || member.Type() == tvtReal
	{
		tRisseVariantOctet *o = octet->AsOctetNoAddRef();
		risse_int n = (risse_int)member.AsInteger();
		risse_int len = o?o->GetLength():0;
		if(n<0 || n>=len)
			Risse_eRisseError(RisseRangeError);
		*result = tTVInteger(o->GetData()[n]);
		return;
	}
}
//---------------------------------------------------------------------------
static void SetOctetProperty(tRisseVariant *param, const tRisseVariant *octet,
	const tRisseVariant &member)
{
	// processes properties toward octets.
	if(member.Type() != tvtInteger && member.Type() != tvtReal)
	{
		const risse_char *name = member.GetString();
		if(!name) RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, RISSE_WS(""));

		if(!Risse_strcmp(name, RISSE_WS("length")))
		{
			RisseThrowFrom_risse_error(RISSE_E_ACCESSDENYED, RISSE_WS(""));
		}
		else if(name[0] >= RISSE_WC('0') && name[0] <= RISSE_WC('9'))
		{
			RisseThrowFrom_risse_error(RISSE_E_ACCESSDENYED, RISSE_WS(""));
		}

		RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, name);
	}
	else // member.Type() == tvtInteger || member.Type() == tvtReal
	{
		RisseThrowFrom_risse_error(RISSE_E_ACCESSDENYED, RISSE_WS(""));
	}
}

//---------------------------------------------------------------------------
// tRisseObjectProxy
//---------------------------------------------------------------------------
class tRisseObjectProxy : public iRisseDispatch2
{
/*
	a class that do:
	1. first access to the Dispatch1
	2. if failed, then access to the Dispatch2
*/
//	risse_uint RefCount;

public:
	tRisseObjectProxy()
	{
//		RefCount = 1;
//		Dispatch1 = NULL;
//		Dispatch2 = NULL;
		// Dispatch1 and Dispatch2 are to be set by subsequent call of SetObjects
	};

	virtual ~tRisseObjectProxy()
	{
		if(Dispatch1) Dispatch1->Release();
		if(Dispatch2) Dispatch2->Release();
	};

	void SetObjects(iRisseDispatch2 *dsp1, iRisseDispatch2 *dsp2)
	{
		Dispatch1 = dsp1;
		Dispatch2 = dsp2;
		if(dsp1) dsp1->AddRef();
		if(dsp2) dsp2->AddRef();
	}

private:
	iRisseDispatch2 *Dispatch1;
	iRisseDispatch2 *Dispatch2;

public:

	risse_uint AddRef(void)
	{
		return 1;
//		return ++RefCount;
	}

	risse_uint Release(void)
	{
		return 1;
/*
		if(RefCount == 1)
		{
			delete this;
			return 0;
		}
		else
		{
			RefCount--;
		}
		return RefCount;
*/
	}

//--
#define OBJ1 ((objthis)?(objthis):(Dispatch1))
#define OBJ2 ((objthis)?(objthis):(Dispatch2))

	risse_error
	FuncCall(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->FuncCall(flag, membername, hint, result, numparams, param, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->FuncCall(flag, membername, hint, result, numparams, param, OBJ2);
		return hr;
	}

	risse_error
	FuncCallByNum(risse_uint32 flag, risse_int num, tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->FuncCallByNum(flag, num, result, numparams, param, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->FuncCallByNum(flag, num, result, numparams, param, OBJ2);
		return hr;
	}

	risse_error
	PropGet(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	tRisseVariant *result,
		iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->PropGet(flag, membername, hint, result, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->PropGet(flag, membername, hint, result, OBJ2);
		return hr;
	}

	risse_error
	PropGetByNum(risse_uint32 flag, risse_int num, tRisseVariant *result,
		iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->PropGetByNum(flag, num, result, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->PropGetByNum(flag, num, result, OBJ2);
		return hr;
	}

	risse_error
	PropSet(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	const tRisseVariant *param,
		iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->PropSet(flag, membername, hint, param, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->PropSet(flag, membername, hint, param, OBJ2);
		return hr;
	}

	risse_error
	PropSetByNum(risse_uint32 flag, risse_int num, const tRisseVariant *param,
		iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->PropSetByNum(flag, num, param, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->PropSetByNum(flag, num, param, OBJ2);
		return hr;
	}

	risse_error
	GetCount(risse_int *result, const risse_char *membername, risse_uint32 *hint,
	iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->GetCount(result, membername, hint, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->GetCount(result, membername, hint, OBJ2);
		return hr;
	}

	risse_error
	GetCountByNum(risse_int *result, risse_int num, iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->GetCountByNum(result, num, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->GetCountByNum(result, num, OBJ2);
		return hr;
	}

	risse_error
	PropSetByVS(risse_uint32 flag, tRisseVariantString *membername,
		const tRisseVariant *param, iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->PropSetByVS(flag, membername, param, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->PropSetByVS(flag, membername, param, OBJ2);
		return hr;
	}

	risse_error
	EnumMembers(risse_uint32 flag, tRisseVariantClosure *callback, iRisseDispatch2 *objthis)
	{
		return RISSE_E_NOTIMPL;
	}

	risse_error
	DeleteMember(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->DeleteMember(flag, membername, hint, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->DeleteMember(flag, membername, hint, OBJ2);
		return hr;
	}

	risse_error
	DeleteMemberByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->DeleteMemberByNum(flag, num, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->DeleteMemberByNum(flag, num, OBJ2);
		return hr;
	}

	risse_error
	Invalidate(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->Invalidate(flag, membername, hint, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->Invalidate(flag, membername, hint, OBJ2);
		return hr;
	}

	risse_error
	InvalidateByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->InvalidateByNum(flag, num, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->InvalidateByNum(flag, num, OBJ2);
		return hr;
	}

	risse_error
	IsValid(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->IsValid(flag, membername, hint, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->IsValid(flag, membername, hint, OBJ2);
		return hr;
	}

	risse_error
	IsValidByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->IsValidByNum(flag, num, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->IsValidByNum(flag, num, OBJ2);
		return hr;
	}

	risse_error
	CreateNew(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	iRisseDispatch2 **result,
		risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->CreateNew(flag, membername, hint, result, numparams, param, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->CreateNew(flag, membername, hint, result, numparams, param, OBJ2);
		return hr;
	}

	risse_error
	CreateNewByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 **result,
		risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->CreateNewByNum(flag, num, result, numparams, param, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->CreateNewByNum(flag, num, result, numparams, param, OBJ2);
		return hr;
	}

	risse_error
	Reserved1()
	{
		return RISSE_E_NOTIMPL;
	}

	risse_error
	IsInstanceOf(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	const risse_char *classname,
		iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->IsInstanceOf(flag, membername, hint, classname, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->IsInstanceOf(flag, membername, hint, classname, OBJ2);
		return hr;
	}

	risse_error
	IsInstanceOfByNum(risse_uint32 flag, risse_int num, const risse_char *classname,
		iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->IsInstanceOfByNum(flag, num, classname, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->IsInstanceOfByNum(flag, num, classname, OBJ2);
		return hr;
	}

	risse_error
	Operation(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	tRisseVariant *result,
		const tRisseVariant *param,	iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->Operation(flag, membername, hint, result, param, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->Operation(flag, membername, hint, result, param, OBJ2);
		return hr;
	}

	risse_error
	OperationByNum(risse_uint32 flag, risse_int num, tRisseVariant *result,
		const tRisseVariant *param,	iRisseDispatch2 *objthis)
	{
		risse_error hr =
			Dispatch1->OperationByNum(flag, num, result, param, OBJ1);
		if(hr == RISSE_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->OperationByNum(flag, num, result, param, OBJ2);
		return hr;
	}

	risse_error
	NativeInstanceSupport(risse_uint32 flag, risse_int32 classid,
		iRisseNativeInstance **pointer)  { return RISSE_E_NOTIMPL; }

	risse_error
	ClassInstanceInfo(risse_uint32 flag, risse_uint num, tRisseVariant *value)
		{ return RISSE_E_NOTIMPL;	}

	risse_error
	Reserved2()
	{
		return RISSE_E_NOTIMPL;
	}


	risse_error
	Reserved3()
	{
		return RISSE_E_NOTIMPL;
	}

};
#undef OBJ1
#undef OBJ2

//---------------------------------------------------------------------------
// tRisseVariantArrayStack
//---------------------------------------------------------------------------
// TODO: adjust RISSE_VA_ONE_ALLOC_MIN
#define RISSE_VA_ONE_ALLOC_MAX 1024
#define RISSE_COMPACT_FREQ 10000
static risse_int RisseCompactVariantArrayMagic = 0;

class tRisseVariantArrayStack
{
//	tRisseCriticalSection CS;

	struct tVariantArray
	{
		tRisseVariant *Array;
		risse_int Using;
		risse_int Allocated;
	};

	tVariantArray * Arrays; // array of array
	risse_int NumArraysAllocated;
	risse_int NumArraysUsing;
	tVariantArray * Current;
	risse_int CompactVariantArrayMagic;
	risse_int OperationDisabledCount;

	void IncreaseVariantArray(risse_int num);

	void DecreaseVariantArray(void);

	void InternalCompact(void);


public:
	tRisseVariantArrayStack();
	~tRisseVariantArrayStack();

	tRisseVariant * Allocate(risse_int num);

	void Deallocate(risse_int num, tRisseVariant *ptr);

	void Compact() { InternalCompact(); }

} *RisseVariantArrayStack = NULL;
//---------------------------------------------------------------------------
tRisseVariantArrayStack::tRisseVariantArrayStack()
{
	NumArraysAllocated = NumArraysUsing = 0;
	Arrays = NULL;
	Current = NULL;
	OperationDisabledCount = 0;
	CompactVariantArrayMagic = RisseCompactVariantArrayMagic;
}
//---------------------------------------------------------------------------
tRisseVariantArrayStack::~tRisseVariantArrayStack()
{
	OperationDisabledCount++;
	risse_int i;
	for(i = 0; i<NumArraysAllocated; i++)
	{
		delete [] Arrays[i].Array;
	}
	Risse_free(Arrays), Arrays = NULL;
}
//---------------------------------------------------------------------------
void tRisseVariantArrayStack::IncreaseVariantArray(risse_int num)
{
	// increase array block
	NumArraysUsing++;
	if(NumArraysUsing > NumArraysAllocated)
	{
		Arrays = (tVariantArray*)
			Risse_realloc(Arrays, sizeof(tVariantArray)*(NumArraysUsing));
		NumArraysAllocated = NumArraysUsing;
		Current = Arrays + NumArraysUsing -1;
		Current->Array = new tRisseVariant[num];
	}
	else
	{
		Current = Arrays + NumArraysUsing -1;
	}

	Current->Allocated = num;
	Current->Using = 0;
}
//---------------------------------------------------------------------------
void tRisseVariantArrayStack::DecreaseVariantArray(void)
{
	// decrease array block
	NumArraysUsing--;
	if(NumArraysUsing == 0)
		Current = NULL;
	else
		Current = Arrays + NumArraysUsing-1;
}
//---------------------------------------------------------------------------
void tRisseVariantArrayStack::InternalCompact(void)
{
	// minimize variant array block
	OperationDisabledCount++;
	try
	{
		while(NumArraysAllocated > NumArraysUsing)
		{
			NumArraysAllocated --;
			delete [] Arrays[NumArraysAllocated].Array;
		}

		if(Current)
		{
			for(risse_int i = Current->Using; i < Current->Allocated; i++)
				Current->Array[i].Clear();
		}

		if(NumArraysUsing == 0)
		{
			if(Arrays) Risse_free(Arrays), Arrays = NULL;
		}
		else
		{
			Arrays = (tVariantArray*)
				Risse_realloc(Arrays, sizeof(tVariantArray)*(NumArraysUsing));
		}
	}
	catch(...)
	{
		OperationDisabledCount--;
		throw;
	}
	OperationDisabledCount--;
}
//---------------------------------------------------------------------------
inline tRisseVariant * tRisseVariantArrayStack::Allocate(risse_int num)
{
//		tRisseCSH csh(CS);

	if(!OperationDisabledCount && num < RISSE_VA_ONE_ALLOC_MAX)
	{
		if(!Current || Current->Using + num > Current->Allocated)
		{
			IncreaseVariantArray( RISSE_VA_ONE_ALLOC_MAX );
		}
		tRisseVariant *ret = Current->Array + Current->Using;
		Current->Using += num;
		return ret;
	}
	else
	{
		return new tRisseVariant[num];
	}
}
//---------------------------------------------------------------------------
inline void tRisseVariantArrayStack::Deallocate(risse_int num, tRisseVariant *ptr)
{
//		tRisseCSH csh(CS);

	if(!OperationDisabledCount && num < RISSE_VA_ONE_ALLOC_MAX)
	{
		Current->Using -= num;
		if(Current->Using == 0)
		{
			DecreaseVariantArray();
		}
	}
	else
	{
		delete [] ptr;
	}

	if(!OperationDisabledCount)
	{
		if(CompactVariantArrayMagic != RisseCompactVariantArrayMagic)
		{
			Compact();
			CompactVariantArrayMagic = RisseCompactVariantArrayMagic;
		}
	}
}
//---------------------------------------------------------------------------
static risse_int RisseVariantArrayStackRefCount = 0;
//---------------------------------------------------------------------------
void RisseVariantArrayStackAddRef()
{
	if(RisseVariantArrayStackRefCount == 0)
	{
		RisseVariantArrayStack = new tRisseVariantArrayStack;
	}
	RisseVariantArrayStackRefCount++;
}
//---------------------------------------------------------------------------
void RisseVariantArrayStackRelease()
{
	if(RisseVariantArrayStackRefCount == 1)
	{
		delete RisseVariantArrayStack;
		RisseVariantArrayStack = NULL;
		RisseVariantArrayStackRefCount = 0;
	}
	else
	{
		RisseVariantArrayStackRefCount--;
	}
}
//---------------------------------------------------------------------------
void RisseVariantArrayStackCompact()
{
	RisseCompactVariantArrayMagic++;
}
//---------------------------------------------------------------------------
void RisseVariantArrayStackCompactNow()
{
	if(RisseVariantArrayStack) RisseVariantArrayStack->Compact();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseInterCodeContext ( class definitions are in risseInterCodeGen.h )
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ExecuteAsFunction(iRisseDispatch2 *objthis,
	tRisseVariant **args, risse_int numargs, tRisseVariant *result, risse_int start_ip)
{
	risse_int num_alloc = MaxVariableCount + VariableReserveCount + 1 + MaxFrameCount;
	RisseVariantArrayStackAddRef();
//	AddRef();
//	if(objthis) objthis->AddRef();
	try
	{
		tRisseVariant *regs =
			RisseVariantArrayStack->Allocate(num_alloc);
		tRisseVariant *ra = regs + MaxVariableCount + VariableReserveCount; // register area

		// objthis-proxy

		tRisseObjectProxy proxy;
		if(objthis)
		{
			proxy.SetObjects(objthis, Block->GetRisse()->GetGlobalNoAddRef());
			// TODO: caching of objthis-proxy

			ra[-2] = &proxy;
		}
		else
		{
			proxy.SetObjects(NULL, NULL);

			iRisseDispatch2 *global = Block->GetRisse()->GetGlobalNoAddRef();

			ra[-2].SetObject(global, global);
		}

/*
		if(objthis)
		{
			// TODO: caching of objthis-proxy
			tRisseObjectProxy *proxy = new tRisseObjectProxy();
			proxy->SetObjects(objthis, Block->GetRisse()->GetGlobalNoAddRef());

			ra[-2] = proxy;

			proxy->Release();
		}
		else
		{
			iRisseDispatch2 *global = Block->GetRisse()->GetGlobalNoAddRef();

			ra[-2].SetObject(global, global);
		}
*/
		if(RisseStackTracerEnabled()) RisseStackTracerPush(this, false);

		// check whether the objthis is deleting
		if(RisseWarnOnExecutionOnDeletingObject && RisseObjectFlagEnabled() &&
			Block->GetRisse()->GetConsoleOutput())
			RisseWarnIfObjectIsDeleting(Block->GetRisse()->GetConsoleOutput(), objthis);

		try
		{
			ra[-1].SetObject(objthis, objthis);
			ra[0].Clear();

			// transfer arguments
			if(numargs >= FuncDeclArgCount)
			{
				// given arguments are greater than or equal to desired arguments
				if(FuncDeclArgCount)
				{
					tRisseVariant *r = ra - 3;
					tRisseVariant **a = args;
					risse_int n = FuncDeclArgCount;
					while(true)
					{
						*r = **(a++);
						n--;
						if(!n) break;
						r--;
					}
				}
			}
			else
			{
				// given arguments are less than desired arguments
				tRisseVariant *r = ra - 3;
				tRisseVariant **a = args;
				risse_int i;
				for(i = 0; i<numargs; i++) *(r--) = **(a++);
				for(; i<FuncDeclArgCount; i++) (r--)->Clear();
			}

			// collapse into array when FuncDeclCollapseBase >= 0
			if(FuncDeclCollapseBase >= 0)
			{
				tRisseVariant *r = ra - 3 - FuncDeclCollapseBase; // target variant
				iRisseDispatch2 * dsp = RisseCreateArrayObject();
				*r = tRisseVariant(dsp, dsp);
				dsp->Release();

				if(numargs > FuncDeclCollapseBase)
				{
					// there are arguments to store
					for(risse_int c = 0, i = FuncDeclCollapseBase; i < numargs; i++, c++)
						dsp->PropSetByNum(0, c, args[i], dsp);
				}
			}

			// execute
			ExecuteCode(ra, start_ip, args, numargs, result);
		}
		catch(...)
		{
#if 0
			for(risse_int i=0; i<num_alloc; i++) regs[i].Clear();
#endif
			ra[-2].Clear(); // at least we must clear the object placed at local stack
			RisseVariantArrayStack->Deallocate(num_alloc, regs);
			if(RisseStackTracerEnabled()) RisseStackTracerPop();
			throw;
		}

#if 0
		for(risse_int i=0; i<MaxVariableCount + VariableReserveCount; i++)
			regs[i].Clear();
#endif
		ra[-2].Clear(); // at least we must clear the object placed at local stack

		RisseVariantArrayStack->Deallocate(num_alloc, regs);

		if(RisseStackTracerEnabled()) RisseStackTracerPop();
	}
	catch(...)
	{
//		if(objthis) objthis->Release();
//		Release();
		RisseVariantArrayStackRelease();
		throw;
	}
//	if(objthis) objthis->Release();
//	Release();
	RisseVariantArrayStackRelease();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::DisplayExceptionGeneratedCode(risse_int codepos,
	const tRisseVariant *ra)
{
	tRisse *risse = Block->GetRisse();
	ttstr info(
		RISSE_WS1("==== An exception occured at ") +
		GetPositionDescriptionString(codepos) +
		RISSE_WS2(", VM ip = ") + ttstr(codepos) + RISSE_WS3(" ===="));
	risse_int info_len = info.GetLen();

	risse->OutputToConsole(info.c_str());
	risse->OutputToConsole(RISSE_WS("-- Disassembled VM code --"));
	DisassenbleSrcLine(codepos);

	risse->OutputToConsole(RISSE_WS("-- Register dump --"));

	const tRisseVariant *ra_start = ra - (MaxVariableCount + VariableReserveCount);
	risse_int ra_count = MaxVariableCount + VariableReserveCount + 1 + MaxFrameCount;
	ttstr line;
	for(risse_int i = 0; i < ra_count; i ++)
	{
		ttstr reg_info = RISSE_WS("%") + ttstr(i - (MaxVariableCount + VariableReserveCount))
			+ RISSE_WS("=") + RisseVariantToReadableString(ra_start[i]);
		if(line.GetLen() + reg_info.GetLen() + 2 > info_len)
		{
			risse->OutputToConsole(line.c_str());
			line = reg_info;
		}
		else
		{
			if(!line.IsEmpty()) line += RISSE_WS("  ");
			line += reg_info;
		}
	}

	if(!line.IsEmpty())
	{
		risse->OutputToConsole(line.c_str());
	}

	risse->OutputToConsoleSeparator(RISSE_WS("-"), info_len);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ThrowScriptException(tRisseVariant &val,
	tRisseScriptBlock *block, risse_int srcpos)
{
	tRisseString msg;
	if(val.Type() == tvtObject)
	{
		try
		{
			tRisseVariantClosure clo = val.AsObjectClosureNoAddRef();
			if(clo.Object != NULL)
			{
				tRisseVariant v2;
				static tRisseString message_name(RISSE_WS("message"));
				risse_error hr = clo.PropGet(0, message_name.c_str(),
					message_name.GetHint(), &v2, NULL);
				if(RISSE_SUCCEEDED(hr))
				{
					msg = ttstr(RISSE_WS("script exception : ")) + ttstr(v2);
				}
			}
		}
		catch(...)
		{
		}
	}

	if(msg.IsEmpty())
	{
		msg = RISSE_WS("script exception");
	}

	Risse_eRisseScriptException(msg, this, srcpos, val);
}
//---------------------------------------------------------------------------
risse_int tRisseInterCodeContext::ExecuteCode(tRisseVariant *ra_org, risse_int startip,
	tRisseVariant **args, risse_int numargs, tRisseVariant *result)
{
	// execute VM codes
	risse_int32 *codesave;
	try
	{
		risse_int32 *code = codesave = CodeArea + startip;

		if(RisseStackTracerEnabled()) RisseStackTracerSetCodePointer(CodeArea, &codesave);

		tRisseVariant *ra = ra_org;
		tRisseVariant *da = DataArea;

		bool flag = false;

		while(true)
		{
			codesave = code;
			switch(*code)
			{
			case VM_NOP:
				code ++;
				break;

			case VM_CONST:
				RISSE_GET_VM_REG(ra, code[1]).CopyRef(RISSE_GET_VM_REG(da, code[2]));
				code += 3;
				break;

			case VM_CP:
				RISSE_GET_VM_REG(ra, code[1]).CopyRef(RISSE_GET_VM_REG(ra, code[2]));
				code += 3;
				break;

			case VM_CL:
				RISSE_GET_VM_REG(ra, code[1]).Clear();
				code += 2;
				break;

			case VM_CCL:
				ContinuousClear(ra, code);
				code += 3;
				break;

			case VM_TT:
				flag = RISSE_GET_VM_REG(ra, code[1]).operator bool();
				code += 2;
				break;

			case VM_TF:
				flag = !(RISSE_GET_VM_REG(ra, code[1]).operator bool());
				code += 2;
				break;

			case VM_CEQ:
				flag = RISSE_GET_VM_REG(ra, code[1]).NormalCompare(
					RISSE_GET_VM_REG(ra, code[2]));
				code += 3;
				break;

			case VM_CDEQ:
				flag = RISSE_GET_VM_REG(ra, code[1]).DiscernCompare(
					RISSE_GET_VM_REG(ra, code[2]));
				code += 3;
				break;

			case VM_CLT:
				flag = RISSE_GET_VM_REG(ra, code[1]).GreaterThan(
					RISSE_GET_VM_REG(ra, code[2]));
				code += 3;
				break;

			case VM_CGT:
				flag = RISSE_GET_VM_REG(ra, code[1]).LittlerThan(
					RISSE_GET_VM_REG(ra, code[2]));
				code += 3;
				break;

			case VM_SETF:
				RISSE_GET_VM_REG(ra, code[1]) = (tTVInteger)flag;
				code += 2;
				break;

			case VM_SETNF:
				RISSE_GET_VM_REG(ra, code[1]) = (tTVInteger)!flag;
				code += 2;
				break;

			case VM_LNOT:
				RISSE_GET_VM_REG(ra, code[1]).logicalnot();
				code += 2;
				break;

			case VM_NF:
				flag = !flag;
				code ++;
				break;

			case VM_JF:
				if(flag)
					RISSE_ADD_VM_CODE_ADDR(code, code[1]);
				else
					code += 2;
				break;

			case VM_JNF:
				if(!flag)
					RISSE_ADD_VM_CODE_ADDR(code, code[1]);
				else
					code += 2;
				break;

			case VM_JMP:
				RISSE_ADD_VM_CODE_ADDR(code, code[1]);
				break;

			case VM_INC:
				RISSE_GET_VM_REG(ra, code[1]).increment();
				code += 2;
				break;

			case VM_INCPD:
				OperatePropertyDirect0(ra, code, RISSE_OP_INC);
				code += 4;
				break;

			case VM_INCPI:
				OperatePropertyIndirect0(ra, code, RISSE_OP_INC);
				code += 4;
				break;

			case VM_INCP:
				OperateProperty0(ra, code, RISSE_OP_INC);
				code += 3;
				break;

			case VM_DEC:
				RISSE_GET_VM_REG(ra, code[1]).decrement();
				code += 2;
				break;

			case VM_DECPD:
				OperatePropertyDirect0(ra, code, RISSE_OP_DEC);
				code += 4;
				break;

			case VM_DECPI:
				OperatePropertyIndirect0(ra, code, RISSE_OP_DEC);
				code += 4;
				break;

			case VM_DECP:
				OperateProperty0(ra, code, RISSE_OP_DEC);
				code += 3;
				break;

#define RISSE_DEF_VM_P(vmcode, rope) \
			case VM_##vmcode: \
				RISSE_GET_VM_REG(ra, code[1]).rope(RISSE_GET_VM_REG(ra, code[2])); \
				code += 3; \
				break; \
			case VM_##vmcode##PD: \
				OperatePropertyDirect(ra, code, RISSE_OP_##vmcode); \
				code += 5; \
				break; \
			case VM_##vmcode##PI: \
				OperatePropertyIndirect(ra, code, RISSE_OP_##vmcode); \
				code += 5; \
				break; \
			case VM_##vmcode##P: \
				OperateProperty(ra, code, RISSE_OP_##vmcode); \
				code += 4; \
				break

				RISSE_DEF_VM_P(LOR, logicalorequal);
				RISSE_DEF_VM_P(LAND, logicalandequal);
				RISSE_DEF_VM_P(BOR, operator |=);
				RISSE_DEF_VM_P(BXOR, operator ^=);
				RISSE_DEF_VM_P(BAND, operator &=);
				RISSE_DEF_VM_P(SAR, operator >>=);
				RISSE_DEF_VM_P(SAL, operator <<=);
				RISSE_DEF_VM_P(SR, rbitshiftequal);
				RISSE_DEF_VM_P(ADD, operator +=);
				RISSE_DEF_VM_P(SUB, operator -=);
				RISSE_DEF_VM_P(MOD, operator %=);
				RISSE_DEF_VM_P(DIV, operator /=);
				RISSE_DEF_VM_P(IDIV, idivequal);
				RISSE_DEF_VM_P(MUL, operator *=);

#undef RISSE_DEF_VM_P

			case VM_BNOT:
				RISSE_GET_VM_REG(ra, code[1]).bitnot();
				code += 2;
				break;

			case VM_ASC:
				CharacterCodeOf(RISSE_GET_VM_REG(ra, code[1]));
				code += 2;
				break;

			case VM_CHR:
				CharacterCodeFrom(RISSE_GET_VM_REG(ra, code[1]));
				code += 2;
				break;

			case VM_NUM:
				RISSE_GET_VM_REG(ra, code[1]).tonumber();
				code += 2;
				break;

			case VM_CHS:
				RISSE_GET_VM_REG(ra, code[1]).changesign();
				code += 2;
				break;

			case VM_INV:
				RISSE_GET_VM_REG(ra, code[1]) = (tTVInteger)
					(RISSE_GET_VM_REG(ra, code[1]).AsObjectClosureNoAddRef().Invalidate(0,
					NULL, NULL, ra[-1].AsObjectNoAddRef()) == RISSE_S_TRUE);
				code += 2;
				break;

			case VM_CHKINV:
				RISSE_GET_VM_REG(ra, code[1]) = (tTVInteger)
					RisseIsObjectValid(RISSE_GET_VM_REG(ra, code[1]).AsObjectClosureNoAddRef().IsValid(0,
					NULL, NULL, ra[-1].AsObjectNoAddRef()));
				code += 2;
				break;

			case VM_INT:
				RISSE_GET_VM_REG(ra, code[1]).ToInteger();
				code += 2;
				break;

			case VM_REAL:
				RISSE_GET_VM_REG(ra, code[1]).ToReal();
				code += 2;
				break;

			case VM_STR:
				RISSE_GET_VM_REG(ra, code[1]).ToString();
				code += 2;
				break;

			case VM_OCTET:
				RISSE_GET_VM_REG(ra, code[1]).ToOctet();
				code += 2;
				break;

			case VM_TYPEOF:
				TypeOf(RISSE_GET_VM_REG(ra, code[1]));
				code += 2;
				break;

			case VM_TYPEOFD:
				TypeOfMemberDirect(ra, code, RISSE_MEMBERMUSTEXIST);
				code += 4;
				break;

			case VM_TYPEOFI:
				TypeOfMemberIndirect(ra, code, RISSE_MEMBERMUSTEXIST);
				code += 4;
				break;

			case VM_EVAL:
				Eval(RISSE_GET_VM_REG(ra, code[1]),
					RisseEvalOperatorIsOnGlobal ? NULL : ra[-1].AsObjectNoAddRef(),
					true);
				code += 2;
				break;

			case VM_EEXP:
				Eval(RISSE_GET_VM_REG(ra, code[1]),
					RisseEvalOperatorIsOnGlobal ? NULL : ra[-1].AsObjectNoAddRef(),
					false);
				code += 2;
				break;

			case VM_CHKINS:
				InstanceOf(RISSE_GET_VM_REG(ra, code[2]),
					RISSE_GET_VM_REG(ra, code[1]));
				code += 3;
				break;

			case VM_CALL:
			case VM_NEW:
				code += CallFunction(ra, code, args, numargs);
				break;

			case VM_CALLD:
				code += CallFunctionDirect(ra, code, args, numargs);
				break;

			case VM_CALLI:
				code += CallFunctionIndirect(ra, code, args, numargs);
				break;

			case VM_GPD:
				GetPropertyDirect(ra, code, 0);
				code += 4;
				break;

			case VM_GPDS:
				GetPropertyDirect(ra, code, RISSE_IGNOREPROP);
				code += 4;
				break;

			case VM_SPD:
				SetPropertyDirect(ra, code, 0);
				code += 4;
				break;

			case VM_SPDE:
				SetPropertyDirect(ra, code, RISSE_MEMBERENSURE);
				code += 4;
				break;

			case VM_SPDEH:
				SetPropertyDirect(ra, code, RISSE_MEMBERENSURE|RISSE_HIDDENMEMBER);
				code += 4;
				break;

			case VM_SPDS:
				SetPropertyDirect(ra, code, RISSE_MEMBERENSURE|RISSE_IGNOREPROP);
				code += 4;
				break;

			case VM_GPI:
				GetPropertyIndirect(ra, code, 0);
				code += 4;
				break;

			case VM_GPIS:
				GetPropertyIndirect(ra, code, RISSE_IGNOREPROP);
				code += 4;
				break;

			case VM_SPI:
				SetPropertyIndirect(ra, code, 0);
				code += 4;
				break;

			case VM_SPIE:
				SetPropertyIndirect(ra, code, RISSE_MEMBERENSURE);
				code += 4;
				break;

			case VM_SPIS:
				SetPropertyIndirect(ra, code, RISSE_MEMBERENSURE|RISSE_IGNOREPROP);
				code += 4;
				break;

			case VM_GETP:
				GetProperty(ra, code);
				code += 3;
				break;

			case VM_SETP:
				SetProperty(ra, code);
				code += 3;
				break;

			case VM_DELD:
				DeleteMemberDirect(ra, code);
				code += 4;
				break;

			case VM_DELI:
				DeleteMemberIndirect(ra, code);
				code += 4;
				break;

			case VM_SRV:
				if(result) result->CopyRef(RISSE_GET_VM_REG(ra, code[1]));
				code += 2;
				break;

			case VM_RET:
				return code+1-CodeArea;

			case VM_ENTRY:
				code = CodeArea + ExecuteCodeInTryBlock(ra, code-CodeArea + 3, args,
					numargs, result, RISSE_FROM_VM_CODE_ADDR(code[1])+code-CodeArea,
					RISSE_FROM_VM_REG_ADDR(code[2]));
				break;

			case VM_EXTRY:
				return code+1-CodeArea;  // same as ret

			case VM_THROW:
				ThrowScriptException(RISSE_GET_VM_REG(ra, code[1]),
					Block, CodePosToSrcPos(code-CodeArea));
				code += 2; // actually here not proceed...
				break;

			case VM_CHGTHIS:
				RISSE_GET_VM_REG(ra, code[1]).ChangeClosureObjThis(
					RISSE_GET_VM_REG(ra, code[2]).AsObjectNoAddRef());
				code += 3;
				break;

			case VM_GLOBAL:
				RISSE_GET_VM_REG(ra, code[1]) = Block->GetRisse()->GetGlobalNoAddRef();
				code += 2;
				break;

			case VM_ADDCI:
				AddClassInstanceInfo(ra, code);
				code+=3;
				break;

			case VM_REGMEMBER:
				RegisterObjectMember(ra[-1].AsObjectNoAddRef());
				code ++;
				break;

			case VM_DEBUGGER:
				RisseNativeDebuggerBreak();
				code ++;
				break;

			default:
				ThrowInvalidVMCode();
			}
		}
	}
	catch(eRisseSilent &e)
	{
		throw e;
	}
	catch(eRisseScriptException &e)
	{
		e.AddTrace(this, codesave-CodeArea);
		throw e;
	}
	catch(eRisseScriptError &e)
	{
		e.AddTrace(this, codesave-CodeArea);
		throw e;
	}
	catch(eRisse &e)
	{
		DisplayExceptionGeneratedCode(codesave - CodeArea, ra_org);
		Risse_eRisseScriptError(e.GetMessageString(), this, codesave-CodeArea);
	}
/*
	TODO: standard exception catch
	catch(exception &e)
	{
		DisplayExceptionGeneratedCode(codesave - CodeArea, ra_org);
		Risse_eRisseScriptError(e.what(), this, codesave-CodeArea);
	}
*/
	catch(const wchar_t *text)
	{
		DisplayExceptionGeneratedCode(codesave - CodeArea, ra_org);
		Risse_eRisseScriptError(text, this, codesave-CodeArea);
	}
/*
	TODO: narrow text catch
	catch(const char *text)
	{
		DisplayExceptionGeneratedCode(codesave - CodeArea, ra_org);
		Risse_eRisseScriptError(text, this, codesave-CodeArea);
	}
*/
	return codesave-CodeArea;
}
//---------------------------------------------------------------------------
risse_int tRisseInterCodeContext::ExecuteCodeInTryBlock(tRisseVariant *ra, risse_int startip,
	tRisseVariant **args, risse_int numargs, tRisseVariant *result, risse_int catchip,
	risse_int exobjreg)
{
	// execute codes in a try-protected block

	try
	{
		if(RisseStackTracerEnabled()) RisseStackTracerPush(this, true);
		risse_int ret;
		try
		{
			ret = ExecuteCode(ra, startip, args, numargs, result);
		}
		catch(...)
		{
			if(RisseStackTracerEnabled()) RisseStackTracerPop();
			throw;
		}
		if(RisseStackTracerEnabled()) RisseStackTracerPop();
		return ret;
	}
	RISSE_CONVERT_TO_RISSE_EXCEPTION_OBJECT(
			Block->GetRisse(),
			exobjreg,
			ra + exobjreg,
			{
				;
			},
			{
				return catchip;
			}
		)
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ContinuousClear(
	tRisseVariant *ra, const risse_int32 *code)
{
	tRisseVariant *r = RISSE_GET_VM_REG_ADDR(ra, code[1]);
	tRisseVariant *rl = r + code[2]; // code[2] is count ( not reg offset )
	while(r<rl) (r++)->Clear();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::GetPropertyDirect(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 flags)
{
	// ra[code[1]] = ra[code[2]][DataArea[ra[code[3]]]];

	tRisseVariant * ra_code2 = RISSE_GET_VM_REG_ADDR(ra, code[2]);
	tRisseVariantType type = ra_code2->Type();
	if(type == tvtString)
	{
		GetStringProperty(RISSE_GET_VM_REG_ADDR(ra, code[1]), ra_code2,
			RISSE_GET_VM_REG(DataArea, code[3]));
		return;
	}
	if(type == tvtOctet)
	{
		GetOctetProperty(RISSE_GET_VM_REG_ADDR(ra, code[1]), ra_code2,
			RISSE_GET_VM_REG(DataArea, code[3]));
		return;
	}


	risse_error hr;
	tRisseVariantClosure clo = ra_code2->AsObjectClosureNoAddRef();
	tRisseVariant *name = RISSE_GET_VM_REG_ADDR(DataArea, code[3]);
	hr = clo.PropGet(flags,
		name->GetString(), name->GetHint(), RISSE_GET_VM_REG_ADDR(ra, code[1]),
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr, RISSE_GET_VM_REG(DataArea, code[3]).GetString());
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::SetPropertyDirect(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 flags)
{
	// ra[code[1]][DataArea[ra[code[2]]]] = ra[code[3]]]

	tRisseVariant * ra_code1 = RISSE_GET_VM_REG_ADDR(ra, code[1]);
	tRisseVariantType type = ra_code1->Type();
	if(type == tvtString)
	{
		SetStringProperty(RISSE_GET_VM_REG_ADDR(ra, code[3]), ra_code1,
			RISSE_GET_VM_REG(DataArea, code[2]));
		return;
	}
	if(type == tvtOctet)
	{
		SetOctetProperty(RISSE_GET_VM_REG_ADDR(ra, code[3]), ra_code1,
			RISSE_GET_VM_REG(DataArea, code[2]));
		return;
	}

	risse_error hr;
	tRisseVariantClosure clo = ra_code1->AsObjectClosureNoAddRef();
	tRisseVariant *name = RISSE_GET_VM_REG_ADDR(DataArea, code[2]);
	hr = clo.PropSetByVS(flags,
		name->AsStringNoAddRef(), RISSE_GET_VM_REG_ADDR(ra, code[3]),
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	if(hr == RISSE_E_NOTIMPL)
		hr = clo.PropSet(flags,
			name->GetString(), name->GetHint(), RISSE_GET_VM_REG_ADDR(ra, code[3]),
				clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr, RISSE_GET_VM_REG(DataArea, code[2]).GetString());
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::GetProperty(tRisseVariant *ra, const risse_int32 *code)
{
	// ra[code[1]] = * ra[code[2]]
	tRisseVariantClosure clo =
		RISSE_GET_VM_REG_ADDR(ra, code[2])->AsObjectClosureNoAddRef();
	risse_error hr;
	hr = clo.PropGet(0, NULL, NULL, RISSE_GET_VM_REG_ADDR(ra, code[1]),
		clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr, NULL);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::SetProperty(tRisseVariant *ra, const risse_int32 *code)
{
	// * ra[code[1]] = ra[code[2]]
	tRisseVariantClosure clo =
		RISSE_GET_VM_REG_ADDR(ra, code[1])->AsObjectClosureNoAddRef();
	risse_error hr;
	hr = clo.PropSet(0, NULL, NULL, RISSE_GET_VM_REG_ADDR(ra, code[2]),
		clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr, NULL);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::GetPropertyIndirect(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 flags)
{
	// ra[code[1]] = ra[code[2]][ra[code[3]]];

	tRisseVariant * ra_code2 = RISSE_GET_VM_REG_ADDR(ra, code[2]);
	tRisseVariantType type = ra_code2->Type();
	if(type == tvtString)
	{
		GetStringProperty(RISSE_GET_VM_REG_ADDR(ra, code[1]), ra_code2,
			RISSE_GET_VM_REG(ra, code[3]));
		return;
	}
	if(type == tvtOctet)
	{
		GetOctetProperty(RISSE_GET_VM_REG_ADDR(ra, code[1]), ra_code2,
			RISSE_GET_VM_REG(ra, code[3]));
		return;
	}

	risse_error hr;
	tRisseVariantClosure clo = ra_code2->AsObjectClosureNoAddRef();
	tRisseVariant * ra_code3 = RISSE_GET_VM_REG_ADDR(ra, code[3]);
	if(ra_code3->Type() != tvtInteger)
	{
		tRisseVariantString *str;
		str = ra_code3->AsString();

		try
		{
			// TODO: verify here needs hint holding
			hr = clo.PropGet(flags, *str, NULL, RISSE_GET_VM_REG_ADDR(ra, code[1]),
				clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(RISSE_FAILED(hr)) RisseThrowFrom_risse_error(hr, *str);
		}
		catch(...)
		{
			if(str) str->Release();
			throw;
		}
		if(str) str->Release();
	}
	else
	{
		hr = clo.PropGetByNum(flags, (risse_int)ra_code3->AsInteger(),
			RISSE_GET_VM_REG_ADDR(ra, code[1]),
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
		if(RISSE_FAILED(hr)) ThrowFrom_risse_error_num(hr, (risse_int)ra_code3->AsInteger());
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::SetPropertyIndirect(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 flags)
{
	// ra[code[1]][ra[code[2]]] = ra[code[3]]]

	tRisseVariant *ra_code1 = RISSE_GET_VM_REG_ADDR(ra, code[1]);
	tRisseVariantType type = ra_code1->Type();
	if(type == tvtString)
	{
		SetStringProperty(RISSE_GET_VM_REG_ADDR(ra, code[3]),
			RISSE_GET_VM_REG_ADDR(ra, code[1]), RISSE_GET_VM_REG(ra, code[2]));
		return;
	}
	if(type == tvtOctet)
	{
		SetOctetProperty(RISSE_GET_VM_REG_ADDR(ra, code[3]),
			RISSE_GET_VM_REG_ADDR(ra, code[1]), RISSE_GET_VM_REG(ra, code[2]));
		return;
	}

	tRisseVariantClosure clo = ra_code1->AsObjectClosure();
	tRisseVariant *ra_code2 = RISSE_GET_VM_REG_ADDR(ra, code[2]);
	if(ra_code2->Type() != tvtInteger)
	{
		tRisseVariantString *str;
		try
		{
			str = ra_code2->AsString();
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		risse_error hr;

		try
		{
			hr = clo.PropSetByVS(flags,
				str, RISSE_GET_VM_REG_ADDR(ra, code[3]),
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(hr == RISSE_E_NOTIMPL)
				hr = clo.PropSet(flags,
					*str, NULL, RISSE_GET_VM_REG_ADDR(ra, code[3]),
						clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(RISSE_FAILED(hr)) RisseThrowFrom_risse_error(hr, *str);
		}
		catch(...)
		{
			if(str) str->Release();
			clo.Release();
			throw;
		}
		if(str) str->Release();
		clo.Release();
	}
	else
	{
		risse_error hr;

		try
		{
			hr = clo.PropSetByNum(flags,
				(risse_int)ra_code2->AsInteger(),
					RISSE_GET_VM_REG_ADDR(ra, code[3]),
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(RISSE_FAILED(hr))
				ThrowFrom_risse_error_num(hr, (risse_int)ra_code2->AsInteger());
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::OperatePropertyDirect(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 ope)
{
	// ra[code[1]] = ope(ra[code[2]][DataArea[ra[code[3]]]], /*param=*/ra[code[4]]);

	tRisseVariantClosure clo =  RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
	risse_error hr;
	try
	{
		tRisseVariant *name = RISSE_GET_VM_REG_ADDR(DataArea, code[3]);
		hr = clo.Operation(ope,
			name->GetString(), name->GetHint(),
			code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL,
			RISSE_GET_VM_REG_ADDR(ra, code[4]),
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr, RISSE_GET_VM_REG(DataArea, code[3]).GetString());
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::OperatePropertyIndirect(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 ope)
{
	// ra[code[1]] = ope(ra[code[2]][ra[code[3]]], /*param=*/ra[code[4]]);

	tRisseVariantClosure clo = RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tRisseVariant *ra_code3 = RISSE_GET_VM_REG_ADDR(ra, code[3]);
	if(ra_code3->Type() != tvtInteger)
	{
		tRisseVariantString *str;
		try
		{
			str = ra_code3->AsString();
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		risse_error hr;
		try
		{
			hr = clo.Operation(ope, *str, NULL,
				code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL,
				RISSE_GET_VM_REG_ADDR(ra, code[4]),
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(RISSE_FAILED(hr)) RisseThrowFrom_risse_error(hr, *str);
		}
		catch(...)
		{
			if(str) str->Release();
			clo.Release();
			throw;
		}
		if(str) str->Release();
		clo.Release();
	}
	else
	{
		risse_error hr;
		try
		{
			hr = clo.OperationByNum(ope, (risse_int)ra_code3->AsInteger(),
				code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL,
				RISSE_GET_VM_REG_ADDR(ra, code[4]),
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(RISSE_FAILED(hr))
				ThrowFrom_risse_error_num(hr,
					(risse_int)RISSE_GET_VM_REG(ra, code[3]).AsInteger());
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::OperateProperty(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 ope)
{
	// ra[code[1]] = ope(ra[code[2]], /*param=*/ra[code[3]]);
	tRisseVariantClosure clo =  RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
	risse_error hr;
	try
	{
		hr = clo.Operation(ope,
			NULL, NULL,
			code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL,
			RISSE_GET_VM_REG_ADDR(ra, code[3]),
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr, NULL);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::OperatePropertyDirect0(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 ope)
{
	// ra[code[1]] = ope(ra[code[2]][DataArea[ra[code[3]]]]);

	tRisseVariantClosure clo = RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
	risse_error hr;
	try
	{
		tRisseVariant *name = RISSE_GET_VM_REG_ADDR(DataArea, code[3]);
		hr = clo.Operation(ope,
			name->GetString(), name->GetHint(),
			code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL, NULL,
			ra[-1].AsObjectNoAddRef());
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr, RISSE_GET_VM_REG(DataArea, code[3]).GetString());
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::OperatePropertyIndirect0(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 ope)
{
	// ra[code[1]] = ope(ra[code[2]][ra[code[3]]]);

	tRisseVariantClosure clo = RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tRisseVariant *ra_code3 = RISSE_GET_VM_REG_ADDR(ra, code[3]);
	if(ra_code3->Type() != tvtInteger)
	{
		tRisseVariantString *str;
		try
		{
			str = ra_code3->AsString();
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		risse_error hr;
		try
		{
			hr = clo.Operation(ope, *str, NULL,
				code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL, NULL,
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(RISSE_FAILED(hr)) RisseThrowFrom_risse_error(hr, *str);
		}
		catch(...)
		{
			if(str) str->Release();
			clo.Release();
			throw;
		}
		if(str) str->Release();
		clo.Release();
	}
	else
	{
		risse_error hr;
		try
		{
			hr = clo.OperationByNum(ope, (risse_int)RISSE_GET_VM_REG(ra, code[3]).AsInteger(),
				code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL, NULL,
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(RISSE_FAILED(hr))
				ThrowFrom_risse_error_num(hr,
					(risse_int)RISSE_GET_VM_REG(ra, code[3]).AsInteger());
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::OperateProperty0(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 ope)
{
	// ra[code[1]] = ope(ra[code[2]]);
	tRisseVariantClosure clo =  RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
	risse_error hr;
	try
	{
		hr = clo.Operation(ope,
			NULL, NULL,
			code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL, NULL,
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr, NULL);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::DeleteMemberDirect(tRisseVariant *ra,
	const risse_int32 *code)
{
	// ra[code[1]] = delete ra[code[2]][DataArea[ra[code[3]]]];

	tRisseVariantClosure clo = RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
	risse_error hr;
	try
	{
		tRisseVariant *name = RISSE_GET_VM_REG_ADDR(DataArea, code[3]);
		hr = clo.DeleteMember(0,
			name->GetString(), name->GetHint(), ra[-1].AsObjectNoAddRef());
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(code[1])
	{
		if(RISSE_FAILED(hr))
			RISSE_GET_VM_REG(ra, code[1]) = (tTVInteger)false;
		else
			RISSE_GET_VM_REG(ra, code[1]) = (tTVInteger)true;
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::DeleteMemberIndirect(tRisseVariant *ra,
	const risse_int32 *code)
{
	// ra[code[1]] = delete ra[code[2]][ra[code[3]]];

	tRisseVariantClosure clo = RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tRisseVariantString *str;
	try
	{
		str = RISSE_GET_VM_REG(ra, code[3]).AsString();
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	risse_error hr;

	try
	{
		hr = clo.DeleteMember(0, *str, NULL,
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
		if(code[1])
		{
			if(RISSE_FAILED(hr))
				RISSE_GET_VM_REG(ra, code[1]) = (tTVInteger)false;
			else
				RISSE_GET_VM_REG(ra, code[1]) = (tTVInteger)true;
		}
	}
	catch(...)
	{
		if(str) str->Release();
		clo.Release();
		throw;
	}
	if(str) str->Release();
	clo.Release();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::TypeOfMemberDirect(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 flags)
{
	// ra[code[1]] = typeof ra[code[2]][DataArea[ra[code[3]]]];
	tRisseVariantType type = RISSE_GET_VM_REG(ra, code[2]).Type();
	if(type == tvtString)
	{
		GetStringProperty(RISSE_GET_VM_REG_ADDR(ra, code[1]),
			RISSE_GET_VM_REG_ADDR(ra, code[2]),
			RISSE_GET_VM_REG(DataArea,code[3]));
		TypeOf(RISSE_GET_VM_REG(ra, code[1]));
		return;
	}
	if(type == tvtOctet)
	{
		GetOctetProperty(RISSE_GET_VM_REG_ADDR(ra, code[1]),
			RISSE_GET_VM_REG_ADDR(ra, code[2]),
			RISSE_GET_VM_REG(DataArea, code[3]));
		TypeOf(RISSE_GET_VM_REG(ra, code[1]));
		return;
	}

	risse_error hr;
	tRisseVariantClosure clo = RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
	try
	{
		tRisseVariant *name = RISSE_GET_VM_REG_ADDR(DataArea, code[3]);
		hr = clo.PropGet(flags,
			name->GetString(), name->GetHint(), RISSE_GET_VM_REG_ADDR(ra, code[1]),
				clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(hr == RISSE_S_OK)
	{
		TypeOf(RISSE_GET_VM_REG(ra, code[1]));
	}
	else if(hr == RISSE_E_MEMBERNOTFOUND)
	{
		RISSE_GET_VM_REG(ra, code[1]) = RISSE_WS("undefined");
	}
	else if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr, RISSE_GET_VM_REG(DataArea, code[3]).GetString());
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::TypeOfMemberIndirect(tRisseVariant *ra,
	const risse_int32 *code, risse_uint32 flags)
{
	// ra[code[1]] = typeof ra[code[2]][ra[code[3]]];

	tRisseVariantType type = RISSE_GET_VM_REG(ra, code[2]).Type();
	if(type == tvtString)
	{
		GetStringProperty(RISSE_GET_VM_REG_ADDR(ra, code[1]),
			RISSE_GET_VM_REG_ADDR(ra, code[2]),
			RISSE_GET_VM_REG(ra, code[3]));
		TypeOf(ra[code[1]]);
		return;
	}
	if(type == tvtOctet)
	{
		GetOctetProperty(RISSE_GET_VM_REG_ADDR(ra, code[1]),
			RISSE_GET_VM_REG_ADDR(ra, code[2]),
			RISSE_GET_VM_REG(ra, code[3]));
		TypeOf(RISSE_GET_VM_REG(ra, code[1]));
		return;
	}

	risse_error hr;
	tRisseVariantClosure clo = RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
	if(RISSE_GET_VM_REG(ra, code[3]).Type() != tvtInteger)
	{
		tRisseVariantString *str;
		try
		{
			str = RISSE_GET_VM_REG(ra, code[3]).AsString();
		}
		catch(...)
		{
			clo.Release();
			throw;
		}

		try
		{
			// TODO: verify here needs hint holding
			hr = clo.PropGet(flags, *str, NULL, RISSE_GET_VM_REG_ADDR(ra, code[1]),
				clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(hr == RISSE_S_OK)
			{
				TypeOf(RISSE_GET_VM_REG(ra, code[1]));
			}
			else if(hr == RISSE_E_MEMBERNOTFOUND)
			{
				RISSE_GET_VM_REG(ra, code[1]) = RISSE_WS("undefined");
			}
			else if(RISSE_FAILED(hr)) RisseThrowFrom_risse_error(hr, *str);
		}
		catch(...)
		{
			if(str) str->Release();
			clo.Release();
			throw;
		}
		if(str) str->Release();
		clo.Release();
	}
	else
	{
		try
		{
			hr = clo.PropGetByNum(flags,
				(risse_int)RISSE_GET_VM_REG(ra, code[3]).AsInteger(),
				RISSE_GET_VM_REG_ADDR(ra, code[1]),
				clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(hr == RISSE_S_OK)
			{
				TypeOf(RISSE_GET_VM_REG(ra, code[1]));
			}
			else if(hr == RISSE_E_MEMBERNOTFOUND)
			{
				RISSE_GET_VM_REG(ra, code[1]) = RISSE_WS("undefined");
			}
			else if(RISSE_FAILED(hr))
				ThrowFrom_risse_error_num(hr,
					(risse_int)RISSE_GET_VM_REG(ra, code[3]).AsInteger());
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	}
}
//---------------------------------------------------------------------------
// Macros for preparing function argument pointer array.
// code[0] is an argument count;
// -1 for omitting ('...') argument to passing unmodified args from the caller.
// -2 for expanding array to argument
#define RISSE_PASS_ARGS_PREPARED_ARRAY_COUNT 20

#define RISSE_BEGIN_FUNC_CALL_ARGS(_code)                                   \
	tRisseVariant ** pass_args;                                             \
	tRisseVariant *pass_args_p[RISSE_PASS_ARGS_PREPARED_ARRAY_COUNT];         \
	tRisseVariant * pass_args_v = NULL;                                     \
	risse_int code_size;                                                    \
	bool alloc_args = false;                                              \
	try                                                                   \
	{                                                                     \
		risse_int pass_args_count = (_code)[0];                             \
		if(pass_args_count == -1)                                         \
		{                                                                 \
			/* omitting args; pass intact aguments from the caller */     \
			pass_args = args;                                             \
			pass_args_count = numargs;                                    \
			code_size = 1;                                                \
		}                                                                 \
		else if(pass_args_count == -2)                                    \
		{                                                                 \
			risse_int args_v_count = 0;                                     \
			/* count total argument count */                              \
			pass_args_count = 0;                                          \
			risse_int arg_written_count = (_code)[1];                       \
			code_size = arg_written_count * 2 + 2;                        \
			for(risse_int i = 0; i < arg_written_count; i++)                \
			{                                                             \
				switch((_code)[i*2+2])                                    \
				{                                                         \
				case fatNormal:                                           \
					pass_args_count ++;                                   \
					break;                                                \
				case fatExpand:                                           \
					args_v_count +=                                       \
						RisseGetArrayElementCount(                          \
							RISSE_GET_VM_REG(ra, (_code)[i*2+1+2]).         \
								AsObjectNoAddRef());                      \
					break;                                                \
				case fatUnnamedExpand:                                    \
					pass_args_count +=                                    \
						(numargs > FuncDeclUnnamedArgArrayBase)?          \
							(numargs - FuncDeclUnnamedArgArrayBase) : 0;  \
					break;                                                \
				}                                                         \
			}                                                             \
			pass_args_count += args_v_count;                              \
			/* allocate temporary variant array for Array object */       \
			pass_args_v = new tRisseVariant[args_v_count];                  \
			/* allocate pointer array */                                  \
			if(pass_args_count < RISSE_PASS_ARGS_PREPARED_ARRAY_COUNT)      \
				pass_args = pass_args_p;                                  \
			else                                                          \
				pass_args = new tRisseVariant * [pass_args_count],          \
					alloc_args = true;                                    \
			/* create pointer array to pass to callee function */         \
			args_v_count = 0;                                             \
			pass_args_count = 0;                                          \
			for(risse_int i = 0; i < arg_written_count; i++)                \
			{                                                             \
				switch((_code)[i*2+2])                                    \
				{                                                         \
				case fatNormal:                                           \
					pass_args[pass_args_count++] =                        \
						RISSE_GET_VM_REG_ADDR(ra, (_code)[i*2+1+2]);        \
					break;                                                \
				case fatExpand:                                           \
				  {                                                       \
					risse_int count = RisseCopyArrayElementTo(                \
						RISSE_GET_VM_REG(ra, (_code)[i*2+1+2]).             \
								AsObjectNoAddRef(),                       \
								pass_args_v + args_v_count, 0, -1);       \
					for(risse_int j = 0; j < count; j++)                    \
						pass_args[pass_args_count++] =                    \
							pass_args_v + j + args_v_count;               \
	                                                                      \
					args_v_count += count;                                \
	                                                                      \
					break;                                                \
				  }                                                       \
				case fatUnnamedExpand:                                    \
				  {                                                       \
					risse_int count =                                       \
						(numargs > FuncDeclUnnamedArgArrayBase)?          \
							(numargs - FuncDeclUnnamedArgArrayBase) : 0;  \
					for(risse_int j = 0; j < count; j++)                    \
						pass_args[pass_args_count++] =                    \
							args[FuncDeclUnnamedArgArrayBase + j];        \
					break;                                                \
				  }                                                       \
				}                                                         \
			}                                                             \
		}                                                                 \
		else if(pass_args_count <= RISSE_PASS_ARGS_PREPARED_ARRAY_COUNT)    \
		{                                                                 \
			code_size = pass_args_count + 1;                              \
			pass_args = pass_args_p;                                      \
			for(risse_int i = 0; i < pass_args_count; i++)                  \
				pass_args[i] = RISSE_GET_VM_REG_ADDR(ra, (_code)[1+i]);     \
		}                                                                 \
		else                                                              \
		{                                                                 \
			code_size = pass_args_count + 1;                              \
			pass_args = new tRisseVariant * [pass_args_count];              \
			alloc_args = true;                                            \
			for(risse_int i = 0; i < pass_args_count; i++)                  \
				pass_args[i] = RISSE_GET_VM_REG_ADDR(ra, (_code)[1+i]);     \
		}


#define RISSE_END_FUNC_CALL_ARGS                                            \
	}                                                                     \
	catch(...)                                                            \
	{                                                                     \
		if(alloc_args) delete [] pass_args;                               \
		if(pass_args_v) delete [] pass_args_v;                            \
		throw;                                                            \
	}                                                                     \
	if(alloc_args) delete [] pass_args;                                   \
	if(pass_args_v) delete [] pass_args_v;
//---------------------------------------------------------------------------
risse_int tRisseInterCodeContext::CallFunction(tRisseVariant *ra,
	const risse_int32 *code, tRisseVariant **args, risse_int numargs)
{
	// function calling / create new object
	risse_error hr;

	RISSE_BEGIN_FUNC_CALL_ARGS(code + 3)

		tRisseVariantClosure clo = RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
		try
		{
			if(code[0] == VM_CALL)
			{
				hr = clo.FuncCall(0, NULL, NULL,
					code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL, pass_args_count, pass_args,
						clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			}
			else
			{
				iRisseDispatch2 *dsp;
				hr = clo.CreateNew(0, NULL, NULL,
					&dsp, pass_args_count, pass_args,
						clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
				if(RISSE_SUCCEEDED(hr))
				{
					if(dsp)
					{
						if(code[1]) RISSE_GET_VM_REG(ra, code[1]) = tRisseVariant(dsp, dsp);
						dsp->Release();
					}
				}
			} 
		}
		catch(...)
		{ 
			clo.Release();
			throw;
		}
		clo.Release();
		// TODO: Null Check

	RISSE_END_FUNC_CALL_ARGS

	if(RISSE_FAILED(hr)) RisseThrowFrom_risse_error(hr, RISSE_WS(""));

	return code_size + 3;
}
#undef _code
//---------------------------------------------------------------------------
risse_int tRisseInterCodeContext::CallFunctionDirect(tRisseVariant *ra,
	const risse_int32 *code, tRisseVariant **args, risse_int numargs)
{
	risse_error hr;

	RISSE_BEGIN_FUNC_CALL_ARGS(code + 4)

		tRisseVariantType type = RISSE_GET_VM_REG(ra, code[2]).Type();
		tRisseVariant * name = RISSE_GET_VM_REG_ADDR(DataArea, code[3]);
		if(type == tvtString)
		{
			ProcessStringFunction(name->GetString(),
				RISSE_GET_VM_REG(ra, code[2]),
				pass_args, pass_args_count, code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL);
			hr = RISSE_S_OK;
		}
		else if(type == tvtOctet)
		{
			ProcessOctetFunction(name->GetString(),
				RISSE_GET_VM_REG(ra, code[2]),
				pass_args, pass_args_count, code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL);
			hr = RISSE_S_OK;
		}
		else
		{
			tRisseVariantClosure clo =  RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
			try
			{
				hr = clo.FuncCall(0,
					name->GetString(), name->GetHint(),
					code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL,
						pass_args_count, pass_args,
						clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			}
			catch(...)
			{
				clo.Release();
				throw;
			}
			clo.Release();
		}

	RISSE_END_FUNC_CALL_ARGS

	if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr,
			ttstr(RISSE_GET_VM_REG(DataArea, code[3])).c_str());

	return code_size + 4;
}
//---------------------------------------------------------------------------
risse_int tRisseInterCodeContext::CallFunctionIndirect(tRisseVariant *ra,
	const risse_int32 *code, tRisseVariant **args, risse_int numargs)
{
	risse_error hr;

	ttstr name = RISSE_GET_VM_REG(ra, code[3]);

	RISSE_BEGIN_FUNC_CALL_ARGS(code + 4)

		tRisseVariantType type = RISSE_GET_VM_REG(ra, code[2]).Type();
		if(type == tvtString)
		{
			ProcessStringFunction(name.c_str(),
				RISSE_GET_VM_REG(ra, code[2]),
				pass_args, pass_args_count, code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL);
			hr = RISSE_S_OK;
		}
		else if(type == tvtOctet)
		{
			ProcessOctetFunction(name.c_str(),
				RISSE_GET_VM_REG(ra, code[2]),
				pass_args, pass_args_count, code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL);
			hr = RISSE_S_OK;
		}
		else
		{
			tRisseVariantClosure clo =  RISSE_GET_VM_REG(ra, code[2]).AsObjectClosure();
			try
			{
				hr = clo.FuncCall(0,
					name.c_str(), name.GetHint(),
					code[1]?RISSE_GET_VM_REG_ADDR(ra, code[1]):NULL,
						pass_args_count, pass_args,
						clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			}
			catch(...)
			{
				clo.Release();
				throw;
			}
			clo.Release();
		}

	RISSE_END_FUNC_CALL_ARGS

	if(RISSE_FAILED(hr))
		RisseThrowFrom_risse_error(hr, name.c_str());

	return code_size + 4;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::AddClassInstanceInfo(tRisseVariant *ra,
	const risse_int32 *code)
{
	iRisseDispatch2 *dsp;
	dsp = RISSE_GET_VM_REG(ra, code[1]).AsObjectNoAddRef();
	if(dsp)
	{
		dsp->ClassInstanceInfo(RISSE_CII_ADD, 0, RISSE_GET_VM_REG_ADDR(ra, code[2]));
	}
	else
	{
		// ?? must be an error
	}
}
//---------------------------------------------------------------------------
static const risse_char *StrFuncs[] = {
	RISSE_WS("charAt"),
	RISSE_WS("indexOf"),
	RISSE_WS("toUpperCase"),
	RISSE_WS("toLowerCase"),
	RISSE_WS("substring"),
	RISSE_WS("substr"),
	RISSE_WS("sprintf"),
	RISSE_WS("replace"),
	RISSE_WS("escape"),
	RISSE_WS("split") };
#define RISSE_STRFUNC_MAX (sizeof(StrFuncs) / sizeof(StrFuncs[0]))
static risse_int32 StrFuncHash[RISSE_STRFUNC_MAX];
static bool RisseStrFuncInit = false;
static void InitRisseStrFunc()
{
	RisseStrFuncInit = true;
	for(risse_size i=0; i<RISSE_STRFUNC_MAX; i++)
	{
		const risse_char *p = StrFuncs[i];
		risse_int32 hash = 0;
		while(*p) hash += *p, p++;
		StrFuncHash[i] = hash;
	}
}

void tRisseInterCodeContext::ProcessStringFunction(const risse_char *member,
	const ttstr & target, tRisseVariant **args, risse_int numargs, tRisseVariant *result)
{
	if(!RisseStrFuncInit) InitRisseStrFunc();

	risse_int32 hash;

	if(!member) RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, RISSE_WS(""));

	const risse_char *m = member;
	hash = 0;
	while(*m) hash += *m, m++;

	const risse_char *s = target.c_str(); // target string
	const risse_int s_len = target.GetLen();

	if(hash == StrFuncHash[0] && !Risse_strcmp(StrFuncs[0], member)) // charAt
	{
		if(numargs != 1) RisseThrowFrom_risse_error(RISSE_E_BADPARAMCOUNT);
		if(s_len == 0)
		{
			if(result) *result = RISSE_WS("");
			return;
		}
		risse_int i = (risse_int)*args[0];
		if(i<0 || i>=s_len)
		{
			if(result) *result = RISSE_WS("");
			return;
		}
		risse_char bt[2];
		bt[1] = 0;
		bt[0] = s[i];
		if(result) *result = bt;
		return;
	}
	else if(hash == StrFuncHash[1] && !Risse_strcmp(StrFuncs[1], member)) // indexOf
	{
		if(numargs != 1 && numargs != 2) RisseThrowFrom_risse_error(RISSE_E_BADPARAMCOUNT);
		tRisseVariantString *pstr = args[0]->AsString(); // sub string

		if(!s || !pstr)
		{
			if(result) *result = (tTVInteger)-1;
			if(pstr) pstr->Release();
			return;
		}
		risse_int start;
		if(numargs == 1)
		{
			start = 0;
		}
		else
		{
			try // integer convertion may raise an exception
			{
				start = (risse_int)*args[1];
			}
			catch(...)
			{
				pstr->Release();
				throw;
			}
		}
		if(start >= s_len)
		{
			if(result) *result = (tTVInteger)-1;
			if(pstr) pstr->Release();
			return;
		}
		risse_char *p;
		p = Risse_strstr(s + start, (const risse_char*)*pstr);
		if(!p)
		{
			if(result) *result = (tTVInteger)-1;
		}
		else
		{
			if(result) *result = (tTVInteger)(p-s);
		}
		if(pstr) pstr->Release();
		return;
	}
	else if(hash == StrFuncHash[2] && !Risse_strcmp(StrFuncs[2], member)) // toUpperCase
	{
		if(numargs != 0) RisseThrowFrom_risse_error(RISSE_E_BADPARAMCOUNT);
		if(result)
		{
			*result = s; // here s is copyed to *result ( not reference )
			const risse_char *pstr = result->GetString();  // buffer in *result
			if(pstr)
			{
				risse_char *p = (risse_char*)pstr;    // WARNING!! modification of const
				while(*p)
				{
					if(*p>=RISSE_WC('a') && *p<=RISSE_WC('z')) *p += RISSE_WC('Z')-RISSE_WC('z');
					p++;
				}
			}
		}
		return;
	}
	else if(hash == StrFuncHash[3] && !Risse_strcmp(StrFuncs[3], member)) // toLowerCase
	{
		if(numargs != 0) RisseThrowFrom_risse_error(RISSE_E_BADPARAMCOUNT);
		if(result)
		{
			*result = s;
			const risse_char *pstr = result->GetString();
			if(pstr)
			{
				risse_char *p = (risse_char*)pstr;    // WARNING!! modification of const
				while(*p)
				{
					if(*p>=RISSE_WC('A') && *p<=RISSE_WC('Z')) *p += RISSE_WC('z')-RISSE_WC('Z');
					p++;
				}
			}
		}
		return;
	}
	else if((hash == StrFuncHash[4]  && !Risse_strcmp(StrFuncs[4], member)) ||
		(hash == StrFuncHash[5] && !Risse_strcmp(StrFuncs[5], member))) // substring , substr
	{
		if(numargs != 1 && numargs != 2) RisseThrowFrom_risse_error(RISSE_E_BADPARAMCOUNT);
		risse_int start = (risse_int)*args[0];
		if(start < 0 || start >= s_len)
		{
			if(result) *result=RISSE_WS("");
			return;
		}
		risse_int count;
		if(numargs == 2)
		{
			count = (risse_int)*args[1];
			if(count<0)
			{
				if(result) *result = RISSE_WS("");
				return;
			}
			if(start + count > s_len) count = s_len - start;
			if(result)
				*result = ttstr(s+start, count);
			return;
		}
		else
		{
			if(result) *result = s + start;
		}
		return;
	}
	else if(hash == StrFuncHash[6] && !Risse_strcmp(StrFuncs[6], member)) // sprintf
	{
		if(result)
		{
			tRisseVariantString *res;
			res = RisseFormatString(s, numargs, args);
			*result = res;
			if(res) res->Release();
		}
		return;
	}
	else if(hash == StrFuncHash[7] && !Risse_strcmp(StrFuncs[7], member))  // replace
	{
		// string.replace(pattern, replacement-string)  -->
		// pattern.replace(string, replacement-string)
		if(numargs < 2) RisseThrowFrom_risse_error(RISSE_E_BADPARAMCOUNT);

		tRisseVariantClosure clo = args[0]->AsObjectClosureNoAddRef();
		tRisseVariant str = target;
		tRisseVariant *params[] = { &str, args[1] };
		static tRisseString replace_name(RISSE_WS("replace"));
		clo.FuncCall(0, replace_name.c_str(), replace_name.GetHint(),
			result, 2, params, NULL);

		return;
	}
	else if(hash == StrFuncHash[8] && !Risse_strcmp(StrFuncs[8], member))  // escape
	{
		if(result)
			*result = target.EscapeC();

		return;
	}
	else if(hash == StrFuncHash[9] && !Risse_strcmp(StrFuncs[9], member))  // split
	{
		// string.split(pattern, reserved, purgeempty) -->
		// Array.split(pattern, string, reserved, purgeempty)
		if(numargs < 1) RisseThrowFrom_risse_error(RISSE_E_BADPARAMCOUNT);

		iRisseDispatch2 * array = RisseCreateArrayObject();
		try
		{
			tRisseVariant str = target;
			risse_int arg_count = 2;
			tRisseVariant *params[4] = {
				args[0],
				&str };
			if(numargs >= 2)
			{
				arg_count ++;
				params[2] = args[1];
			}
			if(numargs >= 3)
			{
				arg_count ++;
				params[3] = args[2];
			}
			static tRisseString split_name(RISSE_WS("split"));
			array->FuncCall(0, split_name.c_str(), split_name.GetHint(),
				NULL, arg_count, params, array);

			if(result) *result = tRisseVariant(array, array);
		}
		catch(...)
		{
			array->Release();
			throw;
		}
		array->Release();

		return;
	}

	RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, member);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ProcessOctetFunction(const risse_char *member, const ttstr & target,
		tRisseVariant **args, risse_int numargs, tRisseVariant *result)
{
	// TODO: unpack/pack implementation

	RisseThrowFrom_risse_error(RISSE_E_MEMBERNOTFOUND, member);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::TypeOf(tRisseVariant &val)
{
	// processes Risse's typeof operator.
	static tRisseString void_name(RISSE_WS("void"));
	static tRisseString Object_name(RISSE_WS("Object"));
	static tRisseString String_name(RISSE_WS("String"));
	static tRisseString Integer_name(RISSE_WS("Integer"));
	static tRisseString Real_name(RISSE_WS("Real"));
	static tRisseString Octet_name(RISSE_WS("Octet"));

	switch(val.Type())
	{
	case tvtVoid:
		val = void_name;   // differs from Risse1
		break;

	case tvtObject:
		val = Object_name;
		break;

	case tvtString:
		val = String_name;
		break;

	case tvtInteger:
		val = Integer_name; // differs from Risse1
		break;

	case tvtReal:
		val = Real_name; // differs from Risse1
		break;

	case tvtOctet:
		val = Octet_name;
		break;
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::Eval(tRisseVariant &val,
	iRisseDispatch2 * objthis, bool resneed)
{
	if(objthis) objthis->AddRef();
	try
	{
		tRisseVariant res;
		ttstr str(val);
		if(!str.IsEmpty())
		{
			if(resneed)
				Block->GetRisse()->EvalExpression(str, &res, objthis);
			else
				Block->GetRisse()->EvalExpression(str, NULL, objthis);
		}
		if(resneed) val = res;
	}
	catch(...)
	{
		if(objthis) objthis->Release();
		throw;
	}
	if(objthis) objthis->Release();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::CharacterCodeOf(tRisseVariant &val)
{
	// puts val's character code on val
	tRisseVariantString * str = val.AsString();
	if(str)
	{
		const risse_char *ch = (const risse_char*)*str;
		val = tTVInteger(ch[0]);
		str->Release();
		return;
	}
	val = tTVInteger(0);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::CharacterCodeFrom(tRisseVariant &val)
{
	risse_char ch[2];
	ch[0] = val.AsInteger();
	ch[1] = 0;
	val = ch;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::InstanceOf(const tRisseVariant &name, tRisseVariant &targ)
{
	// checks instance inheritance.
	tRisseVariantString *str = name.AsString();
	if(str)
	{
		risse_error hr;
		try
		{
			hr = RisseDefaultIsInstanceOf(0, targ, (const risse_char*)*str, NULL);
		}
		catch(...)
		{
			str->Release();
			throw;
		}
		str->Release();
		if(RISSE_FAILED(hr)) RisseThrowFrom_risse_error(hr);

		targ = (tTVInteger)(hr == RISSE_S_TRUE);
		return;
	}
	targ = (tTVInteger)false;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::RegisterObjectMember(iRisseDispatch2 * dest)
{
	// register this object member to 'dest' (destination object).
	// called when new object is to be created.
	// a class to receive member callback from class

	class tCallback : public tRisseDispatch
	{
	public:
		iRisseDispatch2 * Dest; // destination object
		risse_error FuncCall(
			risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
			tRisseVariant *result, risse_int numparams, tRisseVariant **param,
			iRisseDispatch2 *objthis)
		{
			// *param[0] = name   *param[1] = flags   *param[2] = value
			risse_uint32 flags = (risse_int)*param[1];
			if(!(flags & RISSE_STATICMEMBER))
			{
				tRisseVariant val = *param[2];
				if(val.Type() == tvtObject)
				{
					// change object's objthis if the object's objthis is null
//					if(val.AsObjectThisNoAddRef() == NULL)
						val.ChangeClosureObjThis(Dest);
				}

				if(Dest->PropSetByVS(RISSE_MEMBERENSURE|RISSE_IGNOREPROP|flags,
					param[0]->AsStringNoAddRef(), &val, Dest) == RISSE_E_NOTIMPL)
					Dest->PropSet(RISSE_MEMBERENSURE|RISSE_IGNOREPROP|flags,
					param[0]->GetString(), NULL, &val, Dest);
			}
			if(result) *result = (tTVInteger)(1); // returns true
			return RISSE_S_OK;
		}
	};

	tCallback callback;
	callback.Dest = dest;

	// enumerate members
	tRisseVariantClosure clo(&callback, (iRisseDispatch2*)NULL);
	EnumMembers(RISSE_IGNOREPROP,
		&clo, this);
}
//---------------------------------------------------------------------------
#define RISSE_DO_SUPERCLASS_PROXY_BEGIN \
		std::vector<risse_int> &pointer = SuperClassGetter->SuperClassGetterPointer; \
		if(pointer.size() != 0) \
		{ \
			std::vector<risse_int>::iterator i; \
			for(i = pointer.end()-1; \
				i >=pointer.begin(); i--) \
			{ \
				tRisseVariant res; \
				SuperClassGetter->ExecuteAsFunction(NULL, NULL, 0, &res, *i); \
				tRisseVariantClosure clo = res.AsObjectClosureNoAddRef();


#define RISSE_DO_SUPERCLASS_PROXY_END \
				if(hr != RISSE_E_MEMBERNOTFOUND) break; \
			} \
		}

risse_error  tRisseInterCodeContext::FuncCall(
		risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
			tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		switch(ContextType)
		{
		case ctTopLevel:
			ExecuteAsFunction(
				objthis?objthis:Block->GetRisse()->GetGlobalNoAddRef(),
				NULL, 0, result, 0);
			break;

		case ctFunction:
		case ctExprFunction:
		case ctPropertyGetter:
		case ctPropertySetter:
			ExecuteAsFunction(objthis, param, numparams, result, 0);
			break;

		case ctClass: // on super class' initialization
			ExecuteAsFunction(objthis, param, numparams, result, 0);
			break;

		case ctProperty:
		case ctSuperClassGetter:
			return RISSE_E_INVALIDTYPE;
		}

		return RISSE_S_OK;
	}

	risse_error hr = inherited::FuncCall(flag, membername, hint, result,
		numparams, param, objthis);

	if(membername != NULL && hr == RISSE_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		RISSE_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.FuncCall(flag, membername, hint, result,
				numparams, param, objthis);
		RISSE_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
risse_error  tRisseInterCodeContext::PropGet(risse_uint32 flag,
	const risse_char * membername, risse_uint32 *hint, tRisseVariant *result,
		iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		if(ContextType == ctProperty)
		{
			// executed as a property getter
			if(PropGetter)
				return PropGetter->FuncCall(0, NULL, NULL, result, 0, NULL,
					objthis);
			else
				return RISSE_E_ACCESSDENYED;
		}
	}

	risse_error hr = inherited::PropGet(flag, membername, hint, result, objthis);

	if(membername != NULL && hr == RISSE_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		RISSE_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.PropGet(flag, membername, hint, result, objthis);
		RISSE_DO_SUPERCLASS_PROXY_END
	}
	return hr;

}
//---------------------------------------------------------------------------
risse_error  tRisseInterCodeContext::PropSet(risse_uint32 flag,
	const risse_char *membername, risse_uint32 *hint,
		const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		if(ContextType == ctProperty)
		{
			// executed as a property setter
			if(PropSetter)
				return PropSetter->FuncCall(0, NULL, NULL, NULL, 1,
					const_cast<tRisseVariant **>(&param), objthis);
			else
				return RISSE_E_ACCESSDENYED;

				// WARNING!! const tRisseVariant ** -> tRisseVariant** force casting
		}
	}

	risse_error hr;
	if(membername != NULL && ContextType == ctClass && SuperClassGetter)
	{
		risse_uint32 pseudo_flag =
			(flag & RISSE_IGNOREPROP) ? flag : (flag &~ RISSE_MEMBERENSURE);
			// member ensuring is temporarily disabled unless RISSE_IGNOREPROP

		hr = inherited::PropSet(pseudo_flag, membername, hint, param,
			objthis);
		if(hr == RISSE_E_MEMBERNOTFOUND)
		{
			RISSE_DO_SUPERCLASS_PROXY_BEGIN
				hr = clo.PropSet(pseudo_flag, membername, hint, param,
					objthis);
			RISSE_DO_SUPERCLASS_PROXY_END
		}
		
		if(hr == RISSE_E_MEMBERNOTFOUND && (flag & RISSE_MEMBERENSURE))
		{
			// re-ensure the member for "this" object
			hr = inherited::PropSet(flag, membername, hint, param,
				objthis);
		}
	}
	else
	{
		hr = inherited::PropSet(flag, membername, hint, param,
			objthis);
	}

	return hr;
}
//---------------------------------------------------------------------------
risse_error  tRisseInterCodeContext::CreateNew(risse_uint32 flag,
	const risse_char * membername, risse_uint32 *hint,
	iRisseDispatch2 **result, risse_int numparams,
	tRisseVariant **param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		if(ContextType != ctClass) return RISSE_E_INVALIDTYPE;

		iRisseDispatch2 *dsp = new inherited();

		try
		{
			ExecuteAsFunction(dsp, NULL, 0, NULL, 0);
			FuncCall(0, Name, NULL, NULL, numparams, param, dsp);
		}
		catch(...)
		{
			dsp->Release();
			throw;
		}

		*result = dsp;
		return RISSE_S_OK;
	}

	risse_error hr = inherited::CreateNew(flag, membername, hint,
		result, numparams, param,
		objthis);

	if(membername != NULL && hr == RISSE_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		RISSE_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.CreateNew(flag, membername, hint, result, numparams, param,
				objthis);
		RISSE_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
risse_error  tRisseInterCodeContext::IsInstanceOf(risse_uint32 flag,
	const risse_char *membername, risse_uint32 *hint, const risse_char *classname,
		iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		switch(ContextType)
		{
		case ctTopLevel:
		case ctPropertySetter:
		case ctPropertyGetter:
		case ctSuperClassGetter:
			break;

		case ctFunction:
		case ctExprFunction:
			if(!Risse_strcmp(classname, RISSE_WS("Function"))) return RISSE_S_TRUE;
			break;

		case ctProperty:
			if(!Risse_strcmp(classname, RISSE_WS("Property"))) return RISSE_S_TRUE;
			break;
			
		case ctClass:
			if(!Risse_strcmp(classname, RISSE_WS("Class"))) return RISSE_S_TRUE;
			break;
		}
	}

	risse_error hr = inherited::IsInstanceOf(flag, membername, hint,
		classname, objthis);

	if(membername != NULL && hr == RISSE_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		RISSE_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.IsInstanceOf(flag, membername, hint, classname, objthis);
		RISSE_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
risse_error
	tRisseInterCodeContext::GetCount(risse_int *result, const risse_char *membername,
		risse_uint32 *hint, iRisseDispatch2 *objthis)
{
	risse_error hr = inherited::GetCount(result, membername, hint,
		objthis);

	if(membername != NULL && hr == RISSE_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		RISSE_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.GetCount(result, membername, hint, objthis);
		RISSE_DO_SUPERCLASS_PROXY_END
	}
	return hr;

}
//---------------------------------------------------------------------------
risse_error
	tRisseInterCodeContext::DeleteMember(risse_uint32 flag, const risse_char *membername,
		risse_uint32 *hint,  iRisseDispatch2 *objthis)
{
	risse_error hr = inherited::DeleteMember(flag, membername, hint,
		objthis);

	if(membername != NULL && hr == RISSE_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		RISSE_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.DeleteMember(flag, membername, hint, objthis);
		RISSE_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
risse_error
	tRisseInterCodeContext::Invalidate(risse_uint32 flag, const risse_char *membername,
		risse_uint32 *hint, iRisseDispatch2 *objthis)
{
	risse_error hr = inherited::Invalidate(flag, membername, hint,
		objthis);

	if(membername != NULL && hr == RISSE_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		RISSE_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.Invalidate(flag, membername, hint, objthis);
		RISSE_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
risse_error
	tRisseInterCodeContext::IsValid(risse_uint32 flag, const risse_char *membername,
		risse_uint32 *hint, iRisseDispatch2 *objthis)
{
	risse_error hr = inherited::IsValid(flag, membername, hint,
		objthis);

	if(membername != NULL && hr == RISSE_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		RISSE_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.IsValid(flag, membername, hint, objthis);
		RISSE_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
risse_error
	tRisseInterCodeContext::Operation(risse_uint32 flag, const risse_char *membername,
		risse_uint32 *hint, tRisseVariant *result,
			const tRisseVariant *param,	iRisseDispatch2 *objthis)
{
	if(membername == NULL)
	{
		if(ContextType == ctProperty)
		{
			// operation for property object
			return tRisseDispatch::Operation(flag, membername, hint, result, param,
				objthis);
		}
		else
		{
			return inherited::Operation(flag, membername, hint, result, param,
				objthis);
		}
	}

	if(membername != NULL && ContextType == ctClass && SuperClassGetter)
	{
		risse_uint32 pseudo_flag =
			(flag & RISSE_IGNOREPROP) ? flag : (flag &~ RISSE_MEMBERENSURE);

		risse_error hr = inherited::Operation(pseudo_flag, membername, hint,
			result, param, objthis);

		if(hr == RISSE_E_MEMBERNOTFOUND)
		{
			// look up super class
			RISSE_DO_SUPERCLASS_PROXY_BEGIN
				hr = clo.Operation(pseudo_flag, membername, hint, result, param,
					objthis);
			RISSE_DO_SUPERCLASS_PROXY_END
		}

		if(hr == RISSE_E_MEMBERNOTFOUND)
			hr = inherited::Operation(flag, membername, hint, result,
				param, objthis);

		return hr;
	}
	else
	{
		return inherited::Operation(flag, membername, hint, result, param,
			objthis);
	}
}
//---------------------------------------------------------------------------

}

