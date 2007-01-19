//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトのメンバの属性
//---------------------------------------------------------------------------
#ifndef risseMemberAttributeH
#define risseMemberAttributeH

#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseGC.h"
#include "risseString.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief	メンバの属性
//---------------------------------------------------------------------------
class tRisseMemberAttribute : public tRisseCollectee
{
public:
	//! @brief	アクセス制限を規定するもの
	enum tAccessControl
	{
		acNone,
		acPublic, //!< public アクセス
		acInternal, //!< internal アクセス
		acPrivate,	//!< private アクセス
	};

	//! @brief	可視性を規定するもの
	enum tVisibilityControl
	{
		vcNone,
		vcEnumerable, //!< 列挙可能
		vcHidden, //!< 列挙不可
	};

	//! @brief	オーバーライド性を規定する物
	enum tOverrideControl
	{
		ocNone,
		ocStatic, //!< 同じクラスの異なるインスタンス間で共有
		ocVirtual, //!< インスタンスごとに異なる値を持ち、サブクラスでオーバーライド可
		ocFinal, //!< インスタンスごとに異なる値を持ち、サブクラスでオーバーライド不可
	};

	//! @brief	プロパティアクセス方法を規定する物
	enum tPropertyControl
	{
		pcNone,
		pcVar, //!< 普通のメンバ
		pcConst, //!< 定数
		pcProperty, //!< 読み出しと書き込みにおいてゲッタとセッタの呼び出しを伴うメンバ
	};

private:
	union
	{
		struct
		{
			tAccessControl		Access		: 2;
			tVisibilityControl	Visibility	: 2;
			tOverrideControl	Override	: 2;
			tPropertyControl	Property	: 2;
		};
		risse_uint8 Value;
	};

public:
	//! @brief		デフォルトコンストラクタ
	tRisseMemberAttribute()
	{
		Access = acNone;
		Visibility = vcNone;
		Override = ocNone;
		Property = pcNone;
	}

	//! @brief		コンストラクタ (risse_uint32 から)
	//! @param		value  値
	explicit tRisseMemberAttribute(risse_uint32 value)
	{
		Value = static_cast<risse_uint8>(value);
	}

	//! @brief		コンストラクタ (accessから)
	//! @param		access	アクセス制限
	explicit tRisseMemberAttribute(tAccessControl access)
	{
		Access = access;
		Visibility = vcNone;
		Override = ocNone;
		Property = pcNone;
	}

	//! @brief		コンストラクタ (visibilityから)
	//! @param		visibility	可視性
	explicit tRisseMemberAttribute(tVisibilityControl visibility)
	{
		Access = acNone;
		Visibility = visibility;
		Override = ocNone;
		Property = pcNone;
	}

	//! @brief		コンストラクタ (overrideから)
	//! @param		override	オーバーライド性
	explicit tRisseMemberAttribute(tOverrideControl override)
	{
		Access = acNone;
		Visibility = vcNone;
		Override = override;
		Property = pcNone;
	}

	//! @brief		コンストラクタ (propertyから)
	//! @param		property	プロパティアクセス方法
	explicit tRisseMemberAttribute(tPropertyControl property)
	{
		Access = acNone;
		Visibility = vcNone;
		Override = ocNone;
		Property = property;
	}

	//! @brief		アクセス制限を得る
	//! @return		アクセス制限
	tAccessControl GetAccess() const { return Access; }
	//! @brief		アクセス制限を設定する
	//! @param		v	アクセス制限
	void SetAccess(tAccessControl v) { Access = v; }

	//! @brief		可視性を得る
	//! @return		可視性
	tVisibilityControl GetVisibility() const { return Visibility; }
	//! @brief		可視性を設定する
	//! @param		v	可視性
	void SetVisibility(tVisibilityControl v) { Visibility = v; }

	//! @brief		オーバーライド性を得る
	//! @return		オーバーライド性
	tOverrideControl GetOverride() const { return Override; }
	//! @brief		オーバーライド性を設定する
	//! @param		v	オーバーライド性
	void SetOverride(tOverrideControl v) { Override = v; }

	//! @brief		プロパティアクセス方法を得る
	//! @return		プロパティアクセス方法
	tPropertyControl GetProperty() const { return Property; }
	//! @brief		プロパティアクセス方法を設定する
	//! @param		v	プロパティアクセス方法
	void SetProperty(tPropertyControl v) { Property = v; }

	//! @brief		属性を上書きする
	//! @param		rhs		上書きする属性
	//! @return		上書きされた属性があった場合に真
	bool Overwrite(tRisseMemberAttribute rhs);

	//! @brief		risse_uint32 へのキャスト
	//! @note		この戻り値は、tRisseObjectInterface::Do() や tRisseObjectInterface::Operate の
	//!				flags に直接渡すことができる
	operator risse_uint32() const { return static_cast<risse_uint32>(Value); }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	アクセス制限
	bool Has(tAccessControl v) const { return Access == v; }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	可視性
	bool Has(tVisibilityControl v) const { return Visibility == v; }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	オーバーライド性
	bool Has(tOverrideControl v) const { return Override == v; }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	プロパティアクセス方法
	bool Has(tPropertyControl v) const { return Property == v; }


	//! @brief		なにか属性を持っているかどうかを調べる
	//! @return		何か属性を持っていれば真
	bool HasAny() const
		{ return
			Access != acNone ||
			Visibility != vcNone ||
			Override != ocNone ||
			Property != pcNone;
		}



	//! @brief		属性を文字列化する
	//! @return		文字列化された属性
	tRisseString AsString() const;
};
//---------------------------------------------------------------------------
}
#endif

