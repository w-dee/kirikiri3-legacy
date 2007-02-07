//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Function" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseFunctionClassH
#define risseFunctionClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseSingleton.h"
#include "risseGC.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Function" クラスのインスタンス用 C++クラス
//! @note		ネイティブ関数もスクリプトで生成した関数もいったんこのクラスを
//!				ゲートウェイとして使う。Risseで"Function"クラスのインスタンスである
//!				と見せかけるための仕掛け。
//---------------------------------------------------------------------------
class tRisseFunctionInstance : public tRisseObjectBase
{
	typedef tRisseObjectBase inherited; //!< 親クラスの typedef

private:
	tRisseVariant Body; //!< 関数のBodyを表すオブジェクト

public:
	//! @brief		コンストラクタ
	tRisseFunctionInstance();

	//! @brief		関数のBodyを表すオブジェクトを取得する
	//! @return		関数のBodyを表すオブジェクト
	tRisseVariant & GetBody() { return Body; }

	//! @brief		関数のBodyを表すオブジェクトを設定する
	//! @param		m		関数のBodyを表すオブジェクト
	void SetBody(const tRisseVariant & m) { Body = m; }


	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tRisseFunctionInstance() {;}

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Function" クラス
//---------------------------------------------------------------------------
class tRisseFunctionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseFunctionClass>
{
public:
	//! @brief		コンストラクタ
	tRisseFunctionClass();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif