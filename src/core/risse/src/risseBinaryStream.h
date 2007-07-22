//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイナリストリーム
//---------------------------------------------------------------------------

#ifndef risseBinaryStreamH
#define risseBinaryStreamH

#include "risseTypes.h"
#include "risseGC.h"
#include "risseString.h"

#ifdef RISSE_SUPPORT_WX
	#include <wx/file.h>
	#include <wx/datetime.h>
#endif

namespace Risse
{
class tBinaryStream;

//---------------------------------------------------------------------------
//! @brief		tFileSystem::GetFileListAt で用いられるコールバックインターフェース
//---------------------------------------------------------------------------
class tFileSystemIterationCallback
{
public:
	//! @brief		ファイルが見つかった際に呼ばれる
	//! @param		filename		ファイル名
	//! @return		探索を続行する場合に真、中断したい場合に偽
	virtual bool OnFile(const tString & filename) = 0;

	//! @brief		ディレクトリが見つかった際に呼ばれる
	//! @param		filename		ディレクトリ名
	//! @return		探索を続行する場合に真、中断したい場合に偽
	virtual bool OnDirectory(const tString & dirname) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tFileSystem::Stat で返される構造体
//---------------------------------------------------------------------------
struct tStatStruc : public tAtomicCollectee
{
	// TODO: 今のところ wxWidgets にここが依存しているので注意
	risse_uint64	Size;	//!< ファイルサイズ。 risse_uint64_maxの場合は無効
#ifdef RISSE_SUPPORT_WX
	wxDateTime		MTime;	//!< ファイル修正時刻 (wxDateTime::IsValidで有効性をチェックのこと)
	wxDateTime		ATime;	//!< アクセス時刻 (wxDateTime::IsValidで有効性をチェックのこと)
	wxDateTime		CTime;	//!< 作成時刻 (wxDateTime::IsValidで有効性をチェックのこと)
#endif

