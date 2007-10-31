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
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//!
//---------------------------------------------------------------------------
class tGDSGeneration : public tCollectee
{
	risse_uint32		Data;
public:
	tGDSGeneration() { Data = 0; }
	tGDSGeneration(const tGDSGeneration & r) { Data = r.Data; }
	tGDSGeneration(risse_uint32 g) { Data = g; }

	operator risse_uint32() const { return Data; }

	tGDSGeneration & operator = (const tGDSGeneration & r) { Data = r.Data; return *this; }
	bool operator == (const tGDSGeneration & r) const { return Data == r.Data; }
	bool operator != (const tGDSGeneration & r) const { return ! operator ==(r); }

	bool operator < (const tGDSGeneration & r) const
	{
		// 世代の比較は、wrap around を考慮しなければならない
		return static_cast<risse_int32>(Data - r.Data) < 0;
	}

	void operator ++ (void)
	{
		++Data;
	}
};
//---------------------------------------------------------------------------






class tGDSNodeBase;
class tGDSNodeData;
//---------------------------------------------------------------------------
//! @brief		GDSグラフ
//---------------------------------------------------------------------------
class tGDSGraph : public tCollectee
{
	tGDSGeneration		LastFreezedGeneration; //!< 一番最後にフリーズを行った世代
	tCriticalSection * CS; //!< このグラフを保護するクリティカルセクション
	tGDSNodeBase * RootNode; //!< (現在の最新の)ルートノード

public:
	static const int MaxGenerations = 3; //!< 最大で何世代までの管理を許すか(「最新の」世代も含める)
								//!< 3 = トリプルバッファリングを考慮した際の最大値

public:
	//! @brief		コンストラクタ
	tGDSGraph();

	//! @brief		一番最後にフリーズを行った世代を返す
	const tGDSGeneration & GetLastFreezedGeneration() const
		{ return LastFreezedGeneration; }

	//! @brief		このグラフを保護するクリティカルセクションを得る
	tCriticalSection & GetCS() { return *CS; }

	//! @brief		ルートノードを設定する
	//! @param		root	ルートノード
	void SetRoot(tGDSNodeBase * root);

	//! @brief		ルートノードを得る
	tGDSNodeBase * GetRoot();

	//! @brief		ルートノードを設定する(ロックはしない)
	//! @param		rootnode	新しいルートノード
	void SetRootNoLock(tGDSNodeBase * rootnode) { RootNode = rootnode; }

	//! @brief		現在のルートノードデータをフリーズし、フリーズされた読み出し
	//!				専用のノードデータを帰す
	//! @return		フリーズされたノードデータ
	//! @note		これで帰されたノードデータは他のフリーズされたノードデータ、あるいは
	//!				最新のルートノードデータからは特にロックをせずに安全に参照できる。
	tGDSNodeData * Freeze();
};
//---------------------------------------------------------------------------






class tGDSNodeData;
//---------------------------------------------------------------------------
//! @brief		ノード用の軽量プール(基底クラス)
//---------------------------------------------------------------------------
class tGDSPoolBase : public tCollectee
{
public:
	virtual tGDSNodeData * Allocate() = 0;
	virtual void Deallocate(tGDSNodeData * obj) = 0;

protected:
	void ThrowAllocationLogicError();
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		ノード用の軽量プール(実装)
//---------------------------------------------------------------------------
template <typename NodeT, typename NodeDataT>
class tGDSPool : public tGDSPoolBase
{
	struct alignment_check
	{
		char a;
		NodeDataT t;
	};

	char *Data;
	NodeT * Node; //!< ノードインスタンス
	risse_uint32 Using; //!< 使用中のビット = 1
	risse_uint32 Instantiated; //!< インスタンス化済みビット = 1

public:
	//! @brief		コンストラクタ
	//! @param		node	ノードインスタンス
	tGDSPool(NodeT * node)
	{
		Node = node; Using = 0; Instantiated = 0;
		risse_size align_size =
			reinterpret_cast<size_t>(&reinterpret_cast<const char &>(((alignment_check*)1) -> t)) - 1;
		int align = 0;
		while(align_size) align ++, align_size >>= 1;
		// アライメントされたメモリ領域に確保を行う
		Data = (char*)AlignedMallocCollectee(tGDSGraph::MaxGenerations * sizeof(NodeDataT), align);
	}

