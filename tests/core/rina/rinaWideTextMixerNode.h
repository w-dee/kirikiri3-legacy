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
//! @brief テスト用のテキストミキサノード管理
//---------------------------------------------------------------------------
#ifndef RINAWIDETEXTMIXERNODE_H
#define RINAWIDETEXTMIXERNODE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaQueue.h"
#include "rinaWideTextProviderNode.h"
#include "rina1DUpdateReceiver.h"


namespace Rina {
//---------------------------------------------------------------------------


class tGraph;
class tInputPin;
class tOutputPin;
//---------------------------------------------------------------------------
//! @brief		テスト用のミキサノード
//---------------------------------------------------------------------------
class tWideTextMixerNode : public t1DUpdateReceiver, public Risa::tSubmorph<tWideTextMixerNode>
{
public:
	typedef t1DUpdateReceiver inherited;
private:
	static tPinDescriptor InputPinDescriptor; //!< 入力ピンのデスクリプタ
	static tPinDescriptor OutputPinDescriptor; //!< 出力ピンのデスクリプタ
	tArrayPins<tWideTextMixerInputPin> InputPins; //!< 入力ピンの配列
	tOnePins<tWideTextOutputPin> OutputPins; //!< 出力ピンの配列

public:
	//! @brief		コンストラクタ
	//! @param		graph		グラフインスタンス
	tWideTextMixerNode(tGraph * graph);

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tWideTextMixerNode() {;}


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
//! @brief		テスト用のミキサコマンドキュー
//---------------------------------------------------------------------------
class tWideTextMixerQueueNode : public tWideTextProviderQueueNode,
	public Risa::tSubmorph<tWideTextMixerQueueNode>
{
public:
	typedef tWideTextProviderQueueNode inherited;

public:
	static const risse_size CanvasSize = 11; //!< キャンバスのサイズ

private:
	risse_char * Canvas; //!< 最終的に表示するテキストのバッファ

public:
	//! @brief		コンストラクタ
	//! @param		request		レンダリング要求
	tWideTextMixerQueueNode(tWideTextRenderRequest * request);

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess();

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess();
};
//---------------------------------------------------------------------------
}

#endif
