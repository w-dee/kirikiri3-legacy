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
#include "base/exception/Exception.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(17086,16655,38940,19271,28579,15284,53763,63856);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tWaveDecoderFactoryManager::tWaveDecoderFactoryManager()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tWaveDecoderFactoryManager::~tWaveDecoderFactoryManager()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveDecoderFactoryManager::Register(const tString & extension,
	boost::shared_ptr<tWaveDecoderFactory> factory)
{
	Map.insert(
		std::pair<tString, boost::shared_ptr<tWaveDecoderFactory> >(
														extension, factory));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveDecoderFactoryManager::Unregister(const tString & extension)
{
	Map.erase(extension);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
boost::shared_ptr<tWaveDecoder>
	tWaveDecoderFactoryManager::Create(const tString & filename)
{
	// 拡張子を取り出す
	tString ext = tFileSystemManager::ExtractExtension(filename);
	ext.ToLowerCase();

	// ファクトリを探す
	tMap::iterator factory = Map.find(ext);
	if(factory != Map.end())
	{
		// ファクトリが見つかった
		boost::shared_ptr<tWaveDecoder> decoder;
		decoder = factory->second->Create(filename);
		return decoder;
	}
	else
	{
		// ファクトリは見つからなかった
		eRisaException::Throw(
			RISSE_WS_TR("'%1' has non-supported file extension"), filename);
		return boost::shared_ptr<tWaveDecoder>(); // これは実行されない
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


