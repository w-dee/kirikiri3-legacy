//---------------------------------------------------------------------------
/*
	Risa [�肳]      alias �g���g��3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief �����O�o�b�t�@���������鎩�Ɛ��e���v���[�g�N���X
//---------------------------------------------------------------------------
#ifndef RingBufferH
#define RingBufferH

/*
	�����O�o�b�t�@, ring buffer, circular buffer, ��o�b�t�@
*/

//---------------------------------------------------------------------------
//! @brief		�Œ蒷�����O�o�b�t�@�̎���
//---------------------------------------------------------------------------
template <typename T>
class tRisaRingBuffer
{
	T * Buffer; //!< �o�b�t�@
	size_t Size; //!< �o�b�t�@�̃T�C�Y
	size_t WritePos; //!< �������݈ʒu
	size_t ReadPos; //!< �ǂݍ��݈ʒu
	size_t DataSize; //!< �o�b�t�@�ɓ����Ă���f�[�^�̃T�C�Y

public:
	//! @brief �R���X�g���N�^
	tRisaRingBuffer(size_t size)
	{
		Size = size:
		Buffer = new T[Size];
		WritePos = ReadPos = 0;
		DataSize = Size;
	}

	//! @brief �f�X�g���N�^
	~tRisaRingBuffer()
	{
		delete [] Buffer;
	}

	//! @brief	�T�C�Y�𓾂�
	size_t GetSize() { return Size; }

	//! @brief	�������݈ʒu�𓾂�
	size_t GetWritePos() { return WritePos; }

	//! @brief	�ǂݍ��݈ʒu�𓾂�
	size_t GetReadPos() { return ReadPos; }

	//! @brief	�o�b�t�@�ɓ����Ă���f�[�^�̃T�C�Y�𓾂�
	size_t GetDataSize() { return DataSize; }

	//! @brief	�o�b�t�@�̋󂫗e�ʂ𓾂�
	size_t GetFreeSize() { return Size - DataSize; }

	//! @brief	�o�b�t�@����ǂݍ��ނ��߂̃|�C���^�𓾂�
	//! @param	readsize �ǂݍ��݂����f�[�^��
	//! @param	p1		�u���b�N1�̐擪�ւ̃|�C���^���i�[���邽�߂̕ϐ�
	//! @param	p1size	p1�̕\���u���b�N�̃T�C�Y
	//! @param	p2		�u���b�N2�̐擪�ւ̃|�C���^���i�[���邽�߂̕ϐ�(NULL�����蓾��)
	//! @param	p2size	p2�̕\���u���b�N�̃T�C�Y(0�����蓾��)
	//! @note	��o�b�t�@�Ƃ����Ă��A���ۂ̓��j�A�ȗ̈�Ƀo�b�t�@���m�ۂ���Ă���B
	//!			���̂��߁A ReadPos + readsize ���o�b�t�@�̏I�[�𒴂��Ă���ꍇ�A������
	//!			�u���b�N�͂Q�ɕ��f����邱�ƂɂȂ�B
	//!			���̃��\�b�h�́Areadsize�����ۂɃo�b�t�@�ɓ����Ă���f�[�^�̃T�C�Y�ȉ��ł��邩
	//!			�Ȃǂ̃`�F�b�N�͂��������s��Ȃ��B���O�� GetDataSize �𒲂ׁA�ǂݍ��݂���
	//!			�T�C�Y�����ۂɃo�b�t�@�ɂ��邩�ǂ������`�F�b�N���邱�ƁB
	void GetReadPointer(size_t readsize,
						T * const & p1, size_t &p1size,
						T * const & p2, size_t &p2size)
	{
		if(readsize + ReadPos >= Size)
		{
			// readsize + ReadPos ���o�b�t�@�̏I�[�𒴂��Ă���
			//  �� �Ԃ����u���b�N��2��
			p1 = ReadPos + Buffer;
			p1size = Size - ReadPos;
			p2 = Buffer;
			p2size = readsize - p1size;
		}
		else
		{
			// readsize + ReadPos ���o�b�t�@�̏I�[�𒴂��Ă��Ȃ�
			//  �� �Ԃ����u���b�N��1��
			p1 = ReadPos + Buffer;
			p1size = readsize;
			p2 = NULL;
			p2size = 0;
		}
	}

	//! @brief	�ǂݍ��݃|�C���^��i�߂�
	//! @param	advance		�i�߂�v�f��
	//! @note	���̃��\�b�h�͎��ۂ� advance < GetDataSize() �ł��邱�Ƃ��m�F���Ȃ��B
	//!			�K�v�Ȃ�ΌĂяo�����Ń`�F�b�N���邱�ƁB
	void AdvanceReadPos(size_t advance)
	{
		ReadPos += advance;
		if(ReadPos >= Size) ReadPos -= Size;
		DataSize -= advance;
	}

	//! @brief	�o�b�t�@�ɏ������ނ̃|�C���^�𓾂�
	//! @param	writesize �������݂����f�[�^��
	//! @param	p1		�u���b�N1�̐擪�ւ̃|�C���^���i�[���邽�߂̕ϐ�
	//! @param	p1size	p1�̕\���u���b�N�̃T�C�Y
	//! @param	p2		�u���b�N2�̐擪�ւ̃|�C���^���i�[���邽�߂̕ϐ�(NULL�����蓾��)
	//! @param	p2size	p2�̕\���u���b�N�̃T�C�Y(0�����蓾��)
	//! @note	GetReadPointer�̐������Q�Ƃ̂���
	void GetWritePointer(size_t writesize,
						T * & p1, size_t &p1size,
						T * & p2, size_t &p2size)
	{
		if(writesize + WritePos >= Size)
		{
			// writesize + WritePos ���o�b�t�@�̏I�[�𒴂��Ă���
			//  �� �Ԃ����u���b�N��2��
			p1 = WritePos + Buffer;
			p1size = Size - WritePos;
			p2 = Buffer;
			p2size = writesize - p1size;
		}
		else
		{
			// writesize + WritePos ���o�b�t�@�̏I�[�𒴂��Ă��Ȃ�
			//  �� �Ԃ����u���b�N��1��
			p1 = WritePos + Buffer;
			p1size = writesize;
			p2 = NULL;
			p2size = 0;
		}
	}

	//! @brief	�������݃|�C���^��i�߂�
	//! @param	advance		�i�߂�v�f��
	//! @note	���̃��\�b�h�͎��ۂ� advance < GetFreeSize() �ł��邱�Ƃ��m�F���Ȃ��B
	//!			�K�v�Ȃ�ΌĂяo�����Ń`�F�b�N���邱�ƁB
	void AdvanceWritePos(size_t advance)
	{
		WritePos += advance;
		if(WritePos >= Size) WritePos -= Size;
		DataSize += advance;
	}


};

#endif
