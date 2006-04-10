//---------------------------------------------------------------------------
/*
	Risse [�肹]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief �X�N���v�g�u���b�N�Ǘ�
//---------------------------------------------------------------------------
#ifndef risseScriptBlockH
#define risseScriptBlockH

#include "risseTypes.h"
#include "risseCxxString.h"
#include "risseVariant.h"
#include "risseAssert.h"
#include "gc_cpp.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		�X�N���v�g�u���b�N�̊��N���X
//---------------------------------------------------------------------------
class tRisseScriptBlockBase : public gc
{
protected:
	tRisseString Script; //!< �X�N���v�g�̓��e
	risse_size LineOffset; //!< �X�N���v�g�̍s�I�t�Z�b�g (�h�L�������g���ߍ��݃X�N���v�g�p)
	mutable risse_size * LinesToPosition; //!< �e�s�̐擪�ɑΉ�����R�[�h�|�C���g�ʒu�̔z��
	mutable risse_size LineCount; //!< �X�N���v�g�̍s��

protected:
	//! @brief		�R���X�g���N�^
	//! @param		script		�X�N���v�g�̓��e
	//! @param		lineofs		�s�I�t�Z�b�g(�h�L�������g���ߍ��݃X�N���v�g�p�ɁA
	//!							�X�N���v�g�̃I�t�Z�b�g���L�^�ł���)
	tRisseScriptBlockBase(const tRisseString & script, risse_size lineofs = 0);

	//! @brief		LinesToPosition �̓��e���쐬����
	void CreateLinesToPositionArary() const;

	//! @brief		LinesToPosition �̓��e���쐬����Ă��Ȃ���΍쐬����
	void EnsureLinesToPositionArary() const
	{ if(!LinesToPosition) CreateLinesToPositionArary(); }

public:
	//! @brief		�X�N���v�g�̓��e�𓾂�
	const tRisseString & GetScript() const { return Script; }

	//! @brief		�X�N���v�g�̃R�[�h�|�C���g�ʒu����s/���ʒu�ւ̕ϊ����s��
	//! @param		pos			�R�[�h�|�C���g�ʒu
	//! @param		line		�s�ʒu(0�`; �������Ȃ��ꍇ��null��)
	//! @param		col			���ʒu(0�`; �������Ȃ��ꍇ��null��)
	void PositionToLineAndColumn(risse_size pos, risse_size *line, risse_size *col) const;
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

