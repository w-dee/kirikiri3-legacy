//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA ピン管理
//---------------------------------------------------------------------------
#ifndef RINAPIN_H
#define RINAPIN_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaGraph.h"

namespace Rina {
//---------------------------------------------------------------------------


class tProcessNode;
//---------------------------------------------------------------------------
//! @brief		ピン
//---------------------------------------------------------------------------
class tPin : public Risa::tPolymorphic
{
public:
	typedef Risa::tPolymorphic inherited;

protected:
	//! @brief		グラフをロックするためのクラス
	class tGraphLocker : public Risa::tCriticalSection::tLocker
	{
	public:
		tGraphLocker(const tPin & _this) :
			tCriticalSection::tLocker(_this.GetNode()->GetGraph()->GetCS()) {;}
	};

protected:
	tProcessNode * Node; //!< このピンを保有しているノード

public:
	//! @brief		コンストラクタ
	tPin();

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tPin() {;}

	//! @brief		プロセスノードにこのピンをアタッチする
	//! @param		node		プロセスノード (NULL=デタッチ)
	void Attach(tProcessNode * node) { Node = node; }

	//! @brief		Attachと同じ
	//! @param		node		プロセスノード (NULL=デタッチ)
	void SetNode(tProcessNode * node) { Attach(node); }

	//! @brief		このピンを保有しているノードを得る
	//! @return		このピンを保有しているノード
	tProcessNode * GetNode() const { return Node; }

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes() = 0;

	//! @brief		タイプの提案を行う
	//! @param		pin		接続先のピン
	//! @param		strong_suggest	この提案が強い提案であれば *strong_suggest に真が入る(NULL=イラナイ)
	//! @return		提案されたタイプ (0=提案なし)
	virtual risse_uint32 SuggestType(tPin * pin, bool * strong_suggest = NULL);
};
//---------------------------------------------------------------------------



class tOutputPin;
class tRenderRequest;
//---------------------------------------------------------------------------
//! @brief		入力ピン
//! @note		一つの入力ピンは複数の出力ピンとは接続されない。このため
//!				入力ピンはほぼ「エッジ」と同じと見なすことができる。このため
//!				入力ピンごとに「エッジを流通するデータのタイプ」としての
//!				tInputPin::AgreedTypeを持っている。
//---------------------------------------------------------------------------
class tInputPin : public tPin, public Risa::tSubmorph<tInputPin>
{
public:
	typedef tPin inherited;

	typedef tInputPin tPinDirectionType; //!< テンプレート中で派生クラスが tInputPin の派生なのか tOutputPin の派生なのかを調べるために使われる

private:
	tOutputPin * OutputPin; //!< この入力ピンにつながっている出力ピン
	risse_uint32 AgreedType; //!< 同意されたタイプ

	tIdRegistry::tRenderGeneration RenderGeneration; //!< 最新の情報が設定されたレンダリング世代

public:
	//! @brief		親ノードから子ノードへのレンダリング要求の配列のtypedef
	typedef gc_vector<const tRenderRequest*> tRenderRequests;

private:
	tRenderRequests RenderRequests; //!< 親ノードから子ノードへのレンダリング要求の配列

public:
	//! @brief		コンストラクタ
	tInputPin();

public:
	//! @brief		接続先の出力ピンを取得する
	//! @return		接続先の出力ピン
	tOutputPin * GetOutputPin() const { return OutputPin; }

	//! @brief		最新の情報が設定されたレンダリング世代を設定する
	//! @param		gen		最新の情報が設定されたレンダリング世代
	void SetRenderGeneration(tIdRegistry::tRenderGeneration gen) { RenderGeneration = gen; }

	//! @brief		最新の情報が設定されたレンダリング世代を得る
	//! @return		最新の情報が設定されたレンダリング世代
	//! @note		GetParentQueueNode() などを使う場合は、このレンダリング世代を必ず
	//!				自分のレンダリング世代と比較し、古い世代を間違って使ってしまっていないかを
	//!				チェックすること
	tIdRegistry::tRenderGeneration GetRenderGeneration() const { return RenderGeneration; }

	//! @brief		ネゴシエーションを行う
	//! @param		output_pin		接続先の出力ピン
	//! @return		同意のとれたタイプ (0=同意無し)
	//! @note		出力ピンとの間でネゴシエーションを行い、同意のとれたタイプを返す。
	//!				実際に接続したりはしない。
	//! @note		出力ピン側のこのメソッドはprotectedになっていて外部からアクセスできない。
	//!				入力ピンと出力ピンに対して行う SuggestType() のうち、
	//!				片方が strong_suggest を真にした場合は
	//!				そちらの結果が優先される。両方とも strong_suggest を真にした場合や
	//!				両方とも偽の場合は入力ピンの結果が優先される。
	//!				どちらかが 0 (提案なし) を返した場合は 0 が帰る。
	virtual risse_uint32 Negotiate(tOutputPin * output_pin);

	//! @brief		出力ピンを接続する
	//! @param		output_pin		出力ピン(NULL=接続解除)
	//! @note		サブクラスでオーバーライドしたときは最後に親クラスのこれを呼ぶこと。
	//! @note		ネゴシエーションに失敗した場合は例外が発生する。
	//! @note		ピンは入力ピンがかならず何かの出力ピンを接続するという方式なので
	//!				出力ピン側のこのメソッドはprotectedになっていて外部からアクセスできない。
	virtual void InternalConnect(tOutputPin * output_pin);

	//! @brief		親ノードから子ノードへのレンダリング要求の配列を得る
	//! return		親ノードから子ノードへのレンダリング要求の配列
	const tRenderRequests & GetRenderRequests() const { return RenderRequests; }

	//! @brief		親ノードから子ノードへのレンダリング要求の配列をクリアする
	void ClearRenderRequests() { RenderRequests.clear(); }

	//! @brief		親ノードから子ノードへのレンダリング要求の配列にアイテムを追加する
	//! @param		req			要求データ
	void AddRenderRequest(const tRenderRequest * req) { RenderRequests.push_back(req); }

protected:
	//! @brief		同意されたタイプを設定する
	//! @param		type	同意されたタイプ
	void SetAgreedType(risse_uint32 type) { AgreedType = type; }

public: // 公開インターフェース
	//! @brief		出力ピンを接続する
	//! @param		output_pin		出力ピン(NULL=接続解除)
	void Connect(tOutputPin * output_pin);

	//! @brief		同意されたタイプを得る
	//! @return		同意されたタイプ
	risse_uint32 GetAgreedType() const;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		出力ピン
//---------------------------------------------------------------------------
class tOutputPin : public tPin, public Risa::tSubmorph<tOutputPin>
{
public:
	typedef tPin inherited;

	typedef tOutputPin tPinDirectionType; //!< テンプレート中で派生クラスが tInputPin の派生なのか tOutputPin の派生なのかを調べるために使われる

private:
	friend class tInputPin;

public:
	typedef gc_vector<tInputPin *> tInputPins; //!< 入力ピンの配列

private:
	tInputPins InputPins; //!< この出力ピンにつながっている入力ピンの配列

public:
	//! @brief		コンストラクタ
	tOutputPin();

	//! @brief		接続先の入力ピンの配列を取得する
	//! @return		接続先の入力ピンの配列
	const tInputPins & GetInputPins() const { return InputPins; }

public:
	//! @brief		このピンの先に繋がってる入力ピンに繋がってるノードのルートからの最長距離を求める
	//! @return		ルートからの最長距離
	risse_size GetLongestDistance() const;

	//! @brief		入力ピンを接続する(tInputPin::Connectから呼ばれる)
	//! @param		input_pin	入力ピン
	//! @note		サブクラスでオーバーライドしたときは最後に親クラスのこれを呼ぶこと。
	virtual void Connect(tInputPin * input_pin);

	//! @brief		入力ピンの接続を解除する(tInputPin::Connectから呼ばれる)
	//! @param		input_pin	入力ピン
	//! @note		サブクラスでオーバーライドしたときは最後に親クラスのこれを呼ぶこと。
	virtual void Disconnect(tInputPin * input_pin);
};
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
//! @brief		ピンの説明
//---------------------------------------------------------------------------
struct tPinDescriptor
{
public:
	tString			ShortName; //!< 短い名前 (i18nしないこと)
	tString			LongDescription; //!< 長い説明 (i18n可)

	//! @brief		デフォルトコンストラクタ
	tPinDescriptor() {;}

	//! @brief		コンストラクタ
	//! @param		shortname		短い名前 (i18nしないこと)
	//! @param		longdesc		長い説明 (i18n可)
	tPinDescriptor(const tString shortname, const tString longdesc)
		: ShortName(shortname), LongDescription(longdesc) {;}

	//! @brief		ref の 各文字列メンバの %1 を数値に置き換えて設定する
	//! @param		ref		元になるピンの説明
	//! @param		n		各文字列メンバの '%1' に置き換わる文字列
	void MakeNumbered(const tPinDescriptor & ref, risse_size n)
	{
		tString num(tString::AsString(static_cast<risse_int64>(n)));
		ShortName			= tString(ref.ShortName			, num);
		LongDescription		= tString(ref.LongDescription	, num);
	}
};
//---------------------------------------------------------------------------


class tProcessNode;
//---------------------------------------------------------------------------
//! @brief		ピン配列
//! @note		ピン操作の各メソッドはグラフをロックしない。
//!				ピンを操作する前にはグラフをロックすること。また、グラフがロック
//!				されていることを確実にするために、各メソッドにはグラフがロックされている
//!				かどうかの ASSERT を挿入することを強く推奨する。
//---------------------------------------------------------------------------
template <typename PINTYPE>
class tPins : public Risa::tPolymorphic
{
public:
	typedef Risa::tPolymorphic inherited;

private:
	tProcessNode * Node; //!< このピン配列を保持しているノードインスタンス

public:
	//! @brief		コンストラクタ
	//! @param		node		このピン配列を保持するノードインスタンス
	tPins(tProcessNode * node) { Node = node; }

	//! @brief		このピン配列を保持しているノードインスタンスを得る
	//! @return		このピン配列を保持しているノードインスタンス
	tProcessNode * GetNode() const { return Node; }

	//! @brief		ピンの数を得る
	//! @return		ピンの総数
	virtual risse_size GetCount()
	{
		RISSE_ASSERT_CS_LOCKED(GetNode()->GetGraph()->GetCS());
		return 0;
	}

	//! @brief		指定インデックスのピンを得る
	//! @param		index		インデックス
	virtual tPin * Get(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(GetNode()->GetGraph()->GetCS());
		/* TODO: 例外 */
		return NULL;
	}

	//! @brief		指定インデックスのピンを削除する
	//! @param		index		インデックス
	virtual void Delete(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(GetNode()->GetGraph()->GetCS());
		/* TODO: 例外 */
	}

	//! @brief		指定インデックスにピンを挿入する
	//! @param		index		インデックス
	virtual void Insert(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(GetNode()->GetGraph()->GetCS());
		/* TODO: 例外 */
	}

	//! @brief		指定インデックスのピンの情報を得る
	//! @param		index		インデックス
	//! @return		ピンの情報
	virtual tPinDescriptor GetDescriptor(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(GetNode()->GetGraph()->GetCS());
		/* TODO: 例外 */
		return tPinDescriptor();
	}

