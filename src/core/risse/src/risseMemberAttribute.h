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
	//! @brief	変更性を規定する物
	enum tVariableControl
	{
		vcNone,
		vcVar, //!< メンバは変更可能
		vcConst, //!< 一度assignした値は変更不可
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

private:
	union
	{
		struct
		{
			tVariableControl	Variable	: 2;
			tOverrideControl	Override	: 2;
			tPropertyControl	Property	: 2;
		};
		risse_uint8 Value;
	};

public:
	//! @brief		デフォルトコンストラクタ
	tRisseMemberAttribute()
	{
		Variable = vcNone;
		Override = ocNone;
		Property = pcNone;
	}

	//! @brief		コンストラクタ (risse_uint32 から)
	//! @param		value  値
	explicit tRisseMemberAttribute(risse_uint32 value)
	{
		Value = static_cast<risse_uint8>(value);
	}

	//! @brief		デフォルトのメンバ属性を得る
	//! @return		デフォルトのメンバ属性
	static tRisseMemberAttribute GetDefault()
	{
		tRisseMemberAttribute ret;
		return ret.Set(vcVar).Set(ocVirtual).Set(pcField);
	}

	//! @brief		コンストラクタ (variableから)
	//! @param		variable	変更性
	explicit tRisseMemberAttribute(tVariableControl variable)
	{
		Variable = variable;
		Override = ocNone;
		Property = pcNone;
	}


	//! @brief		コンストラクタ (overrideから)
	//! @param		override	オーバーライド性
	explicit tRisseMemberAttribute(tOverrideControl override)
	{
		Variable = vcNone;
		Override = override;
		Property = pcNone;
	}

	//! @brief		コンストラクタ (propertyから)
	//! @param		property	プロパティアクセス方法
	explicit tRisseMemberAttribute(tPropertyControl property)
	{
		Variable = vcNone;
		Override = ocNone;
		Property = property;
	}

	//! @brief		変更性を得る
	//! @return		変更性
	tVariableControl GetVariable() const { return Variable; }
	//! @brief		変更性を設定する
	//! @param		v	変更性
	//! @return		このオブジェクト自身への参照
	tRisseMemberAttribute & Set(tVariableControl v) { Variable = v; return *this; }
	//! @brief		変更性を設定する
	//! @param		v	変更性
	//! @return		このオブジェクト自身への参照
	tRisseMemberAttribute & operator = (tVariableControl v) { Variable = v; return *this; }

	//! @brief		オーバーライド性を得る
	//! @return		オーバーライド性
	tOverrideControl GetOverride() const { return Override; }
	//! @brief		オーバーライド性を設定する
	//! @param		v	オーバーライド性
	//! @return		このオブジェクト自身への参照
	tRisseMemberAttribute & Set(tOverrideControl v) { Override = v; return *this; }
	//! @brief		オーバーライド性を設定する
	//! @param		v	オーバーライド性
	//! @return		このオブジェクト自身への参照
	tRisseMemberAttribute & operator =(tOverrideControl v) { Override = v; return *this; }

	//! @brief		プロパティアクセス方法を得る
	//! @return		プロパティアクセス方法
	tPropertyControl GetProperty() const { return Property; }
	//! @brief		プロパティアクセス方法を設定する
	//! @param		v	プロパティアクセス方法
	//! @return		このオブジェクト自身への参照
	tRisseMemberAttribute & Set(tPropertyControl v) { Property = v; return *this; }
	//! @brief		プロパティアクセス方法を設定する
	//! @param		v	プロパティアクセス方法
	//! @return		このオブジェクト自身への参照
	tRisseMemberAttribute & operator =(tPropertyControl v) { Property = v; return *this; }

	//! @brief		属性を上書きする
	//! @param		rhs		上書きする属性
	//! @return		上書きされた属性があった場合に真
	bool Overwrite(tRisseMemberAttribute rhs);

	//! @brief		risse_uint32 へのキャスト
	//! @note		この戻り値は、tRisseObjectInterface::Do() や tRisseObjectInterface::Operate の
	//!				flags に直接渡すことができる
	operator risse_uint32() const { return static_cast<risse_uint32>(Value); }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	変更性
	bool Has(tVariableControl v) const { return Variable == v; }


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
			Variable != vcNone ||
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

