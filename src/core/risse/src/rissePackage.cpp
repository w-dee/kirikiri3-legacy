//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief パッケージ管理
//---------------------------------------------------------------------------

#include "prec.h"
#include "rissePackage.h"
#include "risseThread.h"
#include "risseStringTemplate.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"
#include "risseStaticStrings.h"
#include "risseBindingInfo.h"
#include "risseArrayClass.h"
#include "risseExceptionClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(58978,40915,31180,20110,43417,63480,33374,56600);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		ダミーのイニシャライザ
//---------------------------------------------------------------------------
class tDummyBuiltinPackageInitializer : public tBuiltinPackageInitializerInterface
{
public:
	virtual void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global) {;}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		組み込みパッケージ用の仮想的なファイルシステム
//! @note		組み込みパッケージに対する要求でなければユーザプログラム指定の
//!				ファイルシステムインターフェースを呼び出す機構も持つ
//---------------------------------------------------------------------------
class tBuiltinPackageFileSystem : public tCollectee
{
	//! @brief		ディレクトリのノード
	struct tNode : public tCollectee
	{
		typedef gc_map<tString, tNode *> tChildren;
		tString Name; //!< 名前
		tChildren Children; //!< 子アイテムのリスト
		tBuiltinPackageInitializerInterface * Initializer; //!< パッケージ初期化用インターフェース
		tNode() { Initializer = NULL; }
		bool IsFile() const { return Initializer != NULL; }
	};

	tNode Root; //!< ルートノード
	tScriptEngine * ScriptEngine; //!< スクリプトエンジンインスタンス
	tDummyBuiltinPackageInitializer DummyBuiltinPackageInitializer;
		//!< ダミーのパッケージイニシャライザ

public:
	//! @brief			コンストラクタ
	//! @param			engine		スクリプトエンジンインスタンス
	tBuiltinPackageFileSystem(tScriptEngine * engine) { ScriptEngine = engine; }


	//! @brief		指定ディレクトリにあるファイル名をすべて列挙する
	//! @param		dir		ディレクトリ(区切りには '/' が用いられる)
	//! @param		files	そこにあるファイル/ディレクトリ名一覧
	//! @note		. で始まるディレクトリやファイル,隠しファイルは含めなくて良い。
	//!				ディレクトリの場合はfilesの最後を '/' で終わらせること。
	//!				files は呼び出し側で最初に clear() しておくこと。
	void List(const tString & dir, gc_vector<tString> & files)
	{
		if(!dir.StartsWith(tSS<'[','*','b','u','i','l','t','i','n','*',']'>()))
		{
			tPackageFileSystemInterface * drain = ScriptEngine->GetPackageFileSystem();
			if(drain) drain->List(dir, files);
			return;
		}

		tNode * node = GetNodeAt(tString(dir, 11)); // 11 = length of "[*builtin*]"
		if(!node) return; // ノードが見つからない
		if(node->IsFile()) return; // それファイル
		for(tNode::tChildren::iterator i = node->Children.begin();
			i != node->Children.end(); i++)
		{
			if(i->second->IsFile())
				files.push_back(i->first);
			else
				files.push_back(i->first + tSS<'/'>());
		}
	}

	//! @brief		ファイル種別を得る
	//! @param		file	ファイル名
	//! @return		種別(0=ファイルが存在しない, 1=ファイル, 2=ディレクトリ)
	int GetType(const tString & file)
	{
		if(!file.StartsWith(tSS<'[','*','b','u','i','l','t','i','n','*',']'>()))
		{
			tPackageFileSystemInterface * drain = ScriptEngine->GetPackageFileSystem();
			if(drain) return drain->GetType(file);
			return 0;
		}

		tNode * node = GetNodeAt(tString(file, 11)); // 11 = length of "[*builtin*]"
		if(!node) return 0; // ノードが見つからない
		return node->IsFile() ? 1 : 2;
	}

