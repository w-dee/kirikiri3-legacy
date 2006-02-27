//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief System クラス バインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/script/RisseEngine.h"
#include "base/system/SystemBind.h"
#include "risse/include/risseNative.h"

RISSE_DEFINE_SOURCE_ID(42003,44071,47107,17145,37820,23105,1673,13784);






















//---------------------------------------------------------------------------
//! @brief		Systemネイティブクラス
//---------------------------------------------------------------------------
class tRisseNC_System : public tRisseNativeClass
{
public:
	tRisseNC_System();

	static risse_uint32 ClassID;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		SystemクラスID
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_System::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNC_System::tRisseNC_System() :
	tRisseNativeClass(RISSE_WS("System"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/System)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*Risse class name*/ System)
{
	// このクラスはオブジェクトを持たないので (static class) 
	// ここでは何もしない
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/System)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS

	// register default "exceptionHandler" member
	tRisseVariant val((iRisseDispatch2*)NULL, (iRisseDispatch2*)NULL);
	PropSet(RISSE_MEMBERENSURE, RISSE_WS("exceptionHandler"), NULL, &val, this);
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaSystemRegisterer::tRisaSystemRegisterer()
{
	SystemClass =  new tRisseNC_System();
	try
	{
		tRisaRisseScriptEngine::instance()->
			RegisterGlobalObject(RISSE_WS("System"), SystemClass);
	}
	catch(...)
	{
		SystemClass->Release();
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief デストラクタ
//---------------------------------------------------------------------------
tRisaSystemRegisterer::~tRisaSystemRegisterer()
{
	SystemClass->Release();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		System.exceptionHandler を得る
//! @param		dest		exceptionHandler を格納するクロージャ
//! @return		正常な exceptionHandler を得られれば真、得られなければ偽
//---------------------------------------------------------------------------
bool tRisaSystemRegisterer::GetExceptionHandlerObject(tRisseVariantClosure & dest)
{
	tRisseVariant val;

	if(RISSE_FAILED(SystemClass->PropGet(
			RISSE_MEMBERMUSTEXIST, RISSE_WS("exceptionHandler"),
			NULL, &val, NULL)))
		return false;

	if(val.Type() != tvtObject) return false;

	dest = val.AsObjectClosure();

	if(!dest.Object)
	{
		dest.Release();
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------


