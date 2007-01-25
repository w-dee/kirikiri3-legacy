//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクト用C++クラスの基底実装
//---------------------------------------------------------------------------

#ifndef risseObjectBaseH
#define risseObjectBaseH

#include "risseObject.h"
#include "risseVariant.h"
#include "risseHashTable.h"
#include "risseString.h"

namespace Risse
{
class tRisseClass;
//---------------------------------------------------------------------------
//! @brief		"Object" クラス
//---------------------------------------------------------------------------
class tRisseObjectBase : public tRisseObjectInterface
{
public:
	//! @brief	メンバ情報を表す構造体
	struct tMemberData
	{
		tRisseVariant			Value;		//!< 値
		tRisseMemberAttribute	Attribute; //!< メンバの属性

		//! @brief		コンストラクタ
		//! @param		value		値
		//! @param		attrib		メンバの属性
		tMemberData(const tRisseVariant & value, tRisseMemberAttribute attrib) :
			Value(value), Attribute(attrib) {;}

		//! @brief		プロパティアクセス方法を得る
		//! @param		flags		オーバーライドする方法
		//! @return		プロパティアクセス方法
		tRisseMemberAttribute::tPropertyControl GetPropertyControl(tRisseOperateFlags flags) const
		{
			// flags にプロパティアクセス方法が指定されていた場合はそちらを優先する
			tRisseMemberAttribute::tPropertyControl wanted_prop_control =
				flags.operator tRisseMemberAttribute().GetProperty();
			if(wanted_prop_control != tRisseMemberAttribute::pcNone)
				return wanted_prop_control;

			// flags にプロパティアクセス方法が記述されていなかった場合はメンバの属性を返す
			tRisseMemberAttribute::tPropertyControl member_prop_control =
				Attribute.GetProperty();
			RISSE_ASSERT(member_prop_control != tRisseMemberAttribute::pcNone);
			return member_prop_control;
		}
	};

protected:
	typedef tRisseHashTable<tRisseString, tMemberData>
		tHashTable; //!< ハッシュ表の型
	tHashTable HashTable; //!< ハッシュ表
	const tRisseString & PrototypeName;
		//!< プロトタイプ名; このインスタンスにメンバが無かったときに読みに行く先のオブジェクト
	const tRisseVariant * DefaultMethodContext;
		//!< メンバを読み出すときにコンテキストがnullだった場合のデフォルトのコンテキスト(デフォルトのThis)

public:
	//! @brief		コンストラクタ
	tRisseObjectBase();

	//! @brief		コンストラクタ
	//! @param		prototype_name			プロトタイプ名
	//!										(内部でこれへの参照が保持されるので、
	//!										スタック上の文字列を指定しないこと！！！)
	tRisseObjectBase(const tRisseString & prototype_name);

public:

	//! @brief		メンバを読み出す
	//! @param		name		メンバ名
	//! @param		flags		操作フラグ
	//! @param		result		結果の格納先
	//! @param		This		メンバが呼び出される際のThisオブジェクト
	//! @return		読み出せた場合true、メンバが無かった場合にfalse
	//! @note		このインスタンスが保持するハッシュ表以外を読みに行くようなことはない。
	//! 			読み出そうとしたメンバがプロパティの場合はプロパティメソッドを呼び出す。
	//!				(ただしフラグでそれが抑制されていない場合)
	bool Read(const tRisseString & name, tRisseOperateFlags flags, tRisseVariant &result, const tRisseVariant &This);

	//! @brief		メンバに書き込む
	//! @param		name		メンバ名
	//! @param		flags		操作フラグ
	//! @param		value		書き込む値
	//! @param		This		メンバが呼び出される際のThisオブジェクト
	//! @return		書き込めた場合true、メンバが無かった場合にfalse
	//! @note		このインスタンスが保持するハッシュ表以外を参照しに行くようなことはない。
	//! 			書き込もうとしたメンバがプロパティの場合はプロパティメソッドを呼び出す。
	//!				(ただしフラグでそれが抑制されていない場合)
	bool Write(const tRisseString & name, tRisseOperateFlags flags, const tRisseVariant &value, const tRisseVariant &This);

private:
	//! @brief		メンバに値を設定する
	//! @param		name			メンバ名
	//! @param		flags			操作フラグ
	//! @param		member			設定先のデータ
	//! @param		prop_control	プロパティアクセス方法
	//! @param		value			書き込む値
	//! @param		This			メンバが呼び出される際のThisオブジェクト
	void WriteMember(const tRisseString & name, tRisseOperateFlags flags, 
		tMemberData & member, tRisseMemberAttribute::tPropertyControl prop_control,
		const tRisseVariant & value, const tRisseVariant &This);

public:
	//! @brief		(このオブジェクトのメンバに対する)関数呼び出し		FuncCall
	//! @param		name		関数名
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	bool FuncCall(
		tRisseVariantBlock * ret,
		const tRisseString & name, risse_uint32 flags = 0,
		const tRisseMethodArgument & args = tRisseMethodArgument::Empty(),
		const tRisseVariant & This = tRisseVariant::GetNullObject());

	//! @brief		(このオブジェクトのメンバに対する)属性設定		DSetAttrib
	//! @param		name		メンバ名
	//! @param		flags		設定する属性などが含まれたフラグ
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	bool SetAttribute(
		const tRisseString & name, tRisseOperateFlags flags = 0,
			const tRisseVariant & This = tRisseVariant::GetNullObject());

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);


public:
	// ユーティリティ

	//! @brief		普通のメンバ(メソッドや変数、プロパティなど)をインスタンスに登録する(既存のメンバば上書きされる)
	//! @param		name		名前
	//! @param		value		値
	//! @param		attrib		属性
	void RegisterNormalMember(const tRisseString & name,
						const tRisseVariant & value,
						tRisseMemberAttribute attrib = tRisseMemberAttribute());
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
