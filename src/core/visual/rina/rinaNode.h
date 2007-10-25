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


namespace Rina {
//---------------------------------------------------------------------------

class tPropertyNode;
class tPinNode;
//---------------------------------------------------------------------------
class tNode : public tGDSNode
{
//	risse_size NumInputPins; //!< 入力ピンの現在の個数
//	risse_size NumOutputPins; //!< 出力ピンの現在の個数


public:
	//! @brief		コンストラクタ
	tNode();

	//! @brief		コピーコンストラクタ
	tNode(const tNode & rhs);

	//! @brief		プロパティノードを得る
	tPropertyNode * GetPropertyNode() const
	{
		// プロパティノードは必ず 0 番のノードになる
		return static_cast<tPropertyNode*>(GetChildAt(0));
	}

	//! @brief		入力ピンの個数を得る
	//! @return		入力ピンの個数
	risse_size GetInputPinCount() const
	{
		// 0 番目の子はプロパティノードなのでそれを除いた数を返す
		return GetChildCount() - 1;
	}

	//! @brief		出力ピンの個数を得る
	//! @return		出力ピンの個数
	risse_size GetOutputPinCount() const
	{
		return GetParentCount();
	}

protected: // サブクラスで実装するもの
	//! @brief			内容のコピーを行うべきタイミングで発生される
	//! @note			コピーされた内容
	virtual tGDSNode * Clone() const { return new tNode(*this); }

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}

