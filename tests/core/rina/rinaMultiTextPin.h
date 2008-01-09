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
//! @brief テスト用の複数形式をサポートするテキストピン管理
//---------------------------------------------------------------------------
#ifndef RINAMULTITEXTPIN_H
#define RINAMULTITEXTPIN_H

#include "visual/rina/rinaPin.h"
#include "visual/rina/rinaIdRegistry.h"

namespace Rina {
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		出力ピン
//---------------------------------------------------------------------------
class tMultiTextOutputPin : public tOutputPin, public Risa::tSubmorph<tMultiTextOutputPin>
{
public:
	typedef tOutputPin inherited;
private:

public:
	//! @brief		コンストラクタ
	//! @param		node		ノード
	tMultiTextOutputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