	//! @brief		確保を行う
	//! @return		確保されたオブジェクト
	virtual tGDSNodeData * Allocate()
	{
		for(int i = 0; i < tGDSGraph::MaxGenerations; i++)
		{
			risse_uint32 bit = 1 << i;
			if(!(bit & Using))
			{
				// 空きあり
				if(!(bit & Instantiated))
				{
					// インスタンス化されていない
					Instantiated |= bit;
					new (Data + i * sizeof(NodeDataT)) NodeDataT(Node);
				}
				Using |= bit;
				return reinterpret_cast<NodeDataT*>(Data + i * sizeof(NodeDataT));
			}
		}
		// 見つからなかった (ロジックがおかしいので致命的なエラー)
		ThrowAllocationLogicError();
		return NULL;
	}

	//! @brief		解放を行う
	//! @param		obj		解放を行いたいオブジェクト
	//! @note		obj.Free() が呼ばれる
	virtual void Deallocate(tGDSNodeData * obj)
	{
		reinterpret_cast<NodeDataT*>(obj)->Free();

		for(int i = 0; i < tGDSGraph::MaxGenerations; i++)
		{
			if(Data + i * sizeof(NodeDataT) == reinterpret_cast<char *>(obj))
			{
				// そこのフラグを倒す
				Using &= ~(1<<i);
			}
		}
	}

};
//---------------------------------------------------------------------------







class tGDSNodeBase;
//---------------------------------------------------------------------------
//! @brief		GDSノードのデータを表すクラス
//---------------------------------------------------------------------------
class tGDSNodeData : public tCollectee
{
	friend class tGDSNodeBase;

	tGDSNodeBase * Node; //!< ノードインスタンスへのポインタ
	typedef gc_vector<tGDSNodeData *> tNodeVector;
	tNodeVector Children; //!< 子ノード(その世代での情報を持つ)
	int RefCount; //!< 参照カウンタ
	tGDSGeneration LastGeneration; //!< 最後に更新された世代

public:
	//! @brief		コンストラクタ
	//! @param		node		ノードインスタンスへのポインタ
	tGDSNodeData(tGDSNodeBase * node);

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tGDSNodeData() {;}

	//! @brief		参照カウンタを減らす
	void AddRef() { RefCount ++; }

	//! @brief		参照カウンタを減らす
	void Release();

	//! @brief		N番目にある子ノードデータを得る
	//! @param		n		インデックス
	//! @return		その位置にある子ノードデータ
	//! @note		n の範囲チェックは行われないので注意
	tGDSNodeData * GetChildAt(risse_size n) const { return Children[n]; }

	//! @brief		子ノードデータの個数を得る
	//! @return		子ノードデータの個数
	risse_size GetChildCount() const { return Children.size(); }

private:
	//! @brief		N番目にある子ノードデータを設定する
	//! @param		cn			(このノードデータ内の)子のインデックス
	//! @param		nodedata	子として設定するノードデータ
	//! @param		pn			(子のノードデータ内の)親のインデックス
	void SetChildAt(risse_size cn, tGDSNodeData * nodedata, risse_size pn);

public:
	//! @brief		modify を行うためにノードデータのコピーを行う(内部関数)
	//! @param		oldchild		古い子ノードデータ
	//! @param		newchild		新しい子ノードデータ
	//! @return		コピーされたノードデータ
	tGDSNodeData * BeginIndepend(tGDSNodeData * oldchild, tGDSNodeData * newchild);

	//! @brief		modify を行うためにノードデータのコピーを行う(内部関数)
	//! @param		newnode			BeginIndepend() の戻りの値を渡す
	void EndIndepend(tGDSNodeData * newnodedata);

private:
	//! @brief		子を付け替える
	//! @param		oldnodedata		古い子ノードデータ
	//! @param		newnodedata		新しい子ノードデータ
	//! @note		子の中からoldnodedataを探し、newnodedataに付け替える
	void ReconnectChild(tGDSNodeData * oldnodedata, tGDSNodeData * newnodedata);

public:
	// graphviz 形式でのダンプを行う
	void DumpGraphviz() const;

public: // サブクラスで実装しても良い物
	//! @brief		コピーを行う
	//! @param		rhs		コピー元オブジェクト
	//! @note		もしサブクラスでオーバーライドしたならば、スーパークラスのこれを
	//!				呼ぶことを忘れないこと
	virtual void Copy(const tGDSNodeData * rhs);

	//! @brief		使用を終了する
	//! @note		もしサブクラスでオーバーライドしたならば、スーパークラスのこれを
	//!				呼ぶことを忘れないこと
	virtual void Free();

	//! @brief		名前を得る
	virtual tString GetName() const;

