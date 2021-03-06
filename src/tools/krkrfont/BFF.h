//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief BFF (Bitmap Font File) ファイル用構造体
//---------------------------------------------------------------------------

#ifndef _BFF_H_
#define _BFF_H_

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * BFF ファイルタグ
 */
enum tBFFFileTags
{
	bftCharacteMap		= 0x4d524843		/*!< "CHRM" 文字マップ */,
	bftKerningVector	= 0x564e524b		/*!< "KRNV" カーニングベクタ */,
	bftBitmaps			= 0x504d5442		/*!< "BTMP" ビットマップ */,
	bftNone				= 0					/*!< 未定義 */
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * BFF ファイルヘッダ
 * @note	ここで定義される構造体は実際にファイル中のバイト表現と同じ。
 *			ただし、ファイル中の各整数値の表現は Little Endian なので注意。
 *			この構造体の直後に tBFFDirectory 構造体が、NumFiles メンバで
 *			指定された個数分続く。
 */
#pragma pack(push, 1)
struct tBFFHeader
{
	risse_uint8 Magic[4];		//!< マジック("BFF\0"固定)
	risse_uint8 Version[1]; 	//!< バージョン(現在は1)
	risse_uint8 Reserved0[3];	//!< 予備(すべて0で埋めること)
	risse_uint32 NumFiles;		//!< ディレクトリ中に存在するアイテム数
};
#pragma pack(pop)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * BFF ディレクトリ
 * @note	ここで定義される構造体は実際にファイル中のバイト表現と同じ。
 *			ただし、ファイル中の各整数値の表現は Little Endian なので注意。
 */
#pragma pack(push, 1)
struct tBFFDirectory
{
	risse_uint32 Tag;			//!< タグ(4バイト) tBFFFileTags のいずれか
	risse_uint32 Size;			//!< アイテムサイズ
	risse_uint32 Offset;		//!< アイテムオフセット (BFFファイル先頭から)
	risse_uint32 Reserved;		//!< 予備(0にすること)
};
#pragma pack(pop)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * BFF 文字マップ (CHRM)
 * @note	ここで定義される構造体は実際にファイル中のバイト表現と同じ。
 *			ただし、ファイル中の各整数値の表現は Little Endian なので注意。
 */
#pragma pack(push, 1)
struct tBFFCharacterMap
{
	risse_uint32 Unicode;		//!< Unicode 文字
	risse_int32  CellIncX;		//!< 一文字進めるの必要なX方向のピクセル数(64倍されている数値なので注意)
	risse_int32  CellIncY;		//!< 一文字進めるの必要なY方向のピクセル数(64倍されている数値なので注意)
	risse_uint32 Offset;		//!< ビットマップ中でのオフセット (0=ビットマップなしの場合)
};
#pragma pack(pop)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * BFF カーニングベクタ (KRNV)
 * @note	ここで定義される構造体は実際にファイル中のバイト表現と同じ。
 *			ただし、ファイル中の各整数値の表現は Little Endian なので注意。
 */
#pragma pack(push, 1)
struct tBFFKerningVector
{
	risse_uint32 Unicode1;		//!< 前の文字
	risse_uint32 Unicode2;		//!< 次の文字
	risse_int32  AdjustX;		//!< 一文字進めるの必要なX方向のピクセル調整値(64倍されている数値なので注意)
	risse_int32  AdjustY;		//!< 一文字進めるの必要なY方向のピクセル調整値(64倍されている数値なので注意)
};
#pragma pack(pop)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * BFF ビットマップ (BTMP) のヘッダ
 * @note	ここで定義される構造体は実際にファイル中のバイト表現と同じ。
 *			ただし、ファイル中の各整数値の表現は Little Endian なので注意。
 *			BFF の BTMP の先頭には 4 バイトの 0 が入っている(仕様)。
 *			BFF の BTMP 内ビットマップは BFF の CHRM の Offset メンバから
 *			参照されるオフセットによりアクセスする。
 *			この構造体の直後には圧縮されたビットマップデータが来る。
 */
#pragma pack(push, 1)
struct tBFFBitmapHeader
{
	risse_uint16 Flags;			//!< 未定義: 常に 0 であること(将来的にはビットマップ形式とか圧縮形式とかを表すのに使う)
	risse_int16  OriginX;		//!< 描画点からブラックボックスの左端までの位置
	risse_int16  OriginY;		//!< 描画点からブラックボックスの上端までの位置
	risse_uint16 BlackBoxW;		//!< ブラックボックスの幅(ie. ビットマップのサイズ)
	risse_uint16 BlackBoxH;		//!< ブラックボックスの高さ(ie. ビットマップのサイズ)
};
#pragma pack(pop)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif


