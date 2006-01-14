//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief C++ バインディングのサポート
//---------------------------------------------------------------------------
#ifndef risseNativeH
#define risseNativeH

#include "risseObject.h"

namespace Risse
{
//---------------------------------------------------------------------------
risse_int32 RisseRegisterNativeClass(const risse_char *name);
risse_int32 RisseFindNativeClassID(const risse_char *name);
const risse_char *RisseFindNativeClassName(risse_int32 id);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// tRisseNativeInstanceType
//---------------------------------------------------------------------------
enum tRisseNativeInstanceType
{
	nitClass,
	nitMethod,
	nitProperty
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseNativeInstance
//---------------------------------------------------------------------------
class tRisseNativeInstance : public iRisseNativeInstance
{
public:
	virtual risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj) {return RISSE_S_OK;}
	virtual void Invalidate() {;}
	virtual void Destruct() { delete this; }
	virtual ~tRisseNativeInstance() {;};
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseNativeClassMethod
//---------------------------------------------------------------------------
typedef risse_error (*tRisseNativeClassMethodCallback)
	(tRisseVariant *result,risse_int numparams, tRisseVariant **param,
	iRisseDispatch2 *objthis);
#ifdef __TP_STUB_H__
class tRisseNativeClassMethod : public iRisseDispatch2 { };
#else
class tRisseNativeClassMethod;
#endif


tRisseNativeClassMethod *RisseCreateNativeClassMethod (tRisseNativeClassMethodCallback callback);
//---------------------------------------------------------------------------
class tRisseNativeClassMethod : public tRisseDispatch
{
	typedef tRisseDispatch inherited;

protected:
	tRisseNativeClassMethodCallback Process;
public:

	tRisseNativeClassMethod(tRisseNativeClassMethodCallback processfunc);
	~tRisseNativeClassMethod();

	risse_error
	IsInstanceOf(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		const risse_char *classname,
		iRisseDispatch2 *objthis);
	risse_error 
		FuncCall(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
		risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseNativeClassConstructor
//---------------------------------------------------------------------------
tRisseNativeClassMethod *RisseCreateNativeClassConstructor(tRisseNativeClassMethodCallback callback);
//---------------------------------------------------------------------------
class tRisseNativeClassConstructor : public tRisseNativeClassMethod
{
	typedef tRisseNativeClassMethod inherited;
public:
	tRisseNativeClassConstructor(tRisseNativeClassMethodCallback processfunc)
		: tRisseNativeClassMethod(processfunc) {;}

	risse_error 
		FuncCall(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
		risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseNativeClassProperty
//---------------------------------------------------------------------------
typedef risse_error (*tRisseNativeClassPropertyGetCallback)
	(tRisseVariant *result, iRisseDispatch2 *objthis);
typedef risse_error (*tRisseNativeClassPropertySetCallback)
	(const tRisseVariant *param, iRisseDispatch2 *objthis);
#ifdef __TP_STUB_H__
class tRisseNativeClassProperty : public iRisseDispatch2 { };
#else
class tRisseNativeClassProperty;
#endif

tRisseNativeClassProperty *RisseCreateNativeClassProperty
	(tRisseNativeClassPropertyGetCallback get,
	 tRisseNativeClassPropertySetCallback set);
//---------------------------------------------------------------------------
class tRisseNativeClassProperty : public tRisseDispatch
{
	typedef tRisseDispatch inherited;

protected:
	tRisseNativeClassPropertyGetCallback Get;
	tRisseNativeClassPropertySetCallback Set;
public:

	tRisseNativeClassProperty(tRisseNativeClassPropertyGetCallback get,
		tRisseNativeClassPropertySetCallback set);
	~tRisseNativeClassProperty();

	risse_error
	IsInstanceOf(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		const risse_char *classname,
		iRisseDispatch2 *objthis);

	risse_error
	PropGet(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
		iRisseDispatch2 *objthis);

	risse_error
	PropSet(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseNativeClass
//---------------------------------------------------------------------------
class tRisseNativeClass : public tRisseCustomObject
{
	typedef tRisseCustomObject inherited;

public:
	tRisseNativeClass(const ttstr &name);
	~tRisseNativeClass();

	void RegisterNCM(const risse_char *name,
		iRisseDispatch2 *dsp,
		const risse_char *classname,
		tRisseNativeInstanceType type,
		risse_uint32 flags = 0);

protected:
	risse_int32 _ClassID;
	ttstr ClassName;
	void Finalize(void);

	virtual iRisseNativeInstance *CreateNativeInstance()  {return NULL;}

	virtual iRisseDispatch2 *CreateBaseRisseObject();

private:

public:
	risse_error
	FuncCall(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
			risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis);
	risse_error
	CreateNew(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		 iRisseDispatch2 **result,
			risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis);
	risse_error
	IsInstanceOf(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		 const risse_char *classname,
		iRisseDispatch2 *objthis);


	const ttstr & GetClassName() const { return ClassName; }

	void SetClassID(risse_int32 classid) { _ClassID = classid; }
};
//---------------------------------------------------------------------------
inline void RisseNativeClassRegisterNCM(
		tRisseNativeClass *cls,
		const risse_char *name,
		iRisseDispatch2 *dsp,
		const risse_char *classname,
		tRisseNativeInstanceType type,
		risse_uint32 flags = 0)
{
	cls->RegisterNCM(name, dsp, classname, type, flags);
}
//---------------------------------------------------------------------------
inline void RisseNativeClassSetClassID(
		tRisseNativeClass *cls,
		risse_int32 classid)
{
	cls->SetClassID(classid);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseNativeClassForPlugin : service class for plugins
//---------------------------------------------------------------------------
typedef iRisseNativeInstance * (*tRisseCreateNativeInstance)();
#ifdef __TP_STUB_H__
class tRisseNativeClass : public iRisseDispatch2 { };
class tRisseNativeClassForPlugin : public tRisseNativeClass { };
#else
class tRisseNativeClassForPlugin;
#endif

//---------------------------------------------------------------------------
// This class is for nasty workaround of inter-compiler compatibility
class tRisseNativeClassForPlugin : public tRisseNativeClass
{
	tRisseCreateNativeInstance procCreateNativeInstance;
public:
	tRisseNativeClassForPlugin(const ttstr &name,
		tRisseCreateNativeInstance proc) :
			tRisseNativeClass(name), procCreateNativeInstance(proc)
	{
		;
	}

protected:
	iRisseNativeInstance *CreateNativeInstance()
		{return procCreateNativeInstance();}
};
//---------------------------------------------------------------------------
inline tRisseNativeClassForPlugin *RisseCreateNativeClassForPlugin
	(const ttstr &name, tRisseCreateNativeInstance createinstance)
{
	return new tRisseNativeClassForPlugin(name, createinstance);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// following macros are to be written in the constructor of child class
// to define native methods/properties.
#ifdef __TP_STUB_H__
#define RISSE_NCM_REG_THIS classobj
#define RISSE_NATIVE_SET_ClassID RISSE_NATIVE_CLASSID_NAME = RISSE_NCM_CLASSID;
#else
#define RISSE_NCM_REG_THIS  this
#define RISSE_NATIVE_SET_ClassID ClassID = RISSE_NCM_CLASSID;
#define RISSE_NATIVE_CLASSID_NAME RISSE_NCM_CLASSID
#endif

#define RISSE_GET_NATIVE_INSTANCE(varname, typename) \
		if(!objthis) return RISSE_E_NATIVECLASSCRASH; \
		typename *varname; \
		{ \
			risse_error hr; \
			hr = objthis->NativeInstanceSupport(RISSE_NIS_GETINSTANCE, \
					RISSE_NATIVE_CLASSID_NAME, (iRisseNativeInstance**)&varname); \
			if(RISSE_FAILED(hr)) return RISSE_E_NATIVECLASSCRASH; \
		}

#define RISSE_GET_NATIVE_INSTANCE_OUTER(classname, varname, typename) \
		if(!objthis) return RISSE_E_NATIVECLASSCRASH; \
		typename *varname; \
		{ \
			risse_error hr; \
			hr = objthis->NativeInstanceSupport(RISSE_NIS_GETINSTANCE, \
					classname::ClassID, (iRisseNativeInstance**)&varname); \
			if(RISSE_FAILED(hr)) return RISSE_E_NATIVECLASSCRASH; \
		}

#define RISSE_BEGIN_NATIVE_MEMBERS(classname) \
	{ \
		static const risse_char *__classname = RISSE_WS(#classname); \
		static risse_int32 RISSE_NCM_CLASSID = \
			RisseRegisterNativeClass(__classname); \
		RisseNativeClassSetClassID(RISSE_NCM_REG_THIS, RISSE_NCM_CLASSID); \
		RISSE_NATIVE_SET_ClassID

#define RISSE_BEGIN_NATIVE_METHOD_DECL(name) \
		struct NCM_##name { \
			static risse_error \
			Process( tRisseVariant *result, \
				risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis) {

#define RISSE_END_NATIVE_METHOD_DECL_INT \
			} \
		};

#define RISSE_END_NATIVE_METHOD_DECL(name) \
		RISSE_END_NATIVE_METHOD_DECL_INT \
		RisseNativeClassRegisterNCM(RISSE_NCM_REG_THIS, RISSE_WS(#name), \
		RisseCreateNativeClassMethod(NCM_##name::Process), __classname, nitMethod);

#define RISSE_END_NATIVE_HIDDEN_METHOD_DECL(name) \
		RISSE_END_NATIVE_METHOD_DECL_INT \
		RisseNativeClassRegisterNCM(RISSE_NCM_REG_THIS, RISSE_WS(#name), \
		RisseCreateNativeClassMethod(NCM_##name::Process), __classname, nitMethod, \
		RISSE_HIDDENMEMBER);

#define RISSE_END_NATIVE_STATIC_METHOD_DECL(name) \
		RISSE_END_NATIVE_METHOD_DECL_INT \
		RisseNativeClassRegisterNCM(RISSE_NCM_REG_THIS, RISSE_WS(#name), \
		RisseCreateNativeClassMethod(NCM_##name::Process), __classname, nitMethod, \
		RISSE_STATICMEMBER);

#define RISSE_END_NATIVE_METHOD_DECL_OUTER(object, name) \
		RISSE_END_NATIVE_METHOD_DECL_INT \
		RisseNativeClassRegisterNCM((object), RISSE_WS(#name), \
		RisseCreateNativeClassMethod(NCM_##name::Process), \
		(object)->GetClassName().c_str(), nitMethod);

#define RISSE_DECL_EMPTY_FINALIZE_METHOD \
	RISSE_BEGIN_NATIVE_METHOD_DECL(finalize) \
	{ return RISSE_S_OK; } \
	RISSE_END_NATIVE_METHOD_DECL(finalize)

#define RISSE_NATIVE_CONSTRUCTOR_CALL_NATIVE_CONSTRUCTOR(varname, typename) \
				typename *varname; \
				{ \
					risse_error hr; \
					hr = objthis->NativeInstanceSupport(RISSE_NIS_GETINSTANCE, \
						RISSE_NATIVE_CLASSID_NAME, \
						(iRisseNativeInstance**)&varname); \
					if(RISSE_FAILED(hr)) return RISSE_E_NATIVECLASSCRASH; \
					if(!varname) return RISSE_E_NATIVECLASSCRASH; \
					hr = varname->Construct(numparams, param, objthis); \
					if(RISSE_FAILED(hr)) return hr; \
				}

#define RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(classname) \
		struct NCM_##classname { \
			static risse_error \
			Process(tRisseVariant *result, \
			risse_int numparams, tRisseVariant **param,	iRisseDispatch2 *objthis) {

#define RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(varname, typename, classname) \
		RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(classname) \
		RISSE_NATIVE_CONSTRUCTOR_CALL_NATIVE_CONSTRUCTOR(varname, typename)

#define RISSE_END_NATIVE_CONSTRUCTOR_DECL(name) \
		RISSE_END_NATIVE_METHOD_DECL_INT \
		RisseNativeClassRegisterNCM(RISSE_NCM_REG_THIS, RISSE_WS(#name), \
		RisseCreateNativeClassConstructor(NCM_##name::Process), __classname, \
		nitMethod);

#define RISSE_END_NATIVE_STATIC_CONSTRUCTOR_DECL(name) \
		RISSE_END_NATIVE_METHOD_DECL_INT \
		RisseNativeClassRegisterNCM(RISSE_NCM_REG_THIS, RISSE_WS(#name), \
		RisseCreateNativeClassConstructor(NCM_##name::Process), __classname, \
		nitMethod, RISSE_STATICMEMBER);

#define RISSE_BEGIN_NATIVE_PROP_DECL(name) \
		struct NCM_##name

#define RISSE_END_NATIVE_PROP_DECL(name) \
		;RisseNativeClassRegisterNCM(RISSE_NCM_REG_THIS, RISSE_WS(#name), \
		RisseCreateNativeClassProperty(NCM_##name::Get, NCM_##name::Set), \
		__classname, nitProperty);

#define RISSE_END_NATIVE_PROP_DECL_OUTER(object, name) \
		;RisseNativeClassRegisterNCM((object), RISSE_WS(#name), \
		RisseCreateNativeClassProperty(NCM_##name::Get, NCM_##name::Set), \
		(object)->GetClassName().c_str(), nitProperty);

#define RISSE_END_NATIVE_STATIC_PROP_DECL(name) \
		;RisseNativeClassRegisterNCM(RISSE_NCM_REG_THIS, RISSE_WS(#name), \
		RisseCreateNativeClassProperty(NCM_##name::Get, NCM_##name::Set), \
		__classname, nitProperty, RISSE_STATICMEMBER);

#define RISSE_BEGIN_NATIVE_PROP_GETTER \
		static risse_error Get(tRisseVariant *result, \
		iRisseDispatch2 *objthis) { \

#define RISSE_END_NATIVE_PROP_GETTER \
		}

#define RISSE_DENY_NATIVE_PROP_GETTER \
		static risse_error Get(tRisseVariant *result, \
		iRisseDispatch2 *objthis) \
		{ return RISSE_E_ACCESSDENYED; }

#define RISSE_BEGIN_NATIVE_PROP_SETTER \
		static risse_error Set(const tRisseVariant *param, \
		iRisseDispatch2 *objthis) { \

#define RISSE_END_NATIVE_PROP_SETTER \
		}

#define RISSE_DENY_NATIVE_PROP_SETTER \
		static risse_error Set(const tRisseVariant *param, \
		iRisseDispatch2 *objthis) \
		{ return RISSE_E_ACCESSDENYED; }

#define RISSE_END_NATIVE_MEMBERS \
	}

#define RISSE_PARAM_EXIST(num) (numparams>(num) ? param[num]->Type()!=tvtVoid : false)



//---------------------------------------------------------------------------
// tRisseNativeFunction
//---------------------------------------------------------------------------
// base class used for native function ( for non-class-method )
class tRisseNativeFunction : public tRisseDispatch
{
	typedef tRisseDispatch inherited;
public:
	tRisseNativeFunction(const risse_char *name = NULL);
		// 'name' is just to be used as a label for debugging
	~tRisseNativeFunction();

	risse_error FuncCall(
		risse_uint32 flag, const risse_char * membername, risse_uint32 *hint, tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis);

	risse_error
	IsInstanceOf(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		const risse_char *classname,
		iRisseDispatch2 *objthis);

protected:
	risse_error virtual Process(tRisseVariant *result, risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *objthis) = 0;
		// override this instead of FuncCall
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif
