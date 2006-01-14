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
#include "prec.h"
#include "FontDriver.h"


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		indata 入力ビットマップ
//! @param		inpitch 入力ビットマップの横方向のバイト数
//! @param		originx   描画点からブラックボックスの左端までの位置
//! @param		originy   描画点からブラックボックスの上端までの位置
//! @param		blackboxw ブラックボックスの幅
//! @param		blackboxh ブラックボックスの高さ
//! @param		metrics メトリック
//! @note		入力ビットマップのサイズは blackboxw と blackboxh で指定
//---------------------------------------------------------------------------
tTVPGlyphBitmap::tTVPGlyphBitmap(
	risse_uint8 * indata,
	risse_int inpitch,
	risse_int originx, risse_int originy,
	risse_uint blackboxw, risse_uint blackboxh,
	const tTVPGlyphMetrics & metrics)
{
	// フィールドのクリア
	RefCount = 1; // 参照カウンタの初期値は 1
	Data = NULL;

	// Metrics やビットマップ情報のコピー
	Metrics = metrics;
	OriginX = originx;
	OriginY = originy;
	BlackBoxW = blackboxw;
	BlackBoxH = blackboxh;

	// サイズのチェック
	if(BlackBoxW != 0 && BlackBoxH != 0)
	{
		try
		{
			// ビットマップをコピー

			//- 横方向のピッチを計算
			// MMX 等の使用を考えて横方向は 8 バイトでアライン
			Pitch = (((blackboxw - 1) >> 3) + 1) << 3;

			//- バイト数を計算してメモリを確保
			Data = new risse_uint8 [Pitch * blackboxh];

			//- ビットマップをコピー
			for(risse_uint y = 0; y < blackboxh; y++)
			{
				memcpy(
					Data + Pitch * y,
					indata + inpitch * y,
					blackboxw);
			}

		}
		catch(...)
		{
			if(Data) delete [] Data;
			throw;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コピーコンストラクタ
//! @param		ref 参照オブジェクト
//---------------------------------------------------------------------------
tTVPGlyphBitmap::tTVPGlyphBitmap(const tTVPGlyphBitmap & ref)
{
	// コピーコンストラクタは未サポート
	// TODO: ちゃんとした例外オブジェクトを投げよう
	throw "unimplemented: tTVPGlyphBitmap::tTVPGlyphBitmap(const tTVPGlyphBitmap & ref)";
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPGlyphBitmap::~tTVPGlyphBitmap()
{
	if(Data) delete [] Data;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		参照カウンタを一つ増やす
//---------------------------------------------------------------------------
void tTVPGlyphBitmap::AddRef()
{
	++RefCount;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		参照カウンタを一つ減らし、0になればオブジェクトを消滅させる
//---------------------------------------------------------------------------
void tTVPGlyphBitmap::Release()
{
	if(--RefCount == 0)
		delete this;
}
//---------------------------------------------------------------------------














