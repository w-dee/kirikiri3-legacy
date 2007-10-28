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
//---------------------------------------------------------------------------
//! @brief		GDSグラフ
//---------------------------------------------------------------------------
class tGDSGraph : public tCollectee
{
	tGDSGeneration		LastFreezedGeneration; //!< 一番最後にフリーズを行った世代
	tCriticalSection * CS; //!< このグラフを保護するクリティカルセクション
	tGDSNode * RootNode; //!< (現在の最新の)ルートノード

	const int MaxGenerations = 3; //!< 最大で何世代までの管理を許すか(「最新の」世代も含める)
								//!< 3 = トリプルバッファリングを考慮した際の最大値

public:
	//! @brief		コンストラクタ
	tGDSGraph();

	//! @brief		一番最後にフリーズを行った世代を返す
	const tGDSGeneration & GetLastFreezedGeneration() const
		{ return LastFreezedGeneration; }

	//! @brief		このグラフを保護するクリティカルセクションを得る
	tCriticalSection & GetCS() { return *CS; }

	//! @brief		(最新の)ルートノードデータを得る
	//! @note		これで帰されたノードデータのCurrentは他のフリーズされたノード
	//!				のことを気にせずに変更などをくわえることができるはず。
	tGDSNode * GetRoot();

	//! @brief		ルートノードを設定する(ロックはしない)
	//! @param		root		新しいルートノード
	void SetRootNoLock(tGDSNodeBase * root) { Root = root; }

	//! @brief		現在のルートノードをフリーズし、フリーズされた読み出し
	//!				専用のノードを帰す
	//! @return		フリーズされたノード
	//! @note		これで帰されたノードは他のフリーズされたノード、あるいは
	//!				最新のルートノードからは特にロックをせずに安全に参照できる。
	tGDSNode * Freeze();
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		ノード用の軽量プール(基底クラス)
//---------------------------------------------------------------------------
class tGDSPoolBase : public tCollectee
{
public:
	virtual T & Allocate() = 0;
	virtual void Deallocate(T & obj) = 0;
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		ノード用の軽量プール(実装)
//---------------------------------------------------------------------------
template <typename T>
class tGDSPool : public tGDSPoolBase
{
	//! @brief		パディングも含めたTのサイズを特定するための簡単なクラス
	struct tSizeDetector
	{
		T t1
		T t2;
	};
	enum {
		T_size = reinterpret_cast<int>(&reinterpret_cast<char &>(((T*)0)->t2));
	};

	char Data[tGDSGraph::MaxGenerations][T_size]; //!< データ
	tGDSGraph * Graph; //!< グラフインスタンス
	risse_uint32 Using; //!< 使用中のビット = 1
	risse_uint32 Instantiated; //!< インスタンス化済みビット = 1

public:
	//! @brief		コンストラクタ
	//! @param		graph			グラフインスタンス
	tGDSPool(tGDSGraph * graph) { Graph = graph; Using = 0; Instantiated = 0; }

