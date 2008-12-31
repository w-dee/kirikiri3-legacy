//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Pathファイルシステムの実装
//---------------------------------------------------------------------------

#ifndef PATHFSH
#define PATHFSH

#include <boost/shared_ptr.hpp>
#include "base/utils/RisaThread.h"
#include "base/fs/common/FSManager.h"


namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * PathFS の実装
 */
class tPathFS : public tFileSystem
{
	tCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション
	typedef tHashTable<tString, tString> tHash; //!< ファイルシステム中のファイル名と実際のファイル名の対応表のtypedef
	tHash Hash;//!< ファイルシステム中のファイル名と実際のファイル名の対応表
	gc_vector<tString> Paths; //!< パス(最初の文字が ' ' ならばディレクトリ単独、'+' ならばrecursive)
	bool NeedRebuild; //!< パスのハッシュ表を作り直す必要がある場合に真になる

public:
	/**
	 * コンストラクタ
	 */
	tPathFS();

	//-- tFileSystem メンバ
	/**
	 * デストラクタ
	 */
	~tPathFS();

	/**
	 * ファイル一覧を取得する
	 * @param dirname	ディレクトリ名
	 * @param callback	コールバックオブジェクト
	 * @return	取得できたファイル数
	 */
	size_t GetFileListAt(const tString & dirname,
		tFileSystemIterationCallback * callback);

	/**
	 * ファイルが存在するかどうかを得る
	 * @param filename	ファイル名
	 * @return	ファイルが存在する場合真
	 */
	bool FileExists(const tString & filename);

	/**
	 * ディレクトリが存在するかどうかを得る
	 * @param dirname	ディレクトリ名
	 * @return	ディレクトリが存在する場合真
	 */
	bool DirectoryExists(const tString & dirname);

	/**
	 * ファイルを削除する
	 * @param filename	ファイル名
	 */
	void RemoveFile(const tString & filename);

	/**
	 * ディレクトリを削除する
	 * @param dirname	ディレクトリ名
	 * @param recursive	再帰的にディレクトリを削除するかどうか
	 */
	void RemoveDirectory(const tString & dirname, bool recursive = false);

	/**
	 * ディレクトリを作成する
	 * @param dirname	ディレクトリ名
	 * @param recursive	再帰的にディレクトリを作成するかどうか
	 */
	void CreateDirectory(const tString & dirname, bool recursive = false);

	/**
	 * 指定されたファイルの stat を得る
	 * @param filename	ファイル名
	 * @param struc		stat 結果の出力先
	 */
	void Stat(const tString & filename, tStatStruc & struc);

	/**
	 * 指定されたファイルのストリームを得る
	 * @param filename	ファイル名
	 * @param flags		フラグ
	 * @return	ストリームオブジェクト
	 */
	tBinaryStream * CreateStream(const tString & filename, risse_uint32 flags);

	//-- tFileSystem メンバ ここまで
public:
	/**
	 * パスにディレクトリを追加する
	 * name: 名前
	 * recursive: 再帰的に名前を検索するかどうか
	 */
	void Add(const tString & name, bool recursive = false);

	/**
	 * パスからディレクトリを削除する
	 * @param name	名前
	 */
	void Remove(const tString & name);

private:
	/**
	 * ファイルシステム中のファイル名と実際のファイル名の対応表を作り直す
	 */
	void Ensure();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa



#endif