	tStatStruc() { Clear(); }
	void Clear()
	{
		Size = risse_uint64_max;
#ifdef RISSE_SUPPORT_WX
		MTime = ATime = CTime = wxDateTime();
#endif
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムインターフェースクラス
//---------------------------------------------------------------------------
class tFileSystem
{
public: // 定数など
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
		omAppendBit = 3 //!< 追加
	};


public:
	//! @brief		ファイル一覧を取得する
	//! @param		dirname ディレクトリ名
	//! @param		callback コールバックオブジェクト
	//! @return		取得できたファイル数
	virtual size_t GetFileListAt(const tString & dirname,
		tFileSystemIterationCallback * callback) = 0;

	//! @brief		ファイルが存在するかどうかを得る
	//! @param		filename ファイル名
	//! @return		ファイルが存在する場合真
	virtual bool FileExists(const tString & filename) = 0;

	//! @brief		ディレクトリが存在するかどうかを得る
	//! @param		dirname ディレクトリ名
	//! @return		ディレクトリが存在する場合真
	virtual bool DirectoryExists(const tString & dirname) = 0;

	//! @brief		ファイルを削除する
	//! @param		filename ファイル名
	virtual void RemoveFile(const tString & filename) = 0;

	//! @brief		ディレクトリを削除する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを削除するかどうか
	virtual void RemoveDirectory(const tString & dirname,
		bool recursive = false) = 0;

	//! @brief		ディレクトリを作成する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを作成するかどうか
	virtual void CreateDirectory(const tString & dirname,
		bool recursive = false) = 0;

	//! @brief		指定されたファイルの stat を得る
	//! @param		filename ファイル名
	//! @param		struc stat 結果の出力先
	virtual void Stat(const tString & filename,
		tStatStruc & struc) = 0;

	//! @brief		指定されたファイルのストリームを得る
	//! @param		filename ファイル名
	//! @param		flags フラグ
	//! @return		ストリームオブジェクト
	virtual tBinaryStream * CreateStream(const tString & filename,
		risse_uint32 flags) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		バイナリストリームの基本クラス
//! @note		このクラスではシークが不可能な、本当の意味での「ストリーム」は扱わない
//! @note		各メソッドはスレッド保護されていない。
//---------------------------------------------------------------------------
class tBinaryStream : public tDestructee
	/* ストリームは通常「閉じる」必要があるためデストラクタを呼ぶようにする */
{
private:
	tString Name; //!< ストリームの名前

public:
	//! @param		基準位置の列挙
	enum tOrigin
	{
		soSet , //!< ストリームの先頭
		soCurrent, //!< ストリームの現在位置
		soEnd, //!< ストリームの終端
		soCur = soCurrent //!< ストリームの現在位置
	};

	//! @brief		ストリームの名前を設定する
	//! @param		name		名前
	//! @note		このクラス内ではこの名前は例外生成時に使用するだけであるが、
	//!				例外をユーザフレンドリーにするために、なるべく名前は設定するべき
	void SetName(const tString & name) { Name = name; }

	//! @brief		ストリームの名前を取得する
	//! @return		ストリームの名前を取得する
	const tString & GetName() { return Name; }

	//! @brief		指定位置にシークする
	//! @param		offset			基準位置からのオフセット (正の数 = ファイルの後ろの方)
	//! @param		whence			基準位置
	//! @return		このメソッドは成功すれば真、失敗すれば偽を返す
	//! @note		このメソッドは下位クラスで実装しなければならない。
	//!				エラーが発生した場合は、シーク位置を変えず、現在位置を保つべき
	virtual bool Seek(risse_int64 offset, tOrigin whence) { return false; }

	//! @brief		現在位置を取得する
	//! @return		現在位置(先頭からのオフセット)
	virtual risse_uint64 Tell() = 0;

	//! @brief		ストリームから読み込む
	//! @param		buffer		読み込んだデータを書き込む先のポインタ
	//! @param		read_size	読み込むサイズ
	//! @return		実際に読み込まれたサイズ
	virtual risse_size Read(void *buffer, risse_size read_size) { return 0; }

	//! @brief		ストリームに書き込む
	//! @param		buffer		書き込むデータを表すポインタ
	//! @param		read_size	書き込むサイズ
	//! @return		実際に書き込まれたサイズ
	virtual risse_uint Write(const void *buffer, risse_uint write_size) { return 0; }

	//! @brief		ストリームを現在位置で切りつめる
	//! @note		これを実装しない場合は例外が発生する
	virtual void Truncate();

	//! @brief		ストリームのサイズを得る
	//! @return		ストリームのサイズ
	//! @note		実装しなくても良いが、
	//!				実装した方が高いパフォーマンスを得られるようならば実装すべき
	virtual risse_uint64 GetSize();

	//! @brief		デストラクタ
	virtual ~tBinaryStream() {;}


public: // ユーティリティ
	//! @brief		現在位置を得る
	//! @return		現在位置
	risse_uint64 GetPosition() { return Tell(); }

	//! @brief		現在位置を設定する
	//! @param		pos		現在位置
	//! @note		シークに失敗した場合は例外が発生する
	void SetPosition(risse_uint64 pos);

	//! @brief		ストリームから読み込む
	//! @param		buffer		読み込んだデータを書き込む先のポインタ
	//! @param		read_size	読み込むサイズ
	//! @note		読み込みに失敗した場合は例外が発生する
	void ReadBuffer(void *buffer, risse_size read_size);

	//! @brief		ストリームに書き込む
	//! @param		buffer		書き込むデータを表すポインタ
	//! @param		read_size	書き込むサイズ
	//! @note		書き込みに失敗した場合は例外が発生する
	void WriteBuffer(const void *buffer, risse_size write_size);

	//! @brief		ストリームから64bit little endian 整数を読み込んで返す
	//! @return		読み込んだ値
	//! @note		読み込みに失敗した場合は例外が発生する
	risse_uint64 ReadI64LE();

	//! @brief		ストリームから32bit little endian 整数を読み込んで返す
	//! @return		読み込んだ値
	//! @note		読み込みに失敗した場合は例外が発生する
	risse_uint32 ReadI32LE();

	//! @brief		ストリームから16bit little endian 整数を読み込んで返す
	//! @return		読み込んだ値
	//! @note		読み込みに失敗した場合は例外が発生する
	risse_uint16 ReadI16LE();

	//! @brief		ストリームから8bit 整数を読み込んで返す
	//! @return		読み込んだ値
	//! @note		読み込みに失敗した場合は例外が発生する
	risse_uint8 ReadI8LE();
	//! @brief		ストリームから8bit 整数を読み込んで返す
	//! @return		読み込んだ値
	//! @note		読み込みに失敗した場合は例外が発生する
	risse_uint8 ReadI8() { return ReadI8LE(); }

};
//---------------------------------------------------------------------------

} // namespace Risse
#endif