	//! @brief		確保を行う
	//! @return		確保されたオブジェクト
	virtual T * Allocate()
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
					new (Data[i]) T(Graph);
				}
				Using |= bit;
				return reinterpret_cast<T*>(Data[i]);
			}
		}
		// 見つからなかった (ロジックがおかしいので致命的なエラー)
		return reinterpret_cast<T*>(NULL);
	}

	//! @brief		解放を行う
	//! @param		obj		解放を行いたいオブジェクト
	//! @note		obj.Free() が呼ばれる
	virtual void Deallocate(T * obj)
	{
		obj->Free();

		for(int i = 0; i < tGDSGraph::MaxGenerations; i++)
		{
			if(Data[i] == obj)
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
	tGDSNodeBase * Node; //!< ノードインスタンスへのポインタ
	typedef gc_vector<tGDSNodeData *> tNodeVector;
	tNodeVector Parents; //!< 親ノード
	tNodeVector Children; //!< 子ノード
	int RefCount; //!< 参照カウンタ

public:
	//! @brief		コンストラクタ
	//! @param		node		ノードインスタンスへのポインタ
	tGDSNodeData(tGDSNodeBase * node) { Node = node; RefCount = 1; }

	//! @brief		参照カウンタを減らす
	void AddRef() { RefCount ++; }

	//! @brief		参照カウンタを減らす
	void Release();

	//! @brief		N番目にある親ノードを得る
	//! @param		n		インデックス
	//! @return		その位置にある親ノード
	//! @note		n の範囲チェックは行われないので注意
	tGDSNodeBase * GetParentAt(risse_size n) const { return Parents[i]; }

	//! @brief		親ノードの個数を得る
	//! @return		親ノードの個数
	risse_size GetParentCount() const { return Parents.size(); }

	//! @brief		N番目にある子ノードを得る
	//! @param		n		インデックス
	//! @return		その位置にある子ノード
	//! @note		n の範囲チェックは行われないので注意
	tGDSNodeBase * GetChildAt(risse_size n) const { return Children[i]; }

	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	risse_size GetChildCount() const { return Children.size(); }

private:
	//! @brief		子を付け替える
	//! @param		oldnode		古い子ノード
	//! @param		newnode		新しい子ノード
	//! @note		子の中からoldnodeを探し、newnodeに付け替える
	void ReconnectChild(tGDSNodeData * oldnode, tGDSNodeData * newnode);

protected: // サブクラスで実装してほしいもの
	//! @brief		コピーを行う
	//! @param		rhs		コピー元オブジェクト
	//! @note		もしサブクラスでオーバーライドしたならば、スーパークラスのこれを
	//!				呼ぶことを忘れないこと
	virtual void Copy(const tGDSNodeData * rhs);

	//! @brief		使用を終了する
	//! @note		もしサブクラスでオーバーライドしたならば、スーパークラスのこれを
	//!				呼ぶことを忘れないこと
	virtual void Free() { RefCount = 1; }
};
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//! @brief		GDSノードの基底クラス
//---------------------------------------------------------------------------
class tGDSNodeBase : public tCollectee
{
	tGDSNodeData * Current; //!< 最新のノードデータへのポインタ
	tGDSPoolBase * Pool; //!< プール

public:
	//! @brief		更新ロック
	//! @note		このインスタンスの生存期間中に、GetNewNode() で取得できた
	//!				ノードのデータを弄ること。
	//!				スタック上に配置して使うこと！
	class tUpdateLock
	{
		tGDSNodeData * NewNodeData; //!< 新しくクローンされたノードデータ
		volatile tCriticalSection::tLocker Lock;

	public:
		//! @brief		コンストラクタ
		//! @param		node		ロックするノード
		tUpdateLock(tGDSNodeBase * node);

		//! @brief		デストラクタ
		~tUpdateLock();

	public:
		//! @brief		弄るべきノードデータを帰す
		//! @return		弄るべきノードデータ
		tGDSNodeData * GetNewNodeData() const { return NewNodeData; }
	};

public:
	//! @brief		コンストラクタ
	//! @param		graph			グラフインスタンスへのポインタ
	//! @param		pool			プール
	tGDSNodeBase(tGDSGraph * graph, tGDSPoolBase * pool);

	//! @brief		プールインスタンスを得る
	//! @return		プールインスタンス
	tGFSPoolBase * GetPool() const { return Pool; }

private:
	//! @brief			modify を行うためにノードデータのコピーを行う(内部関数)
	//! @param			oldchild		古い子ノードデータ
	//! @param			newchild		新しい子ノードデータ
	//! @return			コピーされたノードデータ
	tGDSNodeData * BeginIndepend(tGDSNodeData * oldchild, tGDSNodeData * newchild);

	//! @brief			modify を行うためにノードデータのコピーを行う(内部関数)
	//! @param			newnode			BeginIndepend() の戻りの値を渡す
	void EndIndepend(tGDSNodeData * newnodedata);
};
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//! @brief		GDSノードの基底クラス (実際のサブクラスにはこっちを継承させる)
//---------------------------------------------------------------------------
template <typename NodeDataT> // NodeDataT = ノードデータの型
class tGDSNode : public tGDSNodeBase
{
public:
	//! @brief			コンストラクタ
	//! @param			graph			グラフインスタンスへのポインタ
	tGDSNode(tGDSGraph * graph) : tGDSNodeBase(graph, new tGDSPool<NodeDataT>) {;}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Rina
