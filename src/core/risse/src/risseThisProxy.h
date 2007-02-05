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
//---------------------------------------------------------------------------
//! @brief		this-proxyクラス (オブジェクトAにメンバがなければオブジェクトBを見に行く特殊なオブジェクト)
//---------------------------------------------------------------------------
class tRisseThisProxy : public tRisseObjectInterface
{
	tRisseVariant & A; //!< オブジェクトA
	tRisseVariant & B; //!< オブジェクトB
public:
	//! @brief		コンストラクタ
	//! @param		a		オブジェクトA
	//! @param		b		オブジェクトB
	tRisseThisProxy(tRisseVariant & a, tRisseVariant & b) : A(a), B(b)
	{
	}

	//! @brief		tRisseObjectInterface::Operate() オーバーライド
	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
	{
		// まず A をみる
		tRetValue ret = A.Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
		if(ret == rvMemberNotFound)
		{
			// A にメンバがなかったのでBを見る
			ret = B.Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
		}
		return ret;
	}

};
//---------------------------------------------------------------------------
}
#endif

