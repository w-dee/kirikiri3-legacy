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
#include "risseSingleton.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseArrayClass : public tRisseClass, public tRisseSingleton<tRisseArrayClass>
{
public:
	//! @brief		コンストラクタ
	tRisseArrayClass();
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
