//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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
#include "risseHashTable.h"
#include "risa/common/Singleton.h"
#include "risa/common/RisaThread.h"
#include "risa/common/RisseEngine.h"
#include "risseString.h"
#include "risseStream.h"
#include "builtin/stream/risseStreamClass.h"
#include "risa/common/RisaException.h"

namespace Risa {
//---------------------------------------------------------------------------


class tFileSystemInstance;




//---------------------------------------------------------------------------
/**
 * ファイルシステム基底インスタンス("FileSystem" クラスのインスタンス)
 */
class tFileSystemInstance : public tObjectBase, public tFileOpenModes
{
public: // コンストラクタ
	tFileSystemInstance() {;}

public: // 定数など


public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

	//-- サブクラスで実装すべき物

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
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
/**
 * "FileSystem" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tFileSystemClass, tClassBase, tFileSystemInstance, itNoInstance)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa


#endif
