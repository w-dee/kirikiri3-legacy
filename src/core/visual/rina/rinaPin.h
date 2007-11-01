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


namespace Rina {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ピンノードデータ
//---------------------------------------------------------------------------
class tPinNodeData : public tGDSNodeData
{
	typedef tGDSNodeData inherited;

public:
	//! @brief		コンストラクタ
	tPinNodeData();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ピンノード
//! @param		ピンノードは、GDS 上ではノードの子として表現される
//---------------------------------------------------------------------------
class tPinNode : public tGDSNode<tPinNode>
{
	typedef tGDSNode<tNode> inherited;

public:
	//! @brief		コンストラクタ
	//! @param		graph		GDS グラフインスタンス
	tPinNode(tGDSGraph * graph);
};
//---------------------------------------------------------------------------







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

	typedef tGDSNodeData inherited;

public:
	//! @brief		コンストラクタ
	tInputPinNodeData();


	//! @brief		プロパティノードを得る
	tPropertyNode * GetPropertyNode() const
	{
		// プロパティノードは必ず 0 番のノードになる
		return reinterpret_cast<tPropertyNode*>(GetChildAt(0));
	}

	//! @brief		出力ピンノードを得る
	tPropertyNode * GetOutputPinNode() const
	{
		// 出力ピンノードは必ず 1 番のノードになる
		return reinterpret_cast<tPropertyNode*>(GetChildAt(1));
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

	typedef tGDSNodeData inherited;

public:
	//! @brief		コンストラクタ
	tOutputPinNodeData();

	//! @brief		プロパティノードを得る
	tPropertyNode * GetPropertyNode() const
	{
		// プロパティノードは必ず 0 番のノードになる
		return reinterpret_cast<tPropertyNode*>(GetChildAt(0));
	}

	//! @brief		プロセスノードを得る
	tPropertyNode * GetProcessNode() const
	{
		// プロセスノードは必ず 1 番のノードになる
		return reinterpret_cast<tPropertyNode*>(GetChildAt(1));
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

