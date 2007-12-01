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
//! @brief RINA コマンドキュー管理
//---------------------------------------------------------------------------
#ifndef RINAPROPERTY_H
#define RINAPROPERTY_H

#include "visual/rina/rinaNode.h"

namespace Rina {
//---------------------------------------------------------------------------


/*!
	@note
	コマンドキューとは言っても線形キューではなくて、依存関係を表せるように
	ツリー状のキューとなる
*/

//---------------------------------------------------------------------------
//! @brief		コマンドキューノード
//---------------------------------------------------------------------------
class tQueueNode : public tCollectee
{
	typedef tCollectee inherited;

protected:
	typedef gc_vector<tQueueNode*> tChildren; //!< 子ノードの配列のtypedef
	tChildren Children; //!< 子ノード

public:
	//! @brief		コンストラクタ
	tQueueNode();

	//! @brief		ノードの処理を行う(仮実装)
	void Process();

	//! @brief		ノードの子を追加する
	//! @param		child		子
	void AddChild(tQueueNode * child) { Children.push_back(child); }

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess() = 0;

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess() = 0;

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		ルートコマンドキューノード
//---------------------------------------------------------------------------
class tRootQueueNode : public tQueueNode
{
	typedef tQueueNode inherited;

public:
	//! @brief		コンストラクタ
	tRootQueueNode();

protected: //!< サブクラスでオーバーライドして使う物

	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess();

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess();

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}

#endif
