//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Dictionary クラスの実装
//---------------------------------------------------------------------------

#include "risseCommHead.h"

#include "risseDictionary.h"
#include "risseArray.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(20974,40492,34098,18873,32396,15263,54513,4852);
//---------------------------------------------------------------------------
static risse_int32 ClassID_Dictionary;
//---------------------------------------------------------------------------
// tRisseDictionaryClass : tRisseDictionary class
//---------------------------------------------------------------------------
risse_uint32 tRisseDictionaryClass::ClassID = (risse_uint32)-1;
tRisseDictionaryClass::tRisseDictionaryClass() :
	tRisseNativeClass(RISSE_WS("Dictionary"))
{
	// risse class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/Dictionary)
//---------------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/* var. name */_this,
	/* var. type */tRisseDictionaryNI, /* Risse class name */ Dictionary)
{
	return RISSE_S_OK;
}
RISSE_END_NATIVE_STATIC_CONSTRUCTOR_DECL(/*Risse class name*/Dictionary)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/load)
{
	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseDictionaryNI);
	if(!ni->IsValid()) return RISSE_E_INVALIDOBJECT;

	// TODO: implement Dictionary.load()
	return RISSE_S_OK;
}
RISSE_END_NATIVE_STATIC_METHOD_DECL(/*func. name*/load)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func.name*/save)
{
	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseDictionaryNI);
	if(!ni->IsValid()) return RISSE_E_INVALIDOBJECT;

	// TODO: implement Dictionary.save();
	return RISSE_S_OK;
}
RISSE_END_NATIVE_STATIC_METHOD_DECL(/*func.name*/save)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func.name*/saveStruct)
{
	// Structured output for flie;
	// the content can be interpret as an expression to re-construct the object.

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseDictionaryNI);
	if(!ni->IsValid()) return RISSE_E_INVALIDOBJECT;

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	ttstr name(*param[0]);
	ttstr mode;
	if(numparams >= 2 && param[1]->Type() != tvtVoid) mode = *param[1];

	iRisseTextWriteStream * stream = RisseCreateTextStreamForWrite(name, mode);
	try
	{
		std::vector<iRisseDispatch2 *> stack;
		stack.push_back(objthis);
		tRisseStringAppender string;
		ni->SaveStructuredData(stack, string, RISSE_WS(""));
		stream->Write(ttstr(string.GetData(), string.GetLen()));
	}
	catch(...)
	{
		delete stream;
		throw;
	}
	delete stream;

	if(result) *result = tRisseVariant(objthis, objthis);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_STATIC_METHOD_DECL(/*func.name*/saveStruct)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func.name*/assign)
{
	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseDictionaryNI);
	if(!ni->IsValid()) return RISSE_E_INVALIDOBJECT;

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	bool clear = true;
	if(numparams >= 2 && param[1]->Type() != tvtVoid)
		clear = (risse_int)*param[1];

	tRisseVariantClosure clo = param[0]->AsObjectClosureNoAddRef();
	if(clo.ObjThis)
		ni->Assign(clo.ObjThis, clear);
	else if(clo.Object)
		ni->Assign(clo.Object, clear);
#ifdef _MSC_VER
	else Risse::Risse_eRisseError(RisseNullAccess);
#else
	else Risse_eRisseError(RisseNullAccess);
#endif

	return RISSE_S_OK;
}
RISSE_END_NATIVE_STATIC_METHOD_DECL(/*func.name*/assign)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */assignStruct)
{
	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseDictionaryNI);
	if(!ni->IsValid()) return RISSE_E_INVALIDOBJECT;

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	std::vector<iRisseDispatch2 *> stack;

	tRisseVariantClosure clo = param[0]->AsObjectClosureNoAddRef();
	if(clo.ObjThis)
		ni->AssignStructure(clo.ObjThis, stack);
	else if(clo.Object)
		ni->AssignStructure(clo.Object, stack);
#ifdef _MSC_VER
	else Risse::Risse_eRisseError(RisseNullAccess);
#else
	else Risse_eRisseError(RisseNullAccess);
