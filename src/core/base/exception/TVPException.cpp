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
#include "prec.h"
#include "TVPException.h"

TJS_DEFINE_SOURCE_ID(2100);

//---------------------------------------------------------------------------
//! @brief		eTVPException�^�̗�O�𓊂���
//---------------------------------------------------------------------------
void eTVPException::Throw(const ttstr &str)
{
	throw eTVPException(str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		eTVPException�^�̗�O�𓊂���
//! @param		str  ������ (���� %1 �Ȃǂ̎w�߂𖄂ߍ���)
//! @param		s1   �����񒆂� %1 �ƒu����������������
//---------------------------------------------------------------------------
void eTVPException::Throw(const ttstr &str, const ttstr & s1)
{
	throw eTVPException(ttstr(str, s1));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		eTVPException�^�̗�O�𓊂���
//! @param		str  ������ (���� %1 �Ȃǂ̎w�߂𖄂ߍ���)
//! @param		s1   �����񒆂� %1 �ƒu����������������
//! @param		s2   �����񒆂� %2 �ƒu����������������
//---------------------------------------------------------------------------
void eTVPException::Throw(const ttstr &str, const ttstr & s1, const ttstr & s2)
{
	throw eTVPException(ttstr(str, s1, s2));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		eTVPException�^�̗�O�𓊂���
//! @param		str  ������ (���� %1 �Ȃǂ̎w�߂𖄂ߍ���)
//! @param		s1   �����񒆂� %1 �ƒu����������������
//! @param		s2   �����񒆂� %2 �ƒu����������������
//! @param		s3   �����񒆂� %3 �ƒu����������������
//---------------------------------------------------------------------------
void eTVPException::Throw(const ttstr &str, const ttstr & s1, const ttstr & s2, const ttstr & s3)
{
	throw eTVPException(ttstr(str, s1, s2, s3));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		eTVPException�^�̗�O�𓊂���
//! @param		str  ������ (���� %1 �Ȃǂ̎w�߂𖄂ߍ���)
//! @param		s1   �����񒆂� %1 �ƒu����������������
//! @param		s2   �����񒆂� %2 �ƒu����������������
//! @param		s3   �����񒆂� %3 �ƒu����������������
//! @param		s4   �����񒆂� %4 �ƒu����������������
//---------------------------------------------------------------------------
void eTVPException::Throw(const ttstr &str, const ttstr & s1, const ttstr & s2, const ttstr & s3, const ttstr & s4)
{
	throw eTVPException(ttstr(str, s1, s2, s3, s4));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		�����G���[��O�𔭐�������
//! @param		line     �G���[�̋N�������s
//! @param		filename �G���[�̋N�������t�@�C����
//---------------------------------------------------------------------------
void eTVPException::ThrowInternalError(int line, const char * filename)
{
	Throw(TJS_WS_TR("Internal error at $2 line $1"), ttstr(line), ttstr(wxString(filename, wxConvUTF8)));
}
//---------------------------------------------------------------------------
