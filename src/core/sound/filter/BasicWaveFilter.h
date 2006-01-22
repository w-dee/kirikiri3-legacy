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

#ifndef BasicWaveFilterH
#define BasicWaveFilterH

#include "WaveFilter.h"


//---------------------------------------------------------------------------
//! @brief 基本的なWaveFilter
/*! @note
	
*/
//---------------------------------------------------------------------------
class tRisaBasicWaveFilter : public tRisaWaveFilter
{

protected:
	boost::shared_ptr<tRisaWaveFilter> Input;
	tRisaWaveFormat InputFormat;
	tRisaWaveFormat OutputFormat;
	risse_uint8 * ConvertBuffer;
	size_t ConvertBufferSize;
	tRisaPCMTypes::tType DesiredOutputType;

	risse_uint8 * QueuedData;
	risse_uint QueuedDataAllocSize;
	risse_uint QueuedSampleGranuleCount;
	risse_uint QueuedSampleGranuleRemain;
	std::vector<tRisaWaveSegment> QueuedSegments;
	std::vector<tRisaWaveEvent> QueuedEvents;

public:
	tRisaBasicWaveFilter(tRisaPCMTypes::tType desired_output_type);
	~tRisaBasicWaveFilter();

	void SetInput(boost::shared_ptr<tRisaWaveFilter> input);
	bool Render(void *dest, risse_uint samples, risse_uint &written,
		std::vector<tRisaWaveSegment> &segments,
		std::vector<tRisaWaveEvent> &events);
	const tRisaWaveFormat & GetFormat();

protected:

	void * PrepareQueue(risse_uint numsamplegranules);
		// キューを準備する
	void Queue(risse_uint numsamplegranules,
		std::vector<tRisaWaveSegment> &segments, std::vector<tRisaWaveEvent> &events);
		// 出力キューにデータをおく

	risse_uint Fill(void * dest, risse_uint numsamplegranules, tRisaPCMTypes::tType desired_type,
		std::vector<tRisaWaveSegment> &segments, std::vector<tRisaWaveEvent> &events);
		// dest に最低でも numsamplegranules のサンプルグラニュールを書き込む
		// 実際に書き込まれたサンプル数が返る




	virtual void InputChanged() = 0; // 入力が変更された
	virtual void Filter() = 0; // フィルタ動作を行う

};
//---------------------------------------------------------------------------

#endif

