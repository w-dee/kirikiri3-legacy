//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイナリストリーム
//---------------------------------------------------------------------------

#ifndef risseStreamH
#define risseStreamH

#include "risseTypes.h"
#include "risseGC.h"
#include "risseString.h"
#include "risseVariant.h"
#include "risseModule.h"

namespace Risse
{
class tStreamInstance;
//---------------------------------------------------------------------------
/**
 * ストリームに関連する定数など
 */
class tStreamConstants
{
public:
	/**
	 * 基準位置の列挙
	 */
	enum tOrigin
	{
		soSet  /*!< ストリームの先頭 */,
		soCurrent /*!< ストリームの現在位置 */,
		soEnd /*!< ストリームの終端 */,
		soCur = soCurrent /*!< ストリームの現在位置 */
	};
};
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
/**
 * Stream クラスのインスタンスを扱いやすくするためのアダプタ
 * @note	tStreamInstance ( extends tObjectBase ) 内の各メソッドを呼ぶためには
 *			Operate メソッドの呼び出しなどいろいろ煩雑である。このアダプタを
 *			使えば、メソッドの呼び出しなどはこれが代理して行うようになるので、
 *			扱いが簡単になる。
 */
class tStreamAdapter : public tCollectee, public tStreamConstants
{
	tStreamInstance * Stream; //!< Stream クラスのインスタンス
public:
	/**
	 * コンストラクタ(tStreamInstanceから)
	 * @param stream	tStreamInstanceのインスタンス
	 */
	tStreamAdapter(tStreamInstance * stream) { Stream = stream; }

	/**
	 * コンストラクタ(tVariantから)
	 * @param stream	tStreamInstanceのインスタンスを含む値
	 */
	tStreamAdapter(const tVariant & stream);

	/**
	 * ストリームの名前を設定する
	 * @param name	名前
	 * @note	このクラス内ではこの名前は例外生成時に使用するだけであるが、
	 *			例外をユーザフレンドリーにするために、なるべく名前は設定するべき
	 */
	void SetName(const tString & name);

	/**
	 * ストリームの名前を取得する
	 * @return	ストリームの名前を取得する
	 */
	tString GetName();

	/**
	 * ストリームを閉じる
	 * @note	基本的にはこれでストリームを閉じること。
	 *			このメソッドでストリームを閉じなかった場合の動作は
	 *			「未定義」である
	 */
	void Dispose();

	/**
	 * 指定位置にシークする
	 * @param offset	基準位置からのオフセット (正の数 = ファイルの後ろの方)
	 * @param whence	基準位置
	 * @return	このメソッドは成功すれば真、失敗すれば偽を返す
	 * @note	このメソッドは下位クラスで実装しなければならない。
	 *			エラーが発生した場合は、シーク位置を変えず、現在位置を保つべき
	 */
	bool Seek(risse_int64 offset, tOrigin whence);

	/**
	 * 現在位置を取得する
	 * @return	現在位置(先頭からのオフセット)
	 */
	risse_uint64 Tell();

	/**
	 * ストリームから読み込む
	 * @param buffer	読み込んだデータを書き込む先のポインタ
	 * @param read_size	読み込むサイズ
	 * @return	実際に読み込まれたサイズ
	 * @note	このメソッドは Risse メソッドの read のラッパーではなくて
	 *			get のラッパーなので注意すること
	 */
	risse_size Read(void *buffer, risse_size read_size);

	/**
	 * ストリームに書き込む
	 * @param buffer	書き込むデータを表すポインタ
	 * @param read_size	書き込むサイズ
	 * @return	実際に書き込まれたサイズ
	 * @note	このメソッドは Risse メソッドの write のラッパーではなくて
	 *			put のラッパーなので注意すること
	 */
	risse_size Write(const void *buffer, risse_size write_size);

	/**
	 * ストリームを現在位置で切りつめる
	 * @note	これを実装しない場合は例外が発生する
	 */
	void Truncate();

