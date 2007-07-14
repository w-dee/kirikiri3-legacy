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
#include <wx/file.h>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		OS ネイティブファイルストリーム
//---------------------------------------------------------------------------
class tRisaOSNativeStream : public tBinaryStream
{
protected:
	wxFile File;

public:
	//! @brief		コンストラクタ
	tRisaOSNativeStream(const wxString & filename, risse_uint32 flags);

	//! @brief		デストラクタ
	~tRisaOSNativeStream();

	//! @brief		シーク
	//! @param		offset 移動オフセット
	//! @param		whence 移動オフセットの基準 (RISSE_BS_SEEK_* 定数)
	//! @return		移動後のファイルポインタ
	risse_uint64 Seek(risse_int64 offset, risse_int whence);

	//! @brief		読み込み
	//! @param		buffer 読み込み先バッファ
	//! @param		read_size 読み込むバイト数
	//! @return		実際に読み込まれたバイト数
	risse_size Read(void *buffer, risse_size read_size);

	//! @brief		書き込み
	//! @param		buffer 書き込むバッファ
	//! @param		read_size 書き込みたいバイト数
	//! @return		実際に書き込まれたバイト数
	risse_size Write(const void *buffer, risse_size write_size);

	//! @brief		ファイルの終わりを現在のポインタに設定する
	void SetEndOfFile();

	//! @brief		サイズを得る
	//! @return		このストリームのサイズ
	risse_uint64 GetSize();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		OS ファイルシステム
//---------------------------------------------------------------------------
class tRisaOSFS : public tRisaFileSystem
{
	tRisaCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション

	wxString BaseDirectory; //!< このファイルシステムがマウントしているOSのディレクトリ
	bool CheckCase; //!< ファイル名の大文字・小文字をチェックする場合は真

public:
	//! @brief		コンストラクタ
	tRisaOSFS(const tString & basedir, bool checkcase = true);

	//-- tRisaFileSystem メンバ

	//! @brief		デストラクタ
	~tRisaOSFS();


	//! @brief		ファイル一覧を取得する
	//! @param		dirname ディレクトリ名
	//! @param		callback コールバックオブジェクト
	//! @return		取得できたファイル数
	size_t GetFileListAt(const tString & dirname,
		tRisaFileSystemIterationCallback * callback);

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
	void Stat(const tString & filename, tRisaStatStruc & struc);

	//! @brief		指定されたファイルのストリームを得る
	//! @param		filename ファイル名
	//! @param		flags フラグ
	//! @return		ストリームオブジェクト
	tBinaryStream * CreateStream(const tString & filename, risse_uint32 flags);

	//-- tRisaFileSystem メンバ ここまで
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
} // namespace Risa


#endif
