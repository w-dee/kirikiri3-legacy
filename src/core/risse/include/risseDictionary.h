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

#ifndef risseDictionaryH
#define risseDictionaryH

#include "risseObject.h"
#include "risseNative.h"
#include "risseArray.h"

namespace Risse
{
//---------------------------------------------------------------------------
// tRisseDictionaryClass : Dictoinary Class
//---------------------------------------------------------------------------
class tRisseDictionaryClass : public tRisseNativeClass
{
	typedef tRisseNativeClass inherited;

public:
	tRisseDictionaryClass();
	~tRisseDictionaryClass();

protected:
	tRisseNativeInstance *CreateNativeInstance();
	iRisseDispatch2 *CreateBaseRisseObject();

	static risse_uint32 ClassID;
private:

protected:
};
//---------------------------------------------------------------------------
// tRisseDictionaryNI : Risse Dictionary Native C++ instance
//---------------------------------------------------------------------------
class tRisseDictionaryNI : public tRisseNativeInstance,
							public tRisseSaveStructuredDataCallback
{
	typedef tRisseNativeInstance inherited;

	tRisseCustomObject * Owner;
public:

	tRisseDictionaryNI();
	~tRisseDictionaryNI();

	risse_error Construct(risse_int numparams, tRisseVariant **param,
		iRisseDispatch2 *obj);

private:
	void Invalidate(); // Invalidate override

public:
	bool IsValid() const { return Owner != NULL; } // check validation

	void Assign(iRisseDispatch2 *dsp, bool clear = true);

	void Clear();

private:
	struct tAssignCallback : public tRisseDispatch
	{
		tRisseCustomObject * Owner;
		risse_error
		FuncCall(risse_uint32 flag, const risse_char * membername,
			risse_uint32 *hint, tRisseVariant *result, risse_int numparams,
			tRisseVariant **param, iRisseDispatch2 *objthis);
			// method from iRisseDispatch2, for enumeration callback
	};
	friend class tSaveStructCallback;

public:
	void SaveStructuredData(std::vector<iRisseDispatch2 *> &stack,
		tRisseStringAppender & string, const ttstr&indentstr);
		// method from tRisseSaveStructuredDataCallback
private:
	struct tSaveStructCallback : public tRisseDispatch
	{
		std::vector<iRisseDispatch2 *> * Stack;
		tRisseStringAppender * String;
		const ttstr * IndentStr;
		bool First;

		risse_error
		FuncCall(risse_uint32 flag, const risse_char * membername,
			risse_uint32 *hint, tRisseVariant *result, risse_int numparams,
			tRisseVariant **param, iRisseDispatch2 *objthis);
	};
	friend class tSaveStructCallback;

public:
	void AssignStructure(iRisseDispatch2 * dsp, std::vector<iRisseDispatch2 *> &stack);

	struct tAssignStructCallback : public tRisseDispatch
	{
		std::vector<iRisseDispatch2 *> * Stack;
		iRisseDispatch2 * Dest;

		risse_error
		FuncCall(risse_uint32 flag, const risse_char * membername,
			risse_uint32 *hint, tRisseVariant *result, risse_int numparams,
			tRisseVariant **param, iRisseDispatch2 *objthis);
	};
	friend class tAssignStructCallback;
};
//---------------------------------------------------------------------------
class tRisseDictionaryObject : public tRisseCustomObject
{
	typedef tRisseCustomObject inherited;

public:
	tRisseDictionaryObject();
	~tRisseDictionaryObject();


	risse_error
	FuncCall(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis);


	risse_error
	PropGet(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
		iRisseDispatch2 *objthis);

	risse_error
	CreateNew(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		iRisseDispatch2 **result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis);


	risse_error
	Operation(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		tRisseVariant *result,
		const tRisseVariant *param, iRisseDispatch2 *objthis);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// RisseGetDictionaryClassID
//---------------------------------------------------------------------------
extern risse_int32 RisseGetDictionaryClassID();
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// RisseCreateDictionaryObject
//---------------------------------------------------------------------------
iRisseDispatch2 *RisseCreateDictionaryObject(iRisseDispatch2 **classout = NULL);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif
