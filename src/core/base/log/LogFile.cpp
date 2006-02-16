//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログファイル
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/log/Log.h"
#include "base/log/LogFile.h"

RISSE_DEFINE_SOURCE_ID(21694,41961,63193,16833,55703,53629,46747,60830);

/*
	tRisaLogFile はシングルトンオブジェクトとしてシステム内に常駐するが、
	実際にファイルにログを採るようになるのは Begin メソッドを呼んだあとだけ
	となる。
*/


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaLogFile::tRisaLogFile() : Receiver(*this)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaLogFile::~tRisaLogFile()
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	if(LogFile.IsOpened())
		tRisaLogger::instance()->UnregisterReceiver(&Receiver);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		行を一行出力する
//! @param		str  行 (EOLはこのメソッドが自動的に出力する)
//---------------------------------------------------------------------------
void tRisaLogFile::OutputOneLine(const ttstr & str)
{
	// UTF-8 に変換する
	size_t utf8_len = str.GetUtf8Length();
	if(utf8_len == static_cast<size_t>(-1L))
		return; // ログ不可(実際にはUTF32がUTF8に変換できないことはないのでこれはあり得ない)

	char * utf8_buf = new char [utf8_len + 1 + 2]; // +2 = CR+LF分の余裕
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
		delete [] utf8_buf;
		throw;
	}
	delete [] utf8_buf;

	// ファイルを flush する
	LogFile.Flush();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログが追加されるとき
//! @param		item  ログアイテム
//---------------------------------------------------------------------------
void tRisaLogFile::OnLog(const tRisaLogger::tItem & item)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	if(!LogFile.IsOpened()) return;

	// 出力形式は
	// 時刻 ログレベル メッセージ

	// 時刻
	ttstr logline = ttstr(item.Timestamp.Format()) + RISSE_WS(" ");

	// ログレベル
	switch(item.Level)
	{
	case tRisaLogger::llDebug:
		logline += RISSE_WS("[D] "); break; // [D]ebug
	case tRisaLogger::llInfo:
		logline += RISSE_WS("[I] "); break; // [I]nformation
	case tRisaLogger::llNotice:
		logline += RISSE_WS("[N] "); break; // [N]otice
	case tRisaLogger::llWarning:
		logline += RISSE_WS("[W] "); break; // [W]arning
	case tRisaLogger::llError:
		logline += RISSE_WS("[E] "); break; // [E]rror
	case tRisaLogger::llCritical:
		logline += RISSE_WS("[C] "); break; // [C]ritical
	}

	// メッセージ
	logline += item.Content;

	OutputOneLine(logline);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログのファイルへの記録を開始する
//---------------------------------------------------------------------------
void tRisaLogFile::Begin()
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	// すでに Begin している場合は戻る
	if(LogFile.IsOpened()) return;

	// ログファイル名を決定
	wxString logfilename = wxString(wxTheApp->argv[0]) + wxT(".console.log");

	// ログファイルを開く
	LogFile.Open(logfilename, wxFile::write);
	if(!LogFile.IsOpened()) return;

	// レシーバを登録
	tRisaLogger::instance()->UnregisterReceiver(&Receiver);

	// ログファイルの最後に移動
	wxFileOffset write_start = LogFile.SeekEnd();

	// セパレータを出力
	if(write_start != 0)
		for(int i = 0; i < 10; i++) OutputOneLine(RisseEmptyString);

	const risse_char * sep = 
	RISSE_WS(
		"============================================================================="
		);

	for(int i = 0; i < 2; i++) OutputOneLine(sep);

	OutputOneLine(RisseEmptyString);

	// LastLogを出力
	tRisaLogger::instance()->SendLogs(&Receiver, NumLastLog);

	// セパレータを出力
	const risse_char * sep2 = 
	RISSE_WS(
		"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
		);
	OutputOneLine(sep2);
	OutputOneLine(RisseEmptyString);

}
//---------------------------------------------------------------------------

