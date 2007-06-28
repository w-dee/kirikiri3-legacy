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
//---------------------------------------------------------------------------
//! @brief		"Object" クラス
//---------------------------------------------------------------------------
class tRisseObjectBase : public tRisseObjectInterface
{
public:
	//! @brief	メンバ情報を表す構造体
	struct tMemberData : public tRisseCollectee
	{
		tRisseVariant			Value;		//!< 値
		tRisseMemberAttribute	Attribute; //!< メンバの属性

		//! @brief		コンストラクタ
		//! @param		value		値
		//! @param		attrib		メンバの属性
		tMemberData(const tRisseVariant & value, tRisseMemberAttribute attrib) :
			Value(value), Attribute(attrib) {;}
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
	//! @return		結果
	//! @note		このインスタンスが保持するハッシュ表以外を読みに行くようなことはない。
	//! 			読み出そうとしたメンバがプロパティの場合はプロパティメソッドを呼び出す。
	//!				(ただしフラグでそれが抑制されていない場合)
	tRetValue Read(const tRisseString & name, tRisseOperateFlags flags,
				tRisseVariant &result, const tRisseVariant &This) const;

	//! @brief		メンバに書き込む
	//! @param		name		メンバ名
	//! @param		flags		操作フラグ
	//! @param		value		書き込む値
	//! @param		This		メンバが呼び出される際のThisオブジェクト
	//! @return		結果
	//! @note		このインスタンスが保持するハッシュ表以外を参照しに行くようなことはない。
	//! 			書き込もうとしたメンバがプロパティの場合はプロパティメソッドを呼び出す。
	//!				(ただしフラグでそれが抑制されていない場合)
	tRetValue Write(const tRisseString & name, tRisseOperateFlags flags,
				const tRisseVariant &value, const tRisseVariant &This);

	//! @brief		メンバを削除する
	//! @param		name		メンバ名
	//! @param		flags		操作フラグ
	//! @return		結果
	tRetValue Delete(const tRisseString & name, tRisseOperateFlags flags);

private:
public:
	//! @brief		(このオブジェクトのメンバに対する)関数呼び出し		FuncCall
	//! @param		name		関数名
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//! @return		結果
	tRetValue FuncCall(
		tRisseVariantBlock * ret,
		const tRisseString & name, risse_uint32 flags = 0,
		const tRisseMethodArgument & args = tRisseMethodArgument::Empty(),
		const tRisseVariant & This = tRisseVariant::GetNullObject());

	//! @brief		(このオブジェクトのメンバあるいは自分自身に対する)インスタンスの作成		New
	//! @param		name		メンバ名
	//! @param		ret			結果の格納先(NULL=結果は必要なし)
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//! @return		結果
	tRetValue New(
		tRisseVariantBlock * ret,
		const tRisseString & name, risse_uint32 flags = 0,
		const tRisseMethodArgument & args = tRisseMethodArgument::Empty(),
		const tRisseVariant & This = tRisseVariant::GetNullObject());

	//! @brief		(このオブジェクトのメンバに対する)属性設定		DSetAttrib
	//! @param		name		メンバ名
	//! @param		flags		設定する属性などが含まれたフラグ
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//! @return		結果
	tRetValue SetAttribute(
		const tRisseString & name, tRisseOperateFlags flags = 0,
			const tRisseVariant & This = tRisseVariant::GetNullObject());

	//! @brief		(このオブジェクトに対して)特定クラスのインスタンスかどうかを調べる InstanceOf
	//! @param		RefClass	クラスオブジェクト
	//! @param		flags		呼び出しフラグ
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//! @return		特定クラスのインスタンスだった場合に真、そうでなければ偽
	bool InstanceOf(
		const tRisseVariant & RefClass, risse_uint32 flags = 0,
		const tRisseVariant & This = tRisseVariant::GetNullObject()
		);

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);


public:
	// ユーティリティ

	//! @brief		普通のメンバ(メソッドや変数、プロパティなど)をインスタンスに登録する(既存のメンバば上書きされる)
	//! @param		name		名前
	//! @param		value		値
	//! @param		attrib		属性
	//! @note		登録に失敗した場合は例外が発生する。
	//!				フラグとしてはtRisseOperateFlags::ofMemberEnsure|
	//!				tRisseOperateFlags::ofInstanceMemberOnlyが自動的に付加される。
	void RegisterNormalMember(const tRisseString & name,
						const tRisseVariant & value,
						tRisseMemberAttribute attrib = tRisseMemberAttribute());

	//! @brief		メンバを読み出す
	//! @param		name		名前
	//! @param		flags		フラグ
	//! @return		読み出された内容
	//! @note		読み出しに失敗した場合は例外が発生する。
	//!				RegisterNormalMember() とは違い、flags はそのまま使われる。
	tRisseVariant ReadMember(const tRisseString & name,
						risse_uint32 flags = 0) const;


	//! @brief		プロパティ(や変数など) の読み込みを行う
	//! @param		name		メンバ名
	//! @param		flags		フラグ
	tRisseVariant GetPropertyDirect(const tRisseString & name, risse_uint32 flags = 0);
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
