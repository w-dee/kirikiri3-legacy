//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "String" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseStringClassH
#define risseStringClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"String" クラス
//---------------------------------------------------------------------------
class tRisseStringClass : public tRissePrimitiveClassBase
{
	typedef tRissePrimitiveClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseStringClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();

public: // Risse用メソッドなど
	static void initialize(const tRisseNativeBindFunctionCallingInfo & info);
	static void charAt(const tRisseNativeBindFunctionCallingInfo & info, risse_offset index);
	static void get_length(const tRisseNativeBindPropertyGetterCallingInfo & info);
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
