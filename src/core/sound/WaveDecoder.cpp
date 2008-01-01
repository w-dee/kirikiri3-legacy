//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveデコーダインターフェースの定義
//---------------------------------------------------------------------------
#include "prec.h"
#include "sound/WaveDecoder.h"
#include "base/fs/common/FSManager.h"
#include "sound/Sound.h"


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
	tWaveDecoderFactory * factory)
{
	Map.insert(
		std::pair<tString, tWaveDecoderFactory* >(
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
tWaveDecoder * tWaveDecoderFactoryManager::Create(const tString & filename)
{
	// 拡張子を取り出す
	tString ext = tFileSystemManager::ExtractExtension(filename);
	ext.ToLowerCaseNC();

	// ファクトリを探す
	tMap::iterator factory = Map.find(ext);
	if(factory != Map.end())
	{
		// ファクトリが見つかった
		tWaveDecoder * decoder;
		decoder = factory->second->Create(filename);
		return decoder;
	}
	else
	{
		// ファクトリは見つからなかった
		tSoundExceptionClass::Throw(tString(
			RISSE_WS_TR("'%1' has non-supported file extension"), filename));
		return NULL; // これは実行されない
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


