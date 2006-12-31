//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Object クラスの実装
//---------------------------------------------------------------------------
#ifndef risseObjectH
#define risseObjectH

#include <vector>
#include "risseConfig.h"
#include "risseInterface.h"
#include "risseVariant.h"
#include "risseUtils.h"
#include "risseError.h"

namespace Risse
{
//---------------------------------------------------------------------------
// utility functions
//---------------------------------------------------------------------------
extern risse_error
	RisseDefaultFuncCall(risse_uint32 flag, tRisseVariant &targ, tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis);

extern risse_error RisseDefaultPropGet(risse_uint32 flag, tRisseVariant &targ, tRisseVariant *result,
	iRisseDispatch2 *objthis);

extern risse_error RisseDefaultPropSet(risse_uint32 flag, tRisseVariant &targ, const tRisseVariant *param,
	iRisseDispatch2 *objthis);

extern risse_error
	RisseDefaultInvalidate(risse_uint32 flag, tRisseVariant &targ, iRisseDispatch2 *objthis);

extern risse_error
	RisseDefaultIsValid(risse_uint32 flag, tRisseVariant &targ, iRisseDispatch2 * objthis);

extern risse_error
	RisseDefaultCreateNew(risse_uint32 flag, tRisseVariant &targ,
		iRisseDispatch2 **result, risse_int numparams, tRisseVariant **param,
		iRisseDispatch2 *objthis);

extern risse_error
	RisseDefaultIsInstanceOf(risse_uint32 flag, tRisseVariant &targ, const risse_char *name,
		iRisseDispatch2 *objthis);

extern risse_error
	RisseDefaultOperation(risse_uint32 flag, tRisseVariant &targ,
		tRisseVariant *result, const tRisseVariant *param, iRisseDispatch2 *objthis);

void RisseDoVariantOperation(risse_int op, tRisseVariant &target, const tRisseVariant *param);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// hash rebuilding
//---------------------------------------------------------------------------
void RisseDoRehash();
//---------------------------------------------------------------------------



/*[*/
//---------------------------------------------------------------------------
// tRisseDispatch
//---------------------------------------------------------------------------
/*
	tRisseDispatch is a base class that implements iRisseDispatch2, and every methods.
	most methods are maked as simply returning "RISSE_E_NOTIMPL";
	methods, those access the object by index, call another methods that access
	the object by string.
*/
/*
#define RISSE_SELECT_OBJTHIS(__closure__, __override__) \
	((__closure__).ObjThis?((__override__)?(__override__):(__closure__).ObjThis):(__override__))
*/
#define RISSE_SELECT_OBJTHIS(__closure__, __override__) \
	((__closure__).ObjThis?(__closure__).ObjThis:(__override__))

class tRisseDispatch : public iRisseDispatch2
{
	virtual void BeforeDestruction(void) {;}
	bool BeforeDestructionCalled;
		// BeforeDestruction will be certainly called before object destruction
private:
	risse_uint RefCount;
public:
	tRisseDispatch();
	virtual ~tRisseDispatch();

//	bool DestructionTrace;

public:
	risse_uint AddRef(void);
	risse_uint Release(void);

protected:
	risse_uint GetRefCount() { return RefCount; }

public:
	risse_error
	FuncCall(
		risse_uint32 flag,
		const risse_char * membername,
		risse_uint32 *hint,
		tRisseVariant *result,
		risse_int numparams,
		tRisseVariant **param,
		iRisseDispatch2 *objthis
		)
	{
		return membername?RISSE_E_MEMBERNOTFOUND:RISSE_E_NOTIMPL;
	}

	risse_error
	FuncCallByNum(
		risse_uint32 flag,
		risse_int num,
		tRisseVariant *result,
		risse_int numparams,
		tRisseVariant **param,
		iRisseDispatch2 *objthis
		);

	risse_error
	PropGet(
		risse_uint32 flag,
		const risse_char * membername,
		risse_uint32 *hint,
		tRisseVariant *result,
		iRisseDispatch2 *objthis
		)
	{
		return membername?RISSE_E_MEMBERNOTFOUND:RISSE_E_NOTIMPL;
	}

	risse_error
	PropGetByNum(
		risse_uint32 flag,
		risse_int num,
		tRisseVariant *result,
		iRisseDispatch2 *objthis
		);

