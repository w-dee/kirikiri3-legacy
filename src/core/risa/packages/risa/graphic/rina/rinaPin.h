//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA ピン管理
//---------------------------------------------------------------------------
#ifndef RINAPIN_H
#define RINAPIN_H

#include "risa/packages/risa/graphic/rina/rinaNode.h"
#include "risa/packages/risa/graphic/rina/rinaGraph.h"
#include "risseArrayClass.h"

namespace Risa {
//---------------------------------------------------------------------------


class tNodeInstance;
//---------------------------------------------------------------------------
/**
 * ピンインスタンス
 */
class tPinInstance : public tObjectBase
{
public:
	typedef tObjectBase inherited;

protected:
	/**
	 * グラフをロックするためのクラス
	 */
	class tGraphLocker : public tObjectInterface::tSynchronizer
	{
	public:
		tGraphLocker(const tPinInstance * _this) :
			tObjectInterface::tSynchronizer(
					_this->GetNodeInstance()->GetGraphInstance()) {;}
	};

protected:
	tNodeInstance * NodeInstance; //!< このピンを保有しているノード

public:
	/**
	 * コンストラクタ
	 */
	tPinInstance();

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tPinInstance() {;}

	/**
	 * プロセスノードにこのピンをアタッチする
	 * @param node	プロセスノード (NULL=デタッチ)
	 */
	void Attach(tNodeInstance * node) { NodeInstance = node; }

	/**
	 * Attachと同じ
	 * @param node	プロセスノード (NULL=デタッチ)
	 */
	void SetNodeInstance(tNodeInstance * node) { Attach(node); }

	/**
	 * このピンを保有しているノードを得る
	 * @return	このピンを保有しているノード
	 * @note	DEBUGビルドではノードがアタッチされてないときに
	 * 			呼ぶとAssertion failureとなる
	 */
	tNodeInstance * GetNodeInstance() const { RISSE_ASSERT(NodeInstance); return NodeInstance; }

	/**
	 * このピンがサポートするタイプの一覧を得る
	 * @return	このピンがサポートするタイプの一覧
	 * @note	返される配列は、最初の物ほど優先度が高い
	 */
	virtual const gc_vector<risse_uint32> & GetSupportedTypes() = 0;

	/**
	 * タイプの提案を行う
	 * @param pin				接続先のピン
	 * @param strong_suggest	この提案が強い提案であれば *strong_suggest に真が入る(NULL=イラナイ)
	 * @return	提案されたタイプ (0=提案なし)
	 */
	virtual risse_uint32 SuggestType(tPinInstance * pin, bool * strong_suggest = NULL);

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ピンクラス
 */
RISSE_DEFINE_CLASS_BEGIN(tPinClass, tClassBase, tPinInstance, itNoInstance)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------




class tOutputPinInstance;
class tRenderRequest;
//---------------------------------------------------------------------------
/**
 * 入力ピンインスタンス
 * @note	一つの入力ピンは複数の出力ピンとは接続されない。このため
 *			入力ピンはほぼ「エッジ」と同じと見なすことができる。このため
 *			入力ピンごとに「エッジを流通するデータのタイプ」としての
 *			tInputPinInstance::AgreedTypeを持っている。
 */
class tInputPinInstance : public tPinInstance
{
public:
	typedef tPinInstance inherited;

	typedef tInputPinInstance tPinDirectionType;
		//!< テンプレート中で派生クラスが tInputPinInstance の派生なのか tOutputPin の派生なのかを調べるために使われる

private:
	tOutputPinInstance * OutputPinInstance; //!< この入力ピンにつながっている出力ピン
	risse_uint32 AgreedType; //!< 同意されたタイプ

	tIdRegistry::tRenderGeneration RenderGeneration; //!< 最新の情報が設定されたレンダリング世代

public:
	/**
	 * 親ノードから子ノードへのレンダリング要求の配列のtypedef
	 */
	typedef gc_vector<const tRenderRequest*> tRenderRequests;

private:
	tRenderRequests RenderRequests; //!< 親ノードから子ノードへのレンダリング要求の配列

public:
	/**
	 * コンストラクタ
	 */
	tInputPinInstance();

public:
	/**
	 * 接続先の出力ピンを取得する
	 * @return	接続先の出力ピン
	 */
	tOutputPinInstance * GetOutputPinInstance() const { return OutputPinInstance; }

	/**
	 * 最新の情報が設定されたレンダリング世代を設定する
	 * @param gen	最新の情報が設定されたレンダリング世代
	 */
	void SetRenderGeneration(tIdRegistry::tRenderGeneration gen) { RenderGeneration = gen; }

	/**
	 * 最新の情報が設定されたレンダリング世代を得る
	 * @return	最新の情報が設定されたレンダリング世代
	 * @note	GetParentQueueNode() などを使う場合は、このレンダリング世代を必ず
	 *			自分のレンダリング世代と比較し、古い世代を間違って使ってしまっていないかを
	 *			チェックすること
	 */
	tIdRegistry::tRenderGeneration GetRenderGeneration() const { return RenderGeneration; }

	/**
	 * ネゴシエーションを行う
	 * @param output_pin	接続先の出力ピン
	 * @return	同意のとれたタイプ (0=同意無し)
	 * @note	出力ピンとの間でネゴシエーションを行い、同意のとれたタイプを返す。
	 *			実際に接続したりはしない。
	 * @note	出力ピン側のこのメソッドはprotectedになっていて外部からアクセスできない。
	 *			入力ピンと出力ピンに対して行う SuggestType() のうち、
	 *			片方が strong_suggest を真にした場合は
	 *			そちらの結果が優先される。両方とも strong_suggest を真にした場合や
	 *			両方とも偽の場合は入力ピンの結果が優先される。
	 *			どちらかが 0 (提案なし) を返した場合は 0 が帰る。
	 */
	virtual risse_uint32 Negotiate(tOutputPinInstance * output_pin);

	/**
	 * 出力ピンを接続する
	 * @param output_pin	出力ピン(NULL=接続解除)
	 * @note	サブクラスでオーバーライドしたときは最後に親クラスのこれを呼ぶこと。
	 * @note	ネゴシエーションに失敗した場合は例外が発生する。
	 * @note	ピンは入力ピンがかならず何かの出力ピンを接続するという方式なので
	 *			出力ピン側のこのメソッドはprotectedになっていて外部からアクセスできない。
	 */
	virtual void InternalConnect(tOutputPinInstance * output_pin);

	/**
	 * 親ノードから子ノードへのレンダリング要求の配列を得る
	 * return		親ノードから子ノードへのレンダリング要求の配列
	 */
	const tRenderRequests & GetRenderRequests() const { return RenderRequests; }

	/**
	 * 親ノードから子ノードへのレンダリング要求の配列をクリアする
	 */
	void ClearRenderRequests() { RenderRequests.clear(); }

	/**
	 * 親ノードから子ノードへのレンダリング要求の配列にアイテムを追加する
	 * @param req	要求データ
	 */
	void AddRenderRequest(const tRenderRequest * req) { RenderRequests.push_back(req); }

protected:
	/**
	 * 同意されたタイプを設定する
	 * @param type	同意されたタイプ
	 */
	void SetAgreedType(risse_uint32 type) { AgreedType = type; }

public: // 公開インターフェース
	/**
	 * 出力ピンを接続する
	 * @param output_pin	出力ピン(NULL=接続解除)
	 */
	void Connect(tOutputPinInstance * output_pin);

	/**
	 * 同意されたタイプを得る
	 * @return	同意されたタイプ
	 */
	risse_uint32 GetAgreedType() const;

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
	void connect(const tVariant & output_pin);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 入力ピンクラス
 */
RISSE_DEFINE_CLASS_BEGIN(tInputPinClass, tClassBase, tInputPinInstance, itNoInstance)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
/**
 * 出力ピンインスタンス
 */
class tOutputPinInstance : public tPinInstance
{
public:
	typedef tPinInstance inherited;

	typedef tOutputPinInstance tPinInstanceDirectionType;
		//!< テンプレート中で派生クラスが tInputPinInstance の派生なのか tOutputPinInstance の派生なのかを調べるために使われる

private:
	friend class tInputPinInstance;

	tArrayInstance * InputPins;

public:
	/**
	 * コンストラクタ
	 */
	tOutputPinInstance();

	/**
	 * 接続先の入力ピンの配列を取得する
	 * @return	接続先の入力ピンの配列
	 */
	tArrayInstance * GetInputPins() const { return InputPins; }

public:
	/**
	 * このピンの先に繋がってる入力ピンに繋がってるノードのルートからの最長距離を求める
	 * @return	ルートからの最長距離
	 */
	risse_size GetLongestDistance() const;

	/**
	 * 入力ピンを接続する(tInputPinInstance::Connectから呼ばれる)
	 * @param input_pin	入力ピンインスタンス
	 * @note	サブクラスでオーバーライドしたときは最後に親クラスのこれを呼ぶこと。
	 */
	virtual void Connect(tInputPinInstance * input_pin);

	/**
	 * 入力ピンの接続を解除する(tInputPinInstance::Connectから呼ばれる)
	 * @param input_pin	入力ピンインスタンス
	 * @note	サブクラスでオーバーライドしたときは最後に親クラスのこれを呼ぶこと。
	 */
	virtual void Disconnect(tInputPinInstance * input_pin);

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * 出力ピンクラス
 */
RISSE_DEFINE_CLASS_BEGIN(tOutputPinClass, tClassBase, tOutputPinInstance, itNoInstance)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------










class tNodeInstance;
//---------------------------------------------------------------------------
/**
 * ピン配列
 * @note	ピン操作の各メソッドはグラフをロックしない。
 *			ピンを操作する前にはグラフをロックすること。また、グラフがロック
 *			されていることを確実にするために、各メソッドにはグラフがロックされている
 *			かどうかの ASSERT を挿入することを強く推奨する。
 */
class tPinArrayInstance: public tObjectBase
{
private:
	tNodeInstance * NodeInstance; //!< このピン配列を保持しているノードインスタンス

protected:
	/**
	 * ノードインスタンスを設定する
	 * @param node	ノードインスタンス
	 */
	void SetNodeInstance(tNodeInstance * node) { NodeInstance = node; }

public:
	/**
	 * コンストラクタ
	 */
	tPinArrayInstance() { NodeInstance = NULL; }

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tPinArrayInstance() {}

	/**
	 * このピン配列を保持しているノードインスタンスを得る
	 * @return	このピン配列を保持しているノードインスタンス
	 */
	tNodeInstance * GetNodeInstance() const { RISSE_ASSERT(NodeInstance); return NodeInstance; }

	/**
	 * ピンの数を得る
	 * @return	ピンの総数
	 */
	virtual risse_size GetCount()
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		return 0;
	}

	/**
	 * 指定インデックスのピンを得る
	 * @param index	インデックス
	 */
	virtual tPinInstance * Get(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		/* TODO: 例外 */
		return NULL;
	}

	/**
	 * 指定インデックスのピンを削除する
	 * @param index	インデックス
	 */
	virtual void Delete(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		/* TODO: 例外 */
	}

	/**
	 * 指定インデックスにピンを挿入する
	 * @param index	インデックス
	 */
	virtual void Insert(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		/* TODO: 例外 */
	}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tVariant & node, const tNativeCallInfo &info);
	tVariant iget(risse_offset ofs_index);
	size_t get_length();
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * ピン配列クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tPinArrayClass, tClassBase, tPinArrayInstance, itNoInstance)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * 入力ピンのピン配列
 * @note	このクラスではすべてのメソッドが実装されているが、
 *			すべてあたかもピンが無いかのように振る舞う。
 *			そのため、そのクラス名に反して、入力ピンを持たないノードの入力ピン配列に使うことができる。
 *			また、すべての InputPinArray の基底クラスである。
 */
class tInputPinArrayInstance : public tPinArrayInstance
{
public:
	typedef tPinArrayInstance inherited;

public:
	typedef tInputPinInstance tPinType;

	/**
	 * コンストラクタ
	 */
	tInputPinArrayInstance() {;}

	/**
	 * 指定インデックスのピンを得る
	 * @param index	インデックス
	 * @note	Get() と異なり、戻りの型は tInputPin * になる (単にGet()の戻りをキャストしてるだけ)
	 */
	tInputPinInstance * At(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		return static_cast<tInputPinInstance*>(Get(index));
	}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tVariant & node, const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * 入力ピンのピン配列クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tInputPinArrayClass, tClassBase, tInputPinArrayInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
/**
 * 入力ピンのピン配列(一個だけの要素の場合)
 */
class tOneInputPinArrayInstance : public tInputPinArrayInstance
{
public:
	typedef tInputPinArrayInstance inherited;

private:
	tInputPinInstance * PinInstance; //!< ピンインスタンス

public:
	/**
	 * コンストラクタ
	 */
	tOneInputPinArrayInstance() {PinInstance=NULL;}

	/**
	 * ピンの数を得る
	 * @return	ピンの総数
	 */
	virtual risse_size GetCount()
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		return 1;
	}

	/**
	 * 指定インデックスのピンを得る
	 * @param index	インデックス
	 */
	virtual tPinInstance * Get(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		/* TODO: 例外 */
		if(index ==0) return PinInstance;
		return NULL;
	}

	/**
	 * 指定インデックスのピンを削除する
	 * @param index	インデックス
	 */
	virtual void Delete(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		/* TODO: 例外 */
	}

	/**
	 * 指定インデックスにピンを挿入する
	 * @param index	インデックス
	 */
	virtual void Insert(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		/* TODO: 例外 */
	}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tVariant & node, const tVariant & pin, const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * 入力ピンのピン配列クラス(一個だけの要素の場合)
 */
RISSE_DEFINE_CLASS_BEGIN(tOneInputPinArrayClass, tClassBase, tOneInputPinArrayInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
/**
 * 出力ピンのピン配列
 * @note	このクラスではすべてのメソッドが実装されているが、
 *			すべてあたかもピンが無いかのように振る舞う。
 *			そのため、そのクラス名に反して、出力ピンを持たないノードの出力ピン配列に使うことができる。
 *			また、すべての OutputPinArray の基底クラスである。
 */
class tOutputPinArrayInstance : public tPinArrayInstance
{
public:
	typedef tPinArrayInstance inherited;

public:
	typedef tOutputPinInstance tPinType;

	/**
	 * コンストラクタ
	 */
	tOutputPinArrayInstance() {;}

	/**
	 * 指定インデックスのピンを得る
	 * @param index	インデックス
	 * @note	Get() と異なり、戻りの型は tOutputPin * になる (単にGet()の戻りをキャストしてるだけ)
	 */
	tOutputPinInstance * At(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		return static_cast<tOutputPinInstance*>(Get(index));
	}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tVariant & node, const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * 出力ピンのピン配列クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tOutputPinArrayClass, tClassBase, tOutputPinArrayInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
/**
 * 出力ピンのピン配列(一個だけの要素の場合)
 */
class tOneOutputPinArrayInstance : public tOutputPinArrayInstance
{
public:
	typedef tOutputPinArrayInstance inherited;

private:
	tOutputPinInstance * PinInstance; //!< ピンインスタンス

public:
	/**
	 * コンストラクタ
	 */
	tOneOutputPinArrayInstance() {PinInstance = NULL;}

	/**
	 * ピンの数を得る
	 * @return	ピンの総数
	 */
	virtual risse_size GetCount()
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		return 1;
	}

	/**
	 * 指定インデックスのピンを得る
	 * @param index	インデックス
	 */
	virtual tPinInstance * Get(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		/* TODO: 例外 */
		if(index ==0) return PinInstance;
		return NULL;
	}

	/**
	 * 指定インデックスのピンを削除する
	 * @param index	インデックス
	 */
	virtual void Delete(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		/* TODO: 例外 */
	}

	/**
	 * 指定インデックスにピンを挿入する
	 * @param index	インデックス
	 */
	virtual void Insert(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());
		/* TODO: 例外 */
	}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tVariant & node, const tVariant & pin, const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * 出力ピンのピン配列クラス(一個だけの要素の場合)
 */
RISSE_DEFINE_CLASS_BEGIN(tOneOutputPinArrayClass, tClassBase, tOneOutputPinArrayInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
/**
 * 与えられた型がtInputPinかtOutputPinかでtypeの型を変えるテンプレート型
 */
template <typename QUESTION>
struct tInputPinArrayOrOutputPinArraySelector
{
};
template <>
struct tInputPinArrayOrOutputPinArraySelector<tInputPinInstance>
{
	typedef tInputPinArrayInstance type;
};
template <>
struct tInputPinArrayOrOutputPinArraySelector<tOutputPinInstance>
{
	typedef tOutputPinArrayInstance type;
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}

#endif
