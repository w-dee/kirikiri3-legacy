//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
#include "rinaWideTextEdge.h"

namespace Rina {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		出力ピン
//---------------------------------------------------------------------------
class tMultiTextOutputPin : public tOutputPin, public Risa::tSubmorph<tMultiTextOutputPin>
{
public:
	typedef tOutputPin inherited;
private:

public:
	//! @brief		コンストラクタ
	//! @param		node		ノード
	tMultiTextOutputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		テスト用の複数形式をサポートするテキストプロバイダノード
//---------------------------------------------------------------------------
class tMultiTextProviderNode : public tProcessNode, public Risa::tSubmorph<tMultiTextProviderNode>
{
public:
	typedef tProcessNode inherited;
private:

	static tPinDescriptor Descriptor; //!< ピンのデスクリプタ
	tInputPins InputPins; //!< 入力ピンの配列
	tOnePins<tMultiTextOutputPin> OutputPins; //!< 出力ピンの配列

	tString Caption; //!< 表示するテキスト(キャプション)

public:
	//! @brief		コンストラクタ
	//! @param		graph		グラフインスタンス
	tMultiTextProviderNode(tGraph * graph);

public: // 公開インターフェース
	//! @brief		キャプションを取得する
	//! @return		キャプション
	const tString & GetCaption() const { volatile tGraphLocker lock(*this); return Caption; }

	//! @brief		キャプションを設定する
	//! @return		caption キャプション
	void SetCaption(const tString & caption) { volatile tGraphLocker lock(*this); Caption = caption; }


public: // サブクラスで実装すべき物

	//! @brief		入力ピンの配列を得る
	//! @return		入力ピンの配列
	virtual tInputPins & GetInputPins() { return InputPins; }

	//! @brief		出力ピンの配列を得る
	//! @return		出力ピンの配列
	virtual tOutputPins & GetOutputPins() { return OutputPins; }

	//! @brief		コマンドキューの組み立てを行う
	//! @param		builder			キュービルダーオブジェクト
	void BuildQueue(tQueueBuilder & builder);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif
