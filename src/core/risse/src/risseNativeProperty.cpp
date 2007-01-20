//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ネイティブプロパティを記述するためのサポート
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseNativeProperty.h"
#include "risseException.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(21996,23593,41879,16445,47022,10284,22019,34197);

//---------------------------------------------------------------------------
void tRisseNativePropertyBase::Read(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack)
{
	RisseThrowPropertyCannotBeRead();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNativePropertyBase::Write(
		const tRisseVariant & value,
		tRisseOperateFlags flags,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack)
{
	RisseThrowPropertyCannotBeWritten();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseNativePropertyBase::tRetValue tRisseNativePropertyBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するプロパティ読み込みか？
		{
			// このオブジェクトに対するプロパティ読み込みなので Read を呼ぶ
			Read(result, flags, This, stack);
			return rvNoError;
		}
		else if(code == ocDSet) // このオブジェクトに対するプロパティ書き込みか？
		{
			// このオブジェクトに対するプロパティ書き込みなので Write を呼ぶ
			if(args.GetCount() < 1) RisseThrowBadArgumentCount(args.GetCount(), 1);
			Write(args[0], flags, This, stack);
			return rvNoError;
		}
	}

	// そのほかの場合は親クラスのメソッドを呼ぶ
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------
} // namespace Risse