	//! @brief		指定の名前のピンのインデックスを探して返す
	//! @param		name		ピン名 (tPinDescriptor::ShortName のもの)
	//! @return		見つかったピンのインデックス (見つからなかった場合は risse_size_max)
	virtual risse_size FindPinByShortName(const tString & name)
	{
		RISSE_ASSERT_CS_LOCKED(GetNode()->GetGraph()->GetCS()); 
		risse_size count = GetCount();
		for(risse_size i = 0; i < count; i++)
			if(GetDescriptor(i).ShortName == name) return i;
		return risse_size_max;
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		入力ピンのピン配列
//! @note		このクラスではすべてのメソッドが実装されているが、
//!				すべてあたかもピンが無いかのように振る舞う。
//!				そのため、入力ピンを持たないノードの入力ピン配列に使うことができる。
//---------------------------------------------------------------------------
class tInputPins : public tPins<tInputPin>, public Risa::tSubmorph<tInputPins>
{
public:
	typedef tPins<tInputPin> inherited;

public:
	typedef tInputPin tPinType;

	//! @brief		コンストラクタ
	//! @param		node		このピン配列を保持するノードインスタンス
	tInputPins(tProcessNode * node) : inherited(node) {;}

	//! @brief		指定インデックスのピンを得る
	//! @param		index		インデックス
	//! @note		Get() と異なり、戻りの型は tInputPin * になる (単にGet()の戻りをキャストしてるだけ)
	tInputPin * At(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(GetNode()->GetGraph()->GetCS());
		return static_cast<tInputPin*>(Get(index));
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		出力ピンのピン配列
//! @note		このクラスではすべてのメソッドが実装されているが、
//!				すべてあたかもピンが無いかのように振る舞う。
//!				そのため、出力ピンを持たないノードの出力ピン配列に使うことができる。
//---------------------------------------------------------------------------
class tOutputPins : public tPins<tOutputPin>, public Risa::tSubmorph<tOutputPins>
{
public:
	typedef tPins<tOutputPin> inherited;

public:
	typedef tOutputPin tPinType;

	//! @brief		コンストラクタ
	//! @param		node		このピン配列を保持するノードインスタンス
	tOutputPins(tProcessNode * node) : inherited(node) {;}

	//! @brief		指定インデックスのピンを得る
	//! @param		index		インデックス
	//! @note		Get() と異なり、戻りの型は tOutputPin * になる (単にGet()の戻りをキャストしてるだけ)
	tOutputPin * At(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(GetNode()->GetGraph()->GetCS());
		return static_cast<tOutputPin*>(Get(index));
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		与えられた型がtInputPinかtOutputPinかでtypeの型を変えるテンプレート型
//---------------------------------------------------------------------------
template <typename QUESTION>
struct tInputPinsOrOutputPinsSelector
{
};
template <>
struct tInputPinsOrOutputPinsSelector<tInputPin>
{
	typedef tInputPins type;
};
template <>
struct tInputPinsOrOutputPinsSelector<tOutputPin>
{
	typedef tOutputPins type;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一つだけのピンを持つピン配列
//---------------------------------------------------------------------------
template <typename PINTYPE>
class tOnePins : public tInputPinsOrOutputPinsSelector<typename PINTYPE::tPinDirectionType>::type,
									public Risa::tSubmorph<tOnePins<PINTYPE> >
{
public:
	typedef typename tInputPinsOrOutputPinsSelector<typename PINTYPE::tPinDirectionType>::type inherited;

private:
	tPinDescriptor Descriptor; //!< そのピンのデスクリプタ
	PINTYPE * Pin; //!< 唯一のピン

public:
	//! @brief		コンストラクタ
	//! @param		node		このピン配列を保持するノードインスタンス
	//! @param		desc		ピンのデスクリプタ
	//! @param		pin			ピンインスタンス
	tOnePins(tProcessNode * node, const tPinDescriptor & desc, PINTYPE * pin) :
		inherited(node), Descriptor(desc), Pin(pin)
	{ Pin->Attach(node); }

	//! @brief		指定インデックスのピンを得る
	//! @param		index		インデックス
	//! @note		Get() と異なり、戻りの型は PINTYPE * になる (単にGet()の戻りをキャストしてるだけ)
	PINTYPE * At(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		return static_cast<PINTYPE*>(Get(index));
	}

	//! @brief		ピンの数を得る
	//! @return		ピンの総数
	virtual risse_size GetCount()
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		return 1;
	}

	//! @brief		指定インデックスのピンを得る
	//! @param		index		インデックス
	virtual tPin * Get(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		if(index == 0) return Pin;
		/* TODO: 例外 */
		return NULL;
	}

	//! @brief		指定インデックスのピンを削除する
	//! @param		index		インデックス
	virtual void Delete(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		/* TODO: 例外 */
		return;
	}

	//! @brief		指定インデックスにピンを挿入する
	//! @param		index		インデックス
	virtual void Insert(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		/* TODO: 例外 */
	}

	//! @brief		指定インデックスのピンの情報を得る
	//! @param		index		インデックス
	//! @return		ピンの情報
	virtual tPinDescriptor GetDescriptor(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		if(index==0) return Descriptor;
		/* TODO: 例外 */
		return tPinDescriptor();
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一つの型のピン配列
//---------------------------------------------------------------------------
template <typename PINTYPE>
class tArrayPins : public tInputPinsOrOutputPinsSelector<typename PINTYPE::tPinDirectionType>::type,
									public Risa::tSubmorph<tArrayPins<PINTYPE> >
{
public:
	typedef typename tInputPinsOrOutputPinsSelector<typename PINTYPE::tPinDirectionType>::type inherited;

public:
	typedef gc_vector<PINTYPE *> tArray;

private:
	tArray Pins; //!< ピンの配列
	tPinDescriptor Descriptor; //!< そのピンのデスクリプタ(文字列中の '%1' の部分はピン番号に置き換わる)

public:
	//! @brief		コンストラクタ
	//! @param		node		このピン配列を保持するノードインスタンス
	//! @param		desc		ピンのデスクリプタ
	tArrayPins(tProcessNode * node, const tPinDescriptor & desc) :
		inherited(node), Descriptor(desc)
	{ ; }

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tArrayPins() {;}

	//! @brief		ピンの配列(内部配列)を得る
	tArray & GetPins()
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		return Pins;
	}

	//! @brief		指定インデックスのピンを得る
	//! @param		index		インデックス
	//! @note		Get() と異なり、戻りの型は PINTYPE * になる (単にGet()の戻りをキャストしてるだけ)
	PINTYPE * At(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		return static_cast<PINTYPE*>(Get(index));
	}

	//! @brief		ピンの数を得る
	//! @return		ピンの総数
	virtual risse_size GetCount()
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		return Pins.size();
	}

	//! @brief		指定インデックスのピンを得る
	//! @param		index		インデックス
	virtual tPin * Get(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		if(index < Pins.size()) return Pins[index];
		/* TODO: 例外 */
		return NULL;
	}

	//! @brief		指定インデックスのピンを削除する
	//! @param		index		インデックス
	virtual void Delete(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		if(index < Pins.size()) Pins.erase(Pins.begin() + index);
		/* TODO: 例外 */
		return;
	}

	//! @brief		指定インデックスにピンを挿入する
	//! @param		index		インデックス
	//! @return		挿入されたピン
	virtual void Insert(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		PINTYPE * newpin = new PINTYPE();
		if(index <= Pins.size())
		{
			newpin->Attach(inherited::GetNode());
			Pins.insert(Pins.begin() + index, newpin);
		}
		/* TODO: 例外 */
	}

	//! @brief		指定インデックスのピンの情報を得る
	//! @param		index		インデックス
	//! @return		ピンの情報
	virtual tPinDescriptor GetDescriptor(risse_size index)
	{
		RISSE_ASSERT_CS_LOCKED(inherited::GetNode()->GetGraph()->GetCS());
		if(index < Pins.size())
		{
			tPinDescriptor ret;
			ret.MakeNumbered(Descriptor, index);
			return ret;
		}
		/* TODO: 例外 */
		return tPinDescriptor();
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
