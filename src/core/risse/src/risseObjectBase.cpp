/*---------------------------------------------------------------------------*/
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

#include "risseTypes.h"
#include "risseObjectBase.h"
#include "risseOperateFlags.h"
#include "risseStaticStrings.h"

/*
	ここではオブジェクトの実装に必要な基底の機能を実装する
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(45114,31718,49668,18467,56195,41722,1990,5427);

//---------------------------------------------------------------------------
bool tRisseObjectBase::Read(const tRisseString & name, tRisseOperateFlags flags,
	tRisseVariant &result, const tRisseVariant &This)
{
	tMemberData * member = HashTable.Find(name);

	if(!member)
	{
		if(flags.Has(tRisseOperateFlags::ofInstanceMemberOnly))
			return false; // クラスを探さない場合はここでかえる

		// クラスを探す
		tRisseVariant Class;
		if(!Read(ss_class, tRisseOperateFlags::ofInstanceMemberOnly, Class, This))
			return false; // クラスを特定できない

		// クラスに対してメンバ取得を行う
		tRetValue rv = Class.OperateForMember(ocDGet, &result, name, flags,
					tRisseMethodArgument::Empty(), tRisseMethodArgument::Empty(), This);
		if(rv != rvNoError) return false;
		return true;
	}

	// プロパティアクセスの方法を決定する
	tRisseMemberAttribute::tPropertyControl member_prop_control = member->GetPropertyControl(flags);
	// プロパティアクセスの方法に従って情報を取得する
	switch(member_prop_control)
	{
	case tRisseMemberAttribute::pcNone: // あり得ない(上でASSERT)
		break;

	case tRisseMemberAttribute::pcVar: // 普通のメンバ
	case tRisseMemberAttribute::pcConst: // 定数
		// 単純に、結果に値をコピーする
		result = member->Value;
		break;

	case tRisseMemberAttribute::pcProperty: // プロパティアクセス
		// member->Value を引数なしで関数呼び出しし、その結果を得る
		tRetValue rv = member->Value.Operate(ocFuncCall, &result, tRisseString::GetEmptyString(),
					flags, tRisseMethodArgument::Empty(), tRisseMethodArgument::Empty(), This);
		if(rv != rvNoError) return false;
		break;
	}

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseObjectBase::Write(const tRisseString & name, tRisseOperateFlags flags,
	const tRisseVariant &value, const tRisseVariant &This)
{
	tMemberData * member;

	// メンバを新規作成する
	// TODO: プロパティアクセス、属性チェックなどなど
	member = HashTable.Find(name);

	if(member)
	{
		// メンバが見つかったのでこれに上書きをする
		// そのまえに属性チェック
		tRisseMemberAttribute::tPropertyControl member_prop_control = member->GetPropertyControl(flags);

		if(flags.Has(tRisseOperateFlags::ofPropertyOrConstOnly))
		{
			// プロパティやconstへの書き込みのみ
			switch(member_prop_control)
			{
			case tRisseMemberAttribute::pcNone: // あり得ない(上でASSERT)
				break;

			case tRisseMemberAttribute::pcVar: // 普通のメンバ
				return false; // 見つからなかったというのと同じ扱い

			case tRisseMemberAttribute::pcConst: // 定数
				// 定数への書き込みは常に失敗するのでそのまま通過させる
				// 下で処理
				break;

			case tRisseMemberAttribute::pcProperty: // プロパティアクセス
				// 下で処理
				break;
			}
		}

		// 値を書き込む
		if(member_prop_control == tRisseMemberAttribute::pcVar)
			member->Value = value;
		else
			WriteMember(name, flags, *member, member_prop_control, value, This);
		return true;
	}

	// この時点でメンバはインスタンスには見つかっていない。
	if(flags.Has(tRisseOperateFlags::ofInstanceMemberOnly))
	{
		// クラスを探さない場合は
		if(flags.Has(tRisseOperateFlags::ofMemberEnsure))
		{
			// 新規作成フラグがある場合はメンバを新規作成する
			tRisseMemberAttribute attrib = flags;
			if(attrib.GetProperty() == tRisseMemberAttribute::pcNone)
				attrib.SetProperty(tRisseMemberAttribute::pcVar); // デフォルトはpcVar
			HashTable.Add(name, tMemberData(tMemberData(value, attrib)));
			return true;
		}
		return false; // そうでない場合はメンバは見つからなかったことにする
	}

	// クラスを見に行くが、クラスにプロパティとして動作する
	// メンバがあった場合のみに、そのプロパティを起動する。
	// または、クラスを探しに行ったときに const があった場合は
	// それに書き込みを試みる(当然エラーになるが、意図した動作である)

	// クラスを探す
	tRisseVariant Class;
	if(Read(ss_class, tRisseOperateFlags::ofInstanceMemberOnly, Class, This))
	{
		// クラスを特定できた場合
		// クラスに対してメンバ設定を行う
		tRetValue result =
			Class.OperateForMember(ocDSet, NULL, name,
							flags|tRisseOperateFlags::ofPropertyOrConstOnly,
							tRisseMethodArgument::New(value),
							tRisseMethodArgument::Empty(), This);
		// ちなみに見つかったのが定数で、書き込みに失敗した場合は
		// 例外が飛ぶので OperateForMember は戻ってこない。
		if(result == rvNoError) return true; // アクセスに成功したので戻る
	}

	if(flags.Has(tRisseOperateFlags::ofMemberEnsure))
	{
		// そのほかの場合、つまりクラスを特定できない場合や、
		// クラスにメンバが無かった場合、
		// クラスにメンバがあったがプロパティとして起動できなかった
		// 場合はこのインスタンスにメンバを作成する。
		tRisseMemberAttribute attrib = flags;
		if(attrib.GetProperty() == tRisseMemberAttribute::pcNone)
			attrib.SetProperty(tRisseMemberAttribute::pcVar); // デフォルトはpcVar
		HashTable.Add(name, tMemberData(tMemberData(value, attrib)));
		return true;
	}
	else
	{
		member = HashTable.Find(name);

		if(!member) return false; // 見つからなかった

		tRisseMemberAttribute::tPropertyControl member_prop_control = member->GetPropertyControl(flags);

		// 値を書き込む
		if(member_prop_control == tRisseMemberAttribute::pcVar)
			member->Value = value;
		else
			WriteMember(name, flags, *member, member_prop_control, value, This);
		return true;
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectBase::WriteMember(const tRisseString & name, tRisseOperateFlags flags, 
	tMemberData & member, tRisseMemberAttribute::tPropertyControl prop_control,
	const tRisseVariant & value, const tRisseVariant &This)
{
	switch(prop_control)
	{
	case tRisseMemberAttribute::pcNone: // あり得ない
		break;

	case tRisseMemberAttribute::pcVar: // 普通のメンバ
		member.Value = value;
		break;

	case tRisseMemberAttribute::pcConst: // 定数
		// 定数には書き込みできません
		RisseThrowMemberIsReadOnly(name);
		break;

	case tRisseMemberAttribute::pcProperty: // プロパティアクセス
		// プロパティハンドラを起動する
		// プロパティの設定の場合は読み出しと違い、ocFuncCallではなくてocDSetを
		// つかう。
		member.Value.Do(ocDSet, NULL, tRisseString::GetEmptyString(),
					flags, tRisseMethodArgument::New(value), tRisseMethodArgument::Empty(), This);
		break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseObjectBase::FuncCall(
		tRisseVariantBlock * ret,
		const tRisseString & name, risse_uint32 flags,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant & This)
{
	if(!name.IsEmpty())
	{
		// TODO: このオブジェクトに対するメソッド呼び出し

		// メンバを読み出す
		tRisseVariant function_object;
		if(!Read(name, flags, function_object, This)) return false;

		// メンバに対して関数呼び出しを実行する
		function_object.FuncCall(ret, flags, args, bargs, This);
	}
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseObjectBase::SetAttribute(
		const tRisseString & name, tRisseOperateFlags flags, const tRisseVariant & This)
{
	tMemberData * member = HashTable.Find(name);

	if(!member)
	{
		if(flags.Has(tRisseOperateFlags::ofInstanceMemberOnly))
			return false; // クラスを探さない場合はここでかえる

		// クラスを探す
		tRisseVariant Class;
		if(!Read(ss_class, tRisseOperateFlags::ofInstanceMemberOnly, Class, This))
			return false; // クラスを特定できない

		// クラスに対して属性設定を行う
		tRetValue rv = Class.OperateForMember(ocDSetAttrib, NULL, name, flags,
					tRisseMethodArgument::Empty(), tRisseMethodArgument::Empty(), This);
		if(rv != rvNoError) return false;
		return true;
	}

	// 属性を設定する
	member->Attribute.Overwrite(flags);

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectBase::tRetValue tRisseObjectBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	switch(code)
	{
	case ocDGet:
		// property get
		// TODO: このオブジェクトそのもに対する操作への対応
		if(!Read(name, flags, *result, This))
			return rvMemberNotFound;
		return rvNoError;

	case ocDSet:
		// property set
		// TODO: このオブジェクトそのもに対する操作への対応
		if(args.GetCount() < 1) { ; /* TODO: raise an error */ }
		if(!Write(name, flags, args[0], This))
			return rvMemberNotFound;
		return rvNoError;

	case ocFuncCall:
		// function call
		if(!FuncCall(result, name, flags, args, bargs, This))
			return rvMemberNotFound;
		return rvNoError;

	case ocDSetAttrib:
		// set member attrubute
		if(!SetAttribute(name, flags))
			return rvMemberNotFound;
		return rvNoError;

	default:
		; // TODO: unhandled operation code support
	}
	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectBase::RegisterNormalMember(const tRisseString & name,
			const tRisseVariant & value, tRisseMemberAttribute attrib)
{
	tRisseOperateFlags access_flags =
		tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly;
	Write(name,
		tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcVar)) |
							access_flags, value, tRisseVariant(this));
	if(attrib.HasAny())
		SetAttribute(name, access_flags|attrib);
}
//---------------------------------------------------------------------------

} /* namespace Risse */

