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



class tGDSNode;
//---------------------------------------------------------------------------
//! @brief		GDSグラフ
//---------------------------------------------------------------------------
class tGDSGraph : public tCollectee
{
	tGDSGeneration		LastFreezedGeneration; //!< 一番最後にフリーズを行った世代
	tCriticalSection * CS; //!< このグラフを保護するクリティカルセクション
	tGDSNode * Root; //!< (現在の最新の)ルートノード

public:
	//! @brief		コンストラクタ
	tGDSGraph();

	//! @brief		一番最後にフリーズを行った世代を返す
	const tGDSGeneration & GetLastFreezedGeneration() const
		{ return LastFreezedGeneration; }

	//! @brief		このグラフを保護するクリティカルセクションを得る
	tCriticalSection & GetCS() { return *CS; }

	//! @brief		(最新の)ルートノードを得る
	//! @note		これで帰されたノードは他のフリーズされたノード
	//!				のことを気にせずに変更などをくわえることができる。
	tGDSNode * GetRoot();

	//! @brief		ルートノードを設定する(ロックはしない)
	//! @param		root		新しいルートノード
	void SetRootNoLock(tGDSNode * root) { Root = root; }

	//! @brief		現在のルートノードをフリーズし、フリーズされた読み出し
	//!				専用のノードを帰す
	//! @return		フリーズされたノード
	//! @note		これで帰されたノードは他のフリーズされたノード、あるいは
	//!				最新のルートノードからは特にロックをせずに安全に参照できる。
	tGDSNode * Freeze();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		GDSノード
//---------------------------------------------------------------------------
class tGDSNode : public tCollectee
{
	typedef gc_vector<tGDSNode *> tNodeVector;
	tNodeVector Parents; //!< 親ノード
	tNodeVector Children; //!< 子ノード

	tGDSGraph * Graph; //!< グラフインスタンスへのポインタ

	tGDSGeneration LastGeneration; //!< 最後に更新が行われた世代

public:
	//! @brief		更新ロック
	//! @note		このインスタンスの生存期間中に、GetNewNode() で取得できた
	//!				ノードのデータを弄ること。
	//!				スタック上に配置して使うこと！
	class tUpdateLock
	{
		tGDSNode * Node; //!< ノード
		tGDSNode * NewNode; //!< 新しくクローンされたノード
		volatile tCriticalSection::tLocker Lock;

	public:
		//! @brief		コンストラクタ
		//! @param		node		ロックするノード
		tUpdateLock(tGDSNode * node);

		//! @brief		デストラクタ
		~tUpdateLock();

	public:
		//! @brief		弄るべきノードを帰す
		//! @return		弄るべきノード
		tGDSNode * GetNewNode() const { return NewNode; }
	};

public:
	//! @brief			コンストラクタ
	//! @param			graph			グラフインスタンスへのポインタ
	tGDSNode(tGDSGraph * graph);

	//! @brief			N番目にある親ノードを得る
	//! @param			n		インデックス
	//! @return			その位置にある親ノード
	//! @note			n の範囲チェックは行われないので注意
	tGDSNode * GetParentAt(risse_size n) const { return Parents[i]; }

	//! @brief			親ノードの個数を得る
	//! @return			親ノードの個数
	risse_size GetParentCount() const { return Parents.size(); }

	//! @brief			N番目にある子ノードを得る
	//! @param			n		インデックス
	//! @return			その位置にある子ノード
	//! @note			n の範囲チェックは行われないので注意
	tGDSNode * GetChildAt(risse_size n) const { return Children[i]; }

	//! @brief			子ノードの個数を得る
	//! @return			子ノードの個数
	risse_size GetChildCount() const { return Children.size(); }

private:
	//! @brief			modify を行うためにオブジェクトのコピーを行う(内部関数)
	//! @param			oldchild		古い子ノード
	//! @param			newchild		新しい子ノード
	//! @return			コピーされたオブジェクト
	tGDSNode * BeginIndepend(tGDSNode * oldchild, tGDSNode * oldchild);

	//! @brief			modify を行うためにオブジェクトのコピーを行う(内部関数)
	//! @param			newnode			BeginIndepend() の戻りの値を渡す
	void EndIndepend(tGDSNode * newnode);

private:
	//! @brief			子を付け替える
	//! @param			oldnode		古い子ノード
	//! @param			newnode		新しい子ノード
	//! @note			子の中からoldnodeを探し、newnodeに付け替える
	void ReconnectChild(tGDSNode * oldnode, tGDSNode * newnode);

protected: // サブクラスで実装してもよいもの
	//! @brief			内容のコピーを行うべきタイミングで発生される
	//! @note			コピーされた内容
	virtual tGDSNode * Clone() const;

};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Rina
