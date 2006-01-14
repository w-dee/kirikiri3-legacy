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

#include "risseError.h"

//---------------------------------------------------------------------------
//! @brief 吉里吉里3用の汎用例外クラス
//---------------------------------------------------------------------------
class eTVPException : public eRisseError
{
public:
	eTVPException(const ttstr & msg) : eRisseError(msg) {;}
	eTVPException(const eTVPException & ref) : eRisseError(ref) {;}

	static void Throw(const ttstr &msg);
	static void Throw(const ttstr &msg, const ttstr & s1);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2, const ttstr & s3);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2, const ttstr & s3, const ttstr & s4);

	static void ThrowInternalError(int line, const char * filename);
};
//---------------------------------------------------------------------------


#define TVPThrowInternalError eTVPException::ThrowInternalError(__LINE__, __FILE__)

#endif
