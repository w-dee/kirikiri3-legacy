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
#include "prec.h"
#include "risseTypes.h"
#include "risseObjectBase.h"
#include "risseOperateFlags.h"
#include "risseStaticStrings.h"
#include "risseExceptionClass.h"

/*
	ここではオブジェクトの実装に必要な基底の機能を実装する
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(45114,31718,49668,18467,56195,41722,1990,5427);

//---------------------------------------------------------------------------
tRisseObjectBase::tRisseObjectBase() : PrototypeName(ss_class)
{
	DefaultMethodContext = new tRisseVariant(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectBase::tRisseObjectBase(const tRisseString & prototype_name) :
				PrototypeName(prototype_name)
{
	DefaultMethodContext = new tRisseVariant(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectBase::tRetValue tRisseObjectBase::Read(const tRisseString & name, tRisseOperateFlags flags,
	tRisseVariant &result, const tRisseVariant &This)
{
	tMemberData * member = HashTable.Find(name);

	if(!member)
	{
		if(flags.Has(tRisseOperateFlags::ofInstanceMemberOnly))
			return rvMemberNotFound; // クラスやモジュールを探さない場合はここでかえる

		// モジュールを探す
		tRisseVariant modules;
		tRetValue modules_rv = Read(ss_modules, tRisseOperateFlags::ofInstanceMemberOnly, modules, This);
		if(modules_rv == rvNoError)
		{
			// モジュール配列がある
			// モジュールは、モジュール配列をたどることで検索を行う
			risse_offset length =
				static_cast<risse_offset>(
					(risse_int64)modules.GetPropertyDirect(ss_length, 0, modules));
			for(risse_offset i = 0; i < length; i++)
			{
				tRisseVariant index(static_cast<risse_int64>(i));
				tRisseVariant module = modules.IGet(index);

				// モジュールに対してメンバ取得を行う
				// モジュールはモジュールインスタンスのクラス (Moduleクラス以上)
				// を検索したりしないように、tRisseOperateFlags::ofInstanceMemberOnly をつけて
				// 検索を行う
				// また、
				// デフォルトのコンテキストを設定しないように(常にThisをコンテキストとして使うように)
				// tRisseOperateFlags::ofUseThisAsContext もつける
				tRetValue rv = module.OperateForMember(ocDGet, &result, name,
						flags|tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofUseThisAsContext,
							tRisseMethodArgument::Empty(),
							(DefaultMethodContext && !flags.Has(tRisseOperateFlags::ofUseThisAsContext)) ? *DefaultMethodContext:This);
				if(rv == rvNoError)
				{
					if(!flags.Has(tRisseOperateFlags::ofUseThisAsContext))
					{
						// コンテキストを設定する
						if(DefaultMethodContext) result.OverwriteContext(DefaultMethodContext);
					}

					return rvNoError;
				}

				if(rv != rvMemberNotFound) return rv; // MemberNotFound 以外はここでエラー
			}
		}
		else if(modules_rv == rvMemberNotFound)
		{
			// modules メンバが見つからないのはかまわない
		}
		else
		{
			// それいがいはまずい
			RaiseError(modules_rv, ss_modules);
		}

		// クラスを探す
		tRisseVariant Class;
		RaiseIfError(Read(PrototypeName, tRisseOperateFlags::ofInstanceMemberOnly, Class, This), PrototypeName);
		if(Class.IsNull()) return rvMemberNotFound; // クラスが null

		// クラスに対してメンバ取得を行う
		// デフォルトのコンテキストを設定しないように(常にThisをコンテキストとして使うように)
		// tRisseOperateFlags::ofUseThisAsContext もつける
		tRetValue rv = Class.OperateForMember(ocDGet, &result, name,
					flags|tRisseOperateFlags::ofUseThisAsContext,
					tRisseMethodArgument::Empty(),
					(DefaultMethodContext && !flags.Has(tRisseOperateFlags::ofUseThisAsContext)) ? *DefaultMethodContext:This);
		if(rv != rvNoError) return rv; // なにかエラーがおこあったらここで戻る

		if(!flags.Has(tRisseOperateFlags::ofUseThisAsContext))
		{
			// コンテキストを設定する
			if(DefaultMethodContext)
				result.OverwriteContext(DefaultMethodContext);
		}

		return rvNoError;
	}

	// プロパティアクセスの方法を決定する
	tRisseMemberAttribute::tPropertyControl member_prop_control = member->GetPropertyControl(flags);
	// プロパティアクセスの方法に従って情報を取得する
	switch(member_prop_control)
	{
	case tRisseMemberAttribute::pcNone: // あり得ない( GetPropertyControl() 中でASSERT)
		break;

	case tRisseMemberAttribute::pcVar: // 普通のメンバ
		// 単純に、結果に値をコピーする
		result = member->Value;
		break;

	case tRisseMemberAttribute::pcProperty: // プロパティアクセス
		// member->Value を引数なしで関数呼び出しし、その結果を得る
		tRetValue rv = member->Value.Operate(ocFuncCall, &result, tRisseString::GetEmptyString(),
					flags, tRisseMethodArgument::Empty(),
					(DefaultMethodContext && !flags.Has(tRisseOperateFlags::ofUseThisAsContext)) ? *DefaultMethodContext:This);
		if(rv != rvNoError && rv != rvMemberNotFound) return rv; // rvMemberNotFound以外のなにかエラーがおこあったらここで戻る
		break;
	}

	if(!flags.Has(tRisseOperateFlags::ofUseThisAsContext))
	{
		// コンテキストを設定する
		if(DefaultMethodContext)
			result.OverwriteContext(DefaultMethodContext);
	}

	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectBase::tRetValue tRisseObjectBase::Write(const tRisseString & name, tRisseOperateFlags flags,
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
		tRisseMemberAttribute::tOverrideControl member_ovl_control  = member->GetOverrideControl(flags);

		switch(member_ovl_control)
		{
		case tRisseMemberAttribute::ocNone: // あり得ない( GetPropertyControl() 中でASSERT)
			RISSE_ASSERT(member_ovl_control != tRisseMemberAttribute::ocNone);
			break;

		case tRisseMemberAttribute::ocVirtual: // ふつうのやつ
			// 下で処理
			break;

		case tRisseMemberAttribute::ocConst: // 定数
			return rvMemberIsReadOnly; // 書き込めません
		}

		switch(member_prop_control)
		{
		case tRisseMemberAttribute::pcNone: // あり得ない( GetPropertyControl() 中でASSERT)
			break;

		case tRisseMemberAttribute::pcVar: // 普通のメンバ
			if(flags.Has(tRisseOperateFlags::ofPropertyOrConstOnly))
				return rvMemberNotFound; // 見つからなかったというのと同じ扱い
			member->Value = value;
			return rvNoError;

		case tRisseMemberAttribute::pcProperty: // プロパティアクセス
			return member->Value.Operate(ocDSet, NULL, tRisseString::GetEmptyString(),
						flags, tRisseMethodArgument::New(value),
						(DefaultMethodContext && !flags.Has(tRisseOperateFlags::ofUseThisAsContext)) ?
							*DefaultMethodContext:This);
		}
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
			if(attrib.GetOverride() == tRisseMemberAttribute::ocNone)
				attrib.SetOverride(tRisseMemberAttribute::ocVirtual); // デフォルトはocVirtual
			HashTable.Add(name, tMemberData(tMemberData(value, attrib)));
			return rvNoError;
		}
		return rvMemberNotFound; // そうでない場合はメンバは見つからなかったことにする
	}

	// クラスやモジュールを見に行くが、クラスやモジュールにプロパティとして動作する
	// メンバがあった場合のみに、そのプロパティを起動する。
	// または、クラスを探しに行ったときに const があった場合は
	// それに書き込みを試みる(当然エラーになるが、意図した動作である)

	tRetValue result = rvMemberNotFound;

	// モジュールを探す
	tRisseVariant modules;
	if(Read(ss_modules, tRisseOperateFlags::ofInstanceMemberOnly, modules, This) == rvNoError)
	{
		// モジュール配列がある
		// モジュールは、モジュール配列をたどることで検索を行う
		risse_offset length =
			static_cast<risse_offset>(
				(risse_int64)modules.GetPropertyDirect(ss_length, 0, modules));
		for(risse_offset i = 0; i < length; i++)
		{
			tRisseVariant index(static_cast<risse_int64>(i));
			tRisseVariant module = modules.IGet(index);

			// モジュールに対してメンバ設定を行う
			// モジュールはモジュールインスタンスのクラス (Moduleクラス以上)
			// を検索したりしないように、tRisseOperateFlags::ofInstanceMemberOnly をつけて
			// 検索を行う
			// デフォルトのコンテキストを設定しないように(常にThisをコンテキストとして使うように)
			// tRisseOperateFlags::ofUseThisAsContext もつける
			result = module.OperateForMember(ocDSet, NULL, name,
				flags|tRisseOperateFlags::ofPropertyOrConstOnly|tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofUseThisAsContext,
				tRisseMethodArgument::New(value),
				(DefaultMethodContext && !flags.Has(tRisseOperateFlags::ofUseThisAsContext)) ? *DefaultMethodContext:This);
			if(result != rvMemberNotFound) return result; // 「メンバが見つからない」以外は戻る
		}
	}

	// クラスを探す
	tRisseVariant Class;
	RaiseIfError(Read(PrototypeName, tRisseOperateFlags::ofInstanceMemberOnly, Class, This), PrototypeName);
	if(!Class.IsNull())
	{
		// クラスを特定できた場合
		// クラスに対してメンバ設定を行う
		// デフォルトのコンテキストを設定しないように(常にThisをコンテキストとして使うように)
		// tRisseOperateFlags::ofUseThisAsContext もつける
		result = Class.OperateForMember(ocDSet, NULL, name,
							flags|tRisseOperateFlags::ofPropertyOrConstOnly|tRisseOperateFlags::ofUseThisAsContext,
							tRisseMethodArgument::New(value),
							(DefaultMethodContext && !flags.Has(tRisseOperateFlags::ofUseThisAsContext)) ? *DefaultMethodContext:This);
		// ちなみに見つかったのが定数で、書き込みに失敗した場合は
		// 例外が飛ぶので OperateForMember は戻ってこない。
		if(result != rvMemberNotFound) return result; // 「メンバが見つからない」以外は戻る
	}

	if(result == rvMemberNotFound && flags.Has(tRisseOperateFlags::ofPropertyOrConstOnly))
	{
		// 親クラスを見に行ったがメンバがなかった、あるいはそもそも親クラスが見つからなかった。
		// PropertyOrConstOnly が指定されているのでこの
		// インスタンスにメンバを作成するわけにもいかない。
		return rvMemberNotFound; // 失敗として返す
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
		if(attrib.GetOverride() == tRisseMemberAttribute::ocNone)
			attrib.SetOverride(tRisseMemberAttribute::ocVirtual); // デフォルトはocVirtual
		HashTable.Add(name, tMemberData(tMemberData(value, attrib)));
		return rvNoError;
	}
	else
	{
		member = HashTable.Find(name);

		if(!member) return rvMemberNotFound; // 見つからなかった

		// メンバが見つかったのでこれに上書きをする
		// そのまえに属性チェック
		tRisseMemberAttribute::tPropertyControl member_prop_control = member->GetPropertyControl(flags);
		tRisseMemberAttribute::tOverrideControl member_ovl_control  = member->GetOverrideControl(flags);

		switch(member_ovl_control)
		{
		case tRisseMemberAttribute::ocNone: // あり得ない( GetPropertyControl() 中でASSERT)
			RISSE_ASSERT(member_ovl_control != tRisseMemberAttribute::ocNone);
			break;

		case tRisseMemberAttribute::ocVirtual: // ふつうのやつ
			// 下で処理
			break;

		case tRisseMemberAttribute::ocConst: // 定数
			return rvMemberIsReadOnly; // 書き込めません
		}

		switch(member_prop_control)
		{
		case tRisseMemberAttribute::pcNone: // あり得ない( GetPropertyControl() 中でASSERT)
			break;

		case tRisseMemberAttribute::pcVar: // 普通のメンバ
			member->Value = value;
			return rvNoError;

		case tRisseMemberAttribute::pcProperty: // プロパティアクセス
			return member->Value.Operate(ocDSet, NULL, tRisseString::GetEmptyString(),
						flags, tRisseMethodArgument::New(value),
						(DefaultMethodContext && !flags.Has(tRisseOperateFlags::ofUseThisAsContext)) ?
							*DefaultMethodContext:This);
		}
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectBase::tRetValue tRisseObjectBase::FuncCall(
		tRisseVariantBlock * ret,
		const tRisseString & name, risse_uint32 flags,
		const tRisseMethodArgument & args,
		const tRisseVariant & This)
{
	if(!name.IsEmpty())
	{

		// メンバを読み出す
		tRisseVariant function_object;
		tRetValue rv = Read(name, flags, function_object, This);
		if(rv != rvNoError) return rv;

		// メンバに対して関数呼び出しを実行する
		function_object.FuncCall(ret, flags, args, This);
	}
	else
	{
		// このオブジェクトに対するメソッド呼び出し
		// 普通、メソッドとしては動作しない
		tRisseUnsupportedOperationExceptionClass::ThrowCannotCallNonFunctionObjectException();
	}
	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectBase::tRetValue tRisseObjectBase::New(
		tRisseVariantBlock * ret,
		const tRisseString & name, risse_uint32 flags,
		const tRisseMethodArgument & args,
		const tRisseVariant & This)
{
	if(!name.IsEmpty())
	{
		// メンバを読み出す
		tRisseVariant class_object;
		tRetValue rv = Read(name, flags, class_object, This);
		if(rv != rvNoError) return rv;

		// メンバに対しNewを実行する
		tRisseVariant result = class_object.New(flags, args);
		if(ret) *ret = result;

		return rvNoError;
	}
	else
	{
		// このオブジェクトに対して Newを実行する
		return FuncCall(ret, mnNew, flags, args, This);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectBase::tRetValue tRisseObjectBase::SetAttribute(
		const tRisseString & name, tRisseOperateFlags flags, const tRisseVariant & This)
{
	tMemberData * member = HashTable.Find(name);

	if(!member)
	{
		if(flags.Has(tRisseOperateFlags::ofInstanceMemberOnly))
			return rvMemberNotFound; // クラスを探さない場合はここでかえる

		// TODO: モジュールに対する設定は？

		// クラスを探す
		tRisseVariant Class;
		RaiseIfError(Read(PrototypeName, tRisseOperateFlags::ofInstanceMemberOnly, Class, This), PrototypeName);
		if(Class.IsNull()) return rvMemberNotFound; // クラスが null

		// クラスに対して属性設定を行う
		tRetValue rv = Class.OperateForMember(ocDSetAttrib, NULL, name, flags,
					tRisseMethodArgument::Empty(), This);
		return rv;
	}

	// 属性を設定する
	member->Attribute.Overwrite(flags);

	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseObjectBase::InstanceOf(
		const tRisseVariant & RefClass, risse_uint32 flags,
		const tRisseVariant & This
		)
{
	if(RefClass.GetType() != tRisseVariant::vtObject) return false;// vt は vtObjectでなければならない

	if(this == RefClass.GetObjectInterface()) return true; // これそのものだ

	if(flags & tRisseOperateFlags::ofInstanceMemberOnly)
		return false; // クラスやモジュールを探さない場合はここでかえる

	// モジュールを探す
	tRisseVariant modules;
	tRetValue modules_rv = Read(ss_modules, tRisseOperateFlags::ofInstanceMemberOnly, modules, This);
	if(modules_rv == rvNoError)
	{
		// モジュール配列がある
		// モジュールは、モジュール配列をたどることで検索を行う
		risse_offset length =
			static_cast<risse_offset>(
				(risse_int64)modules.GetPropertyDirect(ss_length, 0, modules));
		for(risse_offset i = 0; i < length; i++)
		{
			tRisseVariant index(static_cast<risse_int64>(i));
			tRisseVariant module = modules.IGet(index);

			if(module.ObjectInterfaceMatch(RefClass)) return true; // マッチした
		}
	}
	else if(modules_rv == rvMemberNotFound)
	{
		// modules メンバが見つからないのはかまわない
	}
	else
	{
		// それいがいはまずい
		RaiseError(modules_rv, ss_modules);
	}

	// スーパークラスをゲット
	tRisseVariant SuperClass;
	RaiseIfError(Read(PrototypeName, tRisseOperateFlags::ofInstanceMemberOnly, SuperClass, This), PrototypeName);
	if(SuperClass.IsNull()) return false; // クラスが null

	if(SuperClass.ObjectInterfaceMatch(RefClass)) return true; // マッチした

	// クラスに対して再帰
	tRisseVariant result;
	SuperClass.GetObjectInterface()->Do(ocInstanceOf, &result, tRisseString::GetEmptyString(),
		flags|tRisseOperateFlags::ofUseThisAsContext,
		tRisseMethodArgument::New(RefClass),
		(DefaultMethodContext && (flags & tRisseOperateFlags::ofUseThisAsContext)) ? *DefaultMethodContext:This);

	return result.operator bool();
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
		return Read(name, flags, *result, This);

	case ocDSet:
		// property set
		// TODO: このオブジェクトそのもに対する操作への対応
		args.ExpectArgumentCount(1);
		return Write(name, flags, args[0], This);

	case ocFuncCall:
		// function call
		return FuncCall(result, name, flags, args, This);

	case ocNew:
		// create new instance
		return New(result, name, flags, args, This);

	case ocDSetAttrib:
		// set member attrubute
		return SetAttribute(name, flags);

	case ocInstanceOf:
		// TODO: このオブジェクトのメンバに対する操作への対応
		{
			RISSE_ASSERT(name.IsEmpty());
			args.ExpectArgumentCount(1);
			bool res = InstanceOf(args[0], flags, This);
			if(result) *result = res;
			return rvNoError;
		}

	case ocGetDefaultContext:
		// get default method context
		*result = *DefaultMethodContext;
		return rvNoError;

	case ocSetDefaultContext:
		// TODO: この機能要らないかもしれない
		// set default method context
		args.ExpectArgumentCount(1);
		// dynamic のチェック
		if(args[0].IsDynamicContext())
			DefaultMethodContext = NULL; // TODO: この場合も null でよかったっけ？
		else
			DefaultMethodContext = new tRisseVariant(args[0]);
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
	RaiseIfError(Write(name,
		tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcVar)) |
		tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::ocVirtual)) |
							access_flags, value, tRisseVariant(this)), name);
	if(attrib.HasAny())
		SetAttribute(name, access_flags|attrib);
}
//---------------------------------------------------------------------------

} /* namespace Risse */

