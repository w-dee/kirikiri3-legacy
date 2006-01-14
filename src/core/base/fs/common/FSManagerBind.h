//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _FSMANAGERBIND_H_
#define _FSMANAGERBIND_H_

#include "FSManager.h"
#include "risseNative.h"
#include "Singleton.h"
#include "RisseEngine.h"


//---------------------------------------------------------------------------
//! @brief ファイルシステム ネイティブインスタンス
//! @note  このクラスは、RisseのオブジェクトにNativeInstanceとして(tRisseNI_BaseFileSystemとは別に)
//         登録する。
//!        下にあるtRisseNI_BaseFileSystemと混同しないこと。
//!        クラスIDは-2011 (固定) が割り当てられている (RISSE_DEFINE_SOURCE_IDと同じ)
//---------------------------------------------------------------------------
class tRisseNI_FileSystemNativeInstance : public tRisseNativeInstance
{
public:
	static const risse_int32 ClassID = -2011; // = RISSE_DEFINE_SOURCE_ID in FSManagerBind.cpp

private:
	boost::shared_ptr<tTVPFileSystem> FileSystem; //!< ファイルシステムオブジェクト
	iRisseDispatch2 * Owner; //!< このインスタンスを保持している Risse オブジェクト(AddRefしないこと)

public:
	tRisseNI_FileSystemNativeInstance(
		boost::shared_ptr<tTVPFileSystem> filesystem,
		iRisseDispatch2 * owner);

	void Invalidate();

	boost::shared_ptr<tTVPFileSystem> GetFileSystem() { return FileSystem; }

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief ファイルシステム 基底ネイティブインスタンス
//! @note	各ファイルシステムのRisseネイティブインスタンスはこれを継承して用いる
//---------------------------------------------------------------------------
class tRisseNI_BaseFileSystem : public tRisseNativeInstance
{
public:
	tRisseNI_BaseFileSystem();

	void Invalidate();

	boost::shared_ptr<tTVPFileSystem> & GetFileSystem() { return FileSystem; }

protected:
	void RegisterFileSystemNativeInstance(iRisseDispatch2 * risse_obj,
		tTVPFileSystem * filesystem);

private:
	boost::shared_ptr<tTVPFileSystem> FileSystem; //!< ファイルシステムオブジェクト
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		FileSystem ネイティブクラス (tTVPFileSystemManager のバインディング)
//! @note		Risseでのクラス名は FileSystem であるためこのように "FileSystem"
//!				の名が付いているが、実際は ファイルシステムマネージャへのバインディング
//!				であってファイルシステムへのバインディングではないので注意
//---------------------------------------------------------------------------
class tRisseNC_FileSystem : public tRisseNativeClass
{
public:
	tRisseNC_FileSystem();

	static risse_uint32 ClassID;
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tTVPFileSystemRegisterer
{
	iRisseDispatch2 * FileSystemClass;

	tTVPSingleton<tTVPRisseScriptEngine> ref_tTVPRisseScriptEngine; //!< tTVPRisseScriptEngine に依存

public:
	tTVPFileSystemRegisterer();
	~tTVPFileSystemRegisterer();

	static boost::shared_ptr<tTVPFileSystemRegisterer> & instance() { return
		tTVPSingleton<tTVPFileSystemRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す

	void RegisterClassObject(const risse_char *name, iRisseDispatch2 * object);
};
//---------------------------------------------------------------------------







#endif
