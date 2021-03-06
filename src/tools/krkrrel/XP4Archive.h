//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4 アーカイブファイルシステム
//---------------------------------------------------------------------------

#ifndef XP4ArchiveH
#define XP4ArchiveH

namespace Risa {
//---------------------------------------------------------------------------


#define RISA_XP4_INDEX_ENCODE_METHOD_MASK 0x03
#define RISA_XP4_INDEX_ENCODE_RAW      0
#define RISA_XP4_INDEX_ENCODE_ZLIB     1

#define RISA_XP4_SEGM_ENCODE_METHOD_MASK  0x03
#define RISA_XP4_SEGM_ENCODE_RAW       0
#define RISA_XP4_SEGM_ENCODE_ZLIB      1

#define RISA_XP4_FILE_COMPRESSED 0x80	//!< not a real code; only used in tXP4Writer related
#define RISA_XP4_FILE_EXCLUDED	0x40	//!< not a real code; only used in tXP4Writer related
#define RISA_XP4_FILE_MARKED		0x20
#define RISA_XP4_FILE_STATE_MASK	0x0f
#define RISA_XP4_FILE_STATE_NONE		0
#define RISA_XP4_FILE_STATE_ADDED	1
#define RISA_XP4_FILE_STATE_DELETED	2
#define RISA_XP4_FILE_STATE_MODIFIED	3
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
