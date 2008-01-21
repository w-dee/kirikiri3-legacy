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
//! @brief テキストを反転するノード
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaWideTextEdge.h"
#include "rinaWideTextReverserNode.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(47400,47442,57022,18181,27294,2531,6297,13005);
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tWideTextReverserNode::tWideTextReverserNode()
{
	// 出力ピンを作成
	OutputPin = new tWideTextOutputPin();
	OutputPin->Attach(this);
	// 入力ピンを作成
	InputPin = new tWideTextInputPin();
	InputPin->Attach(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextReverserNode::GetOutputPinCount()
{
	return 1; // 出力ピンは１個
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tWideTextReverserNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	if(n == 0) return OutputPin;
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextReverserNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextReverserNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextReverserNode::GetInputPinCount()
{
	return 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tWideTextReverserNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	if(n == 0) return InputPin;
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextReverserNode::InsertInputPinAt(risse_size n)
{
	// XXX: 入力ピンを追加することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextReverserNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 入力ピンを削除することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextReverserNode::BuildQueue(tQueueBuilder & builder)
{
	/*
		本来ならば子ノードにバウンディングボックスをといあわせ、それに
		対応しない位置をはじくようなコードが必要
		(まだ子ノードからそのような情報を得るインターフェースはない)
	*/
	t1DRegion region;

	// リバーサのキューノードを作成する
	tQueueNode * new_parent = new tWideTextReverserQueueNode(NULL);

	// 出力ピンの先に繋がってる入力ピンそれぞれについて
	for(tOutputPin::tInputPins::const_iterator i = OutputPin->GetInputPins().begin();
		i != OutputPin->GetInputPins().end(); i++)
	{
		// レンダリング世代が最新の物かどうかをチェック
		if((*i)->GetRenderGeneration() != builder.GetRenderGeneration()) continue;

		// 入力ピンのタイプをチェック
		RISSE_ASSERT((*i)->GetAgreedType() == WideTextEdgeType);

		// 親を設定
		// すべてのリクエストのすべてのキューノードに同じ子を設定する
		const tInputPin::tRenderRequests & requests = (*i)->GetRenderRequests();
		for(tInputPin::tRenderRequests::const_iterator i =
				requests.begin(); i != requests.end(); i ++)
		{
			new_parent->AddParent(*i);
			const tWideTextRenderRequest * req = Risa::DownCast<const tWideTextRenderRequest*>(*i);

			// region に追加
			region.Add(req->GetArea());
		}
	}

	// 入力ピンにレンダリング世代を設定し、レンダリング要求をクリアする
	// また、次に処理すべきノードとして、入力ピンの先の出力ピンのそのまた先の
	// ノードを push する
	InputPin->SetRenderGeneration(builder.GetRenderGeneration());
	InputPin->ClearRenderRequests();
	builder.Push(InputPin->GetOutputPin()->GetNode());

	// Dirty な領域ごとに
	const t1DRegion::tAreas & dirties = region.GetAreas();
	risse_size index = 0;
	for(t1DRegion::tAreas::const_iterator ai = dirties.begin(); ai != dirties.end(); ai++)
	{
		// 入力ピンに再帰
		tWideTextRenderRequest * req =
			new tWideTextRenderRequest(new_parent, index, *ai);
		index ++;
		InputPin->AddRenderRequest(req);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextReverserNode::NotifyUpdate(const t1DArea & area)
{
	// 反転した位置を出力ノードに対して与える
	t1DArea area_rev(-area.GetEnd(), -area.GetStart());
	OutputPin->NotifyUpdate(area_rev);
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tWideTextReverserQueueNode::tWideTextReverserQueueNode(tWideTextRenderRequest * request) :
	inherited(request, tString(), t1DArea(), 0)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextReverserQueueNode::BeginProcess()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextReverserQueueNode::EndProcess()
{
	// 子ノードの範囲を決定する
	bool first = true;
	for(tChildren::iterator i = Children.begin(); i != Children.end(); i++)
	{
		if(! i->GetChild()) continue;

		tWideTextQueueNode * provider = Risa::DownCast<tWideTextQueueNode *>(i->GetChild());
		if(provider->GetArea().GetLength() <= 0) continue; // 範囲が無効
		const tWideTextMixerRenderRequest * req =
			static_cast<const tWideTextMixerRenderRequest*>(i->GetRenderRequest());
		const t1DArea & destarea = req->GetArea();
		const t1DArea & srcarea = provider->GetArea();
		t1DArea srcarea_rev(-srcarea.GetEnd(), -srcarea.GetStart());
		t1DArea intersect;
		if(!destarea.Intersect(srcarea_rev, intersect)) continue; // 交差していない

		if(first)
			first = false, Area = intersect;
		else
			Area.Unite(intersect);
	}
	if(first)
	{
		// 子ノードが無い？？
		Text = tString();
		Area = t1DArea(0, 0);
		Offset = 0;
		return;
	}

	RISSE_ASSERT(Area.GetLength() > 0);
	risse_size area_size = Area.GetLength();
	risse_char * buf = Text.Allocate(area_size);
	for(risse_size i = 0; i < area_size; i++) buf[i] = RISSE_WC(' '); // 空白で埋める
	Offset = 0;

wxFprintf(stdout, wxT("Area: (%d,%d)\n"),
		(int)Area.GetStart(),
		(int)Area.GetEnd() );

	// 子ノードを合成する
	for(tChildren::iterator i = Children.begin(); i != Children.end(); i++)
	{
		if(! i->GetChild()) continue;
		tWideTextQueueNode * provider = Risa::DownCast<tWideTextQueueNode *>(i->GetChild());
		if(provider->GetArea().GetLength() <= 0) continue; // 範囲が無効
		const tWideTextMixerRenderRequest * req =
			static_cast<const tWideTextMixerRenderRequest*>(i->GetRenderRequest());
		const tString & text = provider->GetText();
		risse_offset text_offset = provider->GetOffset();
		const risse_char *pbuf = text.c_str();
		risse_size text_size = text.GetLength();
		const t1DArea & destarea = req->GetArea();
		const t1DArea & srcarea = provider->GetArea();
		t1DArea srcarea_rev(-srcarea.GetEnd(), -srcarea.GetStart());

		// destarea と srcarea_rev の重なっている部分しか転送のしようがないので交差を得る
		t1DArea intersect;
		if(!destarea.Intersect(srcarea_rev, intersect)) continue; // 交差していない

		text_offset += -(intersect.GetEnd() - srcarea_rev.GetEnd());

	wxFprintf(stdout, wxT("\"%s\": destarea:(%d,%d), srcarea:(%d,%d), srcarea_rev:(%d,%d), text_offset:%d\n"),
			tString(text).AsWxString().c_str(),
			(int)destarea.GetStart(),
			(int)destarea.GetEnd(),
			(int)srcarea.GetStart(),
			(int)srcarea.GetEnd(),
			(int)srcarea_rev.GetStart(),
			(int)srcarea_rev.GetEnd(),
			(int)text_offset
			);
	wxFprintf(stdout, wxT("\"%s\": intersect:(%d,%d), text_offset:%d\n"),
			tString(text).AsWxString().c_str(),
			(int)intersect.GetStart(),
			(int)intersect.GetEnd(),
			(int)text_offset
			);

		// 転送元が範囲内に収まっているか
		RISSE_ASSERT(text_offset >= 0);
		RISSE_ASSERT(text_offset + intersect.GetLength() <= static_cast<risse_offset>(text_size));

		risse_size length = intersect.GetLength();
		for(risse_size i = 0 ; i < length; i++)
		{
			risse_size src_idx  = i + text_offset;
			risse_size dest_idx = (intersect.GetStart() - Area.GetStart()) + length - i - 1;
			if(pbuf[src_idx] != RISSE_WC(' '))
				buf[dest_idx] = pbuf[src_idx];
		}

	}

	wxFprintf(stderr, wxT("reversed output : %s\n"), Text.AsWxString().c_str());
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
