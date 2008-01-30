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

	tWideTextOutputPin * OutputPin; //!< 出力ピン
	tWideTextInputPin * InputPin; //!< 入力ピン

public:
	//! @brief		コンストラクタ
	//! @param		graph		グラフインスタンス
	tWideTextReverserNode(tGraph * graph);

public: // サブクラスで実装すべき物

	//! @brief		出力ピンの個数を得る
	//! @return		出力ピンの個数
	virtual risse_size GetOutputPinCount();

	//! @brief		指定位置の出力ピンを得る
	//! @param		n		指定位置
	//! @return		指定位置の出力ピン
	virtual tOutputPin * GetOutputPinAt(risse_size n);

	//! @brief		指定位置に新規出力ピンを挿入する
	//! @param		n		指定位置
	virtual void InsertOutputPinAt(risse_size n);

	//! @brief		指定位置から出力ピンを削除する
	//! @param		n		指定位置
	virtual void DeleteOutputPinAt(risse_size n);



	//! @brief		入力ピンの個数を得る
	//! @return		入力ピンの個数
	virtual risse_size GetInputPinCount();

	//! @brief		指定位置の入力ピンを得る
	//! @param		n		指定位置
	//! @return		指定位置の入力ピン
	virtual tInputPin * GetInputPinAt(risse_size n);

	//! @brief		指定位置に入力ピンを挿入する
	//! @param		n		指定位置
	virtual void InsertInputPinAt(risse_size n);

	//! @brief		指定位置から入力ピンを削除する
	//! @param		n		指定位置
	virtual void DeleteInputPinAt(risse_size n);

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
