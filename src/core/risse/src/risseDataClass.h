//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RisseのDataクラス関連の処理の実装
//---------------------------------------------------------------------------

#ifndef risseDataClassH
#define risseDataClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "risseNativeBinder.h"
#include "rissePrimitiveClass.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		Dataクラスの基底クラス(Risse用)
//---------------------------------------------------------------------------
class tDataClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tDataClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public: // Risse用メソッドなど
	static void construct();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		すべての Data 派生クラスの C++ 実装用の基底クラス
//---------------------------------------------------------------------------
typedef tPrimitiveClassBase tDataClassBase;
//---------------------------------------------------------------------------


} // namespace Risse


#endif
