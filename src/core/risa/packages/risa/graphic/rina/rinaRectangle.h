//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA 矩形クラス
//---------------------------------------------------------------------------
#ifndef RINARECTANGLE_H
#define RINARECTANGLE_H

#include "risa/packages/risa/graphic/rina/rinaTypes.h"

namespace Risa {
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * 矩形クラス
 */
template <typename VALUE_TYPE>
class tRectBase : public tCollectee
{
public:
	typedef VALUE_TYPE tValue; //!< 値の型の typedef

public:
	union
	{
		struct
		{
			tValue Left; //!< 左端
			tValue Top; //!< 上端
			tValue Right; //!< 右端
			tValue Bottom; //!< 下端
		};

		struct
		{
			t2DPointBase<tValue> UpperLeft; //!< 左上点
			t2DPointBase<tValue> BottomRight; //!< 右下点
		};
	};

public:

	/**
	 * コンストラクタ
	 */
	tRectBase(tValue l, tValue t, tValue r, tValue b)
		{ Left = l, Top = t, Right = r, Bottom =b; }

	tValue GetLeft() const { return Left; } //!< 左端を得る
	void SetLeft(tValue v) { Left = v; } //!< 左端を設定する
	tValue GetTop() const { return Top; } //!< 上端を得る
	void SetTop(tValue v) { Top = v; } //!< 上端を設定する
	tValue GetRight() const { return Right; } //!< 右端を得る
	void SetRight(tValue v) { Right = v; } //!< 右端を設定する
	tValue GetBottom() const { return Bottom; } //!< 下端を得る
	void SetBottom(tValue v) { Bottom = v; } //!< 下端を設定する

	tValue GetWidth()  const { return Right - Left; } //!< 横幅を得る
	tValue GetHeight() const { return Bottom - Top; } //!< 縦幅を得る

	void SetWidth(tValue w) { Right = Left + w; } //!< 横幅を設定する(Leftはそのままで、Rightが変更される)
	void SetHeight(tValue h) { Bottom = Top + h; } //!< 縦幅を設定する(Topはそのままで、Bottomが変更される)

	/**
	 * オフセットを加算する(矩形を指定しただけ移動する)
	 * @param x	横方向に加算したい値
	 * @param y	縦方向に加算したい値
	 */
	void AddOffsets(tValue x, tValue y)
	{
		Left += x; Right += x;
		Top += y; Bottom += y;
	}

	/**
	 * オフセットを設定する(矩形の大きさはそのままで、左上点だけを指定位置に変更する)
	 * @param x	左上点の新しいX座標値
	 * @param y	左上点の新しいY座標値
	 */
	void SetOffsets(tValue x, tValue y)
	{
		tValue w = GetWidth();
		tValue h = GetHeight();
		Left = x;
		Top = y;
		Right = x + w;
		Bottom = y + h;
	}

	/**
	 * サイズを設定する(矩形の左上点はそのまま)
	 * @param w	新しい横幅
	 * @param h	新しい縦幅
	 */
	void SetSize(tValue w, tValue h)
	{
		Right = Left + w;
		Bottom = Top + h;
	}

	/**
	 * 矩形をクリアする (頂点をすべて0にする)
	 */
	void Clear()
	{
		Left = Top = Right = Bottom = 0;
	}

	/**
	 * 矩形が有効か (0あるいは正のサイズを持っているか) を返す
	 */
	bool IsValid() const
	{
		return Left >= Right || Top >= Bottom;
	}

	/**
	 * 矩形がサイズを持ってるか (0以上のサイズを持っているか) を返す
	 */
	bool HasArea() const
	{
		return Right > Left && Bottom > Top;
	}

	/**
	 * 矩形を統合する(二つの矩形を囲む矩形にする)
	 * @param ref	統合する相手
	 * @note	この矩形およびrefの有効性はチェックしない
	 */
	void Unite(const tRectBase & ref)
	{
		Left   = std::min(Left,   ref.Left);
		Top    = std::min(Top,    ref.Top);
		Right  = std::max(Right,  ref.Right);
		Bottom = std::max(Bottom, ref.Bottom);
	}

	/**
	 * 矩形の交差を得る(二つの矩形に共通の矩形にする)
	 * @param ref	交差を得る相手
	 * @note	この矩形およびrefの有効性はチェックしない。
	 *			交差が得られなかった場合はこの矩形はサイズが0になるか無効になる(HasAreaでチェック可能)
	 */
	void Intersect(const tRectBase &ref)
	{
		Left   = std::max(Left,   ref.Left);
		Top    = std::max(Top,    ref.Top);
		Right  = std::min(Right,  ref.Right);
		Bottom = std::min(Bottom, ref.Bottom);
	}

};
//---------------------------------------------------------------------------
/**
 * 整数矩形クラスのtypedef
 */
typedef tRectBase<coord> tRect;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
