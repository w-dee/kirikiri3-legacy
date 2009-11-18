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
//! @brief RINA ラスタ画像用ソース
//---------------------------------------------------------------------------
#ifndef RINAIMAGESOURCE_H
#define RINAIMAGESOURCE_H

#include "risa/packages/risa/graphic/rina/rinaIdRegistry.h"
#include "risa/packages/risa/graphic/rina/rinaQueue.h"
#include "risa/packages/risa/graphic/rina/rinaTypes.h"
#include "risa/packages/risa/graphic/rina/rinaPin.h"
#include "risa/packages/risa/graphic/rina/rinaImageEdge.h"
#include "risa/packages/risa/graphic/image/ImageBuffer.h"

namespace Risa {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * ラスタ画像用ソースノード
 */
class tImageSourceNodeInstance : public tNodeInstance
{
	typedef tNodeInstance inherited; //!< 親クラス

	tInputPinArrayInstance * InputPinArrayInstance; //!< 入力ピン配列インスタンス
	tOutputPinArrayInstance * OutputPinArrayInstance; //!< 出力ピン配列インスタンス
	tImageOutputPinInstance * OutputPinInstance; //!< 出力ピンインスタンス

	tImageBuffer * ImageBuffer; //!< イメージバッファ

public:
	/**
	 * コンストラクタ
	 */
	tImageSourceNodeInstance();

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tImageSourceNodeInstance() {}

public: // サブクラスで実装すべき物
	/**
	 * 入力ピンの配列を得る
	 * @return	入力ピンの配列
	 */
	virtual tInputPinArrayInstance & GetInputPinArrayInstance();

	/**
	 * 出力ピンの配列を得る
	 * @return	出力ピンの配列
	 */
	virtual tOutputPinArrayInstance & GetOutputPinArrayInstance();

public:
	/**
	 * コマンドキューの組み立てを行う
	 * @param builder	キュービルダーオブジェクト
	 */
	virtual void BuildQueue(tQueueBuilder & builder);

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * "ImageSource" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tImageSourceNodeClass, tClassBase, tImageSourceNodeInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
}

#endif
