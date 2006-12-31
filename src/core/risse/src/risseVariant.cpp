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
#include "risseCommHead.h"

#include "risseVariant.h"
#include "risseError.h"
#include "risseLex.h"
#include "risseUtils.h"
#include "risseDebug.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(48473,29200,16591,20340,48528,338,1161,40290);

//---------------------------------------------------------------------------
// tRisseVariantOctet related
//---------------------------------------------------------------------------
tRisseVariantOctet::tRisseVariantOctet(const risse_uint8 *data, risse_uint length)
{
	// TODO : tRisseVariantOctet check
	RefCount = 1;
	Length = length;
	Data = new risse_uint8[length];
	Risse_octetcpy(Data, data, length);
}
//---------------------------------------------------------------------------
tRisseVariantOctet::tRisseVariantOctet(const risse_uint8 *data1, risse_uint len1,
	const risse_uint8 *data2, risse_uint len2)
{
	RefCount = 1;
	Length = len1 + len2;
	Data = new risse_uint8[Length];
	if(len1) Risse_octetcpy(Data, data1, len1);
	if(len2) Risse_octetcpy(Data + len1, data2, len2);
}
//---------------------------------------------------------------------------
tRisseVariantOctet::tRisseVariantOctet(const tRisseVariantOctet *o1,
	const tRisseVariantOctet *o2)
{
	RefCount = 1;
	Length = (o1?o1->Length:0) + (o2?o2->Length:0);
	Data = new risse_uint8[Length];
	if(o1 && o1->Length) Risse_octetcpy(Data, o1->Data, o1->Length);
	if(o2 && o2->Length) Risse_octetcpy(Data + (o1?o1->Length:0), o2->Data,
							o2->Length);
}
//---------------------------------------------------------------------------
tRisseVariantOctet::~tRisseVariantOctet()
{
	delete [] Data;
}
//---------------------------------------------------------------------------
void tRisseVariantOctet::Release()
{
	if(RefCount == 1)
		delete this;
	else
		RefCount--;
}
//---------------------------------------------------------------------------
tRisseVariantOctet * RisseAllocVariantOctet(const risse_uint8 *data, risse_uint length)
{
	if(!data) return NULL;
	if(length == 0) return NULL;
	return new tRisseVariantOctet(data, length);
}
//---------------------------------------------------------------------------
tRisseVariantOctet * RisseAllocVariantOctet(const risse_uint8 *data1, risse_uint len1,
	const risse_uint8 *data2, risse_uint len2)
{
	if(!data1) len1 = 0;
	if(!data2) len2 = 0;
	if(len1 + len2 == 0) return NULL;

	return new tRisseVariantOctet(data1, len1, data2, len2);
}
//---------------------------------------------------------------------------
tRisseVariantOctet * RisseAllocVariantOctet(const tRisseVariantOctet *o1, const
	tRisseVariantOctet *o2)
{
	if(!o1 && !o2) return NULL;
	return new tRisseVariantOctet(o1, o2);
}
//---------------------------------------------------------------------------
tRisseVariantOctet * RisseAllocVariantOctet(const risse_uint8 **src)
{
	risse_uint size = *(const risse_uint*)(*src);
	*src += sizeof(risse_uint);
	if(!size) return NULL;
	tRisseVariantOctet *octet =  new tRisseVariantOctet(*src, size);
	*src += size;
	return octet;
}
//---------------------------------------------------------------------------
void RisseDeallocVariantOctet(tRisseVariantOctet *o)
{
	delete o;
}
//---------------------------------------------------------------------------
tRisseVariantString * RisseOctetToListString(const tRisseVariantOctet *oct)
{
	if(!oct) return NULL;
	if(oct->GetLength() == 0) return NULL;
	risse_int stringlen = oct->GetLength() * 3 -1;
	tRisseVariantString * str = RisseAllocVariantStringBuffer(stringlen);

	risse_char *buf = const_cast<risse_char*>(str->operator const risse_char*());
	static const risse_char hex[] = { 
		L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A',
		L'B', L'C', L'D', L'E', L'F' };
	const risse_uint8 *data = oct->GetData();
	risse_uint n = oct->GetLength();
	while(n--)
	{
		buf[0] = hex[*data >> 4];
		buf[1] = hex[*data & 0x0f];
		if(n != 0)
			buf[2] = RISSE_WC(' ');
		buf+=3;
		data++;
	}

	return str;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// Utility Functions
//---------------------------------------------------------------------------
tRisseVariantClosure_S RisseNullVariantClosure={NULL,NULL};
//---------------------------------------------------------------------------
void RisseThrowVariantConvertError(const tRisseVariant & from, tRisseVariantType to)
{
	if(to == tvtObject)
	{
		ttstr msg(RISSE_WS_TR("can not convert %1 to object"),
			RisseVariantToReadableString(from));
		Risse_eRisseVariantError(msg);
	}
	else
	{
		ttstr msg(RISSE_WS_TR("can not convert %1 to type %2"),
			RisseVariantToReadableString(from),
			RisseVariantTypeToTypeString(to));
		Risse_eRisseVariantError(msg);
	}
}
//---------------------------------------------------------------------------
void RisseThrowVariantConvertError(const tRisseVariant & from, tRisseVariantType to1,
	tRisseVariantType to2)
{
	ttstr msg(RISSE_WS_TR("can not convert %1 to type %2 or %3"),
			RisseVariantToReadableString(from),
			RisseVariantTypeToTypeString(to1),
			RisseVariantTypeToTypeString(to2));
	Risse_eRisseVariantError(msg);
}
//---------------------------------------------------------------------------
void RisseThrowNullAccess()
{
	Risse_eRisseError(RisseNullAccess);
}
//---------------------------------------------------------------------------
void RisseThrowDivideByZero()
{
	Risse_eRisseVariantError(RisseDivideByZero);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantString * RisseObjectToString(const tRisseVariantClosure &dsp)
{
	if(RisseObjectTypeInfoEnabled())
	{
		// retrieve object type information from debugging facility
		risse_char tmp1[30];
		risse_char tmp2[30];
		Risse_pointer_to_str(dsp.Object,  tmp1);
		Risse_pointer_to_str(dsp.ObjThis, tmp2);
		ttstr type1 = RisseGetObjectTypeInfo(dsp.Object);
		ttstr type2 = RisseGetObjectTypeInfo(dsp.ObjThis);
		if(!type1.IsEmpty()) type1 = RISSE_WS1("[") + type1 + RISSE_WS2("]");
		if(!type2.IsEmpty()) type2 = RISSE_WS1("[") + type2 + RISSE_WS2("]");

		ttstr ret(ttstr(RISSE_WS("(Object 0x%1%2:0x%3%4)")),
			ttstr(tmp1), type1, ttstr(tmp2), type2);

		tRisseVariantString * str = ret.AsVariantStringNoAddRef();
		str->AddRef();
		return str;
	}
	else
	{
		risse_char tmp1[30];
		risse_char tmp2[30];
		Risse_pointer_to_str(dsp.Object,  tmp1);
		Risse_pointer_to_str(dsp.ObjThis, tmp2);

		ttstr ret(ttstr(RISSE_WS("(Object 0x%1:0x%2)")),
			ttstr(tmp1), ttstr(tmp2));

		tRisseVariantString * str = ret.AsVariantStringNoAddRef();
		str->AddRef();
		return str;
	}
}
//---------------------------------------------------------------------------
tRisseVariantString * RisseIntegerToString(risse_int64 i)
{
	risse_char tmp[34];
	return RisseAllocVariantString( Risse_tTVInt_to_str( i , tmp));
}
//---------------------------------------------------------------------------
static  tRisseVariantString * RisseSpecialRealToString(risse_real r)
{
	risse_int32 cls = RisseGetFPClass(r);

	if(RISSE_FC_IS_NAN(cls))
	{
		return RisseAllocVariantString(RISSE_WS("NaN"));
	}
	if(RISSE_FC_IS_INF(cls))
	{
		if(RISSE_FC_IS_NEGATIVE(cls))
			return RisseAllocVariantString(RISSE_WS("-Infinity"));
		else
			return RisseAllocVariantString(RISSE_WS("+Infinity"));
	}
	if(r == 0.0)
	{
		if(RISSE_FC_IS_NEGATIVE(cls))
			return RisseAllocVariantString(RISSE_WS("-0.0"));
		else
			return RisseAllocVariantString(RISSE_WS("+0.0"));
	}
	return NULL;
}
//---------------------------------------------------------------------------
tRisseVariantString * RisseRealToString(risse_real r)
{
	tRisseVariantString *v = RisseSpecialRealToString(r);
	if(v) return v;

	RisseSetFPUE();

	// ここでは一度 ナロー文字列として数値を書き出してから
	// risse_char 文字列に変換する方法を採る
	risse_char tmp[25];
	Risse_tTVReal_to_str(r, tmp);

	return RisseAllocVariantString(tmp);
}
//---------------------------------------------------------------------------
tRisseVariantString * RisseRealToHexString(risse_real r)
{
	tRisseVariantString *v = RisseSpecialRealToString(r);
	if(v) return v;

	risse_uint64 *ui64 = (risse_uint64*)&r;

	risse_char tmp[128];

	risse_char *p;

	if(RISSE_IEEE_D_GET_SIGN(*ui64))
	{
		Risse_strcpy(tmp, RISSE_WS("-0x1."));
		p = tmp + 5;
	}
	else
	{
		Risse_strcpy(tmp, RISSE_WS("0x1."));
		p = tmp + 4;
	}

	const risse_char * hexdigits = RISSE_WS("0123456789ABCDEF");

	risse_int exp = RISSE_IEEE_D_GET_EXP(*ui64);

	risse_int bits = RISSE_IEEE_D_SIGNIFICAND_BITS;

	while(true)
	{
		bits -= 4;
		if(bits < 0) break;
		*(p++) = hexdigits[(risse_int)(*ui64 >> bits) & 0x0f];
	}

	*(p++) = RISSE_WC('p');
	Risse_int_to_str(exp, p);

	return RisseAllocVariantString(tmp);
}
//---------------------------------------------------------------------------
tTVInteger RisseStringToInteger(const risse_char *str)
{
	tRisseVariant val;
	if(RisseParseNumber(val, &str)) 	return val.AsInteger();
	return 0;
}
//---------------------------------------------------------------------------
tTVReal RisseStringToReal(const risse_char *str)
{
	tRisseVariant val;
	if(RisseParseNumber(val, &str)) 	return val.AsReal();
	return 0;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tRisseVariant member
//---------------------------------------------------------------------------
tRisseVariant::tRisseVariant(const tRisseVariant &ref) // from tRisseVariant
{

	tRisseVariant_BITCOPY(*this, ref);
	if(vt==tvtObject)
	{
		if(Object.Object) Object.Object->AddRef();
		if(Object.ObjThis) Object.ObjThis->AddRef();
	}
	else
	{
		if(vt==tvtString) {	if(String) String->AddRef(); }
		else if(vt==tvtOctet) { if(Octet) Octet->AddRef(); }
	}
}
//---------------------------------------------------------------------------
tRisseVariant::tRisseVariant(const risse_uint8 ** src)
{
	// from persistent storage
	vt = (tRisseVariantType) **src;
	*src++;

	switch(vt)
	{
	case tvtVoid:
		break;

	case tvtObject:
		Object = *(tRisseVariantClosure_S*)(*src);
		// no addref
		break;

	case tvtString:
		String = RisseAllocVariantString(src);
		break;

	case tvtInteger:
		Integer = *(const tTVInteger *)(*src);
		break;

	case tvtReal:
		RisseSetFPUE();
		Real = *(const tTVReal *)(*src);
		break;

	case tvtOctet:
		Octet = RisseAllocVariantOctet(src);
		break;

	default:
		break; // unknown type
	}
}
//---------------------------------------------------------------------------
tRisseVariant::~tRisseVariant()
{
	Clear();
}
//---------------------------------------------------------------------------
void tRisseVariant::Clear()
{
	tRisseVariantType o_vt = vt;
	vt = tvtVoid;
	switch(o_vt)
	{
	case tvtObject:
		if(Object.Object) Object.Object->Release();
		if(Object.ObjThis) Object.ObjThis->Release();
		break;
	case tvtString:
		if(String) String->Release();
		break;
	case tvtOctet:
		if(Octet) Octet->Release();
		break;
	default:
		break;
	}
}
//---------------------------------------------------------------------------
void tRisseVariant::ToString()
{
	switch(vt)
	{
	case tvtVoid:
		String=NULL;
		vt=tvtString;
		break;

	case tvtObject:
	  {
		tRisseVariantString * string = RisseObjectToString(*(tRisseVariantClosure*)&Object);
		ReleaseObject();
		String = string;
		vt=tvtString;
		break;
	  }

	case tvtString:
		break;

	case tvtInteger:
		String=RisseIntegerToString(Integer);
		vt=tvtString;
		break;

	case tvtReal:
		String=RisseRealToString(Real);
		vt=tvtString;
		break;

	case tvtOctet:
		RisseThrowVariantConvertError(*this, tvtString);
		break;
	}
}
//---------------------------------------------------------------------------
void tRisseVariant::ToOctet()
{
	switch(vt)
	{
	case tvtVoid:
		Octet = NULL;
		vt = tvtOctet;
		break;


	case tvtOctet:
		break;

	case tvtString:
	case tvtInteger:
	case tvtReal:
	case tvtObject:
		RisseThrowVariantConvertError(*this, tvtOctet);
	}
}
//---------------------------------------------------------------------------
void tRisseVariant::ToInteger()
{
	tTVInteger i = AsInteger();
	ReleaseContent();
	vt = tvtInteger;
	Integer = i;
}
//---------------------------------------------------------------------------
void tRisseVariant::ToReal()
{
	tTVReal r = AsReal();
	ReleaseContent();
	vt = tvtReal;
	Real = r;
}
//---------------------------------------------------------------------------
void tRisseVariant::CopyRef(const tRisseVariant & ref) // from reference to tRisseVariant
{
	switch(ref.vt)
	{
	case tvtVoid:
		ReleaseContent();
		vt = tvtVoid;
		return;
	case tvtObject:
		if(this != &ref)
		{
			/*
				note:
				ReleaseContent makes the object variables null during clear,
				thus makes the resulting object also null when the ref and this are
				exactly the same object.
				This does not affect string nor octet because the ReleaseContent
				does *not* make the pointer null during clear when the variant type
				is string or octet.
			*/
			((tRisseVariantClosure&)ref.Object).AddRef();
			ReleaseContent();
			Object = ref.Object;
			vt = tvtObject;
		}
		return;
	case tvtString:
		if(ref.String) ref.String->AddRef();
		ReleaseContent();
		String = ref.String;
		vt = tvtString;
		return;
	case tvtOctet:
		if(ref.Octet) ref.Octet->AddRef();
		ReleaseContent();
		Octet = ref.Octet;
		vt = tvtOctet;
		return;
	default:
		ReleaseContent();
		tRisseVariant_BITCOPY(*this,ref);
		return;
	}

}
//---------------------------------------------------------------------------

tRisseVariant & tRisseVariant::operator =(iRisseDispatch2 *ref) // from Object
{
	if(ref) ref->AddRef();
	ReleaseContent();
	vt = tvtObject;
	Object.Object = ref;
	Object.ObjThis = NULL;
	return *this;
}
//---------------------------------------------------------------------------
tRisseVariant & tRisseVariant::SetObject(iRisseDispatch2 *object, iRisseDispatch2 *objthis)
{
	if(object) object->AddRef();
	if(objthis) objthis->AddRef();
	ReleaseContent();
	vt = tvtObject;
	Object.Object = object;
	Object.ObjThis = objthis;
	return *this;
}
//---------------------------------------------------------------------------

tRisseVariant & tRisseVariant::operator =(tRisseVariantClosure ref) // from Object Closure
{
	ReleaseContent();
	vt=tvtObject;
	Object=ref;
	AddRefContent();
	return *this;
}
//---------------------------------------------------------------------------

#ifdef RISSE_SUPPORT_VCL
tRisseVariant & tRisseVariant::operator =(WideString s) // from WideString
{
	ReleaseContent();
	vt=tvtString;
	if(s.IsEmpty())
		String=NULL;
	else
		String=RisseAllocVariantString(s.c_bstr());
	return *this;
}
#endif

//---------------------------------------------------------------------------
tRisseVariant & tRisseVariant::operator =(tRisseVariantString *ref) // from tRisseVariantString
{
	if(ref) ref->AddRef();
	ReleaseContent();
	vt = tvtString;
	String = ref;
	return *this;
}

//---------------------------------------------------------------------------
tRisseVariant & tRisseVariant::operator =(tRisseVariantOctet *ref) // from tRisseVariantOctet
{
	if(ref) ref->AddRef();
	ReleaseContent();
	vt = tvtOctet;
	Octet = ref;
	return *this;
}

//---------------------------------------------------------------------------
tRisseVariant & tRisseVariant::operator =(const tRisseString & ref) // from tRisseString
{
	ReleaseContent();
	vt = tvtString;
	String = ref.AsVariantStringNoAddRef();
	if(String) String->AddRef();
	return *this;
}

//---------------------------------------------------------------------------
tRisseVariant & tRisseVariant::operator =(const risse_char *ref) //  from String
{
	ReleaseContent();
	vt=tvtString;
	String=RisseAllocVariantString(ref);
	return *this;
}

//---------------------------------------------------------------------------
tRisseVariant & tRisseVariant::operator =(const tTVInteger ref) // from Integer64
{
	ReleaseContent();
	vt=tvtInteger;
	Integer=ref;
	return *this;
}

//---------------------------------------------------------------------------
tRisseVariant & tRisseVariant::operator =(risse_real ref) // from double
{
	ReleaseContent();
	RisseSetFPUE();
	vt=tvtReal;
	Real=ref;
	return *this;
}
//---------------------------------------------------------------------------
bool tRisseVariant::NormalCompare(const tRisseVariant &val2) const
{
	try
	{

		if(vt == val2.vt)
		{
			if(vt == tvtInteger)
			{
				return Integer == val2.Integer;
			}

			if(vt == tvtString)
			{
				tRisseVariantString *s1, *s2;
				s1 = String;
				s2 = val2.String;
				if(s1 == s2) return true; // both empty string or the same pointer
				if(!s1 && s2) return false;
				if(s1 && !s2) return false;
				if(s1->Length != s2->Length) return false;
				return (! Risse_strcmp(*s1, *s2));
			}

			if(vt == tvtOctet)
			{
				if(Octet == val2.Octet) return true; // both empty octet or the same pointer
				if(!Octet && val2.Octet) return false;
				if(Octet && !val2.Octet) return false;
				if(Octet->GetLength() != val2.Octet->GetLength()) return false;
				return !Risse_octetcmp(Octet->GetData(), val2.Octet->GetData(),
					Octet->GetLength());
			}

			if(vt == tvtObject)
			{
				return Object.Object == val2.Object.Object/* &&
					Object.ObjThis == val2.Object.ObjThis*/;
			}

			if(vt == tvtVoid) return true;
		}

		if(vt==tvtString || val2.vt==tvtString)
		{
			tRisseVariantString *s1, *s2;
			s1 = AsString();
			s2 = val2.AsString();
			if(!s1 && !s2) return true; // both empty string
			if(!s1 && s2)
			{
				s2->Release();
				return false;
			}
			if(s1 && !s2)
			{
				s1->Release();
				return false;
			}
			bool res = ! Risse_strcmp(*s1, *s2);
			s1->Release();
			s2->Release();
			return res;
		}

		if(vt == tvtVoid)
		{
			switch(val2.vt)
			{
			case tvtInteger:	return val2.Integer == 0;
			case tvtReal:		{ RisseSetFPUE(); return val2.Real == 0; }
			case tvtString:		return val2.String == 0;
			default:			return false;
			}
		}
		if(val2.vt == tvtVoid)
		{
			switch(vt)
			{
			case tvtInteger:	return Integer == 0;
			case tvtReal:		{ RisseSetFPUE(); return Real == 0; }
			case tvtString:		return String == 0;
			default:			return false;
			}
		}

		RisseSetFPUE();

		tTVReal r1 = AsReal();
		tTVReal r2 = val2.AsReal();

		risse_uint32 c1 = RisseGetFPClass(r1);
		risse_uint32 c2 = RisseGetFPClass(r2);

		if(RISSE_FC_IS_NAN(c1) || RISSE_FC_IS_NAN(c2)) return false;
			// compare to NaN is always false
		if(RISSE_FC_IS_INF(c1) || RISSE_FC_IS_INF(c2))
		{
			return c1 == c2;
			// +inf == +inf : true , -inf == -inf : true, otherwise : false
		}
		return r1 == r2;
	}
	catch(eRisseVariantError &e)
	{
		return false;
	}
	catch(...)
	{
		throw;
	}
}
//---------------------------------------------------------------------------
bool tRisseVariant::DiscernCompare(const tRisseVariant &val2) const
{
	if(vt==val2.vt)
	{
		switch(vt)
		{
		case tvtObject:
			return Object.Object == val2.Object.Object &&
				Object.ObjThis == val2.Object.ObjThis;
		case tvtString:
			return NormalCompare(val2);
		case tvtOctet:
			return NormalCompare(val2);
		case tvtVoid:
			return true;
		case tvtReal:
		  {
			RisseSetFPUE();

			risse_uint32 c1 = RisseGetFPClass(Real);
			risse_uint32 c2 = RisseGetFPClass(val2.Real);

			if(RISSE_FC_IS_NAN(c1) || RISSE_FC_IS_NAN(c2)) return false;
				// compare to NaN is always false
			if(RISSE_FC_IS_INF(c1) || RISSE_FC_IS_INF(c2))
			{
				return c1 == c2;
				// +inf == +inf : true , -inf == -inf : true, otherwise : false
			}
			return Real == val2.Real;
		  }
		case tvtInteger:
			return Integer == val2.Integer;
		}
		return false;
	}
	else
	{
		return false;
	}
}
//---------------------------------------------------------------------------
bool tRisseVariant::DiscernCompareStrictReal(const tRisseVariant &val2) const
{
	// this performs strict real compare
	if(vt == val2.vt && vt == tvtReal)
	{
		risse_uint64 *ui64 = (risse_uint64*)&Real;
		risse_uint64 *v2ui64 = (risse_uint64*)&(val2.Real);
		return *ui64 == *v2ui64;
	}

	return DiscernCompare(val2);
}
//---------------------------------------------------------------------------
bool tRisseVariant::GreaterThan(const tRisseVariant &val2) const
{
	if(vt!=tvtString || val2.vt!=tvtString)
	{
		// compare as number
		if(vt==tvtInteger && val2.vt==tvtInteger)
		{
			return AsInteger()<val2.AsInteger();
		}
		RisseSetFPUE();
		return AsReal()<val2.AsReal();
	}
	// compare as string
	tRisseVariantString *s1, *s2;
	s1 = AsString();
	s2 = val2.AsString();
	const risse_char *p1 = *s1;
	const risse_char *p2 = *s2;
	static risse_char empty[] = { 0 };
	if(!p1) p1=empty;
	if(!p2) p2=empty;
	bool res = Risse_strcmp(p1, p2)<0;
	if(s1) s1->Release();
	if(s2) s2->Release();
	return res;
}
//---------------------------------------------------------------------------
bool tRisseVariant::LittlerThan(const tRisseVariant &val2) const
{
	if(vt!=tvtString || val2.vt!=tvtString)
	{
		// compare as number
		if(vt==tvtInteger && val2.vt==tvtInteger)
		{
			return AsInteger()>val2.AsInteger();
		}
		RisseSetFPUE();
		return AsReal()>val2.AsReal();
	}
	// compare as string
	tRisseVariantString *s1, *s2;
	s1 = AsString();
	s2 = val2.AsString();
	const risse_char *p1 = *s1;
	const risse_char *p2 = *s2;
	static risse_char empty[] = { 0 };
	if(!p1) p1=empty;
	if(!p2) p2=empty;
	bool res = Risse_strcmp(p1, p2)>0;
	if(s1) s1->Release();
	if(s2) s2->Release();
	return res;
}
//---------------------------------------------------------------------------
void tRisseVariant::logicalorequal (const tRisseVariant &rhs)
{
	bool l=operator bool();
	ReleaseContent();
	vt=tvtInteger;
	Integer=l||rhs.operator bool();
}
//---------------------------------------------------------------------------
void tRisseVariant::logicalandequal (const tRisseVariant &rhs)
{
	bool l=operator bool();
	ReleaseContent();
	vt=tvtInteger;
	Integer=l&&rhs.operator bool();
}
//---------------------------------------------------------------------------
void tRisseVariant::operator |= (const tRisseVariant &rhs)
{
	tTVInteger l=AsInteger();
	ReleaseContent();
	vt=tvtInteger;
	Integer=l|rhs.AsInteger();
}
//---------------------------------------------------------------------------
void tRisseVariant::increment(void)
{
	if(vt == tvtString)
		String->ToNumber(*this);

	if(vt == tvtReal)
	{
		RisseSetFPUE();
		Real+=1.0;
	}
	else if(vt == tvtInteger)
		Integer ++;
	else if(vt == tvtVoid)
		vt = tvtInteger, Integer = 1;
	else
		RisseThrowVariantConvertError(*this, tvtInteger, tvtReal);
}
//---------------------------------------------------------------------------
void tRisseVariant::decrement(void)
{
	if(vt == tvtString)
		String->ToNumber(*this);

	if(vt == tvtReal)
	{
		RisseSetFPUE();
		Real-=1.0;
	}
	else if(vt == tvtInteger)
		Integer --;
	else if(vt == tvtVoid)
		vt = tvtInteger, Integer = -1;
	else
		RisseThrowVariantConvertError(*this, tvtInteger, tvtReal);
}
//---------------------------------------------------------------------------
void tRisseVariant::operator ^= (const tRisseVariant &rhs)
{
	risse_int64 l=AsInteger();
	ReleaseContent();
	vt=tvtInteger;
	Integer=l^rhs.AsInteger();
}
//---------------------------------------------------------------------------
void tRisseVariant::operator &= (const tRisseVariant &rhs)
{
	tTVInteger l=AsInteger();
	ReleaseContent();
	vt=tvtInteger;
	Integer=l&rhs.AsInteger();
}
//---------------------------------------------------------------------------
void tRisseVariant::operator >>= (const tRisseVariant &rhs)
{
	tTVInteger l=AsInteger();
	ReleaseContent();
	vt=tvtInteger;
	Integer=l>>(risse_int)rhs.AsInteger();
}
//---------------------------------------------------------------------------
void tRisseVariant::rbitshiftequal (const tRisseVariant &rhs)
{
	tTVInteger l=AsInteger();
	ReleaseContent();
	vt=tvtInteger;
	Integer=(risse_int64)((risse_uint64)l>> (risse_int)rhs);
}
//---------------------------------------------------------------------------
void tRisseVariant::operator <<= (const tRisseVariant &rhs)
{
	tTVInteger l=AsInteger();
	ReleaseContent();
	vt=tvtInteger;
	Integer=l<<(risse_int)rhs.AsInteger();
}
//---------------------------------------------------------------------------
void tRisseVariant::operator %= (const tRisseVariant &rhs)
{
	tTVInteger r=rhs.AsInteger();
	if(r == 0) RisseThrowDivideByZero();
	tTVInteger l=AsInteger();
	ReleaseContent();
	vt=tvtInteger;
	Integer=l%r;
}
//---------------------------------------------------------------------------
void tRisseVariant::operator /= (const tRisseVariant &rhs)
{
	RisseSetFPUE();
	tTVReal l=AsReal();
	tTVReal r=rhs.AsReal();
	ReleaseContent();
	vt=tvtReal;
	Real=l/r;
}
//---------------------------------------------------------------------------
void tRisseVariant::idivequal (const tRisseVariant &rhs)
{
	tTVInteger r=rhs.AsInteger();
	tTVInteger l=AsInteger();
	if(r == 0) RisseThrowDivideByZero();
	ReleaseContent();
	vt=tvtInteger;
	Integer=l/r;
}
//---------------------------------------------------------------------------
void tRisseVariant::InternalMul(const tRisseVariant &rhs)
{
	tRisseVariant l;
	AsNumber(l);
	ReleaseContent();
	tRisseVariant r;
	rhs.AsNumber(r);
	if(l.vt == tvtInteger && r.vt == tvtInteger)
	{
		vt = tvtInteger;
		Integer = l.Integer * r.Integer;
		return;
	}
	vt = tvtReal;
	RisseSetFPUE();
	Real = l.AsReal() * r.AsReal();
}
//---------------------------------------------------------------------------
void tRisseVariant::logicalnot()
{
	bool res = !operator bool();
	ReleaseContent();
	vt = tvtInteger;
	Integer = (risse_int)res;
}
//---------------------------------------------------------------------------
void tRisseVariant::bitnot()
{
	risse_int64 res = ~AsInteger();
	ReleaseContent();
	vt = tvtInteger;
	Integer = res;
}
//---------------------------------------------------------------------------
void tRisseVariant::tonumber()
{
	if(vt==tvtInteger || vt==tvtReal)
		return; // nothing to do

	if(vt==tvtString)
	{
		String->ToNumber(*this);
		return;
	}

	if(vt==tvtVoid) { *this = (tTVInteger)0; return; }

	RisseThrowVariantConvertError(*this, tvtInteger, tvtReal);
}
//---------------------------------------------------------------------------
void tRisseVariant::InternalChangeSign()
{
	tRisseVariant val;
	AsNumber(val);
	ReleaseContent();
	if(val.vt == tvtInteger)
	{
		vt = tvtInteger;
		Integer = -val.Integer;
	}
	else
	{
		vt = tvtReal;
		RisseSetFPUE();
		Real = -val.Real;
	}
}
//---------------------------------------------------------------------------
void tRisseVariant::InternalSub(const tRisseVariant &rhs)
{
	tRisseVariant l;
	AsNumber(l);
	ReleaseContent();
	tRisseVariant r;
	rhs.AsNumber(r);
	if(l.vt == tvtInteger && r.vt == tvtInteger)
	{
		vt = tvtInteger;
		Integer = l.Integer - r.Integer;
		return;
	}
	vt = tvtReal;
	RisseSetFPUE();
	Real = l.AsReal() - r.AsReal();
}
//---------------------------------------------------------------------------
void tRisseVariant::operator +=(const tRisseVariant &rhs)
{
	if(vt==tvtString || rhs.vt==tvtString)
	{
		if(vt == tvtString && rhs.vt == tvtString)
		{
			// both are string

			// independ string
			if(String && String->GetRefCount() != 0)
			{
				// sever dependency
				tRisseVariantString *orgstr = String;
				String = RisseAllocVariantString(String->operator const risse_char*());
				orgstr->Release();
			}

			// append
			String = RisseAppendVariantString(String, rhs.String);
			return;
		}

		tRisseVariant val;
		val.vt = tvtString;
		tRisseVariantString *s1, *s2;
		s1 = AsString();
		s2 = rhs.AsString();
		val.String = RisseAllocVariantString(*s1, *s2);
		if(s1) s1->Release();
		if(s2) s2->Release();   
		*this=val;
		return;
	}


	if(vt == rhs.vt)
	{
		if(vt==tvtOctet)
		{
			tRisseVariant val;
			val.vt = tvtOctet;
			val.Octet = RisseAllocVariantOctet(Octet, rhs.Octet);
			*this=val;
			return;
		}

		if(vt==tvtInteger)
		{
			tTVInteger l=Integer;
			ReleaseContent();
			vt=tvtInteger;
			Integer=l+rhs.Integer;
			return;
		}
	}

	if(vt == tvtVoid)
	{
		if(rhs.vt == tvtInteger)
		{
			vt = tvtInteger;
			Integer = rhs.Integer;
			return;
		}
		if(rhs.vt == tvtReal)
		{
			vt = tvtReal;
			RisseSetFPUE();
			Real = rhs.Real;
			return;
		}
	}

	if(rhs.vt == tvtVoid)
	{
		if(vt == tvtInteger) return;
		if(vt == tvtReal) return;
	}

	RisseSetFPUE();
	tTVReal l=AsReal();
	ReleaseContent();
	vt=tvtReal;
	Real=l+rhs.AsReal();
}
//---------------------------------------------------------------------------
bool tRisseVariant::IsInstanceOf(const risse_char * classname) const
{
	// not implemented
	return false;
}
//---------------------------------------------------------------------------
risse_int tRisseVariant::QueryPersistSize() const
{
	// return the size, in bytes, of storage needed to store current state.
	// this state cannot walk across platforms.

	switch(vt)
	{
	case tvtVoid:
		return 1;

	case tvtObject:
		return sizeof(tRisseVariantClosure_S) + 1;

	case tvtString:
		if(String) return sizeof(tRisseVariantString*) + 1 + String->QueryPersistSize();
		return sizeof(tRisseVariantString*) + 1;

	case tvtInteger:
		return sizeof(tTVInteger) + 1;

	case tvtReal:
		return sizeof(tTVReal) + 1;

	case tvtOctet:
		if(Octet) return sizeof(tRisseVariantOctet*) + 1 + Octet->QueryPersistSize();
		return sizeof(tRisseVariantOctet*) + 1;
	}

	return 0;
}
//---------------------------------------------------------------------------
void tRisseVariant::Persist(risse_uint8 * dest)
{
	// store current state to dest
	*dest = (risse_uint8)vt;
	dest++;

	switch(vt)
	{
	case tvtVoid:
		break;

	case tvtObject:
		*(tRisseVariantClosure_S*)(dest) = Object;
		break;

	case tvtString:
		if(String)
			String->Persist(dest);
		else
			*(risse_uint*)(dest) = 0;
		break;

	case tvtInteger:
		*(tTVInteger*)(dest) = Integer;
		break;

	case tvtReal:
		*(tTVReal*)(dest) = Real;
		break;

	case tvtOctet:
		if(Octet)
			Octet->Persist(dest);
		else
			*(risse_uint*)(dest) = 0;
		break;
	}
}
//---------------------------------------------------------------------------


} // namespace Risse

