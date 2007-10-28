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
	return Root;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNodeBase * tGDSGraph::Freeze()
{
	volatile tCriticalSection::tLocker lock(*CS);

	++LastFreezedGeneration;

	return Root;
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
void tGDSNodeData::ReconnectChild(tGDSNodeData * oldnodedata, tGDSNodeData * newnodedata)
{
	// TODO: これ線形検索でいいんかいな
	tNodeVector::iterator i = std::find(Children.begin(), Children.end(), oldnodedata);
	RISSE_ASSERT(i != Children.end());
	oldnodedata->Release();
	newnodedata->AddRef();
	(*i) = newnode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::Copy(const tGDSNodeData * rhs)
{
	Node = rhs->Node;
	Parents = rhs->Parents;
	Children = rhs->Children;
	// 参照カウンタはコピーしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::Free()
{
	// 何もしない
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tGDSNodeBase::tUpdateLock::tUpdateLock(tGDSNodeBase * node) : Lock(node->Graph->GetCS())
{
	NewNodeData = node->BeginIndepend(NULL, NULL);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNodeBase::tUpdateLock::~tUpdateLock()
{
	node->EndIndepend(NewNodeData);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tGDSNodeBase::tGDSNodeBase(tGDSGraph * graph, tGDSPoolBase * pool)
{
	Graph = graph;
	Pool = pool;
	LastGeneration = Graph->GetLastFreezedGeneration();
	Current = Pool->Allocate();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNodeData * tGDSNodeBase::BeginIndepend(tGDSNodeData * oldchild, tGDSNodeData * newchild);
{
	// このノードが記録している最終フリーズ世代とグラフ全体の最終フリーズ世代を比べる。
	// もしそれらが同じならばコピーは行わなくてよいが、異なっていればコピーを行う
	bool need_clone = LastGeneration != Graph->GetLastFreezedGeneration();

	// 必要ならば最新状態のクローンを作成
	tGDSNodeBase * newnodedata;
	if(need_clone)
	{
		newnodedata = Pool->Allocate();
		Pool->Copy(Current);
	}
	else
	{
		newnodedata = Current;
	}

	// クローン/あるいはCurrentの子ノードのうち、oldchild に向いている
	// ポインタを newchild に置き換える
	if(oldchild) newnodedata->ReconnectChild(oldchild, newchild);

	return newnodedata;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeBase::EndIndepend(tGDSNodeData * newnodedata);
{
	// クローンを行わなかった場合は親ノードに再帰する必要はないので、ここで戻る
	if(newnode == this) return this;

	// ルートノードに向かってノードのクローンを作成していく
	if(Parents.size() == 0)
	{
		// 自分がルート
	}
	else
	{
		// 親に向かって再帰
		for(tNodeVector::iterator i = Parents.begin(); i != Parents.end(); i++)
		{
			if(*i)
			{
				tGDSNodeData * parent_node_data = (*i)->BeginIndepend(Current, newnodedata);
				(*i)->EndIndepend(parent_node_data);
			}
		}
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Rina

