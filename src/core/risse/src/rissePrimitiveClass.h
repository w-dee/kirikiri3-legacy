//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseのプリミティブ型用クラス関連の処理の実装
//---------------------------------------------------------------------------

#ifndef rissePrimitiveClassH
#define rissePrimitiveClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseSingleton.h"
#include "risseGC.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		プリミティブ型用クラスの基底クラス
//---------------------------------------------------------------------------
class tRissePrimitiveClass : public tRisseClass
{
	tRisseVariant Gateway;
		//!< ゲートウェイアクセス用の代表オブジェクト(すべてのプリミティブへのアクセスは
		//!< このオブジェクトを経由して行われる
public:
	//! @brief		コンストラクタ
	tRissePrimitiveClass();

	//! @brief		ゲートウェイアクセス用の代表オブジェクトを得る
	//! @return		ゲートウェイアクセス用の代表オブジェクト
	tRisseVariant & GetGateway() { return Gateway; }
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
