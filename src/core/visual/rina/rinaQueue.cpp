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
#include "prec.h"
#include "visual/rina/rinaQueue.h"
#include "visual/rina/rinaPin.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(19972,63368,40219,19790,30879,4075,829,3560);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tQueueNode::tQueueNode(tQueueNode * parent)
{
	WaitingChildren = 0;
	WaitingParents = 0;

	if(parent)
	{
		parent->AddChild(this);
		Parents.push_back(parent);
		WaitingParents ++;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tQueueNode::Process(tCommandQueue * queue, bool is_begin)
{
	if(is_begin)
	{
		// BeginProcess を呼び出す
		BeginProcess();

		// Children の WaitingParents をデクリメントする。
		// それが 0 になった子は(依存関係が解決された子は)キューに push する。
		for(tNodes::iterator i = Children.begin(); i != Children.end(); i++)
		{
			if(-- (*i)->WaitingParents == 0) // TODO: アトミックなデクリメント
				queue->Push(*i, true);
		}

		if(Children.size() == 0)
		{
			// 子を持たないノードの場合は EndProcess を即呼び出す
			Process(queue, false);
		}
	}
	else
	{
		// EndProcess を呼び出す
		EndProcess();

		// Parents の WaitingChildren をデクリメントする。
		// それが 0 になった親は(依存関係が解決された親は)キューに push する。
		for(tNodes::iterator i = Parents.begin(); i != Parents.end(); i++)
		{
			if(-- (*i)->WaitingChildren == 0) // TODO: アトミックなデクリメント
				queue->Push(*i, false);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tQueueNode::AddParent(tQueueNode * parent)
{
	parent->AddChild(this);
	Parents.push_back(parent);
	WaitingParents++;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tQueueNode::AddChild(tQueueNode * child)
{
	Children.push_back(child);
	WaitingChildren++;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tCommandQueue::tCommandQueue()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCommandQueue::Process(tQueueNode * rootqueuenode)
{
	// ルートのキューノードを最初にキューに積む
	Push(rootqueuenode, true);

	// キューというかスタックが空になるまで実行を続ける。
	// 依存関係が解決された実行単位のみがここのキューにはいるため、
	// 基本的にここにたまっている実行単位はすべて同時実行が可能なもののみ。
	while(Items.size() > 0)
	{
		// TODO: 複数スレッドによる同時実行
		const tItem & item = Items.front();
		item.Node->Process(this, item.IsBegin);
		Items.pop_front();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCommandQueue::Push(tQueueNode * node, bool is_begin)
{
	tItem item;
	item.Node = node;
	item.IsBegin = is_begin;
	Items.push_back(item);
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tRenderState::tRenderState()
{
	RootQueueNode = NULL;
	RenderGeneration = tIdRegistry::instance()->GetNewRenderGeneration();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRenderState::Render(tProcessNode * node)
{
	// root を Map に挿入
	BuildQueueMap.insert(tBuildQueueMap::value_type(node, 0));

	RootQueueNode = NULL;

	// map が空になるまでループ
	while(BuildQueueMap.size() > 0)
	{
		// 先頭、すなわち最長距離が最も小さいノードから処理を行う
		tBuildQueueMap::iterator i = BuildQueueMap.begin();
		i->first->BuildQueue(this);
			// この間に PushNextBuildQueueNode() が呼ばれる可能性があることに注意
		BuildQueueMap.erase(i); // Map の場合は insert 後もイテレータは有効なのでここでeraseは可
	}

	// キューを作成し終わったということで
	RISSE_ASSERT(RootQueueNode != NULL);
	tCommandQueue command_queue;
	command_queue.Process(RootQueueNode);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRenderState::PushNextBuildQueueNode(tProcessNode * node)
{
	// insert 時にキーが同じ物、つまり同じノードインスタンスは
	// 重複することはない。これにより同じノードインスタンスが２回以上
	// 呼ばれないようにしている。
	BuildQueueMap.insert(tBuildQueueMap::value_type(node, 0));
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
