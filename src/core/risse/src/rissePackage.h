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

	//! @brief		インポートを行う
	//! @param		dest		インポート先オブジェクト
	//! @param		packages	インポートするパッケージを表す辞書配列の配列
	void DoImport(tVariant & dest, const tVariant & packages);

	//! @brief		インポートを行う
	//! @param		dest		インポート先オブジェクト
	//! @param		packages	インポートするパッケージを表す配列
	//! @param		ids			インポートする識別子を表す辞書配列
	void DoImport(tVariant & dest, const tVariant & packages, const tVariant & ids);

private:
	//! @brief		パッケージグローバルを作成する
	//! @param		name		パッケージグローバルの完全装飾名
	//! @param		global		パッケージグローバルを格納する先
	//! @return		見つかった場合は真、見つからずに新しくパッケージグローバルが作成された場合は偽
	//! @note		既にそのパッケージが作成済みの場合は単にそのパッケージグローバルを返す
	bool AddPackageGlobal(const tString & name, tVariant & global);

	//! @brief		パッケージを初期化する
	//! @param		name		パッケージグローバルの完全装飾名
	//! @param		filename	パッケージが入ったファイル名
	//! @return		パッケージグローバル
	tVariant InitPackage(const tString & filename, const tString & name);

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

	//! @brief		パッケージ用の名前空間を「掘る」
	//! @param		dest		掘る先のオブジェクト
	//! @param		id			掘るidの配列
	//! @param		deepest		一番深いところの名前空間とする内容
	//! @note		dest に Object のインスタンスによる階層を作成する。
	//!				たとえば id が ["a","b","c"] ならば dest.a.b.c という階層
	//!				を作成する。また、最後の c は deepest の内容になる。
	void Dig(tVariant & dest, const tVariant & id, const tVariant & deepest);

	//! @brief		パッケージ用の名前空間を「掘る」
	//! @param		dest		掘る先のオブジェクト
	//! @param		id			掘るidがドットで繋がった名前
	//! @param		deepest		一番深いところの名前空間とする内容
	void Dig(tVariant & dest, const tString & id, const tVariant & deepest);
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

