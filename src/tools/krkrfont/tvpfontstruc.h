//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// "tTVPFont" definition
//---------------------------------------------------------------------------

#ifndef __TVPFONTSTRUC_H__
#define __TVPFONTSTRUC_H__

//---------------------------------------------------------------------------
// tTVPFont definition
//---------------------------------------------------------------------------
struct tTVPFont
{
	tjs_int Height; // height of text
	tjs_uint32 Flags;
	tjs_int Angle; // rotation angle ( in tenths of degrees ) 0 .. 1800 .. 3600

	ttstr Face; // font name

	bool operator == (const tTVPFont & rhs) const
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
#define TVP_TF_ITALIC    0x0100
#define TVP_TF_BOLD      0x0200
#define TVP_TF_UNDERLINE 0x0400
#define TVP_TF_STRIKEOUT 0x0800


//---------------------------------------------------------------------------
#define TVP_FSF_FIXEDPITCH   1      // fsfFixedPitch
#define TVP_FSF_SAMECHARSET  2      // fsfSameCharSet
#define TVP_FSF_NOVERTICAL   4      // fsfNoVertical
#define TVP_FSF_OUTLINEONLY  8      // fsfOutLineOnly
#define TVP_FSF_USEFONTFACE  0x100  // fsfUseFontFace

/*]*/

//---------------------------------------------------------------------------
#endif
