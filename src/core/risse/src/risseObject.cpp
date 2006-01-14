//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Object クラスの実装
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include "risseObject.h"
#include "risseUtils.h"
#include "risseNative.h"
#include "risseHashSearch.h"
#include "risseGlobalStringMap.h"
#include "risseDebug.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(1025);

//---------------------------------------------------------------------------
// utility functions
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void RisseDoVariantOperation(risse_int op, tRisseVariant &target, const tRisseVariant *param)
{
	switch(op)
	{
	case RISSE_OP_BAND:
		target.operator &= (*param);
		return;
	case RISSE_OP_BOR:
		target.operator |= (*param);
		return;
	case RISSE_OP_BXOR:
		target.operator ^= (*param);
		return;
	case RISSE_OP_SUB:
		target.operator -= (*param);
		return;
	case RISSE_OP_ADD:
		target.operator += (*param);
		return;
	case RISSE_OP_MOD:
		target.operator %= (*param);
		return;
	case RISSE_OP_DIV:
		target.operator /= (*param);
		return;
	case RISSE_OP_IDIV:
		target.idivequal(*param);
		return;
	case RISSE_OP_MUL:
		target.operator *= (*param);
		return;
	case RISSE_OP_LOR:
		target.logicalorequal(*param);
		return;
	case RISSE_OP_LAND:
		target.logicalandequal(*param);
		return;
	case RISSE_OP_SAR:
		target.operator >>= (*param);
		return;
	case RISSE_OP_SAL:
		target.operator <<= (*param);
		return;
	case RISSE_OP_SR:
		target.rbitshiftequal(*param);
		return;
	case RISSE_OP_INC:
		target.increment();
		return;
	case RISSE_OP_DEC:
		target.decrement();
		return;
	}

}
//---------------------------------------------------------------------------




