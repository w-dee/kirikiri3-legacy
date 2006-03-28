//---------------------------------------------------------------------------
/*
	Risse [�肹]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief �����񑀍�
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseCxxString.h"

RISSE_DEFINE_SOURCE_ID(45632,47818,10920,18335,63117,13582,59145,24628);

/*! @note
Risse ������ɂ���

Risse ������� tRisseStringBlock �N���X�ŕ\�����B
���̃N���X�́Arisse_char * �^�̕������ێ�����B���̕������ \0 ���܂ނ�
�Ƃ͂ł��Ȃ��B

�ȉ��̐����ł̓R�[�h�|�C���g���u�����v�ƕ\�L����B

�� �\��

	tRisseStringBlock �͈ȉ��̓�̃����o�������Ă���B

	risse_char  *	Buffer;	// ������o�b�t�@
	risse_size		Length;	// ������ (�Ō�� \0 �͊܂߂Ȃ�)

	tRisseStringBlock �͂��̓�̃����o��p���A�uBuffer����n�܂�Length����
	��������������v��\���B
	�����̂Ȃ� (���) ������̏ꍇ�ABuffer �ɂ� NULL ������ALength �� 0 �ɂȂ�B


�� �o�b�t�@

	�o�b�t�@�� �Œ�ł� (Length + 2) * sizeof(risse_char) + sizeof(risse_size)
	�����B
	tRisseStringBlock ���C�ӂ̕���������ɍ쐬�����ꍇ�A
	(Length + 2) * sizeof(risse_char) + sizeof(size_t) �̒����̃o�b�t�@����
	���m�ۂ���A�����񒆂̊e�����͈ȉ��̂悤�ɔz�u�����B

	capacity \0 ����0 ����1 ����3 .... \0

	���̂悤�ɁA�ŏ��� capacity �Ƃ��ăo�b�t�@�Ɏ��ۂɊm�ۂ���Ă��钷����
	����A����ȍ~�� �e CP �̗��[�� \0 �������o�b�t�@�ƂȂ�B
	capacity �́A�o�b�t�@�̒������҂�����ł���� Length �Ɠ����ɂȂ�B

	tRisseStringBlock::Buffer �́A�ŏ��� capacity �� \0 �ł͂Ȃ��A���̎���
	����0(�܂蕶����̐擪) ���w���悤�ɂȂ�B

	cpacity �̎��� \0 �́A Buffer ������ tRisseStringBlock �Ƌ��L����Ă���
	�ꍇ�� \0 �ł͂Ȃ��Ȃ�B�Ō�� \0 �́A�I�[�� \0 �ł��邱�Ƃ����҂��Ă�
	�� C ����n�̊֐��ɓn���ۂ� NULL�I�[ ��\���B

�� �o�b�t�@�̋��L

	tRisseStringBlock �̃R�s�[�R���X�g���N�^�������Z�q�́A�o�b�t�@�̒��g��
	�R�s�[�����ABuffer �� Length �������R�s�[����B���̍ہA�o�b�t�@�����ł�
	���L���ꂽ���Ƃ�\�����߁ABuffer[-1] �� \0 �łȂ���΁ABuffer[-1] ��
	-1 ��������B����̓o�b�t�@�����L����Ă���\����\���B

�� ����������̋��L

	tRisseStringBlock �́A���̕�����̈ꕔ���w���������Ƃ��ł���B

	tRisseStringBlock a ���ȉ��̃o�b�t�@�������Ă���ꍇ�A

	4 \0 ����0 ����1 ����2 ����3 \0
	    ��
	   Buffer
	Length = 4

	����1 �` ����2 �̂Q������\�� tRisseStringBlock b �͈ȉ��̂悤�ɕ\����
	�Ƃ��ł���B

	4 -1 ����0 ����1 ����2 ����3 \0
	          ��
	        Buffer
	Length = 2

	���̏ꍇ���o�b�t�@�̋��L�Ɠ������A�o�b�t�@�̐擪�� \0 �� -1 �ɏ���������
	��A�o�b�t�@�����L����Ă��邱�Ƃ�\���B

�� �o�b�t�@�̋��L�̔���

	tRisseStringBlock ������ tRisseStringBlock �ƃo�b�t�@�����L���Ă���\
	�������邩�ǂ����𔻒f����ɂ́ABuffer[-1] �� \0 �łȂ����ǂ����������
	�悢�BBuffer[-1] �� �� \0 �ł���̓o�b�t�@�͋��L����Ă���\��������B

	tRisseStringBlock �͕���������L����ꍇ�ABuffer[-1] ���� \0 �̏ꍇ��
	 -1 �������邪�A����ɂ��o�b�t�@�����L����Ă���\����\���B

	tRisseStringBlock �͕�������������L����ꍇ�A�ꕶ����� Buffer[-1] ��
	�� \0�̏ꍇ�� -1 �������邵�A����������� Buffer[-1] �͕K�R�I�� \0 ��
	�Ȃ�B����́A���������񂪕ꕶ����̐擪���狤�L���Ă���Ȃ�΁ABuffer[-1]
	�͕ꕶ����Ɠ��� -1 �ɂȂ邵�A���������񂪕ꕶ����̓r�����狤�L���Ă���
	�Ȃ�΁ABuffer[-1] �͒��O�̕�����\���AtRisseStringBlock �͕����񒆂�
	\0 ���܂ނ��Ƃ͂Ȃ�����A����͔� \0 �ƂȂ�B

	�����͋��L�̉\����\�������ł���B�\���������Ă����ۂ͋��L����Ă�
	�Ȃ��ꍇ�����蓾��B

�� �o�b�t�@�̗e�ʂƎ��ۂ� Length

	+= ���Z�q�Ȃǂ� Length �������Ȃ�A�o�b�t�@���g�������ہA�o�b�t�@�͎�
	�ۂɕK�v�ȗe�ʂ������������߂Ɋm�ۂ���A���̊g�����Ƀo�b�t�@���ēx�m��
	���Ȃ��Ă��ςނ悤�ɂȂ�B
	���̍ہA���ۂɃo�b�t�@���i�[�\�� CP ����\���̂��A�o�b�t�@�̐擪��
	size_t �^�Ŋm�ۂ���Ă���̈�ł���B
	�����̗̈�́ABuffer[-1] �� 0 �̂Ƃ� (���L���Ă���\�����Ȃ��ꍇ)��
	�̂ݗL���Ȓl��ێ����Ă���ƍl����ׂ��ł���B���ۂ̂Ƃ���ABuffer[-1] ��
	0 �łȂ��ꍇ�� += �̂悤�ȃo�b�t�@�̓��e��j�󂷂鑀��ł͒��O��
	�o�b�t�@�̓��e�̃R�s�[���s���邽�߁A���̗̈悪�Q�Ƃ���邱�Ƃ͂Ȃ��B

�� Independ

	Independ ���\�b�h�́A�����񂪂��̃o�b�t�@�����L���Ă���\��������ꍇ�A
	�V���Ƀo�b�t�@���m�ۂ��A���e���R�s�[����B����ɂ��A�o�b�t�@�ɉ�����
	�X����������ł��A���̕�����ɉe�����y�΂Ȃ��悤�ɂ��邱�Ƃ��ł���B

	Independ �͐V���Ɋm�ۂ����o�b�t�@�̐擪�� \0 �ɂ��邪�A���̃o�b�t�@��
	���e�ɂ͎�������Ȃ��B���̃o�b�t�@�̓��e�́A����ɑ��̕����񂩂狤�L
	����Ă���\�������邪�A���ۂɋ��L����Ă���̂��A���邢�͂���Ă���
	���̂��́AtRisseStringBlock �������Ă����񂩂�͔���ł��Ȃ�����ł���B

�� c_str()

	c_str() �́A������ C ����n API �����҂���悤�ȁANULL �I�[�������Ԃ��B
	�����񂪑��̕�����̕����������\���Ă���ꍇ�A������̍Ōオ \0 �ł���
	�ۏ؂͂Ȃ����A���̂悤�ȏꍇ�́Ac_str() �͐V���Ƀo�b�t�@���m�ۂ��A�Ōオ
	\0 �ŏI�����Ă���o�b�t�@�����A�����Ԃ��B

�� �o�b�t�@�̉��

	�Q�Ƃ���Ȃ��Ȃ����o�b�t�@�́AGC �ɂ�莩���I�ɉ�������B

*/

namespace risse
{

//---------------------------------------------------------------------------
//! @brief -1, 0 �������Ă���z��(��̃o�b�t�@��\��)
//---------------------------------------------------------------------------
risse_char tRisseStringBlock::EmptyBuffer[2] = { tRisseStringBlock::MightBeShared, 0 };
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		�R���X�g���N�^(risse_char * ����)
//! @param		ref		���̕�����
//---------------------------------------------------------------------------
tRisseStringBlock::tRisseStringBlock(const risse_char * ref)
{
	if((Length = Risse_strlen(ref)) == 0)
	{
		Buffer = RISSE_STRING_EMPTY_BUFFER;
	}
	else
	{
		Buffer = AllocateInternalBuffer(Length);
		Risse_strcpy(Buffer, ref);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		�R���X�g���N�^(risse_char * ����A�R�[�h�|�C���g�������t��)
//! @param		ref		���̕�����
//! @param		n		�R�[�h�|�C���g��
//! @note		[ref, ref+n) �͈̔͂ɂ� \0 ���Ȃ�����
//---------------------------------------------------------------------------
tRisseStringBlock::tRisseStringBlock(const risse_char * ref, risse_size n)
{
	Length = n;
	if(n == 0)
	{
		Buffer = RISSE_STRING_EMPTY_BUFFER;
	}
	else
	{
		Buffer = AllocateInternalBuffer(Length);
		Buffer[n] = 0;
		memcpy(Buffer, ref, sizeof(risse_char) * n);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		�R���X�g���N�^(char * ����)
//! @param		ref		���̕�����
//---------------------------------------------------------------------------
tRisseStringBlock::tRisseStringBlock(const char * ref)
{
	Length = RisseUtf8ToRisseCharString(ref, NULL); // �R�[�h�|�C���g���𓾂�
//	if(Length == static_cast<risse_size>(-1L))
//		; /////////////////////////////////////////// TODO: ��O�𓊂���
	Buffer = AllocateInternalBuffer(Length);
	RisseUtf8ToRisseCharString(ref, Buffer);
	Buffer[Length] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		������̘A��
//! @param		ref		�A�����镶����
//! @return		���̃I�u�W�F�N�g
//---------------------------------------------------------------------------
tRisseStringBlock & tRisseStringBlock::opetator += (const tRisseStringBlock & ref)
{
	if(ref.Length == 0) return *this; // �ǉ�������̂Ȃ�
	if(Length == 0) return *this = ref; // �P���ȃR�s�[�ł悢

	risse_size newlength = Length + ref.Length;

	if(Buffer[-1])
	{
		// ���L�\���t���O�������Ă���
		// �V�����̈���m�ۂ��A�����ɃR�s�[����
		risse_char * newbuf = AllocateInternalBuffer(newlength);
		memcpy(newbuf, Buffer, Length * sizeof(risse_char));
		memcpy(newbuf + Length, ref.Buffer, ref.Length * sizeof(risse_char));
		Buffer = newbuf;
	}
	else
	{
		// ���L�\���t���O�͗����Ă��Ȃ�
		// ���݂̗̈���g������K�v������H
		if(GetBufferCapacity(Buffer) < newlength)
		{
			// �e�ʂ�����Ȃ��̂Ŋg������K�v����
			// �K���ɐV�K�m�ۂ̗e�ʂ��v�Z
			risse_size newcpacity;
			if(newlength < 16*1024)
				newcpacity = newlength * 2;
			else
				newcpacity = newlength + 16*1024;
			// �o�b�t�@���Ċm��
			Buffer = AllocateInternalBuffer(newcapacity, Buffer);
		}

		// ���ݕێ����Ă��镶����̒���� ref �� Buffer ���R�s�[����
		memcpy(Buffer + Length, ref.Buffer, ref.Length * sizeof(risse_char));
	}

	// null �I�[��ݒ肷��
	Length = newlength;
	newbuf[newlength] = 0;

	return *this;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		������̘A��
//---------------------------------------------------------------------------
tRisseStringBlock tRisseStringBlock::opetator +  (const tRisseStringBlock & ref) const
{
	if(Length == 0) return ref;
	if(ref.Length == 0) return *this;

	tRisseStringBlock newblock;
	risse_size newsize = Length + ref.Length;
	newblock.Allocate(newsize);
	memcpy(newblock.Buffer, Buffer, Length * sizeof(risse_char));
	memcpy(newblock.Buffer + Length, ref.Buffer, ref.Length * sizeof(risse_char));

	return newblock;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		n�̃R�[�h�|�C���g����Ȃ�o�b�t�@�����蓖�Ă�
//! @param		n	�R�[�h�|�C���g��
//! @param		prevbuf	�ȑO�̃o�b�t�@(�o�b�t�@���Ċm�ۂ���ꍇ)
//! @return		���蓖�Ă�ꂽ�o�b�t�@
//! @note		���ۂɂ� (n+2)*sizeof(risse_char) + sizeof(risse_size) �����蓖
//! �Ă��A2�Ԗڂ̕������w���|�C���^���A��B���L�\���t���O�̓N���A����A
//! �e�ʂ��������܂�邪�Anull�I�[�͏������܂�Ȃ��̂Œ��ӁB
//---------------------------------------------------------------------------
static risse_char * tRisseStringBlock::AllocateInternalBuffer(
	risse_size n, risse_char *prevbuf)
{
	// �o�b�t�@���m��
	size_t newbytes = sizeof(risse_size) + (n + 2)*sizeof(risse_char);
	void *ptr;
	if(!prevbuf)
		ptr = GC_malloc_atomic(newbytes);
	else
		ptr = GC_realloc(prevbuf, newbytes);

	// �Q�Ԗڂ̕������w���|�C���^���l��
	risse_char *  buffer = reinterpret_cast<risse_char*>(
		reinterpret_cast<char*>(ptr) +
				( sizeof(risse_char) + sizeof(risse_size) ) );

	// ���L�\���t���O�� 0 ��
	buffer[-1] = 0;

	// �m�ۗe�ʂ���������
	*reinterpret_cast<risse_size *>(ptr) = n;

	// ���ǂ�
	return buffer;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		������o�b�t�@���R�s�[���A�Ɨ�������
//! @return		�����o�b�t�@
//---------------------------------------------------------------------------
risse_char * tRisseStringBlock::InternalIndepend() const
{
	risse_char * newbuf = AllocateInternalBuffer(Length);
	memcpy(newbuf, Buffer, sizeof(risse_char) * Length);
	newbuf[Length] = 0;
	return Buffer = newbuf;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace risse
