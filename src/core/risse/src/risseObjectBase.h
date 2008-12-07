//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
/**
 * "Object" クラス
 */
class tObjectBase : public tObjectInterface
{
public:
	/**
	 * tClassBase のtypedef
	 */
	typedef ::Risse::tClassBase tClassBase;

	/**
	 * メンバ情報を表す構造体
	 */
	struct tMemberData : public tCollectee
	{
		tVariant			Value;		//!< 値
		tMemberAttribute	Attribute; //!< メンバの属性

		/**
		 * コンストラクタ
		 * @param value		値
		 * @param attrib	メンバの属性
		 */
		tMemberData(const tVariant & value, tMemberAttribute attrib) :
			Value(value), Attribute(attrib) {;}
	};

	/**
	 * メンバのenumuration用コールバックインターフェース
	 */
	class tEnumMemberCallback : public tCollectee
	{
	public:
		virtual ~ tEnumMemberCallback() {;} // おそらく呼ばれない
		/**
		 * コールバック
		 * @param key	キー(文字列)
		 * @param data	tMemberData 構造体
		 * @return	真ならばenumuration継続、偽ならば中止
		 */
		virtual bool OnEnum(const tString & name, const tMemberData & data) = 0;
	};


protected:
	typedef tHashTable<tString, tMemberData>
		tMemberHashTable; //!< ハッシュ表の型
	tMemberHashTable HashTable; //!< ハッシュ表
	const tString & PrototypeName;
		//!< プロトタイプ名; このインスタンスにメンバが無かったときに読みに行く先のオブジェクトの名前
	const tString & MembersName;
		//!< Members名; ofUseMembersRule が指定された場合に読みに行く先のオブジェクトの名前
	const tVariant * DefaultMethodContext;
		//!< メンバを読み出すときにコンテキストがnullだった場合のデフォルトのコンテキスト(デフォルトのThis)

public:
	/**
	 * コンストラクタ
	 */
	tObjectBase();

	/**
	 * コンストラクタ
	 * @param prototype_name	プロトタイプ名
	 *							(内部でこれへの参照が保持されるので、
	 *							スタック上の文字列を指定しないこと！！！)
	 * @param members_name		Members名
	 *							(内部でこれへの参照が保持されるので、
	 *							スタック上の文字列を指定しないこと！！！)
	 */
	tObjectBase(const tString & prototype_name, const tString & members_name = tString::GetEmptyString());

public:
	/**
	 * メンバを列挙する
	 * @param callback	コールバックインターフェース
	 * @note	このオブジェクトのメンバのみを列挙する
	 */
	void Enumurate(tEnumMemberCallback * callback);

public:

	/**
	 * メンバを読み出す
	 * @param name		メンバ名
	 * @param flags		操作フラグ
	 * @param result	結果の格納先
	 * @param This		メンバが呼び出される際のThisオブジェクト
	 * @return	結果
	 * @note	このインスタンスが保持するハッシュ表以外を読みに行くようなことはない。
	 *			読み出そうとしたメンバがプロパティの場合はプロパティメソッドを呼び出す。
	 *			(ただしフラグでそれが抑制されていない場合)
	 */
	tRetValue Read(const tString & name, tOperateFlags flags,
				tVariant &result, const tVariant &This) const;

	/**
	 * メンバに書き込む
	 * @param name	メンバ名
	 * @param flags	操作フラグ
	 * @param value	書き込む値
	 * @param This	メンバが呼び出される際のThisオブジェクト
	 * @return	結果
	 * @note	このインスタンスが保持するハッシュ表以外を参照しに行くようなことはない。
	 *			書き込もうとしたメンバがプロパティの場合はプロパティメソッドを呼び出す。
	 *			(ただしフラグでそれが抑制されていない場合)
	 */
	tRetValue Write(const tString & name, tOperateFlags flags,
				const tVariant &value, const tVariant &This);

	/**
	 * メンバを削除する
	 * @param name	メンバ名
	 * @param flags	操作フラグ
	 * @return	結果
	 */
	tRetValue Delete(const tString & name, tOperateFlags flags);

private:
public:
	/**
	 * (このオブジェクトのメンバに対する)関数呼び出し		FuncCall
	 * @param name	関数名
	 * @param ret	関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	 * @param flags	呼び出しフラグ
	 * @param args	引数
	 * @param This	このメソッドが実行されるべき"Thisオブジェクト"
	 * @return	結果
	 */
	tRetValue FuncCall(
		tVariantBlock * ret,
		const tString & name, risse_uint32 flags = 0,
		const tMethodArgument & args = tMethodArgument::Empty(),
		const tVariant & This = tVariant::GetNullObject());

