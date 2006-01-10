//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TJS3スクリプトエンジンの開始・終了・スクリプト実行などのインターフェース
//---------------------------------------------------------------------------
#ifndef TJSENGINEH
#define TJSENGINEH

#include "tjs.h"
#include <boost/pool/detail/singleton.hpp>



//---------------------------------------------------------------------------
//! @brief		TJS3スクリプトエンジンへのインターフェース
//---------------------------------------------------------------------------
class tTVPTJS3ScriptEngine
{
	tTJS *Engine;

public:
	tTVPTJS3ScriptEngine();
	~tTVPTJS3ScriptEngine();

	static tTVPTJS3ScriptEngine & instance() { return
		boost::details::pool::singleton_default<tTVPTJS3ScriptEngine>::instance();
			} //!< このシングルトンのインスタンスを返す

	tTJS * GetEngineNoAddRef() { return Engine; } //!< スクリプトエンジンを返す
	iTJSDispatch2 * GetGlobalNoAddRef()
		{ return Engine->GetGlobalNoAddRef(); } //!< スクリプトエンジンを返す
	void RegisterGlobalObject(const tjs_char *name, iTJSDispatch2 * object);

};
//---------------------------------------------------------------------------




#endif
//---------------------------------------------------------------------------
