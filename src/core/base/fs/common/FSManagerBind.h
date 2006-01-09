//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャのTJS3バインディング
//---------------------------------------------------------------------------
#ifndef _FSMANAGERBIND_H_
#define _FSMANAGERBIND_H_

#include "FSManager.h"
#include "tjsNative.h"

//---------------------------------------------------------------------------
//! @brief ファイルシステム ネイティブインスタンス
//! @note  このクラスは、TJSのオブジェクトにNativeInstanceとして(tTJSNI_BaseFileSystemとは別に)
//         登録する。
//!        下にあるtTJSNI_BaseFileSystemと混同しないこと。
//!        クラスIDは-2011 (固定) が割り当てられている (TJS_DEFINE_SOURCE_IDと同じ)
//---------------------------------------------------------------------------
class tTJSNI_FileSystemNativeInstance : public tTJSNativeInstance
{
public:
	static const tjs_int32 ClassID = -2011; // = TJS_DEFINE_SOURCE_ID in FSManagerBind.cpp

private:
	boost::shared_ptr<tTVPFileSystem> FileSystem; //!< ファイルシステムオブジェクト
	iTJSDispatch2 * Owner; //!< このインスタンスを保持している TJS オブジェクト(AddRefしないこと)

public:
	tTJSNI_FileSystemNativeInstance(
		boost::shared_ptr<tTVPFileSystem> filesystem,
		iTJSDispatch2 * owner);

	void Invalidate();

	boost::shared_ptr<tTVPFileSystem> GetFileSystem() { return FileSystem; }

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief ファイルシステム 基底ネイティブインスタンス
//! @note	各ファイルシステムのTJSネイティブインスタンスはこれを継承して用いる
//---------------------------------------------------------------------------
class tTJSNI_BaseFileSystem : public tTJSNativeInstance
{
public:
	tTJSNI_BaseFileSystem();

	void Invalidate();

	boost::shared_ptr<tTVPFileSystem> & GetFileSystem() { return FileSystem; }

protected:
	void RegisterFileSystemNativeInstance(iTJSDispatch2 * tjs_obj,
		tTVPFileSystem * filesystem);

private:
	boost::shared_ptr<tTVPFileSystem> FileSystem; //!< ファイルシステムオブジェクト
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		FileSystem ネイティブクラス (tTVPFileSystemManager のバインディング)
//! @note		TJSでのクラス名は FileSystem であるためこのように "FileSystem"
//!				の名が付いているが、実際は ファイルシステムマネージャへのバインディング
//!				であってファイルシステムへのバインディングではないので注意
//---------------------------------------------------------------------------
class tTJSNC_FileSystem : public tTJSNativeClass
{
public:
	tTJSNC_FileSystem();

	static tjs_uint32 ClassID;
};
//---------------------------------------------------------------------------


#endif
