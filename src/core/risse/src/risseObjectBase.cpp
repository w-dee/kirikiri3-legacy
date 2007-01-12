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
		if(flags & tRisseOperateFlags::ofInstanceMemberOnly)
			return false; // クラスを探さない場合はここでかえる

		// クラスを探す
		tRisseVariant Class;
		if(!Read(ss_class, tRisseOperateFlags::ofInstanceMemberOnly, Class, This))
			return false; // クラスを特定できない

		// クラスに対してメンバ取得を行う
		tRetValue rv = Class.OperateForMember(ocDGet, &result, name, flags);
		if(rv != rvNoError) return false;
	}

	// TODO: プロパティアクセス、属性チェックなどなど
	result = member->Value;

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
		member->Value = value;
		return true;
	}

	// この時点でメンバはインスタンスには見つかっていない。
	if(flags & tRisseOperateFlags::ofInstanceMemberOnly)
	{
		// クラスを探さない場合は
		if(flags & tRisseOperateFlags::ofMemberEnsure)
		{
			// 新規作成フラグがある場合はメンバを新規作成する
			HashTable.Add(name, tMemberData(tMemberData(value, flags)));
			return true;
		}
		return false; // そうでない場合はメンバは見つからなかったことにする
	}

	// クラスを見に行くが、クラスにプロパティとして動作する
	// メンバがあった場合のみに、そのプロパティを起動する。

	// クラスを探す
	tRisseVariant Class;
	if(Read(ss_class, tRisseOperateFlags::ofInstanceMemberOnly, Class, This))
	{
		// クラスを特定できた場合
		// クラスに対してメンバ設定を行う
		tRetValue result =
			Class.OperateForMember(ocDSet, NULL, name,
							flags|tRisseOperateFlags::ofPropertyOnly,
							tRisseMethodArgument::New(value));
		if(result == rvNoError) return true; // アクセスに成功したので戻る
	}

	if(flags & tRisseOperateFlags::ofMemberEnsure)
	{
		// そのほかの場合、つまりクラスを特定できない場合や、
		// クラスにメンバが無かった場合、
		// クラスにメンバがあったがプロパティとして起動できなかった
		// 場合はこのインスタンスにメンバを作成する。
		HashTable.Add(name, tMemberData(tMemberData(value, flags)));
	}
	else
	{
		member = HashTable.Find(name);

		if(!member) return false; // 見つからなかった
		// TODO: プロパティアクセス、属性チェックなどなど

		member->Value = value;
	}

	return true;
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

	default:
		; // TODO: unhandled operation code support
	}
	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectBase::RegisterNormalMember(const tRisseString & name, const tRisseVariant & value)
{
	Write(name,
		tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcVar)) |
			tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
			value, tRisseVariant(this));
}
//---------------------------------------------------------------------------

} /* namespace Risse */

