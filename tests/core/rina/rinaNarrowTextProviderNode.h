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
//! @brief テスト用のナローテキストプロバイダノード管理
//---------------------------------------------------------------------------
#ifndef RINANARROWTEXTPROVIDERNODE_H
#define RINANARROWTEXTPROVIDERNODE_H

#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaQueue.h"
#include "rinaNarrowTextEdge.h"
#include "rina1DUpdateReceiver.h"

namespace Rina {
//---------------------------------------------------------------------------



class tNarrowTextOutputPin;
//---------------------------------------------------------------------------
//! @brief		テスト用のテキストプロバイダノード
//---------------------------------------------------------------------------
class tNarrowTextProviderNode : public t1DUpdateReceiver, public Risa::tSubmorph<tNarrowTextProviderNode>
{
public:
	typedef t1DUpdateReceiver inherited;
private:

	static tPinDescriptor Descriptor; //!< ピンのデスクリプタ
	tInputPins InputPins; //!< 入力ピンの配列
	tOnePins<tNarrowTextOutputPin> OutputPins; //!< 出力ピンの配列

	char * Caption; //!< 表示するテキスト(キャプション)

public:
	//! @brief		コンストラクタ
	//! @param		graph		グラフインスタンス
	tNarrowTextProviderNode(tGraph * graph);

	//! @brief		キャプションを取得する
	//! @return		キャプション
	const char * GetCaption() const { volatile tGraphLocker lock(*this); return Caption; }

	//! @brief		キャプションを設定する
	//! @return		caption キャプション (内容はコピーされる)
	void SetCaption(const char * caption);


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
//! @brief		テスト用のテキストコマンドキューノード
//---------------------------------------------------------------------------
class tNarrowTextProviderQueueNode : public tNarrowTextQueueNode, public Risa::tSubmorph<tNarrowTextProviderQueueNode>
{
public:
	typedef tNarrowTextQueueNode inherited;
private:

protected:
	const char * Text; //!< テキスト
	t1DArea Area; //!< テキストの範囲
	risse_offset Offset; //!< Area.GetBegin() に対応する Text のインデックス

public:
	//! @brief		コンストラクタ
	//! @param		request		レンダリング要求
	//! @param		text	テキスト
	//! @param		text_area	テキストの使用範囲
	tNarrowTextProviderQueueNode(const tNarrowTextRenderRequest * request,
			const char * text, const t1DArea & area, risse_offset offset) :
		inherited(request),
		Text(text),
		Area(area),
		Offset(offset) {;}

	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const char * GetText() { return Text; }

	//! @brief		テキストの範囲を得る
	//! @return		範囲
	//! @brief		GetText() で得られたテキストがどこからどこまでの範囲を表しているか
	virtual const t1DArea & GetArea() { return Area; }

	//! @brief		テキストのオフセットを得る
	//! @return		オフセット
	//! @brief		Text 中のどのオフセットが GetArea().GetBegin() 位置を表しているかを表す。
	//! @brief		GetText() で得られたテキストのうち、[GetOffset(), GetOffset() + GetArea().GetLength()) の
	//! 			範囲のみを使用すること。それ以外はたとえアクセス可能であってもアクセスしてはならない。
	virtual risse_offset GetOffset() { return Offset; }

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
