//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief FFTを用いたピッチシフタ
//---------------------------------------------------------------------------
#include "prec.h"
#include "PitchShifter.h"

RISSE_DEFINE_SOURCE_ID(36731,9189,20036,16631,50318,29001,55122,51935);

//---------------------------------------------------------------------------
tRisaPitchShifter::tRisaPitchShifter(float pitch, risse_uint framesize) :
	tRisaBasicWaveFilter(tRisaPCMTypes::tf32)
{
	Buffer = NULL;
	FrameSize = framesize;
	Pitch = pitch;
}

tRisaPitchShifter::~tRisaPitchShifter()
{
	delete [] Buffer, Buffer = NULL;
}

void tRisaPitchShifter::InputChanged()
{
	Shifters.clear();
	for(risse_uint ch = 0; ch < InputFormat.Channels; ch++)
	{
		boost::shared_ptr<smbPitchShifter> shifter(new smbPitchShifter());
		Shifters.push_back(shifter);
	}
	delete [] Buffer, Buffer = NULL;
	Buffer = new float[InputFormat.Channels * FrameSize];
}

void tRisaPitchShifter::Filter()
{
	// Buffer にデータを読み込む
	std::vector<tRisaWaveSegment> segments;
	std::vector<tRisaWaveEvent> events;

	risse_uint filled = Fill(Buffer, FrameSize, tRisaPCMTypes::tf32, segments, events);
	const risse_uint channels = InputFormat.Channels;

	if(filled < FrameSize)
	{
		// データが足りない
		// 後半を 0 で埋める
		for(; filled < FrameSize; filled++)
		{
			for(risse_uint i = 0; i < channels; i++)
				Buffer[filled * channels + i] = 0;
		}
	}

	// Buffer から各シフタへデータを deinterleave する
	for(risse_uint i = 0; i < channels; i++)
	{
		float * dest = Shifters[i]->GetInputBuffer();
		for(risse_uint n = 0; n < FrameSize; n++)
			dest[n] = *(Buffer + i + n * channels);
	}

	// ピッチシフトを実行する
	for(risse_uint i = 0; i < channels; i++)
	{
		Shifters[i]->filter(Pitch, FrameSize, FrameSize, 32, InputFormat.Frequency);
	}

	// バッファを確保
	float * dest_buf = reinterpret_cast<float*>(PrepareQueue(FrameSize));
	if(!dest_buf) return;

	// 各シフタからdest_bufへデータをinterleaveする
	for(risse_uint i = 0; i < channels; i++)
	{
		float * src = Shifters[i]->GetOutputBuffer();
		for(risse_uint n = 0; n < FrameSize; n++)
			*(dest_buf + i + n * channels) = src[n];
	}

	// キューする
	Queue(FrameSize, segments, events);
}




