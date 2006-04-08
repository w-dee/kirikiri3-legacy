//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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



//---------------------------------------------------------------------------
//! @brief		PathFS の実装
//---------------------------------------------------------------------------
class tRisaPathFS : public tRisaFileSystem
{
	tRisaCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション
	typedef tRisseHashTable<ttstr, ttstr> tHash; //!< ファイルシステム中のファイル名と実際のファイル名の対応表のtypedef
	tHash Hash;//!< ファイルシステム中のファイル名と実際のファイル名の対応表
	std::vector<ttstr> Paths; //!< パス(最初の文字が ' ' ならばディレクトリ単独、'+' ならばrecursive)
	bool NeedRebuild; //!< パスのハッシュ表を作り直す必要がある場合に真になる

public:
	//! @brief		コンストラクタ
	tRisaPathFS();

	//-- tRisaFileSystem メンバ
	//! @brief		デストラクタ
	~tRisaPathFS();

	//! @brief		ファイル一覧を取得する
	//! @param		dirname ディレクトリ名
	//! @param		callback コールバックオブジェクト
	//! @return		取得できたファイル数
	size_t GetFileListAt(const ttstr & dirname,
		tRisaFileSystemIterationCallback * callback);

	//! @brief		ファイルが存在するかどうかを得る
	//! @param		filename ファイル名
	//! @return		ファイルが存在する場合真
	bool FileExists(const ttstr & filename);

	//! @brief		ディレクトリが存在するかどうかを得る
	//! @param		dirname ディレクトリ名
	//! @return		ディレクトリが存在する場合真
	bool DirectoryExists(const ttstr & dirname);

	//! @brief		ファイルを削除する
	//! @param		filename ファイル名
	void RemoveFile(const ttstr & filename);

	//! @brief		ディレクトリを削除する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを削除するかどうか
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);

	//! @brief		ディレクトリを作成する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを作成するかどうか
	void CreateDirectory(const ttstr & dirname, bool recursive = false);

	//! @brief		指定されたファイルの stat を得る
	//! @param		filename ファイル名
	//! @param		struc stat 結果の出力先
	void Stat(const ttstr & filename, tRisaStatStruc & struc);

	//! @brief		指定されたファイルのストリームを得る
	//! @param		filename ファイル名
	//! @param		flags フラグ
	//! @return		ストリームオブジェクト
	tRisseBinaryStream * CreateStream(const ttstr & filename, risse_uint32 flags);

	//-- tRisaFileSystem メンバ ここまで
public:
	//! @brief		パスにディレクトリを追加する
	//! @brief		name: 名前
	//! @brief		recursive: 再帰的に名前を検索するかどうか
	void Add(const ttstr & name, bool recursive = false);

	//! @brief		パスからディレクトリを削除する
	//! @param		name 名前
	void Remove(const ttstr & name);

private:
	//! @brief		ファイルシステム中のファイル名と実際のファイル名の対応表を作り直す
	void Ensure();
};
//---------------------------------------------------------------------------




#endif
