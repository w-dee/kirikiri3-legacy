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
#include "prec.h"
#include "sound/WaveDecoder.h"
#include "base/fs/common/FSManager.h"
#include "base/exception/RisaException.h"


RISSE_DEFINE_SOURCE_ID(17086,16655,38940,19271,28579,15284,53763,63856);

//---------------------------------------------------------------------------
//! @brief	 	コンストラクタ
//---------------------------------------------------------------------------
tRisaWaveDecoderFactoryManager::tRisaWaveDecoderFactoryManager()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	 	デストラクタ
//---------------------------------------------------------------------------
tRisaWaveDecoderFactoryManager::~tRisaWaveDecoderFactoryManager()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	 	ファクトリを登録する
//! @param		extension	拡張子 (小文字を使うこと;ドットも含む)
//! @param		factory		ファクトリ
//---------------------------------------------------------------------------
void tRisaWaveDecoderFactoryManager::Register(const ttstr & extension,
	boost::shared_ptr<tRisaWaveDecoderFactory> factory)
{
	Map.insert(
		std::pair<ttstr, boost::shared_ptr<tRisaWaveDecoderFactory> >(
														extension, factory));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	 	ファクトリの登録を解除する
//! @param		extension	拡張子 (小文字を使うこと;ドットも含む)
//---------------------------------------------------------------------------
void tRisaWaveDecoderFactoryManager::Unregister(const ttstr & extension)
{
	Map.erase(extension);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	 	デコーダを作成する
//! @param		filename ファイル名
//! @return		作成されたデコーダの shared_ptr
//---------------------------------------------------------------------------
boost::shared_ptr<tRisaWaveDecoder>
	tRisaWaveDecoderFactoryManager::Create(const ttstr & filename)
{
	// 拡張子を取り出す
	ttstr ext = tRisaFileSystemManager::ExtractExtension(filename);
	ext.ToLowerCase();

	// ファクトリを探す
	tMap::iterator factory = Map.find(ext);
	if(factory != Map.end())
	{
		// ファクトリが見つかった
		boost::shared_ptr<tRisaWaveDecoder> decoder;
		decoder = factory->second->Create(filename);
		return decoder;
	}
	else
	{
		// ファクトリは見つからなかった
		eRisaException::Throw(
			RISSE_WS_TR("'%1' has non-supported file extension"), filename);
		return boost::shared_ptr<tRisaWaveDecoder>(); // これは実行されない
	}
}
//---------------------------------------------------------------------------


