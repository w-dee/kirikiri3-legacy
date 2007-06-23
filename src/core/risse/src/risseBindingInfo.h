//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バインディングに関する情報の管理
//---------------------------------------------------------------------------

#ifndef risseBindingInfoH
#define risseBindingInfoH

#include "risseCodeBlock.h"
#include "risseGC.h"
#include "risseThread.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		バインディングに関する情報
//! @note		一度作成されればこのオブジェクト自体は read-only な使い方しかしない
//!				(もっとも Frames 内の変数へのアクセスは Frames->GetCS() によって得られる
//!				クリティカルセクションを利用すること。
//---------------------------------------------------------------------------
class tRisseBindingInfo : public tRisseCollectee
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
	//! @brief		コンストラクタ
	//! @param		This		このバインディングの "This"
	//! @param		frames		共有フレーム
	tRisseBindingInfo(const tRisseVariant & This, tRisseSharedVariableFrames * frames)
	{
		this->This = This;
		Frames = frames;
	}

	//! @brief		コンストラクタ
	//! @param		This		このバインディングの "This"
	tRisseBindingInfo(const tRisseVariant & This)
	{
		this->This = This;
		Frames = NULL;
	}

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tRisseBindingInfo() {;}

	//! @brief		共有フレームを得る
	//! @return		共有フレーム
	tRisseSharedVariableFrames * GetFrames() const { return Frames; }

	//! @brief		ローカル変数のバインディングへの参照を得る
	//! @return		ローカル変数のバインディングへの参照
	tBindingMap & GetBindingMap() { return BindingMap; }

	//! @brief		ローカル変数のバインディングへの参照を得る
	//! @return		ローカル変数のバインディングへの参照
	const tBindingMap & GetBindingMap() const { return BindingMap; }

	//! @brief		このバインディングの "This" を得る
	//! @return		このバインディングの "This"
	const tRisseVariant & GetThis() const { return This; }

	//! @brief		ローカル変数のバインディングのマップを追加する
	//! @param		This		Bindingクラスのインスタンス
	//! @param		name		ローカル変数名
	//! @param		reg			レジスタ番号
	static void AddMap(tRisseVariant &This, const tRisseString &name, risse_uint32 reg);
};
//---------------------------------------------------------------------------
}
#endif
