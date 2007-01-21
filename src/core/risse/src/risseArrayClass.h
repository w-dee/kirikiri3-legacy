//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Array" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseArrayClassH
#define risseArrayClassH

#include "risseObject.h"
#include "risseClass.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseArrayClass : public tRisseClass
{
public:
	//! @brief		コンストラクタ
	//! @param		super		スーパークラス(Objectクラスのクラスインスタンスを指定すること)
	tRisseArrayClass(const tRisseVariant & super);
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
