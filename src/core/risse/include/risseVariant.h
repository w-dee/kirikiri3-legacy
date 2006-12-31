//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief tRisseVariant (バリアント型) クラスの実装
//---------------------------------------------------------------------------
#ifndef risseVariantH
#define risseVariantH

#include <stdlib.h>
//#include <stdexcept>


#include "risseInterface.h"
#include "risseVariantString.h"
#include "risseString.h"

namespace Risse
{
void RisseThrowNullAccess();
void RisseThrowDivideByZero();
//---------------------------------------------------------------------------
class tRisseVariantString;
class tRisseString;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseVariantOctet
//---------------------------------------------------------------------------

#pragma pack(push, 4)
struct tRisseVariantOctet_S
{
	risse_uint Length;
	risse_int RefCount;
	risse_uint8 *Data;
};
#pragma pack(pop)

class tRisseVariantOctet : protected tRisseVariantOctet_S
{
public:
	tRisseVariantOctet(const risse_uint8 *data, risse_uint length);
	tRisseVariantOctet(const risse_uint8 *data1, risse_uint len1, const risse_uint8 *data2,
		risse_uint len2);
	tRisseVariantOctet(const tRisseVariantOctet *o1, const tRisseVariantOctet *o2);
	~tRisseVariantOctet();

	void AddRef()
	{
		RefCount ++;
	}

	void Release();

	risse_uint GetLength() const
	{
		return Length;
	}

	const risse_uint8 *GetData() const
	{
		return Data;
	}

	risse_int QueryPersistSize() { return sizeof(risse_uint) + Length; }
	void Persist(risse_uint8 * dest)
	{
		*(risse_uint*)dest = Length;
		if(Data) Risse_octetcpy(dest + sizeof(risse_uint), Data, Length);
	}
};
//---------------------------------------------------------------------------
tRisseVariantOctet *RisseAllocVariantOctet(const risse_uint8 *data, risse_uint length);
tRisseVariantOctet *RisseAllocVariantOctet(const risse_uint8 *data1, risse_uint len1,
	const risse_uint8 *data2, risse_uint len2);
tRisseVariantOctet *RisseAllocVariantOctet(const tRisseVariantOctet *o1, const
	tRisseVariantOctet *o2);
tRisseVariantOctet *RisseAllocVariantOctet(const risse_uint8 **src);
void RisseDeallocVariantOctet(tRisseVariantOctet *o);
tRisseVariantString *RisseOctetToListString(const tRisseVariantOctet *oct);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseVariant_S
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma option push -b
#endif
enum tRisseVariantType
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
class iRisseDispatch2;
struct tRisseVariantClosure_S
{
	iRisseDispatch2 *Object;
	iRisseDispatch2 *ObjThis;
};
class tRisseVariantClosure;

class tRisseVariantString;
class tRisseVariantOctet;
struct tRisseVariant_S
{
	//---- data members -----------------------------------------------------

	#define tRisseVariant_BITCOPY(a,b) \
	{\
		*(tRisseVariant_S*)&(a) = *(tRisseVariant_S*)&(b); \
	}

	union
	{
		tRisseVariantClosure_S Object;
		tTVInteger Integer;
		tTVReal Real;
		tRisseVariantString *String;
		tRisseVariantOctet *Octet;
	};
	tRisseVariantType vt;
};
#pragma pack(pop)

#ifdef __BORLANDC__
#pragma option pop
#endif
//---------------------------------------------------------------------------
// tRisseVariantClosure
//---------------------------------------------------------------------------
extern tRisseVariantClosure_S RisseNullVariantClosure;

//---------------------------------------------------------------------------
// tRisseVariantClosure
//---------------------------------------------------------------------------
class tRisseVariantClosure : public tRisseVariantClosure_S
{
	// tRisseVariantClosure does not provide any function of object lifetime
	// namagement. ( AddRef and Release are provided but tRisseVariantClosure
	// has no responsibility for them )

public:
	tRisseVariantClosure() {;} // note that default constructor does nothing 

