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
//! @brief テスト用のナローテキストピン管理
//---------------------------------------------------------------------------
#ifndef RINANARROWTEXTPIN_H
#define RINANARROWTEXTPIN_H

#include "visual/rina/rinaPin.h"
#include "visual/rina/rinaIdRegistry.h"

namespace Rina {
//---------------------------------------------------------------------------


static const risse_uint32 NarrowTextEdgeType = tFourCharId<'t','x','t','n'>::value;
	//!< テキストデータのエッジタイプ

//---------------------------------------------------------------------------
//! @brief		テキストピンのエッジタイプを登録するためのシングルトン
//---------------------------------------------------------------------------
class tNarrowTextEdgeTypeRegisterer : public Risa::singleton_base<tNarrowTextEdgeTypeRegisterer>
{
public:
	//! @brief		コンストラクタ
	tNarrowTextEdgeTypeRegisterer();
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		入力ピン
//---------------------------------------------------------------------------
class tNarrowTextInputPin : public tInputPin
{
	typedef tInputPin inherited;

	risse_int32		Position; //!< 位置

public:

	//! @brief		コンストラクタ
	tNarrowTextInputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();

	//! @brief		位置を設定する
	//! @param		pos		位置
	void SetPosition(int pos) { Position = pos; }

	//! @brief		位置を取得する
	//! @return		位置
	risse_int32 GetPosition() { return Position; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		出力ピン
//---------------------------------------------------------------------------
class tNarrowTextOutputPin : public tOutputPin
{
	typedef tOutputPin inherited;

public:
	//! @brief		コンストラクタ
	tNarrowTextOutputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
