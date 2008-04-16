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
#ifndef _FILESYSTEMH_
#define _FILESYSTEMH_

#include <wx/file.h>
#include <wx/datetime.h>
#include "risse/include/risseHashTable.h"
#include "risa/common/Singleton.h"
#include "risa/common/RisaThread.h"
#include "risa/common/RisseEngine.h"
#include "risse/include/risseString.h"
#include "risse/include/risseStream.h"
#include "risse/include/builtin/stream/risseStreamClass.h"
#include "risa/common/RisaException.h"

namespace Risa {
//---------------------------------------------------------------------------


class tFileSystemInstance;




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
} // namespace Risa


#endif