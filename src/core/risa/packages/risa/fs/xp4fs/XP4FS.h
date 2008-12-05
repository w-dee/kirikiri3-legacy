//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4ファイルシステムの実装
//---------------------------------------------------------------------------

#ifndef XP4FSH
#define XP4FSH

#include <boost/shared_ptr.hpp>
#include "base/fs/common/FSManager.h"
#include "base/fs/xp4fs/XP4Archive.h"
#include "base/utils/RisaThread.h"

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * XP4FS の実装
 */
class tXP4FS : public tFileSystem
{
	tCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション

	/**
	 * ファイル一個一個の情報
	 */
	struct tFileItemBasicInfo
	{
		risse_size ArchiveIndex; //!< どのアーカイブに含まれるか
		risse_size FileIndex; //!< そのアーカイブ内でのファイルのインデックス
	};

	/**
	 * 
	 */
	struct tFileItemInfo : public tFileItemBasicInfo
	{
		tString Name; //!< 名前
		tFileItemInfo(const tFileItemBasicInfo & info, const tString & name) :
			tFileItemBasicInfo(info), Name(name) {;} //!< コンストラクタ
		bool operator < (const tFileItemInfo & rhs) const 
			{ return Name < rhs.Name; } //!< 比較関数
	};

	gc_vector<boost::shared_ptr<tXP4Archive> > Archives; //!< アーカイブの配列
	gc_vector<tFileItemInfo> FileItems; //! ファイルの情報の配列

public:
	/**
	 * コンストラクタ
	 */
	tXP4FS(const tString & name);

	//-- tFileSystem メンバ
	/**
	 * デストラクタ
	 */
	~tXP4FS();

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
private:
	/**
	 * name ファイル名で始まる最初の FileItems内のインデックスを得る
	 * @param name	名前
	 * @return	FileItems内のインデックス (見つからなかった場合は (risse_size)-1 が返る)
	 */
	risse_size GetFileItemStartIndex(const tString & name);

	/**
	 * name に対応する FileItems内のインデックスを得る
	 * @param name	名前
	 * @return	FileItems内のインデックス (見つからなかった場合は (risse_size)-1 が返る)
	 * @note	GetFileItemStartIndex と違い、その名前とぴったり一致しない限りは見つからないとみなす
	 */
	risse_size GetFileItemIndex(const tString & name);

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


#endif
