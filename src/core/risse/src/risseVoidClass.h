//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Void" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseVoidClassH
#define risseVoidClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Void" クラス
//---------------------------------------------------------------------------
class tRisseVoidClass : public tRissePrimitiveClassBase
{
	typedef tRissePrimitiveClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseVoidClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tRisseVariant ovulate();

public: // Risse用メソッドなど
	static void initialize(const tRisseNativeCallInfo & info);
	static tRisseString toString();
	static tRisseString dump();
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
