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
//! @brief テスト用のナローテキストピン管理
//---------------------------------------------------------------------------
#ifndef RINANARROWTEXTPIN_H
#define RINANARROWTEXTPIN_H

#include "visual/rina/rinaPin.h"
#include "visual/rina/rinaQueue.h"
#include "visual/rina/rinaIdRegistry.h"
#include "rina1DRegion.h"
#include "rinaTextProperty.h"


namespace Rina {
//---------------------------------------------------------------------------


static const risse_uint32 NarrowTextEdgeType = tFourCharId<'t','x','t','n'>::value;
	//!< テキストデータのエッジタイプ

//---------------------------------------------------------------------------
//! @brief		テキストピンのエッジタイプを登録するためのシングルトン
//---------------------------------------------------------------------------
class tNarrowTextEdgeTypeRegisterer : public Risa::singleton_base<tNarrowTextEdgeTypeRegisterer>
{
public:
	//! @brief		コンストラクタ
	tNarrowTextEdgeTypeRegisterer();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		親ノードから子ノードへ要求するデータの内容(レンダリング要求)
//---------------------------------------------------------------------------
class tNarrowTextRenderRequest : public tRenderRequest
{
	typedef tRenderRequest inherited;

	t1DArea		Area; //!< 範囲

public:
	//! @brief		コンストラクタ
	//! @param		parent		親キューノード
	//! @param		index		親キューノード内でのインデックス
	//! @param		area		要求範囲
	tNarrowTextRenderRequest(tQueueNode * parent, risse_size index, const t1DArea & area) :
		inherited(parent, index), Area(area) {;}

	//! @brief		要求範囲を得る
	const t1DArea & GetArea() const { return Area; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ミキサ(あるいはdrawdevice)用の親ノードから子ノードへ要求するデータの内容
//---------------------------------------------------------------------------
class tNarrowTextMixerRenderRequest : public tNarrowTextRenderRequest
{
	typedef tNarrowTextRenderRequest inherited;

	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ

public:
	//! @brief		コンストラクタ
	//! @param		parent		親キューノード
	//! @param		index		親キューノード内でのインデックス
	//! @param		area		要求範囲
	//! @param		pops		プロパティ
	tNarrowTextMixerRenderRequest(tQueueNode * parent, risse_size index, const t1DArea & area,
		const tTextInheritableProperties & props) :
		inherited(parent, index, area), InheritableProperties(props) {;}

	//! @brief		継承可能なプロパティを得る
	//! @return		継承可能なプロパティ
	const tTextInheritableProperties & GetInheritableProperties() const { return InheritableProperties; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		入力ピン
//---------------------------------------------------------------------------
class tNarrowTextInputPin : public tInputPin
{
	typedef tInputPin inherited;

public:

	//! @brief		コンストラクタ
	tNarrowTextInputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		ミキサ(あるいはdrawdevice)用入力ピン
//---------------------------------------------------------------------------
class tNarrowTextMixerInputPin : public tNarrowTextInputPin
{
	typedef tNarrowTextInputPin inherited;

	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ

public:
	//! @brief		コンストラクタ
	tNarrowTextMixerInputPin() : tNarrowTextInputPin() { ; }

	//! @brief		継承可能なプロパティを得る
	//! @return		継承可能なプロパティ
	tTextInheritableProperties & GetInheritableProperties() { return InheritableProperties; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		出力ピン
//---------------------------------------------------------------------------
class tNarrowTextOutputPin : public tOutputPin
{
	typedef tOutputPin inherited;

public:
	//! @brief		コンストラクタ
	tNarrowTextOutputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ナロー文字列用のデータ取得用インターフェース
//---------------------------------------------------------------------------
class tNarrowTextDataInterface : public tCollectee
{
public:
	static const risse_uint32 Type = NarrowTextEdgeType; //!< このインターフェースの型

	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const char * GetText() = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ナロー文字列用のテキストコマンドキュー
//---------------------------------------------------------------------------
class tNarrowTextQueueNode : public tQueueNode, public tNarrowTextDataInterface
{
	typedef tQueueNode inherited;

public:

	//! @brief		コンストラクタ
	//! @param		request		レンダリング要求
	tNarrowTextQueueNode(const tNarrowTextRenderRequest * request) :
		inherited(request) {;}

	//! @brief		インターフェースを返す
	//! @param		type		返すインターフェースに対応するエッジタイプ
	//! @return		そのインターフェース(NULL=対応していない)
	virtual void * GetInterface(risse_uint32 type) { if(type == NarrowTextEdgeType) return (tNarrowTextDataInterface*)this; else return NULL; }

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
