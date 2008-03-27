//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 組み込みパッケージイニシャライザの共通クラス
//---------------------------------------------------------------------------
#ifndef risseBuiltinPackageInitializer
#define risseBuiltinPackageInitializer


#include "../rissePackage.h"
#include "../risseScriptEngine.h"

namespace Risse {
//---------------------------------------------------------------------------
//! @brief		組み込みパッケージイニシャライザの共通クラス
//---------------------------------------------------------------------------
class tBuiltinPackageInitializer : public tBuiltinPackageInitializerInterface
{
	tString Name; //!< パッケージ名

public:
	//! @brief		コンストラクタ
	//! @param		name		パッケージ名
	tBuiltinPackageInitializer(const tString & name) : Name(name) {;}

	//! @brief		インスタンスをパッケージマネージャに登録する
	//! @param		manager		パッケージマネージャインスタンス
	void RegisterInstance(tPackageManager * manager)
	{
		manager->AddBuiltinPackage(Name, this);
	}

	//! @brief		パッケージを初期化する
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		パッケージ名
	//! @param		global		パッケージグローバル
	virtual void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global) = 0;
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

