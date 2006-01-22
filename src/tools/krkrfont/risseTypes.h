/*---------------------------------------------------------------------------*/
/*
	Risse2 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/* "Risse2" type definitions                                                   */
/*---------------------------------------------------------------------------*/

#ifndef __RisseTYPES_H__
#define __RisseTYPES_H__





/* Functions that needs to be exported ( for non-class-member functions ) */
/* This should only be applyed for function declaration in headers ( not body ) */
#define RISSE_EXP_FUNC_DEF(rettype, name, arg) extern rettype name arg


/* Functions that needs to be exported ( for class-member functions ) */
#define RISSE_METHOD_DEF(rettype, name, arg) rettype name arg
#define RISSE_CONST_METHOD_DEF(rettype, name, arg) rettype name arg const
#define RISSE_STATIC_METHOD_DEF(rettype, name, arg) static rettype name arg
#define RISSE_STATIC_CONST_METHOD_DEF(rettype, name, arg) static rettype name arg const
#define RISSE_METHOD_RET_EMPTY
#define RISSE_METHOD_RET(type)


/*[*/
#if defined(_WX_DEFS_H_)
/* Use wxWidgets definitions */

	typedef wxInt8 risse_int8;
	typedef wxUint8 risse_uint8;
	typedef wxInt16 risse_int16;
	typedef wxUint16 risse_uint16;
	typedef wxInt32 risse_int32;
	typedef wxUint32 risse_uint32;
	typedef wxLongLong_t risse_int64;
	typedef wxULongLong_t risse_uint64;
	typedef size_t risse_size;
	typedef ssize_t risse_ptrdiff;
	typedef wxInt32 risse_int;    /* at least 32bits */
	typedef wxUint32 risse_uint;    /* at least 32bits */

	typedef risse_int32 risse_char; /* whatever, UTF-32 */

	typedef char risse_nchar;
	typedef double risse_real;

	typedef int risse_int;
	typedef unsigned int risse_uint;

	#define RISSE_I64_VAL(x) (static_cast<risse_int64>(x##LL))
	#define RISSE_UI64_VAL(x) (static_cast<risse_uint64>(x##LL))

	#ifdef WORDS_BIGENDIAN
		#define RISSE_HOST_IS_BIG_ENDIAN 1
		#define RISSE_HOST_IS_LITTLE_ENDIAN 0
	#else
		#define RISSE_HOST_IS_BIG_ENDIAN 0
		#define RISSE_HOST_IS_LITTLE_ENDIAN 1
	#endif

	#define RISSE_INTF_METHOD
	#define RISSE_USERENTRY


#elif defined(_WIN32)

/* VC++/BCC */

	typedef __int8 risse_int8;
	typedef unsigned __int8 risse_uint8;
	typedef __int16 risse_int16;
	typedef unsigned __int16 risse_uint16;
	typedef __int32 risse_int32;
	typedef unsigned __int32 risse_uint32;
	typedef __int64 risse_int64;
	typedef unsigned __int64 risse_uint64;
	typedef size_t risse_size;
	typedef ptrdiff_t risse_ptrdiff;
	typedef int risse_int;    /* at least 32bits */
	typedef unsigned int risse_uint;    /* at least 32bits */

	typedef risse_int32 risse_char; /* whatever, UTF-32 */

	typedef char risse_nchar;
	typedef double risse_real;

	#define RISSE_HOST_IS_BIG_ENDIAN 0
	#define RISSE_HOST_IS_LITTLE_ENDIAN 1

	#ifndef RISSE_INTF_METHOD
		#define RISSE_INTF_METHOD __cdecl
			/* RISSE_INTF_METHOD is "cdecl" (by default)
				since Risse2 2.4.14 (kirikir2 2.25 beta 1) */
	#endif

	#define RISSE_USERENTRY __cdecl

	#define RISSE_I64_VAL(x) (static_cast<risse_int64>(x##i64))
	#define RISSE_UI64_VAL(x) (static_cast<risse_uint64>(x##i64))


#endif /* end of defined(_WIN32) && !defined(__GNUC__) */

