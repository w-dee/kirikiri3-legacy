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
//! @brief RINA GDS (Generational Data Structure)
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/rina/rinaGDS.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(36775,10222,29493,19855,18072,28928,53028,33122);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tGDSGraph::tGDSGraph()
{
	CS = new tCriticalSection();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNodeBase * tGDSGraph::GetRoot()
{
	volatile tCriticalSection::tLocker lock(*CS);
	return RootNode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNodeData * tGDSGraph::Freeze()
{
	volatile tCriticalSection::tLocker lock(*CS);

	++LastFreezedGeneration;

	return RootNode->GetCurrent();
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tGDSNodeData::tUpdateLock::tUpdateLock(tGDSNodeData * nodedata) :
	NodeData(nodedata),
	NewNodeData(NULL),
	Lock(nodedata->Node->GetGraph()->GetCS())
{
	NewNodeData = nodedata->BeginIndepend(NULL, NULL);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNodeData::tUpdateLock::~tUpdateLock()
{
	NodeData->EndIndepend(NewNodeData);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tGDSNodeData::tGDSNodeData(tGDSNodeBase * node)
{
	Node = node;
	RefCount = 1;
	LastGeneration = Node->GetGraph()->GetLastFreezedGeneration();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::Release()
{
	if(--RefCount == 0)
	{
		// 参照カウンタが 0 になった
		Node->GetPool()->Deallocate(this);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNodeData * tGDSNodeData::BeginIndepend(tGDSNodeData * oldchild, tGDSNodeData * newchild)
{
	// このノードが記録している最終フリーズ世代とグラフ全体の最終フリーズ世代を比べる。
	// もしそれらが同じならばコピーは行わなくてよいが、異なっていればコピーを行う
	bool need_clone = LastGeneration != Node->GetGraph()->GetLastFreezedGeneration();

	// 必要ならば最新状態のクローンを作成
	tGDSNodeData * newnodedata;
	if(need_clone)
	{
		newnodedata = Node->GetPool()->Allocate();
		newnodedata->Copy(this);
	}
	else
	{
		newnodedata = this;
	}

	// クローン/あるいはthisの子ノードのうち、oldchild に向いている
	// ポインタを newchild に置き換える
	if(oldchild) newnodedata->ReconnectChild(oldchild, newchild);

	return newnodedata;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::EndIndepend(tGDSNodeData * newnodedata)
{
	// クローンを行わなかった場合は親ノードに再帰する必要はないので、ここで戻る
	if(newnodedata == this) return;

	// ルートノードに向かってノードのクローンを作成していく
	if(Parents.size() == 0)
	{
		// 自分がルート
	}
	else
	{
		// 親に向かって再帰
		for(tNodeVector::iterator i = newnodedata->Parents.begin(); i != newnodedata->Parents.end(); i++)
		{
			if(*i)
			{
				tGDSNodeData * parent_node_data = (*i)->BeginIndepend(this, newnodedata);
				(*i)->EndIndepend(parent_node_data);
			}
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::ReconnectChild(tGDSNodeData * oldnodedata, tGDSNodeData * newnodedata)
{
	// TODO: これ線形検索でいいんかいな
	tNodeVector::iterator i = std::find(Children.begin(), Children.end(), oldnodedata);
	RISSE_ASSERT(i != Children.end());
	oldnodedata->Release();
	newnodedata->AddRef();
	(*i) = newnodedata;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::Copy(const tGDSNodeData * rhs)
{
	Node = rhs->Node;
	Parents = rhs->Parents;
	Children = rhs->Children;
	LastGeneration = Node->GetGraph()->GetLastFreezedGeneration();
	// 参照カウンタはコピーしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::Free()
{
	RefCount = 1; // 参照カウンタを 1 にリセットしておく
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tGDSNodeBase::tGDSNodeBase(tGDSGraph * graph, tGDSPoolBase * pool)
{
	Graph = graph;
	Pool = pool;
	Current = Pool->Allocate();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Rina