	risse_error
	PropSet(
		risse_uint32 flag,
		const risse_char *membername,
		risse_uint32 *hint,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis
		)
	{
		return membername?RISSE_E_MEMBERNOTFOUND:RISSE_E_NOTIMPL;
	}

	risse_error
	PropSetByNum(
		risse_uint32 flag,
		risse_int num,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis
		);
	
	risse_error
	GetCount(
		risse_int *result,
		const risse_char *membername,
		risse_uint32 *hint,
		iRisseDispatch2 *objthis
		)
	{
		return RISSE_E_NOTIMPL;
	}

	risse_error
	GetCountByNum(
		risse_int *result,
		risse_int num,
		iRisseDispatch2 *objthis
		);


	risse_error
	PropSetByVS(risse_uint32 flag, tRisseVariantString *membername,
		const tRisseVariant *param, iRisseDispatch2 *objthis)
	{
		return RISSE_E_NOTIMPL;
	}

	risse_error
	EnumMembers(risse_uint32 flag, tRisseVariantClosure *callback, iRisseDispatch2 *objthis)
	{
		return RISSE_E_NOTIMPL;
	}

	risse_error
	DeleteMember(
		risse_uint32 flag,
		const risse_char *membername,
		risse_uint32 *hint,
		iRisseDispatch2 *objthis
		)
	{
		return membername?RISSE_E_MEMBERNOTFOUND:RISSE_E_NOTIMPL;
	}

	risse_error
	DeleteMemberByNum(
		risse_uint32 flag,
		risse_int num,
		iRisseDispatch2 *objthis
		);

	risse_error
	Invalidate(
		risse_uint32 flag,
		const risse_char *membername,
		risse_uint32 *hint,
		iRisseDispatch2 *objthis
		)
	{
		return membername?RISSE_E_MEMBERNOTFOUND:RISSE_E_NOTIMPL;
	}

	risse_error
	InvalidateByNum(
		risse_uint32 flag,
		risse_int num,
		iRisseDispatch2 *objthis
		);

	risse_error
	IsValid(
		risse_uint32 flag,
		const risse_char *membername,
		risse_uint32 *hint,
		iRisseDispatch2 *objthis
		)
	{
		return membername?RISSE_E_MEMBERNOTFOUND:RISSE_E_NOTIMPL;
	}

	risse_error
	IsValidByNum(
		risse_uint32 flag,
		risse_int num,
		iRisseDispatch2 *objthis
		);

	risse_error
	CreateNew(
		risse_uint32 flag,
		const risse_char * membername,
		risse_uint32 *hint,
		iRisseDispatch2 **result,
		risse_int numparams,
		tRisseVariant **param,
		iRisseDispatch2 *objthis
		)
	{
		return membername?RISSE_E_MEMBERNOTFOUND:RISSE_E_NOTIMPL;
	}

	risse_error
	CreateNewByNum(
		risse_uint32 flag,
		risse_int num,
		iRisseDispatch2 **result,
		risse_int numparams,
		tRisseVariant **param,
		iRisseDispatch2 *objthis
		);

	risse_error
	Reserved1(
		)
	{
		return RISSE_E_NOTIMPL;
	}


	risse_error
	IsInstanceOf(
		risse_uint32 flag,
		const risse_char *membername,
		risse_uint32 *hint,
		const risse_char *classname,
		iRisseDispatch2 *objthis
		)
	{
		return membername?RISSE_E_MEMBERNOTFOUND:RISSE_E_NOTIMPL;
	}

	risse_error
	IsInstanceOfByNum(
		risse_uint32 flag,
		risse_int num,
		const risse_char *classname,
		iRisseDispatch2 *objthis
		);

	risse_error
	Operation(
		risse_uint32 flag,
		const risse_char *membername,
		risse_uint32 *hint,
		tRisseVariant *result,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis
		);

	risse_error
	OperationByNum(
		risse_uint32 flag,
		risse_int num,
		tRisseVariant *result,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis
		);


	risse_error
	NativeInstanceSupport(
		risse_uint32 flag,
		risse_int32 classid,
		iRisseNativeInstance **pointer
		)
	{
		return RISSE_E_NOTIMPL;
	}

	risse_error
	ClassInstanceInfo(
		risse_uint32 flag,
		risse_uint num,
		tRisseVariant *value
		)
	{
		return RISSE_E_NOTIMPL;
	}

