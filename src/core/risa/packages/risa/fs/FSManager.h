//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------
#ifndef _FSMANAGER_H_
#define _FSMANAGER_H_

#include <wx/file.h>
#include <wx/datetime.h>
#include "risseHashTable.h"
#include "risa/common/Singleton.h"
#include "risa/common/RisaThread.h"
#include "risa/common/RisseEngine.h"
#include "risseString.h"
#include "risseStream.h"
#include "builtin/stream/risseStreamClass.h"

namespace Risa {
//---------------------------------------------------------------------------


class tFileSystemInstance;
//---------------------------------------------------------------------------
/**
 * tFileSystem::List で用いられるコールバックインターフェース
 */
class tFileSystemIterationCallback : public tObjectInterface
{
public:
	/**
	 * コンストラクタ
	 */
	tFileSystemIterationCallback() :
		tObjectInterface(tRisseScriptEngine::instance()->GetDefaultRTTI(), NULL)
	{
	}

	/**
	 * Operate メソッド
	 */
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

protected:
	/**
	 * ファイルが見つかった際に呼ばれる
	 * @param filename	ファイル名
	 */
	virtual void OnFile(const tString & filename) = 0;

	/**
	 * ディレクトリが見つかった際に呼ばれる
	 * @param filename	ディレクトリ名
	 */
	virtual void OnDirectory(const tString & dirname) = 0;
};
//---------------------------------------------------------------------------


#if 0
//---------------------------------------------------------------------------
/**
 * tFileSystem::Stat で返される構造体
 */
struct tStatStruc : public tAtomicCollectee
{
	risse_uint64	Size;	//!< ファイルサイズ。 risse_uint64_maxの場合は無効
	wxDateTime		MTime;	//!< ファイル修正時刻 (wxDateTime::IsValidで有効性をチェックのこと)
	wxDateTime		ATime;	//!< アクセス時刻 (wxDateTime::IsValidで有効性をチェックのこと)
	wxDateTime		CTime;	//!< 作成時刻 (wxDateTime::IsValidで有効性をチェックのこと)

	tStatStruc() { Clear(); }
	void Clear()
	{
		Size = risse_uint64_max;
		MTime = ATime = CTime = wxDateTime();
	}
};
//---------------------------------------------------------------------------
#endif




//---------------------------------------------------------------------------
/**
 * オープンモード定数を含む構造体
 */
struct tFileOpenModes
{
	/**
	 * ファイルオープンモード
	 */
	enum tOpenMode
	{
		omRead = 1 /*!< 読み込みのみ(対象ファイルが無い場合は失敗する) */,
		omWrite = 2 /*!< 書き込みのみ(対象ファイルが無い場合は新規に作成される) */,
		omUpdate = 3 /*!< 読み込みと書き込み(対象ファイルが無い場合は失敗する) */,

		omAccessMask = 0x03 /*!< アクセス方法に対するマスク */,

		omAppend = 7 /*!< 追加書き込み(対象ファイルが無い場合は新規に作成される) */,

