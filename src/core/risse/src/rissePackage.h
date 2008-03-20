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
#ifndef rissePackageH
#define rissePackageH

#include "risseTypes.h"
#include "risseString.h"
#include "risseVariant.h"
#include "risseGC.h"

namespace Risse
{
class tCriticalSection;
class tScriptEngine;
//---------------------------------------------------------------------------
//! @brief		パッケージ管理クラス
//---------------------------------------------------------------------------
class tPackageManager : public tCollectee
{
	tScriptEngine * ScriptEngine; //!< スクリプトエンジンインスタンス

	typedef gc_map<tString, tVariant> tMap; //!< パッケージ完全装飾名 -> パッケージグローバルへのマップのtypedef
	tMap Map; //!< パッケージ完全装飾名 -> パッケージグローバルへのマップ
	tVariant RissePackageGlobal; //!< "risse" パッケージグローバル

	tCriticalSection * CS; //!< このオブジェクトを保護するクリティカルセクション

public:
	//! @brief		コンストラクタ
	//! @param		script_engine		スクリプトエンジンインスタンス
	tPackageManager(tScriptEngine * script_engine);

	//! @brief		"risse" パッケージを得る
	//! @return		"risse" パッケージ
	const tVariant & GetRissePackageGlobal() const { return RissePackageGlobal; }

	//! @brief		パッケージグローバルを取得する
	//! @param		name		パッケージの完全装飾名
	//! @return		そのパッケージのグローバルオブジェクト
	tVariant GetPackageGlobal(const tString & name);

private:
	//! @brief		パッケージグローバルを作成する
	//! @param		name		パッケージグローバルの完全装飾名
	//! @param		global		パッケージグローバルを格納する先
	//! @return		見つかった場合は真、見つからずに新しくパッケージグローバルが作成された場合は偽
	//! @note		既にそのパッケージが作成済みの場合は単にそのパッケージグローバルを返す
	bool AddPackageGlobal(const tString & name, tVariant & global);

	//! @brief		パッケージから識別子をインポートする
	//! @param		from		インポート元のパッケージグローバル
	//! @param		to			インポート先のパッケージグローバル
	//! @param		ids			インポートしたいidが入った辞書配列(NULL可)
	//! @note		ids が null の場合はすべてのpublicな識別子がインポートされる。
	//! @note		ids の中の辞書配列アイテムは、見つかれば削除される。
	static void ImportIds(const tVariant & from, const tVariant & to,
		const tVariant * ids);

	//! @brief		パッケージをパスから検索する
	//! @param		name		パッケージの完全装飾名が入っている配列
	//! @param		filenames	見つかったパッケージのファイルパスが格納される配列
	//!							(呼び出し側で clear() すること)
	//! @param		packages	見つかったパッケージの完全そう職名が filenames
	//!							と同じ順番で格納される配列
	//!							(呼び出し側で clear() すること)
	void SearchPackage(const tVariant & name,
				gc_vector<tString> & filenames,
				gc_vector<tString> & packages
				);
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

