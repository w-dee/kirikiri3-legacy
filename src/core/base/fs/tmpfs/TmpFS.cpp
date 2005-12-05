//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tmpfs の実装
//---------------------------------------------------------------------------

#include "TmpFS.h"




//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPTmpFSNode::tTVPTmpFSNode(tTVPTmpFSNode *parent, tTVPTmpFSNode::tType type,
							const ttstr & name) :
	Parent(parent),
	Type(type),
	Name(name)
{
	if(Type == ntDirectory)
	{
		// ディレクトリのノードなので ハッシュ表を作成
		Directory = new tTJSHashTable<ttstr, tTVPTmpFSNode *>();
	}
	else
	{
		// ファイルのノードなので Block を作成
		Block = new tTVPMemoryStreamBlock();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPTmpFSNode::~tTVPTmpFSNode(const ttstr & name)
{
	// デストラクタ
	// デストラクタは、保持しているノードを「すべて」解放するので注意
	// 要するに、このノード以下のすべてのノードも連鎖的に一緒に消滅する
	if(Type == ntDirectory)
	{
		// このノードはディレクトリ
		// Directory が保持しているすべての要素を解放する
		tTJSHashTable<ttstr, tTVPTmpFSNode *>::tIterator i;
		for(i = Directory->GetFirst(); !i.IsNull(); i++)
		{
			delete (i->GetValue());
			i->GetValue() = NULL;
		}

		// Directory も解放する
		delete Directory, Directory = NULL;
	}
	else if(Type == ntFile)
	{
		// このノードはファイル
		File->Release();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された名前を持つノードを返す
//! @param		name: 名前
//! @return		ノード(ノードが見つからない場合は NULL)
//! @note		このノードが File を表す場合も NULL が返る
//---------------------------------------------------------------------------
tTVPTmpFSNode * tTVPTmpFSNode::GetSubNode(const ttstr & name)
{
	if(Type != ntDirectory) return NULL;

	tTVPTmpFSNode ** node = Directory->Find(name);
	if(!node) return NULL;
	return * node;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された名前を持つサブノードを削除する
//! @param		name: 名前
//! @return		削除に成功すれば真
//---------------------------------------------------------------------------
bool tTVPTmpFSNode::DeleteSubNodeByName(const ttstr & name)
{
	if(Type != ntDirectory) return false;
	iTVPTmpFSNode * node = GetSubNode(name);
	if(node)
	{
		delete node;
		Directory->Delete(name);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された名前を持つディレクトリを作成する
//! @param		name: 名前
//! @return		新規に作成されたディレクトリノード
//! @note		すでにその名前を持つノードがあった場合は何もしないで NULL を返すので注意
//---------------------------------------------------------------------------
tTVPTmpFSNode * tTVPTmpFSNode::CreateDirectory(const ttstr & name)
{
	if(Type != ntDirectory) return false;
	if(GetSubNode(name)) return NULL; // すでにそこに何かがある
	tTVPTmpFSNode *newnode = new tTVPTmpFSNode(this, ntDirectory, name);
	Directory->Add(name, newnode);
	return newnode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された名前を持つファイルを作成する
//! @param		name: 名前
//! @return		新規に作成されたファイルノード
//! @note		すでにその名前を持つノードがあった場合は何もしないで NULL を返すので注意
//---------------------------------------------------------------------------
tTVPTmpFSNode * tTVPTmpFSNode::CreateFile(const ttstr & name)
{
	if(Type != ntDirectory) return false;
	if(GetSubNode(name)) return NULL; // すでにそこに何かがある
	tTVPTmpFSNode *newnode = new tTVPTmpFSNode(this, ntFile, name);
	Directory->Add(name, newnode);
	return newnode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ノードのサイズを取得する
//! @return		ノードのサイズ (ファイルの場合はファイルサイズ、そうでない場合は 0)
//---------------------------------------------------------------------------
tjs_size tTVPTmpFSNode::GetSize() const
{
	if(Type == ntDirectory) return 0;
	if(Type == ntFile) return File->GetSize();
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		すべての子要素に対して callback を呼び出す
//! @return		callback を呼び出した回数
//---------------------------------------------------------------------------
size_t tTVPTmpFSNode::Iterate(iTVPFileSystemIterationCallback * callback)
{
	if(Type != ntDirectory) return 0;
	size_t count = 0;
	tTJSHashTable<ttstr, tTVPTmpFSNode *>::tIterator i;
	for(i = Directory->GetFirst(); !i.IsNull(); i++)
	{
		count ++;
		if(i->GetValue()->Type == ntDirectory)
		{
			if(!callback->OnDirectory(i->GetValue()->Name)) break;
		}
		else if(i->GetValue()->Type == ntFile)
		{
			if(!callback->OnFile(i->GetValue()->Name)) break;
		}
	}
	return count;
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPTmpFS::tTVPTmpFS()
{
	// 変数初期化
	RefCount = 1; // 初期の参照カウントは1
	CreateRoot();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPTmpFS::~tTVPTmpFS()
{
	RemoveRoot();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		参照カウンタを一つ増やす
//---------------------------------------------------------------------------
void tTVPTmpFS::AddRef()
{
	volatile tTJSCriticalSectionHolder holder(CS);

	RefCount ++;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		参照カウンタを一つ減らす
//---------------------------------------------------------------------------
void tTVPTmpFS::Release()
{
	tjs_uint decremented_count;

	{
		volatile tTJSCriticalSectionHolder holder(CS);

		RefCount --;
		decremented_count = RefCount;
	}

	if(decremented_count == 0) delete this;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル一覧を取得する
//! @param		dirname: ディレクトリ名
//! @param		callback: コールバックオブジェクト
//! @return		取得できたファイル数
//---------------------------------------------------------------------------
size_t tTVPTmpFS::GetFileListAt(const ttstr & dirname,
	iTVPFileSystemIterationCallback * callback)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tTVPTmpFSNode * node = GetNodeAt(dirname);
	if(!node) tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsDirectory()) TVPThrowExceptionMessage(_("specified name is not a directory"));

	return node->Iterate(callback);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルが存在するかどうかを得る
//! @param		filename: ファイル名
//! @return		ファイルが存在する場合真
//---------------------------------------------------------------------------
bool tTVPTmpFS::FileExists(const ttstr & filename)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tTVPTmpFSNode * node = GetNodeAt(filename);
	if(!node) return false;
	if(!node->IsFile()) return false; // それがファイルではない場合も false
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリが存在するかどうかを得る
//! @param		dirname: ディレクトリ名
//! @return		ディレクトリが存在する場合真
//---------------------------------------------------------------------------
bool tTVPTmpFS::DirectoryExists(const ttstr & dirname)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tTVPTmpFSNode * node = GetNodeAt(filename);
	if(!node) return false;
	if(!node->IsDirectory()) return false; // それがディレクトリではない場合も false
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルを削除する
//! @param		filename: ファイル名
//---------------------------------------------------------------------------
void tTVPTmpFS::RemoveFile(const ttstr & filename)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tTVPTmpFSNode * node = GetNodeAt(filename);
	if(!node) tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsFile()) TVPThrowExceptionMessage(_("specified name is not a file"));

	// 親ノードから切り離す
	tTVPTmpFSNode * parent = node->GetParent();
	if(!parent) return; // parent が null なのは root ノードだけなのでこれはあり得ないが...
	parent->DeleteSubNodeByName(node->GetName());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを削除する
//! @param		dirname: ディレクトリ名
//! @param		recursive: 再帰的にディレクトリを削除するかどうか
//---------------------------------------------------------------------------
void tTVPTmpFS::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tTVPTmpFSNode * node = GetNodeAt(dirname);
	if(!node) tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsDirectory()) TVPThrowExceptionMessage(_("specified name is not a directory"));

	if(!recursive)
	{
		// recursive が false の場合、消そうとしたディレクトリが空で無ければ
		// 失敗する。

		if(node->HasSubNode()) TVPThrowExceptionMessage(_("specified directory is not empty"));

		// root ノードは消すことはできない
		if(node == Root) TVPThrowExceptionMessage(_("can not remove file system root directory"));
	}
	else
	{
		// recursive が true の場合、そのノードがディレクトリであることを確認後、消すだけ
	}

	// 親ノードから切り離す
	if(node != Root)
	{
		tTVPTmpFSNode * parent = node->GetParent();
		if(parent) parent->DeleteSubNodeByName(node->GetName());
	}
	else
	{
		// root に対して再帰的に削除しようとしたとき
		Clear(); // 内容をすべて削除する
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを作成する
//! @param		dirname: ディレクトリ名
//! @param		recursive: 再帰的にディレクトリを作成するかどうか
//---------------------------------------------------------------------------
void tTVPTmpFS::CreateDirectory(const ttstr & dirname, bool recursive)
{
	volatile tTJSCriticalSectionHolder holder(CS);


	if(recursive)
	{
		// 再帰的にディレクトリを作成する場合
		// '/' で name を区切り、順に root からノードをたどっていく
		const tjs_char * p = dirname.c_str();
		const tjs_char *pp = p;

		tTVPTmpFSNode *node = Root;
		while(*p)
		{
			while(*p != TJS_W('/') && *p != 0) p++;
			if(p != pp)
			{
				// '/' で挟まれた区間が得られた
				tTVPTmpFSNode *parent = node;
				ttstr partname(p, p - pp);
				node = node->GetSubNode(partname);
				if(!node)
				{
					// そのノードは見つからなかった
					if(recursive)
					{
						// 再帰的にディレクトリを作成するならば
						// ディレクトリをそこに作成する
						node = parent->CreateDirectory(partname);
					}
				}
				else
				{
					// ノードは見つかったが
					if(node->IsFile())
					{
						// ファイルだったりする
						TVPThrowExceptionMessage(_("about to create file on "));
					}
				}
			}
			pp = p;
		}
	}
	else
	{
		// 再帰的にはディレクトリを作成しない場合
		// 最終的なディレクトリ名となる名前を取得する

		ttstr path(dirname);
		tTVPFileSystemManager::TrimLastPathDelimiter(path); // dirname の最後の '/' は取り去る

		tTVPFileSystemManager::SplitPathAndName(path, parentdir, name); // パスを分離
		
		TTVPTmpFSNode * parentnode = GetNodeAt(parentdir);
		if(!parentnode) tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();
		parentnode->CreateDirectory(name);
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルの stat を得る
//! @param		filename: ファイル名
//! @param		struc: stat 結果の出力先
//---------------------------------------------------------------------------
void tTVPTmpFS::Stat(const ttstr & filename, tTVPStatStruc & struc)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	// XXX: MACタイムは現バージョンでは保存していない
	struc.Clear():

	tTVPTmpFSNode * node = GetNodeAt(filename);
	if(!node) tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	struc.Size = node->GetSize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルのストリームを得る
//! @param		filename: ファイル名
//! @param		flags: フラグ
//! @return		ストリームオブジェクト
//---------------------------------------------------------------------------
iTVPBinaryStream * tTVPTmpFS::CreateStream(const ttstr & filename, tjs_uint32 flags)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tTVPTmpFSNode * node = GetNodeAt(filename);
	if(!node) tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsFile()) TVPThrowExceptionMessage(_("specified name is not a file"));

	return new tTVPMemoryStream(flags, node->GetMemoryStreamBlock());
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		指定された位置のノードを得る
//! @param		name: ノード
//! @return		その位置にあるノード。その位置が見つからない場合は NULL
//---------------------------------------------------------------------------
tTVPTmpFSNode * tTVPTmpFS::GetNodeAt(const ttstr & name)
{
	// '/' で name を区切り、順に root からノードをたどっていく
	const tjs_char * p = name.c_str();
	const tjs_char *pp = p;

	tTVPTmpFSNode *node = Root;

	while(*p)
	{
		while(*p != TJS_W('/') && *p != 0) p++;
		if(p != pp)
		{
			// '/' で挟まれた区間が得られた
			node = node->GetSubNode(ttstr(p, p - pp));
			if(!node) return NULL;
		}
		pp = p;
	}

	if(name.EndWith(TJS_W('/')) && !node->IsDirectory())
	{
		// 名前の最後が '/' で終わっている (つまり、ディレクトリである
		// ことを期待している) がノードがディレクトリではない
		return NULL; // null を返す
	}

	return node;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ルートディレクトリを作成する
//---------------------------------------------------------------------------
void tTVPTmpFS::CreateRoot()
{
	if(Root) return;

	// ルートノードを作成
	Root = new tTVPTmpFSNode(NULL, ntDirectory, ttstr());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ルートディレクトリを削除する
//---------------------------------------------------------------------------
void tTVPTmpFS::RemoveRoot()
{
	// root ノードを削除
	delete Root, Root = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		内容をすべてクリアする
//---------------------------------------------------------------------------
void tTVPTmpFS::Clear()
{
	RemoveRoot();
	CreateRoot();
}
//---------------------------------------------------------------------------

