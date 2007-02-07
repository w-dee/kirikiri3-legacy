//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Property" クラスの実装
//---------------------------------------------------------------------------

#ifndef rissePropertyClassH
#define rissePropertyClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseSingleton.h"
#include "risseGC.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Property" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tRissePropertyInstance : public tRisseObjectBase
{
	typedef tRisseObjectBase inherited; //!< 親クラスの typedef

private:
	tRisseVariant Getter; //!< ゲッタメソッド
	tRisseVariant Setter; //!< セッタメソッド

public:
	//! @brief		コンストラクタ
	tRissePropertyInstance();

	//! @brief		ゲッタメソッドを取得する
	//! @return		ゲッタメソッド
	tRisseVariant & GetGetter() { return Getter; }

	//! @brief		ゲッタメソッドを設定する
	//! @param		m		ゲッタメソッド
	void SetGetter(const tRisseVariant & m) { Getter = m; }

	//! @brief		セッタメソッドを取得する
	//! @return		セッタメソッド
	tRisseVariant & GetSetter() { return Setter; }

	//! @brief		セッタメソッドを設定する
	//! @param		m		セッタメソッド
	void SetSetter(const tRisseVariant & m) { Setter = m; }

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tRissePropertyInstance() {;}

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Property" クラス
//---------------------------------------------------------------------------
class tRissePropertyClass : public tRisseClassBase, public tRisseClassSingleton<tRissePropertyClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	tRissePropertyClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
