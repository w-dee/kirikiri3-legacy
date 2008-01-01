//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 1次元のリージョン管理
//---------------------------------------------------------------------------
#ifndef RINA1DREGION_H
#define RINA1DREGION_H


namespace Rina {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一次元エリア
//---------------------------------------------------------------------------
class t1DArea : public tCollectee
{
	typedef tCollectee inherited;

	risse_offset Start; //!< 開始位置
	risse_offset End; //!< 終了位置 ( [start, end) )

public:
	//! @brief		コンストラクタ
	t1DArea();

	//! @brief		コンストラクタ
	//! @param		start		開始位置
	//! @param		end			終了位置 ( [start, end) )
	t1DArea(risse_int start, risse_int end) { RISSE_ASSERT(Start < End); Start = start, End = end; }

	//! @brief		範囲が重なっているかどうか
	//! @param		ref			重なっているかを調べたい相手
	//! @return		範囲が重なっていたかどうか(範囲が連続している場合は偽)
	bool Overlaps(const t1DArea & ref) const
	{
		if(End <= ref.Start) return false;
		if(Start >= ref.End) return false;
		return true;
	}

	//! @brief		範囲が重なっているか連続している場合は自分を相手に合わせて延長する
	//! @param		ref			重なっているかを調べたい相手
	//! @return		延長が行われた場合は真
	bool Extend(const t1DArea & ref)
	{
		if(End < ref.Start) return false;
		if(Start > ref.End) return false;
		if(Start > ref.Start) Start = ref.Start;
		if(End < ref.End) End = ref.End;
		return true;
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一次元リージョンクラス
//---------------------------------------------------------------------------
class t1DRegion : public tCollectee
{
	typedef tCollectee inherited;

public:
	typedef gc_vector<t1DArea> tAreas;
		//!< エリアの集合

private:
	tAreas Areas; // エリア

public:

	//! @brief		コンストラクタ
	t1DRegion();

	//! @param		エリア(個々の範囲)の配列を得る
	const tAreas & GetAreas() const { return Areas; }

	//! @brief		範囲を追加する
	//! @param		area		範囲
	void Add(t1DArea area);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
