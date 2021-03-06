//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
class tVariant;
class tStackFrameContext;
//---------------------------------------------------------------------------
/**
 * Operate/Doメソッドへの引数(クラス宣言用)
 * @note	Operate/Doメソッドの引数が変わるたびにすべてのOperate/Doメソッドの
 *			引数を変える気にはなれない。
 *			あまりマクロは使いたくないが、それにしても
 *			インターフェースの仕様が固まるまではこうしたい
 */
#define RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG \
		tOpCode code,                                                 \
		tVariant * result = NULL,                                     \
		const tString & name = tString::GetEmptyString(),        \
		tOperateFlags flags = tOperateFlags(),                   \
		const tMethodArgument & args = tMethodArgument::Empty(), \
		const tVariant &This = tVariant::GetNullObject()

/**
 * Operate/Doメソッドへの引数(実装用)
 */
#define RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG \
		tOpCode code,                     \
		tVariant * result,                \
		const tString & name,             \
		tOperateFlags flags,              \
		const tMethodArgument & args,     \
		const tVariant &This

/**
 * Operate/Doメソッドの引数一覧
 */
#define RISSE_OBJECTINTERFACE_PASS_ARG \
		code, result, name, flags, args, This

//---------------------------------------------------------------------------
}
#endif
