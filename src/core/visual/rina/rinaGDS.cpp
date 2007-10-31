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

	tGDSNodeData * newroot = RootNode->GetCurrent();
	newroot->AddRef();
	return newroot;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
void tGDSPoolBase::ThrowAllocationLogicError()
{
	RISSE_ASSERT(!"Allocation logic error in tGDSPool::Allocate()");
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tGDSNodeData::tGDSNodeData(tGDSNodeBase * node)
{
	CS = new tCriticalSection();
	Node = node;
	RefCount.reset();
	LastGeneration = Node->GetGraph()->GetLastFreezedGeneration();
	DumpMark = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::Release()
{
	if(-- RefCount == 0)
	{
		volatile tCriticalSection::tLocker lock(*CS);

		// 参照カウンタが 0 になった
		// 子をすべて解放する
		for(tNodeVector::iterator i = Children.begin(); i != Children.end(); i++)
		{
			(*i)->Release();
		}
		Node->GetPool()->Deallocate(this);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeData::SetChildAt(risse_size cn, tGDSNodeData * nodedata, risse_size pn)
{
	// ノードデータの親を設定する
	volatile tCriticalSection::tLocker lock(*CS);

	// 親子の領域は空いてる？
	if(Children.size() <= cn)
		Children.resize(cn + 1, NULL);

	// 置き換える場所に既に子が設定されている場合は その子を Release する
	if(Children[cn] != NULL) Children[cn]->Release();

	// 子を書き込み、子 (nodedata) を AddRef する
	Children[cn] = nodedata;
	nodedata->AddRef();

	// 親を書き込む
	nodedata->Node->SetParentAt(pn, this->Node);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSNodeData * tGDSNodeData::BeginIndepend(tGDSNodeData * oldchild, tGDSNodeData * newchild)
{
	volatile tCriticalSection::tLocker lock(*CS);

	// このノードが記録している最終フリーズ世代とグラフ全体の最終フリーズ世代を比べる。
	// もしそれらが同じならばコピーは行わなくてよいが、異なっていればコピーを行う
	bool need_clone = LastGeneration != Node->GetGraph()->GetLastFreezedGeneration();

	// 必要ならば最新状態のクローンを作成
	tGDSNodeData * newnodedata;
	if(need_clone)
	{
		// メモリ領域を確保
		newnodedata = Node->GetPool()->Allocate();

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
	volatile tCriticalSection::tLocker lock(*CS);

	// クローンを行わなかった場合は親ノードに再帰する必要はないので、ここで戻る
	if(newnodedata == this) return;

	// ルートノードに向かってノードのクローンを作成していく

	// 親に向かって再帰
	risse_size parent_count = Node->GetParentCount();
	for(risse_size i = 0; i < parent_count; i++)
	{
		tGDSNodeBase * node = Node->GetParentAt(i);
		if(node)
		{
			tGDSNodeData * parent_node_data = node->GetCurrent()->BeginIndepend(this, newnodedata);
			node->GetCurrent()->EndIndepend(parent_node_data);
		}
	}

	// current を設定する
	Node->SetCurrent(newnodedata);
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
	volatile tCriticalSection::tLocker lock(*CS);

	wxPrintf(wxT("digraph GDS {\n"));

	gc_vector<const tGDSNodeData *> nodedatas;

	void * dumpmark;

	// phase 1: すべてのノードを書き出す
	dumpmark = MallocAtomicCollectee(1);
	nodedatas.push_back(this);
	while(nodedatas.size() > 0)
	{
		const tGDSNodeData * nodedata = nodedatas.back();
		nodedatas.pop_back();
		nodedata->DumpMark = dumpmark;

		wxPrintf(wxT("\t0x%p [label=\"%s\", shape=box];\n"),
			nodedata,
			(nodedata->GetName() + RISSE_WS("\\n") + nodedata->DumpText()).AsWxString().c_str());

		for(tNodeVector::const_iterator i = nodedata->Children.begin();
			i != nodedata->Children.end(); i++)
		{
			if((*i)->DumpMark != dumpmark)
				nodedatas.push_back(*i);
		}
	}


	// phase 2: すべてのエッジを書き出す
	dumpmark = MallocAtomicCollectee(1);
	nodedatas.push_back(this);
	while(nodedatas.size() > 0)
	{
		const tGDSNodeData * nodedata = nodedatas.back();
		nodedatas.pop_back();
		nodedata->DumpMark = dumpmark;

		for(tNodeVector::const_iterator i = nodedata->Children.begin();
			i != nodedata->Children.end(); i++)
		{
			wxPrintf(wxT("\t0x%p -> 0x%p;\n"),
				nodedata, (*i));
			if((*i)->DumpMark != dumpmark)
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
	// AddRef/Release と同時に実行するとクラッシュする可能性があるので注意
	// (ロジック上はそういうことがないようにできるはずだが)
	volatile tCriticalSection::tLocker lock(*CS);

	Node = rhs->Node;
	Children = rhs->Children;
	LastGeneration = Node->GetGraph()->GetLastFreezedGeneration();
	RefCount.reset(); // 参照カウンタはコピーされた時点で0になる
	DumpMark = NULL;

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
	// AddRef/Release と同時に実行するとクラッシュする可能性があるので注意
	// (ロジック上はそういうことがないようにできるはずだが)
	RefCount.reset(); // 参照カウンタを 0 にリセットしておく
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tGDSNodeData::GetName() const
{
	volatile tCriticalSection::tLocker lock(*CS);

	return tString(RISSE_WS("G:")) +
		tString::AsString((int)LastGeneration.operator risse_uint32()) +
		RISSE_WS(" R:") +
		tString::AsString((int)(RefCount.operator long()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tGDSNodeData::DumpText() const
{
	volatile tCriticalSection::tLocker lock(*CS);

	return tString();
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
tGDSNodeBase::tGDSNodeBase(tGDSGraph * graph, tGDSPoolBase * pool)
{
	Graph = graph;
	Pool = pool;
	Current = Pool->Allocate();
	Current->AddRef();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeBase::SetCurrent(tGDSNodeData * nodedata)
{
	tCriticalSection::tLocker lock(Graph->GetCS());
	Current->Release();
	Current = nodedata;
	Current->AddRef();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeBase::SetParentAt(risse_size n, tGDSNodeBase * node)
{
	tCriticalSection::tLocker lock(Graph->GetCS());
	// ノードの親を設定する

	// 親の領域は空いてる？
	if(Parents.size() <= n)
		Parents.resize(n + 1, NULL);

	// 親を書き込む
	Parents[n] = node;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSNodeBase::ReconnectParent(tGDSNodeBase * oldnode, tGDSNodeBase * newnode)
{
	tCriticalSection::tLocker lock(Graph->GetCS());

	// TODO: これ線形検索でいいんかいな
	tNodeVector::iterator i = std::find(Parents.begin(), Parents.end(), oldnode);
	RISSE_ASSERT(i != Parents.end());
	(*i) = newnode;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tGDSNodeBase::GetName() const
{
	tCriticalSection::tLocker lock(Graph->GetCS());

	risse_char tmp[25];
	pointer_to_str(this, tmp);

	return tString(RISSE_WS("Node@0x")) + tmp;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tGDSNodeBase::DumpText() const
{
	tCriticalSection::tLocker lock(Graph->GetCS());

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
	void test1();
	void test2();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tGDSTester::tGDSTester()
{
	printf("------------- test1 -------------\n");
	fflush(stdout);
	test1();
	fflush(stdout);
	printf("------------- test2 -------------\n");
	fflush(stdout);
	test2();
	fflush(stdout);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSTester::test1()
{
	tGDSGraph * graph = new tGDSGraph();

	tTestNodeData * g1;
	tTestNodeData * g2;

	tTestNode * node1 = new tTestNode(graph); // will be root
	tTestNode * node2 = new tTestNode(graph);
	tTestNode * node3 = new tTestNode(graph);

	graph->SetRoot(node1);

	node1->GetCurrent()->SetText(RISSE_WS("node1:A"));
	node2->GetCurrent()->SetText(RISSE_WS("node2:A"));
	node3->GetCurrent()->SetText(RISSE_WS("node3:A"));

	node1->SetChildAt(0, node2, 0);
	node1->SetChildAt(1, node3, 0);

	node1->GetCurrent()->DumpGraphviz();

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node2);
		lock.GetNewNodeData()->SetText(RISSE_WS("node2:B"));
	}

	node1->GetCurrent()->DumpGraphviz();

	printf("freeze\n");

	g1 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

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

	printf("relesae freezed node\n");
	g1->Release();
	node1->GetCurrent()->DumpGraphviz();

	printf("freeze\n");

	g1 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

	printf("display freezed node\n");
	g1->DumpGraphviz();

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node2);
		lock.GetNewNodeData()->SetText(RISSE_WS("node2:F"));
	}

	node1->GetCurrent()->DumpGraphviz();

	printf("display freezed node\n");
	g1->DumpGraphviz();


	printf("relesae freezed node\n");
	g1->Release();
	node1->GetCurrent()->DumpGraphviz();


	printf("freeze g1\n");

	g1 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node2);
		lock.GetNewNodeData()->SetText(RISSE_WS("node2:G"));
	}


	printf("freeze g2\n");

	g2 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node3);
		lock.GetNewNodeData()->SetText(RISSE_WS("node3:H"));
	}

	printf("dump current\n");
	node1->GetCurrent()->DumpGraphviz();
	printf("dump g1\n");
	g1->DumpGraphviz();
	printf("dump g2\n");
	g2->DumpGraphviz();

	printf("freeze more\n");

	tTestNodeData * g3 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

	printf("attempt to change node value ...");

	try
	{
		tTestNode::tUpdateLock lock(node3);
		lock.GetNewNodeData()->SetText(RISSE_WS("node3:I"));
	}
	catch(...)
	{
		printf("successfully crashed\n");
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tGDSTester::test2()
{
	tGDSGraph * graph = new tGDSGraph();

	tTestNodeData * g1;
	tTestNodeData * g2;

	tTestNode * node1 = new tTestNode(graph); // will be root
	tTestNode * node2 = new tTestNode(graph);
	tTestNode * node3 = new tTestNode(graph);
	tTestNode * node4 = new tTestNode(graph);

	graph->SetRoot(node1);

	node1->GetCurrent()->SetText(RISSE_WS("node1:A"));
	node2->GetCurrent()->SetText(RISSE_WS("node2:A"));
	node3->GetCurrent()->SetText(RISSE_WS("node3:A"));
	node4->GetCurrent()->SetText(RISSE_WS("node4:A"));

	node1->SetChildAt(0, node2, 0);
	node1->SetChildAt(1, node3, 0);
	node2->SetChildAt(0, node4, 0);
	node3->SetChildAt(0, node4, 1);

	node1->GetCurrent()->DumpGraphviz();

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node4);
		lock.GetNewNodeData()->SetText(RISSE_WS("node4:B"));
	}

	node1->GetCurrent()->DumpGraphviz();

	printf("freeze\n");

	g1 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node4);
		lock.GetNewNodeData()->SetText(RISSE_WS("node4:C"));
	}

	node1->GetCurrent()->DumpGraphviz();

	printf("display freezed node\n");
	g1->DumpGraphviz();

	printf("change node value again\n");
	{
		tTestNode::tUpdateLock lock(node4);
		lock.GetNewNodeData()->SetText(RISSE_WS("node4:D"));
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

	printf("relesae freezed node\n");
	g1->Release();
	node1->GetCurrent()->DumpGraphviz();

	printf("freeze\n");

	g1 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

	printf("display freezed node\n");
	g1->DumpGraphviz();

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node4);
		lock.GetNewNodeData()->SetText(RISSE_WS("node4:F"));
	}

	node1->GetCurrent()->DumpGraphviz();

	printf("display freezed node\n");
	g1->DumpGraphviz();


	printf("relesae freezed node\n");
	g1->Release();
	node1->GetCurrent()->DumpGraphviz();


	printf("freeze g1\n");

	g1 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node4);
		lock.GetNewNodeData()->SetText(RISSE_WS("node4:G"));
	}


	printf("freeze g2\n");

	g2 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

	printf("change node value\n");

	{
		tTestNode::tUpdateLock lock(node3);
		lock.GetNewNodeData()->SetText(RISSE_WS("node3:H"));
	}

	printf("dump current\n");
	node1->GetCurrent()->DumpGraphviz();
	printf("dump g1\n");
	g1->DumpGraphviz();
	printf("dump g2\n");
	g2->DumpGraphviz();

	printf("freeze more\n");

	tTestNodeData * g3 = reinterpret_cast<tTestNodeData*>(graph->Freeze());

	printf("attempt to change node value ...");

	try
	{
		tTestNode::tUpdateLock lock(node4);
		lock.GetNewNodeData()->SetText(RISSE_WS("node4:I"));
	}
	catch(...)
	{
		printf("successfully crashed\n");
	}
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif



