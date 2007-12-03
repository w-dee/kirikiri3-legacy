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
//! @brief テスト用のテキストプロバイダノード管理
//---------------------------------------------------------------------------
#ifndef RINATEXTPROVIDERNODE_H
#define RINATEXTPROVIDERNODE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaProperty.h"
#include "visual/rina/rinaQueue.h"

namespace Rina {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		テスト用のテキストプロパティセット
//---------------------------------------------------------------------------
class tTextPropertySet :
	public tPropertySet,
	public Risa::singleton_base<tTextPropertySet>,
	Risa::manual_start<tTextPropertySet>
{
public:
	//! @brief		コンストラクタ
	tTextPropertySet();
};
//---------------------------------------------------------------------------



class tTextOutputPin;
//---------------------------------------------------------------------------
//! @brief		テスト用のテキストプロバイダノード
//---------------------------------------------------------------------------
class tTextProviderNode : public tProcessNode
{
	typedef tProcessNode inherited;

	tTextOutputPin * OutputPin; //!< 出力ピン

public:
	//! @brief		コンストラクタ
	tTextProviderNode();

public: // サブクラスで実装すべき物

	//! @brief		出力ピンの個数を得る
	//! @return		出力ピンの個数
	virtual risse_size GetOutputPinCount();

	//! @brief		指定位置の出力ピンを得る
	//! @param		n		指定位置
	//! @return		指定位置の出力ピン
	virtual tOutputPin * GetOutputPinAt(risse_size n);

	//! @brief		指定位置に新規出力ピンを挿入する
	//! @param		n		指定位置
	virtual void InsertOutputPinAt(risse_size n);

	//! @brief		指定位置から出力ピンを削除する
	//! @param		n		指定位置
	virtual void DeleteOutputPinAt(risse_size n);



	//! @brief		入力ピンの個数を得る
	//! @return		入力ピンの個数
	virtual risse_size GetInputPinCount();

	//! @brief		指定位置の入力ピンを得る
	//! @param		n		指定位置
	//! @return		指定位置の入力ピン
	virtual tInputPin * GetInputPinAt(risse_size n);

	//! @brief		指定位置に入力ピンを挿入する
	//! @param		n		指定位置
	virtual void InsertInputPinAt(risse_size n);

	//! @brief		指定位置から入力ピンを削除する
	//! @param		n		指定位置
	virtual void DeleteInputPinAt(risse_size n);

	//! @brief		コマンドキューを組み立てる
	//! @param		parent	親のコマンドキュー
	virtual void BuildQueue(tQueueNode * parent);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		テスト用のテキストコマンドキュー
//---------------------------------------------------------------------------
class tTextProviderQueueNode : public tQueueNode
{
	typedef tQueueNode inherited;

	risse_int32 Position; //!< 位置
	tString Text; //!< テキスト

public:
	//! @brief		コンストラクタ
	//! @param		pos		位置
	//! @param		text	テキスト
	tTextProviderQueueNode(risse_int32 pos, const tString & text) : Position(pos), Text(text) {;}

	//! @brief		位置を得る
	//! @return		位置
	risse_int32 GetPosition() const { return Position; }

	//! @brief		テキストを得る
	//! @return		テキスト
	const tString & GetText() const { return Text; }

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess() {;}

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess() {;}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}

#endif
