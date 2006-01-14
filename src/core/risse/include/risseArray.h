//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 配列クラスの実装
//---------------------------------------------------------------------------

#ifndef risseArrayH
#define risseArrayH


#include <deque>
#include "risseNative.h"


// note: this Risse class cannot be inherited

namespace Risse
{
//---------------------------------------------------------------------------
// tRisseStringAppender
//---------------------------------------------------------------------------
class tRisseStringAppender
{
	// fast operation for string concat

	risse_char * Data;
	risse_int DataLen;
	risse_int DataCapacity; // in characters, not in bytes

public:
	tRisseStringAppender();
	~tRisseStringAppender();

	risse_int GetLen() const { return DataLen; }
	const risse_char * GetData() const { return Data; }
	void Append(const risse_char *string, risse_int len);

	void operator += (const risse_char * string)
		{ Append(string, Risse_strlen(string)); }

	void operator += (const ttstr & string)
		{ Append(string.c_str(), string.GetLen()); }
};
//---------------------------------------------------------------------------
// tRisseSaveStructuredDataCallback
//---------------------------------------------------------------------------
struct tRisseSaveStructuredDataCallback
{
	virtual void SaveStructuredData(std::vector<iRisseDispatch2 *> &stack,
		tRisseStringAppender & string, const ttstr&indentstr) = 0;

};
//---------------------------------------------------------------------------
// tRisseArrayClass : tRisseArray Risse class
//---------------------------------------------------------------------------
class tRisseArrayClass : public tRisseNativeClass
{
	typedef tRisseNativeClass inherited;

public:
	tRisseArrayClass();
	~tRisseArrayClass();

protected:
	tRisseNativeInstance *CreateNativeInstance();
	iRisseDispatch2 *CreateBaseRisseObject();

private:

	static risse_uint32 ClassID;

protected:
};
//---------------------------------------------------------------------------
// tRisseArrayNI : Risse Array native C++ instance
//---------------------------------------------------------------------------
class tRisseArrayNI : public tRisseNativeInstance,
					public tRisseSaveStructuredDataCallback
{
	typedef tRisseNativeInstance inherited;
public:
	typedef std::vector<tRisseVariant>::iterator tArrayItemIterator;
	std::vector<tRisseVariant> Items;
	tRisseArrayNI();

	risse_error Construct(risse_int numparams, tRisseVariant **params,
		iRisseDispatch2 *risseobj);

	void Assign(iRisseDispatch2 *dsp);

private:
	struct tDictionaryEnumCallback : public tRisseDispatch
	{
		std::vector<tRisseVariant> * Items;
		
		risse_error
		FuncCall(risse_uint32 flag, const risse_char * membername,
			risse_uint32 *hint, tRisseVariant *result, risse_int numparams,
			tRisseVariant **param, iRisseDispatch2 *objthis);
	};
	friend class tDictionaryEnumCallback;

public:
	void SaveStructuredData(std::vector<iRisseDispatch2 *> &stack,
		tRisseStringAppender & string, const ttstr&indentstr);
		// method from tRisseSaveStructuredDataCallback
	static void SaveStructuredDataForObject(iRisseDispatch2 *dsp,
		std::vector<iRisseDispatch2 *> &stack, tRisseStringAppender &string, const ttstr&indentstr);

	void AssignStructure(iRisseDispatch2 * dsp, std::vector<iRisseDispatch2 *> &stack);
//---------------------------------------------------------------------------
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class tRisseArrayObject : public tRisseCustomObject
{
	typedef tRisseCustomObject inherited;

	void CheckObjectClosureAdd(const tRisseVariant &val)
	{
		// member's object closure often points the container,
		// so we must adjust the reference counter to avoid
		// mutual reference lock.
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



public:
	tRisseArrayObject();
	~tRisseArrayObject();

protected:
	void Finalize(); // Finalize override

public:
	void Clear(tRisseArrayNI *ni);

	void Add(tRisseArrayNI *ni, const tRisseVariant &val);
	risse_int Remove(tRisseArrayNI *ni, const tRisseVariant &ref, bool removeall);
	void Erase(tRisseArrayNI *ni, risse_int num);
	void Insert(tRisseArrayNI *ni, const tRisseVariant &val, risse_int num);

public:
	risse_error
	FuncCall(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis);

	risse_error
	FuncCallByNum(risse_uint32 flag, risse_int num, tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis);

	risse_error
	PropGet(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
		iRisseDispatch2 *objthis);

	risse_error
	PropGetByNum(risse_uint32 flag, risse_int num, tRisseVariant *result,
		iRisseDispatch2 *objthis);

	risse_error
	PropSet(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis);

	risse_error
	PropSetByNum(risse_uint32 flag, risse_int num, const tRisseVariant *param,
		iRisseDispatch2 *objthis);

/*
	GetCount
	GetCountByNum
*/

	risse_error
	PropSetByVS(risse_uint32 flag, tRisseVariantString *membername,
		const tRisseVariant *param, iRisseDispatch2 *objthis);

	risse_error
	EnumMembers(risse_uint32 flag, tRisseVariantClosure *callback, iRisseDispatch2 *objthis)
	{
		return RISSE_E_NOTIMPL; // currently not implemented
	}

	risse_error
	DeleteMember(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		iRisseDispatch2 *objthis);

	risse_error
	DeleteMemberByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 *objthis);

	risse_error
	Invalidate(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		iRisseDispatch2 *objthis);

	risse_error
	InvalidateByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 *objthis);

	risse_error
	IsValid(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		iRisseDispatch2 *objthis);

	risse_error
	IsValidByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 *objthis);

	risse_error
	CreateNew(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		iRisseDispatch2 **result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis);

	risse_error
	CreateNewByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 **result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis);
/*
	risse_error
	GetSuperClass(risse_uint32 flag, iRisseDispatch2 **result, iRisseDispatch2 *objthis);
*/
	risse_error
	IsInstanceOf(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		const risse_char *classname,
		iRisseDispatch2 *objthis);

	risse_error
	IsInstanceOfByNum(risse_uint32 flag, risse_int num, const risse_char *classname,
		iRisseDispatch2 *objthis);

	risse_error
	Operation(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		tRisseVariant *result,
		const tRisseVariant *param, iRisseDispatch2 *objthis);

	risse_error
	OperationByNum(risse_uint32 flag, risse_int num, tRisseVariant *result,
		const tRisseVariant *param, iRisseDispatch2 *objthis);
/*
	risse_error
	NativeInstanceSupport(risse_uint32 flag, risse_int32 classid,
		tRisseNativeInstance **pointer);
*/
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// RisseGetArrayClassID
//---------------------------------------------------------------------------
extern risse_int32 RisseGetArrayClassID();
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// RisseCreateArrayObject
//---------------------------------------------------------------------------
iRisseDispatch2 *RisseCreateArrayObject(iRisseDispatch2 **classout = NULL);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Utility functions
//---------------------------------------------------------------------------
risse_int RisseGetArrayElementCount(iRisseDispatch2 * dsp);
//---------------------------------------------------------------------------
risse_int RisseCopyArrayElementTo(iRisseDispatch2 * dsp, tRisseVariant *dest, risse_uint start, risse_int count);
//---------------------------------------------------------------------------


} // namespace Risse

#endif
