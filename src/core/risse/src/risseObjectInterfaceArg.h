//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトの C++ インターフェースの引数
//---------------------------------------------------------------------------
#ifndef risseObjectInterafaceArgH
#define risseObjectInterafaceArgH

#include "risseOperateFlags.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseVariantBlock;
typedef tRisseVariantBlock tRisseVariant;
class tRisseStackFrameContext;
//---------------------------------------------------------------------------
//! @brief		Operate/Doメソッドへの引数(クラス宣言用)
//!	@note		Operate/Doメソッドの引数が変わるたびにすべてのOperate/Doメソッドの
//!				引数を変える気にはなれない。
//! 			あまりマクロは使いたくないが、それにしても
//!				インターフェースの仕様が固まるまではこうしたい
#define RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG \
		tRisseOpCode code,                                                 \
		tRisseVariant * result = NULL,                                     \
		const tRisseString & name = tRisseString::GetEmptyString(),        \
		tRisseOperateFlags flags = tRisseOperateFlags(),                   \
		const tRisseMethodArgument & args = tRisseMethodArgument::Empty(), \
		const tRisseVariant &This = tRisseVariant::GetNullObject()

//! @brief		Operate/Doメソッドへの引数(実装用)
#define RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG \
		tRisseOpCode code,                     \
		tRisseVariant * result,                \
		const tRisseString & name,             \
		tRisseOperateFlags flags,              \
		const tRisseMethodArgument & args,     \
		const tRisseVariant &This

//! @brief		Operate/Doメソッドの引数一覧
#define RISSE_OBJECTINTERFACE_PASS_ARG \
		code, result, name, flags, args, This

//---------------------------------------------------------------------------
}
#endif
