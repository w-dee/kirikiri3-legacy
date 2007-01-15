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
	static const risse_uint32 ofMemberEnsure = 0x1000;  //!< メンバが無かった場合に作成を行う
	static const risse_uint32 ofInstanceMemberOnly = 0x2000; //!< インスタンスメンバのみ参照(クラスのメンバを見に行かない)
	static const risse_uint32 ofPropertyOnly = 0x4000; //!< プロパティとして起動できる物のみを探す(見つからなかった場合やプロパティとして起動できない場合はrvMemberNotFoundがかえる)
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

	//! @brief		フラグを文字列化する
	//! @return		文字列化されたフラグ
	tRisseString AsString() const;


};
//---------------------------------------------------------------------------
}
#endif