/*[*/
#define RISSE_W(X) L##X
#define RISSE_N(X) X


typedef risse_int32 risse_error;

typedef risse_int64 tTVInteger;
typedef risse_real tTVReal;


/* IEEE double manipulation support
 (Risse requires IEEE double(64-bit float) native support on machine or C++ compiler) */

/*

63 62       52 51                         0
+-+-----------+---------------------------+
|s|    exp    |         significand       |
+-+-----------+---------------------------+

s = sign,  negative if this is 1, otherwise positive.



*/

/* double related constants */
#define RISSE_IEEE_D_EXP_MAX 1023
#define RISSE_IEEE_D_EXP_MIN -1022
#define RISSE_IEEE_D_SIGNIFICAND_BITS 52

#define RISSE_IEEE_D_EXP_BIAS 1023

/* component extraction */
#define RISSE_IEEE_D_SIGN_MASK              (RISSE_UI64_VAL(0x8000000000000000))
#define RISSE_IEEE_D_EXP_MASK               (RISSE_UI64_VAL(0x7ff0000000000000))
#define RISSE_IEEE_D_SIGNIFICAND_MASK       (RISSE_UI64_VAL(0x000fffffffffffff))
#define RISSE_IEEE_D_SIGNIFICAND_MSB_MASK   (RISSE_UI64_VAL(0x0008000000000000))

#define RISSE_IEEE_D_GET_SIGN(x)   (static_cast<bool>(x & RISSE_IEEE_D_SIGN_MASK))
#define RISSE_IEEE_D_GET_EXP(x)  (static_cast<risse_int>(((x & RISSE_IEEE_D_EXP_MASK) >> \
								RISSE_IEEE_D_SIGNIFICAND_BITS) - RISSE_IEEE_D_EXP_BIAS))
#define RISSE_IEEE_D_GET_SIGNIFICAND(x) (x & RISSE_IEEE_D_SIGNIFICAND_MASK)

/* component composition */
#define RISSE_IEEE_D_MAKE_SIGN(x)  ((x)?RISSE_UI64_VAL(0x8000000000000000):RISSE_UI64_VAL(0))
#define RISSE_IEEE_D_MAKE_EXP(x)   (static_cast<risse_uint64>(x + RISSE_IEEE_D_EXP_BIAS) << 52)
#define RISSE_IEEE_D_MAKE_SIGNIFICAND(x) (static_cast<risse_uint64>(x))

/* special expression */
 /* (quiet) NaN */
  #define RISSE_IEEE_D_P_NaN static_cast<risse_uint64>(RISSE_IEEE_D_EXP_MASK|RISSE_IEEE_D_SIGNIFICAND_MSB_MASK)
  #define RISSE_IEEE_D_N_NaN static_cast<risse_uint64>(RISSE_IEEE_D_SIGN_MASK|RISSE_IEEE_D_P_NaN)
 /* infinite */
  #define RISSE_IEEE_D_P_INF static_cast<risse_uint64>(RISSE_IEEE_D_EXP_MASK)
  #define RISSE_IEEE_D_N_INF static_cast<risse_uint64>(RISSE_IEEE_D_SIGN_MASK|RISSE_IEEE_D_P_INF)

/* special expression check */
  #define RISSE_IEEE_D_IS_NaN(x) ((RISSE_IEEE_D_EXP_MASK & (x)) == RISSE_IEEE_D_EXP_MASK) && \
				(((x) & RISSE_IEEE_D_SIGNIFICAND_MSB_MASK) || \
				(!((x) & RISSE_IEEE_D_SIGNIFICAND_MSB_MASK) && \
				((x) & (RISSE_IEEE_D_SIGNIFICAND_MASK ^ RISSE_IEEE_D_SIGNIFICAND_MSB_MASK))))
  #define RISSE_IEEE_D_IS_INF(x) (((RISSE_IEEE_D_EXP_MASK & (x)) == RISSE_IEEE_D_EXP_MASK) && \
				(!((x) & RISSE_IEEE_D_SIGNIFICAND_MASK)))

/*]*/

#endif
