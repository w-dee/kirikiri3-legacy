//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OggVorbis デコーダ
//---------------------------------------------------------------------------

#ifndef OGGVORBISDECODERH
#define OGGVORBISDECODERH

#include "sound/WaveDecoder.h"
#include "risse/include/risse.h"
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief	 OggVorbis デコーダ
//---------------------------------------------------------------------------
class tRisaOggVorbisDecoder : public tRisaWaveDecoder
{
	tBinaryStream * Stream; //!< 入力ストリーム
	tRisaWaveFileInfo FileInfo; //!< サウンドファイル情報
	OggVorbis_File InputFile; //!< OggVorbis_File instance
	int CurrentSection;

public:
	//! @brief		コンストラクタ
	//! @param		filename   ファイル名
	tRisaOggVorbisDecoder(const tString & filename);

	// tRisaWaveDecoder をオーバーライドするもの
	//! @brief		デストラクタ
	//! @param		filename   ファイル名
	virtual ~tRisaOggVorbisDecoder();

	//! @brief		PCMフォーマットを提案する
	//! @param		format   PCMフォーマット
	virtual void SuggestFormat(const tRisaWaveFormat & format);

	//! @brief		サウンド情報を得る
	//! @param		fileinfo   情報を格納するための構造体
	virtual void GetFormat(tRisaWaveFileInfo & fileinfo);

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

	//! @brief		読み込み関数
	static size_t read_func(void *ptr,
		size_t size, size_t nmemb, void *datasource);

	//! @brief		シーク関数
	static int seek_func(void *datasource, ogg_int64_t offset, int whence);

	//! @brief		クローズ関数
	static int close_func(void *datasource);

	//! @brief		tell関数
	static long tell_func(void *datasource);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif


