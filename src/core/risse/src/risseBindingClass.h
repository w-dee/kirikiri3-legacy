//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Binding" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseBindingClassH
#define risseBindingClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseCodeBlock.h"
#include "risseGC.h"
#include "risseBindingInfo.h"
#include "risseNativeBinder.h"

namespace Risse
{
class tScriptEngine;
//---------------------------------------------------------------------------
/**
 * "Binding" クラスのインスタンス用 C++クラス
 */
class tBindingInstance : public tObjectBase
{
	tBindingInfo * Info; //!< バインディングに関する情報
public:
	/**
	 * コンストラクタ
	 */
	tBindingInstance() : tObjectBase() { Info = NULL; }

	/**
	 * ダミーのデストラクタ(おそらく呼ばれない)
	 */
	virtual ~tBindingInstance() {;}

	/**
	 * ローカル変数のバインディングへの参照を得る
	 * @return	ローカル変数のバインディングへの参照
	 */
	tBindingInfo::tBindingMap & GetBindingMap() const { RISSE_ASSERT(Info != NULL); return Info->GetBindingMap(); }

	/**
	 * 共有フレームを得る
	 * @return	共有フレーム
	 */
	tSharedVariableFrames * GetFrames() const { RISSE_ASSERT(Info != NULL); return Info->GetFrames(); }

	/**
	 * ローカル変数のバインディングのマップを追加する
	 * @param This	Bindingクラスのインスタンス
	 * @param name	ローカル変数名
	 * @param reg	レジスタ番号
	 */
	static void AddMap(tVariant &This, const tString &name, risse_uint32 reg);

	/**
	 * バインディングに関する情報を設定する
	 * @param info	バインディングに関する情報
	 */
	void SetInfo(tBindingInfo * info) { Info = info; }

	/**
	 * バインディングに関する情報を取得する
	 * @return	バインディングに関する情報
	 */
	tBindingInfo * GetInfo() const { return Info; }

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
	void eval(const tString & script, const tNativeCallInfo &info) const;
	tVariant iget(const tString & name) const;
	void iset(const tVariant & value, const tString & name);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * "Binding" クラス
 */
class tBindingClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	/**
	 * コンストラクタ
	 * @param engine	スクリプトエンジンインスタンス
	 */
	tBindingClass(tScriptEngine * engine);

	/**
	 * 各メンバをインスタンスに追加する
	 */
	void RegisterMembers();

	/**
	 * newの際の新しいオブジェクトを作成して返す
	 */
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
