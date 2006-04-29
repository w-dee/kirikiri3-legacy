//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトの C++ インターフェースの実装
//---------------------------------------------------------------------------
#ifndef risseCxxObjectH
#define risseCxxObjectH

#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseGC.h"
#include "risseCxxString.h"


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



typedef void * tRisseObjectImpl;
//---------------------------------------------------------------------------
//! @brief	オブジェクト用データ
//! @note
//! ポインタの最下位の2ビットが常に 01 なのは、このポインタが オブジェクトであることを
//! 表している。ポインタは常に少なくとも 32bit 境界に配置されるため、最下位の２ビットは
//! オブジェクトのタイプを表すのに利用されている。tRisseVariantを参照。
//---------------------------------------------------------------------------
class tRisseObjectData : public tRisseCollectee
{
	tRisseObjectImpl * Impl; //!< ブロックへのポインタ (最下位の2ビットは常に10なので注意)
							//!< アクセス時は必ず GetBlock, SetBlock を用いること

protected: // pointer operation
	void SetImpl(tRisseObjectImpl * impl)
		{ Impl = reinterpret_cast<tRisseObjectImpl*>(reinterpret_cast<risse_ptruint>(impl) + 1); }

	tRisseObjectImpl * GetImpl() const
		{ return reinterpret_cast<tRisseObjectImpl*>(reinterpret_cast<risse_ptruint>(Impl) - 1); }

public:
	//! @brief null ポインタ。オブジェクトが null を表す場合は、この値をとる。
	#define RISSE_OBJECT_NULL_PTR (reinterpret_cast<tRisseObjectImpl*>((risse_ptruint)0x10))
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
class tRisseObjectBlock : public tRisseObjectData
{
public:
	// デフォルトコンストラクタ
	tRisseObjectBlock()
	{
		SetImpl(RISSE_OBJECT_NULL_PTR);
	}
};
//---------------------------------------------------------------------------

typedef tRisseObjectBlock tRisseObject; //!< いまのところ tRisseObject は tRisseObjectBlock と同じ

//---------------------------------------------------------------------------
}
#endif

