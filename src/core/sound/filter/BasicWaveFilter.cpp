//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 基本的なWaveFilterの各機能の実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "sound/filter/BasicWaveFilter.h"
#include "sound/WaveFormatConverter.h"
#include "sound/Sound.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(35549,59301,21418,20212,56467,33012,49239,37291);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tWaveFilterInstance::tWaveFilterInstance(tPCMTypes::tType desired_output_type)
{
	QueuedData = NULL;
	QueuedDataAllocSize = 0;
	QueuedSampleGranuleCount = 0;
	QueuedSampleGranuleRemain = 0;
	ConvertBuffer = NULL;
	ConvertBufferSize = 0;
	DesiredOutputType = desired_output_type;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tWaveFilterInstance::~tWaveFilterInstance()
{
	if(ConvertBuffer) FreeCollectee(ConvertBuffer);
	if(QueuedData) FreeCollectee(QueuedData);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveFilterInstance::Reset()
{
	if(ConvertBuffer) FreeCollectee(ConvertBuffer), ConvertBuffer = NULL;
	if(QueuedData) FreeCollectee(QueuedData), QueuedData = NULL;
	QueuedDataAllocSize = 0;
	QueuedSampleGranuleCount = 0;
	QueuedSampleGranuleRemain = 0;
	ConvertBufferSize = 0;

	SegmentQueue.Clear();
	InputChanged();
	Input->Reset(); // 入力フィルタの Reset メソッドも呼び出す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveFilterInstance::SetInput(tWaveFilter * input)
{
	Input = input;
	InputFormat = input->GetFormat();
	OutputFormat = InputFormat;
	OutputFormat.PCMType = DesiredOutputType;
	InputChanged();

	// もう一度入力フォーマットを得る
	// (サブクラスがInputChanged内でSuggestFormatをInputに対して行い
	//  その結果入力のPCM形式が変わっている可能性があるため)
	InputFormat = input->GetFormat();
	OutputFormat = InputFormat;
	OutputFormat.PCMType = DesiredOutputType;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tWaveFilterInstance::Render(void *dest, risse_uint samples, risse_uint &written,
	tWaveSegmentQueue & segmentqueue)
{
	written = 0;
	risse_uint8 * dest_buf = reinterpret_cast<risse_uint8*>(dest);
	risse_uint sample_granule_bytes = OutputFormat.GetSampleGranuleSize();

	tWaveSegmentQueue new_segmentqueue;

	while(samples > 0)
	{
		if(QueuedSampleGranuleRemain == 0)
		{
			// キューに残りがないので Filter を呼ぶ
			Filter();
			if(QueuedSampleGranuleRemain == 0)
				return false; // それでも残りがないので帰る
		}

		// 出力にデータをコピーする
		risse_uint one_unit =
			QueuedSampleGranuleRemain < samples ?
				QueuedSampleGranuleRemain : samples;

		risse_uint copy_start = QueuedSampleGranuleCount - QueuedSampleGranuleRemain;
		memcpy(dest_buf + written * sample_granule_bytes,
			QueuedData + copy_start * sample_granule_bytes,
			one_unit * sample_granule_bytes);

		// SegmentQueue の先頭から one_unit 分を segmentqueue にコピー
		SegmentQueue.Dequeue(new_segmentqueue, one_unit);
			// Dequeue は格納先キューをクリアするのでいったん別のキューにとる
		segmentqueue.Enqueue(new_segmentqueue);

		// カウンタの調整
		QueuedSampleGranuleRemain -= one_unit;
		samples -= one_unit;
		written += one_unit;
	}

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tWaveFormat & tWaveFilterInstance::GetFormat()
{
	if(!Input)
		tSoundExceptionClass::Throw(RISSE_WS_TR("The filter input is not yet connected"));
	return OutputFormat;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tWaveFilterInstance::PrepareQueue(risse_uint numsamplegranules)
{
	// キューを準備する
	risse_uint buffer_size_needed = numsamplegranules * OutputFormat.GetSampleGranuleSize();
	if(QueuedDataAllocSize < buffer_size_needed)
	{
		void * newbuffer;
		if(QueuedData == NULL)
			newbuffer = MallocAtomicCollectee(buffer_size_needed);
		else
			newbuffer = ReallocCollectee(QueuedData, buffer_size_needed);
		if(!newbuffer)
		{
			FreeCollectee(QueuedData), QueuedData = NULL;
			QueuedDataAllocSize = 0;
		}
		else
		{
			QueuedData = reinterpret_cast<risse_uint8 *>(newbuffer);
			QueuedDataAllocSize = buffer_size_needed;
		}
	}

	SegmentQueue.Clear();

	QueuedSampleGranuleCount = QueuedSampleGranuleRemain = 0;

	return QueuedData;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveFilterInstance::Queue(risse_uint numsamplegranules,
		const tWaveSegmentQueue & segmentqueue)
{
	// 出力キューにデータをおく
	QueuedSampleGranuleCount = numsamplegranules;
	QueuedSampleGranuleRemain = QueuedSampleGranuleCount;
	SegmentQueue = segmentqueue;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint tWaveFilterInstance::Fill(void * dest, risse_uint numsamplegranules,
	tPCMTypes::tType desired_type, 
	bool fill_silence,
	tWaveSegmentQueue & segmentqueue)
{
	// バッファにデータをレンダリングする
	if(!Input) return 0; // Input が無い

	risse_uint desired_type_samplegranule_bytes =
		tPCMTypes::TypeToSampleBytes(desired_type) * InputFormat.Channels;
	risse_uint8 * render_buffer = reinterpret_cast<risse_uint8*>(dest);
	risse_uint remain = numsamplegranules;
	if(remain == 0)
		remain = static_cast<risse_uint>(-1); // remain に整数の最大値を入れる

	risse_uint rendered = 0;
	while(remain > 0)
	{
		bool cont = true;
		risse_uint one_rendered = 0;
		risse_uint one_want = remain;

		// フィルタの現在のフォーマット形式をチェックする
		const tWaveFormat & format = Input->GetFormat();

		if(InputFormat.Frequency != format.Frequency)
			break; // 周波数が変わった
		else if(InputFormat.Channels != format.Channels)
			break; // チャンネル数が変わった

		tPCMTypes::tType filter_pcm_type = tPCMTypes::tunknown;
		risse_uint filter_sample_granule_bytes = 0;

		if(cont)
		{
			filter_pcm_type = format.PCMType;
			filter_sample_granule_bytes = format.GetSampleGranuleSize();
		}

		// 一回のこのループ単位で要求するサイズを決定
		if(cont)
		{
			risse_uint max_request_granules = 128*1024 / filter_sample_granule_bytes;
			if(one_want > max_request_granules) one_want = max_request_granules;
		}

		// 形式変換の必要は？
		bool need_convert = filter_pcm_type != desired_type;

		// ところで書き込み先バッファのサイズは十分？
		if(cont)
		{
			if(need_convert)
			{
				// ConvertBuffer のサイズをチェック
				risse_uint buffer_size_needed = one_want * filter_sample_granule_bytes;
				if(ConvertBufferSize < buffer_size_needed)
				{
					void * newbuffer;
					if(ConvertBuffer == NULL)
						newbuffer = MallocAtomicCollectee(buffer_size_needed);
					else
						newbuffer = ReallocCollectee(ConvertBuffer, buffer_size_needed);
					if(!newbuffer)
					{
						FreeCollectee(ConvertBuffer), ConvertBuffer = NULL;
						ConvertBufferSize = 0;
						cont = false;
					}
					else
					{
						ConvertBuffer = reinterpret_cast<risse_uint8 *>(newbuffer);
						ConvertBufferSize = buffer_size_needed;
					}
				}
			}
		}

		// フィルタのデータの書き込み先を決定
		risse_uint8 * filter_destination = NULL;
		if(cont)
		{
			if(!need_convert)
			{
				// サブクラスの望む形式が入力フィルタの出力形式と
				// 同じなので直接出力バッファに書き込む
				filter_destination = render_buffer +
					rendered * desired_type_samplegranule_bytes;
			}
			else
			{
				// サブクラスの望む形式が入力フィルタの出力形式と
				// 違うのでいったん変換バッファに書き込む
				filter_destination = ConvertBuffer;
			}
		}

		// フィルタにデータを要求する
		if(cont)
			cont = Input->Render(
				filter_destination,
				one_want, one_rendered, segmentqueue);

		// ループの初回でデコードが終了したか？
		if(rendered == 0 && !cont)
		{
			// このループ初回かつ、デコード終了
			break;
		}

		// PCM 形式の変換
		if(need_convert)
		{
			tWaveFormatConverter::Convert(
				desired_type,
				render_buffer + rendered * desired_type_samplegranule_bytes,
				filter_pcm_type,
				ConvertBuffer,
				InputFormat.Channels,
				one_rendered);
		}

		// カウンタを進める
		rendered += one_rendered;
		remain -= one_rendered;

		if(!cont)
		{
			// デコードの終了
			break;
		}
	}

	if(fill_silence && rendered < numsamplegranules)
	{
		// 残りを無音で埋める
		int sil_value = tPCMTypes::GetSilenceValueFromType(desired_type);
		memset(render_buffer + rendered * desired_type_samplegranule_bytes,
			sil_value,
			desired_type_samplegranule_bytes * (numsamplegranules - rendered));
	}

	return rendered;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveFilterInstance::construct()
{
	// デフォルトではなにもしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveFilterInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
tWaveFilterClass::tWaveFilterClass(tScriptEngine * engine) :
	tClassBase(tSS<'W','a','v','e','F','i','l','t','e','r'>(), engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveFilterClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tWaveFilterClass::ovulate);
	BindFunction(this, ss_construct, &tWaveFilterInstance::construct);
	BindFunction(this, ss_initialize, &tWaveFilterInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tWaveFilterClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		WaveFilter クラスレジストラ
template class tRisseClassRegisterer<tWaveFilterClass>;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

