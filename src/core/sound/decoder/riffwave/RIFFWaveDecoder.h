//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RIFF Wave デコーダ
//---------------------------------------------------------------------------

#ifndef RIFFWAVEDECODERH
#define RIFFWAVEDECODERH

#include "sound/WaveDecoder.h"
#include "risse/include/risse.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief	 RIFF Wave デコーダ
//---------------------------------------------------------------------------
class tRisaRIFFWaveDecoder : public tRisaWaveDecoder
{
	tBinaryStream * Stream; //!< 入力ストリーム
	tRisaWaveFileInfo FileInfo; //!< サウンド形式
	risse_uint64 DataStart; //!< ファイル中でのデータの開始位置
	risse_uint64 CurrentPos; //!< ファイル中でのデータの読み込み位置

public:
	//! @brief		コンストラクタ
	//! @param		filename   ファイル名
	tRisaRIFFWaveDecoder(const tString & filename);

	// tRisaWaveDecoder をオーバーライドするもの
	//! @brief		デストラクタ
	virtual ~tRisaRIFFWaveDecoder();

	virtual void SuggestFormat(const tRisaWaveFormat & format) {;}

	//! @brief		サウンド情報を得る
	//! @param		fileinfo   情報を格納するための構造体
	virtual void GetFormat(tRisaWaveFileInfo & format);

	//! @brief		サウンドをレンダリングする
	//! @param		buf					データ格納先バッファ
	//! @param		bufsamplelen		バッファのサンプル数(サンプルグラニュール単位)
	//! @param		rendered			実際にレンダリングが行われたサンプル数
	//! @return		サウンドの最後に達すると偽、それ以外は真
	//! @note		rendered != bufsamplelen の場合はサウンドの最後に達したことを
	//!				示さない。返値が偽になったかどうかでサウンドの最後に達したかどうかを
	//!				判断すること。
	virtual bool Render(void *buf, risse_uint bufsamplelen, risse_uint& rendered);

	//! @brief		デコード位置を変更する
	//! @param		samplepos		変更したい位置
	//! @return		デコード位置の変更に成功すると真
	virtual bool SetPosition(risse_uint64 samplepos);

private:
	//! @brief		サウンドを開く
	//! @return		開くことに成功すれば真
	bool Open();

	//! @brief		RIFFチャンクを探す
	//! @param		stream		ストリーム(このストリームの現在位置から検索が始まる)
	//! @param		chunk		探したいチャンク
	//! @return		指定された RIFF チャンクが見つかれば真
	static bool FindRIFFChunk(tBinaryStream * stream, const risse_uint8 *chunk);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif


