//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトクラスの基底実装
//---------------------------------------------------------------------------

#ifndef risseObjectBaseH
#define risseObjectBaseH

#include "risseObject.h"
#include "risseVariant.h"
#include "risseHashTable.h"
#include "risseString.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseObjectBase : public tRisseObjectInterface
{
public:
	//! @brief	メンバ情報を表す構造体
	struct tMemberData
	{
		tRisseMemberAttribute	Attribute; //!< メンバの属性
		tRisseVariant			Value;		//!< 値
	};

protected:
	typedef tRisseHashTable<tRisseString, tMemberData>
		tHashTable; //!< ハッシュ表の型
	tHashTable HashTable; //!< ハッシュ表

public:
	//! @brief		メンバを読み出す
	//! @param		name		メンバ名
	//! @param		flags		操作フラグ
	//! @param		This		メンバが呼び出される際のThisオブジェクト
	//! @param		result		結果の格納先
	//! @return		読み出せた場合true、メンバが無かった場合にfalse
	//! @note		このインスタンスが保持するハッシュ表以外を読みに行くようなことはない。
	//! 			読み出そうとしたメンバがプロパティの場合はプロパティメソッドを呼び出す。
	//!				(ただしフラグでそれが抑制されていない場合)
	bool Read(const tRisseString & name, tjs_uint32 flags, const tRisseVariant &This, tRisseVariant &result);

	//! @brief		メンバに書き込む
	//! @param		name		メンバ名
	//! @param		flags		操作フラグ
	//! @param		This		メンバが呼び出される際のThisオブジェクト
	//! @param		value		書き込む値
	//! @return		書き込めた場合true、メンバが無かった場合にfalse
	//! @note		このインスタンスが保持するハッシュ表以外を参照しに行くようなことはない。
	//! 			書き込もうとしたメンバがプロパティの場合はプロパティメソッドを呼び出す。
	//!				(ただしフラグでそれが抑制されていない場合)
	bool Write(const tRisseString & name, tjs_uint32 flags, const tRisseVariant &This, const tRisseVariant &value);


	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
