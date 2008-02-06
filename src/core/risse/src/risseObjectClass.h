//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
class tObjectClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tObjectClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize();
	static bool DiscEqual(const tNativeCallInfo & info, const tVariant &rhs); // === 演算子
	static bool identify(const tNativeCallInfo & info, const tVariant &rhs); // Dictionary で hash とともに同定に用いる
	static void get_hint(const tNativePropGetInfo & info); // hint プロパティ
	static void set_hint(const tNativePropSetInfo & info); // hint プロパティ
	static void get_hash(const tNativePropGetInfo & info); // hash プロパティ
	static bool isA(const tVariant & Class,
				const tNativeCallInfo & info);
	static void eval(const tString & script,
				const tNativeCallInfo & info);
	static tVariant getInstanceMember(
				const tNativeCallInfo & info,
				const tString & membername);
	static void setInstanceMember(
				const tNativeCallInfo & info,
				const tString & membername, const tVariant & value);
	static tVariant toException(
				const tNativeCallInfo & info);
	static void p(const tMethodArgument & args);
	static bool toBoolean() { return true; }

public: // ユーティリティ
	//! @brief		intf を Object クラスのインスタンスとしてマークする
	//! @param		intf		マークする対象となるインスタンス
	//! @note		いまだ Object クラスが初期化されていない時期に作成されたインスタンス
	//!				に対してオブジェクトクラスとしての情報を付加するために用いる
	void Bless(tObjectInterface * intf);
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
