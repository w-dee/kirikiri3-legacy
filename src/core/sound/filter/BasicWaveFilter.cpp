//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 基本的なWaveFilterの各機能の実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "BasicWaveFilter.h"
#include "RisaException.h"
#include "WaveFormatConverter.h"

RISSE_DEFINE_SOURCE_ID(35549,59301,21418,20212,56467,33012,49239,37291);

//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		desired_output_type    サブクラスが望む PCM 形式
//---------------------------------------------------------------------------
tRisaBasicWaveFilter::tRisaBasicWaveFilter(tRisaPCMTypes::tType desired_output_type)
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
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaBasicWaveFilter::~tRisaBasicWaveFilter()
{
	if(ConvertBuffer) free(ConvertBuffer);
	if(QueuedData) free(QueuedData);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		フィルタの入力を設定する
//! @param		input  入力となるフィルタ
//---------------------------------------------------------------------------
void tRisaBasicWaveFilter::SetInput(boost::shared_ptr<tRisaWaveFilter> input)
{
	Input = input;
	InputFormat = input->GetFormat();
	OutputFormat = InputFormat;
	OutputFormat.SetRisaPCMType(DesiredOutputType);
	InputChanged();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デコードを行う
//! @param		dest		デコード結果を格納するバッファ
//! @param		samples		デコードを行いたいサンプル数
//! @param		written		実際にデコード出来たサンプル数
//! @param		segments	再生セグメント情報を書き込む配列
//! @param		events		通過イベント情報(=ラベル情報)を書き込む配列
//! @return		まだデコードすべきデータが残っているかどうか
//---------------------------------------------------------------------------
bool tRisaBasicWaveFilter::Render(void *dest, risse_uint samples, risse_uint &written,
	std::vector<tRisaWaveSegment> &segments,
	std::vector<tRisaWaveEvent> &events)
{
	written = 0;
	risse_uint8 * dest_buf = reinterpret_cast<risse_uint8*>(dest);
	risse_uint sample_granule_bytes = OutputFormat.Channels * OutputFormat.BytesPerSample;

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

		// segments の該当部分をコピー
		risse_int64 ofs = 0;
		for(std::vector<tRisaWaveSegment>::iterator i = QueuedSegments.begin();
			i != QueuedSegments.end(); i++)
		{
			// [ofs, ofs+i->FilteredLength) と [copy_start, copy_start + one_unit) の交差を得る
			risse_int64 i1, i2;
			i1 = std::max(ofs, static_cast<risse_int64>(copy_start));
			i2 = std::min(ofs + i->FilteredLength, static_cast<risse_int64>(copy_start + one_unit));
			if(i2 > i1)
			{
				// length は線形補間を行う
				risse_int64 length =
					static_cast<risse_int64>(
						(double)i->Length / (double)i->FilteredLength * (i2-i1));
				if(length > 0)
					segments.push_back(tRisaWaveSegment(i1, length, i2-i1));
			}
			ofs += i->FilteredLength;
		}

		// events の該当部分をコピー
		for(std::vector<tRisaWaveEvent>::iterator i = QueuedEvents.begin();
			i != QueuedEvents.end(); i++)
		{
			if(static_cast<risse_uint>(i->Offset) >= copy_start &&
				static_cast<risse_uint>(i->Offset) < copy_start + one_unit)
			{
				// イベントのオフセットを修正して push
				tRisaWaveEvent ev(*i);
				ev.Offset = ev.Offset - copy_start + written;
				events.push_back(ev);
			}
		}

		// カウンタの調整
		QueuedSampleGranuleRemain -= one_unit;
		samples -= one_unit;
		written += one_unit;
	}

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		PCM形式を返す
//! @return		PCM形式への参照
//---------------------------------------------------------------------------
const tRisaWaveFormat & tRisaBasicWaveFilter::GetFormat()
{
	if(!Input)
		eRisaException::Throw(RISSE_WS_TR("The filter input is not yet connected"));
	return OutputFormat;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		出力キューを準備する
//! @param		numsamplegranules  準備したいサンプルグラニュール数
//! @return		出力バッファ
//---------------------------------------------------------------------------
void * tRisaBasicWaveFilter::PrepareQueue(risse_uint numsamplegranules)
{
	// キューを準備する
	risse_uint buffer_size_needed = numsamplegranules * OutputFormat.BytesPerSample * OutputFormat.Channels;
	if(QueuedDataAllocSize < buffer_size_needed)
	{
		void * newbuffer = realloc(QueuedData, buffer_size_needed);
		if(!newbuffer)
		{
			free(QueuedData), QueuedData = NULL;
			QueuedDataAllocSize = 0;
		}
		else
		{
			QueuedData = reinterpret_cast<risse_uint8 *>(newbuffer);
			QueuedDataAllocSize = buffer_size_needed;
		}
	}

	QueuedSegments.clear();
	QueuedEvents.clear();

	QueuedSampleGranuleCount = QueuedSampleGranuleRemain = 0;

	return QueuedData;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		出力キューにデータをおく
//! @param		numsamplegranules	サンプル数
//! @param		segments			セグメント配列
//! @param		events				イベント配列
//---------------------------------------------------------------------------
void tRisaBasicWaveFilter::Queue(risse_uint numsamplegranules,
		const std::vector<tRisaWaveSegment> &segments, const std::vector<tRisaWaveEvent> &events)
{
	// 出力キューにデータをおく
	QueuedSampleGranuleCount = numsamplegranules;
	QueuedSampleGranuleRemain = QueuedSampleGranuleCount;

	for(std::vector<tRisaWaveSegment>::const_iterator i = segments.begin();
		i != segments.end(); i++)
		QueuedSegments.push_back(*i);
	for(std::vector<tRisaWaveEvent>::const_iterator i = events.begin();
		i != events.end(); i++)
		QueuedEvents.push_back(*i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたバッファに入力フィルタから情報を読み出し、書き込む
//! @param		dest				書き込みたいバッファ
//! @param		numsamplegranules	欲しいサンプルグラニュール数
//! @param		desired_type		欲しいPCM形式
//! @param		segments	再生セグメント情報を書き込む配列
//! @param		events		通過イベント情報(=ラベル情報)を書き込む配列
//! @return		実際に書き込まれたサンプルグラニュール数
//---------------------------------------------------------------------------
risse_uint tRisaBasicWaveFilter::Fill(void * dest, risse_uint numsamplegranules,
	tRisaPCMTypes::tType desired_type, 
	std::vector<tRisaWaveSegment> &segments, std::vector<tRisaWaveEvent> &events)
{
	// バッファにデータをレンダリングする
	if(!Input) return 0; // Input が無い

	risse_uint desired_type_sample_bytes = tRisaPCMTypes::TypeToSampleBytes(desired_type);
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
		const tRisaWaveFormat & format = Input->GetFormat();

		if(InputFormat.Frequency != format.Frequency)
			break; // 周波数が変わった
		else if(InputFormat.Channels != format.Channels)
			break; // チャンネル数が変わった

		tRisaPCMTypes::tType filter_pcm_type = tRisaPCMTypes::tunknown;
		risse_uint filter_sample_granule_bytes = 0;

		if(cont)
		{
			filter_pcm_type = format.GetRisaPCMType();
			filter_sample_granule_bytes = format.BytesPerSample * format.Channels;
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
					void * newbuffer = realloc(ConvertBuffer, buffer_size_needed);
					if(!newbuffer)
					{
						free(ConvertBuffer), ConvertBuffer = NULL;
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
				// フィルタの出力タイプが 整数 16bit なので直接出力バッファに書き込む
				filter_destination = render_buffer +
					rendered * desired_type_sample_bytes;
			}
			else
			{
				// フィルタの出力タイプが 整数 16bit ではないのでいったん変換バッファに書き込む
				filter_destination = ConvertBuffer;
			}
		}

		// フィルタにデータを要求する
		size_t events_start = events.size();
		if(cont)
			cont = Input->Render(
				filter_destination,
				one_want, one_rendered, segments, events);

		// events の Offset の修正
		for(std::vector<tRisaWaveEvent>::iterator i = events.begin() + events_start;
			i != events.end(); i++)
		{
			i->Offset += rendered;
		}

		// ループの初回でデコードが終了したか？
		if(rendered == 0 && !cont)
		{
			// このループ初回かつ、デコード終了
			break;
		}

		// PCM 形式の変換
		if(need_convert)
		{
			tRisaWaveFormatConverter::Convert(
				desired_type,
				render_buffer + rendered * desired_type_sample_bytes,
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

	return rendered;
}
//---------------------------------------------------------------------------


