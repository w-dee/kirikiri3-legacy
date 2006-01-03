//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief �g���g��3�œ����I�ɗp���Ă����O�N���X�̊Ǘ��Ȃ�
//---------------------------------------------------------------------------
#ifndef _TVPExceptionH_
#define _TVPExceptionH_

#include "tjsError.h"

//---------------------------------------------------------------------------
//! @brief �g���g��3�p�̔ėp��O�N���X
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
