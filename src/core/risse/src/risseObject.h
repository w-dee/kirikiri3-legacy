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


class tRisseStackFrameContext;
class tRisseMethodArgument;
class tRisseVariantBlock;
typedef tRisseVariantBlock tRisseVariant;
//---------------------------------------------------------------------------

//! @brief		Operateメソッドへの引数(クラス宣言用)
//!	@note		Opereteメソッドの引数が変わるたびにすべての Operate メソッドの
//!				引数を変える気にはなれない。
//! 			あまりマクロは使いたくないが、それにしても
//!				インターフェースの仕様が固まるまではこうしたい
#define RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG \
		tRisseOpCode code,                                                                \
		tRisseVariant * result = NULL,                                                    \
		const tRisseString & name = tRisseString::GetEmptyString(),                       \
		risse_uint32 flags = 0,                                                           \
		const tRisseMethodArgument & args = tRisseMethodArgument::New(),                  \
		const tRisseMethodArgument & bargs = tRisseMethodArgument::New(),                 \
		const tRisseVariant *This = NULL,                                                 \
		const tRisseStackFrameContext *stack = NULL

//! @brief		Operateメソッドへの引数(実装用)
#define RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG \
		tRisseOpCode code,                     \
		tRisseVariant * result,                \
		const tRisseString & name,             \
		risse_uint32 flags,                    \
		const tRisseMethodArgument & args,     \
		const tRisseMethodArgument & bargs,    \
		const tRisseVariant *This,             \
		const tRisseStackFrameContext *stack

//! @brief		Operateメソッドの引数一覧
#define RISSE_OBJECTINTERFACE_PASS_ARG \
		code, result, name, flags, args, bargs, This, stack

//---------------------------------------------------------------------------
//! @brief		Risseオブジェクトインターフェース
//---------------------------------------------------------------------------
class tRisseObjectInterface
{
public:

	//! @brief		オブジェクトに対して操作を行う
	//! @param		code	オペレーションコード
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		bargs	ブロック引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	//! @note		何か操作に失敗した場合は例外が発生する。このため、このメソッドに
	//!				エラーコードなどの戻り値はない
	virtual void Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG) = 0;
};
//---------------------------------------------------------------------------

}
#endif

