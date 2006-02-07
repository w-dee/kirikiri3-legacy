//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveデコーダインターフェースの定義
//---------------------------------------------------------------------------
#ifndef _WAVEDECODERH_
#define _WAVEDECODERH_


#include "sound/Wave.h"


//---------------------------------------------------------------------------
//! @brief	 デコーダインターフェース
/*! @note
	一般的なデコーダの使用方法の流れは、
	1. インスタンスを作成する
	2. (必要であれば) SuggestFormat で欲しいファイル形式を提案する
	3. GetFormat でファイルフォーマットを取得する
	4. Render や SetPosition を必要回数呼び出す。
	となる。一度RenderやSetPositionを呼び出した後はファイル形式を変えることは
	できないし、許されない。
*/
//---------------------------------------------------------------------------
class tRisaWaveDecoder
{
public:
	virtual ~tRisaWaveDecoder() {};

	virtual void SuggestFormat(const tRisaWaveFormat & format) = 0;
		/*!< @brief Wave形式を提案する
			@note
			このインターフェースを利用する側が欲しいPCM形式を
			提案する。ただし、デコーダ側はこの形式に従う必要はない。
			また、利用する側はデコーダがこれに従うことを期待しては
			ならない。
		*/

	virtual void GetFormat(tRisaWaveFileInfo & fileinfo) = 0;
		/*!< @brief ファイル情報を取得する
			@note
			Retrieve PCM format, etc. */

	virtual bool Render(void *buf, risse_uint bufsamplelen, risse_uint& rendered) = 0;
		/*!< @brief サウンドをレンダリングする
			@note
			Render PCM from current position.
			where "buf" is a destination buffer, "bufsamplelen" is the buffer's
			length in sample granule, "rendered" is to be an actual number of
			written sample granule.
			returns whether the decoding is to be continued.
			because "redered" can be lesser than "bufsamplelen", the player
			should not end until the returned value becomes false.
		*/

	virtual bool SetPosition(risse_uint64 samplepos) = 0;
		/*!< @brief デコード位置を変更する
			@note
			Seek to "samplepos". "samplepos" must be given in unit of sample granule.
			returns whether the seeking is succeeded.
		*/
};
//---------------------------------------------------------------------------


#endif

