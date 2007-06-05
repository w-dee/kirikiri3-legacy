//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログ機構のRisseバインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/log/Log.h"
#include "base/exception/RisaException.h"
#include "base/script/RisseEngine.h"
#include "risse/include/risseNative.h"

RISSE_DEFINE_SOURCE_ID(17391,44513,60617,19121,16540,39031,65356,19696);


#if 0


//---------------------------------------------------------------------------
//! @brief		Log ネイティブクラス (tRisaLogger のバインディング)
//---------------------------------------------------------------------------
class tRisseNC_Log : public tRisseNativeClass
{
public:
	tRisseNC_Log();

	static risse_uint32 ClassID;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief   LogクラスID
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_Log::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief   コンストラクタ
//---------------------------------------------------------------------------
tRisseNC_Log::tRisseNC_Log() :
	tRisseNativeClass(RISSE_WS("Log"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/Log)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*Risse class name*/ Log)
{
	// このクラスはオブジェクトを持たないので (static class) 
	// ここでは何もしない
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/Log)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/message)
{
	/*%
		@brief	メッセージをログに出力する
		@param	message			メッセージ
		@param	level			ログレベル(llDebugなど)
		@note
		<p>
			メッセージをログに出力します。
		</p>
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	ttstr msg = *param[0];
	tRisaLogger::tLevel level = tRisaLogger::llInfo;
	if(RISSE_PARAM_EXIST(1))
		level = static_cast<tRisaLogger::tLevel>((risse_int)*param[1]);

	tRisaLogger::Log(msg, level);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/message)
//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief Logクラスレジストラ
//---------------------------------------------------------------------------
class tRisaLogRegisterer :  public singleton_base<tRisaLogRegisterer>, protected depends_on<tRisaRisseScriptEngine>
{
public:
	tRisaLogRegisterer();
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaLogRegisterer::tRisaLogRegisterer()
{
	// クラスを登録する

	iRisseDispatch2 *dsp = new tRisseNC_Log();
	try
	{
		depends_on<tRisaRisseScriptEngine>::locked_instance()->RegisterGlobalObject(RISSE_WS("Log"), dsp);
	}
	catch(...)
	{
		dsp->Release();
		throw;
	}
	dsp->Release();

	// 定数を登録する
	depends_on<tRisaRisseScriptEngine>::locked_instance()->ExecuteScript(
		ttstr(
		"var llDebug = 0;"		//!< デバッグに関する物
		"var llInfo = 1;"		//!< 情報通知
		"var llNotice = 2;"		//!< 通常状態だが大事な情報通知
		"var llWarning = 3;"	//!< 警告
		"var llError = 4;"		//!< 通常のエラー
		"var llRecord = 5;"		//!< 記録すべき重要な情報
		"var llCritical = 6;"	//!< 致命的なエラー
		));
}
//---------------------------------------------------------------------------

#endif


