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
//! @brief テスト用のテキスト描画デバイスプロセスノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaWideTextDrawDeviceNode.h"
#include "rinaWideTextEdge.h"
#include "rinaWideTextProviderNode.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(10207,53962,31748,17392,1438,46335,5173,19226);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tPinDescriptor tWideTextDrawDeviceNode::Descriptor(
	RISSE_WS("input %1"), RISSE_WS_TR("Input Pin %1") );
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tWideTextDrawDeviceNode::tWideTextDrawDeviceNode(tGraph * graph) :
	inherited(graph),
	InputPins(this, Descriptor),
	OutputPins(this)
{
	// 最初は全領域が dirty
	DirtyRegion.Add(t1DArea(0, tWideTextDrawDeviceQueueNode::CanvasSize));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::BuildQueue(tQueueBuilder & builder)
{
	RISSE_ASSERT_CS_LOCKED(GetGraph()->GetCS());

	tQueueNode * new_parent = new tWideTextDrawDeviceQueueNode(NULL);

	// まず入力ピンにレンダリング世代を設定し、レンダリング要求をクリアする
	// また、次に処理すべきノードとして、入力ピンの先の出力ピンのそのまた先の
	// ノードを push する
	typedef tArrayPins<tWideTextMixerInputPin>::tArray inputarray_t;
	inputarray_t & inputarray = InputPins.GetPins();
	for(inputarray_t::iterator i = inputarray.begin();
		i != inputarray.end(); i++)
	{
		(*i)->SetRenderGeneration(builder.GetRenderGeneration());
		(*i)->ClearRenderRequests();
		builder.Push((*i)->GetOutputPin()->GetNode());
	}

	// Dirty な領域ごとに
	const t1DRegion::tAreas & dirties = DirtyRegion.GetAreas();
	risse_size index = 0;
	for(t1DRegion::tAreas::const_iterator ai = dirties.begin(); ai != dirties.end(); ai++)
	{
		// 入力ピンに再帰
		for(inputarray_t::iterator i = inputarray.begin();
			i != inputarray.end(); i++)
		{
			tWideTextMixerRenderRequest * req =
				new tWideTextMixerRenderRequest(new_parent, index, *ai,
					(Risa::DownCast<tWideTextMixerInputPin*>(*i))->GetInheritableProperties());
			index ++;
			(*i)->AddRenderRequest(req);
		}
	}

	// Dirty な領域をクリア
	DirtyRegion.Clear();

	// ルートのキューノードはnew_parentであると主張(ここは後から修正するかも………)
	builder.SetRootQueueNode(new_parent);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::NotifyUpdate(const t1DArea & area)
{
	RISSE_ASSERT_CS_LOCKED(GetGraph()->GetCS());

	DirtyRegion.Add(area);
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tWideTextDrawDeviceQueueNode::tWideTextDrawDeviceQueueNode(
				tWideTextRenderRequest * request) : inherited(request)
{
	Canvas = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceQueueNode::BeginProcess()
{
	// キャンバス用にメモリを確保
	Canvas = (risse_char *)MallocAtomicCollectee(sizeof(risse_char) * (CanvasSize + 1));

	// キャンバスをドットで埋める
	for(risse_size i = 0; i < CanvasSize; i++) Canvas[i] = RISSE_WC('.');
	Canvas[CanvasSize] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceQueueNode::EndProcess()
{
	// 子ノードを合成する
	for(tChildren::iterator i = Children.begin(); i != Children.end(); i++)
	{
		if(! i->GetChild()) continue;
		tWideTextQueueNode * provider = Risa::DownCast<tWideTextQueueNode *>(i->GetChild());
		const tWideTextMixerRenderRequest * req =
			static_cast<const tWideTextMixerRenderRequest*>(i->GetRenderRequest());
		const tString & text = provider->GetText();
		risse_offset text_offset = provider->GetOffset();
		const risse_char *pbuf = text.c_str();
		risse_size text_size = text.GetLength();
		risse_int32 pos = req->GetInheritableProperties().GetPosition();
		const t1DArea & destarea = req->GetArea();
		const t1DArea & srcarea = provider->GetArea();

		// position で表された位置 + destarea.Start に、
		// pbuf で表されたテキスト + text_offset から srcarea.GetLength() 分の
		// 長さを書き込む。
		// dest に範囲外が指定された場合は明らかなエラーだが、src については
		// 範囲外を含む場合は値が範囲外になっている可能性があるので補正する
		// ここで destarea は親ノードが子ノードに対して要求した範囲であり、
		// srcarea はそれに対して子ノードがどう反応したかである

		// destarea と srcarea の重なっている部分しか転送のしようがないので交差を得る
		t1DArea intersect;
		if(!destarea.Intersect(srcarea, intersect)) continue; // 交差していない

		text_offset += intersect.GetStart() - srcarea.GetStart();

	wxFprintf(stdout, wxT("\"%s\": pos:%d, destarea:(%d,%d), srcarea:(%d,%d), text_offset:%d\n"),
			tString(text).AsWxString().c_str(),
			(int)pos,
			(int)destarea.GetStart(),
			(int)destarea.GetEnd(),
			(int)srcarea.GetStart(),
			(int)srcarea.GetEnd(),
			(int)text_offset
			);
	wxFprintf(stdout, wxT("\"%s\": intersect:(%d,%d), text_offset:%d\n"),
			tString(text).AsWxString().c_str(),
			(int)intersect.GetStart(),
			(int)intersect.GetEnd(),
			(int)text_offset
			);


		// 転送先範囲がキャンバスサイズに収まっているかどうか
		RISSE_ASSERT(intersect.GetStart() + pos >= 0);
		RISSE_ASSERT(intersect.GetEnd() + pos <= static_cast<risse_offset>(CanvasSize));

		// 転送元が範囲内に収まっているか
		RISSE_ASSERT(text_offset >= 0);
		RISSE_ASSERT(text_offset + intersect.GetLength() <= static_cast<risse_offset>(text_size));

		risse_size length = intersect.GetLength();
		for(risse_size i = 0 ; i < length; i++)
		{
			risse_size src_idx = i + text_offset;
			risse_size dest_idx = i + pos + intersect.GetStart();
			Canvas[dest_idx] = pbuf[src_idx];
		}

	wxFprintf(stdout, wxT("result: \"%s\"\n"), tString(Canvas).AsWxString().c_str());

	}

	// 結果を出力する
	wxFprintf(stdout, wxT("DrawDevice output: %s\n"), tString(Canvas).AsWxString().c_str());
}
//---------------------------------------------------------------------------

}
