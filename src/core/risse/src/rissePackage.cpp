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

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(58978,40915,31180,20110,43417,63480,33374,56600);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tPackageManager::tPackageManager(tScriptEngine * script_engine)
{
	ScriptEngine = script_engine;

	CS = new tCriticalSection();

	// "risse" パッケージを作成する
	AddPackageGlobal(tSS<'r','i','s','s','e'>(), RissePackageGlobal);

	// "risse" パッケージに、パッケージ検索パスを表す "packagePath" を追加
	tVariant packagePath = tVariant(ScriptEngine->ArrayClass).New();
	RissePackageGlobal.SetPropertyDirect_Object(tSS<'p','a','c','k','a','g','e','P','a','t','h'>(),
				tOperateFlags::ofMemberEnsure|
				tOperateFlags::ofInstanceMemberOnly|
				tOperateFlags::ofUseClassMembersRule,
				packagePath);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPackageManager::GetPackageGlobal(const tString & name)
{
	tCriticalSection::tLocker lock(*CS); // sync

	tVariant ret;
	AddPackageGlobal(name, ret);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPackageManager::DoImport(tVariant & dest, const tVariant & packages)
{
	// TODO: dest が プリミティブ型の時は dest に新しくメンバを作成できないのでチェック

	tCriticalSection::tLocker lock(*CS); // sync

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
		// TODO: package のインスタンスチェック
		// as を得る
		tVariant as =
			dic.Invoke(ScriptEngine, tSS<'[',']'>(), tVariant(tSS<'a','s'>()));

		// package に対応するパッケージのファイル名を得る
		gc_vector<tString> filenames;
		gc_vector<tString> locations;
		SearchPackage(package_loc, filenames, locations);

		// TODO: as がある場合は filenames や locations は要素数が 1 のみのはず
		// なのでチェック

		// 対応するパッケージを順に読み込む
		for(risse_size j = 0; j < locations.size(); j++)
		{
			tVariant package_global = InitPackage(filenames[j], locations[j]);
			// もし as が指定されている場合はそれを as に、
			// そうでない場合は locations[j] で指定された位置に dig して書き込む
			if(as.IsVoid())
				Dig(dest, locations[j], package_global); // as の指定無し
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
	// TODO: dest が プリミティブ型の時は dest に新しくメンバを作成できないのでチェック

	tCriticalSection::tLocker lock(*CS); // sync

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tPackageManager::AddPackageGlobal(const tString & name, tVariant & global)
{
	tMap::iterator i = Map.find(name);
	if(i == Map.end())
	{
		// 見つからなかったのでパッケージグローバルを作成する
		tVariant new_global = tVariant(ScriptEngine->ObjectClass).New();
		Map.insert(tMap::value_type(name, new_global));

		// 新しい global には "risse" パッケージの中身をすべて import する
		// "risse" パッケージを作るときだけはさすがにこれはできない。
		// 初回は RissePackageGlobal は void のはず………
		if(!RissePackageGlobal.IsVoid())
			ImportIds(RissePackageGlobal, new_global, NULL);

		global = new_global;
		return false;
	}
	global = i->second;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPackageManager::InitPackage(const tString & filename, const tString & name)
{
	bool is_existing_package;
	tVariant package_global;
	is_existing_package = AddPackageGlobal(name, package_global);
	tPackageFileSystemInterface * fs = ScriptEngine->GetPackageFileSystem();
	if(!is_existing_package && fs)
	{
		// パッケージの初回の初期化の場合はパッケージを読み込んで初期化する

		// ファイルを読み込む
		tString content = fs->ReadFile(filename);

		// そのファイルをパッケージ内で実行する
		tBindingInfo bind(package_global, package_global);
		ScriptEngine->Evaluate(content, filename, 0, NULL, &bind, false);
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

	// from と to をロックする
	tVariant::tSynchronizer sync_from(from);
	tVariant::tSynchronizer sync_to(to);

	// from にある識別子を列挙する
	RISSE_ASSERT(from.GetType() == tVariant::vtObject);
	tObjectBase * from_objectbase = static_cast<tObjectBase*>(from.GetObjectInterface());

	class callback : public tObjectBase::tEnumMemberCallback
	{
		tVariant To;
		const tVariant * Ids;
	public:
		callback(const tVariant & to, const tVariant * ids) : To(to), Ids(ids) {}

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
				RISSE_ASSERT(To.GetType() == tVariant::vtObject);
				tOperateFlags access_flags =
					tOperateFlags::ofMemberEnsure|
					tOperateFlags::ofInstanceMemberOnly|
					tOperateFlags::ofUseClassMembersRule;
				tMemberAttribute attrib =
					tMemberAttribute::GetDefault().Set(tMemberAttribute::mcNone);
					// 変更性のみは指定しない。これは定数の上書き時に強制的にエラーにしたいため。

				// To にメンバを作成
				To.SetPropertyDirect_Object(as.operator tString(),
					(risse_uint32)attrib | (risse_uint32)access_flags,
					data.Value);

				// そのメンバの属性を設定
				To.SetAttributeDirect_Object(
					as.operator tString(), data.Attribute);
			}

			return true;
		}
	} cb(to, ids);
	from_objectbase->Enumurate(&cb);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPackageManager::SearchPackage(const tVariant & name,
				gc_vector<tString> & filenames,
				gc_vector<tString> & packages)
{
	// パッケージファイルシステムインターフェースを得る
	tPackageFileSystemInterface * fs = ScriptEngine->GetPackageFileSystem();
	if(!fs) return; // パッケージファイルシステムが利用不可能

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
			if(fs->GetType(filename) != 2) continue; // ディレクトリじゃないので次へ

			// パスにあるファイルを得る
			gc_map<tString, tString> path_map;
			gc_vector<tString> files;
			tString dirname = path_dir + tSS<'/'>() + package_fs_loc;
			fs->List(dirname, files);

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
					if(fs->GetType(filename) != 1) continue; // それが存在しないあるいはファイルではない
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
			if(fs->GetType(filename + tSS<'.','r','s'>()) == 1)
			{
				// 見つかった
				filenames.push_back(filename + tSS<'.','r','s'>());
				packages.push_back(package_loc);
			}
			else if(fs->GetType(filename) == 2)
			{
				// 見つかったけどそれはディレクトリ
				// その下に _init.rs があるかどうかを見る
				filename += tSS<'/'>();
				filename += tSS<'_','i','n','i','t','.','r','s'>();
				if(fs->GetType(filename) == 1)
				{
					// それがファイルだった。見つかった。
					filenames.push_back(filename);
					packages.push_back(package_loc);
				}
			}

			if(filenames.size() > 0) break;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPackageManager::Dig(tVariant & dest, const tVariant & id, const tVariant & deepest)
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
			current.SetPropertyDirect_Object(one,
						tOperateFlags::ofMemberEnsure|
						tOperateFlags::ofInstanceMemberOnly|
						tOperateFlags::ofUseClassMembersRule,
						deepest);
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
				current.SetPropertyDirect_Object(one,
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
void tPackageManager::Dig(tVariant & dest, const tString & id, const tVariant & deepest)
{
	// id を . (ドット) で split する
	// まだこれ書いてる時点では String::split がないんだよなー
	tVariant array = tVariant(ScriptEngine->ArrayClass).New();
	risse_size start = 0;
	risse_size current = 0;
	risse_size length = id.GetLength();
	const risse_char *ref = id.c_str();
	while(true)
	{
		while(current < length && ref[current] != RISSE_WC('.')) current ++;
		if(current >= length) break;

		if(start != current)
		{
			tString component = tString(id, start, current - start);
			array.Invoke_Object(tSS<'p','u','s','h'>(), tVariant(component));
		}

		current ++;
		start = current;
	}

	if(start != current)
	{
		tString component = tString(id, start, current - start);
		array.Invoke_Object(tSS<'p','u','s','h'>(), tVariant(component));
	}

	// Dig の tVariant 配列版を呼び出す
	Dig(dest, array, deepest);
}
//---------------------------------------------------------------------------

} // namespace Risse



