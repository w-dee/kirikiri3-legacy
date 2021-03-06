//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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

namespace Risse{
	/**
	 * Assert メッセージを表示し、強制終了する
	 * @param message	メッセージ
	 * @param filename	assertion failed が発生したファイル名
	 * @param lineno	assertion failed が発生した行
	 */
	void Assert(const char * message, const char * filename, int lineno);
}


#ifdef RISSE_ASSERT_ENABLED
	#define RISSE_ASSERT(X) ((void)( (!(bool)(X))?((void)::Risse::Assert(#X, __FILE__, __LINE__)) : (void)0))
#else
	#define RISSE_ASSERT(X) ((void)0)
#endif

#endif
