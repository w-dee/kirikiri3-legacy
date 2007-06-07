//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4ファイルシステムの実装
//---------------------------------------------------------------------------

#ifndef XP4FSH
#define XP4FSH

#include <boost/shared_ptr.hpp>
#include "base/fs/common/FSManager.h"
#include "base/fs/xp4fs/XP4Archive.h"
#include "base/utils/RisaThread.h"



//---------------------------------------------------------------------------
//! @brief		XP4FS の実装
//---------------------------------------------------------------------------
class tRisaXP4FS : public tRisaFileSystem
{
	tRisaCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション

	//! @brief ファイル一個一個の情報
	struct tFileItemBasicInfo
	{
		risse_size ArchiveIndex; //!< どのアーカイブに含まれるか
		risse_size FileIndex; //!< そのアーカイブ内でのファイルのインデックス
	};

	//! @brief 
	struct tFileItemInfo : public tFileItemBasicInfo
	{
		tRisseString Name; //!< 名前
		tFileItemInfo(const tFileItemBasicInfo & info, const tRisseString & name) :
			tFileItemBasicInfo(info), Name(name) {;} //!< コンストラクタ
		bool operator < (const tFileItemInfo & rhs) const 
			{ return Name < rhs.Name; } //!< 比較関数
	};

	std::vector<boost::shared_ptr<tRisaXP4Archive> > Archives; //!< アーカイブの配列
	std::vector<tFileItemInfo> FileItems; //! ファイルの情報の配列

public:
	//! @brief		コンストラクタ
	tRisaXP4FS(const tRisseString & name);

	//-- tRisaFileSystem メンバ
	//! @brief		デストラクタ
	~tRisaXP4FS();

	//! @brief		ファイル一覧を取得する
	//! @param		dirname ディレクトリ名
	//! @param		callback コールバックオブジェクト
	//! @return		取得できたファイル数
	size_t GetFileListAt(const tRisseString & dirname,
		tRisaFileSystemIterationCallback * callback);

	//! @brief		ファイルが存在するかどうかを得る
	//! @param		filename ファイル名
	//! @return		ファイルが存在する場合真
	bool FileExists(const tRisseString & filename);

	//! @brief		ディレクトリが存在するかどうかを得る
	//! @param		dirname ディレクトリ名
	//! @return		ディレクトリが存在する場合真
	bool DirectoryExists(const tRisseString & dirname);

	//! @brief		ファイルを削除する
	//! @param		filename ファイル名
	void RemoveFile(const tRisseString & filename);

	//! @brief		ディレクトリを削除する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを削除するかどうか
	void RemoveDirectory(const tRisseString & dirname, bool recursive = false);

	//! @brief		ディレクトリを作成する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを作成するかどうか
	void CreateDirectory(const tRisseString & dirname, bool recursive = false);

	//! @brief		指定されたファイルの stat を得る
	//! @param		filename ファイル名
	//! @param		struc stat 結果の出力先
	void Stat(const tRisseString & filename, tRisaStatStruc & struc);

	//! @brief		指定されたファイルのストリームを得る
	//! @param		filename ファイル名
	//! @param		flags フラグ
	//! @return		ストリームオブジェクト
	tRisseBinaryStream * CreateStream(const tRisseString & filename, risse_uint32 flags);

	//-- tRisaFileSystem メンバ ここまで
private:
	//! @brief		name ファイル名で始まる最初の FileItems内のインデックスを得る
	//! @param		name 名前
	//! @return		FileItems内のインデックス (見つからなかった場合は (risse_size)-1 が返る)
	risse_size GetFileItemStartIndex(const tRisseString & name);

	//! @brief		name に対応する FileItems内のインデックスを得る
	//! @param		name 名前
	//! @return		FileItems内のインデックス (見つからなかった場合は (risse_size)-1 が返る)
	//! @note		GetFileItemStartIndex と違い、その名前とぴったり一致しない限りは見つからないとみなす
	risse_size GetFileItemIndex(const tRisseString & name);

};
//---------------------------------------------------------------------------




#endif
