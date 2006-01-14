//---------------------------------------------------------------------------
/*
	Risa [�肳]      alias �g���g��3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief �g���g��3�œ����I�ɗp���Ă����O�N���X�̊Ǘ��Ȃ�
//---------------------------------------------------------------------------
#ifndef _RisaExceptionH_
#define _RisaExceptionH_

#include "risseError.h"

//---------------------------------------------------------------------------
//! @brief �g���g��3�p�̔ėp��O�N���X
//---------------------------------------------------------------------------
class eRisaException : public eRisseError
{
public:
	eRisaException(const ttstr & msg) : eRisseError(msg) {;}
	eRisaException(const eRisaException & ref) : eRisseError(ref) {;}

	static void Throw(const ttstr &msg);
	static void Throw(const ttstr &msg, const ttstr & s1);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2, const ttstr & s3);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2, const ttstr & s3, const ttstr & s4);

	static void ThrowInternalError(int line, const char * filename);
};
//---------------------------------------------------------------------------


#define RisaThrowInternalError eRisaException::ThrowInternalError(__LINE__, __FILE__)

#endif
