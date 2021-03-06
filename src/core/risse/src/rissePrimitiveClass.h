//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
RISSE_DEFINE_CLASS_BEGIN(tPrimitiveClass, tClassBase, tVariant, itNoInstance)
public: // Risse用メソッドなど
	static void construct();
	static tVariant toString(const tNativeCallInfo & info); // toString
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------
} // namespace Risse


#endif
