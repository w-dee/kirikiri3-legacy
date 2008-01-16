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
tWideTextDrawDeviceNode::tWideTextDrawDeviceNode() : inherited()
{
	// 最初は全領域が dirty
	DirtyRegion.Add(t1DArea(0, tWideTextDrawDeviceQueueNode::CanvasSize));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextDrawDeviceNode::GetOutputPinCount()
{
	return 0; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tWideTextDrawDeviceNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextDrawDeviceNode::GetInputPinCount()
{
	return InputPins.size();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tWideTextDrawDeviceNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	return InputPins[n];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::InsertInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	tWideTextInputPin * newpin = new tWideTextMixerInputPin();
	newpin->Attach(this);
	InputPins.insert(InputPins.begin() + n, newpin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	InputPins.erase(InputPins.begin() + n);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::BuildQueue(tQueueBuilder & builder)
{
	tQueueNode * new_parent = new tWideTextDrawDeviceQueueNode(NULL);

	// テスト用に 2 文字ずつレンダリングを行うようにする
	// まず入力ピンにレンダリング世代を設定し、レンダリング要求をクリアする
	// また、次に処理すべきノードとして、入力ピンの先の出力ピンのそのまた先の
	// ノードを push する
	for(gc_vector<tInputPin *>::iterator i = InputPins.begin(); i != InputPins.end(); i++)
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
		for(gc_vector<tInputPin *>::iterator i = InputPins.begin(); i != InputPins.end(); i++)
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

	// キャンバスを空白で埋める
	for(risse_size i = 0; i < CanvasSize; i++) Canvas[i] = RISSE_WC(' ');
	Canvas[CanvasSize] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceQueueNode::EndProcess()
{
	// 子ノードを合成する
	for(tChildren::iterator i = Children.begin(); i != Children.end(); i++)
	{
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
			if(pbuf[src_idx] != RISSE_WC(' '))
				Canvas[dest_idx] = pbuf[src_idx];
		}

	wxFprintf(stdout, wxT("result: \"%s\"\n"), tString(Canvas).AsWxString().c_str());

	}

	// 結果を出力する
	wxFprintf(stdout, wxT("%s\n"), tString(Canvas).AsWxString().c_str());
}
//---------------------------------------------------------------------------

}
