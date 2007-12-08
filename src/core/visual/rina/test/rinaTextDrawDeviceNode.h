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
//! @brief テスト用のテキスト描画デバイスプロセスノード管理
//---------------------------------------------------------------------------
#ifndef RINATEXTDRAWDEVICENODE_H
#define RINATEXTDRAWDEVICENODE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaQueue.h"


namespace Rina {
//---------------------------------------------------------------------------


class tGraph;
class tInputPin;
class tOutputPin;
//---------------------------------------------------------------------------
//! @brief		テスト用のテキスト描画デバイスプロセスノード
//---------------------------------------------------------------------------
class tTextDrawDeviceNode : public tProcessNode
{
	typedef tProcessNode inherited;

	gc_vector<tInputPin *> InputPins; //!< 入力ピンの配列

public:
	//! @brief		コンストラクタ
	tTextDrawDeviceNode();

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tTextDrawDeviceNode() {;}

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



	//! @brief		コマンドキューを組み立てる
	//! @param		parent	親のコマンドキュー
	virtual void BuildQueue(tQueueNode * parent);

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		テスト用のテキスト描画デバイスコマンドキュー
//---------------------------------------------------------------------------
class tTextDrawDeviceQueueNode : public tQueueNode
{
	typedef tQueueNode inherited;

	static const risse_size CanvasSize = 75; //!< キャンバスのサイズ
	risse_char * Canvas; //!< 最終的に表示するテキストのバッファ

public:
	//! @brief		コンストラクタ
	//! @param		parent		親ノード
	tTextDrawDeviceQueueNode(tQueueNode * parent);

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess();

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess();
};
//---------------------------------------------------------------------------
}

#endif
