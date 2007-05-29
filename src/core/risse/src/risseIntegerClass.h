//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Integer" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseIntegerClassH
#define risseIntegerClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Integer" クラス
//---------------------------------------------------------------------------
class tRisseIntegerClass : public tRissePrimitiveClassBase
{
	typedef tRissePrimitiveClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseIntegerClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();

public: // Risse用メソッドなど
	static void initialize(const tRisseNativeBindFunctionCallingInfo & info);
	static void times(const tRisseNativeBindFunctionCallingInfo & info);
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
