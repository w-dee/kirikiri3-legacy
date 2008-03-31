//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSの実装
//---------------------------------------------------------------------------
#ifndef _OSFSH_
#define _OSFSH_

#include "risa/packages/risa/fs/FSManager.h"
#include "risa/packages/risa/fs/FileSystem.h"
#include "base/utils/RisaThread.h"
#include "risse/include/risseWCString.h"
#include "risse/include/builtin/stream/risseStreamClass.h"
#include "risa/packages/risa/fs/osfs/OSNativeStream.h"
#include <wx/file.h>

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		OS ファイルシステム
//---------------------------------------------------------------------------
class tOSFSInstance : public tFileSystemInstance
{
	tWCString BaseDirectory; //!< このファイルシステムがマウントしているOSのディレクトリ(最後にパスデリミタを含む)
	risse_size BaseDirectoryLengthWx; //!< (wxStringにおける) ベースディレクトリ名の長さ
	bool CheckCase; //!< ファイル名の大文字・小文字をチェックする場合は真

public:
	//! @brief		コンストラクタ
	tOSFSInstance();


	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tOSFSInstance() {;}

	//! @brief		オプションを設定する
	//! @param		basedir		ベースとなる OS ファイルシステム上のディレクトリ
	//! @param		checkcase	大文字と小文字を強制的に区別するかどうか
	//!							(trueを指定するとWindowsのような環境でも
	//!							大文字と小文字の区別をするようになる。しかし
	//!							falseにしたからといってUNIXのような環境で大文字と
	//!							小文字を区別しないようになるわけではない)
	void SetOptions(const tString & basedir, bool checkcase = true);

public: // Risse 用メソッドなど
	void construct();
	void initialize(const tString & base_dir, const tNativeCallInfo &info);

	//-- FileSystem メンバ

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

	//-- FileSystem メンバ ここまで

	tString get_source() const { return BaseDirectory; }

private:
	//! @brief		path中に含まれる'/'を、OSネイティブなパス区切り記号に変更する
	//! @param		path パス名
	//! @return		OSネイティブなパス区切りに変更された後のパス名
	static wxString ConvertToNativePathDelimiter(const wxString & path);

	//! @brief		path中のディレクトリ名やファイル名の大文字や小文字がファイルシステム上のそれと一致するかどうかを調べる
	//! @param		path_to_check パス名(フルパスであること)
	//! @param		raise 一致しなかった場合に例外を発生するかどうか
	//! @return		一致した場合に真、しなかった場合に偽
	//! @note		ファイルが「見つからない」場合は真が帰るので注意
	bool CheckFileNameCase(const wxString & path_to_check, bool raise = true);

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"OSFS" クラス
//---------------------------------------------------------------------------
class tOSFSClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

	tOSNativeStreamClass * OSNativeStreamClass; //!< OSNativeStream クラスインスタンス

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tOSFSClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:

	//! @brief OSNativeStreamClass クラスインスタンスを得る
	tOSNativeStreamClass * GetOSNativeStreamClass() const { return OSNativeStreamClass; }
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		risa.fs.osfs パッケージイニシャライザ
//---------------------------------------------------------------------------
class tRisaOsfsPackageInitializer : public tBuiltinPackageInitializer
{
public:
	tOSFSClass * OSFSClass;
	tOSNativeStreamClass * OSNativeStreamClass;

	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisaOsfsPackageInitializer(tScriptEngine * engine);

	//! @brief		パッケージを初期化する
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		パッケージ名
	//! @param		global		パッケージグローバル
	void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risa


#endif