/*[C*/
//---------------------------------------------------------------------------
// tRisseDispatch
//---------------------------------------------------------------------------
tRisseDispatch::tRisseDispatch()
{
	BeforeDestructionCalled = false;
	RefCount = 1;
#ifdef TVP_IN_PLUGIN_STUB // TVP plug-in support
	TVPPluginGlobalRefCount++;
#endif
}
//---------------------------------------------------------------------------
tRisseDispatch::~tRisseDispatch()
{
	if(!BeforeDestructionCalled)
	{
		BeforeDestructionCalled = true;
		BeforeDestruction();
	}
}
//---------------------------------------------------------------------------
risse_uint tRisseDispatch::AddRef(void)
{
#ifdef TVP_IN_PLUGIN_STUB // TVP plug-in support
	TVPPluginGlobalRefCount++;
#endif
	return ++RefCount;
}
//---------------------------------------------------------------------------
risse_uint tRisseDispatch::Release(void)
{
#ifdef TVP_IN_PLUGIN_STUB // TVP plug-in support
	TVPPluginGlobalRefCount--;
#endif
	if(RefCount == 1) // avoid to call "BeforeDestruction" with RefCount == 0
	{
		// object destruction
		if(!BeforeDestructionCalled)
		{
			BeforeDestructionCalled = true;
			BeforeDestruction();
		}

		if(RefCount == 1) // really ready to destruct ?
		{
			delete this;
			return 0;
		}
	}
	return --RefCount;
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::FuncCallByNum(
		risse_uint32 flag,
		risse_int num,
		tRisseVariant *result,
		risse_int numparams,
		tRisseVariant **param,
		iRisseDispatch2 *objthis
		)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	return FuncCall(flag, buf, NULL, result, numparams, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::PropGetByNum(
		risse_uint32 flag,
		risse_int num,
		tRisseVariant *result,
		iRisseDispatch2 *objthis
		)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	return PropGet(flag, buf, NULL, result, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::PropSetByNum(
		risse_uint32 flag,
		risse_int num,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis
		)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	return PropSet(flag, buf, NULL, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::GetCountByNum(
		risse_int *result,
		risse_int num,
		iRisseDispatch2 *objthis
		)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	return GetCount(result, buf, NULL, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::DeleteMemberByNum(
		risse_uint32 flag,
		risse_int num,
		iRisseDispatch2 *objthis
		)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	return DeleteMember(flag, buf, NULL, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::InvalidateByNum(
		risse_uint32 flag,
		risse_int num,
		iRisseDispatch2 *objthis
		)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	return Invalidate(flag, buf, NULL, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::IsValidByNum(
		risse_uint32 flag,
		risse_int num,
		iRisseDispatch2 *objthis
		)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	return IsValid(flag, buf, NULL, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::CreateNewByNum(
		risse_uint32 flag,
		risse_int num,
		iRisseDispatch2 **result,
		risse_int numparams,
		tRisseVariant **param,
		iRisseDispatch2 *objthis
		)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	return CreateNew(flag, buf, NULL, result, numparams, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::IsInstanceOfByNum(
		risse_uint32 flag,
		risse_int num,
		const risse_char *classname,
		iRisseDispatch2 *objthis
		)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	return IsInstanceOf(flag, buf, NULL, classname, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::OperationByNum(
		risse_uint32 flag,
		risse_int num,
		tRisseVariant *result,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis
		)
{
	risse_char buf[34];
	Risse_int_to_str(num, buf);
	return Operation(flag, buf, NULL, result, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseDispatch::Operation(
		risse_uint32 flag,
		const risse_char *membername,
		risse_uint32 *hint,
		tRisseVariant *result,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis
	)
{
	risse_uint32 op = flag & RISSE_OP_MASK;

	if(op!=RISSE_OP_INC && op!=RISSE_OP_DEC && param == NULL)
		return RISSE_E_INVALIDPARAM;

	if(op<RISSE_OP_MIN || op>RISSE_OP_MAX)
		return RISSE_E_INVALIDPARAM;

	tRisseVariant tmp;
	risse_error hr;
	hr = PropGet(0, membername, hint, &tmp, objthis);
	if(RISSE_FAILED(hr)) return hr;

	RisseDoVariantOperation(op, tmp, param);

	hr = PropSet(0, membername, hint, &tmp, objthis);
	if(RISSE_FAILED(hr)) return hr;

	if(result) result->CopyRef(tmp);

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
/*C]*/





//---------------------------------------------------------------------------
// property object to get/set missing member
//---------------------------------------------------------------------------
class tRisseSimpleGetSetProperty : public tRisseDispatch
{
private:
	tRisseVariant &Value;

public:
	tRisseSimpleGetSetProperty(tRisseVariant &value) : tRisseDispatch(), Value(value)
	{
	};

	risse_error
	PropGet(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	 tRisseVariant *result,
		iRisseDispatch2 *objthis)
	{
		if(membername) return RISSE_E_MEMBERNOTFOUND;
		if(result) *result = Value;
		return RISSE_S_OK;
	}


	risse_error
	PropSet(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	 const tRisseVariant *param,
		iRisseDispatch2 *objthis)
	{
		if(membername) return RISSE_E_MEMBERNOTFOUND;
		Value = *param;
		return RISSE_S_OK;
	}

	risse_error
	PropSetByVS(risse_uint32 flag, tRisseVariantString *membername,
		const tRisseVariant *param, iRisseDispatch2 *objthis)
	{
		if(membername) return RISSE_E_MEMBERNOTFOUND;
		Value = *param;
		return RISSE_S_OK;
	}

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// magic number for rebuilding hash
//---------------------------------------------------------------------------
static risse_uint RisseGlobalRebuildHashMagic = 0;
void RisseDoRehash() { RisseGlobalRebuildHashMagic ++; }
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseCustomObject
//---------------------------------------------------------------------------
risse_int RisseObjectHashBitsLimit = 32;
static ttstr FinalizeName;
static ttstr MissingName;
//---------------------------------------------------------------------------
void tRisseCustomObject::tRisseSymbolData::ReShare()
{
	// search shared string map using RisseMapGlobalStringMap,
	// and share the name string (if it can)
	if(Name)
	{
		ttstr name(Name);
		Name->Release(), Name = NULL;
		name = RisseMapGlobalStringMap(name);
		Name = name.AsVariantStringNoAddRef();
		Name->AddRef();
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tRisseCustomObject::tRisseCustomObject(risse_int hashbits)
{
	if(RisseObjectHashMapEnabled()) RisseAddObjectHashRecord(this);
	Count = 0;
	RebuildHashMagic = RisseGlobalRebuildHashMagic;
	if(hashbits > RisseObjectHashBitsLimit) hashbits = RisseObjectHashBitsLimit;
	HashSize = (1 << hashbits);
	HashMask = HashSize - 1;
	Symbols = new tRisseSymbolData[HashSize];
	memset(Symbols, 0, sizeof(tRisseSymbolData) * HashSize);
	IsInvalidated = false;
	IsInvalidating = false;
	CallFinalize = true;
	CallMissing = false;
	ProsessingMissing = false;
	if(FinalizeName.IsEmpty())
	{
		// first time; initialize 'finalize' name and 'missing' name
		FinalizeName = RisseMapGlobalStringMap(RISSE_WS("finalize"));
		MissingName  = RisseMapGlobalStringMap(RISSE_WS("missing"));
	}
	finalize_name = FinalizeName;
	missing_name = MissingName;
	for(risse_int i=0; i<RISSE_MAX_NATIVE_CLASS; i++)
		ClassIDs[i] = (risse_int32)-1;
}
//---------------------------------------------------------------------------
tRisseCustomObject::~tRisseCustomObject()
{
	for(risse_int i=RISSE_MAX_NATIVE_CLASS-1; i>=0; i--)
	{
		if(ClassIDs[i]!=-1)
		{
			if(ClassInstances[i]) ClassInstances[i]->Destruct();
		}
	}
	delete [] Symbols;
	if(RisseObjectHashMapEnabled()) RisseRemoveObjectHashRecord(this);
}
//---------------------------------------------------------------------------
void tRisseCustomObject::_Finalize(void)
{
	if(IsInvalidating) return; // to avoid re-entrance
	IsInvalidating = true;
	try
	{
		if(!IsInvalidated)
		{
			Finalize();
			IsInvalidated = true;
		}
	}
	catch(...)
	{
		IsInvalidating = false;
		throw;
	}
	IsInvalidating = false;
}

//---------------------------------------------------------------------------
void tRisseCustomObject::Finalize(void)
{
	// call this object's "finalize"
	if(CallFinalize)
	{
		FuncCall(0, finalize_name.c_str(), finalize_name.GetHint(), NULL, 0,
			NULL, this);
	}

	for(risse_int i=RISSE_MAX_NATIVE_CLASS-1; i>=0; i--)
	{
		if(ClassIDs[i]!=-1)
		{
			if(ClassInstances[i]) ClassInstances[i]->Invalidate();
		}
	}
	DeleteAllMembers();
}
//---------------------------------------------------------------------------
void tRisseCustomObject::BeforeDestruction(void)
{
	if(RisseObjectHashMapEnabled())
		RisseSetObjectHashFlag(this, RISSE_OHMF_DELETING, RISSE_OHMF_SET);
	_Finalize();
}
//---------------------------------------------------------------------------
bool tRisseCustomObject::CallGetMissing(const risse_char *name, tRisseVariant &result)
{
	// call 'missing' method for PopGet
	if(ProsessingMissing) return false;
	ProsessingMissing = true;
	bool res = false;
	try
	{
		tRisseVariant val;
		tRisseSimpleGetSetProperty * prop = new tRisseSimpleGetSetProperty(val);
		try
		{
			tRisseVariant args[3];
			args[0] = (tTVInteger) false; // false: get
			args[1] = name;        // member name
			args[2] = prop;
			tRisseVariant *pargs[3] = {args +0, args +1, args +2};
			tRisseVariant funcresult;
			risse_error er = 
				FuncCall(0, missing_name.c_str(), missing_name.GetHint(), &funcresult,
					3, pargs, this);
			if(RISSE_FAILED(er))
			{
				res = false;
			}
			else
			{
				res = (bool)(risse_int)funcresult;
				result = val;
			}
		}
		catch(...)
		{
			prop->Release();
			throw;
		}
		prop->Release();
	}
	catch(...)
	{
		ProsessingMissing = false;
		throw;
	}
	ProsessingMissing = false;
	return res;
}
//---------------------------------------------------------------------------
bool tRisseCustomObject::CallSetMissing(const risse_char *name, const tRisseVariant &value)
{
	// call 'missing' method for PopSet
	if(ProsessingMissing) return false;
	ProsessingMissing = true;
	bool res = false;
	try
	{
		tRisseVariant val(value);
		tRisseSimpleGetSetProperty * prop = new tRisseSimpleGetSetProperty(val);
		try
		{
			tRisseVariant args[3];
			args[0] = (tTVInteger) true; // true: set
			args[1] = name;        // member name
			args[2] = prop;
			tRisseVariant *pargs[3] = {args +0, args +1, args +2};
			tRisseVariant funcresult;
			risse_error er = 
				FuncCall(0, missing_name.c_str(), missing_name.GetHint(), &funcresult,
					3, pargs, this);
			if(RISSE_FAILED(er))
			{
				res = false;
			}
			else
			{
				res = (bool)(risse_int)funcresult;
			}
		}
		catch(...)
		{
			prop->Release();
			throw;
		}
		prop->Release();
	}
	catch(...)
	{
		ProsessingMissing = false;
		throw;
	}
	ProsessingMissing = false;
	return res;
}
//---------------------------------------------------------------------------
tRisseCustomObject::tRisseSymbolData * tRisseCustomObject::Add(const risse_char * name,
	risse_uint32 *hint)
{
	// add a data element named "name".
	// return existing element if the element named "name" is already alive.

	if(name == NULL)
	{
		return NULL;
	}

	tRisseSymbolData *data;
	data = Find(name, hint);
	if(data)
	{
		// the element is already alive
		return data;
	}

	risse_uint32 hash;
	if(hint && *hint)
		hash = *hint;  // hint must be hash because of previous calling of "Find"
	else
		hash = tRisseHashFunc<risse_char *>::Make(name);

	tRisseSymbolData *lv1 = Symbols + (hash & HashMask);

	if((lv1->SymFlags & RISSE_SYMBOL_USING))
	{
		// lv1 is using
		// make a chain and insert it after lv1

		data = new tRisseSymbolData;

		data->SelfClear();

		data->Next = lv1->Next;
		lv1->Next = data;

		data->SetName(name, hash);
		data->SymFlags |= RISSE_SYMBOL_USING;
	}
	else
	{
		// lv1 is unused
		if(!(lv1->SymFlags & RISSE_SYMBOL_INIT))
		{
			lv1->SelfClear();
		}

		lv1->SetName(name, hash);
		lv1->SymFlags |= RISSE_SYMBOL_USING;
		data = lv1;
	}

	Count++;


	return data;

}
//---------------------------------------------------------------------------
tRisseCustomObject::tRisseSymbolData * tRisseCustomObject::Add(tRisseVariantString * name)
{
	// tRisseVariantString version of above

	if(name == NULL)
	{
		return NULL;
	}

	tRisseSymbolData *data;
	data = Find((const risse_char *)(*name), name->GetHint());
	if(data)
	{
		// the element is already alive
		return data;
	}

	risse_uint32 hash;
	if(*(name->GetHint()))
		hash = *(name->GetHint());  // hint must be hash because of previous calling of "Find"
	else
		hash = tRisseHashFunc<risse_char *>::Make((const risse_char *)(*name));

	tRisseSymbolData *lv1 = Symbols + (hash & HashMask);

	if((lv1->SymFlags & RISSE_SYMBOL_USING))
	{
		// lv1 is using
		// make a chain and insert it after lv1

		data = new tRisseSymbolData;

		data->SelfClear();

		data->Next = lv1->Next;
		lv1->Next = data;

		data->SetName(name, hash);
		data->SymFlags |= RISSE_SYMBOL_USING;
	}
	else
	{
		// lv1 is unused
		if(!(lv1->SymFlags & RISSE_SYMBOL_INIT))
		{
			lv1->SelfClear();
		}

		lv1->SetName(name, hash);
		lv1->SymFlags |= RISSE_SYMBOL_USING;
		data = lv1;
	}

	Count++;


	return data;
}
//---------------------------------------------------------------------------
tRisseCustomObject::tRisseSymbolData * tRisseCustomObject::AddTo(tRisseVariantString *name,
		tRisseSymbolData *newdata, risse_int newhashmask)
{
	// similar to Add, except for adding member to new hash space.
	if(name == NULL)
	{
		return NULL;
	}

	// at this point, the member must not exist in destination hash space

	risse_uint32 hash;
	hash = tRisseHashFunc<risse_char *>::Make((const risse_char *)(*name));

	tRisseSymbolData *lv1 = newdata + (hash & newhashmask);
	tRisseSymbolData *data;

	if((lv1->SymFlags & RISSE_SYMBOL_USING))
	{
		// lv1 is using
		// make a chain and insert it after lv1

		data = new tRisseSymbolData;

		data->SelfClear();

		data->Next = lv1->Next;
		lv1->Next = data;

		data->SetName(name, hash);
		data->SymFlags |= RISSE_SYMBOL_USING;
	}
	else
	{
		// lv1 is unused
		if(!(lv1->SymFlags & RISSE_SYMBOL_INIT))
		{
			lv1->SelfClear();
		}

		lv1->SetName(name, hash);
		lv1->SymFlags |= RISSE_SYMBOL_USING;
		data = lv1;
	}

	// count is not incremented

	return data;
}
//---------------------------------------------------------------------------
#define GetValue(x) (*((tRisseVariant *)(&(x->Value))))
//---------------------------------------------------------------------------
void tRisseCustomObject::RebuildHash()
{
	// rebuild hash table
	RebuildHashMagic = RisseGlobalRebuildHashMagic;

	// decide new hash table size

	risse_int r, v = Count;
	if(v & 0xffff0000) r = 16, v >>= 16; else r = 0;
	if(v & 0xff00) r += 8, v >>= 8;
	if(v & 0xf0) r += 4, v >>= 4;
	v<<=1;
	risse_int newhashbits = r + ((0xffffaa50 >> v) &0x03) + 2;
	if(newhashbits > RisseObjectHashBitsLimit) newhashbits = RisseObjectHashBitsLimit;
	risse_int newhashsize = (1 << newhashbits);


	if(newhashsize == HashSize) return;

	risse_int newhashmask = newhashsize - 1;
	risse_int orgcount = Count;

	// allocate new hash space
	tRisseSymbolData *newsymbols = new tRisseSymbolData[newhashsize];


	// enumerate current symbol and push to new hash space

	try
	{
		memset(newsymbols, 0, sizeof(tRisseSymbolData) * newhashsize);
		tRisseSymbolData * lv1 = Symbols;
		tRisseSymbolData * lv1lim = lv1 + HashSize;
		for(; lv1 < lv1lim; lv1++)
		{
			tRisseSymbolData * d = lv1->Next;
			while(d)
			{
				tRisseSymbolData * nextd = d->Next;
				if(d->SymFlags & RISSE_SYMBOL_USING)
				{
//					d->ReShare();
					tRisseSymbolData *data = AddTo(d->Name, newsymbols, newhashmask);
					if(data)
					{
						GetValue(data).CopyRef(*(tRisseVariant*)(&(d->Value)));
						data->SymFlags &= ~ (RISSE_SYMBOL_HIDDEN | RISSE_SYMBOL_STATIC);
						data->SymFlags |= d->SymFlags & (RISSE_SYMBOL_HIDDEN | RISSE_SYMBOL_STATIC);
						CheckObjectClosureAdd(GetValue(data));
					}
				}
				d = nextd;
			}

			if(lv1->SymFlags & RISSE_SYMBOL_USING)
			{
//				lv1->ReShare();
				tRisseSymbolData *data = AddTo(lv1->Name, newsymbols, newhashmask);
				if(data)
				{
					GetValue(data).CopyRef(*(tRisseVariant*)(&(lv1->Value)));
					data->SymFlags &= ~ (RISSE_SYMBOL_HIDDEN | RISSE_SYMBOL_STATIC);
					data->SymFlags |= lv1->SymFlags & (RISSE_SYMBOL_HIDDEN | RISSE_SYMBOL_STATIC);
					CheckObjectClosureAdd(GetValue(data));
				}
			}
		}
	}
	catch(...)
	{
		// recover
		risse_int _HashMask = HashMask;
		risse_int _HashSize = HashSize;
		tRisseSymbolData * _Symbols = Symbols;

		Symbols = newsymbols;
		HashSize = newhashsize;
		HashMask = newhashmask;

		DeleteAllMembers();
		delete [] Symbols;

		HashMask = _HashMask;
		HashSize = _HashSize;
		Symbols = _Symbols;
		Count = orgcount;

		throw;
	}

	// delete all current members
	DeleteAllMembers();
	delete [] Symbols;

	// assign new members
	Symbols = newsymbols;
	HashSize = newhashsize;
	HashMask = newhashmask;
	Count = orgcount;
}
//---------------------------------------------------------------------------
bool tRisseCustomObject::DeleteByName(const risse_char * name, risse_uint32 *hint)
{
	// TODO: utilize hint
	// find an element named "name" and deletes it
	risse_uint32 hash = tRisseHashFunc<risse_char *>::Make(name);
	tRisseSymbolData * lv1 = Symbols + (hash & HashMask);

	if(!(lv1->SymFlags & RISSE_SYMBOL_USING) && lv1->Next== NULL)
		return false; // not found

	if((lv1->SymFlags & RISSE_SYMBOL_USING) && lv1->NameMatch(name))
	{
		// mark the element place as "unused"
		CheckObjectClosureRemove(*(tRisseVariant*)(&(lv1->Value)));
		lv1->PostClear();
		Count--;
		return true;
	}

	// chain processing
	tRisseSymbolData * d = lv1->Next;
	tRisseSymbolData * prevd = lv1;
	while(d)
	{
		if((d->SymFlags & RISSE_SYMBOL_USING) && d->Hash == hash)
		{
			if(d->NameMatch(name))
			{
				// sever from the chain
				prevd->Next = d->Next;
				CheckObjectClosureRemove(*(tRisseVariant*)(&(d->Value)));
				d->Destory();

				delete d;

				Count--;
				return true;
			}
		}
		prevd = d;
		d = d->Next;
	}

	return false;
}
//---------------------------------------------------------------------------
void tRisseCustomObject::DeleteAllMembers(void)
{
	// delete all members
	if(Count <= 10) return _DeleteAllMembers();

	std::vector<iRisseDispatch2*> vector;
	try
	{
		tRisseSymbolData * lv1, *lv1lim;

		// list all members up that hold object
		lv1 = Symbols;
		lv1lim = lv1 + HashSize;
		for(; lv1 < lv1lim; lv1++)
		{
			tRisseSymbolData * d = lv1->Next;
			while(d)
			{
				tRisseSymbolData * nextd = d->Next;
				if(d->SymFlags & RISSE_SYMBOL_USING)
				{
					if(((tRisseVariant*)(&(d->Value)))->Type() == tvtObject)
					{
						CheckObjectClosureRemove(*(tRisseVariant*)(&(d->Value)));
						tRisseVariantClosure clo =
							((tRisseVariant*)(&(d->Value)))->AsObjectClosureNoAddRef();
						clo.AddRef();
						if(clo.Object) vector.push_back(clo.Object);
						if(clo.ObjThis) vector.push_back(clo.ObjThis);
						((tRisseVariant*)(&(d->Value)))->Clear();
					}
				}
				d = nextd;
			}

			if(lv1->SymFlags & RISSE_SYMBOL_USING)
			{
				if(((tRisseVariant*)(&(lv1->Value)))->Type() == tvtObject)
				{
					CheckObjectClosureRemove(*(tRisseVariant*)(&(lv1->Value)));
					tRisseVariantClosure clo =
						((tRisseVariant*)(&(lv1->Value)))->AsObjectClosureNoAddRef();
					clo.AddRef();
					if(clo.Object) vector.push_back(clo.Object);
					if(clo.ObjThis) vector.push_back(clo.ObjThis);
					((tRisseVariant*)(&(lv1->Value)))->Clear();
				}
			}
		}

		// delete all members
		lv1 = Symbols;
		lv1lim = lv1 + HashSize;
		for(; lv1 < lv1lim; lv1++)
		{
			tRisseSymbolData * d = lv1->Next;
			while(d)
			{
				tRisseSymbolData * nextd = d->Next;
				if(d->SymFlags & RISSE_SYMBOL_USING)
				{
					d->Destory();
				}

				delete d;

				d = nextd;
			}

			if(lv1->SymFlags & RISSE_SYMBOL_USING)
			{
				lv1->PostClear();
			}

			lv1->Next = NULL;
		}

		Count = 0;
	}
	catch(...)
	{
		std::vector<iRisseDispatch2*>::iterator i;
		for(i = vector.begin(); i != vector.end(); i++)
		{
			(*i)->Release();
		}

		throw;
	}

	// release all objects
	std::vector<iRisseDispatch2*>::iterator i;
	for(i = vector.begin(); i != vector.end(); i++)
	{
		(*i)->Release();
	}
}
//---------------------------------------------------------------------------
void tRisseCustomObject::_DeleteAllMembers(void)
{
	iRisseDispatch2 * dsps[20];
	risse_int num_dsps = 0;

	try
	{
		tRisseSymbolData * lv1, *lv1lim;

		// list all members up that hold object
		lv1 = Symbols;
		lv1lim = lv1 + HashSize;
		for(; lv1 < lv1lim; lv1++)
		{
			tRisseSymbolData * d = lv1->Next;
			while(d)
			{
				tRisseSymbolData * nextd = d->Next;
				if(d->SymFlags & RISSE_SYMBOL_USING)
				{
					if(((tRisseVariant*)(&(d->Value)))->Type() == tvtObject)
					{
						CheckObjectClosureRemove(*(tRisseVariant*)(&(d->Value)));
						tRisseVariantClosure clo =
							((tRisseVariant*)(&(d->Value)))->AsObjectClosureNoAddRef();
						clo.AddRef();
						if(clo.Object) dsps[num_dsps++] = clo.Object;
						if(clo.ObjThis) dsps[num_dsps++] = clo.ObjThis;
						((tRisseVariant*)(&(d->Value)))->Clear();
					}
				}
				d = nextd;
			}

			if(lv1->SymFlags & RISSE_SYMBOL_USING)
			{
				if(((tRisseVariant*)(&(lv1->Value)))->Type() == tvtObject)
				{
					CheckObjectClosureRemove(*(tRisseVariant*)(&(lv1->Value)));
					tRisseVariantClosure clo =
						((tRisseVariant*)(&(lv1->Value)))->AsObjectClosureNoAddRef();
					clo.AddRef();
					if(clo.Object) dsps[num_dsps++] = clo.Object;
					if(clo.ObjThis) dsps[num_dsps++] = clo.ObjThis;
					((tRisseVariant*)(&(lv1->Value)))->Clear();
				}
			}
		}

		// delete all members
		lv1 = Symbols;
		lv1lim = lv1 + HashSize;
		for(; lv1 < lv1lim; lv1++)
		{
			tRisseSymbolData * d = lv1->Next;
			while(d)
			{
				tRisseSymbolData * nextd = d->Next;
				if(d->SymFlags & RISSE_SYMBOL_USING)
				{
					d->Destory();
				}

				delete d;

				d = nextd;
			}

			if(lv1->SymFlags & RISSE_SYMBOL_USING)
			{
				lv1->PostClear();
			}

			lv1->Next = NULL;
		}

		Count = 0;
	}
	catch(...)
	{
		for(int i = 0; i<num_dsps; i++)
		{
			dsps[i]->Release();
		}
		throw;
	}

	// release all objects
	for(int i = 0; i<num_dsps; i++)
	{
		dsps[i]->Release();
	}

}
//---------------------------------------------------------------------------
tRisseCustomObject::tRisseSymbolData * tRisseCustomObject::Find(const risse_char * name,
	risse_uint32 *hint)
{
	// searche an element named "name" and return its "SymbolData".
	// return NULL if the element is not found.

	if(!name) return NULL;

	if(hint && *hint)
	{
		// try finding via hint
		// search over the chain
		risse_uint32 hash = *hint;
		risse_int cnt = 0;
		tRisseSymbolData * lv1 = Symbols + (hash & HashMask);
		tRisseSymbolData * prevd = lv1;
		tRisseSymbolData * d = lv1->Next;
		for(; d; prevd = d, d=d->Next, cnt++)
		{
			if(d->Hash == hash && (d->SymFlags & RISSE_SYMBOL_USING))
			{
				if(d->NameMatch(name))
				{
					if(cnt>2)
					{
						// move to first
						prevd->Next = d->Next;
						d->Next = lv1->Next;
						lv1->Next = d;
					}
					return d;
				}
			}
		}

		if(lv1->Hash == hash && (lv1->SymFlags & RISSE_SYMBOL_USING))
		{
			if(lv1->NameMatch(name))
			{
				return lv1;
			}
		}
	}

	risse_uint32 hash = tRisseHashFunc<risse_char *>::Make(name);
	if(hint && *hint)
	{
		if(*hint == hash) return NULL;
			// given hint was not differ from the hash;
			// we already know that the member was not found.
	}

	if(hint) *hint = hash;

	tRisseSymbolData * lv1 = Symbols + (hash & HashMask);

	if(!(lv1->SymFlags & RISSE_SYMBOL_USING) && lv1->Next == NULL)
		return NULL; // lv1 is unused and does not have any chains

	// search over the chain
	risse_int cnt = 0;
	tRisseSymbolData * prevd = lv1;
	tRisseSymbolData * d = lv1->Next;
	for(; d; prevd = d, d=d->Next, cnt++)
	{
		if(d->Hash == hash && (d->SymFlags & RISSE_SYMBOL_USING))
		{
			if(d->NameMatch(name))
			{
				if(cnt>2)
				{
					// move to first
					prevd->Next = d->Next;
					d->Next = lv1->Next;
					lv1->Next = d;
				}
				return d;
			}
		}
	}

	if(lv1->Hash == hash && (lv1->SymFlags & RISSE_SYMBOL_USING))
	{
		if(lv1->NameMatch(name))
		{
			return lv1;
		}
	}

	return NULL;

}
//---------------------------------------------------------------------------
bool tRisseCustomObject::CallEnumCallbackForData(
	risse_uint32 flags, tRisseVariant ** params,
	tRisseVariantClosure & callback, iRisseDispatch2 * objthis,
	const tRisseCustomObject::tRisseSymbolData * data)
{
	risse_uint32 newflags = 0;
	if(data->SymFlags & RISSE_SYMBOL_HIDDEN) newflags |= RISSE_HIDDENMEMBER;
	if(data->SymFlags & RISSE_SYMBOL_STATIC) newflags |= RISSE_STATICMEMBER;

	*params[0] = data->Name;
	*params[1] = (tTVInteger)newflags;

	if(!(flags & RISSE_ENUM_NO_VALUE))
	{
		// get value
		if(RISSE_FAILED(RisseDefaultPropGet(flags, *(tRisseVariant*)(&(data->Value)),
			params[2], objthis))) return false;
	}

	tRisseVariant res;
	if(RISSE_FAILED(callback.FuncCall(0, NULL, NULL, &res,
		(flags & RISSE_ENUM_NO_VALUE) ? 2 : 3, params, NULL))) return false;
	return (bool)(risse_int)(res);
}
//---------------------------------------------------------------------------
void tRisseCustomObject::InternalEnumMembers(risse_uint32 flags,
	tRisseVariantClosure *callback, iRisseDispatch2 *objthis)
{
	// enumlate members by calling callback.
	// note that member changes(delete or insert) through this function is not guaranteed.
	if(!callback) return;

	tRisseVariant name;
	tRisseVariant newflags;
	tRisseVariant value;
	tRisseVariant * params[3] = { &name, &newflags, &value };

	const tRisseSymbolData * lv1 = Symbols;
	const tRisseSymbolData * lv1lim = lv1 + HashSize;
	for(; lv1 < lv1lim; lv1++)
	{
		const tRisseSymbolData * d = lv1->Next;
		while(d)
		{
			const tRisseSymbolData * nextd = d->Next;

			if(d->SymFlags & RISSE_SYMBOL_USING)
			{
				if(!CallEnumCallbackForData(flags, params, *callback, objthis, d)) return ;
			}
			d = nextd;
		}

		if(lv1->SymFlags & RISSE_SYMBOL_USING)
		{
			if(!CallEnumCallbackForData(flags, params, *callback, objthis, lv1)) return ;
		}
	}
}
//---------------------------------------------------------------------------
risse_int tRisseCustomObject::GetValueInteger(const risse_char * name, risse_uint32 *hint)
{
	tRisseSymbolData *data = Find(name, hint);
	if(!data) return -1;
	return (risse_int)data->Value.Integer;
}
//---------------------------------------------------------------------------
risse_error RisseTryFuncCallViaPropGet(tRisseVariantClosure tvclosure,
	risse_uint32 flag, tRisseVariant *result,
	risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis)
{
	// retry using PropGet
	tRisseVariant tmp;
	tvclosure.AddRef();
	risse_error er;
	try
	{
		er = tvclosure.Object->PropGet(0, NULL, NULL, &tmp,
			RISSE_SELECT_OBJTHIS(tvclosure, objthis));
	}
	catch(...)
	{
		tvclosure.Release();
		throw;
	}
	tvclosure.Release();

	if(RISSE_SUCCEEDED(er))
	{
		tvclosure = tmp.AsObjectClosure();
		er =
			tvclosure.Object->FuncCall(flag, NULL, NULL, result,
				numparams, param, RISSE_SELECT_OBJTHIS(tvclosure, objthis));
		tvclosure.Release();
	}
	return er;
}
//---------------------------------------------------------------------------
risse_error RisseDefaultFuncCall(risse_uint32 flag, tRisseVariant &targ, tRisseVariant *result,
	risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis)
{
	if(targ.Type() == tvtObject)
	{
		risse_error er = RISSE_E_INVALIDTYPE;
		tRisseVariantClosure tvclosure = targ.AsObjectClosure();
		try
		{
			if(tvclosure.Object)
			{
				// bypass
				er =
					tvclosure.Object->FuncCall(flag, NULL, NULL, result,
						numparams, param, RISSE_SELECT_OBJTHIS(tvclosure, objthis));
				if(er == RISSE_E_INVALIDTYPE)
				{
					// retry using PropGet
					er = RisseTryFuncCallViaPropGet(tvclosure, flag, result,
						numparams, param, objthis);
				}
			}
		}
		catch(...)
		{
			tvclosure.Release();
			throw;
		}
		tvclosure.Release();
		return er;
	}

	return RISSE_E_INVALIDTYPE;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::FuncCall(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	tRisseVariant *result, risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		// this function is called as to call a default method,
		// but this object is not a function.
		return RISSE_E_INVALIDTYPE; // so returns RISSE_E_INVALIDTYPE
	}

	tRisseSymbolData *data =  Find(membername, hint);

	if(!data)
	{
		if(CallMissing)
		{
			// call 'missing' method
			tRisseVariant value_func;
			if(CallGetMissing(membername, value_func))
				return RisseDefaultFuncCall(flag, value_func, result, numparams, param, objthis);
		}

		return RISSE_E_MEMBERNOTFOUND; // member not found
	}

	return RisseDefaultFuncCall(flag, GetValue(data), result, numparams, param, objthis);
}
//---------------------------------------------------------------------------
risse_error RisseDefaultPropGet(risse_uint32 flag, tRisseVariant &targ, tRisseVariant *result,
	iRisseDispatch2 *objthis)
{
	if(!(flag & RISSE_IGNOREPROP))
	{
		// if RISSE_IGNOREPROP is not specified

		// if member's type is tvtObject, call the object's PropGet with "member=NULL"
		//  ( default member invocation ). if it is succeeded, return its return value.
		// if the PropGet's return value is RISSE_E_ACCESSDENYED,
		// return as an error, otherwise return the member itself.
		if(targ.Type() == tvtObject)
		{
			tRisseVariantClosure tvclosure = targ.AsObjectClosure();
			risse_error hr = RISSE_E_NOTIMPL;
			try
			{
				if(tvclosure.Object)
				{
					hr = tvclosure.Object->PropGet(0, NULL, NULL, result,
						RISSE_SELECT_OBJTHIS(tvclosure, objthis));
				}
			}
			catch(...)
			{
				tvclosure.Release();
				throw;
			}
			tvclosure.Release();
			if(RISSE_SUCCEEDED(hr)) return hr;
			if(hr != RISSE_E_NOTIMPL && hr != RISSE_E_INVALIDTYPE &&
				hr != RISSE_E_INVALIDOBJECT)
				return hr;
		}
	}

	// return the member itself
	if(!result) return RISSE_E_INVALIDPARAM;

	result->CopyRef(targ);

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::PropGet(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	tRisseVariant *result,
	iRisseDispatch2 *objthis)
{
	if(RebuildHashMagic != RisseGlobalRebuildHashMagic)
	{
		RebuildHash();
	}


	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		// this object itself has no information on PropGet with membername == NULL
		return RISSE_E_INVALIDTYPE;
	}


	tRisseSymbolData * data = Find(membername, hint);
	if(!data)
	{
		if(CallMissing)
		{
			// call 'missing' method
			tRisseVariant value;
			if(CallGetMissing(membername, value))
				return RisseDefaultPropGet(flag, value, result, objthis);
		}
	}

	if(!data && flag & RISSE_MEMBERENSURE)
	{
		// create a member when RISSE_MEMBERENSURE is specified
		data = Add(membername, hint);
	}

	if(!data) return RISSE_E_MEMBERNOTFOUND; // not found

	return RisseDefaultPropGet(flag, GetValue(data), result, objthis);
}
//---------------------------------------------------------------------------
risse_error RisseDefaultPropSet(risse_uint32 flag, tRisseVariant &targ, const tRisseVariant *param,
	iRisseDispatch2 *objthis)
{
	if(!(flag & RISSE_IGNOREPROP))
	{
		if(targ.Type() == tvtObject)
		{
			// roughly the same as RisseDefaultPropGet
			tRisseVariantClosure tvclosure = targ.AsObjectClosure();
			risse_error hr = RISSE_E_NOTIMPL;
			try
			{
				if(tvclosure.Object)
				{
					hr = tvclosure.Object->PropSet(0, NULL, NULL, param,
						RISSE_SELECT_OBJTHIS(tvclosure, objthis));
				}
			}
			catch(...)
			{
				tvclosure.Release();
				throw;
			}
			tvclosure.Release();
			if(RISSE_SUCCEEDED(hr)) return hr;
			if(hr != RISSE_E_NOTIMPL && hr != RISSE_E_INVALIDTYPE &&
				hr != RISSE_E_INVALIDOBJECT)
				return hr;
		}
	}

	// normal substitution
	if(!param) return RISSE_E_INVALIDPARAM;

	targ.CopyRef(*param);

	return RISSE_S_OK;

}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::PropSet(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		// no action is defined with the default member
		return RISSE_E_INVALIDTYPE;
	}

	tRisseSymbolData * data;
	if(CallMissing)
	{
		data = Find(membername, hint);
		if(!data)
		{
			// call 'missing' method
			if(CallSetMissing(membername, *param))
				return RISSE_S_OK;
		}
	}

	if(flag & RISSE_MEMBERENSURE)
		data = Add(membername, hint); // create a member when RISSE_MEMBERENSURE is specified
	else
		data = Find(membername, hint);

	if(!data) return RISSE_E_MEMBERNOTFOUND; // not found

	if(flag & RISSE_HIDDENMEMBER)
		data->SymFlags |= RISSE_SYMBOL_HIDDEN;
	else
		data->SymFlags &= ~RISSE_SYMBOL_HIDDEN;

	if(flag & RISSE_STATICMEMBER)
		data->SymFlags |= RISSE_SYMBOL_STATIC;
	else
		data->SymFlags &= ~RISSE_SYMBOL_STATIC;

	//-- below is mainly the same as RisseDefaultPropSet

	if(!(flag & RISSE_IGNOREPROP))
	{
		if(GetValue(data).Type() == tvtObject)
		{
			tRisseVariantClosure tvclosure =
				GetValue(data).AsObjectClosureNoAddRef();
			if(tvclosure.Object)
			{
				risse_error hr = tvclosure.Object->PropSet(0, NULL, NULL, param,
					RISSE_SELECT_OBJTHIS(tvclosure, objthis));
				if(RISSE_SUCCEEDED(hr)) return hr;
				if(hr != RISSE_E_NOTIMPL && hr != RISSE_E_INVALIDTYPE &&
					hr != RISSE_E_INVALIDOBJECT)
					return hr;
			}
			data = Find(membername, hint);
		}
	}


	if(!param) return RISSE_E_INVALIDPARAM;

	CheckObjectClosureRemove(GetValue(data));
	try
	{
		GetValue(data).CopyRef(*param);
	}
	catch(...)
	{
		CheckObjectClosureAdd(GetValue(data));
		throw;
	}
	CheckObjectClosureAdd(GetValue(data));

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::GetCount(risse_int *result, const risse_char *membername, risse_uint32 *hint,
	iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(!result) return RISSE_E_INVALIDPARAM;

	*result = Count;

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::PropSetByVS(risse_uint32 flag, tRisseVariantString *membername,
	const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		// no action is defined with the default member
		return RISSE_E_INVALIDTYPE;
	}

	tRisseSymbolData * data;
	if(CallMissing)
	{
		data = Find((const risse_char *)(*membername), membername->GetHint());
		if(!data)
		{
			// call 'missing' method
			if(CallSetMissing((const risse_char *)(*membername), *param))
				return RISSE_S_OK;
		}
	}

	if(flag & RISSE_MEMBERENSURE)
		data = Add(membername); // create a member when RISSE_MEMBERENSURE is specified
	else
		data = Find((const risse_char *)(*membername), membername->GetHint());

	if(!data) return RISSE_E_MEMBERNOTFOUND; // not found

	if(flag & RISSE_HIDDENMEMBER)
		data->SymFlags |= RISSE_SYMBOL_HIDDEN;
	else
		data->SymFlags &= ~RISSE_SYMBOL_HIDDEN;

	if(flag & RISSE_STATICMEMBER)
		data->SymFlags |= RISSE_SYMBOL_STATIC;
	else
		data->SymFlags &= ~RISSE_SYMBOL_STATIC;

	//-- below is mainly the same as RisseDefaultPropSet

	if(!(flag & RISSE_IGNOREPROP))
	{
		if(GetValue(data).Type() == tvtObject)
		{
			tRisseVariantClosure tvclosure =
				GetValue(data).AsObjectClosureNoAddRef();
			if(tvclosure.Object)
			{
				risse_error hr = tvclosure.Object->PropSet(0, NULL, NULL, param,
					RISSE_SELECT_OBJTHIS(tvclosure, objthis));
				if(RISSE_SUCCEEDED(hr)) return hr;
				if(hr != RISSE_E_NOTIMPL && hr != RISSE_E_INVALIDTYPE &&
					hr != RISSE_E_INVALIDOBJECT)
					return hr;
			}
			data = Find((const risse_char *)(*membername), membername->GetHint());
		}
	}


	if(!param) return RISSE_E_INVALIDPARAM;

	CheckObjectClosureRemove(GetValue(data));
	try
	{
		GetValue(data).CopyRef(*param);
	}
	catch(...)
	{
		CheckObjectClosureAdd(GetValue(data));
		throw;
	}
	CheckObjectClosureAdd(GetValue(data));

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::EnumMembers(risse_uint32 flag, tRisseVariantClosure *callback, iRisseDispatch2 *objthis)
{
	if(!GetValidity()) return RISSE_E_INVALIDOBJECT;

	InternalEnumMembers(flag, callback, objthis);

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::DeleteMember(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		return RISSE_E_MEMBERNOTFOUND;
	}

	if(!DeleteByName(membername, hint)) return RISSE_E_MEMBERNOTFOUND;

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
risse_error RisseDefaultInvalidate(risse_uint32 flag, tRisseVariant &targ, iRisseDispatch2 *objthis)
{

	if(targ.Type() == tvtObject)
	{
		tRisseVariantClosure tvclosure = targ.AsObjectClosureNoAddRef();
		if(tvclosure.Object)
		{
			// bypass
			return
				tvclosure.Object->Invalidate(flag, NULL, NULL,
					RISSE_SELECT_OBJTHIS(tvclosure, objthis));
		}
	}

	return RISSE_E_INVALIDTYPE;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::Invalidate(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		if(IsInvalidated) return RISSE_S_FALSE;
		_Finalize();
		return RISSE_S_TRUE;
	}

	tRisseSymbolData * data = Find(membername, hint);

	if(!data)
	{
		if(CallMissing)
		{
			// call 'missing' method
			tRisseVariant value;
			if(CallGetMissing(membername, value))
				return RisseDefaultInvalidate(flag, value, objthis);
		}
	}

	if(!data) return RISSE_E_MEMBERNOTFOUND; // not found

	return RisseDefaultInvalidate(flag, GetValue(data), objthis);
}
//---------------------------------------------------------------------------
risse_error RisseDefaultIsValid(risse_uint32 flag, tRisseVariant &targ, iRisseDispatch2 * objthis)
{
	if(targ.Type() == tvtObject)
	{
		tRisseVariantClosure tvclosure =targ.AsObjectClosureNoAddRef();
		if(tvclosure.Object)
		{
			// bypass
			return
				tvclosure.Object->IsValid(flag, NULL, NULL,
					RISSE_SELECT_OBJTHIS(tvclosure, objthis));
		}
	}

	// the target type is not tvtObject
	return RISSE_S_TRUE;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::IsValid(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	iRisseDispatch2 *objthis)
{
	if(membername == NULL)
	{
		if(IsInvalidated) return RISSE_S_FALSE;
		return RISSE_S_TRUE;
	}

	tRisseSymbolData * data = Find(membername, hint);

	if(!data)
	{
		if(CallMissing)
		{
			// call 'missing' method
			tRisseVariant value;
			if(CallGetMissing(membername, value))
				return RisseDefaultIsValid(flag, value, objthis);
		}
	}

	if(!data) return RISSE_E_MEMBERNOTFOUND; // not found

	return RisseDefaultIsValid(flag, GetValue(data), objthis);
}
//---------------------------------------------------------------------------
risse_error RisseDefaultCreateNew(risse_uint32 flag, tRisseVariant &targ,
	iRisseDispatch2 **result, risse_int numparams, tRisseVariant **param,
	iRisseDispatch2 *objthis)
{
	if(targ.Type() == tvtObject)
	{
		tRisseVariantClosure tvclosure = targ.AsObjectClosureNoAddRef();
		if(tvclosure.Object)
		{
			// bypass
			return
				tvclosure.Object->CreateNew(flag, NULL, NULL, result, numparams,
					param, RISSE_SELECT_OBJTHIS(tvclosure, objthis));
		}
	}

	return RISSE_E_INVALIDTYPE;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::CreateNew(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	iRisseDispatch2 **result, risse_int numparams, tRisseVariant **param,
	iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		// as an action of the default member, this object cannot create an object
		// because this object is not a class
		return RISSE_E_INVALIDTYPE;
	}

	tRisseSymbolData * data = Find(membername, hint);

	if(!data)
	{
		if(CallMissing)
		{
			// call 'missing' method
			tRisseVariant value;
			if(CallGetMissing(membername, value))
				return RisseDefaultCreateNew(flag, value, result, numparams, param, objthis);
		}
	}

	if(!data) return RISSE_E_MEMBERNOTFOUND; // not found

	return RisseDefaultCreateNew(flag, GetValue(data), result, numparams, param, objthis);
}
//---------------------------------------------------------------------------
/*
risse_error
tRisseCustomObject::GetSuperClass(risse_uint32 flag, iRisseDispatch2 **result,
		iRisseDispatch2 *objthis)
{
	// TODO: GetSuperClass's reason for being
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	return RISSE_E_NOTIMPL;
}
*/
//---------------------------------------------------------------------------
risse_error RisseDefaultIsInstanceOf(risse_uint32 flag, tRisseVariant &targ, const risse_char *name,
	iRisseDispatch2 *objthis)
{
	tRisseVariantType vt;
	vt = targ.Type();
	if(vt == tvtVoid)
	{
		return RISSE_S_FALSE;
	}

	if(!Risse_strcmp(name, RISSE_WS("Object"))) return RISSE_S_TRUE;


	switch(vt)
	{
	case tvtVoid:
		return RISSE_S_FALSE; // returns always false about tvtVoid
	case tvtInteger:
	case tvtReal:
		if(!Risse_strcmp(name, RISSE_WS("Number"))) return RISSE_S_TRUE;
		return RISSE_S_FALSE;
	case tvtString:
		if(!Risse_strcmp(name, RISSE_WS("String"))) return RISSE_S_TRUE;
		return RISSE_S_FALSE;
	case tvtOctet:
		if(!Risse_strcmp(name, RISSE_WS("Octet"))) return RISSE_S_TRUE;
		return RISSE_S_FALSE;
	case tvtObject:
		if(vt == tvtObject)
		{
			tRisseVariantClosure tvclosure =targ.AsObjectClosureNoAddRef();
			if(tvclosure.Object)
			{
				// bypass
				return
					tvclosure.Object->IsInstanceOf(flag, NULL, NULL, name,
						RISSE_SELECT_OBJTHIS(tvclosure, objthis));
			}
			return RISSE_S_FALSE;
		}
	}

	return RISSE_S_FALSE;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::IsInstanceOf(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	const risse_char *classname, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		// always returns true if "Object" is specified
		if(!Risse_strcmp(classname, RISSE_WS("Object")))
		{
			return RISSE_S_TRUE;
		}

		// look for the class instance information
		for(risse_uint i=0; i<ClassNames.size(); i++)
		{
			if(!Risse_strcmp(ClassNames[i].c_str(), classname))
			{
				return RISSE_S_TRUE;
			}
		}

		return RISSE_S_FALSE;
	}

	tRisseSymbolData * data = Find(membername, hint);

	if(!data)
	{
		if(CallMissing)
		{
			// call 'missing' method
			tRisseVariant value;
			if(CallGetMissing(membername, value))
				return RisseDefaultIsInstanceOf(flag, value, classname, objthis);
		}
	}

	if(!data) return RISSE_E_MEMBERNOTFOUND; // not found

	return RisseDefaultIsInstanceOf(flag, GetValue(data), classname, objthis);
}
//---------------------------------------------------------------------------
risse_error RisseDefaultOperation(risse_uint32 flag, tRisseVariant &targ,
	tRisseVariant *result, const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	risse_uint32 op = flag & RISSE_OP_MASK;

	if(op!=RISSE_OP_INC && op!=RISSE_OP_DEC && param == NULL)
		return RISSE_E_INVALIDPARAM;

	if(op<RISSE_OP_MIN || op>RISSE_OP_MAX)
		return RISSE_E_INVALIDPARAM;

	if(targ.Type() == tvtObject)
	{
		// the member may be a property handler if the member's type is "tvtObject"
		// so here try to access the object.
		risse_error hr;

		tRisseVariantClosure tvclosure;
		tvclosure = targ.AsObjectClosureNoAddRef();
		if(tvclosure.Object)
		{
			iRisseDispatch2 * ot = RISSE_SELECT_OBJTHIS(tvclosure, objthis);

			tRisseVariant tmp;
			hr = tvclosure.Object->PropGet(0, NULL, NULL, &tmp, ot);
			if(RISSE_SUCCEEDED(hr))
			{
				RisseDoVariantOperation(op, tmp, param);

				hr = tvclosure.Object->PropSet(0, NULL, NULL, &tmp, ot);
				if(RISSE_FAILED(hr)) return hr;

				if(result) result->CopyRef(tmp);

				return RISSE_S_OK;
			}
			else if(hr != RISSE_E_NOTIMPL && hr != RISSE_E_INVALIDTYPE &&
				hr != RISSE_E_INVALIDOBJECT)
			{
				return hr;
			}

			// normal operation is proceeded if "PropGet" is failed.
		}
	}

	RisseDoVariantOperation(op, targ, param);

	if(result) result->CopyRef(targ);

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::Operation(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	tRisseVariant *result, const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	// operation about the member
	// processing line is the same as above function

	if(membername == NULL)
	{
		return RISSE_E_INVALIDTYPE;
	}

	risse_uint32 op = flag & RISSE_OP_MASK;

	if(op!=RISSE_OP_INC && op!=RISSE_OP_DEC && param == NULL)
		return RISSE_E_INVALIDPARAM;

	if(op<RISSE_OP_MIN || op>RISSE_OP_MAX)
		return RISSE_E_INVALIDPARAM;

	tRisseSymbolData * data = Find(membername, hint);

	if(!data)
	{
		if(CallMissing)
		{
			// call default operation
			return inherited::Operation(flag, membername, hint, result, param, objthis);
		}
	}

	if(!data) return RISSE_E_MEMBERNOTFOUND; // not found

	if(GetValue(data).Type() == tvtObject)
	{
		risse_error hr;

		tRisseVariantClosure tvclosure;
		tvclosure = GetValue(data).AsObjectClosureNoAddRef();
		if(tvclosure.Object)
		{
			iRisseDispatch2 * ot = RISSE_SELECT_OBJTHIS(tvclosure, objthis);

			tRisseVariant tmp;
			hr = tvclosure.Object->PropGet(0, NULL, NULL, &tmp, ot);
			if(RISSE_SUCCEEDED(hr))
			{
				RisseDoVariantOperation(op, tmp, param);

				hr = tvclosure.Object->PropSet(0, NULL, NULL, &tmp, ot);
				if(RISSE_FAILED(hr)) return hr;

				if(result) result->CopyRef(tmp);

				return RISSE_S_OK;
			}
			else if(hr != RISSE_E_NOTIMPL && hr != RISSE_E_INVALIDTYPE &&
				hr != RISSE_E_INVALIDOBJECT)
			{
				return hr;
			}
		}
	}


	CheckObjectClosureRemove(GetValue(data));

	tRisseVariant &tmp = GetValue(data);
	try
	{
		RisseDoVariantOperation(op, tmp, param);
	}
	catch(...)
	{
		CheckObjectClosureAdd(GetValue(data));
		throw;
	}
	CheckObjectClosureAdd(GetValue(data));

	if(result) result->CopyRef(tmp);

	return RISSE_S_OK;

}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::NativeInstanceSupport(risse_uint32 flag, risse_int32 classid,
		iRisseNativeInstance **pointer)
{
	if(flag == RISSE_NIS_GETINSTANCE)
	{
		// search "classid"
		for(risse_int i=0; i<RISSE_MAX_NATIVE_CLASS; i++)
		{
			if(ClassIDs[i] == classid)
			{
				if(pointer) *pointer = ClassInstances[i];
				return RISSE_S_OK;
			}
		}
		return RISSE_E_FAIL;
	}

	if(flag == RISSE_NIS_REGISTER)
	{
		// search for the empty place
		for(risse_int i=0; i<RISSE_MAX_NATIVE_CLASS; i++)
		{
			if(ClassIDs[i] == -1)
			{
				// found... writes there
				if(!pointer) return RISSE_E_FAIL;
				ClassIDs[i] = classid;
				ClassInstances[i] = *pointer;
				return RISSE_S_OK;
			}
		}
		return RISSE_E_FAIL;
	}

	return RISSE_E_NOTIMPL;
}
//---------------------------------------------------------------------------
risse_error
tRisseCustomObject::ClassInstanceInfo(risse_uint32 flag, risse_uint num, tRisseVariant *value)
{
	switch(flag)
	{
	case RISSE_CII_ADD:
	  {
		// add value
		ttstr name = value->AsStringNoAddRef();
		if(RisseObjectHashMapEnabled() && ClassNames.size() == 0)
			RisseObjectHashSetType(this, RISSE_WS("instance of class ") + name);
				// First class name is used for the object classname
				// because the order of the class name
				// registration is from descendant to ancestor.
		ClassNames.push_back(name);
		return RISSE_S_OK;
	  }

	case RISSE_CII_GET:
	  {
		// get value
		if(num>=ClassNames.size()) return RISSE_E_FAIL;
		*value = ClassNames[num];
		return RISSE_S_OK;
	  }

	case RISSE_CII_SET_FINALIZE:
	  {
		// set 'finalize' method name
		finalize_name = *value;
		CallFinalize = !finalize_name.IsEmpty();
		return RISSE_S_OK;
	  }

	case RISSE_CII_SET_MISSING:
	  {
		// set 'missing' method name
		missing_name = *value;
		CallMissing = !missing_name.IsEmpty();
		return RISSE_S_OK;
	  }


	}

	return RISSE_E_NOTIMPL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// RisseCreateCustomObject
//---------------------------------------------------------------------------
iRisseDispatch2 * RisseCreateCustomObject()
{
	// utility function; returns newly created empty tRisseCustomObject
	return new tRisseCustomObject();
}
//---------------------------------------------------------------------------


}

