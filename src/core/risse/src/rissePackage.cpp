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
	AddPackageGlobal(tSS<'r','i','s','s','e'>(), RissePackage);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tPackageManager::AddPackageGlobal(const tString & name, tVariant & global)
{
	tMap::iterator i = Map.find(name);
	if(i == Map.end())
	{
		// 見つからなかったのでパッケージグローバルを作成する
		tVariant global = tVariant(ScriptEngine->ObjectClass).New();
		Map.insert(tMap::value_type(name, global));
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
				To.GetObjectInterface()->Do(ocDSetAttrib, NULL,
					as.operator tString(), data.Attribute);
			}

			return true;
		}
	} cb(to, ids);
	from_objectbase->Enumurate(&cb);

}
//---------------------------------------------------------------------------

} // namespace Risse



