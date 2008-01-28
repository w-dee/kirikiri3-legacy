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
#include "prec.h"
#include "visual/rina/rinaUpdateRegion.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(59919,45169,54721,18134,39057,23492,3413,9444);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tUpdateRegion::tIterator::tIterator(const tUpdateRegion & ur) :
	UpdateRegion(ur), CurrentX(0), CurrentY(0)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tUpdateRegion::tIterator::GetNext(tRect & rect)
{
	// まず CurrentX および CurrentY を、矩形が存在する場所まで移動する
	// cx が ccx に達している場合はここで補正する
	risse_size cx = CurrentX;
	risse_size cy = CurrentY;
	risse_size ccy = UpdateRegion.CellCountY;
	risse_size ccx = UpdateRegion.CellCountX;
	const risse_uint16 * cells = UpdateRegion.Cells;
	const risse_uint16 * row;

	if(cx == ccx) cx = 0, cy++; // 補正
	while(cy < ccy)
	{
		row = cy * ccx + cells;
		for(/**/ ; cx < ccx; cx++)
			if(row[cx] != EmptyCellValue) goto exit_first_find_loop;

		cx = 0;
		cy ++;
	}

	return false; // もう見つからない

exit_first_find_loop:
	// そこのセルには何かがあることはわかった。
	// とりあえず横方向にスキャンし、連続していて、かつ
	// セルの中の矩形のTopとBottomが同じ範囲を探す
	risse_uint16 y0y1 = Y0Y1(row[cx]);
	risse_size cx_start = cx;
	for(/**/ ; cx < ccx; cx++)
	{
		if(X1(row[cx]) != 7)      { cx++;  break; } // 右方向に続いていない
		if(X0(row[cx]) != 0)      { break; } // 左方向から続いていない
		if(Y0Y1(row[cx]) != y0y1) { break; } // TopとBottomが違う
	}

	cx --;

	// この時点では、cx が ccx に達したか、あるいは連続してないか、あるいは
	//  topとbottomが違うセルが見つかったかのどれかである
	// rect のメンバを設定する
	rect.Left   = (cx_start << CellSizeShift) + X0(row[cx_start]);
	rect.Top    = (cy       << CellSizeShift) + Y0(row[cx_start]);
	rect.Right  = (cx       << CellSizeShift) + X1(row[cx      ]) + 1;
	rect.Bottom = (cy       << CellSizeShift) + Y1(row[cx      ]) + 1;

	// メンバに変数を書き戻す
	cx ++;
	CurrentX = cx;
	CurrentY = cy;

	return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tUpdateRegion::tUpdateRegion()
{
	Width = Height = 0;
	CellCountX = CellCountY = 0;
	Cells = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tUpdateRegion::SetSize(risse_size width, risse_size height)
{
	Width = width;
	Height = height;

	// 横方向のセル数と縦方向のセル数を求める
	// セルは32pixel x 32pixel で一つになる
	CellCountX = Width  == 0 ? 0 : (((Width -1)>>CellSizeShift)+1);
	CellCountY = Height == 0 ? 0 : (((Height-1)>>CellSizeShift)+1);

	if(!CellCountX || !CellCountY) CellCountX = CellCountY = 0;
	TotalCells = CellCountX * CellCountY;

	// メモリを確保する
	Cells = TotalCells ?
		static_cast<risse_uint16*>(MallocAtomicCollectee(sizeof(risse_uint16) * TotalCells)) :
		NULL;

	// 確保したメモリをすべてEmptyで埋める
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tUpdateRegion::Clear()
{
	risse_size total_cells = TotalCells;
	risse_uint16 * cells = Cells;
	for(risse_size i = 0; i < total_cells; i++) cells[i] = EmptyCellValue;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tUpdateRegion::Fill()
{
	risse_size total_cells = TotalCells;
	risse_uint16 * cells = Cells;
	for(risse_size i = 0; i < total_cells; i++) cells[i] = FullCellValue;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tUpdateRegion::Fill(const tRect & rect)
{
	// TODO: 高速化

	if(!rect.HasArea()) return; // 面積を持ってない

	// 開始セル位置と終了セル位置を計算
	risse_size start_x = rect.Left       >> CellSizeShift;
	risse_size end_x   = rect.Right      >> CellSizeShift;
	risse_size start_y = (rect.Top-1)    >> CellSizeShift;
	risse_size end_y   = (rect.Bottom-1) >> CellSizeShift;

	// ループ
	for(risse_size y = start_y; y <= end_y; y++)
	{
		risse_uint16 *row = Cells + y * CellCountX;
		for(risse_size x = start_x; x <= end_x; x++)
		{
			// 各セルとの交差を得る
			tRect cell(x<<CellSizeShift, y<<CellSizeShift,
					(x<<CellSizeShift)+CellSize, (y<<CellSizeShift)+CellSize);

			cell.Intersect(rect);
			cell.SetOffsets(0, 0);

			// 各セルの既存の更新矩形との Union を作成する
			row[x] = UniteCell(row[x], cell.Left, cell.Top, cell.Right-1, cell.Bottom-1);
		}
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
