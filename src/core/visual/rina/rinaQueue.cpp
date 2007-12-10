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
void tQueueNode::AddChild(tQueueNode * child)
{
	Children.push_back(child);
	WaitingChildren++;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void tRootQueueNode::BeginProcess()
{
	// なにもしない
	RISSE_ASSERT(Children.size() == 1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRootQueueNode::EndProcess()
{
	// なにもしない
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tCommandQueue::tCommandQueue()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCommandQueue::Process(tProcessNode * node)
{
	// ルートのキューノードを作成する
	tRootQueueNode * rootqueuenode = new tRootQueueNode();

	// それを頂点にキューノードを作成する
	node->BuildQueue(rootqueuenode);

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
}
