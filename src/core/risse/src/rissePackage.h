//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
class tBuiltinPackageFileSystem;
class tBuiltinPackageInitializerInterface;
//---------------------------------------------------------------------------
/**
 * パッケージ管理クラス
 */
class tPackageManager : public tCollectee
{
	tScriptEngine * ScriptEngine; //!< スクリプトエンジンインスタンス

	typedef gc_map<tString, tVariant> tMap; //!< パッケージ完全装飾名 -> パッケージグローバルへのマップのtypedef
	tMap Map; //!< パッケージ完全装飾名 -> パッケージグローバルへのマップ
	tVariant RissePackageGlobal; //!< "risse" パッケージグローバル
	tBuiltinPackageFileSystem * BuiltinPackageFileSystem; //!< 組み込みパッケージ用の仮想ファイルシステム

	tCriticalSection * CS; //!< このオブジェクトを保護するクリティカルセクション

public:
	/**
	 * コンストラクタ
	 * @param script_engine	スクリプトエンジンインスタンス
	 */
	tPackageManager(tScriptEngine * script_engine);

	/**
	 * "risse" パッケージを得る
	 * @return	"risse" パッケージ
	 */
	const tVariant & GetRissePackageGlobal() const { return RissePackageGlobal; }

	/**
	 * パッケージを(必要ならば)初期化してそのパッケージグローバルを返す
	 * @param name	パッケージの完全装飾名
	 * @return	そのパッケージのグローバルオブジェクト
	 * @note	必要ならばそのパッケージが初期化される。
	 *			初期化済みならば何もせずにそのパッケージを返す。
	 * @note	ワイルドカードは指定しないこと。
	 */
	tVariant GetPackageGlobal(const tString & name);

	/**
	 * 組み込みパッケージの情報を追加する
	 * @param package	パッケージ名
	 * @param init		イニシャライザ
	 *					(パッケージが初めて初期化される際に呼ばれる)
	 */
	void AddBuiltinPackage(const tString & package,
		tBuiltinPackageInitializerInterface * init);

	/**
	 * インポートを行う
	 * @param global	パッケージグローバル
	 *					(パッケージが相対指定された場合に、その基準となる)
	 * @param dest		インポート先オブジェクト
	 * @param packages	インポートするパッケージを表す辞書配列の配列
	 */
	void DoImport(const tVariant & global, tVariant & dest, const tVariant & packages);

	/**
	 * インポートを行う
	 * @param global	パッケージグローバル
	 *					(パッケージが相対指定された場合に、その基準となる)
	 * @param dest		インポート先オブジェクト
	 * @param packages	インポートするパッケージを表す配列
	 * @param ids		インポートする識別子を表す辞書配列
	 */
	void DoImport(const tVariant & global, tVariant & dest, const tVariant & packages, const tVariant & ids);

private:
	/**
	 * パッケージの相対位置を求める
	 * @param global	基準となるglobal
	 * @param package	パッケージ配列
	 * @note	package 配列の先頭にある "" は取り除かれ、
	 *			代わりに絶対指定のパッケージ配列になる。
	 */
	void AdjustRelativePackage(const tVariant & global, const tVariant & package);

	/**
	 * パッケージを初期化する
	 * @param filename	パッケージが入ったファイル名
	 * @param name		パッケージグローバルの完全装飾名
	 * @return	パッケージグローバル
	 * @note	初期化しようとしたパッケージが既に初期化中だった場合は ImportException
	 *			例外が発生する
	 */
	tVariant InitPackage(const tString & filename, const tString & name);

	/**
	 * パッケージから識別子をインポートする
	 * @param from	インポート元のパッケージグローバル
	 * @param to	インポート先のパッケージグローバル
	 * @param ids	インポートしたいidが入った辞書配列(NULL可)
	 * @note	ids が null の場合はすべてのpublicな識別子がインポートされる。
	 * @note	ids の中の辞書配列アイテムは、見つかれば削除される。
	 */
	void ImportIds(const tVariant & from, const tVariant & to,
		const tVariant * ids);

	/**
	 * パッケージをパスから検索する
	 * @param name		パッケージの完全装飾名
	 * @param filenames	見つかったパッケージのファイルパスが格納される配列
	 *					(呼び出し側で clear() すること)
	 * @param packages	見つかったパッケージの完全そう職名が filenames
	 *					と同じ順番で格納される配列
	 *					(呼び出し側で clear() すること)
	 * @return	もしパッケージがすでに初期化済みであることを検出した場合は
	 *			そのパッケージグローバルが帰る。そうでなければvoidが帰る。
	 */
	tVariant SearchPackage(const tString & name,
				gc_vector<tString> & filenames,
				gc_vector<tString> & packages
				);

	/**
	 * パッケージをパスから検索する
	 * @param name		パッケージの完全装飾名が入っている配列
	 * @param filenames	見つかったパッケージのファイルパスが格納される配列
	 *					(呼び出し側で clear() すること)
	 * @param packages	見つかったパッケージの完全そう職名が filenames
	 *					と同じ順番で格納される配列
	 *					(呼び出し側で clear() すること)
	 * @return	もしパッケージがすでに初期化済みであることを検出した場合は
	 *			そのパッケージグローバルが帰る。そうでなければvoidが帰る。
	 */
	tVariant SearchPackage(const tVariant & name,
				gc_vector<tString> & filenames,
				gc_vector<tString> & packages
				);

	/**
	 * パッケージ用の名前空間を「掘る」
	 * @param dest		掘る先のオブジェクト
	 * @param id		掘るidの配列
	 * @param deepest	一番深いところの名前空間とする内容
	 * @note	dest に Object のインスタンスによる階層を作成する。
	 *			たとえば id が ["a","b","c"] ならば dest.a.b.c という階層
	 *			を作成する。また、最後の c は deepest の内容になる。
	 * @param attrib	deepest を書き込むときのそのメンバの属性
	 */
	void Dig(tVariant & dest, const tVariant & id, const tVariant & deepest,
		tMemberAttribute attrib = tMemberAttribute::GetDefault());

	/**
	 * パッケージ用の名前空間を「掘る」
	 * @param dest		掘る先のオブジェクト
	 * @param id		掘るidがドットで繋がった名前
	 * @param deepest	一番深いところの名前空間とする内容
	 * @param attrib	deepest を書き込むときのそのメンバの属性
	 */
	void Dig(tVariant & dest, const tString & id, const tVariant & deepest,
		tMemberAttribute attrib = tMemberAttribute::GetDefault());

	/**
	 * パッケージ名をドットでsplitし、配列オブジェクトにして返す
	 * @param name	パッケージ名
	 * @return	配列オブジェクト
	 */
	tVariant SplitPackageName(const tString & name);

};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

