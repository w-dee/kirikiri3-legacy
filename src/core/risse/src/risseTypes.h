/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/*! @file                                                                    */
/*! @brief Risse で使う型などの宣言                                          */
/*---------------------------------------------------------------------------*/

#ifndef __TYPES_H__
#define __TYPES_H__

#ifdef RISSE_SUPPORT_WX
	#include <wx/defs.h>
	#include <wx/string.h>
	#include <wx/intl.h>
#endif


#include <limits>


namespace Risse
{

#if defined(_WX_DEFS_H_)
/* Use wxWidgets definitions */
	#ifndef wxUSE_UNICODE
		#error "Currently wxWidgets must be configured with UNICODE support"
	#endif

	typedef wxInt8 risse_int8;
	typedef wxUint8 risse_uint8;
	typedef wxInt16 risse_int16;
	typedef wxUint16 risse_uint16;
	typedef wxInt32 risse_int32;
	typedef wxUint32 risse_uint32;
	typedef wxLongLong_t risse_int64;
	typedef wxULongLong_t risse_uint64;
	typedef size_t risse_size;
	typedef ptrdiff_t risse_offset;
	typedef int risse_int;    /* at least 32bits */
	typedef unsigned int risse_uint;    /* at least 32bits */

	#if SIZEOF_WCHAR_T == 4
		typedef wchar_t risse_char; /* UTF-32 */
	#else
		typedef risse_int32 risse_char; /* whatever, UTF-32 */
	#endif

	typedef double risse_real;

	#ifdef WORDS_BIGENDIAN
		#define RISSE_HOST_IS_BIG_ENDIAN 1
		#define RISSE_HOST_IS_LITTLE_ENDIAN 0
	#else
		#define RISSE_HOST_IS_BIG_ENDIAN 0
		#define RISSE_HOST_IS_LITTLE_ENDIAN 1
	#endif

#elif defined(WIN32)

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
	typedef ptrdiff_t risse_offset;
	typedef int risse_int;    /* at least 32bits */
	typedef unsigned int risse_uint;    /* at least 32bits */

	typedef risse_int32 risse_char; /* whatever, UTF-32 */
	#ifndef SIZEOF_WCHAR_T
		#define SIZEOF_WCHAR_T 2
	#endif

	typedef double risse_real;

	#define RISSE_HOST_IS_BIG_ENDIAN 0
	#define RISSE_HOST_IS_LITTLE_ENDIAN 1



#endif /* end of defined(_WIN32) && !defined(__GNUC__) */


#if defined(__VISUALC__)||defined(__BORLAND__)
	#define RISSE_I64_VAL(x) ((risse_int64)(x##i64))
	#define RISSE_UI64_VAL(x) ((risse_uint64)(x##i64))
#else
	#define RISSE_I64_VAL(x)  ((risse_int64)(x##LL))
	#define RISSE_UI64_VAL(x) ((risse_uint64)(x##LL))
#endif



#if SIZEOF_WCHAR_T == 2
	#define RISSE_WCHAR_T_SIZE_IS_16BIT
#endif

// endian check
#ifdef RISSE_HOST_IS_BIG_ENDIAN
#elif  RISSE_HOST_IS_LITTLE_ENDIAN
#else
	#error "unknown host endian"
#endif


// noinline
#ifdef __GNUC__
 // GNU C 3.1 or later has this no-inline attribute specifier
 #if (__GNUC__ == 3 && __GNUC_MINOR__ > 0 ) || __GNUC__ > 3
  #define RISSE_NOINLINE __attribute__ ((noinline))
 #endif
#endif

#ifndef RISSE_NOINLINE
 #define RISSE_NOINLINE
#endif




//! @brief risse_size の最大値
// std::numeric_limits<risse_size>::max() を使おうかと思ったがstatic constではない
// 可能性がある
// char サイズが 8bit なのは下にある sanity checking で保証する
static const risse_size risse_size_max = ((((1UL << ((sizeof(risse_size) * 8) -1))-1)<<1)+1);

//! @brief risse_uint64 の最大値
static const risse_uint64 risse_uint64_max = ((((RISSE_UI64_VAL(1) << ((sizeof(risse_uint64) * 8) -1))-1)<<1)+1);

typedef risse_int32 risse_error;

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

#define RISSE_IEEE_D_GET_SIGN(x)   ((bool)(x & RISSE_IEEE_D_SIGN_MASK))
#define RISSE_IEEE_D_GET_EXP(x)  ((risse_int)(((x & RISSE_IEEE_D_EXP_MASK) >> \
								RISSE_IEEE_D_SIGNIFICAND_BITS) - RISSE_IEEE_D_EXP_BIAS))
#define RISSE_IEEE_D_GET_SIGNIFICAND(x) (x & RISSE_IEEE_D_SIGNIFICAND_MASK)

