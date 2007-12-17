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
//! @brief RINA コマンドキュー管理
//---------------------------------------------------------------------------
#ifndef RINAQUEUE_H
#define RINAQUEUE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaIdRegistry.h"

namespace Rina {
//---------------------------------------------------------------------------


/*!
	@note
	コマンドキューとは言っても線形キューではなくて、依存関係を表せるように
	DAGによる依存グラフを作成した後、それぞれの BeginProcess と EndProcess を
	実行単位として 順々に tCommandQueue 内のキューに push していき、
	キューからその実行単位を取り出しながら実行を行っていく。
	DAGの各ノードは、依存しているノードが実行される前に実行したい内容である
	BeginProcess と、依存しているノードが実行された後に実行したい内容である
	EndProcess をもち、大まかには ルートの BeginProcess から末端のBeginProcessへ、
	末端のEndProcessからルートのEndProcessへの順に実行が行われる。
*/

class tCommandQueue;
//---------------------------------------------------------------------------
//! @brief		コマンドキューノード
//---------------------------------------------------------------------------
class tQueueNode : public tCollectee
{
	typedef tCollectee inherited;

protected:
	typedef gc_vector<tQueueNode*> tNodes; //!< 子ノードの配列のtypedef
	tNodes Children; //!< 子ノード
	tNodes Parents; //!< 親ノード

	risse_size WaitingChildren; //!< 子ノードを待っているカウント
	risse_size WaitingParents; //!< 親ノードを待っているカウント TODO: より効率的な実装

public:
	//! @brief		コンストラクタ
	//! @param		parent		親ノード
	tQueueNode(tQueueNode * parent);

	//! @brief		デストラクタ (おそらく呼ばれない)
	virtual ~tQueueNode() {;}

	//! @brief		ノードの処理を行う
	//! @param		queue		コマンドキューインスタンス
	//! @param		is_begin	BeginProcess を対象とするか(真) EndProcess を対象とするか(偽)
	void Process(tCommandQueue * queue, bool is_begin);

protected:
	//! @brief		ノードの子を追加する
	//! @param		child		子
	void AddChild(tQueueNode * child);

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess() = 0;

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess() = 0;

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		ルートコマンドキューノード
//---------------------------------------------------------------------------
class tRootQueueNode : public tQueueNode
{
	typedef tQueueNode inherited;

public:
	//! @brief		コンストラクタ
	tRootQueueNode() : inherited(NULL) {;}

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess();

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess();

};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		コマンドキュー
//---------------------------------------------------------------------------
class tCommandQueue : public tCollectee
{
	typedef tCollectee inherited;

	//! @brief		アイテム構造体
	struct tItem
	{
		tQueueNode * Node; //!< ノード
		bool IsBegin; //!< BeginProcess を対象とするか(真) EndProcess を対象とするか(偽)
	};

	typedef gc_deque<tItem> tItems; //!< アイテム構造体のキューの typedef
	tItems Items; //!< アイテム構造体のキュー

public:
	//! @brief		コンストラクタ
	tCommandQueue();

	//! @brief		処理を実行する
	//! @param		node		ルートのプロセスノード
	void Process(tRootQueueNode * node);

	//! @brief		キューにコマンドを積む
	//! @param		node		キューノード
	//! @param		is_begin	BeginProcess を対象とするか(真) EndProcess を対象とするか(偽)
	void Push(tQueueNode * node, bool is_begin);

};
//---------------------------------------------------------------------------



class tInputPin;
//---------------------------------------------------------------------------
//! @brief		レンダリングのステートを保持するオブジェクト
//---------------------------------------------------------------------------
class tRenderState : public tCollectee
{
	typedef tCollectee inherited;

	tIdRegistry::tRenderGeneration	 RenderGeneration; //!< レンダリングした世代

	//!@brief 最長距離で比較する比較関数を用いたマップのtypedef
	typedef
		gc_map<tProcessNode *, tInputPin *, tProcessNode::tLongestDistanceComparator> tBuildQueueMap;

	//!@brief 最長距離で比較する比較関数を用いたマップ (キューの組み立てに使う)
	tBuildQueueMap BuildQueueMap;


public:
	//! @brief		コンストラクタ
	tRenderState();

	//! @brief		レンダリング世代を得る
	//! @return		レンダリング世代
	tIdRegistry::tRenderGeneration GetRenderGeneration() const { return RenderGeneration; }

	//! @brief		レンダリングを開始する
	//! @param		node		ルートのプロセスノード
	void Render(tProcessNode * node);

	//! @brief		キュー組み立てを行うための次のノードをpushする
	//! @param		input_pin		入力ピン(この入力ピンの先にあるノードがpushされる)
	void PushNextBuildQueueNode(tInputPin * input_pin);

};
//---------------------------------------------------------------------------


}

#endif
