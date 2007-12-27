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
//! @brief		入力ピン
//---------------------------------------------------------------------------
class tNarrowTextInputPin : public tInputPin
{
	typedef tInputPin inherited;

	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ

public:

	//! @brief		コンストラクタ
	tNarrowTextInputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();

	//! @brief		継承可能プロパティを得る
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
class tNarrowTextData : public tCollectee
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
class tNarrowTextQueueNode : public tQueueNode, public tNarrowTextData
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
	virtual void * GetInterface(risse_uint32 type) { if(type == NarrowTextEdgeType) return (tNarrowTextData*)this; else return NULL; }

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