/* component composition */
#define RISSE_IEEE_D_MAKE_SIGN(x)  ((x)?RISSE_UI64_VAL(0x8000000000000000):RISSE_UI64_VAL(0))
#define RISSE_IEEE_D_MAKE_EXP(x)   ((risse_uint64)(x + RISSE_IEEE_D_EXP_BIAS) << 52)
#define RISSE_IEEE_D_MAKE_SIGNIFICAND(x) ((risse_uint64)(x))

/* special expression */
 /* (quiet) NaN */
  #define RISSE_IEEE_D_P_NaN (risse_uint64)(RISSE_IEEE_D_EXP_MASK|RISSE_IEEE_D_SIGNIFICAND_MSB_MASK)
  #define RISSE_IEEE_D_N_NaN (risse_uint64)(RISSE_IEEE_D_SIGN_MASK|RISSE_IEEE_D_P_NaN)
 /* infinite */
  #define RISSE_IEEE_D_P_INF (risse_uint64)(RISSE_IEEE_D_EXP_MASK)
  #define RISSE_IEEE_D_N_INF (risse_uint64)(RISSE_IEEE_D_SIGN_MASK|RISSE_IEEE_D_P_INF)

/* special expression check */
  #define RISSE_IEEE_D_IS_NaN(x) ((RISSE_IEEE_D_EXP_MASK & (x)) == RISSE_IEEE_D_EXP_MASK) && \
				(((x) & RISSE_IEEE_D_SIGNIFICAND_MSB_MASK) || \
				(!((x) & RISSE_IEEE_D_SIGNIFICAND_MSB_MASK) && \
				((x) & (RISSE_IEEE_D_SIGNIFICAND_MASK ^ RISSE_IEEE_D_SIGNIFICAND_MSB_MASK))))
  #define RISSE_IEEE_D_IS_INF(x) (((RISSE_IEEE_D_EXP_MASK & (x)) == RISSE_IEEE_D_EXP_MASK) && \
				(!((x) & RISSE_IEEE_D_SIGNIFICAND_MASK)))






//---------------------------------------------------------------------------
// floating-point class checker
//---------------------------------------------------------------------------
#define RISSE_FC_CLASS_MASK 7 //!< クラスマスク
#define RISSE_FC_SIGN_MASK 8 //!< サインマスク

#define RISSE_FC_CLASS_NORMAL 0 //!< 普通の数
#define RISSE_FC_CLASS_NAN 1 //!< Not a Number (非数)
#define RISSE_FC_CLASS_INF 2 //!< Infinity (無限大)

#define RISSE_FC_IS_NORMAL(x)  (((x)&RISSE_FC_CLASS_MASK) == RISSE_FC_CLASS_NORMAL) //!< 普通の数？
#define RISSE_FC_IS_NAN(x)  (((x)&RISSE_FC_CLASS_MASK) == RISSE_FC_CLASS_NAN) //!< 非数?
#define RISSE_FC_IS_INF(x)  (((x)&RISSE_FC_CLASS_MASK) == RISSE_FC_CLASS_INF) //!< 無限大?

#define RISSE_FC_IS_NEGATIVE(x) ((bool)((x) & RISSE_FC_SIGN_MASK)) //!< 負?
#define RISSE_FC_IS_POSITIVE(x) (!RISSE_FC_IS_NEGATIVE(x)) //!< 正?


//! @brief	浮動小数点数のクラスを得る
//! @param	r	調べたい値
//! @return	クラス
risse_uint32 GetFPClass(risse_real r);


//! @brief		正の quiet 型 NaN の値を得る
//! @return		正の quiet 型 NaN
static inline risse_real GetNaN()
{
	risse_uint64 tmp = RISSE_IEEE_D_P_NaN;
	return *reinterpret_cast<risse_real*>(&tmp);
}

//! @brief		正の無限大 の値を得る
//! @return		正の無限大
static inline risse_real GetInf()
{
	risse_uint64 tmp = RISSE_IEEE_D_P_INF;
	return *reinterpret_cast<risse_real*>(&tmp);
}

template <int size>
struct tPointerSizedInteger
{
	typedef long type;
	typedef unsigned long utype;
};
template <>
struct tPointerSizedInteger<8>
{
	typedef risse_int64 type;
	typedef risse_uint64 utype;
};
template <>
struct tPointerSizedInteger<4>
{
	typedef risse_int32 type;
	typedef risse_uint32 utype;
};

//! @brief risse_ptrint は、ポインタと同じサイズを持つことが保証されている符号付き整数型
typedef tPointerSizedInteger<sizeof(void*)>::type risse_ptrint;
//! @brief risse_ptruint は、ポインタと同じサイズを持つことが保証されている符号なし整数型
typedef tPointerSizedInteger<sizeof(void*)>::utype risse_ptruint;




} /* namespace Risse */

#endif

