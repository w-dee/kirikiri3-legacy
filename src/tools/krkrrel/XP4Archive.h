//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4 アーカイブファイルシステム
//---------------------------------------------------------------------------

#ifndef XP4ArchiveH
#define XP4ArchiveH



#define TVP_XP4_INDEX_ENCODE_METHOD_MASK 0x03
#define TVP_XP4_INDEX_ENCODE_RAW      0
#define TVP_XP4_INDEX_ENCODE_ZLIB     1

#define TVP_XP4_SEGM_ENCODE_METHOD_MASK  0x03
#define TVP_XP4_SEGM_ENCODE_RAW       0
#define TVP_XP4_SEGM_ENCODE_ZLIB      1

#define TVP_XP4_FILE_COMPRESSED 0x80	//!< not a real code; only used in tTVPXP4Writer related
										//!   (do not trust this flag, the actual
										//!    compression bit is in each segment flags)
#define TVP_XP4_FILE_MARKED		0x40
#define TVP_XP4_FILE_STATE_MASK	0x0f
#define TVP_XP4_FILE_ADDED		0
#define TVP_XP4_FILE_DELETED	1
#define TVP_XP4_FILE_MODIFIED	2
#define TVP_XP4_FILE_EXCLUDED	3
//---------------------------------------------------------------------------

#endif
