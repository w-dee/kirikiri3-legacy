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


#include "../risseScriptEngine.h"

namespace Risse {
//---------------------------------------------------------------------------
//! @brief		組み込みパッケージイニシャライザの共通クラス
//---------------------------------------------------------------------------
class tBuiltinPackageInitializer : public tBuiltinPackageInitializerInterface
{
	tString PackageName; //!< パッケージ名

public:
	//! @brief		コンストラクタ
	//! @param		name		パッケージ名
	tBuiltinPackageInitializer(const tString & name) : PackageName(name) {;}

	//! @brief		インスタンスをパッケージマネージャに登録する
	//! @param		engine		スクリプトエンジンインスタンス
	void RegisterInstance(tScriptEngine * engine)
	{
		engine->AddBuiltinPackage(PackageName, this);
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

