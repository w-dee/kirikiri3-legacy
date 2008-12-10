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
#include "risa/common/RisaThread.h"
#include "risse/include/risseWCString.h"
#include "risse/include/builtin/stream/risseStreamClass.h"
#include "risa/packages/risa/fs/osfs/OSNativeStream.h"
#include <wx/file.h>

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * OS ファイルシステム
 */
class tOSFSInstance : public tFileSystemInstance
{
	tWCString BaseDirectory; //!< このファイルシステムがマウントしているOSのディレクトリ(最後にパスデリミタを含む)
	risse_size BaseDirectoryLengthWx; //!< (wxStringにおける) ベースディレクトリ名の長さ
	bool CheckCase; //!< ファイル名の大文字・小文字をチェックする場合は真

public:
	/**
	 * コンストラクタ
	 */
	tOSFSInstance();


	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tOSFSInstance() {;}

	/**
	 * オプションを設定する
	 * @param basedir	ベースとなる OS ファイルシステム上のディレクトリ
	 * @param checkcase	大文字と小文字を強制的に区別するかどうか
	 *					(trueを指定するとWindowsのような環境でも
	 *					大文字と小文字の区別をするようになる。しかし
	 *					falseにしたからといってUNIXのような環境で大文字と
	 *					小文字を区別しないようになるわけではない)
	 */
	void SetOptions(const tString & basedir, bool checkcase = true);

public: // Risse 用メソッドなど
	void construct();
	void initialize(const tString & base_dir, const tNativeCallInfo &info);

	//-- FileSystem メンバ

	/**
	 * ファイル一覧をコールバックで受け取る
	 * @param dirname	ディレクトリ名
	 * @param args		追加パラメータ(ブロック引数としてコールバック関数オブジェクト)
	 * @return	取得できたファイル数
	 */
	size_t walkAt(const tString & dirname,
		const tMethodArgument &args);

	/**
	 * ファイルが存在するかどうかを得る
	 * @param filename	ファイル名
	 * @return	ファイルが存在する場合真
	 */
	bool isFile(const tString & filename);

	/**
	 * ディレクトリが存在するかどうかを得る
	 * @param dirname	ディレクトリ名
	 * @return	ディレクトリが存在する場合真
	 */
	bool isDirectory(const tString & dirname);

	/**
	 * ファイルを削除する
	 * @param filename	ファイル名
	 */
	void removeFile(const tString & filename);

	/**
	 * ディレクトリを削除する
	 * @param dirname	ディレクトリ名
	 * @param args		追加パラメータ(再帰的にディレクトリを削除するかどうか)
	 */
	void removeDirectory(const tString & dirname,
		const tMethodArgument &args);

	/**
	 * ディレクトリを作成する
	 * @param dirname	ディレクトリ名
	 */
	void createDirectory(const tString & dirname);

	/**
	 * 指定されたファイルの stat を得る
	 * @param filename	ファイル名
	 * @return	stat の結果を表す辞書配列
	 */
	tObjectInterface * stat(const tString & filename);

	/**
	 * 指定されたファイルのストリームを得る
	 * @param filename	ファイル名
	 * @param flags		フラグ
	 * @return	ストリームオブジェクト
	 */
	tStreamInstance * open(const tString & filename,
		risse_uint32 flags);

	/**
	 * ディスク(等に)内容をフラッシュする
	 * @note	このメソッドが呼ばれたら、ファイルシステムはそれまでの
	 *			変更点を確実にディスクなどに永続化することが望まれる。
	 *			逆にこのメソッドが呼ばれずにプログラムが終了したり
	 *			ファイルシステムを開放した場合は、永続化する保証はない。
	 *			永続化されなかった場合、それまでの変更内容が中途半端に
	 *			反映された状態になるのか、あるいはそれまでの変更内容が
	 *			全くなかったかにされるかは、ファイルシステムの実装による。
	 */
	void flush();

	//-- FileSystem メンバ ここまで

	tString get_source() const { return BaseDirectory; }

private:
	/**
	 * path中に含まれる'/'を、OSネイティブなパス区切り記号に変更する
	 * @param path	パス名
	 * @return	OSネイティブなパス区切りに変更された後のパス名
	 */
	static wxString ConvertToNativePathDelimiter(const wxString & path);

	/**
	 * path中のディレクトリ名やファイル名の大文字や小文字がファイルシステム上のそれと一致するかどうかを調べる
	 * @param path_to_check	パス名(フルパスであること)
	 * @param raise			一致しなかった場合に例外を発生するかどうか
	 * @return	一致した場合に真、しなかった場合に偽
	 * @note	ファイルが「見つからない」場合は真が帰るので注意
	 */
	bool CheckFileNameCase(const wxString & path_to_check, bool raise = true);

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * "OSFS" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tOSFSClass, tClassBase)
private:
	tOSNativeStreamClass * OSNativeStreamClass; //!< OSNativeStream クラスインスタンス
public:
	/**
	 * OSNativeStreamClass クラスインスタンスを得る
	 */
	tOSNativeStreamClass * GetOSNativeStreamClass() const { return OSNativeStreamClass; }
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
} // namespace Risa


#endif