	tRisseVariantClosure(iRisseDispatch2 *obj, iRisseDispatch2 *objthis = NULL)
	{ Object = obj, ObjThis = objthis; }

	iRisseDispatch2 * SelectObjectNoAddRef()
		{ return ObjThis ? ObjThis : Object; }

public:

	bool operator == (const tRisseVariantClosure &rhs)
	{
		return Object == rhs.Object && ObjThis == rhs.ObjThis;
	}

	bool operator != (const tRisseVariantClosure &rhs)
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


	risse_error
	FuncCall(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->FuncCall(flag, membername, hint, result, numparams, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	FuncCallByNum(risse_uint32 flag, risse_int num, tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->FuncCallByNum(flag, num, result, numparams, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	PropGet(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
		iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->PropGet(flag, membername, hint, result,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	PropGetByNum(risse_uint32 flag, risse_int num, tRisseVariant *result,
		iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->PropGetByNum(flag, num, result,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	PropSet(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->PropSet(flag, membername, hint, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	PropSetByNum(risse_uint32 flag, risse_int num, const tRisseVariant *param,
		iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->PropSetByNum(flag, num, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	GetCount(risse_int *result, const risse_char *membername, risse_uint32 *hint,
		iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->GetCount(result, membername, hint,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	GetCountByNum(risse_int *result, risse_int num, iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->GetCountByNum(result, num,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	PropSetByVS(risse_uint32 flag, tRisseVariantString *membername,
		const tRisseVariant *param, iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->PropSetByVS(flag, membername, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	EnumMembers(risse_uint32 flag, tRisseVariantClosure *callback,
		iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->EnumMembers(flag, callback,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	DeleteMember(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->DeleteMember(flag, membername, hint,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	DeleteMemberByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->DeleteMemberByNum(flag, num,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	Invalidate(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->Invalidate(flag, membername, hint,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	InvalidateByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->InvalidateByNum(flag, num,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	IsValid(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->IsValid(flag, membername, hint,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	IsValidByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->IsValidByNum(flag, num,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	CreateNew(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		iRisseDispatch2 **result,
		risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->CreateNew(flag, membername, hint, result, numparams,
			param, ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	CreateNewByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 **result,
		risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->CreateNewByNum(flag, num, result, numparams, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

/*
	risse_error
	Reserved1() { }
*/

	risse_error
	IsInstanceOf(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		const risse_char *classname, iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->IsInstanceOf(flag, membername, hint, classname,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	IsInstanceOf(risse_uint32 flag, risse_int num, risse_char *classname,
		iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->IsInstanceOfByNum(flag, num, classname,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	Operation(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		tRisseVariant *result, const tRisseVariant *param,	iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->Operation(flag, membername, hint, result, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

	risse_error
	OperationByNum(risse_uint32 flag, risse_int num, tRisseVariant *result,
		const tRisseVariant *param,	iRisseDispatch2 *objthis) const
	{
		if(!Object) RisseThrowNullAccess();
		return Object->OperationByNum(flag, num, result, param,
			ObjThis?ObjThis:(objthis?objthis:Object));
	}

/*
	risse_error
	Reserved2() { }
*/

/*
	risse_error
	Reserved3() { }
*/

};




//---------------------------------------------------------------------------
tRisseVariantString *RisseObjectToString(const tRisseVariantClosure &dsp);
tRisseVariantString *RisseIntegerToString(risse_int64 i);
tRisseVariantString *RisseRealToString(risse_real r);
tRisseVariantString *RisseRealToHexString(risse_real r);
tTVInteger RisseStringToInteger(const risse_char *str);
tTVReal RisseStringToReal(const risse_char *str);
//---------------------------------------------------------------------------
extern void RisseThrowVariantConvertError(const tRisseVariant & from, tRisseVariantType to);
extern void RisseThrowVariantConvertError(const tRisseVariant & from, tRisseVariantType to1,
	tRisseVariantType to2);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseVariant
//---------------------------------------------------------------------------
class tRisseVariant : protected tRisseVariant_S
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
		iRisseDispatch2 * object = Object.Object;
		iRisseDispatch2 * objthis = Object.ObjThis;
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

	void ChangeClosureObjThis(iRisseDispatch2 *objthis)
	{
		if(objthis) objthis->AddRef();
		if(vt!=tvtObject) RisseThrowVariantConvertError(*this, tvtObject);
		if(Object.ObjThis)
		{
			iRisseDispatch2 * objthis = Object.ObjThis;
			Object.ObjThis = NULL;
			objthis->Release();
		}
		Object.ObjThis = objthis;
	}

	//---- constructor ------------------------------------------------------
public:

	tRisseVariant()
	{
		vt=tvtVoid;
	}

	tRisseVariant(const tRisseVariant &ref); // from tRisseVariant

	tRisseVariant(iRisseDispatch2 *ref) // from Object
	{
		if(ref) ref->AddRef();
		vt=tvtObject;
		Object.Object = ref;
		Object.ObjThis = NULL;
	}

	tRisseVariant(iRisseDispatch2 *obj, iRisseDispatch2 *objthis) // from closure
	{
		if(obj) obj->AddRef();
		if(objthis) objthis->AddRef();
		vt=tvtObject;
		Object.Object = obj;
		Object.ObjThis = objthis;
	}

	tRisseVariant(const risse_char *ref) //  from String
	{
		vt=tvtString;
		if(ref)
		{
			String=RisseAllocVariantString(ref);
		}
		else
		{
			String=NULL;
		}
	}

	tRisseVariant(const tRisseString & ref) // from tRisseString
	{
		vt = tvtString;
		String = ref.AsVariantStringNoAddRef();
		if(String) String->AddRef();
	}

	tRisseVariant(const risse_uint8 *ref, risse_uint len) // from octet
	{
		vt=tvtOctet;
		if(ref)
		{
			Octet = RisseAllocVariantOctet(ref, len);
		}
		else
		{
			Octet = NULL;
		}
	}

	tRisseVariant(bool ref)
	{
		vt=tvtInteger;
		Integer=(risse_int64)(risse_int)ref;
	}

	tRisseVariant(risse_int32 ref)
	{
		vt=tvtInteger;
		Integer=(risse_int64)ref;
	}

	tRisseVariant(risse_int64 ref)  // from Integer64
	{
		vt=tvtInteger;
		Integer=ref;
	}

	tRisseVariant(risse_real ref)  // from double
	{
		vt=tvtReal;
		RisseSetFPUE();
		Real=ref;
	}

	tRisseVariant(const risse_uint8 ** src); // from persistent storage

	//---- destructor -------------------------------------------------------

	~tRisseVariant();

	//---- type -------------------------------------------------------------

	tRisseVariantType Type() const { return vt; }

	//---- compare ----------------------------------------------------------

	bool NormalCompare(const tRisseVariant &val2) const;
	bool DiscernCompare(const tRisseVariant &val2) const;
	bool DiscernCompareStrictReal(const tRisseVariant &val2) const;
	bool GreaterThan(const tRisseVariant &val2) const;
	bool LittlerThan(const tRisseVariant &val2) const;

	bool IsInstanceOf(const risse_char * classname) const;

	//---- clear ------------------------------------------------------------

	void Clear();

	//---- type conversion --------------------------------------------------

	iRisseDispatch2 *AsObject()
	{
		if(vt==tvtObject)
		{
			if(Object.Object) Object.Object->AddRef();
			return Object.Object;
		}

		RisseThrowVariantConvertError(*this, tvtObject);

		return NULL;
	}

	iRisseDispatch2 *AsObjectNoAddRef() const
	{
		if(vt==tvtObject)
			return Object.Object;
		RisseThrowVariantConvertError(*this, tvtObject);
		return NULL;
	}

	iRisseDispatch2 *AsObjectThis()
	{
		if(vt==tvtObject)
		{
			if(Object.ObjThis) Object.ObjThis->AddRef();
			return Object.ObjThis;
		}
		RisseThrowVariantConvertError(*this, tvtObject);
		return NULL;
	}

	iRisseDispatch2 *AsObjectThisNoAddRef() const
	{
		if(vt==tvtObject)
		{
			return Object.ObjThis;
		}
		RisseThrowVariantConvertError(*this, tvtObject);
		return NULL;
	}

	tRisseVariantClosure &AsObjectClosure()
	{
		if(vt==tvtObject)
		{
			AddRefObject();
			return *(tRisseVariantClosure*)&Object;
		}
		RisseThrowVariantConvertError(*this, tvtObject);
		return *(tRisseVariantClosure*)&RisseNullVariantClosure;
	}


	tRisseVariantClosure & AsObjectClosureNoAddRef() const
	{
		if(vt==tvtObject)
		{
			return *(tRisseVariantClosure*)&Object;
		}
		RisseThrowVariantConvertError(*this, tvtObject);
		return *(tRisseVariantClosure*)&RisseNullVariantClosure;
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
		case tvtOctet:    RisseThrowVariantConvertError(*this, tvtObject);
		}

	}

	operator iRisseDispatch2 *()
	{
		return AsObject();
	}

	tRisseVariantString *AsString() const
	{
		switch(vt)
		{
		case tvtVoid:    return NULL;
		case tvtObject:  return RisseObjectToString(*(tRisseVariantClosure*)&Object);
		case tvtString:  { if(String) { String->AddRef(); } return String; }
		case tvtInteger: return RisseIntegerToString(Integer);
		case tvtReal:    return RisseRealToString(Real);
		case tvtOctet:   RisseThrowVariantConvertError(*this, tvtString);
		}
		return NULL;
	}

	tRisseVariantString *AsStringNoAddRef() const
	{
		switch(vt)
		{
		case tvtVoid:    return NULL;
		case tvtString:  return String;
		case tvtObject:
		case tvtInteger:
		case tvtReal:
		case tvtOctet:   RisseThrowVariantConvertError(*this, tvtString);
		}
		return NULL;
	}

	void ToString();

	const risse_char *GetString() const
	{
		// returns String
		if(vt!=tvtString) RisseThrowVariantConvertError(*this, tvtString);
		return *String;
	}

#ifdef RISSE_SUPPORT_WX
	wxString AsWxString() const
	{
		if(vt!=tvtString) RisseThrowVariantConvertError(*this, tvtString);
		return RisseCharToWxString(String->operator const risse_char *());
	}
#endif

	risse_uint32 *GetHint()
	{
		// returns String Hint
		if(vt!=tvtString) RisseThrowVariantConvertError(*this, tvtString);
		if(!String) return NULL;
		return String->GetHint();
	}

	tRisseVariantOctet *AsOctet() const
	{
		switch(vt)
		{
		case tvtVoid:    return NULL;
		case tvtOctet:   { if(Octet) Octet->AddRef(); } return Octet;
		case tvtString:
		case tvtInteger:
		case tvtReal:
		case tvtObject:  RisseThrowVariantConvertError(*this, tvtOctet);
		}
		return NULL;
	}

	tRisseVariantOctet *AsOctetNoAddRef() const
	{
		switch(vt)
		{
		case tvtVoid:    return NULL;
		case tvtOctet:   return Octet;
		case tvtString:
		case tvtInteger:
		case tvtReal:
		case tvtObject:  RisseThrowVariantConvertError(*this, tvtOctet);
		}
		return NULL;
	}

	void ToOctet();

	tTVInteger AsInteger() const
	{
		switch(vt)
		{
		case tvtVoid:    return 0;
		case tvtObject:  RisseThrowVariantConvertError(*this, tvtInteger);
		case tvtString:  return String->ToInteger();
		case tvtInteger: return Integer;
		case tvtReal:    RisseSetFPUE(); return (tTVInteger)Real;
		case tvtOctet:   RisseThrowVariantConvertError(*this, tvtInteger);
		}
		return 0;
	}

	void AsNumber(tRisseVariant &targ) const
	{
		switch(vt)
		{
		case tvtVoid:    targ = (tTVInteger)0; return;
		case tvtObject:  RisseThrowVariantConvertError(*this, tvtInteger, tvtReal);
		case tvtString:  String->ToNumber(targ); return;
		case tvtInteger: targ = Integer; return;
		case tvtReal:    RisseSetFPUE(); targ = Real; return;
		case tvtOctet:   RisseThrowVariantConvertError(*this, tvtInteger, tvtReal);
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
		case tvtReal:		RisseSetFPUE(); return (bool)Real;
		}
		return false;
	}

	operator risse_int() const
	{
		return (risse_int)AsInteger();
	}

	tTVReal AsReal() const
	{
		RisseSetFPUE();

		switch(vt)
		{
		case tvtVoid:    return 0;
		case tvtObject:  RisseThrowVariantConvertError(*this, tvtReal);
		case tvtString:  return String->ToReal();
		case tvtInteger: return (tTVReal)Integer;
		case tvtReal:    return Real;
		case tvtOctet:   RisseThrowVariantConvertError(*this, tvtReal);
		}
		return 0.0f;
	}

	void ToReal();

	operator tTVReal() const
	{
		return AsReal();
	}

	//---- substitution -----------------------------------------------------

	tRisseVariant & operator =(const tRisseVariant &ref)
	{
		// from tRisseVariant
		CopyRef(ref);
		return *this;
	}
	void CopyRef(const tRisseVariant & ref); // from reference to tRisseVariant
	tRisseVariant & operator = (iRisseDispatch2 *ref); // from Object
	tRisseVariant & SetObject(iRisseDispatch2 *ref) { return this->operator =(ref); }
	tRisseVariant & SetObject(iRisseDispatch2 *object, iRisseDispatch2 *objthis);
	tRisseVariant & operator = (tRisseVariantClosure ref); // from Object Closure
	tRisseVariant & operator = (tRisseVariantString *ref); // from tRisseVariantString
	tRisseVariant & operator = (tRisseVariantOctet *ref); // from tRisseVariantOctet
	tRisseVariant & operator = (const tRisseString & ref); // from tRisseString
	tRisseVariant & operator = (const risse_char *ref); //  from String
	tRisseVariant & operator = (const tTVInteger ref); // from Integer64
	tRisseVariant & operator = (risse_real ref); // from double

	//---- operators --------------------------------------------------------

	tRisseVariant operator || (const tRisseVariant & rhs)
	{
		return operator bool()||rhs.operator bool();
	}

	void logicalorequal(const tRisseVariant &rhs);

	tRisseVariant operator && (const tRisseVariant & rhs) const
	{
		return operator bool()&&rhs.operator bool();
	}

	void logicalandequal(const tRisseVariant &rhs);

	tRisseVariant operator | (const tRisseVariant & rhs) const
	{
		return (tTVInteger)(AsInteger()|rhs.AsInteger());
	}

	void operator |= (const tRisseVariant &rhs);

	tRisseVariant operator ^ (const tRisseVariant & rhs) const
	{
		return (tTVInteger)(AsInteger()^rhs.AsInteger());
	}

	void increment();

	void decrement();

	void operator ^= (const tRisseVariant &rhs);

	tRisseVariant operator &(const tRisseVariant & rhs) const
	{
		return (tTVInteger)(AsInteger()&rhs.AsInteger());
	}

	void operator &= (const tRisseVariant &rhs);

	tRisseVariant operator !=(const tRisseVariant & rhs) const
	{
		return !NormalCompare(rhs);
	}

	tRisseVariant operator == (const tRisseVariant & rhs) const
	{
		return NormalCompare(rhs);
	}

	tRisseVariant operator < (const tRisseVariant & rhs) const
	{
		return GreaterThan(rhs);
	}

	tRisseVariant operator > (const tRisseVariant & rhs) const
	{
		return LittlerThan(rhs);
	}

	tRisseVariant operator <= (const tRisseVariant & rhs) const
	{
		return !LittlerThan(rhs);
	}

	tRisseVariant operator >= (const tRisseVariant & rhs) const
	{
		return !GreaterThan(rhs);
	}

	tRisseVariant operator >> (const tRisseVariant & rhs) const
	{
		return (tTVInteger)(AsInteger()>>(risse_int)rhs.AsInteger());
	}

	void operator >>= (const tRisseVariant &rhs);

	tRisseVariant rbitshift(risse_int count) const
	{
		return (tTVInteger)((risse_uint64)AsInteger()>> count);
	}

	void rbitshiftequal(const tRisseVariant &rhs);

	tRisseVariant operator <<(const tRisseVariant & rhs) const
	{
		return (tTVInteger)(AsInteger()<<(risse_int)rhs.AsInteger());
	}

	void operator <<=(const tRisseVariant &rhs);

	tRisseVariant operator % (const tRisseVariant & rhs) const
	{
		tTVInteger r = rhs.AsInteger();
		if(r == 0) RisseThrowDivideByZero();
		return (tTVInteger)(AsInteger()%r);
	}

	void operator %=(const tRisseVariant &rhs);

	tRisseVariant operator / (const tRisseVariant & rhs) const
	{
		RisseSetFPUE();
		tTVReal r = rhs.AsReal();
		return (AsReal()/r);
	}

	void operator /= (const tRisseVariant &rhs);

	tRisseVariant idiv(const tRisseVariant & rhs) const
	{
		tTVInteger r = rhs.AsInteger();
		if(r == 0) RisseThrowDivideByZero();
		return (tTVInteger)(AsInteger() / r);
	}

	void idivequal(const tRisseVariant &rhs);

	tRisseVariant operator *(const tRisseVariant & rhs) const
	{
		tRisseVariant l(*this);
		l *= rhs;
		return l;
	}

private:
	void InternalMul(const tRisseVariant &rhs);
public:
	void operator *=(const tRisseVariant &rhs)
	{
		if(vt == tvtInteger && rhs.vt == tvtInteger)
		{
			Integer *= rhs.Integer;
			return;
		}
		InternalMul(rhs);
	}

	void logicalnot();

	tRisseVariant operator !() const
	{
		return (tTVInteger)!operator bool();
	}

	void bitnot();

	tRisseVariant operator ~ () const
	{
		return (risse_int64)~AsInteger();
	}

	tRisseVariant operator - (const tRisseVariant & rhs) const
	{
		tRisseVariant l(*this);
		l -= rhs;
		return l;
	}

	void tonumber();

	tRisseVariant operator +() const
	{
		if(vt==tvtInteger || vt==tvtReal) return *this;

		if(vt==tvtString)
		{
			tRisseVariant val;
			String->ToNumber(val);
			return val;
		}

		if(vt==tvtVoid) return (tTVInteger)0;

		RisseThrowVariantConvertError(*this, tvtInteger, tvtReal);
		return tRisseVariant();
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

	tRisseVariant operator - () const
	{
		tRisseVariant v(*this);
		v.changesign();
		return v;
	}

private:
	void InternalSub(const tRisseVariant &rhs);
public:
	void operator -= (const tRisseVariant &rhs)
	{
		if(vt == tvtInteger && rhs.vt == tvtInteger)
		{
			Integer -= rhs.Integer;
			return;
		}
        InternalSub(rhs);
	}

	tRisseVariant operator +(const tRisseVariant & rhs) const
	{
		if(vt==tvtString || rhs.vt==tvtString)
		{
			// combines as string
			tRisseVariant val;
			val.vt = tvtString;
			tRisseVariantString *s1, *s2;
			s1 = AsString();
			s2 = rhs.AsString();
			val.String = RisseAllocVariantString(*s1, *s2);
			if(s1) s1->Release();
			if(s2) s2->Release();
			return val;
		}

		if(vt == rhs.vt)
		{
			if(vt==tvtOctet)
			{
				// combine as octet
				tRisseVariant val;
				val.vt = tvtOctet;
				val.Octet = RisseAllocVariantOctet(Octet, rhs.Octet);
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


		RisseSetFPUE();
		return AsReal()+rhs.AsReal();
	}


	void operator +=(const tRisseVariant &rhs);

	risse_int QueryPersistSize() const;
	void Persist(risse_uint8 * dest);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

} // namespace Risse
#endif