	/**
	 * (このオブジェクトのメンバあるいは自分自身に対する)インスタンスの作成		New
	 * @param name	メンバ名
	 * @param ret	結果の格納先(NULL=結果は必要なし)
	 * @param flags	呼び出しフラグ
	 * @param args	引数
	 * @param This	このメソッドが実行されるべき"Thisオブジェクト"
	 * @return	結果
	 */
	tRetValue New(
		tVariantBlock * ret,
		const tString & name, risse_uint32 flags = 0,
		const tMethodArgument & args = tMethodArgument::Empty(),
		const tVariant & This = tVariant::GetNullObject());

	/**
	 * (このオブジェクトのメンバに対する)属性設定		DSetAttrib
	 * @param name	メンバ名
	 * @param flags	設定する属性などが含まれたフラグ
	 * @param This	このメソッドが実行されるべき"Thisオブジェクト"
	 * @return	結果
	 */
	tRetValue SetAttribute(
		const tString & name, tOperateFlags flags = 0,
			const tVariant & This = tVariant::GetNullObject());

	/**
	 * (このオブジェクトに対して)特定クラスのインスタンスかどうかを調べる InstanceOf
	 * @param RefClass	クラスオブジェクト
	 * @param flags		呼び出しフラグ
	 * @param This		このメソッドが実行されるべき"Thisオブジェクト"
	 * @return	特定クラスのインスタンスだった場合に真、そうでなければ偽
	 */
	bool InstanceOf(
		const tVariant & RefClass, risse_uint32 flags = 0,
		const tVariant & This = tVariant::GetNullObject()
		);

	/**
	 * オブジェクトに対して操作を行う
	 */
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);


public:
	// ユーティリティ

	/**
	 * 普通のメンバ(メソッドや変数、プロパティなど)をインスタンスに登録する(既存のメンバば上書きされる)
	 * @param name		名前
	 * @param value		値
	 * @param attrib	属性
	 * @param ismembers	members に登録する場合は true (membersがもしあるばあいのみ) 
	 * @note	登録に失敗した場合は例外が発生する。
	 *			フラグとしてはtOperateFlags::ofMemberEnsure|
	 *			tOperateFlags::ofInstanceMemberOnlyが自動的に付加される。
	 */
	void RegisterNormalMember(const tString & name,
						const tVariant & value,
						tMemberAttribute attrib = tMemberAttribute(), bool ismembers = false);


	/**
	 * final const なメンバ(メソッドや変数、プロパティなど)をインスタンスに登録する(既存のメンバば上書きされる)
	 * @param name		名前
	 * @param value		値
	 * @param ismembers	members に登録する場合は true (membersがもしあるばあいのみ) 
	 * @note	登録に失敗した場合は例外が発生する。
	 *			フラグとしてはtOperateFlags::ofMemberEnsure|
	 *			tOperateFlags::ofInstanceMemberOnlyが自動的に付加される。
	 */
	void RegisterFinalConstMember(const tString & name,
						const tVariant & value,
						bool ismembers = false)
	{
		RegisterNormalMember(name, value,
			tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal),
			ismembers);
	}



	/**
	 * メンバを読み出す
	 * @param name	名前
	 * @param flags	フラグ
	 * @return	読み出された内容
	 * @note	読み出しに失敗した場合は例外が発生する。
	 *			RegisterNormalMember() とは違い、flags はそのまま使われる。
	 */
	tVariant ReadMember(const tString & name,
						risse_uint32 flags = 0) const;


	/**
	 * プロパティ(や変数など) の読み込みを行う
	 * @param name	メンバ名
	 * @param flags	フラグ
	 */
	tVariant GetPropertyDirect(const tString & name, risse_uint32 flags = 0);

	/**
	 * プロパティ(や変数など) の書き込みを行う
	 * @param name	メンバ名
	 * @param flags	フラグ
	 * @param val	値
	 */
	void SetPropertyDirect(const tString & name, risse_uint32 flags, const tVariant &val);

	/**
	 * (このオブジェクトのメンバに対する)単純な関数呼び出し		Invoke
	 * @param membername	メンバ名
	 * @return	戻り値
	 */
	tVariantBlock Invoke(const tString & membername);

	/**
	 * (このオブジェクトのメンバに対する)単純な関数呼び出し		Invoke
	 * @param membername	メンバ名
	 * @param arg1			引数
	 * @return	戻り値
	 */
	tVariantBlock Invoke(
		const tString & membername,
		const tVariant & arg1);

	/**
	 * (このオブジェクトのメンバに対する)単純な関数呼び出し		Invoke
	 * @param membername	メンバ名
	 * @param arg1			引数
	 * @param arg2			引数
	 * @return	戻り値
	 */
	tVariantBlock Invoke(
		const tString & membername,
		const tVariant & arg1,
		const tVariant & arg2
		);

	/**
	 * (このオブジェクトのメンバに対する)単純な関数呼び出し		Invoke
	 * @param membername	メンバ名
	 * @param arg1			引数
	 * @param arg2			引数
	 * @param arg3			引数
	 * @return	戻り値
	 */
	tVariantBlock Invoke(
		const tString & membername,
		const tVariant & arg1,
		const tVariant & arg2,
		const tVariant & arg3
		);

};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
