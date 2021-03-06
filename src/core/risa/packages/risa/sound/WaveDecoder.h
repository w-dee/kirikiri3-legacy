//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveデコーダインターフェースの定義
//---------------------------------------------------------------------------
#ifndef _WAVEDECODERH_
#define _WAVEDECODERH_


#include "risa/packages/risa/sound/Wave.h"
#include "risa/common/Singleton.h"
#include <map>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * デコーダインターフェース
 */
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
class tWaveDecoder : public tCollectee
{
public:
	virtual ~tWaveDecoder() {};

	virtual void SuggestFormat(const tWaveFormat & format) = 0;
		/*!< @brief Wave形式を提案する
			@note
			このインターフェースを利用する側が欲しいPCM形式を
			提案する。ただし、デコーダ側はこの形式に従う必要はない。
			また、利用する側はデコーダがこれに従うことを期待しては
			ならない。
		*/

	virtual void GetFormat(tWaveFileInfo & fileinfo) = 0;
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


//---------------------------------------------------------------------------
/**
 * デコーダファクトリ
 * @note	Risa は 拡張子で音楽形式を判断する。ファクトリの登録は
 *			tWaveDecoderFactoryManager を通して行うこと。
 */
class tWaveDecoderFactory : public tCollectee
{
public:
	virtual ~tWaveDecoderFactory() {} //!< デストラクタ(おそらく呼ばれない)

	virtual tWaveDecoder * Create(const tString & filename) = 0; //!< デコーダを作成する
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * デコーダファクトリーマネージャ
 */
class tWaveDecoderFactoryManager : public singleton_base<tWaveDecoderFactoryManager>
{
	typedef gc_map<tString, tWaveDecoderFactory *>  tMap; //!< 拡張子→ファクトリのマップの型のtypedef
	tMap Map; //!< 拡張子→ファクトリのマップ

public:
	/**
	 * コンストラクタ
	 */
	tWaveDecoderFactoryManager();

	/**
	 * デストラクタ
	 */
	~tWaveDecoderFactoryManager();

	/**
	 * ファクトリを登録する
	 * @param extension	拡張子 (小文字を使うこと;ドットも含む)
	 * @param factory	ファクトリ
	 */
	void Register(const tString & extension, tWaveDecoderFactory * factory);

	/**
	 * ファクトリの登録を解除する
	 * @param extension	拡張子 (小文字を使うこと;ドットも含む)
	 */
	void Unregister(const tString & extension);

	/**
	 * デコーダを作成する
	 * @param filename	ファイル名
	 * @return	作成されたデコーダ
	 */
	tWaveDecoder * Create(const tString & filename);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa



#endif

