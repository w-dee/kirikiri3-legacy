//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tRisseVariantやtRisseObjectInterfaceのOperateメソッドのflags引数の処理
//---------------------------------------------------------------------------
#ifndef risseOperateFlagsH
#define risseOperateFlagsH

#include "risseGC.h"
#include "risseMemberAttribute.h"

//---------------------------------------------------------------------------
namespace Risse
{
//---------------------------------------------------------------------------
class tRisseOperateFlags : public tRisseCollectee
{
	risse_uint32 Flags; //!< フラグの値
public:
	static const risse_uint32 ofMemberEnsure = 0x1000;
		//!< メンバが無かった場合に作成を行う(ocDSetのみ)
	static const risse_uint32 ofInstanceMemberOnly = 0x2000;
		//!< インスタンスメンバのみ参照(クラスのメンバを見に行かない)
		//!< (ocDSet/ocDGetやメンバを参照するもの全般)
	static const risse_uint32 ofPropertyOrConstOnly = 0x4000;
		//!< プロパティとして起動できる物か定数のみを探す(見つからなかった場合
		//!< やプロパティとして起動できない場合はrvMemberNotFoundがかえるが、
		//!< 見つかったのが定数の場合は例外が発生する)(ocDSetのみ)
	static const risse_uint32 ofNoSetDefaultContext = 0x8000;
		//!< デフォルトのコンテキストを設定しない

public:
	//! @brief		デフォルトコンストラクタ
	tRisseOperateFlags() { Flags = 0; }

	//! @brief		コンストラクタ (tRisseMemberAttribute から)
	//! @param		attrib		メンバ属性
	tRisseOperateFlags(tRisseMemberAttribute attrib)
		{ Flags = (risse_uint32)attrib; }

	//! @brief		コンストラクタ (フラグ/risse_uint32から)
	//! @param		flags		フラグ
	tRisseOperateFlags(risse_uint32 flags)
		{ Flags = flags; }

	//! @brief		| 演算子
	//! @param		rhs		右辺
	tRisseOperateFlags operator | (risse_uint32 rhs) const { return tRisseOperateFlags(Flags | rhs); }

	//! @brief		& 演算子
	//! @param		rhs		右辺
	tRisseOperateFlags operator & (risse_uint32 rhs) const { return tRisseOperateFlags(Flags & rhs); }

	//! @brief		bool へのキャスト
	operator bool () const { return Flags != 0; }

	//! @brief		tRisseMemberAttributeへのキャスト
	operator tRisseMemberAttribute () const { return tRisseMemberAttribute(Flags); }

	//! @brief		risse_uint32 へのキャスト
	operator risse_uint32() const { return Flags; }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	アクセス制限
	bool Has(tRisseMemberAttribute::tAccessControl v) const { return tRisseMemberAttribute(Flags).Has(v); }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	可視性
	bool Has(tRisseMemberAttribute::tVisibilityControl v) const { return tRisseMemberAttribute(Flags).Has(v); }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	オーバーライド性
	bool Has(tRisseMemberAttribute::tOverrideControl v) const { return tRisseMemberAttribute(Flags).Has(v); }

	//! @brief		属性を持っているかどうかを調べる
	//! @param		v	プロパティアクセス方法
	bool Has(tRisseMemberAttribute::tPropertyControl v) const { return tRisseMemberAttribute(Flags).Has(v); }

	//! @brief		フラグを持っているかどうかを調べる
	//! @param		v	フラグ
	bool Has(risse_uint32 v) const { return Flags & v; }

	//! @brief		フラグを文字列化する
	//! @return		文字列化されたフラグ
	tRisseString AsString() const;


};
//---------------------------------------------------------------------------
}
#endif

