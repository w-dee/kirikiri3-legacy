//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief wchar_t ベースの文字列のC++クラス
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseWCString.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(8159,12638,3849,19901,12983,59012,32772,31241);



//---------------------------------------------------------------------------
const wchar_t * tWCString::EmptyString = L"";
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tWCString & tWCString::operator = (const tString & ref)
{
	if(ref.IsEmpty())
	{
		Buffer = NULL;
	}
	else
	{
		Buffer = CharToWCStringBuffer(ref.Pointer(), ref.GetLength());
	}
	return *this;
}
//---------------------------------------------------------------------------


#ifdef RISSE_SUPPORT_WX
//---------------------------------------------------------------------------
tWCString & tWCString::operator = (const wxString & ref)
{
	// wxString はすでに wxChar ( = wchar_t ) ベースのはずなので
	// バッファを単純にコピーする
	const wchar_t * ref_buf = ref.c_str();
	size_t ref_len = ref.length();
	wchar_t * new_buffer = new (PointerFreeGC) wchar_t[ref_len + 1];
	memcpy(new_buffer, ref_buf, sizeof(wchar_t) * (ref_len + 1));
	Buffer = new_buffer;
	return *this;
}
//---------------------------------------------------------------------------
#endif



//---------------------------------------------------------------------------
} // namespace Risse