	//! @brief		パッケージを初期化する
	//! @param		file		パッケージのありそうなファイル名
	//! @param		package		パッケージ名
	//! @param		global		パッケージグローバル
	void Initialize(
		const tString & file,
		const tString & package,
		const tVariant & global)
	{
		if(!file.StartsWith(tSS<'[','*','b','u','i','l','t','i','n','*',']'>()))
		{
			tPackageFileSystemInterface * drain = ScriptEngine->GetPackageFileSystem();
			if(drain)
			{
				// ファイルを読み込む
				tString content = drain->ReadFile(file);

				// そのファイルをパッケージ内で実行する
				tBindingInfo bind(global, global);
				ScriptEngine->Evaluate(content, file, 0, NULL, &bind, false);
			}
			return;
		}
		else
		{
			// Initializer を呼ぶ
			tNode * node = GetNodeAt(tString(file, 11)); // 11 = length of "[*builtin*]"
			if(!node) return; // ノードが見つからない
			if(!node->IsFile()) return; // ファイルではない
			node->Initializer->Initialize(ScriptEngine, package, global);
		}
	}

	//! @brief		組み込みパッケージを登録する
	//! @param		name	スラッシュ区切りのディレクトリ/ファイル名
	//!						(最初の '/' や重複する '/' は含めないこと; 例: 'k3/graphics/rina' )
	//! @param		init	パッケージ初期化用インターフェース
	void AddPackage(const tString & name, tBuiltinPackageInitializerInterface * init)
	{
		// name をスラッシュごとに split し、tNode に登録していく
		tString::tSplitter splitter(name + tSS<'.','r','s'>(), RISSE_WC('/'));
		tNode * current = &Root;
		tString token;
		while(splitter(token))
		{
			if(token.IsEmpty()) continue;
			tNode::tChildren::iterator f = current->Children.find(token);
			if(f == current->Children.end())
			{
				// ディレクトリが見つからなかった
				// そこにディレクトリを作る
				f = current->Children.insert(
					tNode::tChildren::value_type(token, new tNode())).first;
			}
			RISSE_ASSERT(f->second->Initializer == NULL);
			// 次へ
			current = f->second;
		}
		// 末尾のノードの initializer に init を設定する
		current->Initializer = init;
	}

	//! @brief		ダミーのパッケージイニシャライザをとってくる
	tBuiltinPackageInitializerInterface * GetDummyBuiltinPackageInitializer()
		{ return &DummyBuiltinPackageInitializer; }

private:
	//! @brief		指定の名前を持つノードを検索する
	//! @param		name		名前
	//! @return		そのノード(null=見つからない)
	tNode * GetNodeAt(const tString & name)
	{
		tString::tSplitter splitter(name, RISSE_WC('/'));
		tNode * current = &Root;
		tString token;
		while(splitter(token))
		{
			if(token.IsEmpty()) continue;
			tNode::tChildren::iterator f = current->Children.find(token);
			if(f == current->Children.end()) return NULL;
			current = f->second;
		}
		return current;
	}
};
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
tPackageManager::tPackageManager(tScriptEngine * script_engine)
{
	ScriptEngine = script_engine;

	CS = new tCriticalSection();
	BuiltinPackageFileSystem = new tBuiltinPackageFileSystem(script_engine);

	// 組み込みパッケージ "risse" と デフォルトのパッケージ "main" を組み込み
	// パッケージ用の仮想ファイルシステムに追加
	BuiltinPackageFileSystem->AddPackage(tSS<'r','i','s','s','e'>(),
		BuiltinPackageFileSystem->GetDummyBuiltinPackageInitializer());
	BuiltinPackageFileSystem->AddPackage(tSS<'m','a','i','n'>(),
		BuiltinPackageFileSystem->GetDummyBuiltinPackageInitializer());

	// "risse" パッケージを作成する
	RissePackageGlobal = InitPackage(tString(), tSS<'r','i','s','s','e'>());

	// "risse" パッケージに、パッケージ検索パスを表す "packagePath" を追加
	tVariant packagePath = tVariant(ScriptEngine->ArrayClass).New();
	RissePackageGlobal.SetPropertyDirect_Object(
				tSS<'p','a','c','k','a','g','e','P','a','t','h'>(),
				tOperateFlags::ofMemberEnsure|
					tOperateFlags::ofInstanceMemberOnly|
					tOperateFlags::ofUseClassMembersRule,
				packagePath);

	// packagePath に組み込みパッケージ用の仮想ファイルシステムを追加
	packagePath.Invoke_Object(tSS<'p','u','s','h'>(),
		tString(tSS<'[','*','b','u','i','l','t','i','n','*',']'>()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPackageManager::GetPackageGlobal(const tString & name)
{
	tCriticalSection::tLocker lock(*CS); // sync

	// package に対応するパッケージのファイル名を得る
	gc_vector<tString> filenames;
	gc_vector<tString> locations;
	tVariant package_global = SearchPackage(name, filenames, locations);
	if(package_global.IsVoid())
	{
		// パッケージは少なくとも初期化済みではない
		if(filenames.size() == 0)
		{
			// パッケージが見つからない
			tImportExceptionClass::ThrowPackageNotFound(ScriptEngine, name);
		}

		RISSE_ASSERT(filenames.size() == 1);

		package_global = InitPackage(filenames[0], locations[0]);
	}

	return package_global;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPackageManager::AddBuiltinPackage(const tString & package,
	tBuiltinPackageInitializerInterface * init)
{
	tCriticalSection::tLocker lock(*CS); // sync

	BuiltinPackageFileSystem->AddPackage(package, init);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPackageManager::DoImport(tVariant & dest, const tVariant & packages)
{
	tCriticalSection::tLocker lock(*CS); // sync

	// プリミティブ型インスタンスのコンテキストにはインポートできない
	if(dest.GetType() != tVariant::vtObject)
		tImportExceptionClass::ThrowCannotImportIntoPrimitiveInstanceContext(ScriptEngine);

	// packages はインポートするパッケージを表す辞書配列の配列
	tVariant::tSynchronizer sync_dest(dest); // sync
	tVariant::tSynchronizer sync_packages(packages); // sync

	risse_size list_count =
			(risse_int64)packages.GetPropertyDirect(ScriptEngine, ss_length);
	for(risse_size i = 0; i < list_count; i++)
	{
		tVariant dic =
			packages.Invoke(ScriptEngine, tSS<'[',']'>(), tVariant((risse_int64)i));

		// package を得る
		tVariant package_loc =
			dic.Invoke(ScriptEngine, tSS<'[',']'>(), tVariant(tSS<'p','a','c','k','a','g','e'>()));

		// as を得る
		tVariant as =
			dic.Invoke(ScriptEngine, tSS<'[',']'>(), tVariant(tSS<'a','s'>()));

		// package_loc に対応するパッケージのファイル名を得る
		gc_vector<tString> filenames;
		gc_vector<tString> locations;
		tVariant package_global = SearchPackage(package_loc, filenames, locations);
		if(package_global.IsVoid())
		{
			// パッケージは少なくとも初期化済みではない

			if(filenames.size() == 0)
			{
				// パッケージが見つからない
				tImportExceptionClass::ThrowPackageNotFound(
					ScriptEngine,
					package_loc.Invoke(ScriptEngine,
						tSS<'j','o','i','n'>(), tVariant(tSS<'.'>())));
			}

			// 対応するパッケージを順に読み込む
			for(risse_size j = 0; j < locations.size(); j++)
			{
				package_global = InitPackage(filenames[j], locations[j]);
				// もし as が指定されている場合はそれを as に、
				// そうでない場合は locations[j] で指定された位置に dig して書き込む
				if(as.IsVoid())
					Dig(dest, locations[j], package_global); // as の指定無し
				else
					Dig(dest, as, package_global); // as の指定あり
			}
		}
		else
		{
			// パッケージは初期化済み

			// もし as が指定されている場合はそれを as に、
			// そうでない場合は package_loc で指定された位置に dig して書き込む
			if(as.IsVoid())
				Dig(dest, package_loc, package_global); // as の指定無し
			else
				Dig(dest, as, package_global); // as の指定あり
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPackageManager::DoImport(tVariant & dest,
	const tVariant & packages, const tVariant & ids)
{
	tCriticalSection::tLocker lock(*CS); // sync

	// プリミティブ型インスタンスのコンテキストにはインポートできない
	if(dest.GetType() != tVariant::vtObject)
		tImportExceptionClass::ThrowCannotImportIntoPrimitiveInstanceContext(ScriptEngine);

	// packages はインポートするパッケージを表す配列の配列
	tVariant::tSynchronizer sync_dest(dest); // sync
	tVariant::tSynchronizer sync_packages(packages); // sync
	tVariant::tSynchronizer sync_ids(ids); // sync

	gc_vector<tVariant> globals; // パッケージグローバルの配列

	// 列挙されたパッケージをとりあえず全て初期化する
	risse_size list_count =
			(risse_int64)packages.GetPropertyDirect(ScriptEngine, ss_length);
	for(risse_size i = 0; i < list_count; i++)
	{
		tVariant package_loc =
			packages.Invoke(ScriptEngine, tSS<'[',']'>(), tVariant((risse_int64)i));

		// package_loc に対応するパッケージのファイル名を得る
		gc_vector<tString> filenames;
		gc_vector<tString> locations;
		tVariant package_global = SearchPackage(package_loc, filenames, locations);
		if(package_global.IsVoid())
		{
			// パッケージは少なくとも初期化済みではない

			if(filenames.size() == 0)
			{
				// パッケージが見つからない
				tImportExceptionClass::ThrowPackageNotFound(
					ScriptEngine,
					package_loc.Invoke(ScriptEngine,
						tSS<'j','o','i','n'>(), tVariant(tSS<'.'>())));
			}

			// 対応するパッケージを順に読み込む
			for(risse_size j = 0; j < locations.size(); j++)
			{
				package_global = InitPackage(filenames[j], locations[j]);
				globals.push_back(package_global);
			}
		}
		else
		{
			// パッケージは初期化済み
			globals.push_back(package_global);
		}
	}

	// それぞれのパッケージから識別子をインポートする
	// ids が true の場合は すべての識別子をインポートすることになる
	bool import_all = (ids.GetType() == tVariant::vtBoolean) && ids.operator bool();
	for(gc_vector<tVariant>::iterator i = globals.begin(); i != globals.end(); i++)
		ImportIds(*i, dest, import_all ? NULL : &ids);

	// インポートできなかったものは ids に残っているはずなのでそれをチェック
	if(!import_all)
	{
		gc_vector<tString> not_imported;

		// 辞書配列の eachPair() のコールバックを受けるためのインターフェース
		class tCallback : public tObjectInterface
		{
			gc_vector<tString> & List;
		public:
			tCallback(gc_vector<tString> & list) : List(list) {}
			tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
			{
				if(code == ocFuncCall && name.IsEmpty())
				{
					// このオブジェクトに対する関数呼び出し
					args.ExpectArgumentCount(2);

					List.push_back(args[0].operator tString());
				}
				return rvNoError;
			}
		} cb(not_imported);

		// メタデータを作成するため dict の eachPair を呼び出す
		tMethodArgument & args = tMethodArgument::Allocate(0, 1);
		tVariant block_arg0(&cb);
		args.SetBlockArgument(0, &block_arg0);
		tVariant(ids).Do(ScriptEngine, ocFuncCall, NULL,
			tSS<'e','a','c','h','P','a','i','r'>(), 0, args);

		// もし残りがあるようならば例外を送出
		if(not_imported.size() > 0)
			tImportExceptionClass::ThrowCannotImportIds(ScriptEngine, not_imported);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPackageManager::InitPackage(const tString & filename, const tString & name)
{
	// パッケージを探す
	tVariant package_global;
	tMap::iterator i = Map.find(name);
	if(i == Map.end())
	{
		// パッケージが見つからなかったよ
		// 仮にそこには void をつっこんでおく
		tMap::iterator f = Map.insert(tMap::value_type(name, tVariant())).first;

		// 新しくパッケージグローバルを作成
		package_global = tVariant(ScriptEngine->ObjectClass).New();

		// 新しい global には "risse" パッケージの中身をすべて import する
		// "risse" パッケージを作るときだけはさすがにこれはできない。
		// 初回は RissePackageGlobal は void のはず………
		if(!RissePackageGlobal.IsVoid())
			ImportIds(RissePackageGlobal, package_global, NULL);

		// パッケージを読み込んで初期化する
		if(!filename.IsEmpty())
			BuiltinPackageFileSystem->Initialize(
				filename, name, package_global);

		// Map のそこを上書きする
		f->second = package_global;
	}
	else
	{
		package_global = i->second;
		if(package_global.IsVoid())
		{
			// パッケージはいま初期化中
			tImportExceptionClass::ThrowPackageIsBeingInitialized(
				ScriptEngine, name);
		}
	}

	return package_global;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPackageManager::ImportIds(const tVariant & from, const tVariant & to,
		const tVariant * ids)
{
	// TODO: ローカル namespace へのimport………は無理か
	// せめて this への import ですかね。グローバル位置で import すると this は
	// 自動的に global になるので global への import という意味に自動的になる

	RISSE_ASSERT(!ids || ids->GetType() == tVariant::vtObject);

	// from と to をロックする
	tVariant::tSynchronizer sync_from(from);
	tVariant::tSynchronizer sync_to(to);

	// from にある識別子を列挙する
	RISSE_ASSERT(from.GetType() == tVariant::vtObject);
	tObjectBase * from_objectbase = static_cast<tObjectBase*>(from.GetObjectInterface());

	class callback : public tObjectBase::tEnumMemberCallback
	{
		tPackageManager * Manager;
		tVariant To;
		const tVariant * Ids;
	public:
		callback(tPackageManager * manager,
			const tVariant & to,
			const tVariant * ids) : Manager(manager), To(to), Ids(ids) {}

		bool OnEnum(const tString & name,
			const tObjectBase::tMemberData & data)
		{
			if(data.Attribute.GetAccess() != tMemberAttribute::acPublic)
				return true; // public なメンバのみ

			tVariant as;

			if(Ids)
			{
				// インポートする識別子が限定されている場合
				// (対応するnameが無ければasはvoidになる)
				as = Ids->Invoke_Object(mnIDelete, tVariant(name));
			}
			else
			{
				// インポートする識別子が限定されていない場合
				as = tVariant(name); // 同じ名前でインポートを行う
			}

			if(!as.IsVoid())
			{
				// インポートする場合
				if(as.GetType() == tVariant::vtString)
				{
					// 文字列指定
					Manager->Dig(To, as.operator tString(), data.Value, data.Attribute);
				}
				else
				{
					// 配列指定とみなす
					Manager->Dig(To, as, data.Value, data.Attribute);
				}
			}

			return true;
		}
	} cb(this, to, ids);
	from_objectbase->Enumurate(&cb);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPackageManager::SearchPackage(const tVariant & name,
				gc_vector<tString> & filenames,
				gc_vector<tString> & packages)
{
	// パッケージファイルシステムインターフェースを得る

	// name を '/' や '.' で連結しつつ、ワイルドカードがあるかどうかを調べる
	tVariant::tSynchronizer sync_name(name); // sync
	bool wildcard_found = false;

	tString package_fs_loc;
	tString package_loc;
	risse_size component_count =
		(risse_int64)name.GetPropertyDirect(ScriptEngine, ss_length);
	for(risse_size i = 0; i < component_count; i++)
	{
		tString loc_component =
			name.Invoke(ScriptEngine, tSS<'[',']'>(), tVariant((risse_int64)i));
		if(loc_component == tSS<'*'>())
		{
			// ワイルドカードが見つかった
			wildcard_found = true;
			break;
		}
		if(i != 0) package_fs_loc += tSS<'/'>(), package_loc += tSS<'.'>();
		package_fs_loc += loc_component;
		package_loc += loc_component;
	}

	// それが既に初期化済みか？
	if(!wildcard_found)
	{
		tMap::iterator i = Map.find(package_loc);
		if(i != Map.end())
		{
			// この時点で i->second はパッケージグローバルだが、
			// void が入ってることもあるので注意。void とは
			// 対象パッケージが現在初期化中であることを表すが、
			// とりあえずその場合はハンドリングしない。
			if(!i->second.IsVoid()) return i->second;
		}
	}

	// パス変数を得る
	tVariant path = RissePackageGlobal.GetPropertyDirect(
						ScriptEngine,
						tSS<'p','a','c','k','a','g','e','P','a','t','h'>());
	tVariant::tSynchronizer sync_path(path); // sync
	risse_size path_count =
		(risse_int64)path.GetPropertyDirect(ScriptEngine, ss_length);

	// パスを探す
	for(risse_size i = 0; i < path_count; i++)
	{
		tString path_dir =
			path.Invoke(ScriptEngine, tSS<'[',']'>(), tVariant((risse_int64)i));
		tString filename = path_dir + tSS<'/'>() + package_fs_loc;
		// ワイルドカードを使うかどうかで分岐
		if(wildcard_found)
		{
			// ワイルドカードを使う場合
			if(BuiltinPackageFileSystem->GetType(filename) != 2)
					continue; // ディレクトリじゃないので次へ

			// パスにあるファイルを得る
			gc_map<tString, tString> path_map;
			gc_vector<tString> files;
			tString dirname = path_dir + tSS<'/'>() + package_fs_loc;
			BuiltinPackageFileSystem->List(dirname, files);

			// files を順に見る
			for(gc_vector<tString>::iterator i = files.begin();
				i != files.end(); i++)
			{
				// 先頭が _ で始まるファイルやディレクトリはここでフィルタする
				if(i->StartsWith(RISSE_WC('_'))) continue;

				// パッケージのbasenameを求める
				bool is_dir = i->EndsWith(RISSE_WC('/'));
				tString basename = is_dir ? tString(*i, 0, i->GetLength() - 1) : *i;

				if(!is_dir)
				{
					if(!basename.EndsWith(tSS<'.','r','s'>()))
						continue; // ファイルの場合は末尾が .rs でない場合ははじく
					basename = tString(basename, 0, basename.GetLength() - 3);
						// 拡張子部分を取り除く
				}

				// ファイル名を求める
				tString filename;

				// もし *i がディレクトリだった場合は、その下に _init.rs があるかどうかをチェックする
				if(is_dir)
				{
					filename = dirname + tSS<'/'>() + *i +
								tSS<'_','i','n','i','t','.','r','s'>();
					if(BuiltinPackageFileSystem->GetType(filename) != 1)
							continue; // それが存在しないあるいはファイルではない
				}
				else
				{
					filename = dirname + tSS<'/'>() + *i;
				}

				// path_map に追加する
				// 追加する前に basename をチェックし、すでに存在した場合は
				// ディレクトリよりもファイルを優先させる
				gc_map<tString, tString>::iterator pmi = path_map.find(basename);
				if(pmi != path_map.end())
				{
					// 存在した
					 // 末尾が /_init.rs で終わってない(つまりファイル)の場合は
					 // continue する
					if(!pmi->first.EndsWith(tSS<'/','_','i','n','i','t','.','r','s'>())) continue;
				}
				path_map.insert(gc_map<tString, tString>::value_type(basename, filename));
			}

			// path_map から filenames と packages にコピーする
			// path_map は map なのでイテレーションすると自動的にコードポイント順になるはず………
			for(gc_map<tString, tString>::iterator i = path_map.begin();
				i != path_map.end(); i++)
			{
				filenames.push_back(i->second);
				packages.push_back(package_loc + tSS<'.'>() + i->first);
			}

			break;
		}
		else
		{
			// ワイルドカードを使わない場合
			if(BuiltinPackageFileSystem->GetType(filename + tSS<'.','r','s'>()) == 1)
			{
				// 見つかった
				filenames.push_back(filename + tSS<'.','r','s'>());
				packages.push_back(package_loc);
			}
			else if(BuiltinPackageFileSystem->GetType(filename) == 2)
			{
				// 見つかったけどそれはディレクトリ
				// その下に _init.rs があるかどうかを見る
				filename += tSS<'/'>();
				filename += tSS<'_','i','n','i','t','.','r','s'>();
				if(BuiltinPackageFileSystem->GetType(filename) == 1)
				{
					// それがファイルだった。見つかった。
					filenames.push_back(filename);
					packages.push_back(package_loc);
				}
			}

			if(filenames.size() > 0) break;
		}
	}

	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPackageManager::SearchPackage(const tString & name,
				gc_vector<tString> & filenames,
				gc_vector<tString> & packages)
{
	return SearchPackage(SplitPackageName(name), filenames, packages);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPackageManager::Dig(tVariant & dest, const tVariant & id,
	const tVariant & deepest, tMemberAttribute attrib)
{
	tVariant::tSynchronizer sync_dest(dest); // sync
	tVariant::tSynchronizer sync_id(id); // sync

	tVariant current = dest;
	risse_size id_count =
		(risse_int64)id.GetPropertyDirect(ScriptEngine, ss_length);
	for(risse_size i = 0; i < id_count; i++)
	{
		tString one =
			id.Invoke(ScriptEngine, tSS<'[',']'>(), tVariant((risse_int64)i));

		if(i == id_count - 1)
		{
			// current に one を deepest の内容で書き込む

			// インポートする場合
			tOperateFlags access_flags =
				tOperateFlags::ofMemberEnsure|
				tOperateFlags::ofInstanceMemberOnly|
				tOperateFlags::ofUseClassMembersRule;
			tMemberAttribute attrib_spec =
				tMemberAttribute::GetDefault().Set(tMemberAttribute::mcNone);
				// 変更性のみは指定しない。これは定数の上書き時に強制的にエラーにしたいため。

			// To にメンバを作成
			current.SetPropertyDirect(ScriptEngine, one,
				(risse_uint32)attrib_spec | (risse_uint32)access_flags,
				deepest);

			// そのメンバの属性を設定
			current.SetAttributeDirect(ScriptEngine, one, attrib);
		}
		else
		{
			// current に one がすでにある？
			tVariant value;
			tObjectInterface::tRetValue rv =
				current.Operate(ScriptEngine, ocDGet, &value, one,
					tOperateFlags::ofUseClassMembersRule|tOperateFlags::ofInstanceMemberOnly);
			if(rv != tObjectInterface::rvNoError)
			{
				// うーん、なんかエラー？たぶんメンバが無いんだと思うけど
				// だったらとりあえず作ってみる
				value = tVariant(ScriptEngine->ObjectClass).New();
				current.SetPropertyDirect(ScriptEngine, one,
						tOperateFlags::ofMemberEnsure|
						tOperateFlags::ofInstanceMemberOnly|
						tOperateFlags::ofUseClassMembersRule,
						value);
			}
			current = value;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPackageManager::Dig(tVariant & dest, const tString & id,
	const tVariant & deepest, tMemberAttribute attrib)
{
	Dig(dest, SplitPackageName(id), deepest, attrib);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPackageManager::SplitPackageName(const tString & name)
{
	// name を . (ドット) で split する
	tVariant array = tVariant(ScriptEngine->ArrayClass).New();
	tString::tSplitter spliter(name, RISSE_WC('.'));
	tString component;
	while(spliter(component))
	{
		if(component.IsEmpty()) continue;
		array.Invoke_Object(tSS<'p','u','s','h'>(), tVariant(component));
	}

	return array;
}
//---------------------------------------------------------------------------

} // namespace Risse



