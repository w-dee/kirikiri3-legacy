//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief FreeType フォントドライバ
//---------------------------------------------------------------------------
#include "prec.h"
#include "FreeType.h"
#include "NativeFreeTypeFace.h"
#include "uni_cp932.h"
#include "cp932_uni.h"
#include <wx/file.h>

#include <ft2build.h>
#include FT_TRUETYPE_UNPATENTED_H

#ifndef wxUSE_UNICODE
	#error Kirikiri2 needs to be compiled with UNICODE enabled wxWidgets.
#endif



//---------------------------------------------------------------------------
//! @brief		ファイルシステム経由でのFreeType Face クラス
//---------------------------------------------------------------------------
class tTVPGenericFreeTypeFace : public tTVPBaseFreeTypeFace
{
protected:
	FT_Face Face;	//!< FreeType face オブジェクト
	wxFile File;	 //!< wxFile オブジェクト
	wxArrayString FaceNames; //!< Face名を列挙した配列

private:
	FT_StreamRec Stream;

public:
	tTVPGenericFreeTypeFace(const ttstr &fontname, tjs_uint32 options);
	virtual ~tTVPGenericFreeTypeFace();

	virtual FT_Face GetFTFace() const;
	virtual void GetFaceNameList(wxArrayString & dest) const;

private:
	void Clear();
	static unsigned long IoFunc(
			FT_Stream stream,
			unsigned long   offset,
			unsigned char*  buffer,
			unsigned long   count );
	static void CloseFunc( FT_Stream  stream );

