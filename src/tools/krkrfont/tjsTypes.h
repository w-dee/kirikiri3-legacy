/*---------------------------------------------------------------------------*/
/*
	TJS2 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/* "TJS2" type definitions                                                   */
/*---------------------------------------------------------------------------*/

#ifndef __TJSTYPES_H__
#define __TJSTYPES_H__





/* Functions that needs to be exported ( for non-class-member functions ) */
/* This should only be applyed for function declaration in headers ( not body ) */
#define TJS_EXP_FUNC_DEF(rettype, name, arg) extern rettype name arg


/* Functions that needs to be exported ( for class-member functions ) */
#define TJS_METHOD_DEF(rettype, name, arg) rettype name arg
#define TJS_CONST_METHOD_DEF(rettype, name, arg) rettype name arg const
#define TJS_STATIC_METHOD_DEF(rettype, name, arg) static rettype name arg
#define TJS_STATIC_CONST_METHOD_DEF(rettype, name, arg) static rettype name arg const
#define TJS_METHOD_RET_EMPTY
#define TJS_METHOD_RET(type)


/*[*/
#if defined(_WX_DEFS_H_)
/* Use wxWidgets definitions */

	typedef wxInt8 tjs_int8;
	typedef wxUint8 tjs_uint8;
	typedef wxInt16 tjs_int16;
	typedef wxUint16 tjs_uint16;
	typedef wxInt32 tjs_int32;
	typedef wxUint32 tjs_uint32;
	typedef wxLongLong_t tjs_int64;
	typedef wxULongLong_t tjs_uint64;
	typedef size_t tjs_size;
	typedef ssize_t tjs_ptrdiff;
	typedef wxInt32 tjs_int;    /* at least 32bits */
	typedef wxUint32 tjs_uint;    /* at least 32bits */

	typedef tjs_int32 tjs_char; /* whatever, UTF-32 */

	typedef char tjs_nchar;
	typedef double tjs_real;

	typedef int tjs_int;
	typedef unsigned int tjs_uint;

	#define TJS_I64_VAL(x) (static_cast<tjs_int64>(x##LL))
	#define TJS_UI64_VAL(x) (static_cast<tjs_uint64>(x##LL))

	#ifdef WORDS_BIGENDIAN
		#define TJS_HOST_IS_BIG_ENDIAN 1
		#define TJS_HOST_IS_LITTLE_ENDIAN 0
	#else
		#define TJS_HOST_IS_BIG_ENDIAN 0
		#define TJS_HOST_IS_LITTLE_ENDIAN 1
	#endif

	#define TJS_INTF_METHOD
	#define TJS_USERENTRY


#elif defined(_WIN32)

/* VC++/BCC */

	typedef __int8 tjs_int8;
	typedef unsigned __int8 tjs_uint8;
	typedef __int16 tjs_int16;
	typedef unsigned __int16 tjs_uint16;
	typedef __int32 tjs_int32;
	typedef unsigned __int32 tjs_uint32;
	typedef __int64 tjs_int64;
	typedef unsigned __int64 tjs_uint64;
	typedef size_t tjs_size;
	typedef ptrdiff_t tjs_ptrdiff;
	typedef int tjs_int;    /* at least 32bits */
	typedef unsigned int tjs_uint;    /* at least 32bits */

	typedef tjs_int32 tjs_char; /* whatever, UTF-32 */

	typedef char tjs_nchar;
	typedef double tjs_real;

	#define TJS_HOST_IS_BIG_ENDIAN 0
	#define TJS_HOST_IS_LITTLE_ENDIAN 1

	#ifndef TJS_INTF_METHOD
		#define TJS_INTF_METHOD __cdecl
			/* TJS_INTF_METHOD is "cdecl" (by default)
				since TJS2 2.4.14 (kirikir2 2.25 beta 1) */
	#endif

	#define TJS_USERENTRY __cdecl

	#define TJS_I64_VAL(x) (static_cast<tjs_int64>(x##i64))
	#define TJS_UI64_VAL(x) (static_cast<tjs_uint64>(x##i64))


#endif /* end of defined(_WIN32) && !defined(__GNUC__) */

/*[*/
#define TJS_W(X) L##X
#define TJS_N(X) X


typedef tjs_int32 tjs_error;

typedef tjs_int64 tTVInteger;
typedef tjs_real tTVReal;


/* IEEE double manipulation support
 (TJS requires IEEE double(64-bit float) native support on machine or C++ compiler) */

/*

63 62       52 51                         0
+-+-----------+---------------------------+
|s|    exp    |         significand       |
+-+-----------+---------------------------+

s = sign,  negative if this is 1, otherwise positive.



*/

/* double related constants */
#define TJS_IEEE_D_EXP_MAX 1023
#define TJS_IEEE_D_EXP_MIN -1022
#define TJS_IEEE_D_SIGNIFICAND_BITS 52

#define TJS_IEEE_D_EXP_BIAS 1023

/* component extraction */
#define TJS_IEEE_D_SIGN_MASK              (TJS_UI64_VAL(0x8000000000000000))
#define TJS_IEEE_D_EXP_MASK               (TJS_UI64_VAL(0x7ff0000000000000))
#define TJS_IEEE_D_SIGNIFICAND_MASK       (TJS_UI64_VAL(0x000fffffffffffff))
#define TJS_IEEE_D_SIGNIFICAND_MSB_MASK   (TJS_UI64_VAL(0x0008000000000000))

#define TJS_IEEE_D_GET_SIGN(x)   (static_cast<bool>(x & TJS_IEEE_D_SIGN_MASK))
#define TJS_IEEE_D_GET_EXP(x)  (static_cast<tjs_int>(((x & TJS_IEEE_D_EXP_MASK) >> \
								TJS_IEEE_D_SIGNIFICAND_BITS) - TJS_IEEE_D_EXP_BIAS))
#define TJS_IEEE_D_GET_SIGNIFICAND(x) (x & TJS_IEEE_D_SIGNIFICAND_MASK)

/* component composition */
#define TJS_IEEE_D_MAKE_SIGN(x)  ((x)?TJS_UI64_VAL(0x8000000000000000):TJS_UI64_VAL(0))
#define TJS_IEEE_D_MAKE_EXP(x)   (static_cast<tjs_uint64>(x + TJS_IEEE_D_EXP_BIAS) << 52)
#define TJS_IEEE_D_MAKE_SIGNIFICAND(x) (static_cast<tjs_uint64>(x))

/* special expression */
 /* (quiet) NaN */
  #define TJS_IEEE_D_P_NaN static_cast<tjs_uint64>(TJS_IEEE_D_EXP_MASK|TJS_IEEE_D_SIGNIFICAND_MSB_MASK)
  #define TJS_IEEE_D_N_NaN static_cast<tjs_uint64>(TJS_IEEE_D_SIGN_MASK|TJS_IEEE_D_P_NaN)
 /* infinite */
  #define TJS_IEEE_D_P_INF static_cast<tjs_uint64>(TJS_IEEE_D_EXP_MASK)
  #define TJS_IEEE_D_N_INF static_cast<tjs_uint64>(TJS_IEEE_D_SIGN_MASK|TJS_IEEE_D_P_INF)

/* special expression check */
  #define TJS_IEEE_D_IS_NaN(x) ((TJS_IEEE_D_EXP_MASK & (x)) == TJS_IEEE_D_EXP_MASK) && \
				(((x) & TJS_IEEE_D_SIGNIFICAND_MSB_MASK) || \
				(!((x) & TJS_IEEE_D_SIGNIFICAND_MSB_MASK) && \
				((x) & (TJS_IEEE_D_SIGNIFICAND_MASK ^ TJS_IEEE_D_SIGNIFICAND_MSB_MASK))))
  #define TJS_IEEE_D_IS_INF(x) (((TJS_IEEE_D_EXP_MASK & (x)) == TJS_IEEE_D_EXP_MASK) && \
				(!((x) & TJS_IEEE_D_SIGNIFICAND_MASK)))

/*]*/

#endif
