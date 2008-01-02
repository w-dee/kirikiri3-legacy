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
//! @brief		ナローテキストの入力ピン用インターフェース
//---------------------------------------------------------------------------
class tNarrowTextInputPinInterface : public tOutputPin
{
public:
	static const risse_uint32 Type = NarrowTextEdgeType; //!< このインターフェースの型

	//! @brief		親ノードから子ノードへ要求するデータの内容
	struct tRenderRequest
	{
		t1DArea		Area; //!< 範囲
		tQueueNode * ParentQueueNode; //!< 親のキューノード
	};

	//! @brief		tRequestInfo の配列の typedef
	typedef gc_vector<tRenderRequest> tRenderRequests;

public:
	//! @brief		継承可能なプロパティを得る
	//! @return		継承可能なプロパティ
	virtual tTextInheritableProperties & GetInheritableProperties() = 0;

	//! @brief		親ノードから子ノードへ要求するデータの配列を得る
	//! return		親ノードから子ノードへ要求するデータの配列
	virtual const tRenderRequests & GetRenderRequests() = 0;

	//! @brief		親ノードから子ノードへ要求するデータの配列をクリアする
	virtual void ClearRenderRequests() = 0;

	//! @brief		親ノードから子ノードへ要求するデータの配列にアイテムを追加する
	//! @param		req			要求データ
	virtual void AddRenderRequest(const tRenderRequest & req) = 0;
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		入力ピン
//---------------------------------------------------------------------------
class tNarrowTextInputPin : public tInputPin, public tNarrowTextInputPinInterface
{
	typedef tInputPin inherited;

	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ

	tRenderRequests RenderRequests; //!< 親ノードから子ノードへ要求するデータの配列

public:

	//! @brief		コンストラクタ
	tNarrowTextInputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();

	//! @brief		インターフェースを返す
	//! @param		type		返すインターフェースに対応するエッジタイプ
	//! @return		そのインターフェース(NULL=対応していない)
	virtual void * GetInterface(risse_uint32 type) { if(type == NarrowTextEdgeType) return (tNarrowTextInputPinInterface*)this; else return NULL; }

	//! @brief		継承可能なプロパティを得る
	//! @return		継承可能なプロパティ
	virtual tTextInheritableProperties & GetInheritableProperties() { return InheritableProperties; }

	//! @brief		親ノードから子ノードへ要求するデータの配列を得る
	//! return		親ノードから子ノードへ要求するデータの配列
	virtual const tRenderRequests & GetRenderRequests() { return RenderRequests; }

	//! @brief		親ノードから子ノードへ要求するデータの配列をクリアする
	void ClearRenderRequests() { RenderRequests.clear(); }

	//! @brief		親ノードから子ノードへ要求するデータの配列にアイテムを追加する
	//! @param		req			要求データ
	void AddRenderRequest(const tRenderRequest & req) { RenderRequests.push_back(req); }
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

	//! @brief		継承可能プロパティを得る
	//! @return		継承可能プロパティ
	virtual const tTextInheritableProperties & GetInheritableProperties() = 0;

	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const char * GetText() = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		テスト用のテキストコマンドキュー
//---------------------------------------------------------------------------
class tNarrowTextQueueNode : public tQueueNode, public tNarrowTextDataInterface
{
	typedef tQueueNode inherited;

protected:

public:

	//! @brief		コンストラクタ
	//! @param		parent		親ノード
	tNarrowTextQueueNode(tQueueNode * parent) :
		inherited(parent) {;}

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
//! @brief		入力ピン用のテキストコマンドキュー
//---------------------------------------------------------------------------
class tNarrowTextInputPinQueueNode : public tNarrowTextQueueNode
{
	typedef tNarrowTextQueueNode inherited;

protected:
	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ
	tTextInheritableProperties		InheritedProperty; //!< 継承されたプロパティ TODO: これ非効率

public:
	//! @brief		コンストラクタ
	//! @param		parent		親ノード
	//! @param		prop		継承可能なプロパティ
	tNarrowTextInputPinQueueNode(tQueueNode * parent, const tTextInheritableProperties & prop) :
		inherited(parent), InheritableProperties(prop) {;}

	//! @brief		継承可能プロパティを得る
	//! @return		継承可能プロパティ
	virtual const tTextInheritableProperties & GetInheritableProperties();

	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const char * GetText();

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
