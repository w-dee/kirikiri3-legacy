//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA コマンドキュー管理
//---------------------------------------------------------------------------
#ifndef RINAQUEUE_H
#define RINAQUEUE_H

#include "risa/packages/risa/graphic/rina/rinaNode.h"
#include "risa/packages/risa/graphic/rina/rinaIdRegistry.h"

namespace Risa {
//---------------------------------------------------------------------------


/*!
	@note
	コマンドキューとは言っても線形キューではなくて、依存関係を表せるように
	DAGによる依存グラフを作成した後、それぞれの BeginProcess と EndProcess を
	実行単位として 順々に tQueue 内のキューに push していき、
	キューからその実行単位を取り出しながら実行を行っていく。
	DAGの各ノードは、依存しているノードが実行される前に実行したい内容である
	BeginProcess と、依存しているノードが実行された後に実行したい内容である
	EndProcess をもち、大まかには ルートの BeginProcess から末端のBeginProcessへ、
	末端のEndProcessからルートのEndProcessへの順に実行が行われる。
*/

class tQueueNode;
//---------------------------------------------------------------------------
/**
 * レンダリング要求の基底クラス
 */
class tRenderRequest : public tPolymorphic
{
public:
	typedef tPolymorphic inherited;
private:

	tQueueNode * Parent; //!< 親キューノード
	risse_size Index; //!< 親キューノード内でのインデックス

public:
	/**
	 * コンストラクタ
	 * @param parent	親キューノード
	 * @param index		親キューノード内でのインデックス
	 */
	tRenderRequest(tQueueNode * parent, risse_size index) : Parent(parent), Index(index) {;}

	/**
	 * 親キューノードを得る
	 * @return	親キューノード
	 */
	tQueueNode * GetParent() const { return Parent; }

