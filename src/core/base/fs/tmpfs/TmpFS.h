//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tmpfs の実装
//---------------------------------------------------------------------------
#ifndef TmpFSH
#define TmpFSH

#include "FSManager.h"
#include "MemoryStream.h"


//---------------------------------------------------------------------------
//! @brief ノードを表すクラス
//---------------------------------------------------------------------------
class tTVPTmpFSNode
{
public:
	enum tType { ntDirectory, ntFile  };

private:
	tTVPTmpFSNode * Parent; //!< 親へのリンク
	tType Type; //!< ノードのタイプ
	ttstr Name;
	union
	{
		tTJSHashTable<ttstr, tTVPTmpFSNode *> * Directory; //!< ディレクトリ
		tTVPMemoryStreamBlock * File; //!< ファイル
	};
public:
	tTVPTmpFSNode(tTVPTmpFSNode *parent, tType type, const ttstr & name);
	tTVPTmpFSNode(tTVPTmpFSNode *parent, tType type, tTJSBinaryStream * src);
	~tTVPTmpFSNode();

public:
	void Serialize(tTJSBinaryStream * dest) const;

	tTVPTmpFSNode * GetSubNode(const ttstr & name);
	bool DeleteSubNodeByName(const ttstr & name);
	tTVPTmpFSNode * CreateDirectory(const ttstr & name);
	tTVPTmpFSNode * CreateFile(const ttstr & name);

	tType GetType() const { return Type; }
	bool IsFile() const { return Type == ntFile; }
	bool IsDirectory() const { return Type == ntDirectory; }
	tTVPMemoryStreamBlock * GetMemoryStreamBlockNoAddRef() { return File; } 
	tTVPTmpFSNode * GetParent() { return Parent; }
	const ttstr & GetName() const { return Name; }
	bool HasSubNode() const { return Type == ntDirectory &&
							Directory->GetCount() != 0; }
	tjs_size GetSize() const;
	size_t Iterate(iTVPFileSystemIterationCallback * callback);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tmp ファイルシステム
//---------------------------------------------------------------------------
class tTVPTmpFS : public iTVPFileSystem
{
	tTJSCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション
	tTVPTmpFSNode * Root; //!< ルートノード

	static const unsigned char SerializeMagic[];

public:
	tTVPTmpFS();

	//-- iTVPFileSystem メンバ
	~tTVPTmpFS();

	size_t GetFileListAt(const ttstr & dirname,
		iTVPFileSystemIterationCallback * callback);
	bool FileExists(const ttstr & filename);
	bool DirectoryExists(const ttstr & dirname);
	void RemoveFile(const ttstr & filename);
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);
	void CreateDirectory(const ttstr & dirname, bool recursive = false);
	void Stat(const ttstr & filename, tTVPStatStruc & struc);
	tTJSBinaryStream * CreateStream(const ttstr & filename, tjs_uint32 flags);

	//-- iTVPFileSystem メンバ ここまで

	void SerializeTo(tTJSBinaryStream * dest);
	void UnserializeFrom(tTJSBinaryStream * src);

private:
	tTVPTmpFSNode * GetNodeAt(const ttstr & name);
	static void SplitPathAndName(const ttstr & in, ttstr & path, ttstr & name);
	static void TrimLastPathDelimiter(ttstr & path);
	void CreateRoot();
	void RemoveRoot();
	void Clear();
};
//---------------------------------------------------------------------------


#endif
