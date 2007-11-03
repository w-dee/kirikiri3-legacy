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
//! @brief RINA ピン管理
//---------------------------------------------------------------------------
#include "visual/rina/rinaGDS.h"

namespace Rina {
//---------------------------------------------------------------------------

class tPinNode;
class tPropertyNodeData;
//---------------------------------------------------------------------------
//! @brief		ピンノードデータ
//---------------------------------------------------------------------------
class tPinNodeData : public tGDSNodeData
{
	typedef tGDSNodeData inherited;

public:
	//! @brief		コンストラクタ
	//! @param		node		ノード
	tPinNodeData(tGDSNodeBase * node);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ピンノード
//! @param		ピンノードは、GDS 上ではノードの子として表現される
//---------------------------------------------------------------------------
class tPinNode : public tGDSNode<tPinNodeData>
{
	typedef tGDSNode<tPinNodeData> inherited;

public:
	//! @brief		コンストラクタ
	//! @param		graph		GDS グラフインスタンス
	tPinNode(tGDSGraph * graph);
};
//---------------------------------------------------------------------------






class tOutputPinNodeData;
//---------------------------------------------------------------------------
//! @brief		入力ピンノードデータ
//---------------------------------------------------------------------------
class tInputPinNodeData : public tPinNodeData
{
	/*
		入力ピンの場合、
		子ノードは二つ
			・プロパティノード
			・出力ピンノード
		親ノードは一つ
			・プロセスノード
	*/

	typedef tPinNodeData inherited;

public:
	//! @brief		コンストラクタ
	//! @param		node		ノード
	tInputPinNodeData(tGDSNodeBase * node);


	//! @brief		プロパティノードを得る
	tPropertyNodeData * GetPropertyNodeData() const
	{
		// プロパティノードは必ず 0 番のノードになる
		return reinterpret_cast<tPropertyNodeData*>(GetChildAt(0));
	}

	//! @brief		出力ピンノードを得る
	tOutputPinNodeData * GetOutputPinNodeData() const
	{
		// 出力ピンノードは必ず 1 番のノードになる
		return reinterpret_cast<tOutputPinNodeData*>(GetChildAt(1));
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		入力ピンノード
//---------------------------------------------------------------------------
class tInputPinNode : public tPinNode
{
	typedef tPinNode inherited;

public:
	//! @brief		コンストラクタ
	//! @param		graph		GDS グラフインスタンス
	tInputPinNode(tGDSGraph * graph);
};
//---------------------------------------------------------------------------







class tProcessNodeData;
//---------------------------------------------------------------------------
//! @brief		出力ピンノードデータ
//---------------------------------------------------------------------------
class tOutputPinNodeData : public tPinNodeData
{
	/*
		出力ピンの場合、
		子ノードは一つ
			・プロパティノード
			・プロセスノード
		親ノードは二つ
			・入力ピンノード
	*/

	typedef tPinNodeData inherited;

public:
	//! @brief		コンストラクタ
	//! @param		node		ノード
	tOutputPinNodeData(tGDSNodeBase * node);

	//! @brief		プロパティノードを得る
	tPropertyNodeData * GetPropertyNodeData() const
	{
		// プロパティノードは必ず 0 番のノードになる
		return reinterpret_cast<tPropertyNodeData*>(GetChildAt(0));
	}

	//! @brief		プロセスノードを得る
	tProcessNodeData * GetProcessNodeData() const
	{
		// プロセスノードは必ず 1 番のノードになる
		return reinterpret_cast<tProcessNodeData*>(GetChildAt(1));
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		出力ピンノード
//---------------------------------------------------------------------------
class tOutputPinNode : public tPinNode
{
	typedef tPinNode inherited;

public:
	//! @brief		コンストラクタ
	//! @param		graph		GDS グラフインスタンス
	tOutputPinNode(tGDSGraph * graph);
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
}

