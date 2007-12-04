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
//! @brief テスト用のテキストミキサノード管理
//---------------------------------------------------------------------------
#ifndef RINATEXTMIXERNODE_H
#define RINATEXTMIXERNODE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaQueue.h"
#include "visual/rina/test/rinaTextProviderNode.h"


namespace Rina {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		テスト用のミキサノード用プロパティセット
//---------------------------------------------------------------------------
class tTextMixerPropertySet :
	public tPropertySet,
	public Risa::singleton_base<tTextPropertySet>,
	Risa::manual_start<tTextPropertySet>
{
public:
	//! @brief		コンストラクタ
	tTextMixerPropertySet();
};
//---------------------------------------------------------------------------


class tGraph;
class tInputPin;
class tOutputPin;
//---------------------------------------------------------------------------
//! @brief		テスト用のミキサノード
//---------------------------------------------------------------------------
class tTextMixerNode : public tProcessNode
{
	typedef tProcessNode inherited;

	gc_vector<tInputPin *> InputPins; //!< 入力ピンの配列

	tTextOutputPin * OutputPin; //!< 出力ピン

public:
	//! @brief		コンストラクタ
	tTextMixerNode();

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tTextMixerNode() {;}

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
//! @brief		テスト用のミキサコマンドキュー
//---------------------------------------------------------------------------
class tTextMixerQueueNode : public tTextProviderQueueNode
{
	typedef tTextProviderQueueNode inherited;

	static const risse_size CanvasSize = 30; //!< キャンバスのサイズ
	risse_char * Canvas; //!< 最終的に表示するテキストのバッファ

public:
	//! @brief		コンストラクタ
	//! @param		pos		位置
	tTextMixerQueueNode(risse_int32 pos);

	//! @brief		位置を得る
	//! @return		位置
	risse_int32 GetPosition() const { return Position; }

	//! @brief		テキストを得る
	//! @return		テキスト
	const tString & GetText() const { return Text; }

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess();

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess();
};
//---------------------------------------------------------------------------
}

#endif