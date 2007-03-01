//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 宣言時の属性
//---------------------------------------------------------------------------
#ifndef risseDeclAttributeH
#define risseDeclAttributeH

#include "../risseMemberAttribute.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief	宣言時の属性
//---------------------------------------------------------------------------
class tRisseDeclAttribute : public tRisseMemberAttribute
{
public:
	//! @brief	コンテキストを規定するもの
	enum tContextControl
	{
		ccNone,
		ccStatic, //!< static コンテキスト
	};

private:
	tContextControl		Context;

public:
	//! @brief		デフォルトコンストラクタ
	tRisseDeclAttribute()
	{
		Context = ccNone;
	}

	//! @brief		コンストラクタ (tRisseMemberAttributeから)
	//! @param		attrib	属性
	tRisseDeclAttribute(const tRisseMemberAttribute & context)
	{
		*(tRisseMemberAttribute*)this = context;
		Context = ccNone;
	}

	//! @brief		コンストラクタ (accessから)
	//! @param		access	アクセス制限
	explicit tRisseDeclAttribute(tAccessControl access) :
		tRisseMemberAttribute(access)
	{
		Context = ccNone;
	}

	//! @brief		コンストラクタ (visibilityから)
	//! @param		visibility	可視性
	explicit tRisseDeclAttribute(tVisibilityControl visibility) :
		tRisseMemberAttribute(visibility)
	{
		Context = ccNone;
	}

	//! @brief		コンストラクタ (overrideから)
	//! @param		override	オーバーライド性
	explicit tRisseDeclAttribute(tOverrideControl override) :
		tRisseMemberAttribute(override)
	{
		Context = ccNone;
	}

	//! @brief		コンストラクタ (propertyから)
	//! @param		property	プロパティアクセス方法
	explicit tRisseDeclAttribute(tPropertyControl property) :
		tRisseMemberAttribute(property)
	{
		Context = ccNone;
	}

	//! @brief		コンストラクタ (contextから)
	//! @param		context	コンテキスト規定
	explicit tRisseDeclAttribute(tContextControl context)
	{
		Context = context;
	}

	//! @brief	ダウンキャスト
	operator tRisseMemberAttribute () const { return *(const tRisseMemberAttribute*)this; }

	//! @brief		コンテキスト規定を得る
	//! @return		コンテキスト規定
	tContextControl GetContext() const { return Context; }
	//! @brief		コンテキスト規定を設定する
	//! @param		v	コンテキスト規定
	void SetAccess(tContextControl v) { Context = v; }

	//! @brief		属性を上書きする
	//! @param		rhs		上書きする属性
	//! @return		上書きされた属性があった場合に真
	bool Overwrite(const tRisseDeclAttribute & rhs);

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	コンテキスト規定
	bool Has(tContextControl v) const { return Context == v; }


	//! @brief		なにか属性を持っているかどうかを調べる
	//! @return		何か属性を持っていれば真
	bool HasAny() const
		{ return tRisseMemberAttribute::HasAny() ||
			Context != ccNone;
		}

	//! @brief		属性を文字列化する
	//! @return		文字列化された属性
	tRisseString AsString() const;
};
//---------------------------------------------------------------------------
}
#endif

