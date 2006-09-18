//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトの C++ インターフェースの定義と実装
//---------------------------------------------------------------------------
#ifndef risseObjectH
#define risseObjectH

#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseGC.h"
#include "risseString.h"
#include "risseOpCodes.h"
#include "risseMethod.h"

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
	tAccessControl		Access		: 2;
	tVisibilityControl	Visibility	: 2;
	tOverrideControl	Override	: 2;
	tPropertyControl	Property	: 2;

public:
	//! @brief		デフォルトコンストラクタ
	tRisseMemberAttribute()
	{
		Access = acNone;
		Visibility = vcNone;
		Override = ocNone;
		Property = pcNone;
	}

	//! @brief		コンストラクタ (accessから)
	//! @param		access	アクセス制限
	tRisseMemberAttribute(tAccessControl access)
	{
		Access = access;
		Visibility = vcNone;
		Override = ocNone;
		Property = pcNone;
	}

	//! @brief		コンストラクタ (visibilityから)
	//! @param		visibility	可視性
	tRisseMemberAttribute(tVisibilityControl visibility)
	{
		Access = acNone;
		Visibility = visibility;
		Override = ocNone;
		Property = pcNone;
	}

	//! @brief		コンストラクタ (overrideから)
	//! @param		override	オーバーライド性
	tRisseMemberAttribute(tOverrideControl override)
	{
		Access = acNone;
		Visibility = vcNone;
		Override = override;
		Property = pcNone;
	}

	//! @brief		コンストラクタ (propertyから)
	//! @param		property	プロパティアクセス方法
	tRisseMemberAttribute(tPropertyControl property)
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

	//! @brief		属性を文字列化する
	tRisseString AsString() const;
};
//---------------------------------------------------------------------------


class tRisseStackFrameClosure;
class tRisseMethodArgument;
class tRisseExecutorContext;
class tRisseVariantBlock;
typedef tRisseVariantBlock tRisseVariant;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		Risseオブジェクトインターフェース
//---------------------------------------------------------------------------
class tRisseObjectInterface
{
public:

	//! @brief		オブジェクトに対して操作を行う
	//! @param		context	実行コンテキスト
	//! @note		呼び出しに関する引数などの情報はcontext->GetTop().Info を参照のこと。@r
	//! @note		別のOperateを呼び出したい場合、context->PushCallee を呼び出して
	//!				別のOperateを登録してからreturnする。
	//!				その「別のOperate」から実行が戻ると、再びこのOperateが呼ばれる。
	//!				その際、２度目以降の実行かどうかの判断をするために
	//!				context->GetTop().State に情報を登録すること(ここは初回の呼び出しの
	//!				場合はNULLになっている) @r
	//! @note		何か操作に失敗した場合は例外が発生する。このため、このメソッドに
	//!				エラーコードなどの戻り値はない。
	virtual void Operate(tRisseExecutorContext * context) = 0;
};
//---------------------------------------------------------------------------

}
#endif

