//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Null" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseNullClassH
#define risseNullClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Null" クラス
//---------------------------------------------------------------------------
class tNullClass : public tPrimitiveClassBase
{
	typedef tPrimitiveClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tNullClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public: // Risse用メソッドなど
	static void initialize(const tNativeCallInfo & info);
	static tString toString();
	static tString dump();
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif