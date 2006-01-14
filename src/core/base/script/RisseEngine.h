//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseスクリプトエンジンの開始・終了・スクリプト実行などのインターフェース
//---------------------------------------------------------------------------
#ifndef RisseENGINEH
#define RisseENGINEH

#include "risse.h"
#include "Singleton.h"


//---------------------------------------------------------------------------
//! @brief		Risseスクリプトエンジンへのインターフェース
//---------------------------------------------------------------------------
class tTVPRisseScriptEngine
{
	tRisse *Engine;

public:
	tTVPRisseScriptEngine();
	~tTVPRisseScriptEngine();

private:
	tTVPSingletonObjectLifeTracer<tTVPRisseScriptEngine> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tTVPRisseScriptEngine> & instance() { return
		tTVPSingleton<tTVPRisseScriptEngine>::instance();
			} //!< このシングルトンのインスタンスを返す

	void Shutdown();

	tRisse * GetEngineNoAddRef() { return Engine; } //!< スクリプトエンジンを返す
	iRisseDispatch2 * GetGlobalNoAddRef()
		{ if(!Engine) return NULL; return Engine->GetGlobalNoAddRef(); } //!< スクリプトエンジンを返す
	void RegisterGlobalObject(const risse_char *name, iRisseDispatch2 * object);

};
//---------------------------------------------------------------------------




#endif
//---------------------------------------------------------------------------