	bool OpenFaceByIndex(tjs_uint index, FT_Face & face);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		fontname: フォント名
//! @param		options: オプション
//---------------------------------------------------------------------------
tTVPGenericFreeTypeFace::tTVPGenericFreeTypeFace(const ttstr &fontname, tjs_uint32 options)
{
	// フィールドの初期化
	Face = NULL;
	memset(&Stream, 0, sizeof(Stream));

	// TrueType ライブラリをフック
	tTVPFreeTypeLibrary::AddRef();

	try
	{
		// ファイルを開く
		if(!File.Open(fontname))
		{
			wxString msg;
			msg.sprintf(_("Can't open font file '%1$s'"), fontname.c_str());
			throw msg;
		}

		// FT_StreamRec の各フィールドを埋める
		FT_StreamRec * fsr = &Stream;
		fsr->base = 0;
		fsr->size = File.Length();
		fsr->pos = 0;
		fsr->descriptor.pointer = this;
		fsr->pathname.pointer = NULL;
		fsr->read = IoFunc;
		fsr->close = CloseFunc;

		// Face をそれぞれ開き、Face名を取得して FaceNames に格納する
		tjs_uint face_num = 1;

		FT_Face face = NULL;

		for(tjs_uint i = 0; i < face_num; i++)
		{
			if(!OpenFaceByIndex(i, face))
			{
				FaceNames.Add(wxEmptyString);
				wxMessageBox(wxT("listing failed"));

			}
			else
			{
				const char * name = face->family_name;
				FaceNames.Add(wxString(name, wxConvUTF8));
				face_num = face->num_faces;
			}
		}

		if(face) FT_Done_Face(face), face = NULL;


		// FreeType エンジンでファイルを開こうとしてみる
		tjs_uint index = TVP_GET_FACE_INDEX_FROM_OPTIONS(options);
		if(!OpenFaceByIndex(index, Face))
		{
			// フォントを開けなかった
			wxString msg;
			msg.sprintf(_("Font '%1$s' cannot be used"), fontname.c_str());
			throw msg;
		}
	}
	catch(...)
	{
		tTVPFreeTypeLibrary::Release(); // FreeType ライブラリを unhook
		throw;
	}
	
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPGenericFreeTypeFace::~tTVPGenericFreeTypeFace()
{
	if(Face) FT_Done_Face(Face), Face = NULL;
	tTVPFreeTypeLibrary::Release(); // FreeType ライブラリを unhook
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		FreeType の Face オブジェクトを返す
//---------------------------------------------------------------------------
FT_Face tTVPGenericFreeTypeFace::GetFTFace() const
{
	return Face;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		このフォントファイルが持っているフォントを配列として返す
//---------------------------------------------------------------------------
void tTVPGenericFreeTypeFace::GetFaceNameList(wxArrayString & dest) const
{
	dest = FaceNames;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		FreeType 用 ストリーム読み込み関数
//---------------------------------------------------------------------------
unsigned long tTVPGenericFreeTypeFace::IoFunc(
			FT_Stream stream,
			unsigned long   offset,
			unsigned char*  buffer,
			unsigned long   count )
{
	tTVPGenericFreeTypeFace * _this = (tTVPGenericFreeTypeFace*)stream->descriptor.pointer;

	size_t result;
	if(count == 0)
	{
		// seek
		result = 0;
		_this->File.Seek((wxFileOffset)offset);
	}
	else
	{
		// read
		_this->File.Seek((wxFileOffset)offset);
		result = _this->File.Read(buffer, count);
	}

	return result;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		FreeType 用 ストリーム削除関数
//---------------------------------------------------------------------------
void tTVPGenericFreeTypeFace::CloseFunc( FT_Stream  stream )
{
	// 何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定インデックスのFaceを開く
//! @param		index: 開くindex
//! @param		face: FT_Face 変数への参照
//! @return		Faceを開ければ true そうでなければ false
//! @note		初めて Face を開く場合は face で指定する変数には null を入れておくこと
//---------------------------------------------------------------------------
bool tTVPGenericFreeTypeFace::OpenFaceByIndex(tjs_uint index, FT_Face & face)
{
	if(face) FT_Done_Face(face), face = NULL;

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

	FT_Error err = FT_Open_Face(tTVPFreeTypeLibrary::Get(), &args, index, &face);

	return err == 0;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		fontname: フォント名
//! @param		options: オプション
//---------------------------------------------------------------------------
tTVPFreeTypeFace::tTVPFreeTypeFace(const wxString &fontname, tjs_uint32 options)
	: FontName(fontname)
{
	// フォントを開く
	if(options & TVP_FACE_OPTIONS_FILE)
	{
		// ファイルを開く
		Face = new tTVPGenericFreeTypeFace(fontname, options);
	}
	else
	{
		// ネイティブのフォント名による指定 (プラットフォーム依存)
		Face = new tTVPNativeFreeTypeFace(fontname, options);
			// 例外がここで発生する可能性があるので注意
	}
	FTFace = Face->GetFTFace();

	Height = 10;

	// フィールドをクリア
	GlyphToCharcodeMap = NULL;
	UnicodeToLocalChar = NULL;
	LocalCharToUnicode = NULL;

	// マッピングを確認する
	if(FTFace->charmap == NULL)
	{
		// FreeType は自動的に UNICODE マッピングを使用するが、
		// フォントが UNICODE マッピングの情報を含んでいない場合は
		// 自動的な文字マッピングの選択は行われない。
		// とりあえず(日本語環境に限って言えば) SJIS マッピングしかもってない
		// フォントが多いのでSJISを選択させてみる。
		FT_Error err = FT_Select_Charmap(FTFace, FT_ENCODING_SJIS);
		if(!err)
		{
			// SJIS への切り替えが成功した
			// 変換関数をセットする
			UnicodeToLocalChar = TVPUnicodeToSJIS;
			LocalCharToUnicode = TVPSJISToUnicode;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPFreeTypeFace::~tTVPFreeTypeFace()
{
	if(GlyphToCharcodeMap) delete GlyphToCharcodeMap;
	if(Face) delete Face;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		このFaceが保持しているglyphの数を得る
//! @return		このFaceが保持しているglyphの数
//---------------------------------------------------------------------------
tjs_uint tTVPFreeTypeFace::GetGlyphCount()
{
	if(!FTFace) return 0;
	return FTFace->num_glyphs;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		Glyph インデックスから対応する文字コードを得る
//! @return		対応する文字コード
//---------------------------------------------------------------------------
tjs_char tTVPFreeTypeFace::CharcodeFromGlyphIndex(tjs_uint index)
{
	// TODO:スレッド保護されていないので注意！！！！！！
	if(!GlyphToCharcodeMap)
	{
		// マップが作成されていないので作成する
		GlyphToCharcodeMap = new tGlyphToCharcodeMap;
		FT_ULong  charcode;
		FT_UInt   gindex;
		charcode = FT_Get_First_Char( FTFace, &gindex );
		while ( gindex != 0 )
		{
			FT_ULong code;
			if(LocalCharToUnicode)
				code = LocalCharToUnicode(charcode);
			else
				code = charcode;
			GlyphToCharcodeMap->insert(
				std::pair<FT_UInt, FT_ULong>(gindex, code));
			charcode = FT_Get_Next_Char( FTFace, charcode, &gindex );
		}
	}

	tGlyphToCharcodeMap::iterator it;
	it = GlyphToCharcodeMap->find((FT_UInt)(index));
	if(it != GlyphToCharcodeMap->end())
	{
		FT_ULong charcode = it->second;
		// tjs_char で扱える範囲内にあるかをチェックし、扱えなければ 0 を返す
		if((FT_ULong)(tjs_char)charcode  != charcode)
			return 0;
		return (tjs_char)charcode;
	}
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		このフォントに含まれるFace名のリストを得る
//! @param		dest: 格納先配列
//---------------------------------------------------------------------------
void tTVPFreeTypeFace::GetFaceNameList(wxArrayString &dest)
{
	Face->GetFaceNameList(dest);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		フォントの高さを設定する
//! @param		height: フォントの高さ(ピクセル単位)
//---------------------------------------------------------------------------
void tTVPFreeTypeFace::SetHeight(int height)
{
	Height = height;
	FT_Error err = FT_Set_Pixel_Sizes(FTFace, 0, Height);
	if(err)
	{
		// TODO: Error ハンドリング
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定した文字コードに対するグリフビットマップを得る
//! @param		文字コード
//! @return		新規作成されたグリフビットマップオブジェクトへのポインタ
//!				NULL の場合は変換に失敗した場合
//---------------------------------------------------------------------------
tTVPGlyphBitmap * tTVPFreeTypeFace::GetGlyphFromCharcode(tjs_char code)
{
	// TODO: スレッド保護

	// 文字コードを得る
	FT_ULong localcode;
	if(UnicodeToLocalChar == NULL)
		localcode = code;
	else
		localcode = UnicodeToLocalChar(code);

	// 文字をレンダリングする
	FT_Error err;
	err = FT_Load_Char(FTFace, localcode,
		FT_LOAD_RENDER|FT_LOAD_NO_BITMAP
//		|FT_LOAD_FORCE_AUTOHINT
//		|FT_LOAD_NO_HINTING
		);
			// note: ビットマップフォントを読み込みたくない場合は FT_LOAD_NO_BITMAP を指定
			// note: デフォルトのレンダリングモードは FT_RENDER_MODE_NORMAL (256色グレースケール)
	if(err) return NULL;

	// 一応ビットマップ形式をチェック
	if(FTFace->glyph->bitmap.pixel_mode != ft_pixel_mode_grays)
	{
		// TODO: これチェック。internal error と見なされるべき？
		return NULL;
	}

	// メトリック構造体を作成
	// CellIncX や CellIncY は ピクセル値が 64 倍された値なので注意
	// これはもともと FreeType の仕様だけれども、吉里吉里でも内部的には
	// この精度で CellIncX や CellIncY を扱う
	// TODO: 文字のサブピクセル単位での位置調整とレンダリング
	tTVPGlyphMetrics metrics;
	metrics.CellIncX =  FTFace->glyph->advance.x;
	metrics.CellIncY =  FTFace->glyph->advance.y;

	// tTVPGlyphBitmap を作成して返す
	return new tTVPGlyphBitmap(
		FTFace->glyph->bitmap.buffer,
		FTFace->glyph->bitmap.pitch,
		  FTFace->glyph->bitmap_left,
		(Height - 1) - FTFace->glyph->bitmap_top, // slot->bitmap_top は下方向が負なので注意
		  FTFace->glyph->bitmap.width,
		  FTFace->glyph->bitmap.rows,
		metrics);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPFreeTypeLibrary: FreeType ライブラリラッパー(singleton)
//---------------------------------------------------------------------------
tTVPFreeTypeLibrary * tTVPFreeTypeLibrary::GlobalLibrary = NULL;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		参照カウンタを増やす
//---------------------------------------------------------------------------
void tTVPFreeTypeLibrary::AddRef()
{
	if(!GlobalLibrary)
	{
		// ライブラリを初期化する
		GlobalLibrary = new tTVPFreeTypeLibrary();
	}
	else
	{
		GlobalLibrary->Hook();
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		参照カウンタを減らす
//---------------------------------------------------------------------------
void tTVPFreeTypeLibrary::Release()
{
	if(!GlobalLibrary)
	{
		// fatal
		throw 0; //TODO: proper exception throw
	}

	if(GlobalLibrary->Unhook() == 0)
	{
//		delete GlobalLibrary;
//		GlobalLibrary = NULL;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPFreeTypeLibrary::tTVPFreeTypeLibrary()
{
	RefCount = 1;
	FT_Error err = FT_Init_FreeType(&Library);
	if(err)
	{
		// FreeType の初期化でエラー
		// 通常は考えられない
		throw(wxT("Error in initializing FreeType Library"));
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPFreeTypeLibrary::~tTVPFreeTypeLibrary()
{
	FT_Done_FreeType(Library);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		参照カウンタを増やす
//---------------------------------------------------------------------------
void tTVPFreeTypeLibrary::Hook()
{
	RefCount ++;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		参照カウンタを減らす
//! @return		参照カウンタを減らした後の参照カウンタの値
//---------------------------------------------------------------------------
tjs_int tTVPFreeTypeLibrary::Unhook()
{
	return --RefCount;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPFreeTypeFontDriver::tTVPFreeTypeFontDriver()
{
	// コンストラクタ
}
//---------------------------------------------------------------------------
tTVPFreeTypeFontDriver::~tTVPFreeTypeFontDriver()
{
	// デストラクタ
}
//---------------------------------------------------------------------------



