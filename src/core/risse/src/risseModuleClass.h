//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief "Module" (モジュール) の実装
//---------------------------------------------------------------------------
#ifndef risseModuleClassH
#define risseModuleClassH

#include "risseCharUtils.h"
#include "risseObject.h"
#include "risseClass.h"
#include "risseObjectBase.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		モジュール用クラスの基底クラス(Risse用)
//---------------------------------------------------------------------------
class tRisseModuleClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef
public:
	// ! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseModuleClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tRisseVariant ovulate(const tRisseNativeCallInfo &info);

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tRisseNativeCallInfo &info);

public: // ユーティリティ
	//! @brief		members 用のオブジェクトを作成する
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		proto		members の prototype が見に行く先
	//! @note		ここで作成されるオブジェクトには、それが Object クラスの
	//!				インスタンスであるという情報が付かないため、必要ならば
	//!				tRisseObjectClass::Bless でクラス情報を付加すること。
	//!				(このメソッドが呼ばれる時点ではまだ Object クラスが有効
	//!				でない可能性があるため)
	static tRisseVariant CreateMembersObject(tRisseScriptEngine * engine, const tRisseVariant proto);
};
//---------------------------------------------------------------------------
}
#endif

