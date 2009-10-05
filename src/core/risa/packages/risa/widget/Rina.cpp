//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Rina 表示領域クラス
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/widget/Rina.h"
#include "risa/packages/risa/graphic/image/Image.h"


#include <gdk/gdk.h>


namespace Risa {
RISSE_DEFINE_SOURCE_ID(58627,32079,6056,17748,10429,30722,59446,14940);
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * Rina widget用のコマンドキューノード
 */
class tRinaWidgetQueueNode : public tQueueNode, public Risa::tSubmorph<tRinaWidgetQueueNode>
{
public:
	typedef tQueueNode inherited;
private:

public:
	/**
	 * コンストラクタ
	 */
	tRinaWidgetQueueNode() :
		inherited() {;}

protected: //!< サブクラスでオーバーライドして使う物

	/**
	 * ノードの処理の最初に行う処理
	 */
	virtual void BeginProcess() = 0;

	/**
	 * ノードの処理の最後に行う処理
	 */
	virtual void EndProcess()
	{
		// TODO: ここに描画処理を
	}

};
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tRinaWidgetNodeInstance::tRinaWidgetNodeInstance()
{
	InputPinArrayInstance = NULL;
	OutputPinArrayInstance = NULL;
	InputPinInstance = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPinArrayInstance & tRinaWidgetNodeInstance::GetInputPinArrayInstance()
{
	return *InputPinArrayInstance;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPinArrayInstance & tRinaWidgetNodeInstance::GetOutputPinArrayInstance()
{
	return *OutputPinArrayInstance;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaWidgetNodeInstance::BuildQueue(tQueueBuilder & builder)
{
	// キューを組み立てる
	// 入力ピンのレンダリング要求に更新領域を設定する
	// TODO: ちゃんとした更新領域の設定

	// まずtTexturePolygon作成
	tTexturePolygon poly = {
			{ 100.0f - 0.5f, 0.0f   - 0.5f },
			{ 0.0f   - 0.5f, 0.0f   - 0.5f },
			{ 0.0f   - 0.5f, 100.0f - 0.5f },
			false
	};

	// tTexturePolygonList 作成
	tTexturePolygonList poly_list;
	poly_list.push_back(poly);

	// キューノードを作成する TODO
	tQueueNode * queue = new tRinaWidgetQueueNode();

	// レンダリング要求の作成
	tRenderRequest * req = new tImageRenderRequest(queue, poly_list);

	// レンダリング要求を入力ピンに設定する
	InputPinInstance->SetRenderRequest(req);

	// 子ノードをbuilderにpushする
	builder.Push(InputPinInstance->GetNodeInstance());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaWidgetNodeInstance::construct()
{
	// 入力ピンインスタンスを作成
	InputPinInstance =
		tClassHolder<tImageInputPinClass>::instance()->GetClass()->
			Invoke(ss_new).
		ExpectAndGetObjectInterface(
		tClassHolder<tImageInputPinClass>::instance()->GetClass()
		);
	// 入力ピン配列と出力ピン配列を生成
	InputPinArrayInstance =
			tClassHolder<tOneInputPinArrayClass>::instance()->GetClass()->
				Invoke(ss_new, tVariant(this), tVariant(InputPinInstance)).
			ExpectAndGetObjectInterface(
			tClassHolder<tOneInputPinArrayClass>::instance()->GetClass()
			);
	OutputPinArrayInstance =
			tClassHolder<tOutputPinArrayClass>::instance()->GetClass()->
				Invoke(ss_new, tVariant(this)).
			ExpectAndGetObjectInterface(
			tClassHolder<tOutputPinArrayClass>::instance()->GetClass()
			);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaWidgetNodeInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass(info.args); // 引数はそのまま渡す
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tRinaWidgetNodeClass,
		(tSS<'R','i','n','a','W','i','d','g','e','t','N','o','d','e'>()),
		tClassHolder<tNodeClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * RinaWidgetNode クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','w','i','d','g','e','t'>,
	tRinaWidgetNodeClass>;
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
/**
 * Rinaの表示領域を表す wxControl 派生クラス用のイベントテーブル
 */
BEGIN_EVENT_TABLE(tRina, wxControl)
	EVT_PAINT(tRina::OnPaint)
END_EVENT_TABLE()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRina::tRina(tRinaInstance * instance, wxWindow * parent) :
	inherited(parent, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
	tBehavior(this, instance)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRina::~tRina()
{
	// wxFrame や wxDialog 以外はこっちでウィンドウ破棄をハンドリングする

	fprintf(stderr, "tRina::~tRina()\n");
	fflush(stderr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRina::OnPaint(wxPaintEvent& event)
{
#if 0
	wxPaintDC dc(this);

	const tImageBuffer::tDescriptor & desc = TestImage->GetDescriptor();
	const tImageBuffer::tBufferPointer & pointer = TestImage->GetBufferPointer();

	gdk_draw_rgb_32_image(dc.m_window, dc.m_penGC, 0, 0,
			desc.Width, desc.Height, GDK_RGB_DITHER_NONE,
			(guchar*)pointer.Buffer, pointer.Pitch);

	pointer.Release();
#endif

	// キュービルダーを作成する
	tRinaWidgetNodeInstance * node =
				GetInternal()->GetInstance()->GetRinaWidgetNodeInstance();
	tGraphInstance * graph = node->GetGraphInstance();
	tQueueBuilder builder(graph);

	// キューノードを作成
	builder.Build(node);

	// 描画処理を実行
	tQueueNode * root = builder.GetRootQueueNode();
	tQueue queue;
	queue.Process(root);

	// ルートキューノードから画像バッファを取り出し、描画
	tImageQueueNode * image_queue = DownCast<tImageQueueNode*>(root);
	const tImageBuffer::tDescriptor & buffer_desc =
			image_queue->GetImageBufferNoAddRef().GetDescriptor();
	const tImageBuffer::tBufferPointer & buffer_pointer =
			image_queue->GetImageBufferNoAddRef().GetBufferPointer();

	wxPaintDC dc(this);

	gdk_draw_rgb_32_image(dc.m_window, dc.m_penGC, 0, 0,
			buffer_desc.Width, buffer_desc.Height, GDK_RGB_DITHER_NONE,
			(guchar*)buffer_pointer.Buffer, buffer_pointer.Pitch);

	buffer_pointer.Release();
}
//---------------------------------------------------------------------------



























//---------------------------------------------------------------------------
tRinaInstance::tRinaInstance()
{
	Internal = NULL;
	RinaWidgetNode = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaInstance::construct()
{
	// 特にやること無し
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRinaInstance::initialize(const tVariant & parent, const tVariant & graph, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// parent から tFrameInstance のインスタンスを取り出す
	tFrameInstance * frame =
		parent.ExpectAndGetObjectInterface(
			tClassHolder<tFrameClass>::instance()->GetClass());

	// graph から tGraphInstance のインスタンスを取り出す
	tGraphInstance * graph_instance =
		graph.ExpectAndGetObjectInterface(
			tClassHolder<tGraphClass>::instance()->GetClass());

	// Rinaコントロールを作成
	SetWxWindow(new tRina(this, frame->GetWxWindow()));

	// tGraphInstance のインスタンスを引数にして RinaWidgetNode のインスタンスを作成する
	RinaWidgetNode =
		tClassHolder<tRinaWidgetNodeClass>::instance()->GetClass()->Invoke(ss_new, tVariant(graph_instance)).
						ExpectAndGetObjectInterface(
						tClassHolder<tRinaWidgetNodeClass>::instance()->GetClass()
						);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tRinaInstance::get_node()
{
	return RinaWidgetNode;
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tRinaClass, (tSS<'R','i','n','a'>()),
		tClassHolder<tWindowClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
	BindProperty(this, tSS<'n','o','d','e'>(), &tRinaInstance::get_node);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * Rina クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','w','i','d','g','e','t'>,
	tRinaClass>;
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risa


