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
//! @brief RINA ノード管理
//---------------------------------------------------------------------------
#include "visual/rina/rinaGDS.h"


namespace Rina {
//---------------------------------------------------------------------------

class tProcessNode;
class tPropertyNodeData;
class tPinNodeData;
//---------------------------------------------------------------------------
//! @brief		プロセスノードデータ
//---------------------------------------------------------------------------
class tProcessNodeData : public tGDSNodeData
{
	typedef tGDSNodeData inherited;

public:
	//! @brief		コンストラクタ
	//! @param		node		ノード
	tProcessNodeData(tGDSNodeBase * node);

	//! @brief		プロパティノードを得る
	tPropertyNodeData * GetPropertyNodeData() const
	{
		// プロパティノードは必ず 0 番のノードになる
		return reinterpret_cast<tPropertyNodeData*>(GetChildAt(0));
	}

	//! @brief		入力ピンの個数を得る
	//! @return		入力ピンの個数
	risse_size GetInputPinCount() const
	{
		// 0 番目の子はプロパティノードなのでそれを除いた数を返す
		return GetChildCount() - 1;
	}

protected: // サブクラスで実装するもの
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		プロセスノード
//---------------------------------------------------------------------------
class tProcessNode : public tGDSNode<tProcessNodeData>
{
	typedef tGDSNode<tProcessNodeData> inherited;
public:
	//! @brief		コンストラクタ
	//! @param		graph		GDS グラフインスタンス
	tProcessNode(tGDSGraph * graph);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
}

