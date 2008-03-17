//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
class tMemberAttribute : public tCollectee
{
public:
	//! @brief	変更性を規定する物
	enum tMutabilityControl
	{
		mcNone,
		mcVar, //!< メンバは変更可能
		mcConst, //!< 一度assignした値は変更不可
	};

	//! @brief	オーバーライド性を規定する物
	enum tOverrideControl
	{
		ocNone,
		ocVirtual, //!< インスタンスごとに異なる値を持ち、サブクラスでオーバーライド可
		ocFinal, //!< サブクラスでオーバーライド不可
	};

	//! @brief	プロパティアクセス方法を規定する物
	enum tPropertyControl
	{
		pcNone,
		pcField, //!< 普通のメンバ
		pcProperty, //!< 読み出しと書き込みにおいてゲッタとセッタの呼び出しを伴うメンバ
	};

	//! @brief	アクセスコントロールを規定する物
	enum tAccessControl
	{
		acNone,
		acPublic, //!< 公開メンバ
		acInternal, //!< パッケージ外にexportできないメンバ
		acProtected, //!< 同じクラスあるいは派生クラスからのみアクセス可能なメンバ
		acPrivate, //!< 同じクラス内からのみアクセス可能なメンバ
	};

private:
	union
	{
		struct
		{
			tMutabilityControl	Mutability	: 2;
			tOverrideControl	Override	: 2;
			tPropertyControl	Property	: 2;
			tAccessControl		Access		: 3;
		};
		risse_uint16 Value;
	};

public:
	//! @brief		デフォルトコンストラクタ
	tMemberAttribute()
	{
		Value = 0;
		Mutability = mcNone;
		Override = ocNone;
		Property = pcNone;
		Access = acNone;
	}

	//! @brief		コンストラクタ (risse_uint32 から)
	//! @param		value  値
	explicit tMemberAttribute(risse_uint32 value)
	{
		Value = static_cast<risse_uint8>(value);
	}

	//! @brief		コピーコンストラクタ
	//! @param		rhs		コピー元
	tMemberAttribute(const tMemberAttribute & rhs)
	{
		Value = rhs.Value;
	}

	//! @brief		デフォルトのメンバ属性を得る
	//! @return		デフォルトのメンバ属性
	static tMemberAttribute GetDefault()
	{
		tMemberAttribute ret;
		return ret.Set(mcVar).Set(ocVirtual).Set(pcField).Set(acPublic);
	}

	//! @brief		コンストラクタ (mutabilityから)
	//! @param		mutability	変更性
	explicit tMemberAttribute(tMutabilityControl mutability)
	{
		Value = 0;
		Mutability = mutability;
		Override = ocNone;
		Property = pcNone;
		Access = acNone;
	}


	//! @brief		コンストラクタ (overrideから)
	//! @param		override	オーバーライド性
	explicit tMemberAttribute(tOverrideControl override)
	{
		Value = 0;
		Mutability = mcNone;
		Override = override;
		Property = pcNone;
		Access = acNone;
	}

	//! @brief		コンストラクタ (propertyから)
	//! @param		property	プロパティアクセス方法
	explicit tMemberAttribute(tPropertyControl property)
	{
		Value = 0;
		Mutability = mcNone;
		Override = ocNone;
		Property = property;
		Access = acNone;
	}

	//! @brief		コンストラクタ (accessから)
	//! @param		access	アクセスコントロール
	explicit tMemberAttribute(tAccessControl access)
	{
		Value = 0;
		Mutability = mcNone;
		Override = ocNone;
		Property = pcNone;
		Access = access;
	}

	//! @brief		変更性を得る
	//! @return		変更性
	tMutabilityControl GetMutability() const { return Mutability; }
	//! @brief		変更性を設定する
	//! @param		v	変更性
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & Set(tMutabilityControl v) { Mutability = v; return *this; }
	//! @brief		変更性を設定する
	//! @param		v	変更性
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & operator = (tMutabilityControl v) { Mutability = v; return *this; }

	//! @brief		オーバーライド性を得る
	//! @return		オーバーライド性
	tOverrideControl GetOverride() const { return Override; }
	//! @brief		オーバーライド性を設定する
	//! @param		v	オーバーライド性
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & Set(tOverrideControl v) { Override = v; return *this; }
	//! @brief		オーバーライド性を設定する
	//! @param		v	オーバーライド性
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & operator =(tOverrideControl v) { Override = v; return *this; }

	//! @brief		プロパティアクセス方法を得る
	//! @return		プロパティアクセス方法
	tPropertyControl GetProperty() const { return Property; }
	//! @brief		プロパティアクセス方法を設定する
	//! @param		v	プロパティアクセス方法
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & Set(tPropertyControl v) { Property = v; return *this; }
	//! @brief		プロパティアクセス方法を設定する
	//! @param		v	プロパティアクセス方法
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & operator =(tPropertyControl v) { Property = v; return *this; }

	//! @brief		アクセスコントロールを得る
	//! @return		アクセスコントロール
	tAccessControl GetAccess() const { return Access; }
	//! @brief		アクセスコントロールを設定する
	//! @param		v	アクセスコントロール
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & Set(tAccessControl v) { Access = v; return *this; }
	//! @brief		アクセスコントロールを設定する
	//! @param		v	アクセスコントロール
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & operator =(tAccessControl v) { Access = v; return *this; }

	//! @brief		属性を上書きする
	//! @param		rhs		上書きする属性
	//! @return		上書きされた属性があった場合に真
	bool Overwrite(tMemberAttribute rhs);

	//! @brief		risse_uint32 へのキャスト
	//! @note		この戻り値は、tObjectInterface::Do() や tObjectInterface::Operate の
	//!				flags に直接渡すことができる
	operator risse_uint32() const { return static_cast<risse_uint32>(Value); }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	変更性
	bool Has(tMutabilityControl v) const { return Mutability == v; }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	オーバーライド性
	bool Has(tOverrideControl v) const { return Override == v; }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	プロパティアクセス方法
	bool Has(tPropertyControl v) const { return Property == v; }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	可視性
	bool Has(tAccessControl v) const { return Access == v; }


	//! @brief		なにか属性を持っているかどうかを調べる
	//! @return		何か属性を持っていれば真
	bool HasAny() const
		{ return
			Mutability != mcNone ||
			Override != ocNone ||
			Property != pcNone ||
			Access != acNone ;
		}



	//! @brief		属性を文字列化する
	//! @return		文字列化された属性
	tString AsString() const;
};
//---------------------------------------------------------------------------
}
#endif

