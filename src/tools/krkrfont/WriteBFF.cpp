//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief BFF (TVP3 Bitmap Font File) の書き出し
//---------------------------------------------------------------------------
#include <prec.h>
#include <wx/progdlg.h>
#include <wx/file.h>
#include "FreeType.h"
#include "BFF.h"


//---------------------------------------------------------------------------
//! @brief		グリフビットマップを圧縮する
//! @param		in 入力データ
//! @param		in_w 入力ビットマップの横のサイズ
//! @param		in_h 入力ビットマップの縦のサイズ
//! @param		in_pitch 入力ビットマップのピッチ
//! @param		out 出力データ
//! @param		out_size 出力データのサイズ
//! @note		このアルゴリズムは最悪の場合で入力データを 2 倍 + 1にする
//! 			そのため、out には in_size の 2 倍 + 1のデータ領域を確保すること
//---------------------------------------------------------------------------
static void TVPCompressGlyphBitmap(const tjs_uint8 * in,
	tjs_uint in_w, tjs_uint in_h,
	tjs_int in_pitch,
	tjs_uint8 * out, tjs_uint *out_size)
{
	// 圧縮は、展開時にあまり CPU 負荷をかけなくても済むように、単純な
	// ランレングス圧縮を変形した物とする

	// TODO: 圧縮方式の説明をここに書く
	tjs_uint8 * diff = new tjs_uint8 [in_w * in_h];
	for(tjs_uint y = 0, i = 0; y < in_h; y++)
	{
		tjs_uint8 prev = 0;
		const tjs_uint8 * ln = in + y * in_pitch;
		for(tjs_uint x = 0; x < in_w; x++)
		{
			diff[i] = ln[x] - prev;
			prev = ln[x];
			i++;
		}
	}

	tjs_uint in_size = in_w * in_h;

	for(tjs_uint i = in_w; i < in_size; i++)
	{
		diff[i] -= diff[i - in_w];
	}

	tjs_uint8 * op = out;

	for(tjs_uint i = 0; i < in_size; i++)
	{
		tjs_uint j;
		tjs_uint limit;

		// count zero part
		limit = std::min(i + 255, in_size);
		for(j = i; j < limit; j++)
			if(diff[j] != 0) break;

		// encode zero length
		*(op++) = static_cast<tjs_uint8>(j - i);
		i = j;

		// count non-zero part
		limit = std::min(i + 255, in_size);
		for(j = i; j < limit; j++)
			if(diff[j] == 0) break;

		// encode non-zero length
		*(op++) = static_cast<tjs_uint8>(j - i);

		// output non-zero values
		for(; i < j; i++)
			*(op++) = diff[i];
	}

	delete [] diff;

	*out_size = op - out;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		出力ファイルにデータを書き出す
//! @param		face 書き出すfaceデータ
//! @param		out_file 出力ファイル
//! @param		write_bitmap ビットマップデータを出力するかどうか(普通true)
//! @param		write_kerning_vector カーニング情報を出力するかどうか
//! @param		parent ダイアログボックスの親ウィンドウ
//---------------------------------------------------------------------------
void TVPWriteGlyphBitmap(tTVPFreeTypeFace * face, const wxString & out_file,
	bool write_bitmap,
	bool write_kerning_vector, wxWindow * parent)
{
	tTVPBFFHeader header;
	tjs_uint num_files = 0;
	tTVPBFFDirectory directory[3];
	tTVPBFFDirectory * char_map_directory = directory + 0;
	tTVPBFFDirectory * bitmap_directory = directory + 1;
//	tTVPBFFDirectory * kern_vector_directory = directory + 2;
	tTVPBFFCharacterMap *map = NULL;
	tTVPBFFKerningVector *kern_vector = NULL;
	tTVPGlyphBitmap * bitmap = NULL;
	tjs_uint8 * bitmap_tmp = NULL;
	bool canceled = false;

	// 出力ファイルを開く
	wxFile file;
	if(!file.Open(out_file, wxFile::write))
	{
		wxString msg;
		msg.sprintf(_("Can't open output file '%1$s'"), out_file.c_str());
		throw msg;
	}

	// 全部のグリフ数を得る
	tjs_uint total_glyph_count = face->GetGlyphCount();

	// ダイアログボックスを作成する
	wxProgressDialog dialog(
							/* font generation progress dialog box's title */
							_("Processing"),
							/* font generation progress dialog box's message */
							_("Generating font file ..."),
							total_glyph_count,	// range
							parent,	// parent
							wxPD_CAN_ABORT |
							wxPD_APP_MODAL |
							wxPD_SMOOTH);

	try
	{
		// map のメモリを確保
		map = new tTVPBFFCharacterMap[total_glyph_count];

		// ディレクトリを準備する
		// ただしディレクトリ中のサイズオフセットについてはまたあとで修正して書き込む
		char_map_directory->Tag = wxUINT32_SWAP_ON_BE(bftCharacteMap);
		char_map_directory->Size = wxUINT32_SWAP_ON_BE(0); // 後で修正
		char_map_directory->Offset = wxUINT32_SWAP_ON_BE(0); // 後で修正
		char_map_directory->Reserved = wxUINT32_SWAP_ON_BE(0);
		num_files ++;

		if(write_bitmap)
		{
			bitmap_directory->Tag = wxUINT32_SWAP_ON_BE(bftBitmaps);
			bitmap_directory->Size = wxUINT32_SWAP_ON_BE(0); // 後で修正
			bitmap_directory->Offset = wxUINT32_SWAP_ON_BE(0); // 後で修正
			bitmap_directory->Reserved = wxUINT32_SWAP_ON_BE(0);
			num_files ++;
		}

		if(write_kerning_vector)
		{
			bitmap_directory->Tag = wxUINT32_SWAP_ON_BE(bftKerningVector);
			bitmap_directory->Size = wxUINT32_SWAP_ON_BE(0); // 後で修正
			bitmap_directory->Offset = wxUINT32_SWAP_ON_BE(0); // 後で修正
			bitmap_directory->Reserved = wxUINT32_SWAP_ON_BE(0);
		}

		// ヘッダを作成して書き込む
		header.Magic[0] = 'B'; header.Magic[1] = 'F'; header.Magic[2] = 'F';
		header.Magic[3] = 0;

		header.Version[0] = 1;
		header.Reserved0[0] = 0; header.Reserved0[1] = 0; header.Reserved0[2] = 0;

		header.NumFiles = wxUINT32_SWAP_ON_BE(num_files);

		file.Write(&header, sizeof(header));


		// ディレクトリを書き込む
		wxFileOffset directory_offset = file.Tell();
		file.Write(directory, sizeof(tTVPBFFDirectory) * num_files);

		// ファイルポインタを 4 byte 境界にアラインメントする
		{
			tjs_uint align_fill_bytes = (4 - (file.Tell() & 0x03)) & 0x03;
			if(align_fill_bytes > 0) file.Write("\0\0\0\0", align_fill_bytes);
		}

		// ビットマップを書き込む
		wxFileOffset bitmap_offset = file.Tell();
		bitmap_directory->Offset = wxUINT32_SWAP_ON_BE(static_cast<tjs_uint32>(bitmap_offset)); // 修正

		//- ビットマップデータの先頭には 4 つの 0 を書く (仕様)
		file.Write("\0\0\0\0", 4);

		//- すべてのグリフに対してビットマップデータを書き出す
		tjs_uint actual_glyph_count = 0;
		for(tjs_uint i = 0; i < total_glyph_count; i++)
		{
			//- ファイルポインタを 2 byte 境界にアラインメントする
			{
				tjs_uint align_fill_bytes = (2 - (file.Tell() & 0x01)) & 0x01;
				if(align_fill_bytes > 0) file.Write("\0\0", align_fill_bytes);
			}

			// このインデックスに対応する文字コードを得る
			tjs_char charcode = face->GetCharcodeFromGlyphIndex(i);

			if(write_bitmap)
			{
				// ビットマップを書き込む場合

				// ビットマップを得る
				bitmap = face->GetGlyphFromCharcode(charcode);
				if(!bitmap)
				{
					// ビットマップを得られなかった
					continue;
				}

				// map に情報を書き込む
				map[actual_glyph_count].Unicode =
					wxUINT32_SWAP_ON_BE(static_cast<tjs_uint32>(charcode));
				map[actual_glyph_count].CellIncX =
					wxINT32_SWAP_ON_BE(bitmap->GetMetrics().CellIncX);
				map[actual_glyph_count].CellIncY =
					wxINT32_SWAP_ON_BE(bitmap->GetMetrics().CellIncY);
				map[actual_glyph_count].Offset =
					wxUINT32_SWAP_ON_BE(static_cast<tjs_uint32>(file.Tell() - bitmap_offset));

				// tTVPBFFBitmapHeader に情報を書き込む
				tTVPBFFBitmapHeader header;
				header.Flags     = wxUINT16_SWAP_ON_BE(0);
				header.OriginX   = wxINT16_SWAP_ON_BE (bitmap->GetOriginX());
				header.OriginY   = wxINT16_SWAP_ON_BE (bitmap->GetOriginY());
				header.BlackBoxW = wxUINT16_SWAP_ON_BE(bitmap->GetBlackBoxW());
				header.BlackBoxH = wxUINT16_SWAP_ON_BE(bitmap->GetBlackBoxH());

				// ビットマップを圧縮
				bitmap_tmp = new tjs_uint8[
					bitmap->GetBlackBoxW() * bitmap->GetBlackBoxH() * 2 + 2];
				tjs_uint bitmap_comp_size = 0;
				TVPCompressGlyphBitmap(bitmap->GetData(),
					bitmap->GetBlackBoxW(),
					bitmap->GetBlackBoxH(),
					bitmap->GetPitch(),
					bitmap_tmp,
					&bitmap_comp_size);

				// bitmap を解放
				bitmap->Release(), bitmap = NULL;

				// ファイルにビットマップのヘッダと圧縮された
				// ビットマップを書き込む
				file.Write(&header, sizeof(header));
				file.Write(bitmap_tmp, bitmap_comp_size);

				// 圧縮されたビットマップを解放
				delete [] bitmap_tmp, bitmap_tmp = NULL;
			}
			else
			{
				// ビットマップを書き込まない場合
				tTVPGlyphMetrics metrics;
				if(!face->GetGlyphMetricsFromCharcode(charcode, metrics))
					continue; // 寸法を得られなかった

				// map に情報を書き込む
				map[actual_glyph_count].Unicode =
					wxUINT32_SWAP_ON_BE(static_cast<tjs_uint32>(charcode));
				map[actual_glyph_count].CellIncX =
					wxINT32_SWAP_ON_BE(metrics.CellIncX);
				map[actual_glyph_count].CellIncY =
					wxINT32_SWAP_ON_BE(metrics.CellIncY);
				map[actual_glyph_count].Offset =
					wxUINT32_SWAP_ON_BE(0);
						// map.Offset == 0 はビットマップが存在しないことを表す。
						// Offset = 0 が無効なオフセットであることは、BFF 中の
						// ビットマップファイル の先頭に 4 つの \0 があることで
						// 保証される。
			}

			// actual_glyph_count を増やす
			actual_glyph_count ++;

			// ダイアログボックスをアップデート
			if(actual_glyph_count % 128 == 0)
			{
				bool cont = dialog.Update(i);
				if(!cont)
				{
					canceled = true; // キャンセルされた
					break;
				}
			}
		}

		if(!canceled)
		{
			// ビットマップのファイルサイズを修正
			bitmap_directory->Size =
				wxUINT32_SWAP_ON_BE(static_cast<tjs_uint32>(file.Tell() - bitmap_offset)); // 修正

			// 文字マップのファイルサイズを修正
			char_map_directory->Size =
				wxUINT32_SWAP_ON_BE(sizeof(tTVPBFFCharacterMap) * actual_glyph_count); // 修正

			// 文字マップを書き込む
			//- ファイルポインタを 16 byte 境界にアラインメントする
			{
				tjs_uint align_fill_bytes = (16 - (file.Tell() & 15)) & 15;
				if(align_fill_bytes > 0) file.Write("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", align_fill_bytes);
			}

			char_map_directory->Offset =
				wxUINT32_SWAP_ON_BE(static_cast<tjs_uint32>(file.Tell())); // 修正

			file.Write(map, sizeof(tTVPBFFCharacterMap) * actual_glyph_count);

			if(write_kerning_vector)
			{
				// カーニングベクトルを作成する

				// TODO: カーニングベクトルの取得

				// カーニングベクトルを書き込む
				//- ファイルポインタを 4 byte 境界にアラインメントする
				{
					tjs_uint align_fill_bytes = (4 - (file.Tell() & 0x03)) & 0x03;
					if(align_fill_bytes > 0) file.Write("\0\0\0\0", align_fill_bytes);
				}
			}

			// 修正されたディレクトリをもう一度書く
			file.Seek(directory_offset);
			file.Write(directory, sizeof(tTVPBFFDirectory) * num_files);
		}
	}
	catch(...)
	{
		if(map) delete [] map;
		if(kern_vector) delete [] kern_vector;
		if(bitmap) bitmap->Release();
		if(bitmap_tmp) delete [] bitmap_tmp;
		wxRemoveFile(out_file); // 例外が発生した場合はファイルを削除
		throw;
	}

	if(map) delete [] map;
	if(kern_vector) delete [] kern_vector;
	if(bitmap) bitmap->Release();
	if(bitmap_tmp) delete [] bitmap_tmp;

	file.Close();

	if(canceled)
	{
		// 途中でキャンセルされた場合はファイルを削除
		wxRemoveFile(out_file);
	}
}
//---------------------------------------------------------------------------