	risse_error
	Reserved2(
		)
	{
		return RISSE_E_NOTIMPL;
	}

	risse_error
	Reserved3(
		)
	{
		return RISSE_E_NOTIMPL;
	}


};
//---------------------------------------------------------------------------
/*]*/




//---------------------------------------------------------------------------
// tRisseCustomObject
//---------------------------------------------------------------------------

#define RISSE_NAMESPACE_DEFAULT_HASH_BITS 3

extern risse_int RisseObjectHashBitsLimit;
	// this limits hash table size


#define RISSE_SYMBOL_USING	0x1
#define RISSE_SYMBOL_INIT     0x2
#define RISSE_SYMBOL_HIDDEN   0x8
#define RISSE_SYMBOL_STATIC	0x10

#define RISSE_MAX_NATIVE_CLASS 4
/*
	Number of "Native Class Instance" that can be used per a risse Object is
	limited as the number above.
*/


class tRisseCustomObject : public tRisseDispatch
{
	typedef tRisseDispatch inherited;

	// tRisseSymbolData -----------------------------------------------------
public:
	struct tRisseSymbolData
	{
		tRisseVariantString *Name; // name
		risse_uint32 Hash; // hash code of the name
		risse_uint32 SymFlags; // management flags
		risse_uint32 Flags;  // flags

		tRisseVariant_S Value; // the value
			/*
				TRisseVariant_S must work with construction that fills
					all member to zero.
			*/

		tRisseSymbolData * Next; // next chain

		void SelfClear(void)
		{
			memset(this, 0, sizeof(*this));
			SymFlags = RISSE_SYMBOL_INIT;
		}

		void _SetName(const risse_char * name)
		{
			if(Name) Name->Release(), Name = NULL;
			if(!name) Risse_eRisseError(RisseIDExpected);
			if(!name[0]) Risse_eRisseError(RisseIDExpected);
			Name = RisseAllocVariantString(name);
		}

		void SetName(const risse_char * name, risse_uint32 hash)
		{
			_SetName(name);
			Hash = hash;
		}

		void _SetName(tRisseVariantString *name)
		{
			if(name == Name) return;
			if(Name) Name->Release();
			Name = name;
			if(Name) Name->AddRef();
		}

		void SetName(tRisseVariantString *name, risse_uint32 hash)
		{
			_SetName(name);
			Hash = hash;
		}

		const risse_char * GetName() const
		{
			return (const risse_char *)(*Name);
		}

		void PostClear()
		{
			if(Name) Name->Release(), Name = NULL;
			((tRisseVariant*)(&Value))->~tRisseVariant();
			memset(&Value, 0, sizeof(Value));
			SymFlags &= ~RISSE_SYMBOL_USING;
		}

		void Destory()
		{
			if(Name) Name->Release();
			((tRisseVariant*)(&Value))->~tRisseVariant();
		}

		bool NameMatch(const risse_char * name)
		{
			const risse_char * this_name = GetName();
			if(this_name == name) return true;
			return !Risse_strcmp(name, this_name);
		}

		void ReShare();
	};



	//---------------------------------------------------------------------
	risse_int Count;
	risse_int HashMask;
	risse_int HashSize;
	tRisseSymbolData * Symbols;
	risse_uint RebuildHashMagic;
	bool IsInvalidated;
	bool IsInvalidating;
	iRisseNativeInstance* ClassInstances[RISSE_MAX_NATIVE_CLASS];
	risse_int32 ClassIDs[RISSE_MAX_NATIVE_CLASS];


	void _Finalize(void);

	//---------------------------------------------------------------------
protected:
	bool GetValidity() const { return !IsInvalidated; }
	bool CallFinalize; // set false if this object does not need to call "finalize"
	ttstr finalize_name; // name of the 'finalize' method
	bool CallMissing; // set true if this object should call 'missing' method
	bool ProsessingMissing; // true if 'missing' method is being called
	ttstr missing_name; // name of the 'missing' method
	virtual void Finalize(void);
	std::vector<ttstr > ClassNames;

	//---------------------------------------------------------------------
public:

	tRisseCustomObject(risse_int hashbits = RISSE_NAMESPACE_DEFAULT_HASH_BITS);
	~tRisseCustomObject();

private:
	void BeforeDestruction(void);

