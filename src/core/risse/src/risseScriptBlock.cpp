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

#include "prec.h"
#include "risseScriptBlock.h"
#include "risseException.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(10462,6972,23868,17748,24487,5141,43296,28534);


//---------------------------------------------------------------------------
tRisseScriptBlockBase::tRisseScriptBlockBase(const tRisseString & script, risse_size lineofs)
{
	// �t�B�[���h�̏�����
	LinesToPosition = NULL;
	Script = script;
	LineOffset = lineofs;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockBase::CreateLinesToPositionArary() const
{
	// �܂��A�S�̂̍s���𐔂���
	LineCount = 0;

	const risse_char *script = Script.c_str();
	const risse_char *ls = script;
	const risse_char *p = ls;
	while(*p)
	{
		if(*p == RISSE_WC('\r') || *p == RISSE_WC('\n'))
		{
			LineCount ++;
			if(*p == RISSE_WC('\r') && p[1] == RISSE_WC('\n')) p++;
			p++;
			ls = p;
		}
		else
		{
			p++;
		}
	}

	if(p != ls)
	{
		LineCount ++;
	}

	// ���������m��
	LinesToPosition = new (PointerFreeGC) risse_size[LineCount];

	// �e�s�̐擪�̃R�[�h�|�C���g�ʒu��z��ɓ���Ă���
	ls = script;
	p = ls;
	risse_size i = 0;
	while(*p)
	{
		if(*p == RISSE_WC('\r') || *p == RISSE_WC('\n'))
		{
			LinesToPosition[i++] = p - script;
			if(*p == RISSE_WC('\r') && p[1] == RISSE_WC('\n')) p++;
			p++;
			ls = p;
		}
		else
		{
			p++;
		}
	}

	if(p != ls)
	{
		LinesToPosition[i++] = p - script;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptBlockBase::PositionToLineAndColumn(risse_size pos,
						risse_size *line, risse_size *col) const
{
	// LinesToPosition �z�񂪍���Ă��Ȃ���΍��
	EnsureLinesToPositionArary();

	// �ړI�̈ʒu�� binary search �ŒT��
	risse_size s = 0;
	risse_size e = LineCount;
	while(true)
	{
		if(e-s <= 1)
		{
			if(line) *line = s + LineOffset; // LineOffset�����Z�����
			if(col) *col = pos - LinesToPosition[s];
			return;
		}
		risse_uint m = s + (e-s)/2;
		if(LinesToPosition[m] > pos)
			e = m;
		else
			s = m;
	}
}
//---------------------------------------------------------------------------

} // namespace Risse



