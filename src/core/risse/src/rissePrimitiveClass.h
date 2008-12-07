//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
/**
 * プリミティブ型用クラスの基底クラス
 */
class tPrimitiveClassBase : public tClassBase
{
	tVariant Gateway;
		//!< ゲートウェイアクセス用の代表オブジェクト(すべてのプリミティブへのアクセスは
		//!< このオブジェクトを経由して行われる
public:
	/**
	 * コンストラクタ
	 * @param name			クラス名
	 * @param super_class	スーパークラス
	 */
	tPrimitiveClassBase(const tString & name, tClassBase * super_class);

	/**
	 * ゲートウェイアクセス用の代表オブジェクトを得る
	 * @return	ゲートウェイアクセス用の代表オブジェクト
	 */
	tVariant & GetGateway() { return Gateway; }


public: // Risse用メソッドなど
	static void construct();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * プリミティブ型用クラスの基底クラス(Risse用)
 * @note	tPrimitiveClassBase と tPrimitiveClass を混同しないこと。
 *			前者はC++のクラス階層において各プリミティブクラスの親クラスになり、
 *			後者はRisseのクラス階層において各プリミティブクラスの親クラスになる。
 */
class tPrimitiveClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef
public:
	/**
	 * コンストラクタ
	 * @param engine	スクリプトエンジンインスタンス
	 */
	tPrimitiveClass(tScriptEngine * engine);

	/**
	 * 各メンバをインスタンスに追加する
	 */
	void RegisterMembers();

	/**
	 * newの際の新しいオブジェクトを作成して返す
	 */
	static tVariant ovulate();

public: // Risse用メソッドなど
	static void construct();
	static tVariant toString(const tNativeCallInfo & info); // toString
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
