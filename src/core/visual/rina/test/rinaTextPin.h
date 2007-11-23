//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief テスト用のテキストピン管理
//---------------------------------------------------------------------------
#ifndef RINATEXTPIN_H
#define RINATEXTPIN_H

#include "visual/rina/rinaPin.h"

namespace Rina {
//---------------------------------------------------------------------------


static const risse_uint32 TextEdgeType = 1; // テキストデータのエッジタイプ


//---------------------------------------------------------------------------
//! @brief		入力ピン
//---------------------------------------------------------------------------
class tTextInputPin : public tInputPin
{
	typedef tInputPin inherited;

public:

	//! @brief		コンストラクタ
	//! @param		node		ノード
	tTextInputPin();

	//! @brief		指定された形式が接続可能かどうかを判断する
	//! @param		type		接続形式
	//! @return		接続可能かどうか
	virtual bool CanConnect(risse_uint32 type);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		入力ピン
//---------------------------------------------------------------------------
class tTextOutputPin : public tOutputPin
{
	typedef tOutputPin inherited;

public:
	//! @brief		コンストラクタ
	//! @param		node		ノード
	tTextOutputPin();

	//! @brief		指定された形式が接続可能かどうかを判断する
	//! @param		type		接続形式
	//! @return		接続可能かどうか
	virtual bool CanConnect(risse_uint32 type);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
