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
//! @brief テキストを反転するノード
//---------------------------------------------------------------------------
#ifndef RINAWIDETEXTREVERSENODE_H
#define RINAWIDETEXTREVERSENODE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaQueue.h"
#include "rinaWideTextProviderNode.h"
#include "rinaWideTextEdge.h"
#include "rina1DUpdateReceiver.h"

namespace Rina {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		テキストの反転を行うノード
//---------------------------------------------------------------------------
class tWideTextReverserNode : public t1DUpdateReceiver, public Risa::tSubmorph<tWideTextReverserNode>
{
public:
	typedef t1DUpdateReceiver inherited;
private:

	static tPinDescriptor InputPinDescriptor; //!< 入力ピンのデスクリプタ
	static tPinDescriptor OutputPinDescriptor; //!< 出力ピンのデスクリプタ
	tOnePins<tWideTextInputPin> InputPins; //!< 入力ピンの配列
	tOnePins<tWideTextOutputPin> OutputPins; //!< 出力ピンの配列

public:
	//! @brief		コンストラクタ
	//! @param		graph		グラフインスタンス
	tWideTextReverserNode(tGraph * graph);

public: // サブクラスで実装すべき物

	//! @brief		入力ピンの配列を得る
	//! @return		入力ピンの配列
	virtual tInputPins & GetInputPins() { return InputPins; }

	//! @brief		出力ピンの配列を得る
	//! @return		出力ピンの配列
	virtual tOutputPins & GetOutputPins() { return OutputPins; }

	//! @brief		コマンドキューの組み立てを行う
	//! @param		builder			キュービルダーオブジェクト
	void BuildQueue(tQueueBuilder & builder);

public:
	//! @brief		内容の更新があったことを伝える(子ノードから呼ばれる)
	//! @param		area		範囲
	virtual void NotifyUpdate(const t1DArea & area);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		テスト用のリバーサコマンドキュー
//---------------------------------------------------------------------------
class tWideTextReverserQueueNode : public tWideTextProviderQueueNode,
	public Risa::tSubmorph<tWideTextReverserQueueNode>
{
public:
	typedef tWideTextProviderQueueNode inherited;

public:
	//! @brief		コンストラクタ
	//! @param		request		レンダリング要求
	tWideTextReverserQueueNode(tWideTextRenderRequest * request);

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess();

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess();
};
//---------------------------------------------------------------------------
}

#endif
