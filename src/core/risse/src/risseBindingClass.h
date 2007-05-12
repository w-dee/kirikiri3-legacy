//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "risseSingleton.h"
#include "risseCodeBlock.h"
#include "risseGC.h"
#include "risseBindingInfo.h"

namespace Risse
{
class tRisseScriptEngine;
//---------------------------------------------------------------------------
//! @brief		"Binding" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tRisseBindingInstance : public tRisseObjectBase
{
	tRisseBindingInfo * Info; //!< バインディングに関する情報
	tRisseScriptEngine * Engine; //!< スクリプトエンジンインスタンス


public:
	//! @brief		コンストラクタ
	tRisseBindingInstance() { Info = NULL; Engine = NULL; }

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tRisseBindingInstance() {;}

	//! @brief		ローカル変数のバインディングへの参照を得る
	//! @return		ローカル変数のバインディングへの参照
	tRisseBindingInfo::tBindingMap & GetBindingMap() { RISSE_ASSERT(Info != NULL); return Info->GetBindingMap(); }

	//! @brief		共有フレームを得る
	//! @return		共有フレーム
	tRisseSharedVariableFrames * GetFrames() const { RISSE_ASSERT(Info != NULL); return Info->GetFrames(); }

	//! @brief		ローカル変数のバインディングのマップを追加する
	//! @param		This		Bindingクラスのインスタンス
	//! @param		name		ローカル変数名
	//! @param		reg			レジスタ番号
	static void AddMap(tRisseVariant &This, const tRisseString &name, risse_uint32 reg);

	//! @brief		バインディングに関する情報をセットする
	//! @param		info		バインディングに関する情報
	void SetInfo(tRisseBindingInfo * info)
	{
		Info = info;
	}

	//! @brief		スクリプトエンジンを設定する
	//! @param		info		スクリプトエンジン
	//! @note		eval する際にスクリプトエンジンの情報が必要。
	void SetScriptEngine(tRisseScriptEngine * engine) { Engine = engine; }

	//! @brief		スクリプトエンジンを取得する
	//! @return		スクリプトエンジン
	tRisseScriptEngine * GetScriptEngine() const { return Engine; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Binding" クラス
//---------------------------------------------------------------------------
class tRisseBindingClass : public tRisseClassBase, public tRisseClassSingleton<tRisseBindingClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseBindingClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();

public:
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
