//---------------------------------------------------------------------------
/*
	Risse [�肹]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief �o���A���g�^�̃f�[�^�����̎���
//---------------------------------------------------------------------------
#ifndef risseVariantDataH
#define risseVariantDataH

#include "risseGC.h"
#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseString.h"
#include "risseOctet.h"


namespace Risse
{
//---------------------------------------------------------------------------
class tRisseObjectInterface;
class tRissePrimitiveClassBase;
class tRisseVariantBlock;
//---------------------------------------------------------------------------
//! @brief	�o���A���g�^�̃f�[�^��
/*! @note

tRisseStringData, tRisseVariantBlock::tObject, tRisseOctetData �̊e�擪
�̃����o�͕K�������̃|�C���^�ł���B�����̓����o Type �ƃX�g���[�W�����L
����B���̃����o Type �Ƌ��L���s���Ă鏑���|�C���^�͂����͎��ۂɂ͂��ꂼ��
tRisseString, tRisseObject, tRisseOctet�Ƃ��Ĉ����邪�A�f�[�^�����o�̃��C
�A�E�g�͓���ł���B

�e�|�C���^��4�̔{���̃A�h���X�ɂ����z�u����Ȃ����Ƃ��������A���P�[�^�̎d�l
����� risse_char �̃T�C�Y�ɂ��ۏ؂���Ă���B���̂��߁A����2�r�b�g�͕K���V
��ł��邱�ƂɂȂ�B�܂��A0x10 �����̂悤�ȋɒ[�ɒႢ�Ԓn�ɂ����̃|�C���^
���z�u����邱�Ƃ͂��蓾�Ȃ��B

���̂��߁AtRisseVariant::GetType() ������΂킩��Ƃ���AType �� 4 �ȏ�
�Ȃ�Ή���2�r�b�g+4�� Type �Ƃ��A4 �����Ȃ�΂�������̂܂� Type �Ƃ��ĕ�
���Ă���B������ӂ� ruby �̎�������q���g�𓾂����B

tRisseString, tRisseVariantBlock::tObject, tRisseOctet ���ɂ���e�́u�{��
�́v�|�C���^��I��ɂ́A~0x03 �Ƃ� bit and ���Ƃ�΂悢�B

tRisseString �̓����|�C���^���w�������Ă���ꏊ�́A�������ێ����Ă���o�b
�t�@�ł���BRisse�ł�UTF-32�������ΏۂƂ��邽�߁A���̃|�C���^�� 32bit���E
�ɂ����Ȃ����Ƃ͂Ȃ��B�f�o�b�K�Ȃǂł��̂܂� UTF-32 �������\���������p
�r������BtRisseString �� Type �̏����� 0x04 �ȏォ����2�r�b�g��00
�ł��邽�߁A�����|�C���^�̒l�Ɩ{���w�������Ă���|�C���^�͓����ɂȂ�B�����
���A�����|�C���^���w�������|�C���^�����̂܂ܕ�����o�b�t�@�̃|�C���^�ƂȂ�A
���̂܂܃f�o�b�K�Ȃǂœ��e��\���ł���B

�Ƃ肠���� tRiseVariant �̃T�C�Y��}�����������߂̋���̍�B����Ȃ��Ƃ�����
������������������Ȃ����A������������������������Ȃ��B 

tRisseVariant �̓p�t�H�[�}���X�̊֌W��AILP32 �V�X�e���ł� 3 * 32bit, LP64 �V�X
�e���ł� 2 * 64bit �Ɏ��܂�悤�ɂ��邱�ƁB
*/
//---------------------------------------------------------------------------
class tRisseVariantData : public tRisseCollectee
{
protected:

	//! @brief void �X�g���[�W�^
	struct tVoid
	{
		risse_ptruint Type; //!< �o���A���g�^�C�v: 0 �Œ�
	};

	//! @brief integer �X�g���[�W�^
	struct tInteger
	{
		risse_ptruint Type; //!< �o���A���g�^�C�v: 1 �Œ�
		risse_int64  Value; //!< �l
	};

	//! @brief real �X�g���[�W�^
	struct tReal
	{
		risse_ptruint Type; //!< �o���A���g�^�C�v: 2 �Œ�
		risse_real Value; //!< �l
	};

