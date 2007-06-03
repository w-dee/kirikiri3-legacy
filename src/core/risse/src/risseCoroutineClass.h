//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Coroutine" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseCoroutineClassH
#define risseCoroutineClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"

namespace Risse
{
class tRisseCoroutine;
//---------------------------------------------------------------------------
//! @brief		"Coroutine" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tRisseCoroutineInstance : public tRisseObjectBase
{
private:
	tRisseCoroutine * Coroutine; //!< コルーチンの実装

public:
	//! @brief		コンストラクタ
	tRisseCoroutineInstance();

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tRisseCoroutineInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tRisseNativeBindFunctionCallingInfo & info);
	tRisseVariant run(const tRisseMethodArgument & args) const;
	tRisseVariant yield(const tRisseMethodArgument & args) const;
	void dispose() const;
	bool get_alive() const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Coroutine" クラス
//---------------------------------------------------------------------------
class tRisseCoroutineClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseCoroutineClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();

public:
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
