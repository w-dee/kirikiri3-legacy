//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief サウンドクラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "sound/Sound.h"
#include "sound/filter/BasicWaveFilter.h"
#include "risse/include/risseStaticStrings.h"
#include "risse/include/risseArrayClass.h"
#include "base/fs/common/FSManager.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(47626,3140,27936,19656,12175,17772,57131,58681);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSoundALSource::tSoundALSource(tSoundInstance * owner,
	tALBuffer * buffer, tWaveLoopManager * loopmanager) :
	tALSource(buffer, loopmanager), Owner(owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSoundALSource::tSoundALSource(tSoundInstance * owner, const tALSource * ref) :
	tALSource(ref), Owner(owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundALSource::OnStatusChanged(tStatus status)
{
	Owner->OnStatusChanged(status);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundALSource::OnStatusChangedAsync(tStatus status)
{
	Owner->OnStatusChangedAsync(status);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundALSource::OnLabel(const tString & name)
{
	Owner->OnLabel(name);
}
//---------------------------------------------------------------------------






/*
	メモ
	データの流れ

	tALSource <-- tALBuffer (<-- tWaveFilter)* <-- tWaveLoopManager <-- tWaveDecoder

	・複数の tALSource は 一つの tALBuffer を共有できる
	  (ただし、tALBuffer が非ストリーミングバッファの場合)
	・ストリーミング時にバッファを fill するのは tALSource の責任
	・tWaveFilter は あってもなくてもOK、複数個が直列する場合もある
	・tWaveLoopManager は一つ tWaveDecoder を入力にとる
*/


//---------------------------------------------------------------------------
tSoundInstance::tSoundInstance()
{
	Init();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Init()
{
	Status = ssUnload;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Clear()
{
	volatile tSynchronizer sync(this); // sync

	// 再生を停止
	if(Source) Source->Stop();

	// すべてのフィルタなどをリセット
	// TODO: 各インスタンスの正しいリセット
	Decoder = NULL; //.Dispose();
	LoopManager = NULL; //.Dispose();
//	Filters.clear();
	Buffer = NULL;
	Source = NULL; //reset();

	// ステータスを unload に
	OnStatusChanged(ssUnload);

	// PendingLabelQueue をクリア
	PendingLabelQueue.clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Open(const tString & filename)
{
	volatile tSynchronizer sync(this); // sync

	// メディアを開くのに先立って内部状態をクリア
	Clear();

	// 各コンポーネントを作成して接続する
	try
	{
		// デコーダを作成
		Decoder = tWaveDecoderFactoryManager::instance()->Create(filename);

		// LoopManager を作成
		LoopManager = new tWaveLoopManager(Decoder);

		// .sli ファイルがあるかどうか？
		tString sli_filename = filename + tSS<'.','s','l','i'>();
		if(tFileSystemManager::instance()->IsFile(sli_filename))
		{
			// LoopManager にそれを読ませる
			// いったん全体をファイルに読み込む
			tStreamAdapter sli_stream(
				tFileSystemManager::instance()->Open(sli_filename,
					tFileOpenModes::omRead));
			risse_size sli_stream_size = sli_stream.GetSize();
			char * sli_content = new (GC) char[sli_stream_size + 1];
			sli_stream.Read(sli_content, sli_stream_size);
			sli_content[sli_stream_size] = '\0';
			sli_stream.Dispose();
			// LoopManager にそれを読ませる
			if(!LoopManager->ReadInformation(sli_content))
				tSoundExceptionClass::Throw(RISSE_WS_TR("failed to read sound loop information: loop information syntax error"));
		}

		// フィルタ配列からフィルタチェーンを作成する
		tWaveFilter * last_filter = LoopManager;
		if(Filters.GetType() != tVariant::vtVoid)
		{
			tScriptEngine * engine = GetRTTI()->GetScriptEngine();

			// フィルタ配列の要素ごとに処理
			risse_size filter_count = (risse_int64)Filters.GetPropertyDirect(engine, ss_length);
			for(risse_size i = 0; i < filter_count; i++)
			{
				tVariant item = Filters.Invoke(engine, mnIGet, tVariant((risse_int64)i));
				if(item.GetType() == tVariant::vtVoid) continue;

				// フィルタ配列のインスタンスを得て、その SetInput メソッドを呼んで
				// 直前のフィルタや LoopManager と接続する
				RISA_PREPEND_EXCEPTION_MESSAGE_BEGIN()
				{
					tWaveFilterInstance * filter =
							item.ExpectAndGetObjectInterafce<tWaveFilterInstance>(
								tRisseClassRegisterer<tWaveFilterClass>::instance()->GetClassInstance());
						filter->SetInput(last_filter);
						last_filter = filter;
				}
				RISA_PREPEND_EXCEPTION_MESSAGE_END(engine, 
						tString(RISSE_WS_TR("Failed to connect filter index %1: "),
							tString::AsString((risse_int64)i)))
			}
		}

		// バッファを作成
		Buffer = new tALBuffer(last_filter, true);

		// ソースを作成
		Source = new tSoundALSource(this, Buffer, LoopManager);

		// ステータスを更新
		OnStatusChanged(ssStop);
	}
	catch(...)
	{
		// 内部状態をクリア
		Clear();
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Close()
{
	volatile tSynchronizer sync(this); // sync

	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Play()
{
	volatile tSynchronizer sync(this); // sync

	if(!Source) return; // ソースがないので再生を開始できない
	Source->Play(); // 再生を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Stop()
{
	volatile tSynchronizer sync(this); // sync

	if(!Source) return; // ソースがないので再生を停止できない
	Source->Stop(); // 再生を停止
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::Pause()
{
	volatile tSynchronizer sync(this); // sync

	if(!Source) return; // ソースがないので再生を一時停止できない
	Source->Pause(); // 再生を一時停止
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tSoundInstance::GetSamplePosition()
{
	if(!Source) return 0; // ソースがないので再生位置を取得できない
	return Source->GetPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
double tSoundInstance::GetTimePosition()
{
	volatile tSynchronizer sync(this); // sync

	if(!Source) return 0; // ソースがないので再生位置を取得できない
	return Source->GetPosition() * 1000 / LoopManager->GetFormat().Frequency;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::SetSamplePosition(risse_uint64 pos)
{
	volatile tSynchronizer sync(this); // sync

	if(!Source) return; // ソースがないので再生位置を変更できない

	GetDestEventQueueInstance()->CancelEvents(&LabelEventSource); // まだ配信されていないラベルイベントは削除する
	Source->SetPosition(pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::SetTimePosition(double pos)
{
	volatile tSynchronizer sync(this); // sync

	if(!Source) return; // ソースがないので再生位置を変更できない

	GetDestEventQueueInstance()->CancelEvents(&LabelEventSource); // まだ配信されていないラベルイベントは削除する
	Source->SetPosition(static_cast<risse_uint64>(pos *  LoopManager->GetFormat().Frequency / 1000));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant & tSoundInstance::GetFilters()
{
	volatile tSynchronizer sync(this); // sync

	if(Filters.GetType() == tVariant::vtVoid)
	{
		// まだフィルタ配列が出来ていないので作成する
		Filters = GetRTTI()->GetScriptEngine()->ArrayClass->Invoke(mnNew);
	}

	return Filters;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::OnStatusChanged(tStatus status)
{
	volatile tSynchronizer sync(this); // sync

	if(Status != status)
	{
		GetDestEventQueueInstance()->CancelEvents(&StatusEventSource);

		// tALSource はイベントを発生すべきタイミングでメソッドを
		// 呼び出しはするが、Risa のイベントシステムに関わる作業はしない。
		// ここではイベントシステムに関わる処理を行う。
		// tALSource の状態を常に tSoundInstance でも知っておかなければ
		// ならないと言うことでもあり、ミスをしやすいところと思われるので注意。
		bool post_pending_label_events = false;

		switch(status)
		{
		case tALSourceStatus::ssUnload:
		case tALSourceStatus::ssStop:
			// この場合はラベルイベントをすべて破棄する
			GetDestEventQueueInstance()->CancelEvents(&LabelEventSource);
			break;

		case tALSourceStatus::ssPlay:
			if(Status == tALSourceStatus::ssPause)
			{
				// 直前が Pause だったばあい、PendingLabelQueue をすべてキューに投げる
				post_pending_label_events = true;
			}
			else
			{
				// そうでない場合はラベルイベントをすべて破棄する
				GetDestEventQueueInstance()->CancelEvents(&LabelEventSource);
			}
			break;

		case tALSourceStatus::ssPause:
			if(Status == tALSourceStatus::ssPlay)
			{
				// 直前が Playだった場合、イベントキューにあるラベルイベントを
				// すべて PendingLabelQueue に待避する
				PendingLabelQueue.clear();
				GetDestEventQueueInstance()->CancelEvents(&LabelEventSource, PendingLabelQueue);
			}
			else
			{
				// そうでない場合はラベルイベントをすべて破棄する
				GetDestEventQueueInstance()->CancelEvents(&LabelEventSource);
			}
			break;
		}

		Status = status;

		// onStatusChanged を呼ぶ
		Operate(ocFuncCall, NULL, tSS<'o','n','S','t','a','t','u','s','C','h','a','n','g','e','d'>(),
				0, tMethodArgument::New((risse_int64)(int)status));


		if(post_pending_label_events)
		{
			// PendingLabelQueue をすべてキューに投げる
			for(tEventQueueInstance::tQueue::iterator i = PendingLabelQueue.begin();
				i != PendingLabelQueue.end(); i++)
				GetDestEventQueueInstance()->PostEvent(*i);
			PendingLabelQueue.clear();
		}

	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::OnStatusChangedAsync(tStatus status)
{
	volatile tSynchronizer sync(this); // sync

	if(Status != status)
	{
		Status = status;
		GetDestEventQueueInstance()->PostEvent(
			new tEventInfo(
				(int)status, // id
				&StatusEventSource, // source
				this // destination
				) );
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::OnLabel(const tString & name)
{
	volatile tSynchronizer sync(this); // sync

	GetDestEventQueueInstance()->PostEvent(
		new tLabelEventInfo(this, name));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::OnEvent(tEventInfo * info)
{
	volatile tSynchronizer sync(this); // sync

	if(info->GetSource() == &StatusEventSource)
	{
		// onStatusChanged イベント
		// onStatusChanged を呼ぶ
		Operate(ocFuncCall, NULL, tSS<'o','n','S','t','a','t','u','s','C','h','a','n','g','e','d'>(),
				0, tMethodArgument::New((risse_int64)info->GetId()));
	}
	else if(info->GetSource() == &LabelEventSource)
	{
		// ラベルイベント
		// onLabel を呼ぶ
		Operate(ocFuncCall, NULL, tSS<'o','n','L','a','b','e','l'>(),
				0, tMethodArgument::New(reinterpret_cast<tLabelEventInfo*>(info)->Name));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSoundInstance::tStatus tSoundInstance::GetStatus() const
{
	volatile tSynchronizer sync(this); // sync

	return Status;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::construct()
{
	// 特にやること無し
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		"Sound" クラス
//---------------------------------------------------------------------------
class tSoundClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tSoundClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tSoundClass::tSoundClass(tScriptEngine * engine) :
	tClassBase(tSS<'S','o','u','n','d'>(),
	tRisseClassRegisterer<tEventSourceClass>::instance()->GetClassInstance())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSoundClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tSoundClass::ovulate);
	BindFunction(this, ss_construct, &tSoundInstance::construct);
	BindFunction(this, ss_initialize, &tSoundInstance::initialize);
	BindFunction(this, tSS<'o','p','e','n'>(), &tSoundInstance::open);
	BindFunction(this, tSS<'p','l','a','y'>(), &tSoundInstance::play);
	BindFunction(this, tSS<'s','t','o','p'>(), &tSoundInstance::stop);
	BindFunction(this, tSS<'p','a','u','s','e'>(), &tSoundInstance::pause);
	BindProperty(this, tSS<'s','a','m','p','l','e','P','o','s','i','t','i','o','n'>(),
			&tSoundInstance::get_samplePosition, &tSoundInstance::set_samplePosition);
	BindProperty(this, tSS<'p','o','s','i','t','i','o','n'>(),
			&tSoundInstance::get_position, &tSoundInstance::set_position);
	BindProperty(this, tSS<'f','i','l','t','e','r','s'>(), &tSoundInstance::get_filters);
	BindProperty(this, tSS<'s','t','a','t','u','s'>(), &tSoundInstance::get_status);
	BindFunction(this, tSS<'o','n','S','t','a','t','u','s','C','h','a','n','g','e','d'>(),
			&tSoundInstance::onStatusChanged);
	BindFunction(this, tSS<'o','n','L','a','b','e','l'>(),
			&tSoundInstance::onLabel);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tSoundClass::ovulate()
{
	return tVariant(new tSoundInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Sound クラスレジストラ
template class tRisseClassRegisterer<tSoundClass>;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risa