	//! @brief boolean �X�g���[�W�^
	struct tBoolean
	{
		risse_ptruint Type; //!< �o���A���g�^�C�v: 3 �Œ�
		bool Value; //!< �l
	};

	//! @brief object �X�g���[�W�^
	struct tObject
	{
		tRisseObjectInterface * Intf; //!< �I�u�W�F�N�g�C���^�[�t�F�[�X�ւ̃|�C���^(���ʂ�2�r�b�g�͏��10)
		const tRisseVariantBlock * Context;
						//!< (Intf�����\�b�h�I�u�W�F�N�g��v���p�e�B�I�u�W�F�N�g��
						//!< �w���Ă���Ƃ���)���\�b�h�����삷��R���e�L�X�g
	};
	#define RISSE_OBJECT_NULL_PTR (reinterpret_cast<tRisseObjectInterface*>((risse_ptruint)0x10))

	//! @brief Integer�^�ւ̎Q�Ƃ��擾 @return Integer�^�t�B�[���h�ւ̎Q��
	risse_int64 & AsInteger() { return reinterpret_cast<tInteger*>(Storage)->Value; }
	//! @brief Integer�^�ւ�const�Q�Ƃ��擾 @return Integer�^�t�B�[���h�ւ�const�Q��
	const risse_int64 & AsInteger() const { return reinterpret_cast<const tInteger*>(Storage)->Value; }

	//! @brief Real�^�ւ̎Q�Ƃ��擾 @return Real�^�t�B�[���h�ւ̎Q��
	risse_real & AsReal() { return reinterpret_cast<tReal*>(Storage)->Value; }
	//! @brief Real�^�ւ�const�Q�Ƃ��擾 @return Real�^�t�B�[���h�ւ�const�Q��
	const risse_real & AsReal() const { return reinterpret_cast<const tReal*>(Storage)->Value; }

	//! @brief Boolean�^�ւ̎Q�Ƃ��擾 @return Boolean�^�t�B�[���h�ւ̎Q��
	bool & AsBoolean() { return reinterpret_cast<tBoolean*>(Storage)->Value; }
	//! @brief Boolean�^�ւ�const�Q�Ƃ��擾 @return Boolean�^�t�B�[���h�ւ�const�Q��
	const bool & AsBoolean() const { return reinterpret_cast<const tBoolean*>(Storage)->Value; }

	//! @brief String�^�ւ̎Q�Ƃ��擾 @return String�^�t�B�[���h�ւ̎Q��
	tRisseString & AsString() { return *reinterpret_cast<tRisseString*>(Storage); }
	//! @brief String�^�ւ�const�Q�Ƃ��擾 @return String�^�t�B�[���h�ւ�const�Q��
	const tRisseString & AsString() const { return *reinterpret_cast<const tRisseString*>(Storage); }

	//! @brief Octet�^�ւ̎Q�Ƃ��擾 @return Octet�^�t�B�[���h�ւ̎Q��
	tRisseOctet & AsOctet() { return *reinterpret_cast<tRisseOctet*>(Storage); }
	//! @brief Octet�^�ւ�const�Q�Ƃ��擾 @return Octet�^�t�B�[���h�ւ�const�Q��
	const tRisseOctet & AsOctet() const { return *reinterpret_cast<const tRisseOctet*>(Storage); }