	void CheckObjectClosureAdd(const tRisseVariant &val)
	{
		// adjust the reference counter when the object closure's "objthis" is
		// referring to the object itself.
		if(val.Type() == tvtObject)
		{
			iRisseDispatch2 *dsp = val.AsObjectClosureNoAddRef().ObjThis;
			if(dsp == (iRisseDispatch2*)this) this->Release();
		}
	}

	void CheckObjectClosureRemove(const tRisseVariant &val)
	{
		if(val.Type() == tvtObject)
		{
			iRisseDispatch2 *dsp = val.AsObjectClosureNoAddRef().ObjThis;
			if(dsp == (iRisseDispatch2*)this) this->AddRef();
		}
	}

	bool CallGetMissing(const risse_char *name, tRisseVariant &result);
	bool CallSetMissing(const risse_char *name, const tRisseVariant &value);

	tRisseSymbolData * Add(const risse_char * name, risse_uint32 *hint);
		// Adds the symbol, returns the newly created data;
		// if already exists, returns the data.

	tRisseSymbolData * Add(tRisseVariantString * name);
		// tRisseVariantString version of above.

	tRisseSymbolData * AddTo(tRisseVariantString *name,
		tRisseSymbolData *newdata, risse_int newhashmask);
		// Adds member to the new hash space, used in RebuildHash

	void RebuildHash(); // rebuild hash table

	bool DeleteByName(const risse_char * name, risse_uint32 *hint);
		// Deletes Name

	void DeleteAllMembers(void);
		// Deletes all members
	void _DeleteAllMembers(void);
		// Deletes all members ( not to use std::vector )

	tRisseSymbolData * Find(const risse_char * name, risse_uint32 *hint) ;
		// Finds Name, returns its data; if not found, returns NULL

	static bool CallEnumCallbackForData(risse_uint32 flags,
		tRisseVariant ** params,
		tRisseVariantClosure & callback, iRisseDispatch2 * objthis,
		const tRisseSymbolData * data);
	void InternalEnumMembers(risse_uint32 flags, tRisseVariantClosure *callback,
		iRisseDispatch2 *objthis);
	//---------------------------------------------------------------------
public:
	void Clear() { DeleteAllMembers(); }

	//---------------------------------------------------------------------
public:
	risse_int GetValueInteger(const risse_char * name, risse_uint32 *hint);
		// service function for lexical analyzer
	//---------------------------------------------------------------------
public:

	risse_error
	FuncCall(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis);

	risse_error
	PropGet(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	 tRisseVariant *result,
		iRisseDispatch2 *objthis);

	risse_error
	PropSet(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	 const tRisseVariant *param,
		iRisseDispatch2 *objthis);

	risse_error
	GetCount(risse_int *result, const risse_char *membername, risse_uint32 *hint,
	 iRisseDispatch2 *objthis);

	risse_error
	PropSetByVS(risse_uint32 flag, tRisseVariantString *membername,
		const tRisseVariant *param, iRisseDispatch2 *objthis);

	risse_error
	EnumMembers(risse_uint32 flag, tRisseVariantClosure *callback, iRisseDispatch2 *objthis);

	risse_error
	DeleteMember(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	 iRisseDispatch2 *objthis);

	risse_error
	Invalidate(risse_uint32 flag, const risse_char *membername,  risse_uint32 *hint,
	iRisseDispatch2 *objthis);

	risse_error
	IsValid(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	 iRisseDispatch2 *objthis);

	risse_error
	CreateNew(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	 iRisseDispatch2 **result,
		risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis);

	risse_error
	IsInstanceOf(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	 const risse_char *classname,
		iRisseDispatch2 *objthis);

	risse_error
	Operation(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	 tRisseVariant *result,
		const tRisseVariant *param,	iRisseDispatch2 *objthis);

	risse_error
	NativeInstanceSupport(risse_uint32 flag, risse_int32 classid,
		iRisseNativeInstance **pointer);

	risse_error
	ClassInstanceInfo(risse_uint32 flag, risse_uint num, tRisseVariant *value);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// RisseCreateCustomObject
//---------------------------------------------------------------------------
iRisseDispatch2 *RisseCreateCustomObject();
//---------------------------------------------------------------------------


} // namespace Risse


#endif
