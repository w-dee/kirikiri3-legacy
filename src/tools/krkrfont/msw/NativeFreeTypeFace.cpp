//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Win32 GDI 経由でのFreeType Face
//! @note フォント名からフォントファイル名を得る動作がOSごとに異なるため、
//! tRisaFreeTypeFace もプラットフォームごとに異なった実装となる。
//---------------------------------------------------------------------------
#include "prec.h"
#include "NativeFreeTypeFace.h"
#include "FreeType.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TAGS_H
#include FT_TRUETYPE_TABLES_H
#include FT_TRUETYPE_UNPATENTED_H


#define RISA__TT_TABLE_ttcf  (('t' << 0) + ('t' << 8) + ('c' << 16) + ('f' << 24))
#define RISA__TT_TABLE_name  (('n' << 0) + ('a' << 8) + ('m' << 16) + ('e' << 24))

//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		fontname フォント名
//! @param		options オプション
//---------------------------------------------------------------------------
tRisaNativeFreeTypeFace::tRisaNativeFreeTypeFace(const wxString &fontname,
	risse_uint32 options)
{
	// フィールドのクリア
	FaceName = fontname;
	Face = NULL;
	memset(&Stream, 0, sizeof(Stream));
	DC = NULL;
	OldFont = NULL;
	IsTTC = false;

	unsigned char *name_content    = NULL; // Windows から取得した name タグの内容
	unsigned char *name_content_ft = NULL; // FreeType から取得した name タグの内容
	risse_size name_content_size;

	// TrueType ライブラリをフック
	tRisaFreeTypeLibrary::AddRef();
	try
	{
		// 指定のフォントを持ったデバイスコンテキストを作成する
		// TODO: Italic, Bold handling

		DC = GetDC(0);
		LOGFONT l;
		l.lfHeight = -12;
		l.lfWidth = 0;
		l.lfEscapement = 0;
		l.lfOrientation = 0;
		l.lfWeight = 400;
		l.lfItalic = FALSE;
		l.lfUnderline = FALSE;
		l.lfStrikeOut = FALSE;
		l.lfCharSet = DEFAULT_CHARSET;
		l.lfOutPrecision = OUT_DEFAULT_PRECIS;
		l.lfQuality = DEFAULT_QUALITY;
		l.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		wxStrncpy(l.lfFaceName, fontname.c_str(), LF_FACESIZE-1);
		l.lfFaceName[LF_FACESIZE] = wxT('\0');

		HFONT newfont = CreateFontIndirect(&l);
		OldFont = static_cast<HFONT>(SelectObject(DC, newfont));

		// このフォントが GetFontData API で扱えるかどうかを
		// 'name' タグの内容を取得しようとすることでチェックする
		// (name タグは GetFontData が扱うような TrueType/OpenType フォントには
		//  必ず入っている)
		DWORD result = GetFontData(DC, RISA__TT_TABLE_name, 0, NULL, 0);
		if(result == GDI_ERROR)
		{
			// エラー; GetFontData では扱えなかった
			wxString msg;
			msg.sprintf(_("Font '%1$s' cannot be used"), fontname.c_str());
			throw msg;
		}

		//- この時点で result は name タグの内容が入るのに必要なバイト数
		name_content_size = result;
		name_content = new unsigned char [name_content_size]; // メモリを確保
		name_content_ft = new unsigned char [name_content_size]; // メモリを確保

		//- name タグの内容をメモリに読み込む
		result = GetFontData(DC, RISA__TT_TABLE_name, 0, name_content, name_content_size);
		if(result == GDI_ERROR)
		{
			// エラー; メモリに読み込むことが出来なかった
			wxString msg;
			msg.sprintf(_("Font '%1$s' cannot be used"), fontname.c_str());
			throw msg;
		}

		// フォントファイルのサイズを取得する
		risse_size fontsize;

		//- TTC (True Type Collection) ファイルのチェック
		//- GetFontData API の仕様では、TTC ファイルに対しては、現在選択されている
		//- フォントに対する情報しか返さない。しかし FreeType は TTC ファイル全体の
		//- 情報を必要とする。この場合、GetFontData に 'ttcf' を得るように指示すると
		//- ファイル全体の情報を得ることが出来る。
		//- 参照 : microsoft.public.win32.programmer.gdi GetFontData and TTC fonts
		unsigned char buf[4];
		result = GetFontData(DC, RISA__TT_TABLE_ttcf, 0, &buf, 1);
		if(result == 1)
		{
			// TTC ファイルだと思われる
			result = GetFontData(DC, RISA__TT_TABLE_ttcf, 0, NULL, 0);
			IsTTC = true;
		}
		else
		{
			result = GetFontData(DC, 0, 0, NULL, 0);
		}

		if(result == GDI_ERROR)
		{
			// エラー; GetFontData では扱えなかった
			wxString msg;
			msg.sprintf(_("Font '%1$s' cannot be used"), fontname.c_str());
			throw msg;
		}
		fontsize = result;

		// FT_StreamRec の各フィールドを埋める
		FT_StreamRec * fsr = &Stream;
		fsr->base = 0;
		fsr->size = fontsize;
		fsr->pos = 0;
		fsr->descriptor.pointer = this;
		fsr->pathname.pointer = NULL;
		fsr->read = IoFunc;
		fsr->close = CloseFunc;

		// FreeType で開く
		// 試しに 0 番の Face を開く
		// (この時点で開くことが出来なければ例外を発生させる)
		int index = 0;
		if(!OpenFaceByIndex(index))
		{
			wxString msg;
			msg.sprintf(_("Font '%1$s' cannot be used"), fontname.c_str());
			throw msg;
		}

		// GDIが現在選択しているファイルとFreeTypeがアクセスしているファイルが
		// 実際に合致しているかどうかを、name タグの一致で見る。
		// とくに TTC ファイルの場合は、name タグの一致を見ながら、face のインデッ
		// クスを一つずつ増やしながら、対象とするフォントを探さなければならない。
		while(true)
		{
			// FreeType から、name タグのサイズを取得する
			FT_ULong length = 0;
			FT_Error err = FT_Load_Sfnt_Table(Face, TTAG_name, 0, NULL, &length);
			if(err)
			{
				wxString msg;
				msg.sprintf(_("Font '%1$s' cannot be used"), fontname.c_str());
				throw msg;
			}

			// FreeType から得た name タグの長さを Windows から得た長さと比較
			if(length == name_content_size)
			{
				// FreeType から name タグを取得
				err = FT_Load_Sfnt_Table(Face, TTAG_name, 0, name_content_ft, &length);
				if(err)
				{
					wxString msg;
					msg.sprintf(_("Font '%1$s' cannot be used"), fontname.c_str());
					throw msg;
				}
				// FreeType から読み込んだ name タグの内容と、Windows から読み込んだ
				// name タグの内容を比較する。
				// 一致していればその index のフォントを使う。
				if(!memcmp(name_content, name_content_ft, name_content_size))
				{
					// 一致した
					// face は開いたまま
					break; // ループを抜ける
				}
			}

			// 一致しなかった
			// インデックスを一つ増やし、その face を開く
			index ++;

			if(!OpenFaceByIndex(index))
			{
				// 一致する face がないまま インデックスが範囲を超えたと見られる
				// index を 0 に設定してその index を開き、ループを抜ける
				index = 0;
				if(!OpenFaceByIndex(index))
				{
					wxString msg;
					msg.sprintf(_("Font '%1$s' cannot be used"), fontname.c_str());
					throw msg;
				}
				break;
			}
		}

	}
	catch(...)
	{
		Clear();
		if(name_content) delete [] name_content;
		if(name_content_ft) delete [] name_content_ft;
		tRisaFreeTypeLibrary::Release();
		throw;
	}
	delete [] name_content;
	delete [] name_content_ft;

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaNativeFreeTypeFace::~tRisaNativeFreeTypeFace()
{
	Clear();
	tRisaFreeTypeLibrary::Release();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		FreeType の Face オブジェクトを返す
//---------------------------------------------------------------------------
FT_Face tRisaNativeFreeTypeFace::GetFTFace() const
{
	return Face;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		このフォントファイルが持っているフォントを配列として返す
//! @param		dest 格納先配列
//---------------------------------------------------------------------------
void tRisaNativeFreeTypeFace::GetFaceNameList(wxArrayString & dest) const
{
	// このFaceの場合、既にFaceは特定されているため、利用可能な
	// Face 数は常に1で、フォント名はこのオブジェクトが構築された際に渡された
	// フォント名となる
	dest.Clear();
	dest.Add(FaceName);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		全てのオブジェクトを解放する
//---------------------------------------------------------------------------
void tRisaNativeFreeTypeFace::Clear()
{
	if(Face) FT_Done_Face(Face), Face = NULL;
	if(OldFont && DC)
	{
		HFONT font = static_cast<HFONT>(SelectObject(DC, OldFont));
		DeleteObject(font);
		OldFont = NULL;
	}
	if(DC) ReleaseDC(0, DC), DC = NULL;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		FreeType 用 ストリーム読み込み関数
//! @param		stream FT_Streamへのポインタ
//! @param		offset ストリーム先頭からのオフセット
//! @param		buffer 格納先バッファ
//! @param		count  読み出すバイト数
//! @return		何バイト読み込まれたか
//---------------------------------------------------------------------------
unsigned long tRisaNativeFreeTypeFace::IoFunc(
			FT_Stream stream,
			unsigned long   offset,
			unsigned char*  buffer,
			unsigned long   count )
{
	if(count != 0)
	{
		tRisaNativeFreeTypeFace * _this =
			static_cast<tRisaNativeFreeTypeFace*>(stream->descriptor.pointer);
		DWORD result = GetFontData(_this->DC, 
				_this->IsTTC ? RISA__TT_TABLE_ttcf : 0,
				offset, buffer, count);
		if(result == GDI_ERROR)
		{
			// エラー
			return 0;
		}
		return result;
	}
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		FreeType 用 ストリーム削除関数
//! @param		stream FT_Streamへのポインタ
//---------------------------------------------------------------------------
void tRisaNativeFreeTypeFace::CloseFunc( FT_Stream  stream )
{
	// 何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定インデックスのFaceを開く
//! @param		index	開くindex
//! @return		Faceを開ければ true そうでなければ false
//---------------------------------------------------------------------------
bool tRisaNativeFreeTypeFace::OpenFaceByIndex(int index)
{
	if(Face) FT_Done_Face(Face), Face = NULL;

	FT_Parameter parameters[1];
	parameters[0].tag = FT_PARAM_TAG_UNPATENTED_HINTING; // Appleの特許回避を行う
	parameters[0].data = NULL;

	FT_Open_Args args;
	memset(&args, 0, sizeof(args));
	args.flags = FT_OPEN_STREAM;
	args.stream = &Stream;
	args.driver = 0;
	args.num_params = 1;
	args.params = parameters;

	FT_Error err = FT_Open_Face(tRisaFreeTypeLibrary::Get(), &args, index, &Face);
	return err == 0;
}
//---------------------------------------------------------------------------




