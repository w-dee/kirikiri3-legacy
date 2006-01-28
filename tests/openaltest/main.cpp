// openal を使って音声をストリーミング再生するテスト

#include "prec.h"
#include "RisseEngine.h"
#include "RIFFWaveDecoder.h"
#include "VorbisDecoder.h"
#include "WaveLoopManager.h"
#include "PhaseVocoder.h"
#include "Reverb.h"
#include "ALCommon.h"
#include "ALSource.h"
#include "ALBuffer.h"


RISSE_DEFINE_SOURCE_ID(58175,40687,29014,16466,35998,12636,24025,23840);













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

		boost::shared_ptr<tRisaWaveDecoder> decoder(new tRisaRIFFWaveDecoder(RISSE_WS("v02.wav")));
//		boost::shared_ptr<tRisaWaveDecoder> decoder(new tRisaOggVorbisDecoder(RISSE_WS("test3.ogg")));
		boost::shared_ptr<tRisaWaveLoopManager> loop_manager(new tRisaWaveLoopManager(decoder));
//		boost::shared_ptr<tRisaWaveFilter> filter(new tRisaPitchShifter(1.1, 4096));
		boost::shared_ptr<tRisaWaveFilter> filter(new tRisaPhaseVocoder());
//		boost::shared_ptr<tRisaWaveFilter> filter(new tRisaReverb());
		filter->SetInput(loop_manager);
		boost::shared_ptr<tRisaALBuffer> buffer(new tRisaALBuffer(filter, true));
		tRisaALSource source(buffer);

		source.Play();

		while(true)
		{
			buffer->QueueStream(source.GetSource());
			Sleep(1);
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

