//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
#include "base/utils/Thread.h"



//---------------------------------------------------------------------------
//! @brief		XP4FS の実装
//---------------------------------------------------------------------------
class tRisaXP4FS : public tRisaFileSystem
{
	tRisaCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション

	//! @brief ファイル一個一個の情報
	struct tFileItemBasicInfo
	{
		risse_size ArchiveIndex; //!< どのアーカイブに含まれるか
		risse_size FileIndex; //!< そのアーカイブ内でのファイルのインデックス
	};

	//! @brief 
	struct tFileItemInfo : public tFileItemBasicInfo
	{
		ttstr Name; //!< 名前
		tFileItemInfo(const tFileItemBasicInfo & info, const ttstr & name) :
			tFileItemBasicInfo(info), Name(name) {;} //!< コンストラクタ
		bool operator < (const tFileItemInfo & rhs) const 
			{ return Name < rhs.Name; } //!< 比較関数
	};

	std::vector<boost::shared_ptr<tRisaXP4Archive> > Archives; //!< アーカイブの配列
	std::vector<tFileItemInfo> FileItems; //! ファイルの情報の配列

public:
	tRisaXP4FS(const ttstr & name);

	//-- tRisaFileSystem メンバ
	~tRisaXP4FS();

	size_t GetFileListAt(const ttstr & dirname,
		tRisaFileSystemIterationCallback * callback);
	bool FileExists(const ttstr & filename);
	bool DirectoryExists(const ttstr & dirname);
	void RemoveFile(const ttstr & filename);
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);
	void CreateDirectory(const ttstr & dirname, bool recursive = false);
	void Stat(const ttstr & filename, tRisaStatStruc & struc);
	tRisseBinaryStream * CreateStream(const ttstr & filename, risse_uint32 flags);

	//-- tRisaFileSystem メンバ ここまで
private:
	risse_size GetFileItemStartIndex(const ttstr & name);
	risse_size GetFileItemIndex(const ttstr & name);

};
//---------------------------------------------------------------------------




#endif
