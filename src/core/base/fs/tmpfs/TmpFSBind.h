//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TmpFSのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _TMPFSBIND_H_
#define _TMPFSBIND_H_

#include "FSManagerBind.h"
#include "TmpFS.h"
#include "risseNative.h"
#include "Singleton.h"

//---------------------------------------------------------------------------
//! @brief TmpFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_TmpFS : public tRisseNI_BaseFileSystem
{
public:
	tRisseNI_TmpFS();

	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);
	void Invalidate();

	tRisaTmpFS * GetFileSystem() 
	{
		return reinterpret_cast<tRisaTmpFS*>(
			tRisseNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief TmpFS ネイティブクラス
//---------------------------------------------------------------------------
class tRisseNC_TmpFS : public tRisseNativeClass
{
public:
	tRisseNC_TmpFS();

	static risse_uint32 ClassID;

private:
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tRisaTmpFSRegisterer
{
	tRisaSingleton<tRisaRisseScriptEngine> ref_tRisaRisseScriptEngine; //!< tRisaRisseScriptEngine に依存
	tRisaSingleton<tRisaFileSystemRegisterer> ref_tRisaFileSystemRegisterer; //!< tRisaFileSystemRegisterer に依存
public:
	tRisaTmpFSRegisterer();

private:
	tRisaSingletonObjectLifeTracer<tRisaTmpFSRegisterer> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tRisaTmpFSRegisterer> & instance() { return
		tRisaSingleton<tRisaTmpFSRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------



#endif
