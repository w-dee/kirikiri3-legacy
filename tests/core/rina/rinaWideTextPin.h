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
//! @brief テスト用のテキストピン管理
//---------------------------------------------------------------------------
#ifndef RINAWIDETEXTPIN_H
#define RINAWIDETEXTPIN_H

#include "visual/rina/rinaPin.h"
#include "visual/rina/rinaIdRegistry.h"
#include "visual/rina/rinaQueue.h"

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

	risse_int32		Position; //!< 位置

public:

	//! @brief		コンストラクタ
	tWideTextInputPin();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();

	//! @brief		位置を設定する
	//! @param		pos		位置
	void SetPosition(int pos) { Position = pos; }

	//! @brief		位置を取得する
	//! @return		位置
	risse_int32 GetPosition() { return Position; }
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

	//! @brief		位置を得る
	//! @return		位置
	virtual risse_int32 GetPosition() const = 0;

	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const tString & GetText() const = 0;

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess() {;}

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess() {;}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		オフセットなどのプロパティを加算するためのアダプタ的オブジェクト
//---------------------------------------------------------------------------
class tWideTextInputPinQueueNode : public tWideTextQueueNode
{
	typedef tWideTextQueueNode inherited;

protected:
	risse_int32 Position; //!< 位置

public:
	//! @brief		コンストラクタ
	//! @param		parent		親ノード
	//! @param		pos		位置
	tWideTextInputPinQueueNode(tQueueNode * parent, risse_int32 pos) :
		inherited(parent), Position(pos) {;}

	//! @brief		位置を得る
	//! @return		位置
	virtual risse_int32 GetPosition() const;

	//! @brief		テキストを得る
	//! @return		テキスト
	virtual const tString & GetText() const;

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
