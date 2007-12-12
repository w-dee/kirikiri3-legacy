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
//! @brief テスト用の複数形式をサポートするテキストプロバイダノード管理
//---------------------------------------------------------------------------
#ifndef RINAMULTITEXTPROVIDERNODE_H
#define RINAMULTITEXTPROVIDERNODE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaQueue.h"

namespace Rina {
//---------------------------------------------------------------------------



class tMultiTextOutputPin;
//---------------------------------------------------------------------------
//! @brief		テスト用の複数形式をサポートするテキストプロバイダノード
//---------------------------------------------------------------------------
class tMultiTextProviderNode : public tProcessNode
{
	typedef tProcessNode inherited;

	tMultiTextOutputPin * OutputPin; //!< 出力ピン

	risse_int32 Position; //!< テキストの結果表示位置
	tString Caption; //!< 表示するテキスト(キャプション)

public:
	//! @brief		コンストラクタ
	tMultiTextProviderNode();

	//! @brief		表示位置を取得する
	//! @return		表示位置
	risse_int32 GetPosition() const { return Position; }

	//! @brief		表示位置を設定する
	//! @return		position 表示位置
	void SetPosition(risse_int32 position) { Position = position; }

	//! @brief		キャプションを取得する
	//! @return		キャプション
	const tString & GetCaption() const { return Caption; }

	//! @brief		キャプションを設定する
	//! @return		caption キャプション
	void SetCaption(const tString & caption) { Caption = caption; }


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
}

#endif