	/**
	 * 親キューノード内でのインデックスを得る
	 * @return	親キューノード内でのインデックス
	 */
	risse_size GetIndex() const { return Index; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 子ノードの情報
 */
class tQueueNodeChild : public tCollectee
{
	typedef tCollectee inherited;

	tQueueNode * Child; //!< 子キューノード
	const tRenderRequest * RenderRequest; //!< 子キューノードが保持していたレンダリング要求の情報

public:
	/**
	 * デフォルトコンストラクタ
	 */
	tQueueNodeChild() : Child(NULL), RenderRequest(NULL) {;}

	/**
	 * コンストラクタ
	 * @param child		子キューノード
	 * @param request	子キューノードが保持していたレンダリング要求の情報
	 */
	tQueueNodeChild(tQueueNode * child, const tRenderRequest * request = NULL) :
		Child(child), RenderRequest(request) {;}

	/**
	 * 子キューノードを得る
	 * @return	子キューノード
	 */
	tQueueNode * GetChild() const { return Child; }

	/**
	 * 子キューノードが保持していたレンダリング要求の情報を得る
	 * @return	子キューノードが保持していたレンダリング要求の情報
	 */
	const tRenderRequest * GetRenderRequest() const { return RenderRequest; }
};
//---------------------------------------------------------------------------



class tQueue;
//---------------------------------------------------------------------------
/**
 * コマンドキューノード
 */
class tQueueNode : public tPolymorphic
{
public:
	typedef tPolymorphic inherited;
private:

protected:
	typedef gc_vector<tQueueNodeChild> tChildren; //!< 子ノードの配列のtypedef
	typedef gc_vector<const tRenderRequest*> tParents; //!< レンダリング要求(親ノード)の配列のtypedef
	tChildren Children; //!< 子ノード
	tParents Parents; //!< 親ノード

	risse_size WaitingChildren; //!< 子ノードを待っているカウント
	risse_size WaitingParents; //!< 親キューノードを待っているカウント TODO: より効率的な実装

public:
	/**
	 * コンストラクタ
	 * @param request	レンダリング要求
	 */
	tQueueNode(const tRenderRequest * request);

	/**
	 * デストラクタ (おそらく呼ばれない)
	 */
	virtual ~tQueueNode() {;}

	/**
	 * ノードの処理を行う
	 * @param queue		コマンドキューインスタンス
	 * @param is_begin	BeginProcess を対象とするか(真) EndProcess を対象とするか(偽)
	 */
	void Process(tQueue * queue, bool is_begin);

	/**
	 * ノードの親とレンダリング要求を追加する
	 * @param parent	親
	 */
	void AddParent(const tRenderRequest * request);

protected:
	/**
	 * ノードの子を追加する
	 * @param child		子
	 * @param request	レンダリング要求
	 */
	void AddChild(tQueueNode * child, const tRenderRequest * request);

protected: //!< サブクラスでオーバーライドして使う物

	/**
	 * ノードの処理の最初に行う処理
	 */
	virtual void BeginProcess() = 0;

	/**
	 * ノードの処理の最後に行う処理
	 */
	virtual void EndProcess() = 0;

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * コマンドキュー
 */
class tQueue : public tCollectee
{
	typedef tCollectee inherited;

	/**
	 * アイテム構造体
	 */
	struct tItem
	{
		tQueueNode * Node; //!< ノード
		bool IsBegin; //!< BeginProcess を対象とするか(真) EndProcess を対象とするか(偽)
	};

	typedef gc_deque<tItem> tItems; //!< アイテム構造体のキューの typedef
	tItems Items; //!< アイテム構造体のキュー

public:
	/**
	 * コンストラクタ
	 */
	tQueue();

	/**
	 * 処理を実行する
	 * @param node	ルートのプロセスノード
	 */
	void Process(tQueueNode * node);

	/**
	 * キューにコマンドを積む
	 * @param node		キューノード
	 * @param is_begin	BeginProcess を対象とするか(真) EndProcess を対象とするか(偽)
	 */
	void Push(tQueueNode * node, bool is_begin);

};
//---------------------------------------------------------------------------



class tInputPin;
class tGraphInstance;
//---------------------------------------------------------------------------
/**
 * コマンドキューを作成するためのオブジェクト
 */
class tQueueBuilder : public tCollectee
{
	typedef tCollectee inherited;

	tGraphInstance * GraphInstance; //!< グラフインスタンス

	tIdRegistry::tRenderGeneration	 RenderGeneration; //!< レンダリングした世代

	/**
	 * 最長距離で比較する比較関数を用いたマップのtypedef
	 */
	typedef
		gc_map<tNodeInstance *, int, tNodeInstance::tLongestDistanceComparator> tBuildQueueMap;

	/**
	 * 最長距離で比較する比較関数を用いたマップ (キューの組み立てに使う)
	 */
	tBuildQueueMap BuildQueueMap;

	tQueueNode * RootQueueNode; //!< ルートとなるコマンドキューノード

public:
	/**
	 * コンストラクタ
	 * @param graph	グラフインスタンス
	 */
	tQueueBuilder(tGraphInstance * graph);

	/**
	 * レンダリング世代を得る
	 * @return	レンダリング世代
	 */
	tIdRegistry::tRenderGeneration GetRenderGeneration() const { return RenderGeneration; }

	/**
	 * コマンドキューを作成する
	 * @param node	ルートのプロセスノード
	 */
	void Build(tNodeInstance * node);

	/**
	 * キュー組み立てを行うための次のノードをpushする
	 * @param node	ノード
	 */
	void Push(tNodeInstance * node);

	/**
	 * ルートとなるコマンドキューノードを登録する
	 * @param node	ルートとなるコマンドキューノード
	 */
	void SetRootQueueNode(tQueueNode * node) { RootQueueNode = node; }

	/**
	 * ルートキューノードを得る
	 */
	tQueueNode * GetRootQueueNode() const { return RootQueueNode; }

};
//---------------------------------------------------------------------------


}

#endif
