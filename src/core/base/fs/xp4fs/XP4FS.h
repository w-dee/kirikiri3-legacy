//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
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
#include "XP4Archive.h"



//---------------------------------------------------------------------------
//! @brief		XP4FS の実装
//---------------------------------------------------------------------------
class tTVPXP4FS : public iTVPFileSystem
{
	tTJSCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション

	//! @brief ファイル一個一個の情報
	struct tFileItemBasicInfo
	{
		tjs_size ArchiveIndex; //!< どのアーカイブに含まれるか
		tjs_size FileIndex; //!< そのアーカイブ内でのファイルのインデックス
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

	std::vector<boost::shared_ptr<tTVPArchive> > Archives; //!< アーカイブの配列
	std::vector<tFileItemInfo> FileItems; //! ファイルの情報の配列

public:
	//! @brief アーカイブを読み込む際に各アーカイブ内のファイルごとに呼ばれるコールバック
	class iMapCallback
	{
		virtual void operator () (const ttstr & name, tjs_size file_index) = 0; //!< 追加/置き換えの場合
		virtual void operator () (const ttstr & name) = 0; //!< 削除の場合
	};


public:
	tTVPXP4FS(const ttstr & name);

	//-- iTVPFileSystem メンバ
	~tTVPXP4FS();

	size_t GetFileListAt(const ttstr & dirname,
		iTVPFileSystemIterationCallback * callback);
	bool FileExists(const ttstr & filename);
	bool DirectoryExists(const ttstr & dirname);
	void RemoveFile(const ttstr & filename);
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);
	void CreateDirectory(const ttstr & dirname, bool recursive = false);
	void Stat(const ttstr & filename, tTVPStatStruc & struc);
	tTVPBinaryStream * CreateStream(const ttstr & filename, tjs_uint32 flags);

	//-- iTVPFileSystem メンバ ここまで
private:
	tjs_size GetFileItemStartIndex(const ttstr & name);
	tjs_size GetFileItemIndex(const ttstr & name);

};
//---------------------------------------------------------------------------




#endif
