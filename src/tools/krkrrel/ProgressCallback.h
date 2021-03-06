//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 進捗コールバック用クラス
//---------------------------------------------------------------------------
#ifndef _PROGRESSCALLBACK_H_
#define _PROGRESSCALLBACK_H_

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * 進捗コールバック用クラス
 */
class iRisaProgressCallback
{
public:
	virtual void OnProgress(int percent) = 0; // パーセント単位での達成率
	virtual ~iRisaProgressCallback() {;}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * サブの処理のパーセント進捗をトータルのパーセント進捗として報告するクラス
 */
class tProgressCallbackAggregator : public iRisaProgressCallback
{
	iRisaProgressCallback * Destination;
	int PercentLow;
	int PercentHigh;

public:
	tProgressCallbackAggregator(
		iRisaProgressCallback *destination,
		int percentlow, int percenthigh
				) :
			Destination(destination),
			PercentLow(percentlow),
			PercentHigh(percenthigh)
			 {;}

	void OnProgress(int percent)
	{
		if(Destination)
		{
			int p = percent * (PercentHigh - PercentLow) / 100 + PercentLow;
			Destination->OnProgress(p);
		}
	}

	void SetRange(int percentlow, int percenthigh)
	{
		PercentLow = percentlow;
		PercentHigh = percenthigh;
	}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif

