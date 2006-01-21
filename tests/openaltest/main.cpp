// openal を使って音声をストリーミング再生するテスト

#include "prec.h"
#include "RisseEngine.h"
#include "RIFFWaveDecoder.h"
#include "VorbisDecoder.h"
#include "ALCommon.h"
#include "ALSource.h"
#include "ALBuffer.h"


RISSE_DEFINE_SOURCE_ID(1);













//---------------------------------------------------------------------------
//! @brief		アプリケーションクラス
//---------------------------------------------------------------------------
class Application : public wxAppConsole
{
public:
	virtual bool OnInit();
	virtual int OnRun();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// アプリケーションメインルーチン定義
//---------------------------------------------------------------------------
IMPLEMENT_APP_CONSOLE(Application)
wxLocale locale;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		アプリケーションが開始するとき
//! @return		成功すれば真
//---------------------------------------------------------------------------
bool Application::OnInit()
{
	locale.Init(wxLANGUAGE_DEFAULT);
	locale.AddCatalogLookupPathPrefix(wxT("locales")); 
	locale.AddCatalogLookupPathPrefix(wxT("../locales")); 
	locale.AddCatalog(wxT("openaltest"));
	return true;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		アプリケーションメインルーチン
//! @return		終了コード
//---------------------------------------------------------------------------
int Application::OnRun()
{
	try
	{
		tRisaSingletonManager::InitAll(); // 全てのシングルトンインスタンスを初期化

		tRisaRisseScriptEngine::instance()->GetEngineNoAddRef()->EvalExpression(
			RISSE_WS("FileSystem.mount('/', new FileSystem.OSFS('.'))"),
			NULL, NULL, NULL);

//		boost::shared_ptr<tRisaWaveDecoder> decoder(new tRisaRIFFWaveDecoder(RISSE_WS("test.wav")));
		boost::shared_ptr<tRisaWaveDecoder> decoder(new tRisaOggVorbisDecoder(RISSE_WS("test.ogg")));
		boost::shared_ptr<tRisaALBuffer> buffer(new tRisaALBuffer(decoder, true));
		tRisaALSource source(buffer);

		source.Play();

		while(true)
		{
			buffer->QueueStream(source.GetSource());
			Sleep(100);
			ALint pos;
			alGetSourcei( source.GetSource(), AL_SAMPLE_OFFSET, &pos);
			wxPrintf(wxT("position : %d\n"), pos);
		}
	}
	catch(const eRisse &e)
	{
		wxFprintf(stderr, wxT("error : %s\n"), e.GetMessage().AsWxString().c_str());
	}

	return 0;
}
//---------------------------------------------------------------------------

