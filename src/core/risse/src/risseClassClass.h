//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief "Class" (クラス) の実装
//---------------------------------------------------------------------------
#ifndef risseClassClassH
#define risseClassClassH

#include "risseCharUtils.h"
#include "risseObject.h"
#include "risseClass.h"
#include "risseObjectBase.h"
#include "risseObjectRTTI.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		クラス用クラスの基底クラス(Risse用)
//! @note		tRisseClassBase と tRisseClassClass を混同しないこと。
//!				前者はC++のクラス階層において各クラスの親クラスになり、
//!				後者はRisseスクリプトで各クラスインスタンスを生成するための
//!				クラスになる。
//---------------------------------------------------------------------------
class tRisseClassClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseClassClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tRisseVariant ovulate(const tRisseNativeCallInfo &info);

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tRisseNativeCallInfo &info);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		クラスインスタンス用クラス(Risse用)
//! @note		tRisseClassInstance と tRisseClassClass を混同しないこと。
//---------------------------------------------------------------------------
class tRisseClassInstance : public tRisseClassClass
{
	typedef tRisseClassClass inherited; //!< 親クラスの typedef

public:
	// ! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseClassInstance(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


}
#endif

