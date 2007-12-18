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
//! @brief テスト用のナロー文字列→ワイド文字列変換ノード
//---------------------------------------------------------------------------
#ifndef RINANARROWTEXTTOWIDETEXTCONVERTERNODE_H
#define RINANARROWTEXTTOWIDETEXTCONVERTERNODE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaQueue.h"
#include "rinaWideTextProviderNode.h"

namespace Rina {
//---------------------------------------------------------------------------



class tWideTextOutputPin;
class tNarrowTextInputPin;
//---------------------------------------------------------------------------
//! @brief		テスト用のテキストプロバイダノード
//---------------------------------------------------------------------------
class tNarrowTextToWideTextConverterNode : public tProcessNode
{
	typedef tProcessNode inherited;

	tWideTextOutputPin * OutputPin; //!< 出力ピン
	tNarrowTextInputPin * InputPin; //!< 入力ピン

public:
	//! @brief		コンストラクタ
	tNarrowTextToWideTextConverterNode();


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
	//! @param		state			レンダリングステート
	void BuildQueue(tRenderState * state);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		テスト用のテキストコマンドキュー
//---------------------------------------------------------------------------
class tNarrowTextToWideTextConverterQueueNode : public tWideTextProviderQueueNode
{
	typedef tWideTextProviderQueueNode inherited;

public:
	//! @brief		コンストラクタ
	tNarrowTextToWideTextConverterQueueNode();

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess();

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}

#endif