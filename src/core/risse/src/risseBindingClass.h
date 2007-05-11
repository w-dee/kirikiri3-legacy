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

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Binding" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tRisseBindingInstance : public tRisseObjectBase
{
public:
	typedef gc_map<tRisseString, risse_uint32> tBindingMap;
		//!< ローカル変数のバインディングのマップのtypedef @n
		//!< second の risse_uint32 には VM コードと同じフォーマットで
		//!< 共有フレーム内の変数番号が入る; つまり上位16bitがネストレベル、
		//!< 下位16bitが変数番号。

private:
	tRisseSharedVariableFrames * Frames; //!< 共有フレーム
	tBindingMap BindingMap; //!< ローカル変数のバインディング
	tRisseVariant This; //!< このバインディングの "This"

public:
	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tRisseBindingInstance() {;}

	//! @brief		ローカル変数のバインディングへの参照を得る
	//! @return		ローカル変数のバインディングへの参照
	tBindingMap & GetBindingMap() { return BindingMap; }

	//! @brief		共有フレームを得る
	//! @return		共有フレーム
	tRisseSharedVariableFrames * GetFrames() const { return Frames; }

	//! @brief		ローカル変数のバインディングのマップを追加する
	//! @param		This		Bindingクラスのインスタンス
	//! @param		name		ローカル変数名
	//! @param		reg			レジスタ番号
	static void AddMap(tRisseVariant &This, const tRisseString &name, risse_uint32 reg);

	//! @brief		各種情報をセットする
	//! @param		This		このバインディングの "This"
	//! @param		frames		共有フレーム
	void Set(const tRisseVariant & This, tRisseSharedVariableFrames * frames)
	{
		this->This = This;
		Frames = frames;
	}
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
