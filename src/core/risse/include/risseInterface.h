//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief iRisseDispatch2 (オブジェクトやクラス、関数などのインターフェース) 定義
//---------------------------------------------------------------------------
#ifndef risseInterfaceH
#define risseInterfaceH

#include "risseConfig.h"
#include "risseErrorDefs.h"

namespace Risse
{
//---------------------------------------------------------------------------

/*[*/
//---------------------------------------------------------------------------
// call flag type
//---------------------------------------------------------------------------
#define RISSE_MEMBERENSURE		0x00000200 // create a member if not exists
#define RISSE_MEMBERMUSTEXIST     0x00000400 // member *must* exist ( for Dictionary/Array )
#define RISSE_IGNOREPROP			0x00000800 // ignore property invoking
#define RISSE_HIDDENMEMBER		0x00001000 // member is hidden
#define RISSE_STATICMEMBER		0x00010000 // member is not registered to the
										   // object (internal use)

#define RISSE_ENUM_NO_VALUE		0x00100000 // values are not retrieved
										   // (for EnumMembers)

#define RISSE_NIS_REGISTER		0x00000001 // set native pointer
#define RISSE_NIS_GETINSTANCE		0x00000002 // get native pointer

#define RISSE_CII_ADD				0x00000001 // register name
										   // 'num' argument passed to CII is to be igonored.
#define RISSE_CII_GET				0x00000000 // retrieve name

#define RISSE_CII_SET_FINALIZE	0x00000002 // register "finalize" method name
										   // (set empty string not to call the method)
										   // 'num' argument passed to CII is to be igonored.
#define RISSE_CII_SET_MISSING		0x00000003 // register "missing" method name.
										   // the method is called when the member is not present.
										   // (set empty string not to call the method)
										   // 'num' argument passed to CII is to be igonored.
										   // the method is to be called with three arguments;
										   // get_or_set    : false for get, true for set
										   // name          : member name
										   // value         : value property; you must
										   //               : dereference using unary '*' operator.
										   // the method must return true for found, false for not-found.

#define RISSE_OL_LOCK				0x00000001 // Lock the object
#define RISSE_OL_UNLOCK			0x00000002 // Unlock the object



//---------------------------------------------------------------------------
// 	Operation  flag
//---------------------------------------------------------------------------

#define RISSE_OP_BAND				0x0001
#define RISSE_OP_BOR				0x0002
#define RISSE_OP_BXOR				0x0003
#define RISSE_OP_SUB				0x0004
#define RISSE_OP_ADD				0x0005
#define RISSE_OP_MOD				0x0006
#define RISSE_OP_DIV				0x0007
#define RISSE_OP_IDIV				0x0008
#define RISSE_OP_MUL				0x0009
#define RISSE_OP_LOR				0x000a
#define RISSE_OP_LAND				0x000b
#define RISSE_OP_SAR				0x000c
#define RISSE_OP_SAL				0x000d
#define RISSE_OP_SR				0x000e
#define RISSE_OP_INC				0x000f
#define RISSE_OP_DEC				0x0010

#define RISSE_OP_MASK				0x001f

#define RISSE_OP_MIN				RISSE_OP_BAND
#define RISSE_OP_MAX				RISSE_OP_DEC

/* SAR = Shift Arithmetic Right, SR = Shift (bitwise) Right */



//---------------------------------------------------------------------------
// iRisseDispatch
//---------------------------------------------------------------------------
/*
	iRisseDispatch interface
*/
class tRisseVariant;
class tRisseVariantClosure;
class tRisseVariantString;
class iRisseNativeInstance;
class iRisseDispatch2
{
/*
	methods, that have "ByNum" at the end of the name, have
	"num" parameter that enables the function to call a member with number directly.
	following two have the same effect:
	FuncCall(NULL, L"123", NULL, 0, NULL, NULL);
	FuncCallByNum(NULL, 123, NULL, 0, NULL, NULL);
*/

public:
	virtual risse_uint AddRef(void) = 0;
	virtual risse_uint Release(void) = 0;

public:

	virtual risse_error
	FuncCall( // function invocation
		risse_uint32 flag,			// calling flag
		const risse_char * membername,// member name ( NULL for a default member )
		risse_uint32 *hint,			// hint for the member name (in/out)
		tRisseVariant *result,		// result
		risse_int numparams,			// number of parameters
		tRisseVariant **param,		// parameters
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	FuncCallByNum( // function invocation by index number
		risse_uint32 flag,			// calling flag
		risse_int num,				// index number
		tRisseVariant *result,		// result
		risse_int numparams,			// number of parameters
		tRisseVariant **param,		// parameters
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	PropGet( // property get
		risse_uint32 flag,			// calling flag
		const risse_char * membername,// member name ( NULL for a default member )
		risse_uint32 *hint,			// hint for the member name (in/out)
		tRisseVariant *result,		// result
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	PropGetByNum( // property get by index number
		risse_uint32 flag,			// calling flag
		risse_int num,				// index number
		tRisseVariant *result,		// result
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	PropSet( // property set
		risse_uint32 flag,			// calling flag
		const risse_char *membername,	// member name ( NULL for a default member )
		risse_uint32 *hint,			// hint for the member name (in/out)
		const tRisseVariant *param,	// parameters
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	PropSetByNum( // property set by index number
		risse_uint32 flag,			// calling flag
		risse_int num,				// index number
		const tRisseVariant *param,	// parameters
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	GetCount( // get member count
		risse_int *result,         	// variable that receives the result
		const risse_char *membername,	// member name ( NULL for a default member )
		risse_uint32 *hint,			// hint for the member name (in/out)
		iRisseDispatch2 *objthis      // object as "this"
		) = 0;

	virtual risse_error
	GetCountByNum( // get member count by index number
		risse_int *result,			// variable that receives the result
		risse_int num,				// by index number
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	PropSetByVS( // property set by tRisseVariantString, for internal use
		risse_uint32 flag,			// calling flag
		tRisseVariantString *membername, // member name ( NULL for a default member )
		const tRisseVariant *param,	// parameters
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	EnumMembers( // enumerate members
		risse_uint32 flag,			// enumeration flag
		tRisseVariantClosure *callback,	// callback function interface ( called on each member )
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	DeleteMember( // delete member
		risse_uint32 flag,			// calling flag
		const risse_char *membername,	// member name ( NULL for a default member )
		risse_uint32 *hint,			// hint for the member name (in/out)
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	DeleteMemberByNum( // delete member by index number
		risse_uint32 flag,			// calling flag
		risse_int num,				// index number
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	Invalidate( // invalidation
		risse_uint32 flag,			// calling flag
		const risse_char *membername,	// member name ( NULL for a default member )
		risse_uint32 *hint,			// hint for the member name (in/out)
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	InvalidateByNum( // invalidation by index number
		risse_uint32 flag,			// calling flag
		risse_int num,				// index number
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	IsValid( // get validation, returns RISSE_S_TRUE (valid) or RISSE_S_FALSE (invalid)
		risse_uint32 flag,			// calling flag
		const risse_char *membername,	// member name ( NULL for a default member )
		risse_uint32 *hint,			// hint for the member name (in/out)
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	IsValidByNum( // get validation by index number, returns RISSE_S_TRUE (valid) or RISSE_S_FALSE (invalid)
		risse_uint32 flag,			// calling flag
		risse_int num,				// index number
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	CreateNew( // create new object
		risse_uint32 flag,			// calling flag
		const risse_char * membername,// member name ( NULL for a default member )
		risse_uint32 *hint,			// hint for the member name (in/out)
		iRisseDispatch2 **result,		// result
		risse_int numparams,			// number of parameters
		tRisseVariant **param,		// parameters
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	CreateNewByNum( // create new object by index number
		risse_uint32 flag,			// calling flag
		risse_int num,				// index number
		iRisseDispatch2 **result,		// result
		risse_int numparams,			// number of parameters
		tRisseVariant **param,		// parameters
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	Reserved1(
		) = 0;

	virtual risse_error
	IsInstanceOf( // class instance matching returns RISSE_S_FALSE or RISSE_S_TRUE
		risse_uint32 flag,			// calling flag
		const risse_char *membername,	// member name ( NULL for a default member )
		risse_uint32 *hint,			// hint for the member name (in/out)
		const risse_char *classname,	// class name to inquire
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	IsInstanceOfByNum( // class instance matching by index number
		risse_uint32 flag,			// calling flag
		risse_int num,					// index number
		const risse_char *classname,	// class name to inquire
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	Operation( // operation with member
		risse_uint32 flag,			// calling flag
		const risse_char *membername,	// member name ( NULL for a default member )
		risse_uint32 *hint,			// hint for the member name (in/out)
		tRisseVariant *result,		// result ( can be NULL )
		const tRisseVariant *param,	// parameters
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	OperationByNum( // operation with member by index number
		risse_uint32 flag,			// calling flag
		risse_int num,				// index number
		tRisseVariant *result,		// result ( can be NULL )
		const tRisseVariant *param,	// parameters
		iRisseDispatch2 *objthis		// object as "this"
		) = 0;

	virtual risse_error
	NativeInstanceSupport( // support for native instance
		risse_uint32 flag,			// calling flag
		risse_int32 classid,			// native class ID
		iRisseNativeInstance **pointer// object pointer
		) = 0;

	virtual risse_error
	ClassInstanceInfo( // support for class instance information
		risse_uint32 flag,			// calling flag
		risse_uint num,				// index number
		tRisseVariant *value			// the name
		) = 0;

	virtual risse_error
	Reserved2(
		) = 0;

	virtual risse_error
	Reserved3(
		) = 0;


};
//---------------------------------------------------------------------------
class iRisseNativeInstance
{
public:
	virtual risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj) = 0;
		// risse constructor
	virtual void Invalidate() = 0;
		// called before destruction
	virtual void Destruct() = 0;
		// must destruct itself
};
/*]*/
//---------------------------------------------------------------------------




} // namespace Risse

#endif
