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

namespace Rina {
//---------------------------------------------------------------------------


class tProcessNode;
//---------------------------------------------------------------------------
//! @brief		ピン
//---------------------------------------------------------------------------
class tPin : public tCollectee
{
	typedef tCollectee inherited;

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

	//! @brief		インターフェースを返す
	//! @param		type		返すインターフェースに対応するエッジタイプ
	//! @return		そのインターフェース(NULL=対応していない)
	virtual void * GetInterface(risse_uint32 type) { return NULL; }
};
//---------------------------------------------------------------------------



class tOutputPin;
//---------------------------------------------------------------------------
//! @brief		入力ピン
//! @note		一つの入力ピンは複数の出力ピンとは接続されない。このため
//!				入力ピンはほぼ「エッジ」と同じと見なすことができる。このため
//!				入力ピンごとに「エッジを流通するデータのタイプ」としての
//!				tInputPin::AgreedTypeを持っている。
//---------------------------------------------------------------------------
class tInputPin : public tPin
{
	typedef tPin inherited;

	tOutputPin * OutputPin; //!< この入力ピンにつながっている出力ピン
	risse_uint32 AgreedType; //!< 同意されたタイプ
	tQueueNode * ParentQueueNode; //!< キュー組み立て時に親となるキューノード

	tIdRegistry::tRenderGeneration RenderGeneration; //!< 最新の情報が設定されたレンダリング世代

public:
	//! @brief		コンストラクタ
	tInputPin();

	//! @brief		接続先の出力ピンを取得する
	//! @return		接続先の出力ピン
	tOutputPin * GetOutputPin() const { return OutputPin; }

	//! @brief		キュー組み立て時に親となるキューノードを設定する
	//! @param		node		親となるキューノード
	//! @param		gen			このレンダリング世代
	void SetParentQueueNode(tQueueNode * node)
		{ ParentQueueNode = node; }

	//! @brief		キュー組み立て時に親となるキューノードを取得する
	//! @return		親となるキューノード
	tQueueNode * GetParentQueueNode() const { return ParentQueueNode; }

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
	//! @note		出力ピントの間でネゴシエーションを行い、同意のとれたタイプを返す。
	//!				実際に接続したりはしない。
	//! @note		出力ピン側のこのメソッドはprotectedになっていて外部からアクセスできない。
	//!				入力ピンと出力ピンに対して行う SuggestType() のうち、
	//! @note		片方が strong_suggest を真にした場合は
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
	virtual void Connect(tOutputPin * output_pin);

protected:

	//! @brief		同意されたタイプを設定する
	//! @param		type	同意されたタイプ
	void SetAgreedType(risse_uint32 type) { AgreedType = type; }

public:
	//! @brief		同意されたタイプを得る
	//! @return		同意されたタイプ
	risse_uint32 GetAgreedType() const { return AgreedType; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		出力ピン
//---------------------------------------------------------------------------
class tOutputPin : public tPin
{
	friend class tInputPin;
	typedef tPin inherited;

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

	//! @brief		このピンの先に繋がってる入力ピンに繋がってるノードのルートからの最長距離を求める
	//! @return		ルートからの最長距離
	risse_size GetLongestDistance() const;

protected:
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
}

#endif
