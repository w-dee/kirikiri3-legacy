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
void tGDSGraph::SetRoot(tGDSNodeBase * root)
{
	volatile tCriticalSection::tLocker lock(*CS);
	RootNode = root;
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
tGDSNodeData::tGDSNodeData(tGDSNodeBase * node)
{
	Node = node;
	RefCount = 0;
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
void tGDSNodeData::SetChildAt(risse_size cn, tGDSNodeData * nodedata, risse_size pn)
{
	// ノードデータの親を設定する

	// 親子の領域は空いてる？
	if(Children.size() <= cn)
		Children.resize(cn + 1, NULL);
	if(nodedata->Parents.size() <= pn)
		nodedata->Parents.resize(pn + 1, NULL);

	// 置き換える場所に既に子が設定されている場合は その子を Release する
	if(Children[cn] != NULL) Children[cn]->Release();

	// 子を書き込み、子 (nodedata) を AddRef する
	Children[cn] = nodedata;
	nodedata->AddRef();

	// 親を書き込む
	nodedata->Parents[pn] = this;
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
		// メモリ領域を確保
		newnodedata = Node->GetPool()->Allocate();

		// すべての子のうち、this を向いている親を newnodedata を向くように置き換える
		for(tNodeVector::iterator i = Children.begin(); i != Children.end(); i++)
		{
			(*i)->ReconnectParent(this, newnodedata);
		}

		// 内容をコピー
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

	// current を設定する
	Node->SetCurrent(newnodedata);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::ReconnectParent(tGDSNodeData * oldnodedata, tGDSNodeData * newnodedata)
{
	// TODO: これ線形検索でいいんかいな
	tNodeVector::iterator i = std::find(Parents.begin(), Parents.end(), oldnodedata);
	RISSE_ASSERT(i != Parents.end());
	(*i) = newnodedata;
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
void tGDSNodeData::DumpGraphviz() const
{
	// ヘッダを出力
	wxPrintf(wxT("digraph GDS {\n"));

	gc_vector<const tGDSNodeData *> nodedatas;

	// phase 1: すべてのノードを書き出す
	nodedatas.push_back(this);
	while(nodedatas.size() > 0)
	{
		const tGDSNodeData * nodedata = nodedatas.back();
		nodedatas.pop_back();
		wxPrintf(wxT("\t0x%p [label=\"%s\", shape=box];\n"),
			nodedata,
			(nodedata->GetName() + RISSE_WS("\\n") + nodedata->DumpText()).AsWxString().c_str());

		for(tNodeVector::const_iterator i = nodedata->Children.begin();
			i != nodedata->Children.end(); i++)
			nodedatas.push_back(*i);
	}

	// phase 2: すべてのエッジを書き出す
	nodedatas.push_back(this);
	while(nodedatas.size() > 0)
	{
		const tGDSNodeData * nodedata = nodedatas.back();
		nodedatas.pop_back();
		for(tNodeVector::const_iterator i = nodedata->Children.begin();
			i != nodedata->Children.end(); i++)
		{
			wxPrintf(wxT("\t0x%p -> 0x%p;\n"),
				nodedata, (*i));
			nodedatas.push_back(*i);
		}
	}

	// フッタを出力
	wxPrintf(wxT("}\n"));

	// 標準出力をフラッシュ
	fflush(stdout);
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

	// コピーした時点で子の参照カウンタがそれぞれ増える
	for(tNodeVector::iterator i = Children.begin(); i != Children.end(); i++)
	{
		(*i)->AddRef();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::Free()
{
	RefCount = 0; // 参照カウンタを 0 にリセットしておく
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tGDSNodeData::GetName() const
{
	return tString(RISSE_WS("Generation ")) +
		tString::AsString((int)LastGeneration.operator risse_uint32());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tGDSNodeData::DumpText() const
{
	return tString();
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
tString tGDSNodeBase::GetName() const
{
	risse_char tmp[25];
	pointer_to_str(this, tmp);

	return tString(RISSE_WS("Node@0x")) + tmp;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tGDSNodeBase::DumpText() const
{
	return tString();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Rina





#define RINA_GDS_TRIVIAL_TEST
#ifdef RINA_GDS_TRIVIAL_TEST
//---------------------------------------------------------------------------
namespace Risa {

using namespace Rina;
//---------------------------------------------------------------------------
class tTestNodeData : public tGDSNodeData
{
	typedef tGDSNodeData inherited;
	tString Text; //!< データとして持つテキスト
public:
	tTestNodeData(tGDSNodeBase * node) : tGDSNodeData(node) {;}
	const tString & GetText() const { return Text; }
	void SetText(const tString & text) { Text = text; }

	void Copy(const tGDSNodeData * rhs)
	{
		Text = reinterpret_cast<const tTestNodeData*>(rhs)->Text;
		inherited::Copy(rhs);
	}

	tString DumpText() const { return Text; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
class tTestNode : public tGDSNode<tTestNodeData>
{
	typedef tGDSNode<tTestNodeData> inherited;
public:
	tTestNode(tGDSGraph * graph) : inherited(graph) {;}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
class tGDSTester : public singleton_base<tGDSTester>
{
public:
	tGDSTester();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSTester::tGDSTester()
{
	tGDSGraph * graph = new tGDSGraph();

	tTestNode * node1 = new tTestNode(graph); // will be root
	tTestNode * node2 = new tTestNode(graph);
	tTestNode * node3 = new tTestNode(graph);

	graph->SetRoot(node1);

	node1->GetCurrent()->SetText(RISSE_WS("node1:A"));
	node2->GetCurrent()->SetText(RISSE_WS("node2:A"));
	node3->GetCurrent()->SetText(RISSE_WS("node3:A"));

	node1->GetCurrent()->SetChildAt(0, node2->GetCurrent(), 0);
	node1->GetCurrent()->SetChildAt(1, node3->GetCurrent(), 0);

	node1->GetCurrent()->DumpGraphviz();

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node2);
		lock.GetNewNodeData()->SetText(RISSE_WS("node2:B"));
	}

	node1->GetCurrent()->DumpGraphviz();

	printf("freeze\n");

	tTestNodeData * g1 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node2);
		lock.GetNewNodeData()->SetText(RISSE_WS("node2:C"));
	}

	node1->GetCurrent()->DumpGraphviz();

	printf("display freezed node\n");
	g1->DumpGraphviz();

	printf("change node value again\n");
	{
		tTestNode::tUpdateLock lock(node2);
		lock.GetNewNodeData()->SetText(RISSE_WS("node2:D"));
	}

	node1->GetCurrent()->DumpGraphviz();

	printf("display freezed node\n");
	g1->DumpGraphviz();

	printf("change another node value\n");
	{
		tTestNode::tUpdateLock lock(node3);
		lock.GetNewNodeData()->SetText(RISSE_WS("node3:E"));
	}

	node1->GetCurrent()->DumpGraphviz();

	printf("display freezed node\n");
	g1->DumpGraphviz();

}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif



