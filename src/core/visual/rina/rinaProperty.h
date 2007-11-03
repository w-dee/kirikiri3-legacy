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
//! @brief RINA プロパティ管理
//---------------------------------------------------------------------------
#include "visual/rina/rinaGDS.h"


namespace Rina {
//---------------------------------------------------------------------------


class tPropertyNode;
//---------------------------------------------------------------------------
//! @brief		プロパティノードデータ
//---------------------------------------------------------------------------
class tPropertyNodeData : public tGDSNodeData
{
	typedef tGDSNodeData inherited;

public:
	//! @brief		コンストラクタ
	//! @param		node		ノード
	tPropertyNodeData(tGDSNodeBase * node);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		プロパティノード
//! @param		プロパティノードは、GDS 上ではノードの子として表現される
//---------------------------------------------------------------------------
class tPropertyNode : public tGDSNode<tPropertyNodeData>
{
	typedef tGDSNode<tPropertyNodeData> inherited;

public:
	//! @brief		コンストラクタ
	//! @param		graph		GDS グラフインスタンス
	tPropertyNode(tGDSGraph * graph);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}

