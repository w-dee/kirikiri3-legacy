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
#include "prec.h"
#include "base/fs/tmpfs/TmpFS.h"
#include "base/exception/RisaException.h"

RISSE_DEFINE_SOURCE_ID(20965,62764,62977,19688,31398,64150,8840,17077);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		parent 親ノード
//! @param		type ノードタイプ
//! @param		name ノードの名前
//---------------------------------------------------------------------------
tRisaTmpFSNode::tRisaTmpFSNode(tRisaTmpFSNode *parent, tRisaTmpFSNode::tType type,
							const ttstr & name) :
	Parent(parent),
	Type(type),
	Name(name)
{
	if(Type == ntDirectory)
	{
		// ディレクトリのノードなので ハッシュ表を作成
		Directory = new tRisseHashTable<ttstr, tRisaTmpFSNode *>();
	}
	else
	{
		// ファイルのノードなので Block を作成
		File = new tRisaMemoryStreamBlock();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シリアライズされたデータを読み取るコンストラクタ
//! @param		parent 親ノード
//! @param		type ノードタイプ
//! @param		src 入力もとストリーム
//---------------------------------------------------------------------------
tRisaTmpFSNode::tRisaTmpFSNode(tRisaTmpFSNode *parent, tRisaTmpFSNode::tType type,
	tRisseBinaryStream * src) :
	Parent(parent)
{
	if(Type == ntDirectory)
	{
		// ディレクトリのノードなので ハッシュ表を作成
		Directory = new tRisseHashTable<ttstr, tRisaTmpFSNode *>();
	}
	else
	{
		// ファイルのノードなので Block を作成
		File = new tRisaMemoryStreamBlock();
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
			unsigned char * p = new unsigned char [metalen];
			src->ReadBuffer(p, metalen);
			p[metalen - 1] = 0;
			Name = ttstr(wxString(reinterpret_cast<char*>(p), wxConvUTF8));
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
			tRisaTmpFSNode * subnode = NULL;
			switch(nodetypeid)
			{
			case 0x80: // ディレクトリ
				subnode = new tRisaTmpFSNode(this, ntDirectory, src);
				break;
			case 0x81: // ファイル
				subnode = new tRisaTmpFSNode(this, ntFile, src);
				break;
			case 0x88: // ディレクトリ終了
				done = true;
				break;
			default:
				eRisaException::Throw(ttstr(
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
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaTmpFSNode::~tRisaTmpFSNode()
{
	// デストラクタ
	// デストラクタは、保持しているノードを「すべて」解放するので注意
	// 要するに、このノード以下のすべてのノードも連鎖的に一緒に消滅する
	if(Type == ntDirectory)
	{
		// このノードはディレクトリ
		// Directory が保持しているすべての要素を解放する
		tRisseHashTable<ttstr, tRisaTmpFSNode *>::tIterator i;
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
//! @brief		内容をシリアライズする
//! @param		dest 出力先ストリーム
//---------------------------------------------------------------------------
void tRisaTmpFSNode::Serialize(tRisseBinaryStream * dest) const
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
		tRisseHashTable<ttstr, tRisaTmpFSNode *>::tIterator i;
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
		volatile tRisaCriticalSection::tLocker  holder(File->GetCS());
		i64 = wxUINT64_SWAP_ON_BE(File->GetSize());
		dest->WriteBuffer(&i64, sizeof(i64));
		dest->WriteBuffer(File->GetBlock(), File->GetSize());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された名前を持つノードを返す
//! @param		name 名前
//! @return		ノード(ノードが見つからない場合は NULL)
//! @note		このノードが File を表す場合も NULL が返る
//---------------------------------------------------------------------------
tRisaTmpFSNode * tRisaTmpFSNode::GetSubNode(const ttstr & name)
{
	if(Type != ntDirectory) return NULL;

	tRisaTmpFSNode ** node = Directory->Find(name);
	if(!node) return NULL;
	return * node;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された名前を持つサブノードを削除する
//! @param		name 名前
//! @return		削除に成功すれば真
//---------------------------------------------------------------------------
bool tRisaTmpFSNode::DeleteSubNodeByName(const ttstr & name)
{
	if(Type != ntDirectory) return false;
	tRisaTmpFSNode * node = GetSubNode(name);
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
//! @param		name 名前
//! @return		新規に作成されたディレクトリノード
//! @note		すでにその名前を持つノードがあった場合は何もしないで NULL を返すので注意
//---------------------------------------------------------------------------
tRisaTmpFSNode * tRisaTmpFSNode::CreateDirectory(const ttstr & name)
{
	if(Type != ntDirectory) return false;
	if(GetSubNode(name)) return NULL; // すでにそこに何かがある
	tRisaTmpFSNode *newnode = new tRisaTmpFSNode(this, ntDirectory, name);
	Directory->Add(name, newnode);
	return newnode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された名前を持つファイルを作成する
//! @param		name 名前
//! @return		新規に作成されたファイルノード
//! @note		すでにその名前を持つノードがあった場合は何もしないで NULL を返すので注意
//---------------------------------------------------------------------------
tRisaTmpFSNode * tRisaTmpFSNode::CreateFile(const ttstr & name)
{
	if(Type != ntDirectory) return false;
	if(GetSubNode(name)) return NULL; // すでにそこに何かがある
	tRisaTmpFSNode *newnode = new tRisaTmpFSNode(this, ntFile, name);
	Directory->Add(name, newnode);
	return newnode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ノードのサイズを取得する
//! @return		ノードのサイズ (ファイルの場合はファイルサイズ、そうでない場合は 0)
//---------------------------------------------------------------------------
risse_size tRisaTmpFSNode::GetSize() const
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
size_t tRisaTmpFSNode::Iterate(tRisaFileSystemIterationCallback * callback)
{
	if(Type != ntDirectory) return 0;
	size_t count = 0;
	tRisseHashTable<ttstr, tRisaTmpFSNode *>::tIterator i;
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
//! @brief		シリアライズ時のファイルの先頭に着くマジック
//---------------------------------------------------------------------------
const unsigned char tRisaTmpFS::SerializeMagic[] = {
	't', 'm' , 'p', 'f', 's', 0x1a,
	0x00, // ファイルレイアウトバージョン
	0x00  // reserved
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaTmpFS::tRisaTmpFS()
{
	// 変数初期化
	CreateRoot();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaTmpFS::~tRisaTmpFS()
{
	RemoveRoot();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル一覧を取得する
//! @param		dirname ディレクトリ名
//! @param		callback コールバックオブジェクト
//! @return		取得できたファイル数
//---------------------------------------------------------------------------
size_t tRisaTmpFS::GetFileListAt(const ttstr & dirname,
	tRisaFileSystemIterationCallback * callback)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisaTmpFSNode * node = GetNodeAt(dirname);
	if(!node) tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsDirectory()) eRisaException::Throw(RISSE_WS_TR("specified name is not a directory"));

	return node->Iterate(callback);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルが存在するかどうかを得る
//! @param		filename ファイル名
//! @return		ファイルが存在する場合真
//---------------------------------------------------------------------------
bool tRisaTmpFS::FileExists(const ttstr & filename)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisaTmpFSNode * node = GetNodeAt(filename);
	if(!node) return false;
	if(!node->IsFile()) return false; // それがファイルではない場合も false
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリが存在するかどうかを得る
//! @param		dirname ディレクトリ名
//! @return		ディレクトリが存在する場合真
//---------------------------------------------------------------------------
bool tRisaTmpFS::DirectoryExists(const ttstr & dirname)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisaTmpFSNode * node = GetNodeAt(dirname);
	if(!node) return false;
	if(!node->IsDirectory()) return false; // それがディレクトリではない場合も false
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルを削除する
//! @param		filename ファイル名
//---------------------------------------------------------------------------
void tRisaTmpFS::RemoveFile(const ttstr & filename)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisaTmpFSNode * node = GetNodeAt(filename);
	if(!node) tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsFile()) eRisaException::Throw(RISSE_WS_TR("specified name is not a file"));

	// 親ノードから切り離す
	tRisaTmpFSNode * parent = node->GetParent();
	if(!parent) return; // parent が null なのは root ノードだけなのでこれはあり得ないが...
	parent->DeleteSubNodeByName(node->GetName());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを削除する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを削除するかどうか
//---------------------------------------------------------------------------
void tRisaTmpFS::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisaTmpFSNode * node = GetNodeAt(dirname);
	if(!node) tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError();
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
		tRisaTmpFSNode * parent = node->GetParent();
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
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを作成するかどうか
//---------------------------------------------------------------------------
void tRisaTmpFS::CreateDirectory(const ttstr & dirname, bool recursive)
{
	volatile tRisaCriticalSection::tLocker holder(CS);


	if(recursive)
	{
		// 再帰的にディレクトリを作成する場合
		// '/' で name を区切り、順に root からノードをたどっていく
		const risse_char * p = dirname.c_str();
		const risse_char *pp = p;

		tRisaTmpFSNode *node = Root;
		while(*p)
		{
			while(*p != RISSE_WC('/') && *p != 0) p++;
			if(p != pp)
			{
				// '/' で挟まれた区間が得られた
				tRisaTmpFSNode *parent = node;
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

		ttstr path(dirname);
		tRisaFileSystemManager::TrimLastPathDelimiter(path); // dirname の最後の '/' は取り去る
		ttstr parentdir, name;
		tRisaFileSystemManager::SplitPathAndName(path, &parentdir, &name); // パスを分離

		tRisaTmpFSNode * parentnode = GetNodeAt(parentdir);
		if(!parentnode) tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError();
		parentnode->CreateDirectory(name);
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルの stat を得る
//! @param		filename ファイル名
//! @param		struc stat 結果の出力先
//---------------------------------------------------------------------------
void tRisaTmpFS::Stat(const ttstr & filename, tRisaStatStruc & struc)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// XXX: MACタイムは現バージョンでは保存していない
	struc.Clear();

	tRisaTmpFSNode * node = GetNodeAt(filename);
	if(!node) tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	struc.Size = node->GetSize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルのストリームを得る
//! @param		filename ファイル名
//! @param		flags フラグ
//! @return		ストリームオブジェクト
//---------------------------------------------------------------------------
tRisseBinaryStream * tRisaTmpFS::CreateStream(const ttstr & filename, risse_uint32 flags)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisaTmpFSNode * node = GetNodeAt(filename);
	if(!node) tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsFile()) eRisaException::Throw(RISSE_WS_TR("specified name is not a file"));

	return new tRisaMemoryStream(flags, node->GetMemoryStreamBlockNoAddRef());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたストリームに内容をシリアライズする
//! @param		dest 出力先ストリーム
//---------------------------------------------------------------------------
void tRisaTmpFS::SerializeTo(tRisseBinaryStream * dest)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// マジックを書き込む
	dest->WriteBuffer(SerializeMagic, 8);

	// root に対して内容Serialize
	// (すると後は自動的に全てのノードがシリアライズされる)
	Root->Serialize(dest);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルに内容をシリアライズする
//! @param		filename 出力先ファイル名
//---------------------------------------------------------------------------
void tRisaTmpFS::SerializeTo(const ttstr & filename)
{
	std::auto_ptr<tRisseBinaryStream>
		stream(tRisaFileSystemManager::instance()->CreateStream(filename, RISSE_BS_WRITE));

	SerializeTo(stream.get());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたストリームから内容を復元する
//! @param		src 入力元ストリーム
//---------------------------------------------------------------------------
void tRisaTmpFS::UnserializeFrom(tRisseBinaryStream * src)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// マジックを読み込み、比較する
	unsigned char magic[8];
	src->ReadBuffer(magic, 8);

	unsigned char firstnodetype;
	src->ReadBuffer(&firstnodetype, 1);
		// 最初のノードタイプ(ディレクトリを表す 0x80 になってないとおかしい)

	if(memcmp(magic, SerializeMagic, 8) || firstnodetype != 0x80)
		eRisaException::Throw(RISSE_WS_TR("not a tmpfs archive"));

	// 再帰的に内容を読み込む
	Root = new tRisaTmpFSNode(NULL, tRisaTmpFSNode::ntDirectory, src);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルから内容を復元する
//! @param		filename 入力元ファイル
//---------------------------------------------------------------------------
void tRisaTmpFS::UnserializeFrom(const ttstr & filename)
{
	std::auto_ptr<tRisseBinaryStream>
		stream(tRisaFileSystemManager::instance()->CreateStream(filename, RISSE_BS_READ));

	UnserializeFrom(stream.get());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された位置のノードを得る
//! @param		name ノード
//! @return		その位置にあるノード。その位置が見つからない場合は NULL
//---------------------------------------------------------------------------
tRisaTmpFSNode * tRisaTmpFS::GetNodeAt(const ttstr & name)
{
	// '/' で name を区切り、順に root からノードをたどっていく
	const risse_char * p = name.c_str();
	const risse_char *pp = p;

	tRisaTmpFSNode *node = Root;

	while(*p)
	{
		while(*p != RISSE_WC('/') && *p != 0) p++;
		if(p != pp)
		{
			// '/' で挟まれた区間が得られた
			node = node->GetSubNode(ttstr(p, p - pp));
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
//! @brief		ルートディレクトリを作成する
//---------------------------------------------------------------------------
void tRisaTmpFS::CreateRoot()
{
	if(Root) return;

	// ルートノードを作成
	Root = new tRisaTmpFSNode(NULL, tRisaTmpFSNode::ntDirectory, ttstr());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ルートディレクトリを削除する
//---------------------------------------------------------------------------
void tRisaTmpFS::RemoveRoot()
{
	// root ノードを削除
	delete Root, Root = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		内容をすべてクリアする
//---------------------------------------------------------------------------
void tRisaTmpFS::Clear()
{
	RemoveRoot();
	CreateRoot();
}
//---------------------------------------------------------------------------

