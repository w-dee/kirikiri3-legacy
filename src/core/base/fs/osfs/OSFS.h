//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSの実装
//---------------------------------------------------------------------------
#ifndef _OSFSH_
#define _OSFSH_

#include "base/fs/common/FSManager.h"
#include "base/utils/RisaThread.h"
#include "risse/include/risseStream.h"
#include "risse/include/risseWCString.h"
#include "risse/include/risseStreamClass.h"
#include <wx/file.h>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		OS ネイティブファイルストリーム(OSNativeStreamクラス)
//---------------------------------------------------------------------------
class tOSNativeStreamInstance : public tStreamInstance
{
protected:
	//! @brief		内部情報用構造体(GCの対象で、回収に際してデストラクタが呼ばれる)
	struct tInternal : public tDestructee
	{
		wxFile File;
	} * Internal;

public:
	//! @brief		コンストラクタ
	tOSNativeStreamInstance() { Internal = NULL; }

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tOSNativeStreamInstance();

public: // risse 用メソッドとか
	void construct() {;}
	void initialize(const tString & path, risse_uint32 flags, const tNativeCallInfo &info);

	//! @brief		ストリームを閉じる
	//! @note		基本的にはこれでストリームを閉じること。
	//!				このメソッドでストリームを閉じなかった場合の動作は
	//!				「未定義」である
	void dispose();

	//! @brief		指定位置にシークする
	//! @param		offset			基準位置からのオフセット (正の数 = ファイルの後ろの方)
	//! @param		whence			基準位置
	//! @return		このメソッドは成功すれば真、失敗すれば偽を返す
	bool seek(risse_int64 offset, tOrigin whence);

	//! @brief		現在位置を取得する
	//! @return		現在位置(先頭からのオフセット)
	risse_uint64 tell();

	//! @brief		ストリームから読み込む
	//! @param		buf		読み込んだデータを書き込む先
	//! @return		実際に読み込まれたサイズ
	risse_size read(const tOctet & buf);

	//! @brief		ストリームに書き込む
	//! @param		buf		書き込むデータ
	//! @return		実際に書き込まれたサイズ
	risse_uint write(const tOctet & buf);

	//! @brief		ストリームを現在位置で切りつめる
	void truncate();

	//! @brief		サイズを得る
	//! @return		このストリームのサイズ
	risse_uint64 get_size();
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		"OSNativeStream" クラス
//---------------------------------------------------------------------------
class tOSNativeStreamClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tOSNativeStreamClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
//! @brief		OS ファイルシステム
//---------------------------------------------------------------------------
class tOSFSInstance : public tFileSystemInstance
{
	tWCString BaseDirectory; //!< このファイルシステムがマウントしているOSのディレクトリ
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

	//-- tFileSystem メンバ

	//! @brief		ファイル一覧を取得する
	//! @param		dirname ディレクトリ名
	//! @param		callback コールバックオブジェクト
	//! @return		取得できたファイル数
	size_t GetFileListAt(const tString & dirname,
		tFileSystemIterationCallback * callback);

	//! @brief		ファイルが存在するかどうかを得る
	//! @param		filename ファイル名
	//! @return		ファイルが存在する場合真
	bool FileExists(const tString & filename);

	//! @brief		ディレクトリが存在するかどうかを得る
	//! @param		dirname ディレクトリ名
	//! @return		ディレクトリが存在する場合真
	bool DirectoryExists(const tString & dirname);

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
	void Stat(const tString & filename, tStatStruc & struc);

	//! @brief		指定されたファイルのストリームを得る
	//! @param		filename ファイル名
	//! @param		flags フラグ
	//! @return		ストリームオブジェクト
	tStreamInstance * CreateStream(const tString & filename, risse_uint32 flags);

	//-- tFileSystem メンバ ここまで
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

public: // Risse 用メソッドなど
	void construct();
	void initialize(const tString & base_dir, const tNativeCallInfo &info);
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
} // namespace Risa


#endif
