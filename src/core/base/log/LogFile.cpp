//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログファイル
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/log/Log.h"
#include "base/log/LogFile.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(21694,41961,63193,16833,55703,53629,46747,60830);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tLogFile::tLogFile() : Receiver(*this)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tLogFile::~tLogFile()
{
	volatile tCriticalSection::tLocker holder(CS);

	if(LogFile.IsOpened())
		tLogger::instance()->UnregisterReceiver(&Receiver);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tLogFile::OutputOneLine(const tString & str)
{
/*
	TODO: handle this
	// UTF-8 に変換する
	size_t utf8_len = str.GetUtf8Length();
	if(utf8_len == static_cast<size_t>(-1L))
		return; // ログ不可(実際にはUTF32がUTF8に変換できないことはないのでこれはあり得ない)

	char * utf8_buf = new (PointerFreeGC) char [utf8_len + 1 + 2]; // +2 = CR+LF分の余裕
	try
	{
		str.GetUtf8String(utf8_buf);

		// 改行コードを書き込む
		#if defined(__WXMSW__) || defined(__WXPM__)
			utf8_buf[utf8_len  ] = '\r';
			utf8_buf[utf8_len+1] = '\n';
			utf8_buf[utf8_len+2] = '\0';
			utf8_len += 2;
		#elif defined(__WXMAC__) && !defined(__DARWIN__)
			utf8_buf[utf8_len  ] = '\r';
			utf8_buf[utf8_len+1] = '\0';
			utf8_len += 1;	
		#else
			utf8_buf[utf8_len  ] = '\n';
			utf8_buf[utf8_len+1] = '\0';
			utf8_len += 1;
		#endif

		// ファイルに書き込む
		LogFile.Write(utf8_buf, utf8_len);
	}
	catch(...)
	{
		delete (PointerFreeGC) [] utf8_buf;
		throw;
	}
	delete (PointerFreeGC) [] utf8_buf;

	// ファイルを flush する
	LogFile.Flush();
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tLogFile::OnLog(const tLogger::tItem & item)
{
	volatile tCriticalSection::tLocker holder(CS);

	if(!LogFile.IsOpened()) return;

	// 出力形式は
	// 時刻 ログレベル メッセージ

	// 時刻
	tString logline = tString(item.Timestamp.Format()) + RISSE_WS(" ");

	// ログレベル
	switch(item.Level)
	{
	case tLogger::llDebug:
		logline += RISSE_WS("[D] "); break; // [D]ebug
	case tLogger::llInfo:
		logline += RISSE_WS("[I] "); break; // [I]nformation
	case tLogger::llNotice:
		logline += RISSE_WS("[N] "); break; // [N]otice
	case tLogger::llWarning:
		logline += RISSE_WS("[W] "); break; // [W]arning
	case tLogger::llError:
		logline += RISSE_WS("[E] "); break; // [E]rror
	case tLogger::llRecord:
		logline += RISSE_WS("[R] "); break; // [R]ecord
	case tLogger::llCritical:
		logline += RISSE_WS("[C] "); break; // [C]ritical
	}

	// メッセージ
	logline += item.Content;

	OutputOneLine(logline);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tLogFile::Begin()
{
	volatile tCriticalSection::tLocker holder(CS);

	// すでに Begin している場合は戻る
	if(LogFile.IsOpened()) return;

	// ログファイル名を決定
	wxString logfilename = wxString(wxTheApp->argv[0]) + wxT(".console.log");

	// ログファイルを開く
	LogFile.Open(logfilename, wxFile::write);
	if(!LogFile.IsOpened()) return;

	// レシーバを登録
	tLogger::instance()->UnregisterReceiver(&Receiver);

	// ログファイルの最後に移動
	wxFileOffset write_start = LogFile.SeekEnd();

	// セパレータを出力
	if(write_start != 0)
		for(int i = 0; i < 10; i++) OutputOneLine(tString::GetEmptyString());

	const risse_char * sep = 
	RISSE_WS(
		"============================================================================="
		);

	for(int i = 0; i < 2; i++) OutputOneLine(sep);

	OutputOneLine(tString::GetEmptyString());

	// LastLogを出力
	tLogger::instance()->SendPreservedLogs(&Receiver);
	tLogger::instance()->SendLogs(&Receiver, NumLastLog);

	// セパレータを出力
	const risse_char * sep2 = 
	RISSE_WS(
		"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
		);
	OutputOneLine(sep2);
	OutputOneLine(tString::GetEmptyString());

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

