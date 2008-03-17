//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
class tDeclAttribute : public tMemberAttribute
{
public:
	//! @brief	コンテキストを規定するもの
	enum tContextControl
	{
		ccNone,
		ccStatic, //!< static コンテキスト
	};

	//! @brief	同期動作を規定する物
	enum tSyncControl
	{
		scNone,
		scSynchronized, //!< synchronized コンテキスト
	};

private:
	union
	{
		struct
		{
			tContextControl		Context;
			tSyncControl		Sync;
		};
	};
public:
	//! @brief		デフォルトコンストラクタ
	tDeclAttribute()
	{
		Context = ccNone;
		Sync = scNone;
	}

	//! @brief		コピーコンストラクタ
	//! @param		rhs		コピー元
	tDeclAttribute(const tDeclAttribute & rhs) :
		tMemberAttribute(rhs)
	{
		Context = rhs.Context;
		Sync = rhs.Sync;
	}

	//! @brief		コンストラクタ (tMemberAttributeから)
	//! @param		attrib	属性
	tDeclAttribute(const tMemberAttribute & context)
	{
		*(tMemberAttribute*)this = context;
		Context = ccNone;
		Sync = scNone;
	}

	//! @brief		コンストラクタ (mutabilityから)
	//! @param		mutability	変更性
	explicit tDeclAttribute(tMutabilityControl mutability) :
		tMemberAttribute(mutability)
	{
		Context = ccNone;
		Sync = scNone;
	}

	//! @brief		コンストラクタ (overrideから)
	//! @param		override	オーバーライド性
	explicit tDeclAttribute(tOverrideControl override) :
		tMemberAttribute(override)
	{
		Context = ccNone;
		Sync = scNone;
	}

	//! @brief		コンストラクタ (propertyから)
	//! @param		property	プロパティアクセス方法
	explicit tDeclAttribute(tPropertyControl property) :
		tMemberAttribute(property)
	{
		Context = ccNone;
		Sync = scNone;
	}

	//! @brief		コンストラクタ (contextから)
	//! @param		context	コンテキスト規定
	explicit tDeclAttribute(tContextControl context)
	{
		Context = context;
		Sync = scNone;
	}

	//! @brief		コンストラクタ (syncから)
	//! @param		sync	コンテキスト規定
	explicit tDeclAttribute(tSyncControl sync)
	{
		Context = ccNone;
		Sync = sync;
	}

	//! @brief	ダウンキャスト
	operator tMemberAttribute () const { return *(const tMemberAttribute*)this; }

	//! @brief		変更性を設定する
	//! @param		v	変更性
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & Set(tMutabilityControl v) { tMemberAttribute::Set(v); return *this; }

	//! @brief		オーバーライド性を設定する
	//! @param		v	オーバーライド性
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & Set(tOverrideControl v) { tMemberAttribute::Set(v); return *this; }

	//! @brief		プロパティアクセス方法を設定する
	//! @param		v	プロパティアクセス方法
	//! @return		このオブジェクト自身への参照
	tMemberAttribute & Set(tPropertyControl v) { tMemberAttribute::Set(v); return *this; }

	//! @brief		コンテキスト規定を得る
	//! @return		コンテキスト規定
	tContextControl GetContext() const { return Context; }
	//! @brief		コンテキスト規定を設定する
	//! @param		v	コンテキスト規定
	//! @return		このオブジェクト自身への参照
	tDeclAttribute & Set(tContextControl v) { Context = v; return *this; }

	//! @brief		同期動作規定を得る
	//! @return		同期動作規定
	tSyncControl GetSync() const { return Sync; }
	//! @brief		同期動作規定を設定する
	//! @param		v	同期動作規定
	//! @return		このオブジェクト自身への参照
	tDeclAttribute & Set(tSyncControl v) { Sync = v; return *this; }

	//! @brief		属性を上書きする
	//! @param		rhs		上書きする属性
	//! @return		上書きされた属性があった場合に真
	bool Overwrite(const tDeclAttribute & rhs);

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	コンテキスト規定
	bool Has(tContextControl v) const { return Context == v; }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	コンテキスト規定
	bool Has(tSyncControl v) const { return Sync == v; }


	//! @brief		なにか属性を持っているかどうかを調べる
	//! @return		何か属性を持っていれば真
	bool HasAny() const
		{ return tMemberAttribute::HasAny() ||
			Context != ccNone || Sync != scNone;
		}

	//! @brief		属性を文字列化する
	//! @return		文字列化された属性
	tString AsString() const;
};
//---------------------------------------------------------------------------
}
#endif

