//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief フォントドライバ管理
//---------------------------------------------------------------------------
#ifndef _FONTDRIVER_H_
#define _FONTDRIVER_H_


//---------------------------------------------------------------------------
//! @brief		１グリフのメトリックを表す構造体
//---------------------------------------------------------------------------
struct tTVPGlyphMetrics
{
	tjs_int CellIncX;		//!< 一文字進めるの必要なX方向のピクセル数
	tjs_int CellIncY;		//!< 一文字進めるの必要なY方向のピクセル数
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		１グリフを表すクラス
//---------------------------------------------------------------------------
class tTVPGlyphBitmap
{
	tjs_uint RefCount;				//!< 参照カウンタ
	tjs_uint8 * Data;				//!< ビットマップデータ(8bpp, グレースケール)
	tjs_int Pitch;					//!< ビットマップデータの横方向のバイト数
	tjs_int OriginX;				//!< 描画点からブラックボックスの左端までの位置
	tjs_int OriginY;				//!< 描画点からブラックボックスの上端までの位置
	tjs_uint BlackBoxW;				//!< ブラックボックスの幅
	tjs_uint BlackBoxH;				//!< ブラックボックスの高さ
	tTVPGlyphMetrics	Metrics;	//!< メトリック

public:
	tTVPGlyphBitmap(
		tjs_uint8 * indata,
		tjs_int inpitch,
		tjs_int originx, tjs_int originy,
		tjs_uint blackboxw, tjs_uint blackboxh,
		const tTVPGlyphMetrics & metrics);
	tTVPGlyphBitmap(const tTVPGlyphBitmap & ref);

private:
	// デストラクタはprivate
	~tTVPGlyphBitmap();

public:
	void AddRef();
	void Release();

public:
	// アクセサの類
	const tjs_uint8 * GetData() const { return Data; }
	tjs_int GetPitch() const { return Pitch; }
	tjs_int GetOriginX() const { return OriginX; }
	tjs_int GetOriginY() const { return OriginY; }
	tjs_uint GetBlackBoxW() const { return BlackBoxW; }
	tjs_uint GetBlackBoxH() const { return BlackBoxH; }
	const tTVPGlyphMetrics & GetMetrics() const { return Metrics; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Face を開くときのオプションなど
//---------------------------------------------------------------------------
#define	TVP_GET_FACE_INDEX_FROM_OPTIONS(x) ((x) & 0xff) //!< オプション整数からFaceインデックスを取り出すマクロ
#define	TVP_FACE_OPTIONS_FACE_INDEX(x) ((x) & 0xff) //!< Faceインデックスをオプション整数に変換するマクロ
#define	TVP_FACE_OPTIONS_FILE 0x00010000 //!< フォント名ではなくてファイル名によるフォントの指定を行う


#endif

