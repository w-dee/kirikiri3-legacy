//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
	risse_int CellIncX;		//!< 一文字進めるの必要なX方向のピクセル数(64倍されている数値なので注意)
	risse_int CellIncY;		//!< 一文字進めるの必要なY方向のピクセル数(64倍されている数値なので注意)
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		１グリフを表すクラス
//---------------------------------------------------------------------------
class tTVPGlyphBitmap
{
	risse_uint RefCount;				//!< 参照カウンタ
	risse_uint8 * Data;				//!< ビットマップデータ(8bpp, グレースケール)
	risse_int Pitch;					//!< ビットマップデータの横方向のバイト数
	risse_int OriginX;				//!< 描画点からブラックボックスの左端までの位置
	risse_int OriginY;				//!< 描画点からブラックボックスの上端までの位置
	risse_uint BlackBoxW;				//!< ブラックボックスの幅
	risse_uint BlackBoxH;				//!< ブラックボックスの高さ
	tTVPGlyphMetrics	Metrics;	//!< メトリック

public:
	tTVPGlyphBitmap(
		risse_uint8 * indata,
		risse_int inpitch,
		risse_int originx, risse_int originy,
		risse_uint blackboxw, risse_uint blackboxh,
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
	const risse_uint8 * GetData() const { return Data; }
	risse_int GetPitch() const { return Pitch; }
	risse_int GetOriginX() const { return OriginX; }
	risse_int GetOriginY() const { return OriginY; }
	risse_uint GetBlackBoxW() const { return BlackBoxW; }
	risse_uint GetBlackBoxH() const { return BlackBoxH; }
	const tTVPGlyphMetrics & GetMetrics() const { return Metrics; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Face を開くときのオプションなど
//---------------------------------------------------------------------------
#define	TVP_GET_FACE_INDEX_FROM_OPTIONS(x) ((x) & 0xff) //!< オプション整数からFaceインデックスを取り出すマクロ
#define	TVP_FACE_OPTIONS_FACE_INDEX(x) ((x) & 0xff) //!< Faceインデックスをオプション整数に変換するマクロ
#define	TVP_FACE_OPTIONS_FILE 0x00010000 //!< フォント名ではなくてファイル名によるフォントの指定を行う
#define TVP_FACE_OPTIONS_NO_HINTING 0x00020000 //!< ヒンティングを行わない
#define TVP_FACE_OPTIONS_FORCE_AUTO_HINTING 0x00020000 //!< 強制的に auto hinting を行う
#define TVP_FACE_OPTIONS_NO_ANTIALIASING 0x00040000 //!< アンチエイリアスを行わない

#endif

