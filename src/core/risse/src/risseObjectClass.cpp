/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Object" クラスの実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risseObjectClass.h"
#include "risseNativeFunction.h"
#include "risseStaticStrings.h"

/*
	Risseスクリプトから見える"Object" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(41134,45186,9497,17812,19604,2796,36426,21671);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectClass::tRisseObjectClass() : tRisseClassBase(NULL)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// デフォルトでは何もしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// デフォルトでは何もしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_getInstanceMember)
	{
		// 引数チェック
		if(args.GetArgumentCount() < 1) RisseThrowBadArgumentCount(args.GetArgumentCount(), 1);

		// This のインスタンスメンバを取得する
		tRisseVariant ret = 
			This.GetPropertyDirect(args[0],
				tRisseOperateFlags::ofInstanceMemberOnly|tRisseMemberAttribute(tRisseMemberAttribute::pcVar),
				This);
		if(result) *result = ret;
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_setInstanceMember)
	{
		// 引数チェック
		if(args.GetArgumentCount() < 2) RisseThrowBadArgumentCount(args.GetArgumentCount(), 2);

		// This のインスタンスメンバを設定する
		This.SetPropertyDirect(args[0],
			tRisseOperateFlags::ofInstanceMemberOnly|
			tRisseOperateFlags::ofMemberEnsure|
			tRisseMemberAttribute(tRisseMemberAttribute::pcVar),
						args[1], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_p)
	{
		for(risse_size i = 0; i < args.GetArgumentCount(); i++)
		{
			const tRisseVariant & v = args[i];

			if(v.GetType() == tRisseVariant::vtObject)
			{
				risse_char buf[40];
				Risse_pointer_to_str(v.GetObjectInterface(), buf);
				RisseFPrint(stdout, (tRisseString(RISSE_WS("Object@")) + buf).c_str());
				const tRisseVariant * context = v.GetContext();
				if(context)
				{
					if(context->GetType() == tRisseVariant::vtObject)
					{
						if(context == tRisseVariant::GetDotContext())
						{
							RisseFPrint(stdout, RISSE_WS(":dot"));
						}
						else
						{
							Risse_pointer_to_str(context->GetObjectInterface(), buf);
							RisseFPrint(stdout, (tRisseString(RISSE_WS(":")) + buf).c_str());
						}
					}
					else
					{
						RisseFPrint(stdout, (context->AsHumanReadable()).c_str());
					}
				}
			}
			else
			{
				RisseFPrint(stdout, (v.operator tRisseString()).c_str());
			}
		}
		RisseFPrint(stdout, RISSE_WS("\n"));
	}
	RISSE_END_NATIVE_METHOD
/*
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_puts)
	{
		// This を標準出力に出力する
		RisseFPrint(stdout, This.operator tRisseString().c_str());
		// 改行する
		RisseFPrint(stdout, RISSE_WS("\n"));
	}
	RISSE_END_NATIVE_METHOD
*/
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
//---------------------------------------------------------------------------

} /* namespace Risse */

