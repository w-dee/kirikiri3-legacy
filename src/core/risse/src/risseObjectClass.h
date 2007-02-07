//---------------------------------------------------------------------------
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

#ifndef risseObjectClassH
#define risseObjectClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseSingleton.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseObjectClass : public tRisseClassBase, public tRisseClassSingleton<tRisseObjectClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	tRisseObjectClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
