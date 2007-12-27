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
//! @brief		入力ピン
//---------------------------------------------------------------------------
class tWideTextInputPin : public tInputPin
{
	typedef tInputPin inherited;

	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ

public:

	//! @brief		コンストラクタ
	tWideTextInputPin();

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
//! @brief		テスト用のテキストコマンドキュー
//---------------------------------------------------------------------------
class tWideTextQueueNode : public tQueueNode
{
	typedef tQueueNode inherited;

protected:

public:
	//! @brief		コンストラクタ
	//! @param		parent		親ノード
	tWideTextQueueNode(tQueueNode * parent) :
		inherited(parent) {;}

	//! @brief		継承可能プロパティを得る
	//! @return		継承可能プロパティ
	virtual const tTextInheritableProperties & GetInheritableProperties() = 0;

	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const tString & GetText() = 0;

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
class tWideTextInputPinQueueNode : public tWideTextQueueNode
{
	typedef tWideTextQueueNode inherited;

protected:
	tTextInheritableProperties		InheritableProperties; //!< 継承可能なプロパティ
	tTextInheritableProperties		InheritedProperty; //!< 継承されたプロパティ TODO: これ非効率

public:
	//! @brief		コンストラクタ
	//! @param		parent		親ノード
	//! @param		prop		継承可能なプロパティ
	tWideTextInputPinQueueNode(tQueueNode * parent, const tTextInheritableProperties & prop) :
		inherited(parent), InheritableProperties(prop) {;}

	//! @brief		継承可能プロパティを得る
	//! @return		継承可能プロパティ
	virtual const tTextInheritableProperties & GetInheritableProperties();

	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const tString & GetText();

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