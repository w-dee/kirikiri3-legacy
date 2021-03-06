//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA ラスタ画像用ソース
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/graphic/rina/rinaImageSource.h"
#include "risa/packages/risa/graphic/rina/rinaPin.h"
#include "risa/common/RisaThread.h"
#include "risseNativeBinder.h"
#include "risseObjectClass.h"
#include "risseStaticStrings.h"
#include "risa/common/RisseEngine.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(7573,48746,61484,31356,16289,41410,60513,41447);
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
/**
 * イメージソースのラスタ画像用のコマンドキュー
 */
class tImageSourceQueueNode : public tImageQueueNode
{
public:
	typedef tImageQueueNode inherited;
private:

	tGCReferencePtr<tImageBuffer> * ImageBuffer; //!< 親キューノードに返すイメージバッファ

public:
	/**
	 * コンストラクタ
	 * @param	im		親キューノードに返すイメージバッファ
	 */
	tImageSourceQueueNode(tGCReferencePtr<tImageBuffer> * im) :
		inherited(), ImageBuffer(im) {;}

public: //!< サブクラスでオーバーライドして使う物
	/**
	 * 画像を得る
	 * @return	画像
	 */
	virtual tGCReferencePtr<tImageBuffer> * GetImageBuffer()
	{
		return ImageBuffer;
	}

	/**
	 * 画像のオフセットを得る
	 * @return	オフセット
	 * GetImageBufferNoAddRef() で得られた画像の左上が、
	 * 要求された座標系におけるどの点なのかを表す。
	 */
	virtual t2DPoint GetOffset()
	{
		t2DPoint zero;
		zero.X = zero.Y = 0;
		return zero;
	}

protected: //!< サブクラスでオーバーライドして使う物
	/**
	 * ノードの処理の最初に行う処理
	 */
	virtual void BeginProcess() {;}

	/**
	 * ノードの処理の最後に行う処理
	 */
	virtual void EndProcess() {;}
};
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tImageSourceNodeInstance::tImageSourceNodeInstance()
{
	InputPinArrayInstance = NULL;
	OutputPinArrayInstance = NULL;
	OutputPinInstance = NULL;
	ImageBuffer = new tGCReferencePtr<tImageBuffer>;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPinArrayInstance & tImageSourceNodeInstance::GetInputPinArrayInstance()
{
	return *InputPinArrayInstance;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPinArrayInstance & tImageSourceNodeInstance::GetOutputPinArrayInstance()
{
	return *OutputPinArrayInstance;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageSourceNodeInstance::BuildQueue(tQueueBuilder & builder)
{
	// キューノードを作成する
	// 子ノードがある場合は子ノードをpushするが、ImageSourceに子ノードはないので
	// それは行わない。

#if 0
	// ImageSource では、親ノードの入力ピンにどういうレンダリング要求が設定されて
	// いても単に画像を親ノードに渡すだけなのでここは何もしない。

	// 出力ピンをたどり、親ノードの入力ピンに設定されているレンダリング要求を確認する
	{
		tEnumerableIterator it(tVariant(OutputPinInstance->GetInputPins()));
		while(it.Next())
		{
			const tRenderRequest * req = it.GetValue().
				ExpectAndGetObjectInterface(
					tClassHolder<tInputPinClass>::instance()->GetClass())->GetRenderRequest();
			// ここでreqをつかってごにょごにょ処理をする
		}
	}
#endif
	// キューノードを作成する
	tImageQueueNode *q = new tImageSourceQueueNode(ImageBuffer);

	// ノードの親を関連付ける
	{
		tEnumerableIterator it(tVariant(OutputPinInstance->GetInputPins()));
		while(it.Next())
		{
			tQueueNode * par = it.GetValue().
				ExpectAndGetObjectInterface(
					tClassHolder<tInputPinClass>::instance()->GetClass())->GetRenderRequest()->GetParentQueueNode();
			// ここでparをつかってごにょごにょ処理をする
			q->AddParent(par);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageSourceNodeInstance::AssignImageInstance(tImageInstance * instance)
{
	// クローンを行う。つまり、これ以降に元画像が変更されてもこのノードの画像には影響しない。
	tImageBuffer * ref = instance->GetBuffer();
	ImageBuffer->set(ref->Clone());
	ref->Release();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageSourceNodeInstance::construct()
{
	// 入力ピンインスタンスを作成
	OutputPinInstance =
		tClassHolder<tImageOutputPinClass>::instance()->GetClass()->
			Invoke(ss_new).
		ExpectAndGetObjectInterface(
		tClassHolder<tImageOutputPinClass>::instance()->GetClass()
		);
	// 入力ピン配列と出力ピン配列を生成
	InputPinArrayInstance =
			tClassHolder<tInputPinArrayClass>::instance()->GetClass()->
				Invoke(ss_new, tVariant(this)).
			ExpectAndGetObjectInterface(
			tClassHolder<tInputPinArrayClass>::instance()->GetClass()
			);
	OutputPinArrayInstance =
			tClassHolder<tOneOutputPinArrayClass>::instance()->GetClass()->
				Invoke(ss_new, tVariant(this), tVariant(OutputPinInstance)).
			ExpectAndGetObjectInterface(
			tClassHolder<tOneOutputPinArrayClass>::instance()->GetClass()
			);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageSourceNodeInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass(info.args); // 引数はそのまま渡す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageSourceNodeInstance::assignImage(const Risse::tVariant & src)
{
	AssignImageInstance(src.ExpectAndGetObjectInterface(
		tClassHolder<tImageClass>::instance()->GetClass()));
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tImageSourceNodeClass,
		(tSS<'I','m','a','g','e','S','o','u','r','c','e'>()),
		tClassHolder<tNodeClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * ImageSourceNode クラスレジストラ
 */
template class tClassRegisterer<
tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tImageSourceNodeClass>;
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
}
