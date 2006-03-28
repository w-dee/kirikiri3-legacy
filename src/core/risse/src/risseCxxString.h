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


namespace risse
{
//---------------------------------------------------------------------------
//! @brief	������u���b�N
//---------------------------------------------------------------------------
class tRisseStringBlock : public gc
{
	mutable risse_char  *	Buffer;	//!< ������o�b�t�@
	risse_size				Length;	//!< ������ (�Ō�� \0 �͊܂߂Ȃ�)

	const risse_char MightBeShared  = static_cast<risse_char>(-1L);
		//!< ���L�\���t���O�Ƃ��� Buffer[-1] �ɐݒ肷��l

	static risse_char EmptyBuffer[2];
		//!< -1, 0 �������Ă���z��(��̃o�b�t�@��\��)
	#define RISSE_STRING_EMPTY_BUFFER (EmptyBuffer+1)

public:
	//! @brief �f�t�H���g�R���X�g���N�^
	tRisseStringBlock()
	{
		// TODO: gc �̓�������0�ŃN���A����H�Ȃ�Έȉ��̑���͕s�v
		Buffer = RISSE_STRING_EMPTY_BUFFER;
		Length = 0;
	}

	//! @brief �R�s�[�R���X�g���N�^
	//! @param ref �R�s�[���I�u�W�F�N�g
	tRisseStringBlock(const tRisseStringBlock & ref)
	{
		*this = ref;
	}

	//! @brief �������������邽�߂̃R���X�g���N�^
	//! @param ref		�R�s�[���I�u�W�F�N�g
	//! @param offset	�؂�o���J�n�ʒu
	//! @param length	�؂�o������
	tRisseStringBlock(const tRisseStringBlock & ref, risse_size offset, risse_size length)
	{
		if(length)
		{
			RISSE_ASSERT(ref.Length - offset >= length);
			if(ref.Buffer[-1] == 0)
				ref.Buffer[-1] = MightBeShared; // ���L�\���t���O�����Ă�
			Buffer = ref.Buffer + offset;
			Length = length;
		}
		else
		{
			Buffer = RISSE_STRING_EMPTY_BUFFER;
			Length = 0;
		}
	}

	tRisseStringBlock(const risse_char * ref);
	tRisseStringBlock(const risse_char * ref, risse_size n);
	tRisseStringBlock(const char * ref);

	//! @brief	������Z�q
	//! @param	ref	�R�s�[���I�u�W�F�N�g
	//! @return	���̃I�u�W�F�N�g�ւ̎Q��
	tRisseStringBlock & operator = (const tRisseStringBlock & ref)
	{
		if(ref.Buffer[-1] == 0)
			ref.Buffer[-1] = MightBeShared; // ���L�\���t���O�����Ă�
		Buffer = ref.Buffer;
		Length = ref.Length;
		return *this;
	}

	//! @brief	�o�b�t�@���R�s�[���A�V���� tRisseStringBlock ��Ԃ�
	//! @param	ref	�R�s�[���I�u�W�F�N�g
	//! @return	�V���� tRisseStringBlock �I�u�W�F�N�g
	//! @note	���̃��\�b�h�́A�K���o�b�t�@���R�s�[���ĕԂ��A
	//!			���̕����� (���̕�����) �̋��L��ԂȂǂ͂�������
	//!			�ύX���Ȃ��B�j���O��Ƃ���������𑼂̕�����
	//!			����쐬����ꍇ�ȂǂɌ����I�B
	tRisseStringBlock MakeBufferCopy(const tRisseStringBlock & ref) const
	{
		return tRisseStringBlock(Buffer, Length);
	}

public: // object property
	//! @brief ������̒����𓾂�
	//! @return	������̒���(�R�[�h�|�C���g�P��) (\0 �͊܂܂�Ȃ�)
	risse_size GetLength() const { return Length; }

	//! @brief ������̒�����ݒ肷��(�؂�߂̂݉�)
	//! @param	n �V��������(�R�[�h�|�C���g�P��)
	void SetLength(risse_size n) const
	{
		RISSE_ASSERT(n <= Length);
		Independ();
		Length = n;
		if(n)
			Buffer[n] = 0;
		else
			Buffer = RISSE_STRING_EMPTY_BUFFER; // Buffer �����
	}

public: // comparison
	//! @brief < ���Z�q
	//! @param	ref		��r����I�u�W�F�N�g
	//! @return	*this<ref���ǂ���
	bool operator <  (const tRisseStringBlock & ref) const
	{
	}

	//! @brief > ���Z�q
	//! @param	ref		��r����I�u�W�F�N�g
	//! @return	*this>ref���ǂ���
	bool operator >  (const tRisseStringBlock & ref) const
		{ return ref < *this; }

	//! @brief <= ���Z�q
	//! @param	ref		��r����I�u�W�F�N�g
	//! @return	*this<=ref���ǂ���
	bool operator <= (const tRisseStringBlock & ref) const
		{ return ! (*this > ref); }

	//! @brief >= ���Z�q
	//! @param	ref		��r����I�u�W�F�N�g
	//! @return	*this>=ref���ǂ���
	bool operator >= (const tRisseStringBlock & ref) const
		{ return ! (*this < ref); }

	//! @brief �����r
	//! @param	ref		��r����I�u�W�F�N�g
	//! @return	*this==ref���ǂ���
	bool operator == (const tRisseStringBlock & ref) const
	{
		if(this == &ref) return true; // �����|�C���^
		if(Buffer == ref.Buffer) return true; // �����o�b�t�@
		if(Length != ref.Length) return false; // �Ⴄ����
		return !Risse_strcmp(Buffer, ref.Buffer);
	}

	//! @brief �s��v����
	//! @param	ref		��r����I�u�W�F�N�g
	//! @return	*this!=ref���ǂ���
	bool operator != (cosnt tRisseStringBlock & ref) const
		{ return ! (*this == ref); }

public: // operators
	tRisseStringBlock & opetator += (const tRisseStringBlock & ref);
	tRisseStringBlock opetator +  (const tRisseStringBlock & ref) const;

	//! @brief [] ���Z�q
	//! @param		n		�ʒu
	//! @return		n�̈ʒu�ɂ���R�[�h
	risse_char operator [] (risse_size n) const
	{
		RISSE_ASSERT(n < Length);
		return Buffer[n];
	}

private:
	static risse_char * AllocateInternalBuffer(risse_size n, risse_char *prevbuf = NULL);


	//! @brief		�o�b�t�@�Ɋ��蓖�Ă��Ă���R�[�h�|�C���g��(�e��)�𓾂�
	//! @param		buffer �o�b�t�@
	//! @return		�R�[�h�|�C���g��
	//! @note		Buffer[-1] �� 0 �̎��݂̂ɂ��̃��\�b�h���ĂԂ��ƁB
	//!				����ȊO�̏ꍇ�͕Ԓl�͐M�p���Ă͂Ȃ�Ȃ��B
	static risse_size GetBufferCapacity(const risse_char * buffer)
	{
		return
			*reinterpret_cast<const risse_size *>(
				reinterpret_cast<const char *>(buffer) -
					(sizeof(risse_char) + sizeof(risse_size)));
	}

public: // pointer
	//! @brief �o�b�t�@�����蓖�Ă�
	//! @param  n �o�b�t�@�Ɋ��蓖�Ă镶���� (�Ō�� \0 �͊܂܂Ȃ�)
	//! @return	������o�b�t�@
	//! @note ���̃��\�b�h���g������A���� n �ƈقȂ�
	//! �������������񂾏ꍇ�́AFixLength ���邢��
	//! SetLength ���ĂԂ��ƁB
	risse_char * Allocate(risse_size n)
	{
		if(n)
			Buffer = AllocateInternalBuffer(n), Buffer[n] = 0;
		else
			Buffer = RISSE_STRING_EMPTY_BUFFER;
		Length = n;
	}

	//! @brief �����Ŏ����Ă��镶����̒������A���ۂ̒����ɍ��킹��
	void FixLength()
	{
		if((Length = Risse_strlen(Buffer)) == 0)
			Buffer = RISSE_STRING_EMPTY_BUFFER;
	}

	//! @brief C ����X�^�C���̃|�C���^�𓾂�
	//! @note  tRisseStringBlock �͓����ɕێ����Ă���o�b�t�@�̍Ōオ \0 ��
	//! �I����ĂȂ��ꍇ�́A�o�b�t�@��V���ɃR�s�[���� \0 �ŏI��点�A����
	//! �o�b�t�@�̃|�C���^��Ԃ��B�܂��A�󕶎���̏ꍇ�� NULL ��Ԃ�����
	//! "" �ւ̃|�C���^��Ԃ��B
	const risse_char * c_str() const
	{
		if(Buffer[Length]) return Independ();
		return Buffer;
	}

	//! @brief �����o�b�t�@�̃|�C���^��Ԃ�
	//! @return �����o�b�t�@�̃|�C���^
	//! @note  ���̃��\�b�h�ŕԂ����|�C���^�́A���΂���
	//! ���҂����ʒu�� \0 ���Ȃ� (null�I�����Ă���ۏ؂��Ȃ�)
	const risse_char * Pointer() const { return Buffer; }

	//! @brief ������o�b�t�@���R�s�[���A�Ɨ�������
	//! @return �����o�b�t�@
	//! @note tRisseStringBlock �͈�̃o�b�t�@�𕡐��̕�����C���X�^���X��
	//! ���L����ꍇ�����邪�A���̃��\�b�h�͋��L��؂�A������o�b�t�@��
	//! �Ɨ�����BRisse �� GC �̓�����A���̕����񂪂��łɓƗ����Ă��邩�ǂ�����
	//! �m���ɒm�邷�ׂ͂Ȃ��A���̃��\�b�h�͂��Ȃ�̊m���Ńo�b�t�@���R�s�[���邽�߁A
	//! ���s�������ɂȂ�ꍇ�����邱�Ƃɒ��ӂ��邱�ƁB
	//! ���̃��\�b�h�͓����o�b�t�@�ւ̃|�C���^��Ԃ����A���̃o�b�t�@�ɁA�������Ƃ�
	//! ���������Z�������̕�������������񂾏ꍇ�́AFixLength ���邢��
	//! SetLength ���ĂԂ��ƁB
	//! ���̃��\�b�h�́A���e����̎��͓Ɨ����s��Ȃ��� NULL ��Ԃ�
	risse_char * Independ() const
	{
		if(Buffer[-1]) // ���L�\���t���O�������Ă���H
			return InternalIndepend();
		return Buffer;
	}

private:
	risse_char * InternalIndepend() const;

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace risse