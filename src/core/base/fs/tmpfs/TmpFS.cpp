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
#include "prec.h"
#include "base/fs/tmpfs/TmpFS.h"
#include "base/exception/RisaException.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(20965,62764,62977,19688,31398,64150,8840,17077);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTmpFSNode::tTmpFSNode(tTmpFSNode *parent, tTmpFSNode::tType type,
							const tString & name) :
	Parent(parent),
	Type(type),
	Name(name)
{
	if(Type == ntDirectory)
	{
		// ディレクトリのノードなので ハッシュ表を作成
		Directory = new tHashTable<tString, tTmpFSNode *>();
	}
	else
	{
		// ファイルのノードなので Block を作成
		File = new tMemoryStreamBlock();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTmpFSNode::tTmpFSNode(tTmpFSNode *parent, tTmpFSNode::tType type,
	tBinaryStream * src) :
	Parent(parent)
{
	if(Type == ntDirectory)
	{
		// ディレクトリのノードなので ハッシュ表を作成
		Directory = new tHashTable<tString, tTmpFSNode *>();
	}
	else
	{
		// ファイルのノードなので Block を作成
		File = new tMemoryStreamBlock();
	}

	// メタデータを読み取る
	while(true)
	{
		unsigned char metadataid;
		src->ReadBuffer(&metadataid, 1);

		if(metadataid == 0) break;

		wxUint32 metalen;
		src->ReadBuffer(&metalen, sizeof(metalen));
		metalen = wxUINT32_SWAP_ON_BE(metalen);
		if(metalen == 0)
			eRisaException::Throw(RISSE_WS_TR("invalid metadata length (data may be corrupted)"));

		if(metadataid == 0x01)
		{
			// ファイル名
			unsigned char * p = new (PointerFreeGC) unsigned char [metalen];
			src->ReadBuffer(p, metalen);
			p[metalen - 1] = 0;
			Name = tString(wxString(reinterpret_cast<char*>(p), wxConvUTF8));
		}
		else
		{
			// 未知のメタデータなので読み飛ばす
			src->SetPosition(src->GetPosition() + metalen);
		}
	}

	// ノードタイプに従って処理
	if(Type == ntDirectory)
	{
		// ディレクトリ
		bool done = false;
		while(!done)
		{
			// サブノードのタイプを読み取る
			unsigned char nodetypeid;
			src->ReadBuffer(&nodetypeid, 1);
			tTmpFSNode * subnode = NULL;
			switch(nodetypeid)
			{
			case 0x80: // ディレクトリ
				subnode = new tTmpFSNode(this, ntDirectory, src);
				break;
			case 0x81: // ファイル
				subnode = new tTmpFSNode(this, ntFile, src);
				break;
			case 0x88: // ディレクトリ終了
				done = true;
				break;
			default:
				eRisaException::Throw(tString(
						wxString::Format(RISSE_WS_TR("unsupported node id %x"),
						static_cast<int>(nodetypeid))));
			}
		}
	}
	else if(Type == ntFile)
	{
		// ファイル
		wxUint64 blocksize;
		src->ReadBuffer(&blocksize, sizeof(blocksize));
		blocksize = wxUINT64_SWAP_ON_BE(blocksize);
		if(static_cast<size_t>(blocksize) != blocksize)
				eRisaException::Throw(RISSE_WS_TR("too big block size"));
		File->ChangeSize(static_cast<size_t>(blocksize));
		File->Fit();
		src->ReadBuffer(File->GetBlock(), static_cast<size_t>(blocksize));
	}

	// 親に自分を登録
	if(Parent)
		Parent->Directory->Add(Name, this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTmpFSNode::~tTmpFSNode()
{
	// デストラクタ
	// デストラクタは、保持しているノードを「すべて」解放するので注意
	// 要するに、このノード以下のすべてのノードも連鎖的に一緒に消滅する
	if(Type == ntDirectory)
	{
		// このノードはディレクトリ
		// Directory が保持しているすべての要素を解放する
		tHashTable<tString, tTmpFSNode *>::tIterator i;
		for(i = Directory->GetFirst(); !i.IsNull(); i++)
		{
			delete (i.GetValue());
			i.GetValue() = NULL;
		}

		// Directory も解放する
		delete Directory, Directory = NULL;
	}
	else if(Type == ntFile)
	{
		// このノードはファイル
		File->Release(), File = NULL;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSNode::Serialize(tBinaryStream * dest) const
{
	// ノードのタイプを記録
	if(Type == ntDirectory)
		dest->WriteBuffer("\x80", 1); // ディレクトリを表す
	else if(Type == ntFile)
		dest->WriteBuffer("\x81", 1); // ファイルを表す

	// 名前を格納
	dest->WriteBuffer("\1", 1); // ファイル名

	wxCharBuffer utf8name = Name.AsWxString().mb_str(wxConvUTF8);
	size_t utf8name_len = strlen(utf8name);

	wxUint32 i32;
	i32 = wxUINT32_SWAP_ON_BE(static_cast<wxUint32>(utf8name_len + 1));
	dest->WriteBuffer(&i32, sizeof(i32));
	dest->WriteBuffer(utf8name, utf8name_len + 1);

	if(Type == ntDirectory)
	{
		// ディレクトリ
		dest->WriteBuffer("\0", 1); // メタデータの終わりとディレクトリの開始

		// 全ての子要素に対して再帰する
		tHashTable<tString, tTmpFSNode *>::tIterator i;
		for(i = Directory->GetFirst(); !i.IsNull(); i++)
		{
			i.GetValue()->Serialize(dest);
		}

		dest->WriteBuffer("\x88", 1); // ディレクトリの終わりを表す
	}
	else if(Type == ntFile)
	{
		// ファイル
		dest->WriteBuffer("\x0", 1); // メタデータの終わりとファイルの中身の開始
		wxUint64 i64;
		volatile tCriticalSection::tLocker  holder(File->GetCS());
		i64 = wxUINT64_SWAP_ON_BE(File->GetSize());
		dest->WriteBuffer(&i64, sizeof(i64));
		dest->WriteBuffer(File->GetBlock(), File->GetSize());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTmpFSNode * tTmpFSNode::GetSubNode(const tString & name)
{
	if(Type != ntDirectory) return NULL;

	tTmpFSNode ** node = Directory->Find(name);
	if(!node) return NULL;
	return * node;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tTmpFSNode::DeleteSubNodeByName(const tString & name)
{
	if(Type != ntDirectory) return false;
	tTmpFSNode * node = GetSubNode(name);
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
tTmpFSNode * tTmpFSNode::CreateDirectory(const tString & name)
{
	if(Type != ntDirectory) return false;
	if(GetSubNode(name)) return NULL; // すでにそこに何かがある
	tTmpFSNode *newnode = new tTmpFSNode(this, ntDirectory, name);
	Directory->Add(name, newnode);
	return newnode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTmpFSNode * tTmpFSNode::CreateFile(const tString & name)
{
	if(Type != ntDirectory) return false;
	if(GetSubNode(name)) return NULL; // すでにそこに何かがある
	tTmpFSNode *newnode = new tTmpFSNode(this, ntFile, name);
	Directory->Add(name, newnode);
	return newnode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTmpFSNode::GetSize() const
{
	if(Type == ntDirectory) return 0;
	if(Type == ntFile) return File->GetSize();
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tTmpFSNode::Iterate(tFileSystemIterationCallback * callback)
{
	if(Type != ntDirectory) return 0;
	size_t count = 0;
	tHashTable<tString, tTmpFSNode *>::tIterator i;
	for(i = Directory->GetFirst(); !i.IsNull(); i++)
	{
		count ++;
		if(i.GetValue()->Type == ntDirectory)
		{
			if(callback)
			{
				if(!callback->OnDirectory(i.GetValue()->Name)) break;
			}
		}
		else if(i.GetValue()->Type == ntFile)
		{
			if(callback)
			{
				if(!callback->OnFile(i.GetValue()->Name)) break;
			}
		}
	}
	return count;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
const unsigned char tTmpFS::SerializeMagic[] = {
	't', 'm' , 'p', 'f', 's', 0x1a,
	0x00, // ファイルレイアウトバージョン
	0x00  // reserved
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTmpFS::tTmpFS()
{
	// 変数初期化
	CreateRoot();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTmpFS::~tTmpFS()
{
	RemoveRoot();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tTmpFS::GetFileListAt(const tString & dirname,
	tFileSystemIterationCallback * callback)
{
	volatile tCriticalSection::tLocker holder(CS);

	tTmpFSNode * node = GetNodeAt(dirname);
	if(!node) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsDirectory()) eRisaException::Throw(RISSE_WS_TR("specified name is not a directory"));

	return node->Iterate(callback);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tTmpFS::FileExists(const tString & filename)
{
	volatile tCriticalSection::tLocker holder(CS);

	tTmpFSNode * node = GetNodeAt(filename);
	if(!node) return false;
	if(!node->IsFile()) return false; // それがファイルではない場合も false
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tTmpFS::DirectoryExists(const tString & dirname)
{
	volatile tCriticalSection::tLocker holder(CS);

	tTmpFSNode * node = GetNodeAt(dirname);
	if(!node) return false;
	if(!node->IsDirectory()) return false; // それがディレクトリではない場合も false
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFS::RemoveFile(const tString & filename)
{
	volatile tCriticalSection::tLocker holder(CS);

	tTmpFSNode * node = GetNodeAt(filename);
	if(!node) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsFile()) eRisaException::Throw(RISSE_WS_TR("specified name is not a file"));

	// 親ノードから切り離す
	tTmpFSNode * parent = node->GetParent();
	if(!parent) return; // parent が null なのは root ノードだけなのでこれはあり得ないが...
	parent->DeleteSubNodeByName(node->GetName());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFS::RemoveDirectory(const tString & dirname, bool recursive)
{
	volatile tCriticalSection::tLocker holder(CS);

	tTmpFSNode * node = GetNodeAt(dirname);
	if(!node) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsDirectory()) eRisaException::Throw(RISSE_WS_TR("specified name is not a directory"));

	if(!recursive)
	{
		// recursive が false の場合、消そうとしたディレクトリが空で無ければ
		// 失敗する。

		if(node->HasSubNode()) eRisaException::Throw(RISSE_WS_TR("specified directory is not empty"));

		// root ノードは消すことはできない
		if(node == Root) eRisaException::Throw(RISSE_WS_TR("can not remove file system root directory"));
	}
	else
	{
		// recursive が true の場合、そのノードがディレクトリであることを確認後、消すだけ
	}

	// 親ノードから切り離す
	if(node != Root)
	{
		tTmpFSNode * parent = node->GetParent();
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
void tTmpFS::CreateDirectory(const tString & dirname, bool recursive)
{
	volatile tCriticalSection::tLocker holder(CS);


	if(recursive)
	{
		// 再帰的にディレクトリを作成する場合
		// '/' で name を区切り、順に root からノードをたどっていく
		const risse_char * p = dirname.c_str();
		const risse_char *pp = p;

		tTmpFSNode *node = Root;
		while(*p)
		{
			while(*p != RISSE_WC('/') && *p != 0) p++;
			if(p != pp)
			{
				// '/' で挟まれた区間が得られた
				tTmpFSNode *parent = node;
				tString partname(p, p - pp);
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
						eRisaException::Throw(RISSE_WS_TR("about to create file on "));
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

		tString path(dirname);
		tFileSystemManager::TrimLastPathDelimiter(path); // dirname の最後の '/' は取り去る
		tString parentdir, name;
		tFileSystemManager::SplitPathAndName(path, &parentdir, &name); // パスを分離

		tTmpFSNode * parentnode = GetNodeAt(parentdir);
		if(!parentnode) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();
		parentnode->CreateDirectory(name);
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFS::Stat(const tString & filename, tStatStruc & struc)
{
	volatile tCriticalSection::tLocker holder(CS);

	// XXX: MACタイムは現バージョンでは保存していない
	struc.Clear();

	tTmpFSNode * node = GetNodeAt(filename);
	if(!node) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	struc.Size = node->GetSize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tBinaryStream * tTmpFS::CreateStream(const tString & filename, risse_uint32 flags)
{
	volatile tCriticalSection::tLocker holder(CS);

	tTmpFSNode * node = GetNodeAt(filename);
	if(!node) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsFile()) eRisaException::Throw(RISSE_WS_TR("specified name is not a file"));

	return new tMemoryStream(flags, node->GetMemoryStreamBlockNoAddRef());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFS::SerializeTo(tBinaryStream * dest)
{
	volatile tCriticalSection::tLocker holder(CS);

	// マジックを書き込む
	dest->WriteBuffer(SerializeMagic, 8);

	// root に対して内容Serialize
	// (すると後は自動的に全てのノードがシリアライズされる)
	Root->Serialize(dest);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFS::SerializeTo(const tString & filename)
{
	std::auto_ptr<tBinaryStream>
		stream(tFileSystemManager::instance()->CreateStream(filename, RISSE_BS_WRITE));

	SerializeTo(stream.get());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFS::UnserializeFrom(tBinaryStream * src)
{
	volatile tCriticalSection::tLocker holder(CS);

	// マジックを読み込み、比較する
	unsigned char magic[8];
	src->ReadBuffer(magic, 8);

	unsigned char firstnodetype;
	src->ReadBuffer(&firstnodetype, 1);
		// 最初のノードタイプ(ディレクトリを表す 0x80 になってないとおかしい)

	if(memcmp(magic, SerializeMagic, 8) || firstnodetype != 0x80)
		eRisaException::Throw(RISSE_WS_TR("not a tmpfs archive"));

	// 再帰的に内容を読み込む
	Root = new tTmpFSNode(NULL, tTmpFSNode::ntDirectory, src);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFS::UnserializeFrom(const tString & filename)
{
	std::auto_ptr<tBinaryStream>
		stream(tFileSystemManager::instance()->CreateStream(filename, RISSE_BS_READ));

	UnserializeFrom(stream.get());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTmpFSNode * tTmpFS::GetNodeAt(const tString & name)
{
	// '/' で name を区切り、順に root からノードをたどっていく
	const risse_char * p = name.c_str();
	const risse_char *pp = p;

	tTmpFSNode *node = Root;

	while(*p)
	{
		while(*p != RISSE_WC('/') && *p != 0) p++;
		if(p != pp)
		{
			// '/' で挟まれた区間が得られた
			node = node->GetSubNode(tString(p, p - pp));
			if(!node) return NULL;
		}
		pp = p;
	}

	if(name.EndsWith(RISSE_WC('/')) && !node->IsDirectory())
	{
		// 名前の最後が '/' で終わっている (つまり、ディレクトリである
		// ことを期待している) がノードがディレクトリではない
		return NULL; // null を返す
	}

	return node;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFS::CreateRoot()
{
	if(Root) return;

	// ルートノードを作成
	Root = new tTmpFSNode(NULL, tTmpFSNode::ntDirectory, tString());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFS::RemoveRoot()
{
	// root ノードを削除
	delete Root, Root = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFS::Clear()
{
	RemoveRoot();
	CreateRoot();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

