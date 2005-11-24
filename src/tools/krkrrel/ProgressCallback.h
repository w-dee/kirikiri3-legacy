//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 進捗コールバック用クラス
//---------------------------------------------------------------------------
#ifndef _PROGRESSCALLBACK_H_
#define _PROGRESSCALLBACK_H_

//---------------------------------------------------------------------------
//! @brief		進捗コールバック用クラス
//---------------------------------------------------------------------------
class iTVPProgressCallback
{
public:
	virtual void OnProgress(int percent) = 0; // パーセント単位での達成率
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サブの処理のパーセント進捗をトータルのパーセント進捗として報告するクラス
//---------------------------------------------------------------------------
class tTVPProgressCallbackAggregator : public iTVPProgressCallback
{
	iTVPProgressCallback * Destination;
	int PercentLow;
	int PercentHigh;

public:
	tTVPProgressCallbackAggregator(
		iTVPProgressCallback *destination,
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
#endif

