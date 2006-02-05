//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログ管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "Log.h"
#include "risseError.h"
#include "LogViewer.h"

RISSE_DEFINE_SOURCE_ID(53503,8125,25269,17586,20367,40881,26023,16793);


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaLogger::tRisaLogger() : Buffer(MaxLogItems)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaLogger::~tRisaLogger()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		全てのログをビューアに送る
//---------------------------------------------------------------------------
void tRisaLogger::SendAllLogsToLogViewer()
{
/*
	size_t num_logs = Buffer.GetDataSize();

	LogViewer->StopUpdate(); // LogViewer の表示更新を停止
	for(size_t i = 0; i < num_logs; i++)
	{
		LogViewer->AddItem(
	}
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログを記録する
//! @param		content		ログの内容
//! @param		type		ログのタイプ
//! @param		linkinfo	リンク情報
//---------------------------------------------------------------------------
void tRisaLogger::Log(const ttstr & content,
	tRisaLogger::tItem::tType type,
	const ttstr & linkinfo)
{
	tItem & item = Buffer.GetLast();
	item.Timestamp = wxDateTime::Now();
	item.Content = content;
	item.Link = linkinfo;
	item.Type = type;
	Buffer.AdvanceWritePosWithDiscard();
}
//---------------------------------------------------------------------------


