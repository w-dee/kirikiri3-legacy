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
//! @note		tClassBase と tClassClass を混同しないこと。
//!				前者はC++のクラス階層において各クラスの親クラスになり、
//!				後者はRisseスクリプトで各クラスインスタンスを生成するための
//!				クラスになる。
//---------------------------------------------------------------------------
class tClassClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tClassClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate(const tNativeCallInfo &info);

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		クラスインスタンス用クラス(Risse用)
//! @note		tClassInstance と tClassClass を混同しないこと。
//---------------------------------------------------------------------------
class tClassInstance : public tClassClass
{
	typedef tClassClass inherited; //!< 親クラスの typedef

public:
	// ! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tClassInstance(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


}
#endif

