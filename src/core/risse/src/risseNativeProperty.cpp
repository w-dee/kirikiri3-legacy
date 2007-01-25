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
tRisseNativePropertyBase::tRisseNativePropertyBase(tRisseNativePropertyBase::tGetter getter,
		tRisseNativePropertyBase::tSetter setter)
{
	Getter = getter;
	Setter = setter;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseNativePropertyBase::tRetValue tRisseNativePropertyBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するプロパティ読み込みか？
		{
			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(!Getter) RisseThrowPropertyCannotBeRead();
			Getter(result, flags, This);
			return rvNoError;
		}
		else if(code == ocDSet) // このオブジェクトに対するプロパティ書き込みか？
		{
			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			if(!Setter) RisseThrowPropertyCannotBeWritten();
			if(args.GetArgumentCount() < 1) RisseThrowBadArgumentCount(args.GetArgumentCount(), 1);
			Setter(args[0], flags, This);
			return rvNoError;
		}
	}

	// そのほかの場合は親クラスのメソッドを呼ぶ
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------
} // namespace Risse
