//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tmpfs の実装
//---------------------------------------------------------------------------
#ifndef TmpFSH
#define TmpFSH

#include "base/fs/common/FSManager.h"
#include "base/fs/tmpfs/MemoryStream.h"
#include "base/utils/RisaThread.h"


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
	tRisseString Name;
	union
	{
		tRisseHashTable<tRisseString, tRisaTmpFSNode *> * Directory; //!< ディレクトリ
		tRisaMemoryStreamBlock * File; //!< ファイル
	};
public:
	//! @brief		コンストラクタ
	//! @param		parent 親ノード
	//! @param		type ノードタイプ
	//! @param		name ノードの名前
	tRisaTmpFSNode(tRisaTmpFSNode *parent, tType type, const tRisseString & name);

	//! @brief		シリアライズされたデータを読み取るコンストラクタ
	//! @param		parent 親ノード
	//! @param		type ノードタイプ
	//! @param		src 入力もとストリーム
	tRisaTmpFSNode(tRisaTmpFSNode *parent, tType type, tRisseBinaryStream * src);

	//! @brief		デストラクタ
	~tRisaTmpFSNode();

public:
	//! @brief		内容をシリアライズする
	//! @param		dest 出力先ストリーム
	void Serialize(tRisseBinaryStream * dest) const;

	//! @brief		指定された名前を持つノードを返す
	//! @param		name 名前
	//! @return		ノード(ノードが見つからない場合は NULL)
	//! @note		このノードが File を表す場合も NULL が返る
	tRisaTmpFSNode * GetSubNode(const tRisseString & name);

	//! @brief		指定された名前を持つサブノードを削除する
	//! @param		name 名前
	//! @return		削除に成功すれば真
	bool DeleteSubNodeByName(const tRisseString & name);

	//! @brief		指定された名前を持つディレクトリを作成する
	//! @param		name 名前
	//! @return		新規に作成されたディレクトリノード
	//! @note		すでにその名前を持つノードがあった場合は何もしないで NULL を返すので注意
	tRisaTmpFSNode * CreateDirectory(const tRisseString & name);

	//! @brief		指定された名前を持つファイルを作成する
	//! @param		name 名前
	//! @return		新規に作成されたファイルノード
	//! @note		すでにその名前を持つノードがあった場合は何もしないで NULL を返すので注意
	tRisaTmpFSNode * CreateFile(const tRisseString & name);

	tType GetType() const { return Type; }
	bool IsFile() const { return Type == ntFile; }
	bool IsDirectory() const { return Type == ntDirectory; }
	tRisaMemoryStreamBlock * GetMemoryStreamBlockNoAddRef() { return File; } 
	tRisaTmpFSNode * GetParent() { return Parent; }
	const tRisseString & GetName() const { return Name; }
	bool HasSubNode() const { return Type == ntDirectory &&
							Directory->GetCount() != 0; }

	//! @brief		ノードのサイズを取得する
	//! @return		ノードのサイズ (ファイルの場合はファイルサイズ、そうでない場合は 0)
	risse_size GetSize() const;

	//! @brief		すべての子要素に対して callback を呼び出す
	//! @return		callback を呼び出した回数
	size_t Iterate(tRisaFileSystemIterationCallback * callback);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tmp ファイルシステム
//---------------------------------------------------------------------------
class tRisaTmpFS : public tRisaFileSystem
{
	tRisaCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション
	tRisaTmpFSNode * Root; //!< ルートノード

	//! @brief		シリアライズ時のファイルの先頭に着くマジック
	static const unsigned char SerializeMagic[];

public:
	//! @brief		コンストラクタ
	tRisaTmpFS();

	//-- tRisaFileSystem メンバ
	//! @brief		デストラクタ
	~tRisaTmpFS();

	//! @brief		ファイル一覧を取得する
	//! @param		dirname ディレクトリ名
	//! @param		callback コールバックオブジェクト
	//! @return		取得できたファイル数
	size_t GetFileListAt(const tRisseString & dirname,
		tRisaFileSystemIterationCallback * callback);

	//! @brief		ファイルが存在するかどうかを得る
	//! @param		filename ファイル名
	//! @return		ファイルが存在する場合真
	bool FileExists(const tRisseString & filename);

	//! @brief		ディレクトリが存在するかどうかを得る
	//! @param		dirname ディレクトリ名
	//! @return		ディレクトリが存在する場合真
	bool DirectoryExists(const tRisseString & dirname);

	//! @brief		ファイルを削除する
	//! @param		filename ファイル名
	void RemoveFile(const tRisseString & filename);

	//! @brief		ディレクトリを削除する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを削除するかどうか
	void RemoveDirectory(const tRisseString & dirname, bool recursive = false);

	//! @brief		ディレクトリを作成する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを作成するかどうか
	void CreateDirectory(const tRisseString & dirname, bool recursive = false);

	//! @brief		指定されたファイルの stat を得る
	//! @param		filename ファイル名
	//! @param		struc stat 結果の出力先
	void Stat(const tRisseString & filename, tRisaStatStruc & struc);

	//! @brief		指定されたファイルのストリームを得る
	//! @param		filename ファイル名
	//! @param		flags フラグ
	//! @return		ストリームオブジェクト
	tRisseBinaryStream * CreateStream(const tRisseString & filename, risse_uint32 flags);
	//-- tRisaFileSystem メンバ ここまで

	//! @brief		指定されたストリームに内容をシリアライズする
	//! @param		dest 出力先ストリーム
	void SerializeTo(tRisseBinaryStream * dest);

	//! @brief		指定されたファイルに内容をシリアライズする
	//! @param		filename 出力先ファイル名
	void SerializeTo(const tRisseString & filename);

	//! @brief		指定されたストリームから内容を復元する
	//! @param		src 入力元ストリーム
	void UnserializeFrom(tRisseBinaryStream * src);

	//! @brief		指定されたファイルから内容を復元する
	//! @param		filename 入力元ファイル
	void UnserializeFrom(const tRisseString & filename);

private:
	//! @brief		指定された位置のノードを得る
	//! @param		name ノード
	//! @return		その位置にあるノード。その位置が見つからない場合は NULL
	tRisaTmpFSNode * GetNodeAt(const tRisseString & name);

	//! @brief		ルートディレクトリを作成する
	void CreateRoot();

	//! @brief		ルートディレクトリを削除する
	void RemoveRoot();

	//! @brief		内容をすべてクリアする
	void Clear();
};
//---------------------------------------------------------------------------


#endif
