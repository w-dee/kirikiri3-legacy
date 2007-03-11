//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tRisseVariantやtRisseObjectInterfaceのOperateメソッドの戻り値処理
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseObject.h"
#include "risseExceptionClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(52663,54398,40985,18806,48826,65290,5179,62065);
//---------------------------------------------------------------------------
void tRisseOperateRetValue::RaiseError(tRetValue ret, const tRisseString & name)
{
	switch(ret)
	{
	case rvMemberNotFound:
		// "メンバが見つからない"エラー
		tRisseNoSuchMemberExceptionClass::Throw(name);
		break;

	case rvMemberIsReadOnly:
		// "読み出し専用メンバに上書きしようとした"エラー
		tRisseIllegalMemberAccessExceptionClass::ThrowMemberIsReadOnly(name);
		break;

	case rvMemberIsFinal:
		// "finalメンバをオーバーライドしようとした"エラー
		tRisseIllegalMemberAccessExceptionClass::ThrowMemberIsFinal(name);
		break;

	case rvPropertyCannotBeRead:
		// "このプロパティからは読み込むことができない"エラー
		tRisseIllegalMemberAccessExceptionClass::ThrowPropertyCannotBeRead(name);
		break;

	case rvPropertyCannotBeWritten:
		// "このプロパティには書き込むことができない"エラー
		tRisseIllegalMemberAccessExceptionClass::ThrowPropertyCannotBeWritten(name);
		break;

	case rvNoError:
		break;

	}
}
//---------------------------------------------------------------------------
}

