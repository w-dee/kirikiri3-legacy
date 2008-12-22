//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XMMユーティリティ
//---------------------------------------------------------------------------

/*
	and

	the original copyright:
*/


/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2003             *
 * by the XIPHOPHORUS Company http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: Header of SSE Function Library
 last mod: $Id: xmmlib.h,v 1.3 2005-07-08 15:00:00+09 blacksword Exp $

 ********************************************************************/

/*
	Based on a file from Ogg Vorbis Optimization Project:
	 http://homepage3.nifty.com/blacksword/
*/

#ifndef _XMMLIB_H_INCLUDED
#define _XMMLIB_H_INCLUDED

#include "risseTypes.h"
using namespace Risse;
#include <malloc.h>

namespace Risa {
//---------------------------------------------------------------------------


#if	defined(__GNUC__)||defined(_MSC_VER)
#else
#error "Not supported System."
#endif

#ifndef	STIN
#ifdef __GNUC__
#define STIN static __inline__
#elif _WIN32
#define STIN static __inline
#else
#define STIN static
#endif
#endif

#if	(defined(__SSE__)&&defined(__GNUC__))||defined(_MSC_VER)

/* We need type definitions from the XMM header file.  */
#include <xmmintrin.h>
#ifdef	_MSC_VER
#include <emmintrin.h>	// if gcc is 3.4, this file need to include.
#endif

#ifdef __GNUC__
#define _SALIGN16(x) static x __attribute__((aligned(16)))
#define _ALIGN16(x) x __attribute__((aligned(16)))
#else
#define _SALIGN16(x) __declspec(align(16)) static x
#define _ALIGN16(x) __declspec(align(16)) x
#endif
#define PM64(x)		(*(__m64*)(x))
#define PM128(x)	(*(__m128*)(x))
#ifdef	__SSE2__
#define PM128I(x)	(*(__m128i*)(x))
#endif

typedef union {
	unsigned char	si8[8];
	unsigned short	si16[4];
	unsigned long	si32[2];
	char			ssi8[8];
	short			ssi16[4];
	long			ssi32[2];
	__m64			pi64;
} __m64x;

#if		defined(_MSC_VER)
typedef union __declspec(intrin_type) __declspec(align(16)) __m128x{
	unsigned long	si32[4];
	float			sf[4];
	__m64			pi64[2];
	__m128			ps;
#ifdef	__SSE2__
	__m128i			pi;
	__m128d			pd;
#endif
} __m128x;

#elif	defined(__GNUC__)
typedef union {
	unsigned long	si32[4];
	float			sf[4];
	__m64			pi64[2];
	__m128			ps;
#ifdef	__SSE2__
	__m128i			pi;
	__m128d			pd;
#endif
} __m128x __attribute__((aligned(16)));

#endif

extern const risse_uint32 PCS_NNRN[4];
extern const risse_uint32 PCS_NNRR[4];
extern const risse_uint32 PCS_NRNN[4];
extern const risse_uint32 PCS_NRNR[4];
extern const risse_uint32 PCS_NRRN[4];
extern const risse_uint32 PCS_NRRR[4];
extern const risse_uint32 PCS_RNNN[4];
extern const risse_uint32 PCS_RNRN[4];
extern const risse_uint32 PCS_RNRR[4];
extern const risse_uint32 PCS_RRNN[4];
extern const risse_uint32 PCS_RNNR[4];
extern const risse_uint32 PCS_RRRR[4];
extern const risse_uint32 PCS_NNNR[4];
extern const risse_uint32 PABSMASK[4];
extern const risse_uint32 PSTARTEDGEM1[4];
extern const risse_uint32 PSTARTEDGEM2[4];
extern const risse_uint32 PSTARTEDGEM3[4];
extern const risse_uint32 PENDEDGEM1[4];
extern const risse_uint32 PENDEDGEM2[4];
extern const risse_uint32 PENDEDGEM3[4];
extern const risse_uint32 PMASKTABLE[16*4];

extern const float PFV_0[4];
extern const float PFV_1[4];
extern const float PFV_2[4];
extern const float PFV_4[4];
extern const float PFV_8[4];
extern const float PFV_INIT[4];
extern const float PFV_0P5[4];

STIN __m128 _mm_untnorm_ps(__m128 x)
{
	_SALIGN16(const risse_uint32)	PIV0[4]	 = {
		0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000
	};
	register __m128 r;
	r	 = _mm_and_ps(x, PM128(PCS_RRRR));
	r	 = _mm_or_ps(x, PM128(PIV0));
	return	r;
}

STIN float _mm_add_horz(__m128 x)
{
#if !defined(__INTEL_COMPILER)
	float r;
#endif
//#pragma warning(disable : 592)
	__m128	y;
	y	 = _mm_movehl_ps(y, x);
//#pragma warning(default : 592)
	x	 = _mm_add_ps(x, y);
	y	 = x;
	y	 = _mm_shuffle_ps(y, y, _MM_SHUFFLE(1,1,1,1));
	x	 = _mm_add_ss(x, y);
#if defined(__INTEL_COMPILER)
	return _mm_cvtss_f32(x);
#else
	_mm_store_ss(&r, x);
	return r;
#endif
}

STIN __m128 _mm_add_horz_ss(__m128 x)
{
//#pragma warning(disable : 592)
	__m128	y;
	y	 = _mm_movehl_ps(y, x);
//#pragma warning(default : 592)
	x	 = _mm_add_ps(x, y);
	y	 = x;
	y	 = _mm_shuffle_ps(y, y, _MM_SHUFFLE(1,1,1,1));
	x	 = _mm_add_ss(x, y);
	return x;
}

STIN float _mm_max_horz(__m128 x)
{
#if !defined(__INTEL_COMPILER)
	float r;
#endif
//#pragma warning(disable : 592)
	__m128	y;
	y	 = _mm_movehl_ps(y, x);
//#pragma warning(default : 592)
	x	 = _mm_max_ps(x, y);
	y	 = x;
	y	 = _mm_shuffle_ps(y, y, _MM_SHUFFLE(1,1,1,1));
	x	 = _mm_max_ss(x, y);
#if defined(__INTEL_COMPILER)
	return _mm_cvtss_f32(x);
#else
	_mm_store_ss(&r, x);
	return r;
#endif
}

STIN float _mm_min_horz(__m128 x)
{
#if !defined(__INTEL_COMPILER)
	float r;
#endif
//#pragma warning(disable : 592)
	__m128	y;
	y	 = _mm_movehl_ps(y, x);
//#pragma warning(default : 592)
	x	 = _mm_min_ps(x, y);
	y	 = x;
	y	 = _mm_shuffle_ps(y, y, _MM_SHUFFLE(1,1,1,1));
	x	 = _mm_min_ss(x, y);
#if defined(__INTEL_COMPILER)
	return _mm_cvtss_f32(x);
#else
	_mm_store_ss(&r, x);
	return r;
#endif
}

#endif /* (defined(__SSE__)&&defined(__GNUC__))||defined(_MSC_VER) */


/**
 * 128ビット境界にポインタがアラインメントされているかどうか
 * @param p	ポインタ
 * @return	128ビット境界にポインタがアラインメントされているかどうか
 */
inline bool IsAlignedTo128bits(const void * p)
{
	return !(reinterpret_cast<risse_offset>(p) & 0xf);
}


#ifdef __GNUC__
// TODO: 64bitではここらへんどうなるんですかね


/**
 * スタックを128bit境界にあわせた上で指定された関数を呼び出す関数を生成するためのマクロ
 */
// alloca を用いると、コンパイラは -fomit-frame-pointer を指定されていても
// フレームポインタを作らざるを得なくなる。
// あとはスタックポインタを調整して目的の関数を呼び出す。
// 目的の関数を呼び出す際にインライン展開されるとやっかいなため、
// いったんもう一つのトランポリンを呼ぶ。
// これらの処理のため、このマクロで生成された関数の呼び出しは少々高価
// になるので注意すること。
#define RISA_DEFINE_STACK_ALIGN_128_TRAMPOLINE(rettype, funcname, args, callee, callee_args) \
	__attribute__((noinline)) rettype funcname##_r args { return callee callee_args; }      \
	__attribute__((noinline)) rettype funcname args {                                       \
		alloca(1); asm __volatile__ ("andl $-16, %%esp" : : : "%esp" );              \
		return funcname##_r callee_args; }

#else
	// おそらくコンパイラにあわせた実装が必要
	#pragma XX Need compiler dependant implementation of stack alignment
#define RISA_DEFINE_STACK_ALIGN_128_TRAMPOLINE(rettype funcname, args, callee) \
	rettype funcname args { return  funcname callee_args; }

#endif

//---------------------------------------------------------------------------
} // namespace Risa

#endif /* _XMMLIB_H_INCLUDED */
