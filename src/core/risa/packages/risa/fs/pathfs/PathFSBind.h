//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief PathFSのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _PATHSFSBIND_H_
#define _PATHSFSBIND_H_

#if 0
#include "base/fs/common/FSManagerBind.h"
#include "base/fs/pathfs/PathFS.h"
#include "risseNative.h"


namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * PathFS ネイティブインスタンス
 */
class tNI_PathFS : public tNI_BaseFileSystem
{
public:
	/**
	 * コンストラクタ
	 */
	tNI_PathFS();

	/**
	 * Risse コンストラクタ
	 */
	risse_error Construct(risse_int numparams,
		tVariant **param, iRisseDispatch2 *risse_obj);

	/**
	 * Risse 無効化関数
	 */
	void Invalidate();

	tPathFS * GetFileSystem() 
	{
		return static_cast<tPathFS*>(
			tNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * PathFS ネイティブクラス
 */
class tNC_PathFS : public tNativeClass
{
public:
	/**
	 * コンストラクタ
	 */
	tNC_PathFS();

	/**
	 * PathFS クラスID
	 */
	static risse_uint32 ClassID;

private:
	/**
	 * ネイティブインスタンスを作成して返す
	 * @return	ネイティブインスタンス
	 */
	tNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * クラスレジストラ
 */
class tPathFSRegisterer :
	public singleton_base<tPathFSRegisterer>,
	protected depends_on<tRisseScriptEngine>,
	protected depends_on<tFileSystemRegisterer>
{
public:
	/**
	 * コンストラクタ
	 */
	tPathFSRegisterer();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif

#endif
