//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
#include "risse/include/risseHashTable.h"
#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"
#include "base/script/RisseEngine.h"
#include "risse/include/risseString.h"
#include "risse/include/risseStream.h"
#include "risse/include/risseStreamClass.h"

namespace Risa {
//---------------------------------------------------------------------------


class tFileSystemInstance;
//---------------------------------------------------------------------------
//! @brief		tFileSystem::List で用いられるコールバックインターフェース
//---------------------------------------------------------------------------
class tFileSystemIterationCallback : public tObjectInterface
{
public:
	//! @brief		コンストラクタ
	tFileSystemIterationCallback() :
		tObjectInterface(tRisseScriptEngine::instance()->GetDefaultRTTI(), NULL)
	{
	}

	//! @brief		Operate メソッド
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

protected:
	//! @brief		ファイルが見つかった際に呼ばれる
	//! @param		filename		ファイル名
	virtual void OnFile(const tString & filename) = 0;

	//! @brief		ディレクトリが見つかった際に呼ばれる
	//! @param		filename		ディレクトリ名
	virtual void OnDirectory(const tString & dirname) = 0;
};
//---------------------------------------------------------------------------


#if 0
//---------------------------------------------------------------------------
//! @brief		tFileSystem::Stat で返される構造体
//---------------------------------------------------------------------------
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
//! @brief		オープンモード定数を含む構造体
//---------------------------------------------------------------------------
struct tFileOpenModes
{
	//! @param		ファイルオープンモード
	enum tOpenMode
	{
		omRead = 1, //!< 読み込みのみ(対象ファイルが無い場合は失敗する)
		omWrite = 2, //!< 書き込みのみ(対象ファイルが無い場合は新規に作成される)
		omUpdate = 3, //!< 読み込みと書き込み(対象ファイルが無い場合は失敗する)

		omAccessMask = 0x03, //!< アクセス方法に対するマスク

		omAppend = 7, //!< 追加書き込み(対象ファイルが無い場合は新規に作成される)

