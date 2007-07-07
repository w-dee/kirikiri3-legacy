//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseのプリミティブ型用クラス関連の処理の実装
//---------------------------------------------------------------------------

#ifndef rissePrimitiveClassH
#define rissePrimitiveClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		プリミティブ型用クラスの基底クラス
//---------------------------------------------------------------------------
class tRissePrimitiveClassBase : public tRisseClassBase
{
	tRisseVariant Gateway;
		//!< ゲートウェイアクセス用の代表オブジェクト(すべてのプリミティブへのアクセスは
		//!< このオブジェクトを経由して行われる
public:
	//! @brief		コンストラクタ
	//! @param		name			クラス名
	//! @param		super_class		スーパークラス
	tRissePrimitiveClassBase(const tRisseString & name, tRisseClassBase * super_class);

	//! @brief		ゲートウェイアクセス用の代表オブジェクトを得る
	//! @return		ゲートウェイアクセス用の代表オブジェクト
	tRisseVariant & GetGateway() { return Gateway; }


public: // Risse用メソッドなど
	static void construct();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		プリミティブ型用クラスの基底クラス(Risse用)
//! @note		tRissePrimitiveClassBase と tRissePrimitiveClass を混同しないこと。
//!				前者はC++のクラス階層において各プリミティブクラスの親クラスになり、
//!				後者はRisseのクラス階層において各プリミティブクラスの親クラスになる。
//---------------------------------------------------------------------------
class tRissePrimitiveClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRissePrimitiveClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tRisseVariant ovulate();

public: // Risse用メソッドなど
	static void construct();
	static tRisseVariant toString(const tRisseNativeCallInfo & info); // toString
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