#endif 

	return RISSE_S_OK;
}
RISSE_END_NATIVE_STATIC_METHOD_DECL(/* func.name */assignStruct)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func.name*/clear)
{
	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseDictionaryNI);
	if(!ni->IsValid()) return RISSE_E_INVALIDOBJECT;

	ni->Clear();

	return RISSE_S_OK;
}
RISSE_END_NATIVE_STATIC_METHOD_DECL(/*func.name*/clear)
//----------------------------------------------------------------------

	ClassID_Dictionary = RISSE_NCM_CLASSID;
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
tRisseDictionaryClass::~tRisseDictionaryClass()
{
}
//---------------------------------------------------------------------------
tRisseNativeInstance *tRisseDictionaryClass::CreateNativeInstance()
{
	return new tRisseDictionaryNI();
}
//---------------------------------------------------------------------------
iRisseDispatch2 *tRisseDictionaryClass::CreateBaseRisseObject()
{
	return new tRisseDictionaryObject();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseDictionaryNI
//---------------------------------------------------------------------------
tRisseDictionaryNI::tRisseDictionaryNI()
{
	Owner = NULL;
}
//---------------------------------------------------------------------------
tRisseDictionaryNI::~tRisseDictionaryNI()
{
}
//---------------------------------------------------------------------------
risse_error tRisseDictionaryNI::Construct(risse_int numparams,
	tRisseVariant **param, iRisseDispatch2 *risseobj)
{
	// called from risse constructor
	if(numparams != 0) return RISSE_E_BADPARAMCOUNT;
	Owner = static_cast<tRisseCustomObject*>(risseobj);
	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
void tRisseDictionaryNI::Invalidate() // Invalidate override
{
	// put here something on invalidation
	Owner = NULL;
	inherited::Invalidate();
}
//---------------------------------------------------------------------------
void tRisseDictionaryNI::Assign(iRisseDispatch2 * dsp, bool clear)
{
	// copy members from "dsp" to "Owner"

	// determin dsp's object type
	tRisseArrayNI *arrayni = NULL;
	if(dsp && RISSE_SUCCEEDED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
		RisseGetArrayClassID(), (iRisseNativeInstance**)&arrayni)) )
	{
		// convert from array
		if(clear) Owner->Clear();

		tRisseArrayNI::tArrayItemIterator i;
		for(i = arrayni->Items.begin(); i != arrayni->Items.end(); i++)
		{
			tRisseVariantString *name = i->AsStringNoAddRef();
			i++;
			if(arrayni->Items.end() == i) break;
			Owner->PropSetByVS(RISSE_MEMBERENSURE|RISSE_IGNOREPROP, name,
				&(*i),
				Owner);
		}
	}
	else
	{
		// otherwise
		if(clear) Owner->Clear();
		tAssignCallback callback;
		callback.Owner = Owner;
		tRisseVariantClosure clo(&callback, NULL);
		dsp->EnumMembers(RISSE_IGNOREPROP, &clo, dsp);

	}
}
//---------------------------------------------------------------------------
void tRisseDictionaryNI::Clear()
{
	Owner->Clear();
}
//---------------------------------------------------------------------------
risse_error
tRisseDictionaryNI::tAssignCallback::FuncCall(risse_uint32 flag,
	const risse_char * membername, risse_uint32 *hint, tRisseVariant *result,
	risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis)
{
	// called from iRisseDispatch2::EnumMembers
	// (tRisseDictionaryNI::Assign calls iRisseDispatch2::EnumMembers)
	if(numparams < 3) return RISSE_E_BADPARAMCOUNT;

	// hidden members are not copied
	risse_uint32 flags = (risse_int)*param[1];
	if(flags & RISSE_HIDDENMEMBER)
	{
		if(result) *result = (tTVInteger)1;
		return RISSE_S_OK;
	}

	Owner->PropSetByVS(RISSE_MEMBERENSURE|RISSE_IGNOREPROP|flags,
		param[0]->AsStringNoAddRef(), param[2], Owner);

	if(result) *result = (tTVInteger)1;
	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
void tRisseDictionaryNI::SaveStructuredData(std::vector<iRisseDispatch2 *> &stack,
	tRisseStringAppender & string, const ttstr &indentstr)
{
#ifdef RISSE_TEXT_OUT_CRLF
	string += RISSE_WS("%[\r\n");
#else
	string += RISSE_WS("%[\n");
#endif
	ttstr indentstr2 = indentstr + RISSE_WS(" ");

	tSaveStructCallback callback;
	callback.Stack = &stack;
	callback.String = &string;
	callback.IndentStr = &indentstr2;
	callback.First = true;
	tRisseVariantClosure clo(&callback, NULL);
	Owner->EnumMembers(RISSE_IGNOREPROP, &clo, Owner);

#ifdef RISSE_TEXT_OUT_CRLF
	if(!callback.First) string += RISSE_WS("\r\n");
#else
	if(!callback.First) string += RISSE_WS("\n");
#endif
	string += indentstr;
	string += RISSE_WS("]");
}
//---------------------------------------------------------------------------
risse_error tRisseDictionaryNI::tSaveStructCallback::FuncCall(
	risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	tRisseVariant *result, risse_int numparams, tRisseVariant **param,
	iRisseDispatch2 *objthis)
{
	// called indirectly from tRisseDictionaryNI::SaveStructuredData

	if(numparams < 3) return RISSE_E_BADPARAMCOUNT;

	// hidden members are not processed
	risse_uint32 flags = (risse_int)*param[1];
	if(flags & RISSE_HIDDENMEMBER)
	{
		if(result) *result = (tTVInteger)1;
		return RISSE_S_OK;
	}

#ifdef RISSE_TEXT_OUT_CRLF
	if(!First) *String += RISSE_WS(",\r\n");
#else
	if(!First) *String += RISSE_WS(",\n");
#endif

	First = false;

	*String += *IndentStr;

	*String += RISSE_WS("\"");
	*String += ttstr(*param[0]).EscapeC();
	*String += RISSE_WS("\" => ");

	tRisseVariantType type = param[2]->Type();
	if(type == tvtObject)
	{
		// object
		tRisseVariantClosure clo = param[2]->AsObjectClosureNoAddRef();
		tRisseArrayNI::SaveStructuredDataForObject(clo.SelectObjectNoAddRef(),
			*Stack, *String, *IndentStr);
	}
	else
	{
		*String += RisseVariantToExpressionString(*param[2]);
	}

	if(result) *result = (tTVInteger)1;
	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
void tRisseDictionaryNI::AssignStructure(iRisseDispatch2 * dsp,
	std::vector<iRisseDispatch2 *> &stack)
{
	// assign structured data from dsp
	tRisseArrayNI *dicni = NULL;
	if(RISSE_SUCCEEDED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
		ClassID_Dictionary, (iRisseNativeInstance**)&dicni)) )
	{
		// copy from dictionary
		stack.push_back(dsp);
		try
		{
			Owner->Clear();

			tAssignStructCallback callback;
			callback.Dest = Owner;
			callback.Stack = &stack;

			tRisseVariantClosure clo(&callback, NULL);
			dsp->EnumMembers(RISSE_IGNOREPROP, &clo, dsp);
		}
		catch(...)
		{
			stack.pop_back();
			throw;
		}
		stack.pop_back();
	}
	else
	{
		Risse_eRisseError(RisseSpecifyDicOrArray);
	}

}
//---------------------------------------------------------------------------
risse_error tRisseDictionaryNI::tAssignStructCallback::FuncCall(
	risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	tRisseVariant *result, risse_int numparams, tRisseVariant **param,
	iRisseDispatch2 *objthis)
{
	// called indirectly from tRisseDictionaryNI::AssignStructure or
	// tRisseArrayNI::AssignStructure

	if(numparams < 3) return RISSE_E_BADPARAMCOUNT;

	// hidden members are not processed
	risse_uint32 flags = (risse_int)*param[1];
	if(flags & RISSE_HIDDENMEMBER)
	{
		if(result) *result = (tTVInteger)1;
		return RISSE_S_OK;
	}

	tRisseVariant &value = *param[2];

	tRisseVariantType type = value.Type();
	if(type == tvtObject)
	{
		// object

		iRisseDispatch2 *dsp = value.AsObjectNoAddRef();
		// determin dsp's object type

		tRisseVariant val;

		tRisseDictionaryNI *dicni = NULL;
		tRisseArrayNI *arrayni = NULL;

		if(dsp && RISSE_SUCCEEDED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
			RisseGetDictionaryClassID(), (iRisseNativeInstance**)&dicni)) )
		{
			// dictionary
			bool objrec = false;
			std::vector<iRisseDispatch2 *>::iterator i;
			for(i = Stack->begin(); i != Stack->end(); i++)
			{
				if(*i == dsp)
				{
					// object recursion detected
					objrec = true;
					break;
				}
			}
			if(objrec)
			{
				val.SetObject(NULL); // becomes null
			}
			else
			{
				iRisseDispatch2 * newobj = RisseCreateDictionaryObject();
				val.SetObject(newobj, newobj);
				newobj->Release();
				tRisseDictionaryNI * newni = NULL;
				if(RISSE_SUCCEEDED(newobj->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
					RisseGetDictionaryClassID(), (iRisseNativeInstance**)&newni)) )
				{
					newni->AssignStructure(dsp, *Stack);
				}
			}
		}
		else if(dsp && RISSE_SUCCEEDED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
			RisseGetArrayClassID(), (iRisseNativeInstance**)&arrayni)) )
		{
			// array
			bool objrec = false;
			std::vector<iRisseDispatch2 *>::iterator i;
			for(i = Stack->begin(); i != Stack->end(); i++)
			{
				if(*i == dsp)
				{
					// object recursion detected
					objrec = true;
					break;
				}
			}
			if(objrec)
			{
				val.SetObject(NULL); // becomes null
			}
			else
			{
				iRisseDispatch2 * newobj = RisseCreateArrayObject();
				val.SetObject(newobj, newobj);
				newobj->Release();
				tRisseArrayNI * newni = NULL;
				if(RISSE_SUCCEEDED(newobj->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
					RisseGetArrayClassID(), (iRisseNativeInstance**)&newni)) )
				{
					newni->AssignStructure(dsp, *Stack);
				}
			}
		}
		else
		{
			// other object types
			val = value;
		}

		Dest->PropSetByVS(RISSE_MEMBERENSURE|RISSE_IGNOREPROP, param[0]->AsStringNoAddRef(), &val, Dest);
	}
	else
	{
		// other types
		Dest->PropSetByVS(RISSE_MEMBERENSURE|RISSE_IGNOREPROP, param[0]->AsStringNoAddRef(), &value, Dest);
	}

	if(result) *result = (tTVInteger)1;
	return RISSE_S_OK;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseDictionaryObject
//---------------------------------------------------------------------------
tRisseDictionaryObject::tRisseDictionaryObject() : tRisseCustomObject()
{
	CallFinalize = false;
}
//---------------------------------------------------------------------------
tRisseDictionaryObject::~tRisseDictionaryObject()
{
}
//---------------------------------------------------------------------------
risse_error
	tRisseDictionaryObject::FuncCall(risse_uint32 flag, const risse_char * membername,
		risse_uint32 *hint,
		tRisseVariant *result, risse_int numparams, tRisseVariant **param,
		iRisseDispatch2 *objthis)
{
	risse_error hr = inherited::FuncCall(flag, membername, hint, result, numparams,
		param, objthis);
//	if(hr == RISSE_E_MEMBERNOTFOUND)
//		return RISSE_E_INVALIDTYPE; // call operation for void
	return hr;
}
//---------------------------------------------------------------------------
risse_error
	tRisseDictionaryObject::PropGet(risse_uint32 flag, const risse_char * membername,
		risse_uint32 *hint,
		tRisseVariant *result, iRisseDispatch2 *objthis)
{
	risse_error hr;
	hr = inherited::PropGet(flag, membername, hint, result, objthis);
	if(hr == RISSE_E_MEMBERNOTFOUND && !(flag & RISSE_MEMBERMUSTEXIST))
	{
		if(result) result->Clear(); // returns void
		return RISSE_S_OK;
	}
	return hr;
}
//---------------------------------------------------------------------------
risse_error
	tRisseDictionaryObject::CreateNew(risse_uint32 flag, const risse_char * membername,
		risse_uint32 *hint,
		iRisseDispatch2 **result, risse_int numparams, tRisseVariant **param,
			iRisseDispatch2 *objthis)
{
	risse_error hr = inherited::CreateNew(flag, membername, hint, result, numparams,
		param, objthis);
	if(hr == RISSE_E_MEMBERNOTFOUND && !(flag & RISSE_MEMBERMUSTEXIST))
		return RISSE_E_INVALIDTYPE; // call operation for void
	return hr;
}
//---------------------------------------------------------------------------
risse_error
	tRisseDictionaryObject::Operation(risse_uint32 flag, const risse_char *membername,
		risse_uint32 *hint,
		tRisseVariant *result, const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	risse_error hr = inherited::Operation(flag, membername, hint, result, param, objthis);
	if(hr == RISSE_E_MEMBERNOTFOUND && !(flag & RISSE_MEMBERMUSTEXIST))
	{
		// value not found -> create a value, do the operation once more
		static tRisseVariant VoidVal;
		hr = inherited::PropSet(RISSE_MEMBERENSURE, membername, hint, &VoidVal, objthis);
		if(RISSE_FAILED(hr)) return hr;
		hr = inherited::Operation(flag, membername, hint, result, param, objthis);
	}
	return hr;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// RisseGetDictionaryClassID
//---------------------------------------------------------------------------
risse_int32 RisseGetDictionaryClassID()
{
	return ClassID_Dictionary;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// RisseCreateDictionaryObject
//---------------------------------------------------------------------------
iRisseDispatch2 * RisseCreateDictionaryObject(iRisseDispatch2 **classout)
{
	// create a Dictionary object
	struct tHolder
	{
		iRisseDispatch2 * Obj;
		tHolder() { Obj = new tRisseDictionaryClass(); }
		~tHolder() { Obj->Release(); }
	} static dictionaryclass;

	if(classout) *classout = dictionaryclass.Obj, dictionaryclass.Obj->AddRef();

	tRisseDictionaryObject *dictionaryobj;
	(dictionaryclass.Obj)->CreateNew(0, NULL,  NULL,
		(iRisseDispatch2**)&dictionaryobj, 0, NULL, dictionaryclass.Obj);
	return dictionaryobj;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risse









