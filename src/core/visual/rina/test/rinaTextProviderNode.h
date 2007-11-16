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
//! @brief テスト用のテキストプロバイダノード管理
//---------------------------------------------------------------------------
#include "visual/rina/rinaNode.h"


namespace Rina {
//---------------------------------------------------------------------------


class tTextOutputPin;
//---------------------------------------------------------------------------
//! @brief		テスト用のテキストプロバイダノード
//---------------------------------------------------------------------------
class tTextProviderNode : public tProcessNode
{
	typedef tProcessNode inherited;

	tTextOutputPin * Parent; //!< 親ノード

public:
	//! @brief		コンストラクタ
	//! @param		graph		グラフインスタンス
	tTextDrawDeviceProcessNode(tGraph * graph);

public: // サブクラスで実装すべき物

	//! @brief		親ノードの個数を得る
	//! @return		親ノードの個数
	virtual risse_size GetParentCount();

	//! @brief		指定位置の親ノードを得る
	//! @param		n		指定位置
	//! @return		指定位置の親ノード
	virtual tProcessNode * GetParentAt(risse_size n);

	//! @brief		指定位置に親ノードを設定する
	//! @param		n		指定位置
	//! @param		node	親ノード
	virtual void SetParentAt(risse_size n, tProcessNode * node);

	//! @brief		指定位置に新規親ノード用ピンを挿入する
	//! @param		n		指定位置
	virtual void InsertParentPinAt(risse_size n);

	//! @brief		指定位置から親ノード用ピンを削除する
	//! @param		n		指定位置
	virtual void DeleteParentPinAt(risse_size n);



	//! @brief		子ノードの個数を得る
	//! @return		子ノードの個数
	virtual risse_size GetChildCount();

	//! @brief		指定位置の子ノードを得る
	//! @param		n		指定位置
	//! @return		指定位置の子ノード
	virtual tInputPin * GetChildAt(risse_size n);

	//! @brief		指定位置に子ノードを設定する
	//! @param		n		指定位置
	//! @param		node	子ノード
	virtual void SetChildAt(risse_size n, tProcessNode * node);

	//! @brief		指定位置に子ノード用ピンを挿入する
	//! @param		n		指定位置
	virtual void InsertChildPinAt(risse_size n);

	//! @brief		指定位置から子ノード用ピンを削除する
	//! @param		n		指定位置
	virtual void DeleteChildPinAt(risse_size n);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}

