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
	static bool DiscEqual(const tRisseNativeCallInfo & info, const tRisseVariant &rhs); // === 演算子
	static bool isA(const tRisseVariant & Class,
				const tRisseNativeCallInfo & info);
	static void eval(const tRisseString & script,
				const tRisseNativeCallInfo & info);
	static tRisseVariant getInstanceMember(
				const tRisseNativeCallInfo & info,
				const tRisseString & membername);
	static void setInstanceMember(
				const tRisseNativeCallInfo & info,
				const tRisseString & membername, const tRisseVariant & value);
	static tRisseVariant toException(
				const tRisseNativeCallInfo & info);
	static void p(const tRisseMethodArgument & args);

public: // ユーティリティ
	//! @brief		intf を Object クラスのインスタンスとしてマークする
	//! @param		intf		マークする対象となるインスタンス
	//! @note		いまだ Object クラスが初期化されていない時期に作成されたインスタンス
	//!				に対してオブジェクトクラスとしての情報を付加するために用いる
	void Bless(tRisseObjectInterface * intf);
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
