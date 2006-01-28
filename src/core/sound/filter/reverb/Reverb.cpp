//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief リバーブフィルタの実装 (Freeverb)
//---------------------------------------------------------------------------
#include "prec.h"
#include "Reverb.h"

RISSE_DEFINE_SOURCE_ID(8600,37386,12503,16952,7601,59827,19639,49324);

//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaReverb::tRisaReverb() :
	tRisaBasicWaveFilter(tRisaPCMTypes::tf32)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaReverb::~tRisaReverb()
{
	delete [] Buffer;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		入力となるフィルタが変わったとき、あるいはリセットされるとき
//---------------------------------------------------------------------------
void tRisaReverb::InputChanged()
{
	Model.mute();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		フィルタ動作を行うとき
//---------------------------------------------------------------------------
void tRisaReverb::Filter()
{
	// Buffer にデータを読み込む
	std::vector<tRisaWaveSegment> segments;
	std::vector<tRisaWaveEvent> events;

	const risse_uint channels = InputFormat.Channels;
	if(channels != 2) return; // チャンネルは 2 (stereo)以外は今のところ対応していない

	// 入力バッファを確保
	if(!Buffer) Buffer = new float[NumBufferSampleGranules * InputFormat.Channels];

	// 出力バッファを確保
	float * dest_buf = reinterpret_cast<float*>(PrepareQueue(NumBufferSampleGranules));
	if(!dest_buf) return;

	// 入力からデータを読み取る
	risse_uint filled = Fill(Buffer, NumBufferSampleGranules, tRisaPCMTypes::tf32, true, segments, events);

	if(filled == 0) return;

	// リバーブエフェクトの実行
	Model.processreplace(Buffer, Buffer+1, dest_buf, dest_buf+1, NumBufferSampleGranules, 2);

	// キューする
	Queue(NumBufferSampleGranules, segments, events);
}
//---------------------------------------------------------------------------




