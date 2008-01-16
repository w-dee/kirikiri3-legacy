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
//! @brief ワイドテキストのエッジタイプ
//---------------------------------------------------------------------------
#ifndef RINAWIDETEXTEDGE_H
#define RINAWIDETEXTEDGE_H

#include "visual/rina/rinaPin.h"
#include "visual/rina/rinaIdRegistry.h"
#include "visual/rina/rinaQueue.h"
#include "rina1DRegion.h"
#include "rinaTextProperty.h"

namespace Rina {
//---------------------------------------------------------------------------



static const risse_uint32 WideTextEdgeType = tFourCharId<'t','x','t','w'>::value;
	//!< テキストデータのエッジタイプ

//---------------------------------------------------------------------------
//! @brief		テキストピンのエッジタイプを登録するためのシングルトン
//---------------------------------------------------------------------------
class tWideTextEdgeTypeRegisterer : public Risa::singleton_base<tWideTextEdgeTypeRegisterer>
{
public:
	//! @brief		コンストラクタ
	tWideTextEdgeTypeRegisterer();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		親ノードから子ノードへ要求するデータの内容(レンダリング要求)
//---------------------------------------------------------------------------
class tWideTextRenderRequest : public tRenderRequest, public Risa::tSubmorph<tWideTextRenderRequest>
{
public:
	typedef tRenderRequest inherited;
private:

	t1DArea		Area; //!< 範囲

public:
	//! @brief		コンストラクタ
	//! @param		parent		親キューノード
	//! @param		index		親キューノード内でのインデックス
	//! @param		area		要求範囲
	tWideTextRenderRequest(tQueueNode * parent, risse_size index, const t1DArea & area) :
		inherited(parent, index), Area(area) {;}

	//! @brief		要求範囲を得る
	const t1DArea & GetArea() const { return Area; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ミキサ(あるいはdrawdevice)用の親ノードから子ノードへ要求するデータの内容
//---------------------------------------------------------------------------
class tWideTextMixerRenderRequest : public tWideTextRenderRequest, public Risa::tSubmorph<tWideTextMixerRenderRequest>
{
public:
	typedef tWideTextRenderRequest inherited;
private:

	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ

public:
	//! @brief		コンストラクタ
	//! @param		area		要求範囲
	//! @param		index		親キューノード内でのインデックス
	//! @param		area		要求範囲(propsにしたがって変形が自動的に行われる)
	//! @param		props		プロパティ
	tWideTextMixerRenderRequest(tQueueNode * parent, risse_size index,
		const t1DArea & area,
		const tTextInheritableProperties & props) :
		inherited(parent, index, props.ToChild(area)), InheritableProperties(props) {;}

	//! @brief		継承可能なプロパティを得る
	//! @return		継承可能なプロパティ
	const tTextInheritableProperties & GetInheritableProperties() const { return InheritableProperties; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		入力ピン
//---------------------------------------------------------------------------
class tWideTextInputPin : public tInputPin, public Risa::tSubmorph<tWideTextInputPin>
{
public:
	typedef tInputPin inherited;
private:

public:

	//! @brief		コンストラクタ
	tWideTextInputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();

	//! @brief		内容の更新があったことを上位のノードに伝える
	//! @param		area		範囲
	//! @brief		デフォルトでは何もしない
	virtual void NotifyUpdate(const t1DArea & area) {;}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ミキサ(あるいはdrawdevice)用入力ピン
//---------------------------------------------------------------------------
class tWideTextMixerInputPin : public tWideTextInputPin, public Risa::tSubmorph<tWideTextMixerInputPin>
{
public:
	typedef tWideTextInputPin inherited;
private:

	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ

public:
	//! @brief		コンストラクタ
	tWideTextMixerInputPin() : tWideTextInputPin() { ; }

	//! @brief		継承可能なプロパティを得る
	//! @return		継承可能なプロパティ
	tTextInheritableProperties & GetInheritableProperties() { return InheritableProperties; }

	//! @brief		内容の更新があったことを上位のノードに伝える
	//! @param		area		範囲
	virtual void NotifyUpdate(const t1DArea & area);
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		出力ピン
//---------------------------------------------------------------------------
class tWideTextOutputPin : public tOutputPin, public Risa::tSubmorph<tWideTextOutputPin>
{
public:
	typedef tOutputPin inherited;
private:

public:
	//! @brief		コンストラクタ
	tWideTextOutputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();

	//! @brief		内容の更新があったことを上位のノードに伝える
	//! @param		area		範囲
	void NotifyUpdate(const t1DArea & area);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		ワイド文字列用のテキストコマンドキュー
//---------------------------------------------------------------------------
class tWideTextQueueNode : public tQueueNode, public Risa::tSubmorph<tWideTextQueueNode>
{
public:
	typedef tQueueNode inherited;
private:

public:
	//! @brief		コンストラクタ
	//! @param		request		レンダリング要求
	tWideTextQueueNode(const tWideTextRenderRequest * request) :
		inherited(request) {;}

public: //!< サブクラスでオーバーライドして使う物
	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const tString & GetText() = 0;

	//! @brief		テキストの範囲を得る
	//! @return		範囲
	//! @brief		GetText() で得られたテキストがどこからどこまでの範囲を表しているか
	virtual const t1DArea & GetArea() = 0;

	//! @brief		テキストのオフセットを得る
	//! @return		オフセット
	//! @brief		Text 中のどのオフセットが GetArea().GetBegin() 位置を表しているかを表す。
	//! @brief		GetText() で得られたテキストのうち、[GetOffset(), GetOffset() + GetArea().GetLength()) の
	//! 			範囲のみを使用すること。それ以外はたとえアクセス可能であってもアクセスしてはならない。
	virtual risse_offset GetOffset() = 0;

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
