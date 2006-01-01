//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief tTJSVariant (バリアント型) クラスの実装
//---------------------------------------------------------------------------
#ifndef tjsVariantH
#define tjsVariantH

#include <stdlib.h>
//#include <stdexcept>


#include "tjsInterface.h"
#include "tjsVariantString.h"
#include "tjsString.h"

namespace TJS
{
void TJSThrowNullAccess();
void TJSThrowDivideByZero();
//---------------------------------------------------------------------------
class tTJSVariantString;
class tTJSString;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSVariantOctet
//---------------------------------------------------------------------------

#pragma pack(push, 4)
struct tTJSVariantOctet_S
{
	tjs_uint Length;
	tjs_int RefCount;
	tjs_uint8 *Data;
};
#pragma pack(pop)

class tTJSVariantOctet : protected tTJSVariantOctet_S
{
public:
	tTJSVariantOctet(const tjs_uint8 *data, tjs_uint length);
	tTJSVariantOctet(const tjs_uint8 *data1, tjs_uint len1, const tjs_uint8 *data2,
		tjs_uint len2);
	tTJSVariantOctet(const tTJSVariantOctet *o1, const tTJSVariantOctet *o2);
	~tTJSVariantOctet();

	void AddRef()
	{
		RefCount ++;
	}

	void Release();

	tjs_uint GetLength() const
	{
		return Length;
	}

	const tjs_uint8 *GetData() const
	{
		return Data;
	}

	tjs_int QueryPersistSize() { return sizeof(tjs_uint) + Length; }
	void Persist(tjs_uint8 * dest)
	{
		*(tjs_uint*)dest = Length;
		if(Data) TJS_octetcpy(dest + sizeof(tjs_uint), Data, Length);
	}
};
//---------------------------------------------------------------------------
tTJSVariantOctet *TJSAllocVariantOctet(const tjs_uint8 *data, tjs_uint length);
tTJSVariantOctet *TJSAllocVariantOctet(const tjs_uint8 *data1, tjs_uint len1,
	const tjs_uint8 *data2, tjs_uint len2);
tTJSVariantOctet *TJSAllocVariantOctet(const tTJSVariantOctet *o1, const
	tTJSVariantOctet *o2);
tTJSVariantOctet *TJSAllocVariantOctet(const tjs_uint8 **src);
void TJSDeallocVariantOctet(tTJSVariantOctet *o);
tTJSVariantString *TJSOctetToListString(const tTJSVariantOctet *oct);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSVariant_S
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma option push -b
#endif
enum tTJSVariantType
{
	tvtVoid,  // empty
	tvtObject,
	tvtString,
	tvtOctet,  // octet binary data
	tvtInteger,
	tvtReal
};
#ifdef __BORLANDC__
#pragma option pop
#endif
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma option push -b -a4
#endif

#pragma pack(push, 4)
class iTJSDispatch2;
struct tTJSVariantClosure_S
{
	iTJSDispatch2 *Object;
	iTJSDispatch2 *ObjThis;
};
class tTJSVariantClosure;

class tTJSVariantString;
class tTJSVariantOctet;
struct tTJSVariant_S
{
	//---- data members -----------------------------------------------------

	#define tTJSVariant_BITCOPY(a,b) \
	{\
		*(tTJSVariant_S*)&(a) = *(tTJSVariant_S*)&(b); \
	}

	union
	{
		tTJSVariantClosure_S Object;
		tTVInteger Integer;
		tTVReal Real;
		tTJSVariantString *String;
		tTJSVariantOctet *Octet;
	};
	tTJSVariantType vt;
};
#pragma pack(pop)

#ifdef __BORLANDC__
#pragma option pop
#endif
//---------------------------------------------------------------------------
// tTJSVariantClosure
//---------------------------------------------------------------------------
extern tTJSVariantClosure_S TJSNullVariantClosure;

//---------------------------------------------------------------------------
// tTJSVariantClosure
//---------------------------------------------------------------------------
class tTJSVariantClosure : public tTJSVariantClosure_S
{
	// tTJSVariantClosure does not provide any function of object lifetime
	// namagement. ( AddRef and Release are provided but tTJSVariantClosure
	// has no responsibility for them )

public:
	tTJSVariantClosure() {;} // note that default constructor does nothing 

	tTJSVariantClosure(iTJSDispatch2 *obj, iTJSDispatch2 *objthis = NULL)
	{ Object = obj, ObjThis = objthis; }

	iTJSDispatch2 * SelectObjectNoAddRef()
		{ return ObjThis ? ObjThis : Object; }

public:

	bool operator == (const tTJSVariantClosure &rhs)
	{
		return Object == rhs.Object && ObjThis == rhs.ObjThis;
	}

	bool operator != (const tTJSVariantClosure &rhs)
	{
		return ! this->operator ==(rhs);
	}


	void AddRef()
	{
		if(Object) Object->AddRef();
		if(ObjThis) ObjThis->AddRef();
	}

	void Release()
	{
		if(Object) Object->Release();
		if(ObjThis) ObjThis->Release();
	}


	tjs_error
	FuncCall(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->FuncCall(flag, membername, hint, result, numparams, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	FuncCallByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->FuncCallByNum(flag, num, result, numparams, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	PropGet(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->PropGet(flag, membername, hint, result,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	PropGetByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result,
		iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->PropGetByNum(flag, num, result,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	PropSet(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->PropSet(flag, membername, hint, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	PropSetByNum(tjs_uint32 flag, tjs_int num, const tTJSVariant *param,
		iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->PropSetByNum(flag, num, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	GetCount(tjs_int *result, const tjs_char *membername, tjs_uint32 *hint,
		iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->GetCount(result, membername, hint,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	GetCountByNum(tjs_int *result, tjs_int num, iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->GetCountByNum(result, num,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	PropSetByVS(tjs_uint32 flag, tTJSVariantString *membername,
		const tTJSVariant *param, iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->PropSetByVS(flag, membername, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	EnumMembers(tjs_uint32 flag, tTJSVariantClosure *callback,
		iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->EnumMembers(flag, callback,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	DeleteMember(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->DeleteMember(flag, membername, hint,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	DeleteMemberByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->DeleteMemberByNum(flag, num,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	Invalidate(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->Invalidate(flag, membername, hint,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	InvalidateByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->InvalidateByNum(flag, num,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	IsValid(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->IsValid(flag, membername, hint,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	IsValidByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->IsValidByNum(flag, num,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	CreateNew(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		iTJSDispatch2 **result,
		tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->CreateNew(flag, membername, hint, result, numparams,
			param, ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	CreateNewByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 **result,
		tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->CreateNewByNum(flag, num, result, numparams, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

/*
	tjs_error
	Reserved1() { }
*/

	tjs_error
	IsInstanceOf(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		const tjs_char *classname, iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->IsInstanceOf(flag, membername, hint, classname,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	IsInstanceOf(tjs_uint32 flag, tjs_int num, tjs_char *classname,
		iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->IsInstanceOfByNum(flag, num, classname,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	Operation(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		tTJSVariant *result, const tTJSVariant *param,	iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->Operation(flag, membername, hint, result, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	tjs_error
	OperationByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result,
		const tTJSVariant *param,	iTJSDispatch2 *objthis) const
	{
		if(!Object) TJSThrowNullAccess();
		return Object->OperationByNum(flag, num, result, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

/*
	tjs_error
	Reserved2() { }
*/

/*
	tjs_error
	Reserved3() { }
*/

};




//---------------------------------------------------------------------------
tTJSVariantString *TJSObjectToString(const tTJSVariantClosure &dsp);
tTJSVariantString *TJSIntegerToString(tjs_int64 i);
tTJSVariantString *TJSRealToString(tjs_real r);
tTJSVariantString *TJSRealToHexString(tjs_real r);
tTVInteger TJSStringToInteger(const tjs_char *str);
tTVReal TJSStringToReal(const tjs_char *str);
//---------------------------------------------------------------------------
extern void TJSThrowVariantConvertError(const tTJSVariant & from, tTJSVariantType to);
extern void TJSThrowVariantConvertError(const tTJSVariant & from, tTJSVariantType to1,
	tTJSVariantType to2);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSVariant
//---------------------------------------------------------------------------
class tTJSVariant : protected tTJSVariant_S
{

	//---- object management ------------------------------------------------
private:

	void AddRefObject()
	{
		if(Object.Object) Object.Object->AddRef();
		if(Object.ObjThis) Object.ObjThis->AddRef();
		// does not addref the string or octet
	}

	void ReleaseObject()
	{
		iTJSDispatch2 * object = Object.Object;
		iTJSDispatch2 * objthis = Object.ObjThis;
		if(object) Object.Object = NULL, object->Release();
		if(objthis) Object.ObjThis = NULL, objthis->Release();
		// does not release the string nor octet
	}

	void AddRefContent()
	{
		if(vt==tvtObject)
		{
			if(Object.Object) Object.Object->AddRef();
			if(Object.ObjThis) Object.ObjThis->AddRef();
		}
		else
		{
			if(vt==tvtString) { if(String) String->AddRef(); }
			else if(vt==tvtOctet) { if(Octet) Octet->AddRef(); }
		}
	}

	void ReleaseContent()
	{
		if(vt==tvtObject)
		{
			ReleaseObject();
		}
		else
		{
			if(vt==tvtString) { if(String) String->Release(); }
			else if(vt==tvtOctet) { if(Octet) Octet->Release(); }
		}
	}

public:

	void ChangeClosureObjThis(iTJSDispatch2 *objthis)
	{
		if(objthis) objthis->AddRef();
		if(vt!=tvtObject) TJSThrowVariantConvertError(*this, tvtObject);
		if(Object.ObjThis)
		{
			iTJSDispatch2 * objthis = Object.ObjThis;
			Object.ObjThis = NULL;
			objthis->Release();
		}
		Object.ObjThis = objthis;
	}

	//---- constructor ------------------------------------------------------
public:

	tTJSVariant()
	{
		vt=tvtVoid;
	}

	tTJSVariant(const tTJSVariant &ref); // from tTJSVariant

	tTJSVariant(iTJSDispatch2 *ref) // from Object
	{
		if(ref) ref->AddRef();
		vt=tvtObject;
		Object.Object = ref;
		Object.ObjThis = NULL;
	}

	tTJSVariant(iTJSDispatch2 *obj, iTJSDispatch2 *objthis) // from closure
	{
		if(obj) obj->AddRef();
		if(objthis) objthis->AddRef();
		vt=tvtObject;
		Object.Object = obj;
		Object.ObjThis = objthis;
	}

	tTJSVariant(const tjs_char *ref) //  from String
	{
		vt=tvtString;
		if(ref)
		{
			String=TJSAllocVariantString(ref);
		}
		else
		{
			String=NULL;
		}
	}

	tTJSVariant(const tTJSString & ref) // from tTJSString
	{
		vt = tvtString;
		String = ref.AsVariantStringNoAddRef();
		if(String) String->AddRef();
	}

	tTJSVariant(const tjs_uint8 *ref, tjs_uint len) // from octet
	{
		vt=tvtOctet;
		if(ref)
		{
			Octet = TJSAllocVariantOctet(ref, len);
		}
		else
		{
			Octet = NULL;
		}
	}

	tTJSVariant(bool ref)
	{
		vt=tvtInteger;
		Integer=(tjs_int64)(tjs_int)ref;
	}

	tTJSVariant(tjs_int32 ref)
	{
		vt=tvtInteger;
		Integer=(tjs_int64)ref;
	}

	tTJSVariant(tjs_int64 ref)  // from Integer64
	{
		vt=tvtInteger;
		Integer=ref;
	}

	tTJSVariant(tjs_real ref)  // from double
	{
		vt=tvtReal;
		TJSSetFPUE();
		Real=ref;
	}

	tTJSVariant(const tjs_uint8 ** src); // from persistent storage

	//---- destructor -------------------------------------------------------

	~tTJSVariant();

	//---- type -------------------------------------------------------------

	tTJSVariantType Type() const { return vt; }

	//---- compare ----------------------------------------------------------

	bool NormalCompare(const tTJSVariant &val2) const;
	bool DiscernCompare(const tTJSVariant &val2) const;
	bool DiscernCompareStrictReal(const tTJSVariant &val2) const;
	bool GreaterThan(const tTJSVariant &val2) const;
	bool LittlerThan(const tTJSVariant &val2) const;

	bool IsInstanceOf(const tjs_char * classname) const;

	//---- clear ------------------------------------------------------------

	void Clear();

	//---- type conversion --------------------------------------------------

	iTJSDispatch2 *AsObject()
	{
		if(vt==tvtObject)
		{
			if(Object.Object) Object.Object->AddRef();
			return Object.Object;
		}

		TJSThrowVariantConvertError(*this, tvtObject);

		return NULL;
	}

	iTJSDispatch2 *AsObjectNoAddRef() const
	{
		if(vt==tvtObject)
			return Object.Object;
		TJSThrowVariantConvertError(*this, tvtObject);
		return NULL;
	}

	iTJSDispatch2 *AsObjectThis()
	{
		if(vt==tvtObject)
		{
			if(Object.ObjThis) Object.ObjThis->AddRef();
			return Object.ObjThis;
		}
		TJSThrowVariantConvertError(*this, tvtObject);
		return NULL;
	}

	iTJSDispatch2 *AsObjectThisNoAddRef() const
	{
		if(vt==tvtObject)
		{
			return Object.ObjThis;
		}
		TJSThrowVariantConvertError(*this, tvtObject);
		return NULL;
	}

	tTJSVariantClosure &AsObjectClosure()
	{
		if(vt==tvtObject)
		{
			AddRefObject();
			return *(tTJSVariantClosure*)&Object;
		}
		TJSThrowVariantConvertError(*this, tvtObject);
		return *(tTJSVariantClosure*)&TJSNullVariantClosure;
	}


	tTJSVariantClosure & AsObjectClosureNoAddRef() const
	{
		if(vt==tvtObject)
		{
			return *(tTJSVariantClosure*)&Object;
		}
		TJSThrowVariantConvertError(*this, tvtObject);
		return *(tTJSVariantClosure*)&TJSNullVariantClosure;
	}

	void ToObject()
	{
		switch(vt)
		{
		case tvtObject:  break;
		case tvtVoid:
		case tvtString:
		case tvtInteger:
		case tvtReal:
		case tvtOctet:    TJSThrowVariantConvertError(*this, tvtObject);
		}

	}

	operator iTJSDispatch2 *()
	{
		return AsObject();
	}

	tTJSVariantString *AsString() const
	{
		switch(vt)
		{
		case tvtVoid:    return NULL;
		case tvtObject:  return TJSObjectToString(*(tTJSVariantClosure*)&Object);
		case tvtString:  { if(String) { String->AddRef(); } return String; }
		case tvtInteger: return TJSIntegerToString(Integer);
		case tvtReal:    return TJSRealToString(Real);
		case tvtOctet:   TJSThrowVariantConvertError(*this, tvtString);
		}
		return NULL;
	}

	tTJSVariantString *AsStringNoAddRef() const
	{
		switch(vt)
		{
		case tvtVoid:    return NULL;
		case tvtString:  return String;
		case tvtObject:
		case tvtInteger:
		case tvtReal:
		case tvtOctet:   TJSThrowVariantConvertError(*this, tvtString);
		}
		return NULL;
	}

	void ToString();

	const tjs_char *GetString() const
	{
		// returns String
		if(vt!=tvtString) TJSThrowVariantConvertError(*this, tvtString);
		return *String;
	}

#ifdef TJS_SUPPORT_WX
	wxString AsWxString() const
	{
		if(vt!=tvtString) TJSThrowVariantConvertError(*this, tvtString);
	#ifdef TJS_WCHAR_T_SIZE_IS_16BIT
		wxMBConvUTF32 conv;
		return wxString(
			reinterpret_cast<const char *>(String->operator const tjs_char *()),
				*(wxMBConv *)&conv);
	#else
		return wxString(String->operator const tjs_char *());
	#endif
	}
#endif

	tjs_uint32 *GetHint()
	{
		// returns String Hint
		if(vt!=tvtString) TJSThrowVariantConvertError(*this, tvtString);
		if(!String) return NULL;
		return String->GetHint();
	}

	tTJSVariantOctet *AsOctet() const
	{
		switch(vt)
		{
		case tvtVoid:    return NULL;
		case tvtOctet:   { if(Octet) Octet->AddRef(); } return Octet;
		case tvtString:
		case tvtInteger:
		case tvtReal:
		case tvtObject:  TJSThrowVariantConvertError(*this, tvtOctet);
		}
		return NULL;
	}

	tTJSVariantOctet *AsOctetNoAddRef() const
	{
		switch(vt)
		{
		case tvtVoid:    return NULL;
		case tvtOctet:   return Octet;
		case tvtString:
		case tvtInteger:
		case tvtReal:
		case tvtObject:  TJSThrowVariantConvertError(*this, tvtOctet);
		}
		return NULL;
	}

	void ToOctet();

	tTVInteger AsInteger() const
	{
		switch(vt)
		{
		case tvtVoid:    return 0;
		case tvtObject:  TJSThrowVariantConvertError(*this, tvtInteger);
		case tvtString:  return String->ToInteger();
		case tvtInteger: return Integer;
		case tvtReal:    TJSSetFPUE(); return (tTVInteger)Real;
		case tvtOctet:   TJSThrowVariantConvertError(*this, tvtInteger);
		}
		return 0;
	}

	void AsNumber(tTJSVariant &targ) const
	{
		switch(vt)
		{
		case tvtVoid:    targ = (tTVInteger)0; return;
		case tvtObject:  TJSThrowVariantConvertError(*this, tvtInteger, tvtReal);
		case tvtString:  String->ToNumber(targ); return;
		case tvtInteger: targ = Integer; return;
		case tvtReal:    TJSSetFPUE(); targ = Real; return;
		case tvtOctet:   TJSThrowVariantConvertError(*this, tvtInteger, tvtReal);
		}
		return;
	}

	void ToInteger();

	operator tTVInteger() const
	{
		return AsInteger();
	}

	operator bool() const
	{
		switch(vt)
		{
		case tvtVoid:		return false;
		case tvtObject:		return (bool)Object.Object;
		case tvtString:		return (bool)AsInteger();
		case tvtOctet:		return (bool)Octet;
		case tvtInteger:	return (bool)Integer;
		case tvtReal:		TJSSetFPUE(); return (bool)Real;
		}
		return false;
	}

	operator tjs_int() const
	{
		return (tjs_int)AsInteger();
	}

	tTVReal AsReal() const
	{
		TJSSetFPUE();

		switch(vt)
		{
		case tvtVoid:    return 0;
		case tvtObject:  TJSThrowVariantConvertError(*this, tvtReal);
		case tvtString:  return String->ToReal();
		case tvtInteger: return (tTVReal)Integer;
		case tvtReal:    return Real;
		case tvtOctet:   TJSThrowVariantConvertError(*this, tvtReal);
		}
		return 0.0f;
	}

	void ToReal();

	operator tTVReal() const
	{
		return AsReal();
	}

	//---- substitution -----------------------------------------------------

	tTJSVariant & operator =(const tTJSVariant &ref)
	{
		// from tTJSVariant
		CopyRef(ref);
		return *this;
	}
	void CopyRef(const tTJSVariant & ref); // from reference to tTJSVariant
	tTJSVariant & operator = (iTJSDispatch2 *ref); // from Object
	tTJSVariant & SetObject(iTJSDispatch2 *ref) { return this->operator =(ref); }
	tTJSVariant & SetObject(iTJSDispatch2 *object, iTJSDispatch2 *objthis);
	tTJSVariant & operator = (tTJSVariantClosure ref); // from Object Closure
	tTJSVariant & operator = (tTJSVariantString *ref); // from tTJSVariantString
	tTJSVariant & operator = (tTJSVariantOctet *ref); // from tTJSVariantOctet
	tTJSVariant & operator = (const tTJSString & ref); // from tTJSString
	tTJSVariant & operator = (const tjs_char *ref); //  from String
	tTJSVariant & operator = (const tTVInteger ref); // from Integer64
	tTJSVariant & operator = (tjs_real ref); // from double

	//---- operators --------------------------------------------------------

	tTJSVariant operator || (const tTJSVariant & rhs)
	{
		return operator bool()||rhs.operator bool();
	}

	void logicalorequal(const tTJSVariant &rhs);

	tTJSVariant operator && (const tTJSVariant & rhs) const
	{
		return operator bool()&&rhs.operator bool();
	}

	void logicalandequal(const tTJSVariant &rhs);

	tTJSVariant operator | (const tTJSVariant & rhs) const
	{
		return (tTVInteger)(AsInteger()|rhs.AsInteger());
	}

	void operator |= (const tTJSVariant &rhs);

	tTJSVariant operator ^ (const tTJSVariant & rhs) const
	{
		return (tTVInteger)(AsInteger()^rhs.AsInteger());
	}

	void increment();

	void decrement();

	void operator ^= (const tTJSVariant &rhs);

	tTJSVariant operator &(const tTJSVariant & rhs) const
	{
		return (tTVInteger)(AsInteger()&rhs.AsInteger());
	}

	void operator &= (const tTJSVariant &rhs);

	tTJSVariant operator !=(const tTJSVariant & rhs) const
	{
		return !NormalCompare(rhs);
	}

	tTJSVariant operator == (const tTJSVariant & rhs) const
	{
		return NormalCompare(rhs);
	}

	tTJSVariant operator < (const tTJSVariant & rhs) const
	{
		return GreaterThan(rhs);
	}

	tTJSVariant operator > (const tTJSVariant & rhs) const
	{
		return LittlerThan(rhs);
	}

	tTJSVariant operator <= (const tTJSVariant & rhs) const
	{
		return !LittlerThan(rhs);
	}

	tTJSVariant operator >= (const tTJSVariant & rhs) const
	{
		return !GreaterThan(rhs);
	}

	tTJSVariant operator >> (const tTJSVariant & rhs) const
	{
		return (tTVInteger)(AsInteger()>>(tjs_int)rhs.AsInteger());
	}

	void operator >>= (const tTJSVariant &rhs);

	tTJSVariant rbitshift(tjs_int count) const
	{
		return (tTVInteger)((tjs_uint64)AsInteger()>> count);
	}

	void rbitshiftequal(const tTJSVariant &rhs);

	tTJSVariant operator <<(const tTJSVariant & rhs) const
	{
		return (tTVInteger)(AsInteger()<<(tjs_int)rhs.AsInteger());
	}

	void operator <<=(const tTJSVariant &rhs);

	tTJSVariant operator % (const tTJSVariant & rhs) const
	{
		tTVInteger r = rhs.AsInteger();
		if(r == 0) TJSThrowDivideByZero();
		return (tTVInteger)(AsInteger()%r);
	}

	void operator %=(const tTJSVariant &rhs);

	tTJSVariant operator / (const tTJSVariant & rhs) const
	{
		TJSSetFPUE();
		tTVReal r = rhs.AsReal();
		return (AsReal()/r);
	}

	void operator /= (const tTJSVariant &rhs);

	tTJSVariant idiv(const tTJSVariant & rhs) const
	{
		tTVInteger r = rhs.AsInteger();
		if(r == 0) TJSThrowDivideByZero();
		return (tTVInteger)(AsInteger() / r);
	}

	void idivequal(const tTJSVariant &rhs);

	tTJSVariant operator *(const tTJSVariant & rhs) const
	{
		tTJSVariant l(*this);
		l *= rhs;
		return l;
	}

private:
	void InternalMul(const tTJSVariant &rhs);
public:
	void operator *=(const tTJSVariant &rhs)
	{
		if(vt == tvtInteger && rhs.vt == tvtInteger)
		{
			Integer *= rhs.Integer;
			return;
		}
		InternalMul(rhs);
	}

	void logicalnot();

	tTJSVariant operator !() const
	{
		return (tTVInteger)!operator bool();
	}

	void bitnot();

	tTJSVariant operator ~ () const
	{
		return (tjs_int64)~AsInteger();
	}

	tTJSVariant operator - (const tTJSVariant & rhs) const
	{
		tTJSVariant l(*this);
		l -= rhs;
		return l;
	}

	void tonumber();

	tTJSVariant operator +() const
	{
		if(vt==tvtInteger || vt==tvtReal) return *this;

		if(vt==tvtString)
		{
			tTJSVariant val;
			String->ToNumber(val);
			return val;
		}

		if(vt==tvtVoid) return (tTVInteger)0;

		TJSThrowVariantConvertError(*this, tvtInteger, tvtReal);
		return tTJSVariant();
	}

private:
	void InternalChangeSign();
public:
	void changesign()
	{
		if(vt==tvtInteger)
		{
			Integer = -Integer;
			return;
		}
		InternalChangeSign();
	}

	tTJSVariant operator - () const
	{
		tTJSVariant v(*this);
		v.changesign();
		return v;
	}

private:
	void InternalSub(const tTJSVariant &rhs);
public:
	void operator -= (const tTJSVariant &rhs)
	{
		if(vt == tvtInteger && rhs.vt == tvtInteger)
		{
			Integer -= rhs.Integer;
			return;
		}
        InternalSub(rhs);
	}

	tTJSVariant operator +(const tTJSVariant & rhs) const
	{
		if(vt==tvtString || rhs.vt==tvtString)
		{
			// combines as string
			tTJSVariant val;
			val.vt = tvtString;
			tTJSVariantString *s1, *s2;
			s1 = AsString();
			s2 = rhs.AsString();
			val.String = TJSAllocVariantString(*s1, *s2);
			if(s1) s1->Release();
			if(s2) s2->Release();
			return val;
		}

		if(vt == rhs.vt)
		{
			if(vt==tvtOctet)
			{
				// combine as octet
				tTJSVariant val;
				val.vt = tvtOctet;
				val.Octet = TJSAllocVariantOctet(Octet, rhs.Octet);
				return val;
			}

			if(vt==tvtInteger)
			{
				return Integer+rhs.Integer;
			}
		}

		if(vt == tvtVoid)
		{
			if(rhs.vt == tvtInteger || rhs.vt == tvtReal)
				return rhs;
		}

		if(rhs.vt == tvtVoid)
		{
			if(vt == tvtInteger || vt == tvtReal) return *this;
		}


		TJSSetFPUE();
		return AsReal()+rhs.AsReal();
	}


	void operator +=(const tTJSVariant &rhs);

	tjs_int QueryPersistSize() const;
	void Persist(tjs_uint8 * dest);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

} // namespace TJS
#endif