	//! @brief		(テキスト形式の)ダンプを得る
	virtual tString DumpText() const;
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		GDSノードの基底クラス
//---------------------------------------------------------------------------
class tGDSNodeBase : public tCollectee
{
	tGDSGraph * Graph; //!< グラフインスタンスへのポインタ
	tGDSNodeData * Current; //!< 最新のノードデータへのポインタ
	tGDSPoolBase * Pool; //!< プール
	typedef gc_vector<tGDSNodeBase *> tNodeVector;
	tNodeVector Parents; //!< 親ノード(常に最新の情報を持つ)

public:
	//! @brief		コンストラクタ
	//! @param		graph			グラフインスタンスへのポインタ
	//! @param		pool			プール
	tGDSNodeBase(tGDSGraph * graph, tGDSPoolBase * pool);

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tGDSNodeBase() {;}

	//! @brief		グラフインスタンスへのポインタを得る
	//! @return		グラフインスタンスへのポインタ
	tGDSGraph * GetGraph() const { return Graph; }

	//! @brief		最新のノードデータを得る
	//! @return		最新のノードデータ
	tGDSNodeData * GetCurrent() const { return Current; }

	//! @brief		最新のノードデータを設定する
	//! @param		nodedata		最新のノードデータ
	void SetCurrent(tGDSNodeData * nodedata);

	//! @brief		プールインスタンスを得る
	//! @return		プールインスタンス
	tGDSPoolBase * GetPool() const { return Pool; }

	//! @brief		N番目にある親ノードを得る
	//! @param		n		インデックス
	//! @return		その位置にある親ノード
	//! @note		n の範囲チェックは行われないので注意
	tGDSNodeBase * GetParentAt(risse_size n) const { return Parents[n]; }

	//! @brief		N番目にある親ノードを設定する
	//! @param		n		インデックス
	//! @param		node	ノード
	//! @note		n が範囲外だった場合は配列サイズが拡張される
	void SetParentAt(risse_size n, tGDSNodeBase * node);

	//! @brief		親ノードの個数を得る
	//! @return		親ノードの個数
	risse_size GetParentCount() const { return Parents.size(); }

	//! @brief		親を付け替える
	//! @param		oldnode		古い親ノード
	//! @param		newnode		新しい親ノード
	//! @note		親の中からoldnodeを探し、newnodeに付け替える
	void ReconnectParent(tGDSNodeBase * oldnode, tGDSNodeBase * newnode);

	//! @brief		N番目にある子ノードを設定する
	//! @param		cn			(このノードデータ内の)子のインデックス
	//! @param		node		子として設定するノード
	//! @param		pn			(子のノードデータ内の)親のインデックス
	void SetChildAt(risse_size cn, tGDSNodeBase * node, risse_size pn)
	{
		Current->SetChildAt(cn, node->Current, pn);
	}

public: // サブクラスでオーバーライドして良い物

	//! @brief		名前を得る
	virtual tString GetName() const;

	//! @brief		(テキスト形式の)ダンプを得る
	virtual tString DumpText() const;

};
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//! @brief		GDSノードの基底クラス (実際のサブクラスにはこっちを継承させる)
//---------------------------------------------------------------------------
template <typename NodeDataT> // NodeDataT = ノードデータの型
class tGDSNode : public tGDSNodeBase
{
	typedef tGDSNodeBase inherited;

public:
	//! @brief		更新ロック
	//! @note		このインスタンスの生存期間中に、GetNewNode() で取得できた
	//!				ノードのデータを弄ること。
	//!				スタック上に配置して使うこと！
	class tUpdateLock
	{
		NodeDataT * NodeData; //!< 元のノードデータ
		NodeDataT * NewNodeData; //!< 新しくクローンされたノードデータ
		volatile tCriticalSection::tLocker Lock;

	public:
		//! @brief		コンストラクタ
		//! @param		node		ロックするノード
		tUpdateLock(tGDSNode<NodeDataT> * node) :
			NodeData(node->GetCurrent()),
			NewNodeData(NULL),
			Lock(node->GetGraph()->GetCS())
		{
			NewNodeData = reinterpret_cast<NodeDataT*>(NodeData->BeginIndepend(NULL, NULL));
		}

		//! @brief		デストラクタ
		~tUpdateLock()
		{
			NodeData->EndIndepend(NewNodeData);
		}

	public:
		//! @brief		弄るべきノードデータを帰す
		//! @return		弄るべきノードデータ
		NodeDataT * GetNewNodeData() const { return NewNodeData; }
	};


public:
	//! @brief		コンストラクタ
	//! @param		graph			グラフインスタンスへのポインタ
	tGDSNode(tGDSGraph * graph) : tGDSNodeBase(graph, new tGDSPool<tGDSNode<NodeDataT>, NodeDataT>(this)) {;}

	//! @brief		最新のノードデータを得る
	//! @return		最新のノードデータ
	NodeDataT * GetCurrent() const { return reinterpret_cast<NodeDataT*>(inherited::GetCurrent()); }

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Rina
