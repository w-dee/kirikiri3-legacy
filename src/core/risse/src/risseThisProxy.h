//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief this-proxy (オブジェクトAにメンバがなければオブジェクトBを見に行く特殊なオブジェクト)
//---------------------------------------------------------------------------
#ifndef risseThisProxyH
#define risseThisProxyH

#include "risseObject.h"

namespace Risse
{
class tRisseScriptEngine;
//---------------------------------------------------------------------------
//! @brief		this-proxyクラス (オブジェクトAにメンバがなければオブジェクトBを見に行く特殊なオブジェクト)
//---------------------------------------------------------------------------
class tRisseThisProxy : public tRisseObjectInterface
{
	tRisseVariant & A; //!< オブジェクトA
	tRisseVariant & B; //!< オブジェクトB
	tRisseScriptEngine * Engine; //!< スクリプトエンジンインスタンス
public:
	//! @brief		コンストラクタ
	//! @param		a		オブジェクトA
	//! @param		b		オブジェクトB
	//! @param		engine	スクリプトエンジンインスタンス
	tRisseThisProxy(tRisseVariant & a, tRisseVariant & b, tRisseScriptEngine * engine) :
		A(a), B(b), Engine(engine)
	{
	}

	//! @brief		tRisseObjectInterface::Operate() オーバーライド
	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
	{
		// まず A をみる
		tRetValue ret = A.Operate(Engine, RISSE_OBJECTINTERFACE_PASS_ARG);
		if(ret == rvMemberNotFound)
		{
			// A にメンバがなかったのでBを見る
			ret = B.Operate(Engine, RISSE_OBJECTINTERFACE_PASS_ARG);
		}
		return ret;
	}

};
//---------------------------------------------------------------------------
}
#endif

