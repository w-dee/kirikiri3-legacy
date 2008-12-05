//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
/**
 * "Integer" クラス
 */
class tIntegerClass : public tPrimitiveClassBase
{
	typedef tPrimitiveClassBase inherited; //!< 親クラスの typedef
public:
	/**
	 * コンストラクタ
	 * @param engine	スクリプトエンジンインスタンス
	 */
	tIntegerClass(tScriptEngine * engine);

	/**
	 * 各メンバをインスタンスに追加する
	 */
	void RegisterMembers();

	/**
	 * newの際の新しいオブジェクトを作成して返す
	 */
	static tVariant ovulate();

public: // Risse用メソッドなど
	static void initialize(const tNativeCallInfo & info);
	static void times(const tNativeCallInfo & info);
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
