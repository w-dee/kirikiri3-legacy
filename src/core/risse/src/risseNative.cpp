//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief C++ バインディングのサポート
//---------------------------------------------------------------------------
#include "risseCommHead.h"


#include "risseNative.h"
#include "risseError.h"
#include "risseGlobalStringMap.h"
#include "risseDebug.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(39831,33902,52867,18676,32909,55902,29093,52317);
//---------------------------------------------------------------------------
// NativeClass registration
//---------------------------------------------------------------------------
static std::vector<ttstr > NativeClassNames;
//---------------------------------------------------------------------------
risse_int32 RisseRegisterNativeClass(const risse_char *name)
{
	for(risse_uint i = 0; i<NativeClassNames.size(); i++)
	{
		if(NativeClassNames[i] == name) return i;
	}

	NativeClassNames.push_back(RisseMapGlobalStringMap(name));

	return NativeClassNames.size() -1;
}
//---------------------------------------------------------------------------
risse_int32 RisseFindNativeClassID(const risse_char *name)
{
	for(risse_uint i = 0; i<NativeClassNames.size(); i++)
	{
		if(NativeClassNames[i] == name) return i;
	}

	return -1;
}
//---------------------------------------------------------------------------
const risse_char * RisseFindNativeClassName(risse_int32 id)
{
	if(id<0 || id>=(risse_int32)NativeClassNames.size()) return NULL;
	return NativeClassNames[id].c_str();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseNativeClassMethod
//---------------------------------------------------------------------------
tRisseNativeClassMethod::tRisseNativeClassMethod(tRisseNativeClassMethodCallback processfunc)
{
	Process = processfunc;
	if(RisseObjectHashMapEnabled()) RisseAddObjectHashRecord(this);
}
//---------------------------------------------------------------------------
tRisseNativeClassMethod::~tRisseNativeClassMethod()
{
	if(RisseObjectHashMapEnabled()) RisseRemoveObjectHashRecord(this);
}
//---------------------------------------------------------------------------
risse_error
tRisseNativeClassMethod::IsInstanceOf(risse_uint32 flag,
	const risse_char *membername,  risse_uint32 *hint,
		const risse_char *classname, iRisseDispatch2 *objthis)
{
	if(membername == NULL)
	{
		if(!Risse_strcmp(classname, RISSE_WS("Function"))) return RISSE_S_TRUE;
	}

	return inherited::IsInstanceOf(flag, membername, hint, classname, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseNativeClassMethod::FuncCall(risse_uint32 flag, const risse_char * membername,
		risse_uint32 *hint, tRisseVariant *result,
		risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis)
{
	if(membername) return inherited::FuncCall(flag, membername, hint,
		result, numparams, param, objthis);
	if(!objthis) return RISSE_E_NATIVECLASSCRASH;

	if(result) result->Clear();
	risse_error er;
	try
	{
		er = Process(result, numparams, param, objthis);
	}
	catch(...)
	{
		throw;
	}
	return er;
}
//---------------------------------------------------------------------------
tRisseNativeClassMethod * RisseCreateNativeClassMethod
	(tRisseNativeClassMethodCallback callback)
{
	return new tRisseNativeClassMethod(callback);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseNativeClassConstructor
//---------------------------------------------------------------------------
risse_error 
	tRisseNativeClassConstructor::FuncCall(risse_uint32 flag,
	const risse_char * membername, risse_uint32 *hint,
	tRisseVariant *result,
	risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis)
{
	if(membername) return tRisseDispatch::FuncCall(flag, membername, hint,
		result, numparams, param, objthis);
	if(result) result->Clear();
	risse_error er;
	try
	{
		er = Process(result, numparams, param, objthis);
	}
	catch(...)
	{
		throw;
	}
	return er;
}
//---------------------------------------------------------------------------
tRisseNativeClassMethod * RisseCreateNativeClassConstructor
	(tRisseNativeClassMethodCallback callback)
{
	return new tRisseNativeClassConstructor(callback);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseNativeClassProperty
//---------------------------------------------------------------------------
tRisseNativeClassProperty::tRisseNativeClassProperty(
	tRisseNativeClassPropertyGetCallback get,
	tRisseNativeClassPropertySetCallback set)
{
	Get = get;
	Set = set;
	if(RisseObjectHashMapEnabled()) RisseAddObjectHashRecord(this);
}
//---------------------------------------------------------------------------
tRisseNativeClassProperty::~tRisseNativeClassProperty()
{
	if(RisseObjectHashMapEnabled()) RisseRemoveObjectHashRecord(this);
}
//---------------------------------------------------------------------------
risse_error
tRisseNativeClassProperty::IsInstanceOf(risse_uint32 flag,
	const risse_char *membername, risse_uint32 *hint,
		const risse_char *classname, iRisseDispatch2 *objthis)
{
	if(membername == NULL)
	{
		if(!Risse_strcmp(classname, RISSE_WS("Property"))) return RISSE_S_TRUE;
	}

	return inherited::IsInstanceOf(flag, membername, hint, classname, objthis);
}
//---------------------------------------------------------------------------
risse_error
tRisseNativeClassProperty::PropGet(risse_uint32 flag, const risse_char * membername,
	risse_uint32 *hint, tRisseVariant *result, iRisseDispatch2 *objthis)
{
	if(membername) return inherited::PropGet(flag, membername, hint,
		result, objthis);
	if(!objthis) return RISSE_E_NATIVECLASSCRASH;

	if(!result) return RISSE_E_FAIL;

	risse_error er;
	try
	{
		er = Get(result, objthis);
	}
	catch(...)
	{
		throw;
	}

	return er;
}
//---------------------------------------------------------------------------
risse_error
tRisseNativeClassProperty::PropSet(risse_uint32 flag, const risse_char *membername,
	risse_uint32 *hint, const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	if(membername) return inherited::PropSet(flag, membername, hint,
		param, objthis);
	if(!objthis) return RISSE_E_NATIVECLASSCRASH;

	if(!param) return RISSE_E_FAIL;

	risse_error er;
	try
	{
		er = Set(param, objthis);
	}
	catch(...)
	{
		throw;
	}

	return er;
}
//---------------------------------------------------------------------------
tRisseNativeClassProperty * RisseCreateNativeClassProperty(
	tRisseNativeClassPropertyGetCallback get,
	tRisseNativeClassPropertySetCallback set)
{
	return new tRisseNativeClassProperty(get, set);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseNativeClass
//---------------------------------------------------------------------------
tRisseNativeClass::tRisseNativeClass(const ttstr &name)
{
	CallFinalize = false;
	ClassName = RisseMapGlobalStringMap(name);

	if(RisseObjectHashMapEnabled())
		RisseObjectHashSetType(this, ttstr(RISSE_WS("(native class) ")) + ClassName);
}
//---------------------------------------------------------------------------
tRisseNativeClass::~tRisseNativeClass()
{
}
//---------------------------------------------------------------------------
void tRisseNativeClass::RegisterNCM(const risse_char *name, iRisseDispatch2 *dsp,
	const risse_char *classname, tRisseNativeInstanceType type, risse_uint32 flags)
{
	// map name via Global String Map
	ttstr tname = RisseMapGlobalStringMap(ttstr(name));

	// set object type for debugging
	if(RisseObjectHashMapEnabled())
	{
		switch(type)
		{
		case nitMethod:
			RisseObjectHashSetType(dsp, ttstr(RISSE_WS("(native function) ")) +
										classname + RISSE_WS(".") + name);
			break;
		case nitProperty:
			RisseObjectHashSetType(dsp, ttstr(RISSE_WS("(native property) ")) +
										classname + RISSE_WS(".") + name);
			break;
		case nitClass:
		/*
			The information is not set here
			(is to be set in tRisseNativeClass::tRisseNativeClass)
		*/
			break;
		}
	}

	// add to this
	tRisseVariant val;
	val = dsp;
	if(PropSetByVS((RISSE_MEMBERENSURE | RISSE_IGNOREPROP) | flags,
		tname.AsVariantStringNoAddRef(), &val, this) == RISSE_E_NOTIMPL)
		PropSet((RISSE_MEMBERENSURE | RISSE_IGNOREPROP) | flags,
			tname.c_str(), NULL, &val, this);

	// release dsp
	dsp->Release();
}
//---------------------------------------------------------------------------
void tRisseNativeClass::Finalize(void)
{
	tRisseCustomObject::Finalize();
}
//---------------------------------------------------------------------------
iRisseDispatch2 * tRisseNativeClass::CreateBaseRisseObject()
{
	return new tRisseCustomObject;
}
//---------------------------------------------------------------------------
risse_error
tRisseNativeClass::FuncCall(risse_uint32 flag, const risse_char * membername,
	risse_uint32 *hint,
	tRisseVariant *result, risse_int numparams, tRisseVariant **param,
	iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	if(membername) return tRisseCustomObject::FuncCall(flag, membername, hint,
		result, numparams, param, objthis);

	tRisseVariant name(ClassName);
	objthis->ClassInstanceInfo(RISSE_CII_ADD, 0, &name); // add class name

	// create base native object
	iRisseNativeInstance * nativeptr = CreateNativeInstance();

	// register native instance information to the object;
	// even if "nativeptr" is null
	objthis->NativeInstanceSupport(RISSE_NIS_REGISTER, _ClassID, &nativeptr);

	// register members to "objthis"

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
					if(val.AsObjectThisNoAddRef() == NULL)
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
	callback.Dest = objthis;

	// enumerate members
	tRisseVariantClosure clo(&callback, (iRisseDispatch2*)NULL);
	EnumMembers(RISSE_IGNOREPROP, &clo, this);

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
risse_error
tRisseNativeClass::CreateNew(risse_uint32 flag, const risse_char * membername,
	risse_uint32 *hint,
	iRisseDispatch2 **result, risse_int numparams, tRisseVariant **param,
	iRisseDispatch2 *objthis)
{
	// CreateNew

	iRisseDispatch2 *dsp = CreateBaseRisseObject();

	risse_error hr;
	try
	{
		// set object type for debugging
		if(RisseObjectHashMapEnabled())
			RisseObjectHashSetType(dsp, RISSE_WS("instance of class ") + ClassName);

		// instance initialization
		hr = FuncCall(0, NULL, NULL, NULL, 0, NULL, dsp); // add member to dsp

		if(RISSE_FAILED(hr)) return hr;

		hr = FuncCall(0, ClassName.c_str(), ClassName.GetHint(), NULL, numparams, param, dsp);
			// call the constructor
		if(hr == RISSE_E_MEMBERNOTFOUND) hr = RISSE_S_OK;
			// missing constructor is OK ( is this ugly ? )
	}
	catch(...)
	{
		dsp->Release();
		throw;
	}

	if(RISSE_SUCCEEDED(hr)) *result = dsp;
	return hr;
}
//---------------------------------------------------------------------------
risse_error
tRisseNativeClass::IsInstanceOf(risse_uint32 flag,
	const risse_char *membername, risse_uint32 *hint, const risse_char *classname,
		iRisseDispatch2 *objthis)
{
	if(membername == NULL)
	{
		if(!Risse_strcmp(classname, RISSE_WS("Class"))) return RISSE_S_TRUE;
		if(!Risse_strcmp(classname, ClassName.c_str())) return RISSE_S_TRUE;
	}

	return inherited::IsInstanceOf(flag, membername, hint, classname, objthis);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tRisseNativeFunction
//---------------------------------------------------------------------------
tRisseNativeFunction::tRisseNativeFunction(const risse_char *name)
{
	if(RisseObjectHashMapEnabled())
	{
		RisseAddObjectHashRecord(this);
		if(name)
			RisseObjectHashSetType(this, ttstr(RISSE_WS("(native function) ")) + name);
	}
}
//---------------------------------------------------------------------------
tRisseNativeFunction::~tRisseNativeFunction()
{
	if(RisseObjectHashMapEnabled()) RisseRemoveObjectHashRecord(this);
}
//---------------------------------------------------------------------------
risse_error tRisseNativeFunction::FuncCall(
	risse_uint32 flag, const risse_char * membername, risse_uint32 *hint, tRisseVariant *result,
	risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis)
{
	if(membername)
	{
		return inherited::FuncCall(flag, membername, hint, result,
			numparams, param, objthis);
	}
	return Process(result, numparams, param, objthis);
}
//---------------------------------------------------------------------------
risse_error tRisseNativeFunction::IsInstanceOf(
	risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
	const risse_char *classname, iRisseDispatch2 *objthis)
{
	if(membername == NULL)
	{
		if(!Risse_strcmp(classname, RISSE_WS("Function"))) return RISSE_S_TRUE;
	}

	return inherited::IsInstanceOf(flag, membername, hint, classname, objthis);
}
//---------------------------------------------------------------------------
} // namespace Risse

