//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RIFF Wave デコーダ
//---------------------------------------------------------------------------

#ifndef RIFFWAVEDECODERH
#define RIFFWAVEDECODERH

#include "risa/packages/risa/sound/WaveDecoder.h"
#include "risa/packages/risa/fs/FSManager.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * RIFF Wave デコーダ
 */
class tRIFFWaveDecoder : public tWaveDecoder
{
	tStreamAdapter Stream; //!< 入力ストリーム
	tWaveFileInfo FileInfo; //!< サウンド形式
	risse_uint64 DataStart; //!< ファイル中でのデータの開始位置
	risse_uint64 CurrentPos; //!< ファイル中でのデータの読み込み位置

public:
	/**
	 * コンストラクタ
	 * @param filename	ファイル名
	 */
	tRIFFWaveDecoder(const tString & filename);

	// tWaveDecoder をオーバーライドするもの

	/**
	 * デストラクタ(たぶんよばれない)
	 */
	virtual ~tRIFFWaveDecoder() {;}

	virtual void SuggestFormat(const tWaveFormat & format) {;}

	/**
	 * サウンド情報を得る
	 * @param fileinfo	情報を格納するための構造体
	 */
	virtual void GetFormat(tWaveFileInfo & format);

	/**
	 * サウンドをレンダリングする
	 * @param buf			データ格納先バッファ
	 * @param bufsamplelen	バッファのサンプル数(サンプルグラニュール単位)
	 * @param rendered		実際にレンダリングが行われたサンプル数
	 * @return	サウンドの最後に達すると偽、それ以外は真
	 * @note	rendered != bufsamplelen の場合はサウンドの最後に達したことを
	 *			示さない。返値が偽になったかどうかでサウンドの最後に達したかどうかを
	 *			判断すること。
	 */
	virtual bool Render(void *buf, risse_uint bufsamplelen, risse_uint& rendered);

	/**
	 * デコード位置を変更する
	 * @param samplepos	変更したい位置
	 * @return	デコード位置の変更に成功すると真
	 */
	virtual bool SetPosition(risse_uint64 samplepos);

private:
	/**
	 * サウンドを開く
	 * @return	開くことに成功すれば真
	 */
	bool Open();

	/**
	 * RIFFチャンクを探す
	 * @param stream	ストリーム(このストリームの現在位置から検索が始まる)
	 * @param chunk		探したいチャンク
	 * @return	指定された RIFF チャンクが見つかれば真
	 */
	static bool FindRIFFChunk(tStreamAdapter stream, const risse_uint8 *chunk);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif


