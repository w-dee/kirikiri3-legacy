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
//! @brief テスト用のテキストミキサノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaWideTextMixerNode.h"
#include "rinaWideTextEdge.h"
#include "rinaWideTextProviderNode.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(8982,48844,33706,17807,17033,58515,58827,7512);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tWideTextMixerNode::tWideTextMixerNode() : inherited()
{
	// 出力ピンを作成
	OutputPin = new tWideTextOutputPin();
	OutputPin->Attach(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextMixerNode::GetOutputPinCount()
{
	return 1; // 出力ピンは1個
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tWideTextMixerNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	if(n == 0) return OutputPin;
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextMixerNode::GetInputPinCount()
{
	return InputPins.size();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tWideTextMixerNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	return InputPins[n];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::InsertInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	tWideTextInputPin * newpin = new tWideTextMixerInputPin();
	newpin->Attach(this);
	InputPins.insert(InputPins.begin() + n, newpin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	InputPins.erase(InputPins.begin() + n);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::BuildQueue(tQueueBuilder & builder)
{
	// 情報収集; 自分に関係する範囲があるかどうか
	t1DRegion region;
	for(tOutputPin::tInputPins::const_iterator i = OutputPin->GetInputPins().begin();
		i != OutputPin->GetInputPins().end(); i++)
	{
		// レンダリング世代が最新の物かどうかをチェック
		if((*i)->GetRenderGeneration() != builder.GetRenderGeneration()) continue;

		// 入力ピンのタイプをチェック
		RISSE_ASSERT((*i)->GetAgreedType() == WideTextEdgeType);

		// 範囲が重なっているか……
		const tInputPin::tRenderRequests & requests = (*i)->GetRenderRequests();
		for(tInputPin::tRenderRequests::const_iterator i =
			requests.begin(); i != requests.end(); i ++)
		{
			const tWideTextRenderRequest * req = Risa::DownCast<const tWideTextRenderRequest*>(*i);
			t1DArea intersection;
			if(req->GetArea().Intersect(t1DArea(0, tWideTextMixerQueueNode::CanvasSize), intersection))
			{
				// 重なっていれば region に追加
				region.Add(intersection);
			}
		}
	}

	// region が何もない場合はそのまま return
	if(region.IsEmpty())
		return;

	// ミキサのキューノードを作成する
	tQueueNode * new_parent = new tWideTextMixerQueueNode(NULL);

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
			new_parent->AddParent(*i);
	}

	// 入力ピンにレンダリング世代を設定し、レンダリング要求をクリアする
	// また、次に処理すべきノードとして、入力ピンの先の出力ピンのそのまた先の
	// ノードを push する
	for(gc_vector<tInputPin *>::iterator i = InputPins.begin(); i != InputPins.end(); i++)
	{
		(*i)->SetRenderGeneration(builder.GetRenderGeneration());
		(*i)->ClearRenderRequests();
		builder.Push((*i)->GetOutputPin()->GetNode());
	}

	// Dirty な領域ごとに
	const t1DRegion::tAreas & dirties = region.GetAreas();
	risse_size index = 0;
	for(t1DRegion::tAreas::const_iterator ai = dirties.begin(); ai != dirties.end(); ai++)
	{
		// 入力ピンに再帰
		for(gc_vector<tInputPin *>::iterator i = InputPins.begin(); i != InputPins.end(); i++)
		{
			tWideTextMixerRenderRequest * req =
				new tWideTextMixerRenderRequest(new_parent, index, *ai,
					(Risa::DownCast<tWideTextMixerInputPin*>(*i))->GetInheritableProperties());
			index ++;
			(*i)->AddRenderRequest(req);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::NotifyUpdate(const t1DArea & area)
{
	// そのまま出力ピンに情報を渡す
	OutputPin->NotifyUpdate(area);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tWideTextMixerQueueNode::tWideTextMixerQueueNode(tWideTextRenderRequest * request) :
	inherited(request, tString(), t1DArea(), 0)
{
	Canvas = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerQueueNode::BeginProcess()
{
	// キャンバス用にメモリを確保
	Canvas = (risse_char *)MallocAtomicCollectee(sizeof(risse_char) * (CanvasSize + 1));

	// キャンバスを空白で埋める
	for(risse_size i = 0; i < CanvasSize; i++) Canvas[i] = RISSE_WC('.');
	Canvas[CanvasSize] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerQueueNode::EndProcess()
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

	// 結果をTextに格納
	Text = Canvas;
	Area = t1DArea(0, CanvasSize);
	Offset = 0;

	wxFprintf(stderr, wxT("mixed output : %s\n"), Text.AsWxString().c_str());
}
//---------------------------------------------------------------------------

}
