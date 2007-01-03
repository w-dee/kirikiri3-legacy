//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトの C++ インターフェースの定義と実装
//---------------------------------------------------------------------------
#ifndef risseObjectH
#define risseObjectH

#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseGC.h"
#include "risseString.h"
#include "risseOpCodes.h"
#include "risseMethod.h"
#include "risseOperateRetValue.h"
#include "risseObjectInterfaceArg.h"
#include "risseVariant.h"

namespace Risse
{
class tRisseStackFrameContext;
class tRisseMethodArgument;
class tRisseVariantBlock;
typedef tRisseVariantBlock tRisseVariant;
//---------------------------------------------------------------------------
//! @brief		Risseオブジェクトインターフェース
//---------------------------------------------------------------------------
class tRisseObjectInterface : public tRisseCollectee, public tRisseOperateRetValue
{
public:
	//! @brief		オブジェクトに対して操作を行う
	//! @param		code	オペレーションコード
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		bargs	ブロック引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	//! @return		エラーコード
	//! @note		何か操作に失敗した場合は例外が発生する。ただし、tRetValueにあるような
	//!				エラーの場合は例外ではなくてエラーコードを返さなければならない。
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG) = 0;

	//! @brief		オブジェクトに対して操作を行う(失敗した場合は例外を発生させる)
	//! @param		code	オペレーションコード
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		bargs	ブロック引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	//! @note		何か操作に失敗した場合は例外が発生する。このため、このメソッドに
	//!				エラーコードなどの戻り値はない
	void Do(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
	{
		tRetValue ret = Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
		if(ret != rvNoError) RaiseError(ret, name);
	}
};
//---------------------------------------------------------------------------

}
#endif

