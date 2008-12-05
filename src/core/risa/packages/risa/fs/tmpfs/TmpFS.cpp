//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tmpfs の実装
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/fs/tmpfs/TmpFS.h"
#include "risa/common/RisaException.h"


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
	tStreamAdapter src) :
	Parent(parent),
	Type(type)
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
		src.ReadBuffer(&metadataid, 1);

		if(metadataid == 0) break; // メタデータの終わりの場合はループをぬける

		wxUint32 metalen;
		src.ReadBuffer(&metalen, sizeof(metalen));
		metalen = wxUINT32_SWAP_ON_BE(metalen);
		if(metalen == 0)
			tIOExceptionClass::Throw(RISSE_WS_TR("invalid metadata length (data may be corrupted)"));

		if(metadataid == 0x01)
		{
			// ファイル名
			unsigned char * p = new (PointerFreeGC) unsigned char [metalen];
			src.ReadBuffer(p, metalen);
			p[metalen - 1] = 0;
			Name = tString(wxString(reinterpret_cast<char*>(p), wxConvUTF8));
		}
		else
		{
			// 未知のメタデータなので読み飛ばす
			src.SetPosition(src.GetPosition() + metalen);
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
			src.ReadBuffer(&nodetypeid, 1);
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
				tIOExceptionClass::Throw(tString(
						wxString::Format(RISSE_WS_TR("unsupported node id %x"),
						static_cast<int>(nodetypeid))));
			}
		}
	}
	else if(Type == ntFile)
	{
		// ファイル
		wxUint64 blocksize;
		src.ReadBuffer(&blocksize, sizeof(blocksize));
		blocksize = wxUINT64_SWAP_ON_BE(blocksize);
		if(static_cast<size_t>(blocksize) != blocksize)
				tIOExceptionClass::Throw(RISSE_WS_TR("too big block size"));
		File->ChangeSize(static_cast<size_t>(blocksize));
		File->Fit();
		src.ReadBuffer(File->GetBlock(), static_cast<size_t>(blocksize));
	}

	// 親に自分を登録
	if(Parent)
		Parent->Directory->Add(Name, this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSNode::Serialize(tStreamAdapter dest) const
{
	// ノードのタイプを記録
	if(Type == ntDirectory)
		dest.WriteBuffer("\x80", 1); // ディレクトリを表す
	else if(Type == ntFile)
		dest.WriteBuffer("\x81", 1); // ファイルを表す

	// 名前を格納
	dest.WriteBuffer("\1", 1); // ファイル名

	wxCharBuffer utf8name = Name.AsWxString().mb_str(wxConvUTF8);
	size_t utf8name_len = strlen(utf8name);

	wxUint32 i32;
	i32 = wxUINT32_SWAP_ON_BE(static_cast<wxUint32>(utf8name_len + 1));
	dest.WriteBuffer(&i32, sizeof(i32));
	dest.WriteBuffer(utf8name, utf8name_len + 1);

	if(Type == ntDirectory)
	{
		// ディレクトリ
		dest.WriteBuffer("\0", 1); // メタデータの終わりとディレクトリの開始

		// 全ての子要素に対して再帰する
		tHashTable<tString, tTmpFSNode *>::tIterator i(*Directory);
		for(; !i.End(); i++)
		{
			i.GetValue()->Serialize(dest);
		}

		dest.WriteBuffer("\x88", 1); // ディレクトリの終わりを表す
	}
	else if(Type == ntFile)
	{
		// ファイル
		dest.WriteBuffer("\x0", 1); // メタデータの終わりとファイルの中身の開始
		wxUint64 i64;
		volatile tCriticalSection::tLocker  holder(File->GetCS());
		i64 = wxUINT64_SWAP_ON_BE(File->GetSize());
		dest.WriteBuffer(&i64, sizeof(i64));
		dest.WriteBuffer(File->GetBlock(), File->GetSize());
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

	return Directory->Delete(name);
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
size_t tTmpFSNode::Iterate(tVariant & callback)
{
	if(Type != ntDirectory) return 0;
	size_t count = 0;
	tHashTable<tString, tTmpFSNode *>::tIterator i(*Directory);
	for(; !i.End(); i++)
	{
		count ++;
		if(i.GetValue()->Type == ntDirectory)
		{
			callback.Do(tRisseScriptEngine::instance()->GetScriptEngine(),
				ocFuncCall, NULL, tString::GetEmptyString(), 0,
				tMethodArgument::New(i.GetValue()->Name, true));
		}
		else if(i.GetValue()->Type == ntFile)
		{
			callback.Do(tRisseScriptEngine::instance()->GetScriptEngine(),
				ocFuncCall, NULL, tString::GetEmptyString(), 0,
				tMethodArgument::New(i.GetValue()->Name, false));
		}
	}
	return count;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
const unsigned char tTmpFSInstance::SerializeMagic[] = {
	't', 'm' , 'p', 'f', 's', 0x1a,
	0x00, // ファイルレイアウトバージョン
	0x00  // reserved
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTmpFSInstance::tTmpFSInstance()
{
	// 変数初期化
	CreateRoot();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSInstance::SerializeTo(tStreamAdapter dest)
{
	volatile tSynchronizer sync(this); // sync

	// マジックを書き込む
	dest.WriteBuffer(SerializeMagic, 8);

	// root に対して内容Serialize
	// (すると後は自動的に全てのノードがシリアライズされる)
	Root->Serialize(dest);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSInstance::UnserializeFrom(tStreamAdapter src)
{
	volatile tSynchronizer sync(this); // sync

	// マジックを読み込み、比較する
	unsigned char magic[8];
	src.ReadBuffer(magic, 8);

	unsigned char firstnodetype;
	src.ReadBuffer(&firstnodetype, 1);
		// 最初のノードタイプ(ディレクトリを表す 0x80 になってないとおかしい)

	if(memcmp(magic, SerializeMagic, 8) || firstnodetype != 0x80)
		tIOExceptionClass::Throw(RISSE_WS_TR("not a tmpfs archive"));

	// 再帰的に内容を読み込む
	Root = new tTmpFSNode(NULL, tTmpFSNode::ntDirectory, src);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTmpFSNode * tTmpFSInstance::GetNodeAt(const tString & name)
{
	// '/' で name を区切り、順に root からノードをたどっていく
	const risse_char * p = name.Pointer();
	const risse_char * p_start = p;
	const risse_char *pp = p;
	const risse_char * p_limit = p + name.GetLength();

	tTmpFSNode *node = Root;

	while(p < p_limit)
	{
		while(*p != RISSE_WC('/') && p < p_limit) p++;
		if(p != pp)
		{
			// '/' で挟まれた区間が得られた
			node = node->GetSubNode(tString(name, pp - p_start, p - pp));
			if(!node) return NULL;
		}
		p ++; // skip '/'
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
void tTmpFSInstance::CreateRoot()
{
	if(Root) return;

	// ルートノードを作成
	Root = new tTmpFSNode(NULL, tTmpFSNode::ntDirectory, tString());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSInstance::Clear()
{
	Root = NULL;
	CreateRoot();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void tTmpFSInstance::construct()
{
	// とくにやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tTmpFSInstance::walkAt(const tString & dirname,
		const tMethodArgument &args)
{
	volatile tSynchronizer sync(this); // sync

	args.ExpectBlockArgumentCount(1); // ブロック引数が一つなければならない
	tVariant callback(args.GetBlockArgument(0));

	tTmpFSNode * node = GetNodeAt(dirname);
	if(!node) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsDirectory()) tIOExceptionClass::Throw(RISSE_WS_TR("specified name is not a directory"));

	return node->Iterate(callback);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tTmpFSInstance::isFile(const tString & filename)
{
	volatile tSynchronizer sync(this); // sync

	tTmpFSNode * node = GetNodeAt(filename);
	if(!node) return false;
	if(!node->IsFile()) return false; // それがファイルではない場合も false
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tTmpFSInstance::isDirectory(const tString & dirname)
{
	volatile tSynchronizer sync(this); // sync

	tTmpFSNode * node = GetNodeAt(dirname);
	if(!node) return false;
	if(!node->IsDirectory()) return false; // それがディレクトリではない場合も false
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSInstance::removeFile(const tString & filename)
{
	volatile tSynchronizer sync(this); // sync

	tTmpFSNode * node = GetNodeAt(filename);
	if(!node) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsFile()) tIOExceptionClass::Throw(RISSE_WS_TR("specified name is not a file"));

	// 親ノードから切り離す
	tTmpFSNode * parent = node->GetParent();
	if(!parent) return; // parent が null なのは root ノードだけなのでこれはあり得ないが...
	parent->DeleteSubNodeByName(node->GetName());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSInstance::removeDirectory(const tString & dirname,
		const tMethodArgument &args)
{
	volatile tSynchronizer sync(this); // sync

	tTmpFSNode * node = GetNodeAt(dirname);
	if(!node) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();
	if(!node->IsDirectory()) tIOExceptionClass::Throw(RISSE_WS_TR("specified name is not a directory"));

	bool recursive = args.HasArgument(1) ? (bool)args[1] : false;

	if(!recursive)
	{
		// recursive が false の場合、消そうとしたディレクトリが空で無ければ
		// 失敗する。

		if(node->HasSubNode()) tIOExceptionClass::Throw(RISSE_WS_TR("specified directory is not empty"));

		// root ノードは消すことはできない
		if(node == Root) tIOExceptionClass::Throw(RISSE_WS_TR("can not remove file system root directory"));
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
void tTmpFSInstance::createDirectory(const tString & dirname)
{
	volatile tSynchronizer sync(this); // sync

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
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectInterface * tTmpFSInstance::stat(const tString & filename)
{
	volatile tSynchronizer sync(this); // sync

	tIOExceptionClass::Throw(tString(RISSE_WS_TR("stat is not yet implemented")));

	return NULL;
/*
	// XXX: MACタイムは現バージョンでは保存していない
	struc.Clear();

	tTmpFSNode * node = GetNodeAt(filename);
	if(!node) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	struc.Size = node->GetSize();
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStreamInstance * tTmpFSInstance::open(const tString & filename,
		risse_uint32 flags)
{
	volatile tSynchronizer sync(this); // sync

	tTmpFSNode * node;

	if((flags & tFileOpenModes::omAccessMask) == tFileOpenModes::omWrite)
	{
		// もしノードが存在しなければ新規作成する
		tString path(filename);
		tString parentdir, name;
		tFileSystemManager::SplitPathAndName(path, &parentdir, &name); // パスを分離
		tTmpFSNode * parentnode = GetNodeAt(parentdir);
		if(!parentnode) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();
		node = parentnode->CreateFile(name);
		if(!node) tIOExceptionClass::Throw(RISSE_WS_TR("failed to create file"));
	}
	else
	{
		node = GetNodeAt(filename);
		if(!node) tFileSystemManager::RaiseNoSuchFileOrDirectoryError();
		if(!node->IsFile()) tIOExceptionClass::Throw(RISSE_WS_TR("specified name is not a file"));
	}

	// MemoryStreamClass からインスタンスを生成して返す
	tVariant obj =
		tClassHolder<tMemoryStreamClass>::instance()->GetClass()->
			Invoke(ss_new, (risse_int64)flags, true);
				// 第２引数の true は、ストリームになんらメモリブロックがアタッチ
				// されずにストリームが作成されることを表す(下でアタッチする)
	obj.AssertClass(tClassHolder<tMemoryStreamClass>::instance()->GetClass());
	tMemoryStreamInstance *memstream = 
		static_cast<tMemoryStreamInstance *>(obj.GetObjectInterface());

	memstream->SetMemoryBlock(node->GetMemoryStreamBlock()); // ここでアタッチ

	if(flags & tFileOpenModes::omAppendBit)
	{
		// 最後に移動
		memstream->SeekEnd();
	}

	return memstream;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSInstance::flush()
{
	// やることなし
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSInstance::save(const tVariant & filename)
{
	tStreamInstance * stream = tFileSystemManager::instance()->Open(filename, tFileOpenModes::omWrite);

	SerializeTo(tStreamAdapter(stream));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSInstance::load(const tVariant & filename)
{
	tStreamInstance * stream = tFileSystemManager::instance()->Open(filename, tFileOpenModes::omRead);

	UnserializeFrom(tStreamAdapter(stream));
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tTmpFSClass::tTmpFSClass(tScriptEngine * engine) :
	tClassBase(tSS<'T','m','p','F','S'>(),
		tClassHolder<tFileSystemClass>::instance()->GetClass())
{
	MemoryStreamClass = new tMemoryStreamClass(engine);

	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTmpFSClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tTmpFSClass::ovulate);
	BindFunction(this, ss_construct, &tTmpFSInstance::construct);
	BindFunction(this, ss_initialize, &tTmpFSInstance::initialize);

	BindFunction(this, tSS<'w','a','l','k','A','t'>(), &tTmpFSInstance::walkAt);
	BindFunction(this, tSS<'i','s','F','i','l','e'>(), &tTmpFSInstance::isFile);
	BindFunction(this, tSS<'i','s','D','i','r','e','c','t','o','r','y'>(), &tTmpFSInstance::isDirectory);
	BindFunction(this, tSS<'r','e','m','o','v','e','F','i','l','e'>(), &tTmpFSInstance::removeFile);
	BindFunction(this, tSS<'r','e','m','o','v','e','D','i','r','e','c','t','o','r','y'>(), &tTmpFSInstance::removeDirectory);
	BindFunction(this, tSS<'c','r','e','a','t','e','D','i','r','e','c','t','o','r','y'>(), &tTmpFSInstance::createDirectory);
	BindFunction(this, tSS<'s','t','a','t'>(), &tTmpFSInstance::stat);
	BindFunction(this, tSS<'o','p','e','n'>(), &tTmpFSInstance::open);
	BindFunction(this, tSS<'f','l','u','s','h'>(), &tTmpFSInstance::flush);

	BindFunction(this, tSS<'s','a','v','e'>(), &tTmpFSInstance::save);
	BindFunction(this, tSS<'l','o','a','d'>(), &tTmpFSInstance::load);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tTmpFSClass::ovulate()
{
	return tVariant(new tTmpFSInstance());
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
/**
 * TmpFS クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','f','s'>,
	tTmpFSClass>;
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
} // namespace Risa

