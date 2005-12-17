/*---------------------------------------------------------------------------*/
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/*! @brief TJS3 で使う型などの宣言                                           */
/*---------------------------------------------------------------------------*/

#ifndef __TJSTYPES_H__
#define __TJSTYPES_H__

#ifdef TJS_SUPPORT_WX
	#include <wx/wx.h>
#endif



#if defined(_WX_DEFS_H_)
/* Use wxWidgets definitions */
	#ifndef wxUSE_UNICODE
		#error "Currently wxWidgets must be configured with UNICODE support"
	#endif

	typedef wxInt8 tjs_int8;
	typedef wxUint8 tjs_uint8;
	typedef wxInt16 tjs_int16;
	typedef wxUint16 tjs_uint16;
	typedef wxInt32 tjs_int32;
	typedef wxUint32 tjs_uint32;
	typedef wxLongLong_t tjs_int64;
	typedef wxULongLong_t tjs_uint64;
	typedef size_t tjs_size;
	typedef ptrdiff_t tjs_offset;
	typedef int tjs_int;    /* at least 32bits */
	typedef unsigned int tjs_uint;    /* at least 32bits */

	#if SIZEOF_WCHAR_T == 4
		typedef wchar_t tjs_char; /* UTF-32 */
	#else
		typedef tjs_int32 tjs_char; /* whatever, UTF-32 */
	#endif

	typedef double tjs_real;

	#ifdef WORDS_BIGENDIAN
		#define TJS_HOST_IS_BIG_ENDIAN 1
		#define TJS_HOST_IS_LITTLE_ENDIAN 0
	#else
		#define TJS_HOST_IS_BIG_ENDIAN 0
		#define TJS_HOST_IS_LITTLE_ENDIAN 1
	#endif

#elif defined(WIN32)

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
	typedef ptrdiff_t tjs_offset;
	typedef int tjs_int;    /* at least 32bits */
	typedef unsigned int tjs_uint;    /* at least 32bits */

	typedef tjs_int32 tjs_char; /* whatever, UTF-32 */
	#ifndef SIZEOF_WCHAR_T
		#define SIZEOF_WCHAR_T 2
	#endif

	typedef double tjs_real;

	#define TJS_HOST_IS_BIG_ENDIAN 0
	#define TJS_HOST_IS_LITTLE_ENDIAN 1



#endif /* end of defined(_WIN32) && !defined(__GNUC__) */


#if defined(__VISUALC__)||defined(__BORLAND__)
	#define TJS_I64_VAL(x) ((tjs_int64)(x##i64))
	#define TJS_UI64_VAL(x) ((tjs_uint64)(x##i64))
#else
	#define TJS_I64_VAL(x)  ((tjs_int64)(x##LL))
	#define TJS_UI64_VAL(x) ((tjs_uint64)(x##LL))
#endif



#if SIZEOF_WCHAR_T == 2
	#define TJS_WCHAR_T_SIZE_IS_16BIT
#endif


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

#define TJS_IEEE_D_GET_SIGN(x)   ((bool)(x & TJS_IEEE_D_SIGN_MASK))
#define TJS_IEEE_D_GET_EXP(x)  ((tjs_int)(((x & TJS_IEEE_D_EXP_MASK) >> \
								TJS_IEEE_D_SIGNIFICAND_BITS) - TJS_IEEE_D_EXP_BIAS))
#define TJS_IEEE_D_GET_SIGNIFICAND(x) (x & TJS_IEEE_D_SIGNIFICAND_MASK)

/* component composition */
#define TJS_IEEE_D_MAKE_SIGN(x)  ((x)?TJS_UI64_VAL(0x8000000000000000):TJS_UI64_VAL(0))
#define TJS_IEEE_D_MAKE_EXP(x)   ((tjs_uint64)(x + TJS_IEEE_D_EXP_BIAS) << 52)
#define TJS_IEEE_D_MAKE_SIGNIFICAND(x) ((tjs_uint64)(x))

/* special expression */
 /* (quiet) NaN */
  #define TJS_IEEE_D_P_NaN (tjs_uint64)(TJS_IEEE_D_EXP_MASK|TJS_IEEE_D_SIGNIFICAND_MSB_MASK)
  #define TJS_IEEE_D_N_NaN (tjs_uint64)(TJS_IEEE_D_SIGN_MASK|TJS_IEEE_D_P_NaN)
 /* infinite */
  #define TJS_IEEE_D_P_INF (tjs_uint64)(TJS_IEEE_D_EXP_MASK)
  #define TJS_IEEE_D_N_INF (tjs_uint64)(TJS_IEEE_D_SIGN_MASK|TJS_IEEE_D_P_INF)

/* special expression check */
  #define TJS_IEEE_D_IS_NaN(x) ((TJS_IEEE_D_EXP_MASK & (x)) == TJS_IEEE_D_EXP_MASK) && \
				(((x) & TJS_IEEE_D_SIGNIFICAND_MSB_MASK) || \
				(!((x) & TJS_IEEE_D_SIGNIFICAND_MSB_MASK) && \
				((x) & (TJS_IEEE_D_SIGNIFICAND_MASK ^ TJS_IEEE_D_SIGNIFICAND_MSB_MASK))))
  #define TJS_IEEE_D_IS_INF(x) (((TJS_IEEE_D_EXP_MASK & (x)) == TJS_IEEE_D_EXP_MASK) && \
				(!((x) & TJS_IEEE_D_SIGNIFICAND_MASK)))


#endif