		omReadBit = 1, //!< 読み込み
		omWriteBit = 2, //!< 書き込み
		omAppendBit = 4 //!< 追加
	};
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		オープンモード定数用モジュール
//---------------------------------------------------------------------------
class tFileOpenModeConstsModule : public tModuleBase, public tFileOpenModes
{
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tFileOpenModeConstsModule(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		ファイルシステムマネージャクラス
//---------------------------------------------------------------------------
class tFileSystemManager :
	public singleton_base<tFileSystemManager>,
	protected depends_on<tRisseScriptEngine>, public tFileOpenModes
{
	tHashTable<tString, tFileSystemInstance *> MountPoints; //!< マウントポイントのハッシュ表
	tString CurrentDirectory; //!< カレントディレクトリ (パスの最後に '/' を含む)

	tCriticalSection CS; //!< このファイルシステムマネージャを保護するクリティカルセクション

public:
	//! @brief		コンストラクタ
	tFileSystemManager();

	//! @brief		デストラクタ(呼ばれることはない)
	virtual ~tFileSystemManager() {;}

public:
	//! @brief		ファイルシステムをマウントする
	//! @param		point マウントポイント
	//! @param		fs_risseobj ファイルシステムオブジェクトを表すRisseオブジェクト
	//! @note		メインスレッド以外から呼び出さないこと
	void Mount(const tString & point, tFileSystemInstance * fs_risseobj);

	//! @brief		ファイルシステムをアンマウントする
	//! @param		point マウントポイント
	//! @note		メインスレッド以外から呼び出さないこと
	void Unmount(const tString & point);

	//! @brief		ファイルシステムをアンマウントする
	//! @param		fs_risseobj アンマウントしたいファイルシステムを表すRisseオブジェクト
	//! @note		メインスレッド以外から呼び出さないこと
	void Unmount(tFileSystemInstance * fs_risseobj);

	//! @brief		パスを正規化する
	//! @param		path 正規化したいパス
	//! @return		正規化されたパス
	tString NormalizePath(const tString & path);

	//! @brief		ファイル一覧を取得する
	//! @param		dirname ディレクトリ名
	//! @param		callback コールバックオブジェクト
	//! @param		recursive 再帰的にファイル一覧を得るかどうか
	//! @return		取得できたファイル数
	size_t WalkAt(const tString & dirname,
		tFileSystemIterationCallback * callback, bool recursive = false);

	//! @brief		ファイルが存在するかどうかを得る
	//! @param		filename ファイル名
	//! @return		ファイルが存在する場合真、見つかったのがファイルでない場合や存在しない場合は偽
	bool IsFile(const tString & filename);

	//! @brief		ディレクトリが存在するかどうかを得る
	//! @param		dirname ディレクトリ名
	//! @return		ディレクトリが存在する場合真、見つかったのがディレクトリでない場合や存在しない場合は偽
	bool IsDirectory(const tString & dirname);

	//! @brief		ファイルを削除する
	//! @param		filename ファイル名
	void RemoveFile(const tString & filename);

	//! @brief		ディレクトリを削除する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを削除するかどうか
	void RemoveDirectory(const tString & dirname, bool recursive = false);

	//! @brief		ディレクトリを作成する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを作成するかどうか
	void CreateDirectory(const tString & dirname, bool recursive = false);

	//! @brief		指定されたファイルの stat を得る
	//! @param		filename ファイル名
	//! @param		struc stat 結果の出力先
	tObjectInterface * Stat(const tString & filename);

	//! @brief		指定されたファイルのストリームを得る
	//! @param		filename ファイル名
	//! @param		flags フラグ
	//! @return		ストリームオブジェクト
	tStreamInstance * Open(const tString & filename, risse_uint32 flags);

	//! @brief		指定されたファイルのストリームを得る(タイプチェックあり)
	//! @param		filename ファイル名またはストリーム
	//! @param		flags	フラグ
	//! @return		ストリームオブジェクト
	//! @note		filename がストリームオブジェクトのインスタンスを表していれば、
	//!				それを帰す。そうでなければ、それを文字列化したものをファイル名と
	//!				みなし、flags に従って open して帰す。
	tStreamInstance * Open(const tVariant & filename, risse_uint32 flags);

	//! @brief		指定された位置にあるファイルシステムインスタンスを得る
	//! @param		path   パス
	//! @return		ファイルシステムインスタンス
	tFileSystemInstance * GetFileSystemAt(const tString & path);

private:
	//! @brief		ファイル一覧を取得する(内部関数)
	//! @param		dirname ディレクトリ名(正規化されているべきこと)
	//! @param		callback コールバック先
	//! @callback	コールバックオブジェクト
	size_t InternalList(const tString & dirname,
		tFileSystemIterationCallback * callback);

	//! @brief		指定された正規フルパスに対応するファイルシステムを得る
	//! @param		fullpath 正規フルパス
	//! @param		fspath ファイルシステム内におけるパス(興味がない場合はNULL可、最初の / は含まれない)
	//! @return		ファイルシステムインスタンス
	//! @note		このメソッドはスレッド保護されていないため、このメソッドを呼ぶ場合は
	//!				CriticalSection 内で呼ぶこと！
	tFileSystemInstance * FindFileSystemAt(const tString & fullpath, tString * fspath = NULL);

	//! @brief		「ファイルシステムが指定されたパスにはない」例外を発生させる(FileSystemException)
	//! @param		filename  マウントポイント
	//! @note		この関数は例外を発生させるため呼び出し元には戻らない
	static void ThrowNoFileSystemError(const tString & filename);

public:

	//! @brief		「そのようなファイルやディレクトリは無い」例外を発生させる(IOException)
	static void RaiseNoSuchFileOrDirectoryError();

	//! @brief		パス名を拡張子より前と拡張子に分離する ( /path/to/file.ext を /path/to/file と .ext に分離する )
	//! @param		in 入力パス名
	//! @param		other [out] 拡張子より前の部分へポインタ(興味ない場合はNULL可)
	//! @param		name [out] 拡張子へのポインタ(興味ない場合はNULL可)  拡張子には .(ドット) を含む。拡張子がない場合は空文字列になる
	//! @note		in と そのほかのパラメータに同じ文字列を指定しないこと
	static void SplitExtension(const tString & in, tString * other, tString * ext);

	//! @brief		パス名をパスと名前に分離する ( /path/to/file を /path/to と file に分離する )
	//! @param		in 入力パス名
	//! @param		path [out] パスへのポインタ(興味ない場合はNULL可)
	//! @param		name [out] 名前へのポインタ(興味ない場合はNULL可)
	//! @note		in と そのほかのパラメータに同じ文字列を指定しないこと
	static void SplitPathAndName(const tString & in, tString * path, tString * name);

	//! @brief		パス名の最後のパスデリミタ ('/') を取り去る ( /path/is/here/ を /path/is/here にする )
	//! @param		path パス
	static void TrimLastPathDelimiter(tString & path);

	//! @brief		拡張子を切り落とす
	//! @param		in 処理したいファイル名
	//! @return		拡張子が取り落とされたファイル名
	static tString ChopExtension(const tString & in);

	//! @brief		拡張子を取り出す
	//! @param		in 処理したいファイル名
	//! @return		拡張子(ドットも含む; 拡張子が無い場合は空文字)
	static tString ExtractExtension(const tString & in);

	//! @brief		ファイル名を返す (パスの部分を含まない)
	//! @param		in 処理したいファイル名
	//! @return		ファイル名
	static tString ExtractName(const tString & in);

	//! @brief		ファイル名のパス名を返す
	//! @param		in 処理したいファイル名
	//! @return		パス名
	static tString ExtractPath(const tString & in);

	//! @brief		フルパスを得る
	tString GetFullPath(const tString & in) { return NormalizePath(in); }

	//! @brief		現在の作業ディレクトリを得る
	//! @return		作業ディレクトリ
	const tString & GetCurrentDirectory();

	//! @brief		作業ディレクトリを設定する
	//! @param		dir   作業ディレクトリ
	//! @note		実際にそのディレクトリが存在するかどうかのチェックは行わない
	void SetCurrentDirectory(const tString &dir);
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		ファイルシステム基底インスタンス("FileSystem" クラスのインスタンス)
//---------------------------------------------------------------------------
class tFileSystemInstance : public tObjectBase, public tFileOpenModes
{
public: // コンストラクタ
	tFileSystemInstance() {;}

public: // 定数など


public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

	//-- サブクラスで実装すべき物

	//! @brief		ファイル一覧をコールバックで受け取る
	//! @param		dirname ディレクトリ名
	//! @param		args 追加パラメータ(ブロック引数としてコールバック関数オブジェクト)
	//! @return		取得できたファイル数
	size_t walkAt(const tString & dirname,
		const tMethodArgument &args);

	//! @brief		ファイルが存在するかどうかを得る
	//! @param		filename ファイル名
	//! @return		ファイルが存在する場合真
	bool isFile(const tString & filename);

	//! @brief		ディレクトリが存在するかどうかを得る
	//! @param		dirname ディレクトリ名
	//! @return		ディレクトリが存在する場合真
	bool isDirectory(const tString & dirname);

	//! @brief		ファイルを削除する
	//! @param		filename ファイル名
	void removeFile(const tString & filename);

	//! @brief		ディレクトリを削除する
	//! @param		dirname ディレクトリ名
	//! @param		args 追加パラメータ(再帰的にディレクトリを削除するかどうか)
	void removeDirectory(const tString & dirname,
		const tMethodArgument &args);

	//! @brief		ディレクトリを作成する
	//! @param		dirname ディレクトリ名
	void createDirectory(const tString & dirname);

	//! @brief		指定されたファイルの stat を得る
	//! @param		filename ファイル名
	//! @return		stat の結果を表す辞書配列
	tObjectInterface * stat(const tString & filename);

	//! @brief		指定されたファイルのストリームを得る
	//! @param		filename ファイル名
	//! @param		flags フラグ
	//! @return		ストリームオブジェクト
	tStreamInstance * open(const tString & filename,
		risse_uint32 flags);

	//! @brief		ディスク(等に)内容をフラッシュする
	//! @note		このメソッドが呼ばれたら、ファイルシステムはそれまでの
	//!				変更点を確実にディスクなどに永続化することが望まれる。
	//!				逆にこのメソッドが呼ばれずにプログラムが終了したり
	//!				ファイルシステムを開放した場合は、永続化する保証はない。
	//!				永続化されなかった場合、それまでの変更内容が中途半端に
	//!				反映された状態になるのか、あるいはそれまでの変更内容が
	//!				全くなかったかにされるかは、ファイルシステムの実装による。
	void flush();
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		"FileSystem" クラス
//---------------------------------------------------------------------------
class tFileSystemClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tFileSystemClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public: // Risse 用メソッドなど
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"File" クラス
//---------------------------------------------------------------------------
class tFileClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tFileClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public: // Risse 用メソッドなど
	static void construct() { }
	static void initialize() { }

	static void mount(const tString & point, const tVariant & fs);
	static void unmount(const tString & point)
		{ tFileSystemManager::instance()->Unmount(point); }
	static tString normalize(const tString & path)
		{ return tFileSystemManager::instance()->NormalizePath(path); }
	static size_t walkAt(const tString & dirname,
		const tMethodArgument &args, tScriptEngine * engine);
	static bool exists(const tString & filename)
		{ return tFileSystemManager::instance()->IsFile(filename) ||
			tFileSystemManager::instance()->IsDirectory(filename); }
	static bool isFile(const tString & filename)
		{ return tFileSystemManager::instance()->IsFile(filename); }
	static bool isDirectory(const tString & dirname)
		{ return tFileSystemManager::instance()->IsDirectory(dirname); }
	static void removeFile(const tString & filename)
		{ return tFileSystemManager::instance()->RemoveFile(filename); }
	static void removeDirectory(const tString & dirname, const tMethodArgument &args);
	static void createDirectory(const tString & dirname, const tMethodArgument &args);
	static tObjectInterface * stat(const tString & filename)
		{ return tFileSystemManager::instance()->Stat(filename); }
	static tVariant open(const tString & filename, const tNativeCallInfo &info);
	static tFileSystemInstance * getFileSystemAt(const tString & path)
		{ return tFileSystemManager::instance()->GetFileSystemAt(path); }
	static tString chopExtension(const tString & filename)
		{ return tFileSystemManager::instance()->ChopExtension(filename); }
	static tString extractExtension(const tString & filename)
		{ return tFileSystemManager::instance()->ExtractExtension(filename); }
	static tString extractName(const tString & filename)
		{ return tFileSystemManager::instance()->ExtractName(filename); }
	static tString extractPath(const tString & filename)
		{ return tFileSystemManager::instance()->ExtractPath(filename); }
	static tString get_cwd()
		{ return tFileSystemManager::instance()->GetCurrentDirectory(); }
	static void set_cwd(const tString & dirname)
		{ tFileSystemManager::instance()->SetCurrentDirectory(dirname); }
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		ファイルシステムクラスインスタンスをFileSystem以下に登録するためのテンプレートクラス
//---------------------------------------------------------------------------
template <typename ClassT>
class tRisseFSClassRegisterer :
	public singleton_base<tRisseFSClassRegisterer<ClassT> >,
	depends_on<tRisseClassRegisterer<tFileSystemClass> >
{
	ClassT * ClassInstance;
public:
	//! @brief		コンストラクタ
	tRisseFSClassRegisterer()
	{
		// ここらへんのプロセスについては tRisseClassRegisterer のコンストラクタも参照のこと
		tScriptEngine * engine = tRisseScriptEngine::instance()->GetScriptEngine();
		ClassT *class_instance = new ClassT(engine);
		ClassInstance = class_instance;
		tVariant FileSystem = engine->GetGlobalObject().
			GetPropertyDirect_Object(tSS<'F','i','l','e','S','y','s','t','e','m'>());
		class_instance->RegisterInstance(FileSystem);
	}

	ClassT * GetClassInstance() const { return ClassInstance; } //!< クラスインスタンスを得る
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


#endif
