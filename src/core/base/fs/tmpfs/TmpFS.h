//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
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
class tRisaTmpFSNode
{
public:
	enum tType { ntDirectory, ntFile  };

private:
	tRisaTmpFSNode * Parent; //!< 親へのリンク
	tType Type; //!< ノードのタイプ
	ttstr Name;
	union
	{
		tRisseHashTable<ttstr, tRisaTmpFSNode *> * Directory; //!< ディレクトリ
		tRisaMemoryStreamBlock * File; //!< ファイル
	};
public:
	tRisaTmpFSNode(tRisaTmpFSNode *parent, tType type, const ttstr & name);
	tRisaTmpFSNode(tRisaTmpFSNode *parent, tType type, tRisseBinaryStream * src);
	~tRisaTmpFSNode();

public:
	void Serialize(tRisseBinaryStream * dest) const;

	tRisaTmpFSNode * GetSubNode(const ttstr & name);
	bool DeleteSubNodeByName(const ttstr & name);
	tRisaTmpFSNode * CreateDirectory(const ttstr & name);
	tRisaTmpFSNode * CreateFile(const ttstr & name);

	tType GetType() const { return Type; }
	bool IsFile() const { return Type == ntFile; }
	bool IsDirectory() const { return Type == ntDirectory; }
	tRisaMemoryStreamBlock * GetMemoryStreamBlockNoAddRef() { return File; } 
	tRisaTmpFSNode * GetParent() { return Parent; }
	const ttstr & GetName() const { return Name; }
	bool HasSubNode() const { return Type == ntDirectory &&
							Directory->GetCount() != 0; }
	risse_size GetSize() const;
	size_t Iterate(tRisaFileSystemIterationCallback * callback);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tmp ファイルシステム
//---------------------------------------------------------------------------
class tRisaTmpFS : public tRisaFileSystem
{
	tRisseCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション
	tRisaTmpFSNode * Root; //!< ルートノード

	static const unsigned char SerializeMagic[];

public:
	tRisaTmpFS();

	//-- tRisaFileSystem メンバ
	~tRisaTmpFS();

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

	void SerializeTo(tRisseBinaryStream * dest);
	void SerializeTo(const ttstr & filename);
	void UnserializeFrom(tRisseBinaryStream * src);
	void UnserializeFrom(const ttstr & filename);

private:
	tRisaTmpFSNode * GetNodeAt(const ttstr & name);
	static void SplitPathAndName(const ttstr & in, ttstr & path, ttstr & name);
	static void TrimLastPathDelimiter(ttstr & path);
	void CreateRoot();
	void RemoveRoot();
	void Clear();
};
//---------------------------------------------------------------------------


#endif
