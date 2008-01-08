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
class tWideTextRenderRequest : public tRenderRequest
{
	typedef tRenderRequest inherited;

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
class tWideTextMixerRenderRequest : public tWideTextRenderRequest
{
	typedef tWideTextRenderRequest inherited;

	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ

public:
	//! @brief		コンストラクタ
	//! @param		area		要求範囲
	//! @param		index		親キューノード内でのインデックス
	//! @param		pops		プロパティ
	tWideTextMixerRenderRequest(tQueueNode * parent, risse_size index, const t1DArea & area, const tTextInheritableProperties & props) :
		inherited(parent, index, area), InheritableProperties(props) {;}

	//! @brief		継承可能なプロパティを得る
	//! @return		継承可能なプロパティ
	const tTextInheritableProperties & GetInheritableProperties() const { return InheritableProperties; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		入力ピン
//---------------------------------------------------------------------------
class tWideTextInputPin : public tInputPin
{
	typedef tInputPin inherited;

public:

	//! @brief		コンストラクタ
	tWideTextInputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ミキサ(あるいはdrawdevice)用入力ピン
//---------------------------------------------------------------------------
class tWideTextMixerInputPin : public tWideTextInputPin
{
	typedef tWideTextInputPin inherited;

	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ

public:
	//! @brief		コンストラクタ
	tWideTextMixerInputPin() : tWideTextInputPin() { ; }

	//! @brief		継承可能なプロパティを得る
	//! @return		継承可能なプロパティ
	tTextInheritableProperties & GetInheritableProperties() { return InheritableProperties; }
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		出力ピン
//---------------------------------------------------------------------------
class tWideTextOutputPin : public tOutputPin
{
	typedef tOutputPin inherited;

public:
	//! @brief		コンストラクタ
	tWideTextOutputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		ワイド文字列用のデータ取得用インターフェース
//---------------------------------------------------------------------------
class tWideTextDataInterface : public tCollectee
{
public:
	static const risse_uint32 Type = WideTextEdgeType; //!< このインターフェースの型

	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const tString & GetText() = 0;
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		ワイド文字列用のテキストコマンドキュー
//---------------------------------------------------------------------------
class tWideTextQueueNode : public tQueueNode, public tWideTextDataInterface
{
	typedef tQueueNode inherited;

public:
	//! @brief		コンストラクタ
	//! @param		request		レンダリング要求
	tWideTextQueueNode(const tWideTextRenderRequest * request) :
		inherited(request) {;}

	//! @brief		インターフェースを返す
	//! @param		type		返すインターフェースに対応するエッジタイプ
	//! @return		そのインターフェース(NULL=対応していない)
	virtual void * GetInterface(risse_uint32 type) { if(type == WideTextEdgeType) return (tWideTextDataInterface*)this; else return NULL; }

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