	//! @brief Object�^�ւ̎Q�Ƃ��擾 @return Object�^�t�B�[���h�ւ̎Q��
	tObject & AsObject() { return *reinterpret_cast<tObject*>(Storage); }
	//! @brief Object�^�ւ�const�Q�Ƃ��擾 @return Object�^�t�B�[���h�ւ�const�Q��
	const tObject & AsObject() const { return *reinterpret_cast<const tObject*>(Storage); }

public:
	//! @brief tRisseObjectInterface�ւ̃|�C���^���擾 @return tRisseObjectInterface�ւ̃|�C���^
	//! @note Intf��������ꍇ�͏�ɂ��̃��\�b�h���g������
	tRisseObjectInterface * GetObjectInterface() const
	{
		RISSE_ASSERT(GetType() == vtObject);
		tRisseObjectInterface * ret = reinterpret_cast<tRisseObjectInterface*>(
			reinterpret_cast<risse_ptruint>(AsObject().Intf) - 2);
		// 2 = Intf �̉���2�r�b�g�͏��10�Ȃ̂ŁA��������ɖ߂�
		if(ret == RISSE_OBJECT_NULL_PTR) return NULL;
			// "null"�������Ă����ꍇ��RISSE_OBJECT_NULL_PTR��������̂ł�����NULL��Ԃ�
		return ret;
	}

protected:
	//! @brief tRisseObjectInterface�ւ̃|�C���^��ݒ� @param intf tRisseObjectInterface�ւ̃|�C���^
	//! @note Intf��������ꍇ�͏�ɂ��̃��\�b�h���g������
	void SetObjectIntf(tRisseObjectInterface * intf)
	{
		RISSE_ASSERT(GetType() == vtObject);
		if(!intf) intf = RISSE_OBJECT_NULL_PTR;
			// "null"�̑����RISSE_OBJECT_NULL_PTR���g��
		AsObject().Intf = reinterpret_cast<tRisseObjectInterface*>(
			reinterpret_cast<risse_ptruint>(intf) + 2);
		// 2 = Intf �̉���2�r�b�g�͏��10�Ȃ̂ŁA���������
	}



	#define RV_SIZE_MAX(a, b) ((a)>(b)?(a):(b))
	#define RV_STORAGE_SIZE \
			RV_SIZE_MAX(sizeof(risse_ptruint),\
			RV_SIZE_MAX(sizeof(tVoid),        \
			RV_SIZE_MAX(sizeof(tInteger),     \
			RV_SIZE_MAX(sizeof(tReal),        \
			RV_SIZE_MAX(sizeof(tBoolean),     \
			RV_SIZE_MAX(sizeof(tRisseString), \
			RV_SIZE_MAX(sizeof(tRisseOctet),  \
			RV_SIZE_MAX(sizeof(tObject),      \
					4                         \
			 ))))))))
			// �� 4 �̓_�~�[


	//! @brief �e�o���A���g�̓����^�� union
	union
	{
		risse_ptruint Type;	//!< �o���A���g�^�C�v

		//! @brief	�f�[�^�X�g���[�W
		//! @note
		//! �킴�킴�}�N���Ŋe�\���̂̃T�C�Y�̍ő�l�𓾂āA���̗v�f����
		//! ������ char �z����m�ۂ��Ă��邪(RV_STORAGE_SIZE�̐錾���Q��)�A
		//! ����� gcc �Ȃ� union �ō\���̂�z�u����ۂɁA�]�܂����Ȃ��p�b�L���O��
		//! �s���Ă��܂��\�������邽�߁B
		char Storage[RV_STORAGE_SIZE];
	};

public:
	//! @brief �o���A���g�̃^�C�v
	enum tType
	{
		vtVoid			= 0,
		vtInteger		= 1,
		vtReal			= 2,
		vtBoolean		= 3,
		vtString		= 4 + 0,
		vtOctet			= 4 + 1,
		vtObject		= 4 + 2,
	//	vtReserved		= 4 + 3,
	};

	//! @brief �o���A���g�̃^�C�v�𓾂�
	//! @return �o���A���g�̃^�C�v
	tType GetType() const
	{
		return static_cast<tType>((Type & 3) + ( (Type >= 4) << 2 ));

		// ��L�̍s�͈ȉ���2�s�Ɠ���
		//	if(Type >= 4) return static_cast<tType>((Type & 3) + 4);
		//	return static_cast<tType>(Type);
	}

	//! @brief �o���A���g��void���ǂ����𓾂�
	//! @return �o���A���g��void���ǂ���
	bool IsVoid() const
	{
		return Type == static_cast<risse_ptruint>(vtVoid);
		// ��L�̍s�͈ȉ��̍s�Ɠ���
		// return GetType() == vtVoid;
	}

	//! @brief		���e������������ (void �ɂ���)
	//! @note		���̃��\�b�h�� Type �� vtVoid �ɂ��邱�Ƃɂ��
	//!				�^�� void �ɂ��邾���ł���B�����̃����o���ێ����Ă���|�C���^�Ȃǂ�
	//!				�j�󂷂�킯�ł͂Ȃ��̂ŁA�Q�Ƃ͂��܂��ێ����ꂽ�܂܂ɂȂ�\��
	//!				�����邱�Ƃɒ��ӂ��邱�ƁB
	void Clear()
	{
		Type = vtVoid;
	}
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
