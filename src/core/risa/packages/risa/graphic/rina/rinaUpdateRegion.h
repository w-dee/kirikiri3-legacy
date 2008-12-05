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
//! @brief RINA 更新矩形領域管理の実装 
//---------------------------------------------------------------------------
#ifndef RINAUPDATEREGION_H
#define RINAUPDATEREGION_H

#include "risa/packages/risa/graphic/rina/rinaRectangle.h"

namespace Risa {
//---------------------------------------------------------------------------


/*!
	@note
	Libart で使用されている
	http://www.levien.com/libart/uta.html
	MicroTile Arrays (uta) を変形した物をここでは実装する。
	uta では32x32の領域を 32bit で表し、それを 8bit ずつにわけて
	それぞれが X0,Y0,X1,Y1 となり、その32x32中の任意の矩形を表している。
	対してここの実装では 16bitを 3bit 5bit 3bit 5bit にわけてそれぞれを
	X0,Y0,X1,Y1 とし、32x32中の任意の座標を表す。X に関しては 4 ピクセル数単
	位の値となる。最近のプロセッサのメモリアクセス方法を考えると、横方向の
	粒度は高くてもしょうがない。

	座標点は矩形に対してinclusive である。
	たとえば (X0,Y0,X1,Y1) = (0,0,7,31) は (0,0)-(32,32) の矩形(つまり32x32
	全域)を表す。(X0,Y0,X1,Y1) = (0,0,0,0) は (0,0)-(4,1) の矩形を表す。
	(X0,Y0,X1,Y1) = (7,31,0,0) つまり 0xff00 を空の状態として見なすことにする。

	32x32のマスをセルと呼ぶことにする。
*/

class tQueueNode;
//---------------------------------------------------------------------------
/**
 * 更新領域管理クラス
 */
class tUpdateRegion : public tCollectee
{
	friend class tIterator;

public:
	typedef tCollectee inherited;

private:
	risse_size		Width;		//!< 横サイズ (ピクセル単位)
	risse_size		Height;		//!< 縦サイズ (ピクセル単位)
	risse_size		CellCountX; //!< X方向のセルの数
	risse_size		CellCountY; //!< Y方向のセルの数
	risse_uint16	*Cells;		//!< セルの配列
	risse_size		TotalCells; //!< 全セル数

	static const risse_uint16	EmptyCellValue = 0xff00; //!< 空のセルの値
	static const risse_uint16	FullCellValue = 0x00ff; //!< 「フル」なセルの値
	static const int CellSizeShift = 5; //!< セルのサイズ log2
	static const int CellSize = 1<<CellSizeShift; //!< セルのサイズ
	static const risse_uint16 MaskX0 =  7 << (5+3+5); //!< セル値に対する x0 のマスク
	static const risse_uint16 MaskY0 = 31 << (  3+5); //!< セル値に対する y0 のマスク
	static const risse_uint16 MaskX1 =  7 << (    5); //!< セル値に対する x1 のマスク
	static const risse_uint16 MaskY1 = 31           ; //!< セル値に対する y1 のマスク

	/**
	 * 座標を16bit値にパックして返す
	 */
	static risse_uint16 Pack(
		unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1)
	{
		RISSE_ASSERT(x0 < 8 && y0 < 32 && x1 < 8 && y1 < 32);
		RISSE_ASSERT(x0 <= x1 && y0 <= y1 || x0 == 7 && y0 == 31 && x1 == 0 && y1 == 0);
		return
			(x0 << (5+3+5)) +
			(y0 << (  3+5)) +
			(x1 << (    5)) +
			(y1           );
	}

	static int X0(risse_uint16 v) { return (v & MaskX0) >> (5+3+5); } //!< 16bit値からX0を取り出して返す
	static int Y0(risse_uint16 v) { return (v & MaskY0) >> (  3+5); } //!< 16bit値からY0を取り出して返す
	static int X1(risse_uint16 v) { return (v & MaskX1) >> (    5); } //!< 16bit値からX1を取り出して返す
	static int Y1(risse_uint16 v) { return (v & MaskY1)           ; } //!< 16bit値からY1を取り出して返す

	/**
	 * 16bit値からY0 Y1だけをマスクした値を返す
	 */
	static risse_uint16 Y0Y1(risse_uint16 v) { return v & (MaskY0 + MaskY1); }

	/**
	 * セル内部の矩形をuniteする
	 */
	static risse_uint16 UniteCell(risse_uint16 v,
		unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1)
	{
		RISSE_ASSERT(x0 < 8 && y0 < 32 && x1 < 8 && y1 < 32);
		RISSE_ASSERT(x0 <= x1 && y0 <= y1);

		x0 <<= (5+3+5);
		y0 <<= (  3+5);
		x1 <<= (    5);

		return
			std::min(static_cast<risse_uint16>(v & MaskX0), static_cast<risse_uint16>(x0)) +
			std::min(static_cast<risse_uint16>(v & MaskY0), static_cast<risse_uint16>(y0)) +
			std::max(static_cast<risse_uint16>(v & MaskX1), static_cast<risse_uint16>(x1)) +
			std::max(static_cast<risse_uint16>(v & MaskY1), static_cast<risse_uint16>(y1)) ;
	}

public:
	/**
	 * イテレータ (STLのイテレータとは使い方がかなり違う)
	 */
	class tIterator : public tCollectee
	{
		const tUpdateRegion & UpdateRegion; //!< 更新領域管理クラスのインスタンス
		risse_size CurrentX; //!< 現在イテレーション中のセルのX座標
		risse_size CurrentY; //!< 現在イテレーション中のセルのY座標

	public:
		/**
		 * コンストラクタ
		 * @param ur	更新領域管理クラスのインスタンス
		 */
		tIterator(const tUpdateRegion & ur);

		/**
		 * 次の矩形を得る
		 * @param rect	矩形の格納先
		 * @return	次の矩形がない場合は偽が帰る。偽が帰った場合はrectには何も格納されない。
		 *			一度偽が帰った後はこのイテレータは無効になるのでアクセスしてはいけない。
		 */
		bool GetNext(tRect & rect);
	};

public:
	/**
	 * コンストラクタ
	 */
	tUpdateRegion();

	/**
	 * サイズを設定する
	 * @param width		横サイズ
	 * @param height	縦サイズ
	 */
	void SetSize(risse_size width, risse_size height);

	/**
	 * 横サイズを得る		@return		横サイズ
	 */
	risse_size GetWidth() const { return Width; }

	/**
	 * 縦サイズを得る		@return		縦サイズ
	 */
	risse_size GetHeight() const { return Height; }

	/**
	 * 領域をすべて空にする
	 */
	void Clear();

	/**
	 * 領域をすべてうめる
	 */
	void Fill();

	/**
	 * 矩形を埋める
	 * @param rect	矩形
	 */
	void Fill(const tRect & rect);
};
//---------------------------------------------------------------------------


}

#endif
