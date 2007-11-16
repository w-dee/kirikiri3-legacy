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

	tProcessNode * Node; //!< このピンを保有しているノード

public:
	//! @brief		コンストラクタ
	tPin();

	//! @brief		指定された形式が接続可能かどうかを判断する
	//! @param		type		接続形式
	//! @return		接続可能かどうか
	virtual bool CanConnect(risse_uint32 type) = 0;

	//! @brief		プロセスノードにこのピンをアタッチする
	//! @param		node		プロセスノード (NULL=デタッチ)
	void Attatch(tProcessNode * node) { Node = node; }
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

public:
	//! @brief		コンストラクタ
	//! @param		node		ノード
	tInputPin();

	//! @brief		出力ピンを接続する
	//! @param		output_pin		出力ピン
	//! @note		サブクラスでオーバーライドしたときは最後に親クラスのこれを呼ぶこと。
	//! @note		実際に形式が合うかどうかのチェックはここでは行わない。強制的につないでしまうので注意
	virtual void Connect(tOutputPin * output_pin);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		出力ピン
//---------------------------------------------------------------------------
class tOutputPin : public tPin
{
	typedef tPin inherited;

	tInputPin * InputPin; //!< この出力ピンにつながっている入力ピン

public:
	//! @brief		コンストラクタ
	//! @param		node		ノード
	tOutputPin();

	//! @brief		入力ピンを接続する
	//! @param		input_pin		入力ピン
	//! @note		サブクラスでオーバーライドしたときは最後に親クラスのこれを呼ぶこと。
	//! @note		実際に形式が合うかどうかのチェックはここでは行わない。強制的につないでしまうので注意
	virtual void Connect(tOutputPin * input_pin);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

