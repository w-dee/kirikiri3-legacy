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
//! @brief テキストのプロパティ
//---------------------------------------------------------------------------
#ifndef RINATEXTPROPERTY_H
#define RINATEXTPROPERTY_H

#include "rina1DRegion.h"

namespace Rina {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		テキストの継承可能プロパティ
//---------------------------------------------------------------------------
class tTextInheritableProperties : public tCollectee
{
	typedef tCollectee inherited;

	risse_offset Position; //!< 位置

public:
	//! @brief		コンストラクタ
	tTextInheritableProperties() { Position = 0; }

	//! @brief		位置を設定する
	//! @param		pos		位置
	void SetPosition(risse_offset pos) { Position = pos; }

	//! @brief		位置を得る
	//! @return		位置
	risse_offset GetPosition() const { return Position; }

	//! @brief		プロパティの継承を行う
	//! @param		rhs		子
	//! @return		新しい情報
	tTextInheritableProperties operator + (const tTextInheritableProperties & rhs) const
	{
		tTextInheritableProperties props;
		props.Position = Position + rhs.Position;
		return props;
	}

	//! @brief		プロパティの継承を行う
	//! @param		rhs		子
	void operator += (const tTextInheritableProperties & rhs)
	{
		Position += rhs.Position;
	}

	//! @brief		親->子方向の変換を行う(t1DArea)
	//! @param		area		エリア
	//! @return		新しく変換されたエリア
	t1DArea ToChild(const t1DArea & area) const
	{
		t1DArea ret(area);
		ret.AddOffset(-Position);
		return ret;
	}

	//! @brief		子->親方向の変換を行う(t1DArea)
	//! @param		area		エリア
	//! @return		新しく変換されたエリア
	t1DArea ToParent(const t1DArea & area) const
	{
		t1DArea ret(area);
		ret.AddOffset(Position);
		return ret;
	}


};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