		omReadBit = 1 /*!< 読み込み */,
		omWriteBit = 2 /*!< 書き込み */,
		omAppendBit = 4 /*!< 追加 */
	};
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * ファイルシステムマネージャクラス
 */
class tFileSystemManager :
	public singleton_base<tFileSystemManager>,
	protected depends_on<tRisseScriptEngine>, public tFileOpenModes
{
	tHashTable<tString, tFileSystemInstance *> MountPoints; //!< マウントポイントのハッシュ表
	tString CurrentDirectory; //!< カレントディレクトリ (パスの最後に '/' を含む)

	tCriticalSection CS; //!< このファイルシステムマネージャを保護するクリティカルセクション

public:
	/**
	 * コンストラクタ
	 */
	tFileSystemManager();

	/**
	 * デストラクタ(呼ばれることはない)
	 */
	virtual ~tFileSystemManager() {;}

public:
	/**
	 * ファイルシステムをマウントする
	 * @param point			マウントポイント
	 * @param fs_risseobj	ファイルシステムオブジェクトを表すRisseオブジェクト
	 * @note	メインスレッド以外から呼び出さないこと
	 */
	void Mount(const tString & point, tFileSystemInstance * fs_risseobj);

	/**
	 * ファイルシステムをアンマウントする
	 * @param point	マウントポイント
	 * @note	メインスレッド以外から呼び出さないこと
	 */
	void Unmount(const tString & point);

	/**
	 * ファイルシステムをアンマウントする
	 * @param fs_risseobj	アンマウントしたいファイルシステムを表すRisseオブジェクト
	 * @note	メインスレッド以外から呼び出さないこと
	 */
	void Unmount(tFileSystemInstance * fs_risseobj);

	/**
	 * パスを正規化する
	 * @param path	正規化したいパス
	 * @return	正規化されたパス
	 */
	tString NormalizePath(const tString & path);

	/**
	 * ファイル一覧を取得する
	 * @param dirname	ディレクトリ名
	 * @param callback	コールバックオブジェクト
	 * @param recursive	再帰的にファイル一覧を得るかどうか
	 * @return	取得できたファイル数
	 */
	size_t WalkAt(const tString & dirname,
		tFileSystemIterationCallback * callback, bool recursive = false);

	/**
	 * ファイルが存在するかどうかを得る
	 * @param filename	ファイル名
	 * @return	ファイルが存在する場合真、見つかったのがファイルでない場合や存在しない場合は偽
	 */
	bool IsFile(const tString & filename);

	/**
	 * ディレクトリが存在するかどうかを得る
	 * @param dirname	ディレクトリ名
	 * @return	ディレクトリが存在する場合真、見つかったのがディレクトリでない場合や存在しない場合は偽
	 */
	bool IsDirectory(const tString & dirname);

	/**
	 * ファイルを削除する
	 * @param filename	ファイル名
	 */
	void RemoveFile(const tString & filename);

	/**
	 * ディレクトリを削除する
	 * @param dirname	ディレクトリ名
	 * @param recursive	再帰的にディレクトリを削除するかどうか
	 */
	void RemoveDirectory(const tString & dirname, bool recursive = false);

	/**
	 * ディレクトリを作成する
	 * @param dirname	ディレクトリ名
	 * @param recursive	再帰的にディレクトリを作成するかどうか
	 */
	void CreateDirectory(const tString & dirname, bool recursive = false);

	/**
	 * 指定されたファイルの stat を得る
	 * @param filename	ファイル名
	 * @param struc		stat 結果の出力先
	 */
	tObjectInterface * Stat(const tString & filename);

	/**
	 * 指定されたファイルのストリームを得る
	 * @param filename	ファイル名
	 * @param flags		フラグ
	 * @return	ストリームオブジェクト
	 */
	tStreamInstance * Open(const tString & filename, risse_uint32 flags);

	/**
	 * 指定されたファイルのストリームを得る(タイプチェックあり)
	 * @param filename	ファイル名またはストリーム
	 * @param flags		フラグ
	 * @return	ストリームオブジェクト
	 * @note	filename がストリームオブジェクトのインスタンスを表していれば、
	 *			それを帰す。そうでなければ、それを文字列化したものをファイル名と
	 *			みなし、flags に従って open して帰す。
	 */
	tStreamInstance * Open(const tVariant & filename, risse_uint32 flags);

	/**
	 * 指定された位置にあるファイルシステムインスタンスを得る
	 * @param path	パス
	 * @return	ファイルシステムインスタンス
	 */
	tFileSystemInstance * GetFileSystemAt(const tString & path);

private:
	/**
	 * ファイル一覧を取得する(内部関数)
	 * @param dirname	ディレクトリ名(正規化されているべきこと)
	 * @param callback	コールバック先
	 */
	size_t InternalList(const tString & dirname,
		tFileSystemIterationCallback * callback);

	/**
	 * 指定された正規フルパスに対応するファイルシステムを得る
	 * @param fullpath	正規フルパス
	 * @param fspath	ファイルシステム内におけるパス(興味がない場合はNULL可、最初の / は含まれない)
	 * @return	ファイルシステムインスタンス
	 * @note	このメソッドはスレッド保護されていないため、このメソッドを呼ぶ場合は
	 *			CriticalSection 内で呼ぶこと！
	 */
	tFileSystemInstance * FindFileSystemAt(const tString & fullpath, tString * fspath = NULL);

	/**
	 * 「ファイルシステムが指定されたパスにはない」例外を発生させる(FileSystemException)
	 * @param filename	マウントポイント
	 * @note	この関数は例外を発生させるため呼び出し元には戻らない
	 */
	static void ThrowNoFileSystemError(const tString & filename);

public:

	/**
	 * 「そのようなファイルやディレクトリは無い」例外を発生させる(IOException)
	 */
	static void RaiseNoSuchFileOrDirectoryError();

	/**
	 * パス名を拡張子より前と拡張子に分離する ( /path/to/file.ext を /path/to/file と .ext に分離する )
	 * @param in	入力パス名
	 * @param other	[out] 拡張子より前の部分へポインタ(興味ない場合はNULL可)
	 * @param name	[out] 拡張子へのポインタ(興味ない場合はNULL可)  拡張子には .(ドット) を含む。拡張子がない場合は空文字列になる
	 * @note	in と そのほかのパラメータに同じ文字列を指定しないこと
	 */
	static void SplitExtension(const tString & in, tString * other, tString * ext);

	/**
	 * パス名をパスと名前に分離する ( /path/to/file を /path/to と file に分離する )
	 * @param in	入力パス名
	 * @param path	[out] パスへのポインタ(興味ない場合はNULL可)
	 * @param name	[out] 名前へのポインタ(興味ない場合はNULL可)
	 * @note	in と そのほかのパラメータに同じ文字列を指定しないこと
	 */
	static void SplitPathAndName(const tString & in, tString * path, tString * name);

	/**
	 * パス名の最後のパスデリミタ ('/') を取り去る ( /path/is/here/ を /path/is/here にする )
	 * @param path	パス
	 */
	static void TrimLastPathDelimiter(tString & path);

	/**
	 * 拡張子を切り落とす
	 * @param in	処理したいファイル名
	 * @return	拡張子が取り落とされたファイル名
	 */
	static tString ChopExtension(const tString & in);

	/**
	 * 拡張子を取り出す
	 * @param in	処理したいファイル名
	 * @return	拡張子(ドットも含む; 拡張子が無い場合は空文字)
	 */
	static tString ExtractExtension(const tString & in);

	/**
	 * ファイル名を返す (パスの部分を含まない)
	 * @param in	処理したいファイル名
	 * @return	ファイル名
	 */
	static tString ExtractName(const tString & in);

	/**
	 * ファイル名のパス名を返す
	 * @param in	処理したいファイル名
	 * @return	パス名
	 */
	static tString ExtractPath(const tString & in);

	/**
	 * フルパスを得る
	 */
	tString GetFullPath(const tString & in) { return NormalizePath(in); }

	/**
	 * 現在の作業ディレクトリを得る
	 * @return	作業ディレクトリ
	 */
	const tString & GetCurrentDirectory();

	/**
	 * 作業ディレクトリを設定する
	 * @param dir	作業ディレクトリ
	 * @note	実際にそのディレクトリが存在するかどうかのチェックは行わない
	 */
	void SetCurrentDirectory(const tString &dir);
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa


#endif
