//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Assertion 定義
//---------------------------------------------------------------------------
#ifndef risseAssertH
#define risseAssertH


#if defined(DEBUG) || defined(_DEBUG)
	#ifdef NDEBUG
		#undef NDEBUG
	#endif
#else
	#ifndef NDEBUG
		#define NDEBUG
	#endif
#endif

#ifndef NDEBUG
	#define RISSE_ASSERT_ENABLED
#endif

#define RISSE_ASSERT assert

#include <assert.h>

#endif
