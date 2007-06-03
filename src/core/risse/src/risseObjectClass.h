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
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseObjectClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseObjectClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize();
	static bool isA(const tRisseVariant & Class,
				const tRisseNativeBindFunctionCallingInfo & info);
	static void eval(const tRisseString & script,
				const tRisseNativeBindFunctionCallingInfo & info);
	static tRisseVariant getInstanceMember(
				const tRisseNativeBindFunctionCallingInfo & info,
				const tRisseString & membername);
	static void setInstanceMember(
				const tRisseNativeBindFunctionCallingInfo & info,
				const tRisseString & membername, const tRisseVariant & value);
	static tRisseVariant toException(
				const tRisseNativeBindFunctionCallingInfo & info);
	static void p(const tRisseMethodArgument & args);
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
