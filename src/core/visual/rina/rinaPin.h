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
//---------------------------------------------------------------------------
//! @brief		入力ピン
//---------------------------------------------------------------------------
class tInputPin : public tPin
{
	typedef tPin inherited;

	tOutputPin * OutputPin; //!< この入力ピンにつながっている出力ピン
	risse_uint32 AgreedType; //!< 同意されたタイプを得る

public:
	//! @brief		コンストラクタ
	tInputPin();

	//! @brief		接続先の出力ピンを取得する
	//! @return		接続先の出力ピン
	tOutputPin * GetOutputPin() const { return OutputPin; }

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

	//! @brief		コマンドキューを組み立てる
	//! @param		parent	親のコマンドキュー
	void BuildQueue(tQueueNode * parent);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		出力ピン
//---------------------------------------------------------------------------
class tOutputPin : public tPin
{
	friend class tInputPin;
	typedef tPin inherited;

	tInputPin * InputPin; //!< この出力ピンにつながっている入力ピン

public:
	//! @brief		コンストラクタ
	tOutputPin();

	//! @brief		接続先の入力ピンを取得する
	//! @return		接続先の入力ピン
	tInputPin * GetInputPin() const { return InputPin; }

protected:
	//! @brief		入力ピンを接続する(tInputPin::Connectから呼ばれる)
	//! @param		input_pin	入力ピン
	//! @note		サブクラスでオーバーライドしたときは最後に親クラスのこれを呼ぶこと。
	virtual void Connect(tInputPin * input_pin);

	//! @brief		コマンドキューを組み立てる
	//! @param		parent	親のコマンドキュー
	void BuildQueue(tQueueNode * parent) { if(Node) Node->BuildQueue(parent); }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
