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
//! @brief テスト用のナローテキストプロバイダノード管理
//---------------------------------------------------------------------------
#ifndef RINANARROWTEXTPROVIDERNODE_H
#define RINANARROWTEXTPROVIDERNODE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaQueue.h"

namespace Rina {
//---------------------------------------------------------------------------



class tNarrowTextOutputPin;
//---------------------------------------------------------------------------
//! @brief		テスト用のテキストプロバイダノード
//---------------------------------------------------------------------------
class tNarrowTextProviderNode : public tProcessNode
{
	typedef tProcessNode inherited;

	tNarrowTextOutputPin * OutputPin; //!< 出力ピン

	risse_int32 Position; //!< テキストの結果表示位置
	char * Caption; //!< 表示するテキスト(キャプション)

public:
	//! @brief		コンストラクタ
	tNarrowTextProviderNode();

	//! @brief		表示位置を取得する
	//! @return		表示位置
	risse_int32 GetPosition() const { return Position; }

	//! @brief		表示位置を設定する
	//! @return		position 表示位置
	void SetPosition(risse_int32 position) { Position = position; }

	//! @brief		キャプションを取得する
	//! @return		キャプション
	const char * GetCaption() const { return Caption; }

	//! @brief		キャプションを設定する
	//! @return		caption キャプション (内容はコピーされる)
	void SetCaption(const char * caption);


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

	//! @brief		コマンドキューの組み立てを行う
	//! @param		state			レンダリングステート
	void BuildQueue(tRenderState * state);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		テスト用のテキストコマンドキュー
//---------------------------------------------------------------------------
class tNarrowTextProviderQueueNode : public tQueueNode
{
	typedef tQueueNode inherited;

protected:
	risse_int32 Position; //!< 位置
	const char * Text; //!< テキスト

public:
	//! @brief		コンストラクタ
	//! @param		parent		親ノード
	//! @param		pos		位置
	//! @param		text	テキスト
	tNarrowTextProviderQueueNode(tQueueNode * parent,	risse_int32 pos, const char * text) :
		inherited(parent),
		Position(pos), Text(text) {;}

	//! @brief		位置を得る
	//! @return		位置
	virtual risse_int32 GetPosition() const { return Position; }

	//! @brief		位置にオフセットを追加する
	//! @param		pos	位置
	virtual void AddPositionOffset(risse_int32 pos) { Position += pos; }

	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const char * GetText() const { return Text; }

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
