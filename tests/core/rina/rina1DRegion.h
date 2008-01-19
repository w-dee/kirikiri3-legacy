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
	t1DArea() {Start=0;End=0;}

	//! @brief		コンストラクタ
	//! @param		start		開始位置
	//! @param		end			終了位置 ( [start, end) )
	t1DArea(risse_offset start, risse_offset end) { Start = start, End = end; RISSE_ASSERT(Start <= End); }

	//! @brief		コンストラクタ(交差を得る)
	//! @param		a1		エリア1
	//! @param		a2		エリア2
	//! @return		交差がなかった場合は GetLength() が 0 より大きい値を返す
	t1DArea(const t1DArea & a1, const t1DArea & a2)
	{
		a1.Intersect(a2, *this);
	}

	//! @brief		開始位置を得る
	//! @return		開始位置
	risse_offset GetStart() const { return Start; }

	//! @brief		終了位置を得る
	//! @return		終了位置
	risse_offset GetEnd() const { return End; }

	//! @brief		エリアの長さを得る
	//! @return		長さ
	//! @note		エリアの長さが無効な場合は負の値が帰る
	risse_offset GetLength() const { return End - Start; }

	//! @brief		範囲が重なっているかどうか
	//! @param		ref			重なっているかを調べたい相手
	//! @return		範囲が重なっていたかどうか(範囲が連続している場合は偽)
	bool Overlaps(const t1DArea & ref) const
	{
		if(End <= ref.Start) return false;
		if(Start >= ref.End) return false;
		return true;
	}

	//! @brief		重なっている範囲を得る
	//! @param		ref			重なっているかを調べたい相手
	//! @param		intersec	重なっている範囲を格納する先
	//! @return		範囲が重なっていれば真、そうでなければ偽
	bool Intersect(const t1DArea & ref, t1DArea & intersec) const
	{
		intersec.Start = std::max(Start, ref.Start);
		intersec.End = std::min(End, ref.End);
		return intersec.Start < intersec.End;
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

	//! @brief		二つの範囲を統合する
	//! @param		ref			統合する相手
	void Unite(const t1DArea & ref)
	{
		risse_offset start = std::min(Start, ref.Start);
		risse_offset end   = std::min(End  , ref.End  );
		Start = start;
		End = end;
	}

	//! @brief		オフセットを加算する
	//! @param		offset		オフセット
	void AddOffset(risse_offset offset) { Start += offset, End += offset; }
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

	//! @brief		内容をクリアする
	void Clear() { Areas.clear(); }

	//! @brief		内容が空かどうかを得る
	//! @return		内容が空かどうか
	bool IsEmpty() const { return Areas.size() == 0; }

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
