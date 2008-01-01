//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
#include "risseGC.h"
#include "risseNativeBinder.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Property" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tPropertyInstance : public tObjectBase
{
	typedef tObjectBase inherited; //!< 親クラスの typedef

private:
	tVariant Getter; //!< ゲッタメソッド
	tVariant Setter; //!< セッタメソッド
	bool Synchronized; //!< synchronized メソッドかどうか

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tPropertyInstance();

	//! @brief		ゲッタメソッドを取得する
	//! @return		ゲッタメソッド
	tVariant & GetGetter() { return Getter; }

	//! @brief		ゲッタメソッドを設定する
	//! @param		m		ゲッタメソッド
	void SetGetter(const tVariant & m) { Getter = m; }

	//! @brief		セッタメソッドを取得する
	//! @return		セッタメソッド
	tVariant & GetSetter() { return Setter; }

	//! @brief		セッタメソッドを設定する
	//! @param		m		セッタメソッド
	void SetSetter(const tVariant & m) { Setter = m; }

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tPropertyInstance() {;}

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

	//! @brief		synchronized メソッドかどうかを取得する
	//! @return		synchronized メソッドかどうか
	bool GetSynchronized() const { return Synchronized; }

	//! @brief		synchronized メソッドかどうかを設定する
	//! @param		b	synchronized メソッドかどうか
	void SetSynchronized(bool b) { Synchronized = b; }


public: // Risse用オブジェクトなど
	void construct();
	void initialize(const tNativeCallInfo & info);
	bool get_synchronized() const { return GetSynchronized(); }
	void set_synchronized(bool b) { SetSynchronized(b); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Property" クラス
//---------------------------------------------------------------------------
class tPropertyClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tPropertyClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
