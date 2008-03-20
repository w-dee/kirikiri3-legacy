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
void tPackageManager::ImportIds(const tVariant & from, const tVariant & to,
		const tVariant * ids)
{
	// from と to をロックする
	// TODO: ローカル namespace へのimport………は無理か
	// せめて this への import ですかね。グローバル位置で import すると this は
	// 自動的に global になるので global への import という意味に自動的になる
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
					tOperateFlags::ofMemberEnsure|tOperateFlags::ofInstanceMemberOnly;
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
	// name を '/' や '.' で連結しつつ、ワイルドカードがあるかどうかを調べる
	tVariant::tSynchronizer sync_name(name); // sync
	bool wildcard_found = false;

	tString package_fs_loc;
	tString package_loc;
	risse_size component_count =
		(risse_int64)name.GetPropertyDirect(ScriptEngine, ss_count);
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

	// パッケージファイルシステムインターフェースを得る
	tPackageFileSystemInterface * fs = ScriptEngine->GetPackageFileSystem();

	// パス変数を得る
	tVariant path = RissePackageGlobal.GetPropertyDirect(
						ScriptEngine,
						tSS<'p','a','c','k','a','g','e','P','a','t','h'>());
	tVariant::tSynchronizer sync_path(path); // sync
	risse_size path_count =
		(risse_int64)path.GetPropertyDirect(ScriptEngine, ss_count);

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
			tString path_dir =
				path.Invoke(ScriptEngine, tSS<'[',']'>(), tVariant((risse_int64)i));
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
			switch(fs->GetType(filename))
			{
			case 1:
				// 見つかった
				filenames.push_back(filename);
				packages.push_back(package_loc);
				break;

			case 2:
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
				break;

			default: ;
			}
			if(filenames.size() > 0) break;
		}
	}
}
//---------------------------------------------------------------------------

} // namespace Risse



