//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Boolean" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseBooleanClassH
#define risseBooleanClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "rissePrimitiveClass.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Boolean" クラス
//---------------------------------------------------------------------------
class tRisseBooleanClass : public tRissePrimitiveClassBase
{
	typedef tRissePrimitiveClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseBooleanClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tRisseVariant ovulate();

public: // Risse用メソッドなど
	static void initialize(const tRisseNativeCallInfo & info);
	static tRisseString toString(const tRisseNativeCallInfo & info);
	static tRisseString dump(const tRisseNativeCallInfo & info);
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
