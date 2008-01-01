//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// "tFont" definition
//---------------------------------------------------------------------------

#ifndef __FONTSTRUC_H__
#define __FONTSTRUC_H__

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// tFont definition
//---------------------------------------------------------------------------
struct tFont
{
	risse_int Height; // height of text
	risse_uint32 Flags;
	risse_int Angle; // rotation angle ( in tenths of degrees ) 0 .. 1800 .. 3600

	ttstr Face; // font name

	bool operator == (const tFont & rhs) const
	{
		return Height == rhs.Height &&
			Flags == rhs.Flags &&
			Angle == rhs.Angle && 
			Face == rhs.Face;
	}
};


/*[*/
//---------------------------------------------------------------------------
// font ralated constants
//---------------------------------------------------------------------------
#define RISA__TF_ITALIC    0x0100
#define RISA__TF_BOLD      0x0200
#define RISA__TF_UNDERLINE 0x0400
#define RISA__TF_STRIKEOUT 0x0800


//---------------------------------------------------------------------------
#define RISA__FSF_FIXEDPITCH   1      // fsfFixedPitch
#define RISA__FSF_SAMECHARSET  2      // fsfSameCharSet
#define RISA__FSF_NOVERTICAL   4      // fsfNoVertical
#define RISA__FSF_OUTLINEONLY  8      // fsfOutLineOnly
#define RISA__FSF_USEFONTFACE  0x100  // fsfUseFontFace

/*]*/

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

#endif
