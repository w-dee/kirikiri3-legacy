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


#include "Wave.h"


//---------------------------------------------------------------------------
//! @brief	 デコーダインターフェース
//---------------------------------------------------------------------------
class tRisaWaveDecoder
{
public:
	virtual ~tRisaWaveDecoder() {};

	virtual void GetFormat(tRisaWaveFormat & format) = 0;
		/*!< @brief フォーマットを取得する
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

