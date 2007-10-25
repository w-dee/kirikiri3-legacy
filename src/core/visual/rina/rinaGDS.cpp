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
tGDSNode * tGDSGraph::GetRoot()
{
	volatile tCriticalSection::tLocker lock(*CS);
	return Root;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNode * tGDSGraph::Freeze()
{
	volatile tCriticalSection::tLocker lock(*CS);

	++LastFreezedGeneration;

	return Root;
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tGDSNode::tUpdateLock::tUpdateLock(tGDSNode * node) : Node(node), Lock(node->Graph->GetCS())
{
	NewNode = node->BeginIndepend(NULL, NULL);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNode::tUpdateLock::~tUpdateLock()
{
	node->EndIndepend(NewNode);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tGDSNode::tGDSNode(tGDSGraph * graph)
{
	Graph = graph;
	LastGeneration = Graph->GetLastFreezedGeneration();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNode * tGDSNode::BeginIndepend(tGDSNode * oldchild, tGDSNode * oldchild)
{
	// このノードが記録している最終フリーズ世代とグラフ全体の最終フリーズ世代を比べる。
	// もしそれらが同じならばコピーは行わなくてよいが、異なっていればコピーを行う
	bool need_clone = LastGeneration != Graph->GetLastFreezedGeneration();

	// 必要ならば自分自身のクローンを作成
	tGDSNode * newnode = need_clone ? Clone() : this;

	// クローン/あるいは自分自身の子ノードのうち、oldchild に向いている
	// ポインタを newchild に置き換える
	if(oldchild) newnode->ReconnectChild(oldchild, newchild);

	return newnode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNode::EndIndepend(tGDSNode * newnode)
{
	// クローンを行わなかった場合は親ノードに再帰する必要はないので、ここで戻る
	if(newnode == this) return this;

	// ルートノードに向かってノードのクローンを作成していく
	if(Parents.size() == 0)
	{
		// 自分がルート
		Graph->SetRootNoLock(newnode);
	}
	else
	{
		// 親に向かって再帰
		for(tNodeVector::iterator i = Parents.begin(); i != Parents.end(); i++)
		{
			if(*i)
			{
				tGDSNode * parent_node = (*i)->BeginIndepend(this, newnode);
				(*i)->EndIndepend(parent_node, this, newnode);
			}
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNode::ReconnectChild(tGDSNode * oldnode, tGDSNode * newnode)
{
	// TODO: これ線形検索でいいんかいな
	tNodeVector::iterator i = std::find(Children.begin(), Children.end(), oldnode);
	RISSE_ASSERT(i != Children.end());
	(*i) = newnode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNode * tGDSNode::Clone() const
{
	return new tGDSNode(*this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Rina

