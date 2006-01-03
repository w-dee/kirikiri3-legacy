//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 吉里吉里3で内部的に用いている例外クラスの管理など
//---------------------------------------------------------------------------
#ifndef _TVPExceptionH_
#define _TVPExceptionH_

#include "tjsError.h"

//---------------------------------------------------------------------------
//! @brief 吉里吉里3用の汎用例外クラス
//---------------------------------------------------------------------------
class eTVPException : public eTJSError
{
public:
	eTVPException(const ttstr & msg) : eTJSError(msg) {;}
	eTVPException(const eTVPException & ref) : eTJSError(ref) {;}

	static void Throw(const ttstr &msg);
	static void Throw(const ttstr &msg, const ttstr & s1);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2, const ttstr & s3);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2, const ttstr & s3, const ttstr & s4);
};
//---------------------------------------------------------------------------

#endif
