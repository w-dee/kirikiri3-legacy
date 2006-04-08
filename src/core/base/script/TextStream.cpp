//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseテキストストリーム
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/exception/RisaException.h"
#include "base/script/RisseEngine.h"
#include "base/fs/common/FSManager.h"
#include "base/script/TextStream.h"

RISSE_DEFINE_SOURCE_ID(32746,62447,35688,17903,39609,14867,42943,32667);

//---------------------------------------------------------------------------
//! @brief		iRisseTextReadStream の実装
//---------------------------------------------------------------------------
class tRisaTextReadStream : public iRisseTextReadStream
{
	static const size_t BufferSize = 2048; //!< バッファとして確保するコードポイント数

	tRisseBinaryStream * Stream; //!< 入力用バイナリストリーム

	risse_char Buffer[BufferSize + 1];  //!< 入力バッファ
	risse_uint BufferReadPos; //!< バッファの読み込み位置
	risse_uint BufferRemain; //!< 入力バッファの残り

	//! @brief エンコーディング種別
	enum tEncoding
	{
		eUnknown,
		eUTF16LE, eUTF16BE,
		eUTF32LE, eUTF32BE,
		eUTF8
	};
	tEncoding Encoding; //!< エンコーディング

	ttstr FileName; //!< ファイル名

public:
	tRisaTextReadStream(const ttstr & name, const ttstr & modestr);
	virtual ~tRisaTextReadStream();
	risse_uint Read(tRisseString & targ, risse_uint size);

private:
	void ReadBuffer();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		name		入力ファイル名
//! @param		modestr		モード文字列
//---------------------------------------------------------------------------
tRisaTextReadStream::tRisaTextReadStream(const ttstr & name, const ttstr & modestr)
{
	// フィールドの初期化
	Stream = NULL;
	BufferRemain = 0;
	FileName = name;

	// ストリームを開く
	Stream = tRisaFileSystemManager::instance()->CreateStream(name, RISSE_BS_READ);

	// 'o' モードをチェック
	risse_uint64 ofs = 0;
	const risse_char * o_ofs;
	o_ofs = Risse_strchr(modestr.c_str(), RISSE_WC('o'));
	if(o_ofs != NULL)
	{
		// 指定位置にシークする
		// ここでは、いったん別のバッファにコピーした後それを
		// ttstr に変換して Integer に変換する…という方法をとる。
		// (速度的にクリティカルではないのでこれで十分だと思う)
		o_ofs++;
		risse_char buf[256];
		int i;
		for(i = 0; i < 255; i++)
		{
			if(o_ofs[i] >= RISSE_WC('0') && o_ofs[i] <= RISSE_WC('9'))
				buf[i] = o_ofs[i];
			else break;
		}
		buf[i] = 0;
		ofs = ttstr(buf).AsInteger();
		Stream->SetPosition(ofs);
	}

	// ファイル先頭のシグニチャをチェックする
	try
	{
		// ISO/IEC10646 における「The use of "signatures"
		//  to identify UCS」によれば
		// "signatures" は最大 4 バイトである
		tEncoding enc = eUTF8;
		risse_uint8 mark[4] = {0xcc,0xcc,0xcc,0xcc};
			// 0xcc= シグニチャの他のバイトと見間違えないテキトーな値
		Stream->Read(mark, 4);

		if(mark[0] == 0xfe && mark[1] == 0xff)
		{
			// UCS-2/UTF-16BE
			enc = eUTF16BE;
			Stream->SetPosition(ofs + 2);
		}
		else if(mark[0] == 0xff && mark[1] == 0xfe)
		{
			// UCS-2/UTF-16LE
			enc = eUTF16LE;
			Stream->SetPosition(ofs + 2);
		}
		else if(mark[0] == 0x00 && mark[1] == 0x00 &&
			mark[2] == 0xfe && mark[3] == 0xff)
		{
			// UCS-4/UTF-32BE
			enc = eUTF32BE;
			Stream->SetPosition(ofs + 4);
		}
		else if(mark[0] == 0xff && mark[1] == 0xfe &&
			mark[2] == 0x00 && mark[3] == 0x00)
		{
			// UCS-4/UTF-32LE
			enc = eUTF32LE;
			Stream->SetPosition(ofs + 4);
		}
		else if(mark[0] == 0xef && mark[1] == 0xbb &&
			mark[2] == 0xbf)
		{
			// UTF-8
			enc = eUTF8;
			Stream->SetPosition(ofs + 3);
		}
		Encoding = enc;

		// 現時点でサポートをしているのは UTF-8 の読み込みのみ
		if(enc != eUTF8)
			eRisaException::Throw(RISSE_WS_TR("can not read text file: unknown or unsupported encoding in text file %1"), FileName);
	}
	catch(...)
	{
		delete Stream; Stream = NULL;
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaTextReadStream::~tRisaTextReadStream()
{
	if(Stream) delete Stream;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ストリームから読み込む
//! @param		targ 読み込み先
//! @param		読み込むサイズ (コードポイント数)  0 = 全部
//! @return		読み込まれたコードポイント数
//---------------------------------------------------------------------------
risse_size tRisaTextReadStream::Read(tRisseString & targ, risse_size size)
{
	if(size != 0)
	{
		// 指定サイズだけ読み出す
		risse_char *buf = targ.AllocBuffer(size);
		size_t i = 0;
		while(size > 0)
		{
			if(BufferRemain == 0) ReadBuffer();
			if(BufferRemain == 0)
			{
				buf[i] = 0;
				break;
			}
			size_t one_copy = BufferRemain > size ? size: BufferRemain;
			memcpy(buf + i, Buffer, sizeof(risse_char) * one_copy);
			BufferRemain -= one_copy;
		}
		targ.FixLen();
	}
	else
	{
		// 全部読み出す
		targ.Clear();

		size_t written = 0;

		while(true)
		{
			if(BufferRemain == 0) ReadBuffer();
			if(BufferRemain == 0) break;
			risse_char * buf = targ.AppendBuffer(BufferRemain);
			memcpy(buf + written, Buffer, sizeof(risse_char) * BufferRemain);
			written += BufferRemain;
			BufferRemain = 0;
		}
		targ.FixLen();
	}
	return targ.GetLen();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		バッファにデータを読み込む
//---------------------------------------------------------------------------
void tRisaTextReadStream::ReadBuffer()
{
	// この関数を読み込む時点では BufferRemain は 0 であること
	BufferReadPos = 0; // BufferReadPos を 0 に

	// エンコーディングに従って処理を行う
	if(Encoding == eUTF8)
	{
		// UTF-8 のように入力のバイト数が可変なエンコーディングでは
		// 事前にバッファのサイズを確認できない。

		// BufferSize - 8 分をとりあえず入力バッファに読み込んでみる。
		// -8 = あとで足りないバイト数を付け足す分の余裕
		unsigned char in[BufferSize];
		size_t read = Stream->Read(in, BufferSize - 8);
		if(read == 0) return; // これ以上読み込めない

		// 最後のコードポイントの UTF-8 におけるバイト数を得る
		int lastcharbytes;
		size_t i;
		for(i = read - 1; i >= 0 && !(in[i] < 0x80 || in[i] >= 0xc3 && in[i] < 0xfe); i--) ;

		if     (in[i] < 0x80) lastcharbytes = 1;
		else if(in[i] < 0xc2) lastcharbytes = 0;
		else if(in[i] < 0xe0) lastcharbytes = 2;
		else if(in[i] < 0xf0) lastcharbytes = 3;
		else if(in[i] < 0xf8) lastcharbytes = 4;
		else if(in[i] < 0xfc) lastcharbytes = 5;
		else if(in[i] < 0xfe) lastcharbytes = 6;
		else                  lastcharbytes = 0;

		// 足りないバイト数を読み足す
		if(lastcharbytes + i > read)
		{
			size_t needed = (lastcharbytes + i) - read;
			Stream->ReadBuffer(in + read, needed);
			read += needed;
		}

		// risse_char に変換する
		in[read] = 0;
		BufferRemain = RisseUtf8ToRisseCharString(reinterpret_cast<char *>(in), Buffer);

		if(BufferRemain == static_cast<size_t>(-1L))
		{
			// 変換に失敗
			BufferRemain = 0;
			eRisaException::Throw(RISSE_WS_TR("can not read text file: invalid UTF-8 charater in file %1"), FileName);
		}

		// この関数ではバッファをすべて満たすことは保証しなくてよいので、ここで帰る
	}
}
//---------------------------------------------------------------------------

























//---------------------------------------------------------------------------
//! @brief		iRisseTextWriteStream の実装
//---------------------------------------------------------------------------
class tRisaTextWriteStream : public iRisseTextWriteStream
{
	tRisseBinaryStream * Stream; //!< ストリーム

public:
	tRisaTextWriteStream(const ttstr & name, const ttstr &modestr);
	virtual ~tRisaTextWriteStream();
	void Write(const ttstr & targ);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaTextWriteStream::tRisaTextWriteStream(const ttstr & name, const ttstr &modestr)
{
	Stream = NULL;

	// 'o' モードをチェック
	risse_uint64 ofs = 0;
	const risse_char * o_ofs;
	o_ofs = Risse_strchr(modestr.c_str(), RISSE_WC('o'));
	if(o_ofs != NULL)
	{
		// 指定位置にシークする
		// ここでは、いったん別のバッファにコピーした後それを
		// ttstr に変換して Integer に変換する…という方法をとる。
		// (速度的にクリティカルではないのでこれで十分だと思う)
		o_ofs++;
		risse_char buf[256];
		int i;
		for(i = 0; i < 255; i++)
		{
			if(o_ofs[i] >= RISSE_WC('0') && o_ofs[i] <= RISSE_WC('9'))
				buf[i] = o_ofs[i];
			else break;
		}
		buf[i] = 0;
		ofs = ttstr(buf).AsInteger();
		Stream = tRisaFileSystemManager::instance()->CreateStream(name, RISSE_BS_UPDATE);
		Stream->SetPosition(ofs);
	}
	else
	{
		Stream = tRisaFileSystemManager::instance()->CreateStream(name, RISSE_BS_WRITE);
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaTextWriteStream::~tRisaTextWriteStream()
{
	if(Stream) delete Stream;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ストリームに書き込む
//! @param		targ		書き込みたい文字列
//---------------------------------------------------------------------------
void tRisaTextWriteStream::Write(const ttstr & targ)
{
	for(;;)
	{

		// UTF-8 に変換したときのサイズを得る
		size_t utf8_len = targ.GetUtf8Length();
		if(utf8_len == static_cast<size_t>(-1L)) break; // 変換に失敗
		char * utf8_buf = new char [utf8_len + 1];
		try
		{
			// UTF-8 に変換する
			targ.GetUtf8String(utf8_buf);

			// ファイルに書き込む
			Stream->WriteBuffer(utf8_buf, utf8_len);

			// 戻る
			return;
		}
		catch(...)
		{
			delete [] utf8_buf;
			throw;
		}
		delete [] utf8_buf;
	}

	// エラーの場合
	eRisaException::Throw(RISSE_WS_TR("can not write text file: failed to convert to UTF-8 charater"));
}
//---------------------------------------------------------------------------





















//---------------------------------------------------------------------------
tRisaTextStreamRegisterer::tRisaTextStreamRegisterer()
{
	RisseCreateTextStreamForRead = CreateForRead;
	RisseCreateTextStreamForWrite = CreateForWrite;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaTextStreamRegisterer::~tRisaTextStreamRegisterer()
{
	RisseCreateTextStreamForRead = NULL;
	RisseCreateTextStreamForWrite = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
iRisseTextReadStream * tRisaTextStreamRegisterer::CreateForRead(const tRisseString &name,
		const tRisseString &modestr)
{
	return new tRisaTextReadStream(name, modestr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
iRisseTextWriteStream * tRisaTextStreamRegisterer::CreateForWrite(const tRisseString &name,
		const tRisseString &modestr)
{
	return new tRisaTextWriteStream(name, modestr);
}
//---------------------------------------------------------------------------

