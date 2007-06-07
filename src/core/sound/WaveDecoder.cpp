//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
tRisaWaveDecoderFactoryManager::tRisaWaveDecoderFactoryManager()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaWaveDecoderFactoryManager::~tRisaWaveDecoderFactoryManager()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaWaveDecoderFactoryManager::Register(const tRisseString & extension,
	boost::shared_ptr<tRisaWaveDecoderFactory> factory)
{
	Map.insert(
		std::pair<tRisseString, boost::shared_ptr<tRisaWaveDecoderFactory> >(
														extension, factory));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaWaveDecoderFactoryManager::Unregister(const tRisseString & extension)
{
	Map.erase(extension);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
boost::shared_ptr<tRisaWaveDecoder>
	tRisaWaveDecoderFactoryManager::Create(const tRisseString & filename)
{
	// 拡張子を取り出す
	tRisseString ext = tRisaFileSystemManager::ExtractExtension(filename);
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


