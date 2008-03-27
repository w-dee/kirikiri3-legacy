//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Coroutine" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseCoroutineClassH
#define risseCoroutineClassH

#include "../../risseObject.h"
#include "../../risseClass.h"
#include "../../risseGC.h"
#include "../risseBuiltinPackageInitializer.h"

namespace Risse
{
class tCoroutine;
//---------------------------------------------------------------------------
//! @brief		"Coroutine" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tCoroutineInstance : public tObjectBase
{
private:
	tCoroutine * Coroutine; //!< コルーチンの実装

public:
	//! @brief		コンストラクタ
	tCoroutineInstance();

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tCoroutineInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo & info);
	void run() const;
	tVariant resume(const tMethodArgument & args) const;
	tVariant yield(const tMethodArgument & args) const;
	void dispose() const;
	bool get_alive() const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Coroutine" クラス
//---------------------------------------------------------------------------
class tCoroutineClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tCoroutineClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"coroutine" パッケージイニシャライザ
//---------------------------------------------------------------------------
class tCoroutinePackageInitializer : public tBuiltinPackageInitializer
{
public:
	tCoroutineClass * CoroutineClass;

	//! @brief		コンストラクタ
	tCoroutinePackageInitializer();

	//! @brief		パッケージを初期化する
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		パッケージ名
	//! @param		global		パッケージグローバル
	virtual void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global);
};
//---------------------------------------------------------------------------


} // namespace Risse


#endif