	/**
	 * ストリームのサイズを得る
	 * @return	ストリームのサイズ
	 * @note	実装しなくても良いが、
	 *			実装した方が高いパフォーマンスを得られるようならば実装すべき
	 */
	risse_uint64 GetSize();

	/**
	 * ストリームをフラッシュする(書き込みバッファをフラッシュする)
	 */
	void Flush();


public: // ユーティリティ
	/**
	 * 現在位置を得る
	 * @return	現在位置
	 */
	risse_uint64 GetPosition();

	/**
	 * 現在位置を設定する
	 * @param pos	現在位置
	 * @note	シークに失敗した場合は例外が発生する
	 */
	void SetPosition(risse_uint64 pos);

	/**
	 * ストリームから読み込む
	 * @param buffer	読み込んだデータを書き込む先のポインタ
	 * @param read_size	読み込むサイズ
	 * @note	読み込みに失敗した場合は例外が発生する
	 * @note	このメソッドは Risse メソッドの readBuffer のラッパーではなくて
	 *			get のラッパーなので注意すること
	 */
	void ReadBuffer(void *buffer, risse_size read_size);

	/**
	 * ストリームに書き込む
	 * @param buffer	書き込むデータを表すポインタ
	 * @param read_size	書き込むサイズ
	 * @note	書き込みに失敗した場合は例外が発生する
	 * @note	このメソッドは Risse メソッドの writeBuffer のラッパーではなくて
	 *			put のラッパーなので注意すること
	 */
	void WriteBuffer(const void *buffer, risse_size write_size);

	/**
	 * ストリームから64bit little endian 整数を読み込んで返す
	 * @return	読み込んだ値
	 * @note	読み込みに失敗した場合は例外が発生する
	 */
	risse_uint64 ReadI64LE();

	/**
	 * ストリームから32bit little endian 整数を読み込んで返す
	 * @return	読み込んだ値
	 * @note	読み込みに失敗した場合は例外が発生する
	 */
	risse_uint32 ReadI32LE();

	/**
	 * ストリームから16bit little endian 整数を読み込んで返す
	 * @return	読み込んだ値
	 * @note	読み込みに失敗した場合は例外が発生する
	 */
	risse_uint16 ReadI16LE();

	/**
	 * ストリームから8bit 整数を読み込んで返す
	 * @return	読み込んだ値
	 * @note	読み込みに失敗した場合は例外が発生する
	 */
	risse_uint8 ReadI8LE();
	/**
	 * ストリームから8bit 整数を読み込んで返す
	 * @return	読み込んだ値
	 * @note	読み込みに失敗した場合は例外が発生する
	 */
	risse_uint8 ReadI8() { return ReadI8LE(); }

	/**
	 * ストリームに64bit little endian 整数を書き込む
	 * @param v	書き込む値
	 * @note	書き込みに失敗した場合は例外が発生する
	 */
	void Write64LE(risse_uint64 v);

	/**
	 * ストリームに32bit little endian 整数を書き込む
	 * @param v	書き込む値
	 * @note	書き込みに失敗した場合は例外が発生する
	 */
	void WriteI32LE(risse_uint32 v);

	/**
	 * ストリームに16bit little endian 整数を書き込む
	 * @param v	書き込む値
	 * @note	書き込みに失敗した場合は例外が発生する
	 */
	void WriteI16LE(risse_uint16 v);

	/**
	 * ストリームに8bit 整数を書き込む
	 * @param v	書き込む値
	 * @note	書き込みに失敗した場合は例外が発生する
	 */
	void WriteI8LE(risse_uint8 v);
	/**
	 * ストリームに8bit 整数を書き込む
	 * @param v	書き込む値
	 * @note	書き込みに失敗した場合は例外が発生する
	 */
	void WriteI8(risse_uint8 v) { WriteI8LE(v); }

};
//---------------------------------------------------------------------------

} // namespace Risse
#endif
