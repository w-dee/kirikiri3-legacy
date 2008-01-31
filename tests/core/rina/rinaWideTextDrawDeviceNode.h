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
//! @brief テスト用のテキスト描画デバイスプロセスノード管理
//---------------------------------------------------------------------------
#ifndef RINAWIDETEXTDRAWDEVICENODE_H
#define RINAWIDETEXTDRAWDEVICENODE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaQueue.h"
#include "rinaWideTextEdge.h"
#include "rina1DUpdateReceiver.h"

namespace Rina {
//---------------------------------------------------------------------------


class tGraph;
class tInputPin;
class tOutputPin;
//---------------------------------------------------------------------------
//! @brief		テスト用のテキスト描画デバイスプロセスノード
//---------------------------------------------------------------------------
class tWideTextDrawDeviceNode : public t1DUpdateReceiver, public Risa::tSubmorph<tWideTextDrawDeviceNode>
{
public:
	typedef t1DUpdateReceiver inherited;

private:
	gc_vector<tInputPin *> InputPins; //!< 入力ピンの配列
	t1DRegion DirtyRegion; //!< ダーティーなリージョン

public:
	//! @brief		コンストラクタ
	//! @param		graph		グラフインスタンス
	tWideTextDrawDeviceNode(tGraph * graph);

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tWideTextDrawDeviceNode() {;}

public: // サブクラスで実装すべき物

	//! @brief		出力ピンの個数を得る
	//! @return		出力ピンの個数
	//! @note		ピンの操作を行う前にグラフをロックすること
	virtual risse_size GetOutputPinCount();

	//! @brief		指定位置の出力ピンを得る
	//! @param		n		指定位置
	//! @return		指定位置の出力ピン
	//! @note		ピンの操作を行う前にグラフをロックすること
	virtual tOutputPin * GetOutputPinAt(risse_size n);

	//! @brief		指定位置に新規出力ピンを挿入する
	//! @param		n		指定位置
	//! @note		ピンの操作を行う前にグラフをロックすること
	virtual void InsertOutputPinAt(risse_size n);

	//! @brief		指定位置から出力ピンを削除する
	//! @param		n		指定位置
	//! @note		ピンの操作を行う前にグラフをロックすること
	virtual void DeleteOutputPinAt(risse_size n);



	//! @brief		入力ピンの個数を得る
	//! @return		入力ピンの個数
	//! @note		ピンの操作を行う前にグラフをロックすること
	virtual risse_size GetInputPinCount();

	//! @brief		指定位置の入力ピンを得る
	//! @param		n		指定位置
	//! @return		指定位置の入力ピン
	//! @note		ピンの操作を行う前にグラフをロックすること
	virtual tInputPin * GetInputPinAt(risse_size n);

	//! @brief		指定位置に入力ピンを挿入する
	//! @param		n		指定位置
	//! @note		ピンの操作を行う前にグラフをロックすること
	virtual void InsertInputPinAt(risse_size n);

	//! @brief		指定位置から入力ピンを削除する
	//! @param		n		指定位置
	//! @note		ピンの操作を行う前にグラフをロックすること
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
//! @brief		テスト用のテキスト描画デバイスコマンドキュー
//---------------------------------------------------------------------------
class tWideTextDrawDeviceQueueNode : public tQueueNode, public Risa::tSubmorph<tWideTextDrawDeviceQueueNode>
{
public:
	typedef tQueueNode inherited;

public:
	static const risse_size CanvasSize = 78; //!< キャンバスのサイズ

private:
	risse_char * Canvas; //!< 最終的に表示するテキストのバッファ

public:
	//! @brief		コンストラクタ
	//! @param		request		レンダリング要求
	tWideTextDrawDeviceQueueNode(tWideTextRenderRequest * request);

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess();

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess();
};
//---------------------------------------------------------------------------
}

#endif
