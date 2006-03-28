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

#include <assert.h>

#if defined(DEBUG) || defined(_DEBUG)
	#ifndef NDEBUG
		#define NDEBUG
	#endif
#endif

#define RISSE_ASSERT assert

#endif
