//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
/**
 * バインディングに関する情報
 * @note	一度作成されればこのオブジェクト自体は read-only な使い方しかしない
 *			(もっとも Frames 内の変数へのアクセスは Frames->GetCS() によって得られる
 *			クリティカルセクションを利用すること。
 */
class tBindingInfo : public tCollectee
{
public:
	typedef gc_map<tString, risse_uint32> tBindingMap;
		//!< ローカル変数のバインディングのマップのtypedef @n
		//!< second の risse_uint32 には VM コードと同じフォーマットで
		//!< 共有フレーム内の変数番号が入る; つまり上位16bitがネストレベル、
		//!< 下位16bitが変数番号。

private:
	tSharedVariableFrames * Frames; //!< 共有フレーム
	tBindingMap BindingMap; //!< ローカル変数のバインディング
	tVariant Global; //!< このバインディングのパッケージグローバル
	tVariant This; //!< このバインディングの "This"

public:
	/**
	 * コンストラクタ
	 * @param global	このバインディングのパッケージグローバル
	 * @param This		このバインディングの "This"
	 * @param frames	共有フレーム
	 */
	tBindingInfo(const tVariant & global,
		const tVariant & This, tSharedVariableFrames * frames)
	{
		this->Global = global;
		this->This = This;
		Frames = frames;
	}

	/**
	 * コンストラクタ
	 * @param global	このバインディングのパッケージグローバル
	 * @param This		このバインディングの "This"
	 */
	tBindingInfo(const tVariant & global, const tVariant & This)
	{
		this->Global = global;
		this->This = This;
		Frames = NULL;
	}

	/**
	 * ダミーのデストラクタ(おそらく呼ばれない)
	 */
	virtual ~tBindingInfo() {;}

	/**
	 * 共有フレームを得る
	 * @return	共有フレーム
	 */
	tSharedVariableFrames * GetFrames() const { return Frames; }

	/**
	 * ローカル変数のバインディングへの参照を得る
	 * @return	ローカル変数のバインディングへの参照
	 */
	tBindingMap & GetBindingMap() { return BindingMap; }

	/**
	 * ローカル変数のバインディングへの参照を得る
	 * @return	ローカル変数のバインディングへの参照
	 */
	const tBindingMap & GetBindingMap() const { return BindingMap; }

	/**
	 * このバインディングのパッケージグローバルを得る
	 * @return	このバインディングのパッケージグローバル
	 */
	const tVariant & GetGlobal() const { return Global; }

	/**
	 * このバインディングの "This" を得る
	 * @return	このバインディングの "This"
	 */
	const tVariant & GetThis() const { return This; }

	/**
	 * ローカル変数のバインディングのマップを追加する
	 * @param This	Bindingクラスのインスタンス
	 * @param name	ローカル変数名
	 * @param reg	レジスタ番号
	 */
	static void AddMap(tVariant &This, const tString &name, risse_uint32 reg);
};
//---------------------------------------------------------------------------
}
#endif
