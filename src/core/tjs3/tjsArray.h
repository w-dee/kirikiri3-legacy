//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS Array class implementation
//---------------------------------------------------------------------------

#ifndef tjsArrayH
#define tjsArrayH


#include <deque>
#include "tjsNative.h"


// note: this TJS class cannot be inherited

namespace TJS
{
//---------------------------------------------------------------------------
// tTJSStringAppender
//---------------------------------------------------------------------------
class tTJSStringAppender
{
	// fast operation for string concat

	tjs_char * Data;
	tjs_int DataLen;
	tjs_int DataCapacity; // in characters, not in bytes

public:
	tTJSStringAppender();
	~tTJSStringAppender();

	tjs_int GetLen() const { return DataLen; }
	const tjs_char * GetData() const { return Data; }
	void Append(const tjs_char *string, tjs_int len);

	void operator += (const tjs_char * string)
		{ Append(string, TJS_strlen(string)); }

	void operator += (const ttstr & string)
		{ Append(string.c_str(), string.GetLen()); }
};
//---------------------------------------------------------------------------
// tTJSSaveStructuredDataCallback
//---------------------------------------------------------------------------
struct tTJSSaveStructuredDataCallback
{
	virtual void SaveStructuredData(std::vector<iTJSDispatch2 *> &stack,
		tTJSStringAppender & string, const ttstr&indentstr) = 0;

};
//---------------------------------------------------------------------------
// tTJSArrayClass : tTJSArray TJS class
//---------------------------------------------------------------------------
class tTJSArrayClass : public tTJSNativeClass
{
	typedef tTJSNativeClass inherited;

public:
	tTJSArrayClass();
	~tTJSArrayClass();

protected:
	tTJSNativeInstance *CreateNativeInstance();
	iTJSDispatch2 *CreateBaseTJSObject();

private:

	static tjs_uint32 ClassID;

protected:
};
//---------------------------------------------------------------------------
// tTJSArrayNI : TJS Array native C++ instance
//---------------------------------------------------------------------------
class tTJSArrayNI : public tTJSNativeInstance,
					public tTJSSaveStructuredDataCallback
{
	typedef tTJSNativeInstance inherited;
public:
	typedef std::vector<tTJSVariant>::iterator tArrayItemIterator;
	std::vector<tTJSVariant> Items;
	tTJSArrayNI();

	tjs_error Construct(tjs_int numparams, tTJSVariant **params,
		iTJSDispatch2 *tjsobj);

	void Assign(iTJSDispatch2 *dsp);

private:
	struct tDictionaryEnumCallback : public tTJSDispatch
	{
		std::vector<tTJSVariant> * Items;
		
		tjs_error
		FuncCall(tjs_uint32 flag, const tjs_char * membername,
			tjs_uint32 *hint, tTJSVariant *result, tjs_int numparams,
			tTJSVariant **param, iTJSDispatch2 *objthis);
	};
	friend class tDictionaryEnumCallback;

public:
	void SaveStructuredData(std::vector<iTJSDispatch2 *> &stack,
		tTJSStringAppender & string, const ttstr&indentstr);
		// method from tTJSSaveStructuredDataCallback
	static void SaveStructuredDataForObject(iTJSDispatch2 *dsp,
		std::vector<iTJSDispatch2 *> &stack, tTJSStringAppender &string, const ttstr&indentstr);

	void AssignStructure(iTJSDispatch2 * dsp, std::vector<iTJSDispatch2 *> &stack);
//---------------------------------------------------------------------------
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class tTJSArrayObject : public tTJSCustomObject
{
	typedef tTJSCustomObject inherited;

	void CheckObjectClosureAdd(const tTJSVariant &val)
	{
		// member's object closure often points the container,
		// so we must adjust the reference counter to avoid
		// mutual reference lock.
		if(val.Type() == tvtObject)
		{
			iTJSDispatch2 *dsp = val.AsObjectClosureNoAddRef().ObjThis;
			if(dsp == (iTJSDispatch2*)this) this->Release();
		}
	}

	void CheckObjectClosureRemove(const tTJSVariant &val)
	{
		if(val.Type() == tvtObject)
		{
			iTJSDispatch2 *dsp = val.AsObjectClosureNoAddRef().ObjThis;
			if(dsp == (iTJSDispatch2*)this) this->AddRef();
		}
	}



public:
	tTJSArrayObject();
	~tTJSArrayObject();

protected:
	void Finalize(); // Finalize override

public:
	void Clear(tTJSArrayNI *ni);

	void Add(tTJSArrayNI *ni, const tTJSVariant &val);
	tjs_int Remove(tTJSArrayNI *ni, const tTJSVariant &ref, bool removeall);
	void Erase(tTJSArrayNI *ni, tjs_int num);
	void Insert(tTJSArrayNI *ni, const tTJSVariant &val, tjs_int num);

public:
	tjs_error
	FuncCall(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);

	tjs_error
	FuncCallByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);

	tjs_error
	PropGet(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		iTJSDispatch2 *objthis);

	tjs_error
	PropGetByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result,
		iTJSDispatch2 *objthis);

	tjs_error
	PropSet(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis);

	tjs_error
	PropSetByNum(tjs_uint32 flag, tjs_int num, const tTJSVariant *param,
		iTJSDispatch2 *objthis);

/*
	GetCount
	GetCountByNum
*/

	tjs_error
	PropSetByVS(tjs_uint32 flag, tTJSVariantString *membername,
		const tTJSVariant *param, iTJSDispatch2 *objthis);

	tjs_error
	EnumMembers(tjs_uint32 flag, tTJSVariantClosure *callback, iTJSDispatch2 *objthis)
	{
		return TJS_E_NOTIMPL; // currently not implemented
	}

	tjs_error
	DeleteMember(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		iTJSDispatch2 *objthis);

	tjs_error
	DeleteMemberByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis);

	tjs_error
	Invalidate(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		iTJSDispatch2 *objthis);

	tjs_error
	InvalidateByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis);

	tjs_error
	IsValid(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		iTJSDispatch2 *objthis);

	tjs_error
	IsValidByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis);

	tjs_error
	CreateNew(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		iTJSDispatch2 **result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);

	tjs_error
	CreateNewByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 **result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
/*
	tjs_error
	GetSuperClass(tjs_uint32 flag, iTJSDispatch2 **result, iTJSDispatch2 *objthis);
*/
	tjs_error
	IsInstanceOf(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		const tjs_char *classname,
		iTJSDispatch2 *objthis);

	tjs_error
	IsInstanceOfByNum(tjs_uint32 flag, tjs_int num, const tjs_char *classname,
		iTJSDispatch2 *objthis);

	tjs_error
	Operation(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		tTJSVariant *result,
		const tTJSVariant *param, iTJSDispatch2 *objthis);

	tjs_error
	OperationByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result,
		const tTJSVariant *param, iTJSDispatch2 *objthis);
/*
	tjs_error
	NativeInstanceSupport(tjs_uint32 flag, tjs_int32 classid,
		tTJSNativeInstance **pointer);
*/
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TJSGetArrayClassID
//---------------------------------------------------------------------------
extern tjs_int32 TJSGetArrayClassID();
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TJSCreateArrayObject
//---------------------------------------------------------------------------
iTJSDispatch2 *TJSCreateArrayObject(iTJSDispatch2 **classout = NULL);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Utility functions
//---------------------------------------------------------------------------
tjs_int TJSGetArrayElementCount(iTJSDispatch2 * dsp);
//---------------------------------------------------------------------------
tjs_int TJSCopyArrayElementTo(iTJSDispatch2 * dsp, tTJSVariant *dest, tjs_uint start, tjs_int count);
//---------------------------------------------------------------------------


} // namespace TJS

#endif
