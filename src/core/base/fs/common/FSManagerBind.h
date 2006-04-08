//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _FSMANAGERBIND_H_
#define _FSMANAGERBIND_H_

#include "base/fs/common/FSManager.h"
#include "risse/include/risseNative.h"
#include "base/utils/Singleton.h"
#include "base/script/RisseEngine.h"


//---------------------------------------------------------------------------
//! @brief ファイルシステム ネイティブインスタンス
//! @note  このクラスは、RisseのオブジェクトにNativeInstanceとして(tRisseNI_BaseFileSystemとは別に)
//         登録する。
//!        下にあるtRisseNI_BaseFileSystemと混同しないこと。
//!        クラスIDは-2011 (固定) が割り当てられている
//---------------------------------------------------------------------------
class tRisseNI_FileSystemNativeInstance : public tRisseNativeInstance
{
public:
	static const risse_int32 ClassID = -2011;

private:
	boost::shared_ptr<tRisaFileSystem> FileSystem; //!< ファイルシステムオブジェクト
	iRisseDispatch2 * Owner; //!< このインスタンスを保持している Risse オブジェクト(AddRefしないこと)

public:
	//! @brief		コンストラクタ
	//! @param		filesystem  ファイルシステムオブジェクト
	//! @param		owner       このインスタンスを保持している Risse オブジェクト
	tRisseNI_FileSystemNativeInstance(
		boost::shared_ptr<tRisaFileSystem> filesystem,
		iRisseDispatch2 * owner);

	//! @brief		Risse 無効化関数
	void Invalidate();

	//! @brief		ファイルシステムをオブジェクトを得る
	boost::shared_ptr<tRisaFileSystem> GetFileSystem() { return FileSystem; }

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
	//! @brief		コンストラクタ
	tRisseNI_BaseFileSystem();

	//! @brief		Risse 無効化関数
	void Invalidate();

	//! @brief		ファイルシステムをオブジェクトを得る
	boost::shared_ptr<tRisaFileSystem> & GetFileSystem() { return FileSystem; }

protected:
	//! @brief		tRisseNI_FileSystemNativeInstance をファイルシステムから構築し、
	//!             オブジェクトに登録する
	//! @param		risse_obj		登録先Risseオブジェクト
	//! @param		fs_obj		ファイルシステムオブジェクト
	void RegisterFileSystemNativeInstance(iRisseDispatch2 * risse_obj,
		tRisaFileSystem * filesystem);

private:
	boost::shared_ptr<tRisaFileSystem> FileSystem; //!< ファイルシステムオブジェクト
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		FileSystem ネイティブクラス (tRisaFileSystemManager のバインディング)
//! @note		Risseでのクラス名は FileSystem であるためこのように "FileSystem"
//!				の名が付いているが、実際は ファイルシステムマネージャへのバインディング
//!				であってファイルシステムへのバインディングではないので注意
//---------------------------------------------------------------------------
class tRisseNC_FileSystem : public tRisseNativeClass
{
public:
	//! @brief   コンストラクタ
	tRisseNC_FileSystem();

	//! @brief   FileSystemクラスID
	static risse_uint32 ClassID;
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tRisaFileSystemRegisterer :
		public singleton_base<tRisaFileSystemRegisterer>,
		protected depends_on<tRisaRisseScriptEngine>
{
	iRisseDispatch2 * FileSystemClass;

public:
	//! @brief		コンストラクタ
	tRisaFileSystemRegisterer();

	//! @brief		デストラクタ
	~tRisaFileSystemRegisterer();

	//! @brief		ファイルシステムにクラスオブジェクトを登録する
	//! @param		name    クラス名
	//! @param		object  クラスオブジェクト
	void RegisterClassObject(const risse_char *name, iRisseDispatch2 * object);
};
//---------------------------------------------------------------------------







#endif
