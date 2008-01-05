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

class tQueueNode;
//---------------------------------------------------------------------------
//! @brief		レンダリング要求の基底クラス
//---------------------------------------------------------------------------
class tRenderRequest : public tCollectee
{
	typedef tCollectee inherited;

	tQueueNode * Parent; //!< 親キューノード
public:
	tRenderRequest(tQueueNode * parent) : Parent(parent) {;}

	//! @brief		親キューノードを得る
	//! @return		親キューノード
	tQueueNode * GetParent() const { return Parent; }
};
//---------------------------------------------------------------------------



class tCommandQueue;
//---------------------------------------------------------------------------
//! @brief		コマンドキューノード
//---------------------------------------------------------------------------
class tQueueNode : public tCollectee
{
	typedef tCollectee inherited;

protected:
	typedef gc_vector<tQueueNode*> tChildren; //!< 子ノードの配列のtypedef
	typedef gc_vector<const tRenderRequest*> tParents; //!< レンダリング要求(親ノード)の配列のtypedef
	tChildren Children; //!< 子ノード
	tParents Parents; //!< 親ノード

	risse_size WaitingChildren; //!< 子ノードを待っているカウント
	risse_size WaitingParents; //!< 親キューノードを待っているカウント TODO: より効率的な実装

public:
	//! @brief		コンストラクタ
	//! @param		request		レンダリング要求
	tQueueNode(const tRenderRequest * request);

	//! @brief		デストラクタ (おそらく呼ばれない)
	virtual ~tQueueNode() {;}

	//! @brief		ノードの処理を行う
	//! @param		queue		コマンドキューインスタンス
	//! @param		is_begin	BeginProcess を対象とするか(真) EndProcess を対象とするか(偽)
	void Process(tCommandQueue * queue, bool is_begin);

	//! @brief		親に対応するレンダリング要求を得る
	//! @param		node		親ノード
	//! @return		その親ノードを指し示しているレンダリング要求
	const tRenderRequest * GetRenderRequest(const tQueueNode * node) const;

	//! @brief		ノードの親とレンダリング要求を追加する
	//! @param		parent		親
	void AddParent(const tRenderRequest * request);

	//! @brief		インターフェースを返す
	//! @param		type		返すインターフェースに対応するエッジタイプ
	//! @return		そのインターフェース(NULL=対応していない)
	virtual void * GetInterface(risse_uint32 type) { return NULL; }

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
	void Process(tQueueNode * node);

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
		gc_map<tProcessNode *, int, tProcessNode::tLongestDistanceComparator> tBuildQueueMap;

	//!@brief 最長距離で比較する比較関数を用いたマップ (キューの組み立てに使う)
	tBuildQueueMap BuildQueueMap;

	tQueueNode * RootQueueNode; //!< ルートとなるコマンドキューノード

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
	//! @param		node		ノード
	void PushNextBuildQueueNode(tProcessNode * node);

	//! @brief		ルートとなるコマンドキューノードを登録する
	//! @param		node	ルートとなるコマンドキューノード
	void SetRootQueueNode(tQueueNode * node) { RootQueueNode = node; }

};
//---------------------------------------------------------------------------


}

#endif
