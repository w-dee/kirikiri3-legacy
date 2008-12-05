//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tVariantやtObjectInterfaceのOperateメソッドのflags引数の処理
//---------------------------------------------------------------------------
#ifndef risseOperateFlagsH
#define risseOperateFlagsH

#include "risseGC.h"
#include "risseMemberAttribute.h"

//---------------------------------------------------------------------------
namespace Risse
{
//---------------------------------------------------------------------------
class tOperateFlags : public tCollectee
{
	risse_uint32 Flags; //!< フラグの値
public:
	static const risse_uint32 ofMemberEnsure = 0x1000;
	static const risse_uint32 ofInstanceMemberOnly = 0x2000;
	static const risse_uint32 ofFinalOnly = 0x4000;
	static const risse_uint32 ofUseClassMembersRule = 0x8000;

public:
	/**
	 * デフォルトコンストラクタ
	 */
	tOperateFlags() { Flags = 0; }

	/**
	 * コンストラクタ (tMemberAttribute から)
	 * @param attrib	メンバ属性
	 */
	tOperateFlags(tMemberAttribute attrib)
		{ Flags = (risse_uint32)attrib; }

	/**
	 * コンストラクタ (フラグ/risse_uint32から)
	 * @param flags	フラグ
	 */
	tOperateFlags(risse_uint32 flags)
		{ Flags = flags; }

	/**
	 * | 演算子
	 * @param rhs	右辺
	 */
	tOperateFlags operator | (risse_uint32 rhs) const { return tOperateFlags(Flags | rhs); }

	/**
	 * & 演算子
	 * @param rhs	右辺
	 */
	tOperateFlags operator & (risse_uint32 rhs) const { return tOperateFlags(Flags & rhs); }

	/**
	 * bool へのキャスト
	 */
	operator bool () const { return Flags != 0; }

	/**
	 * tMemberAttributeへのキャスト
	 */
	operator tMemberAttribute () const { return tMemberAttribute(Flags); }

	/**
	 * risse_uint32 へのキャスト
	 */
	operator risse_uint32() const { return Flags; }

	/**
	 * 属性を持っているかどうかを調べる
	 * @param v	変更性
	 */
	bool Has(tMemberAttribute::tMutabilityControl v) const { return tMemberAttribute(Flags).Has(v); }

	/**
	 * 属性を持っているかどうかを調べる
	 * @param v	オーバーライド性
	 */
	bool Has(tMemberAttribute::tOverrideControl v) const { return tMemberAttribute(Flags).Has(v); }

	/**
	 * 属性を持っているかどうかを調べる
	 * @param v	プロパティアクセス方法
	 */
	bool Has(tMemberAttribute::tPropertyControl v) const { return tMemberAttribute(Flags).Has(v); }

	/**
	 * 属性を持っているかどうかを調べる
	 * @param v	アクセスコントロール
	 */
	bool Has(tMemberAttribute::tAccessControl v) const { return tMemberAttribute(Flags).Has(v); }

	/**
	 * フラグを持っているかどうかを調べる
	 * @param v	フラグ
	 */
	bool Has(risse_uint32 v) const { return Flags & v; }

	/**
	 * フラグを文字列化する
	 * @return	文字列化されたフラグ
	 */
	tString AsString() const;


};
//---------------------------------------------------------------------------
}
#endif

