/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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
#include "risseScriptEngine.h"

/*
	ここではオブジェクトの実装に必要な基底の機能を実装する
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(45114,31718,49668,18467,56195,41722,1990,5427);

//---------------------------------------------------------------------------
tObjectBase::tObjectBase() : PrototypeName(ss_class), MembersName(tString::GetEmptyString())
{
	DefaultMethodContext = new tVariant(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectBase::tObjectBase(const tString & prototype_name, const tString & members_name) :
				PrototypeName(prototype_name), MembersName(members_name)
{
	DefaultMethodContext = new tVariant(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectBase::Enumurate(tEnumMemberCallback * callback)
{
	volatile tSynchronizer sync(this); // sync

	tMemberHashTable::tIterator iterator(HashTable);
	while(!iterator.End())
	{
		if(!callback->OnEnum(iterator.GetKey(), iterator.GetValue())) return;
		++iterator;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectBase::tRetValue tObjectBase::Read(const tString & name, tOperateFlags flags,
	tVariant &result, const tVariant &This) const
{
	volatile tSynchronizer sync(this); // sync

	if(flags.Has(tOperateFlags::ofUseClassMembersRule) && !MembersName.IsEmpty())
	{
		// ofUseClassMembersRule が指定されているのでそちらを見に行く
		tVariant members;
		tRetValue members_rv = Read(ss_members, tOperateFlags::ofInstanceMemberOnly, members, This);
		if(members_rv == rvNoError)
		{
			// 要求を members にリダイレクトする
			return members.OperateForMember(GetRTTI()->GetScriptEngine(), ocDGet, &result, name,
				flags, tMethodArgument::Empty(), This);
		}
	}

	tMemberData * member = HashTable.Find(name);

	if(!member)
	{
		if(flags.Has(tOperateFlags::ofInstanceMemberOnly))
			return rvMemberNotFound; // クラスやモジュールを探さない場合はここでかえる

		// モジュールを探す
		// modules 配列の場合は members の中に modules 配列がある場合があるので
		// ofUseClassMembersRule フラグをつける
		tVariant modules;
		tRetValue modules_rv = Read(ss_modules, tOperateFlags::ofInstanceMemberOnly|tOperateFlags::ofUseClassMembersRule, modules, This);
		if(modules_rv == rvNoError)
		{
			// モジュール配列がある
			// モジュールは、モジュール配列をたどることで検索を行う
			risse_offset length =
				static_cast<risse_offset>(
					(risse_int64)modules.GetPropertyDirect(GetRTTI()->GetScriptEngine(), ss_length, 0, modules));
			for(risse_offset i = 0; i < length; i++)
			{
				tVariant index(static_cast<risse_int64>(i));
				tVariant module = modules.IGet(index);

				// モジュールに対してメンバ取得を行う
				// モジュールはモジュールインスタンスのクラス (Moduleクラス以上)
				// を検索したりしないように、tOperateFlags::ofInstanceMemberOnly をつけて
				// 検索を行う
				// また、
				// デフォルトのコンテキストを設定しないように(常にThisをコンテキストとして使うように)
				// また members の中身を見るように
				// tOperateFlags::ofUseClassMembersRule もつける
				tRetValue rv = module.OperateForMember(GetRTTI()->GetScriptEngine(), ocDGet, &result, name,
						flags|tOperateFlags::ofInstanceMemberOnly|tOperateFlags::ofUseClassMembersRule,
							tMethodArgument::Empty(),
							(DefaultMethodContext && !flags.Has(tOperateFlags::ofUseClassMembersRule)) ? *DefaultMethodContext:This);
				if(rv == rvNoError)
				{
					if(!flags.Has(tOperateFlags::ofUseClassMembersRule))
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
		tVariant Class;
		RaiseIfError(Read(PrototypeName, tOperateFlags::ofInstanceMemberOnly, Class, This), PrototypeName);
		if(Class.IsNull()) return rvMemberNotFound; // クラスが null

		// クラスに対してメンバ取得を行う
		// デフォルトのコンテキストを設定しないように(常にThisをコンテキストとして使うように)
		// またクラスの members の中身を見るように
		// tOperateFlags::ofUseClassMembersRule もつける
		// TODO : もっと効率的な実装
		tRetValue rv = Class.OperateForMember(GetRTTI()->GetScriptEngine(), ocDGet, &result, name,
					flags|tOperateFlags::ofUseClassMembersRule,
					tMethodArgument::Empty(),
					(DefaultMethodContext && !flags.Has(tOperateFlags::ofUseClassMembersRule)) ? *DefaultMethodContext:This);
		if(rv != rvNoError) return rv; // なにかエラーがおこったらここで戻る

		if(!flags.Has(tOperateFlags::ofUseClassMembersRule))
		{
			// コンテキストを設定する
			if(DefaultMethodContext)
				result.OverwriteContext(DefaultMethodContext);
		}

		return rvNoError;
	}

	// プロパティアクセスの方法を決定する
	tMemberAttribute attrib = member->Attribute;
	attrib.Overwrite(flags);
	RISSE_ASSERT(attrib.GetMutability() != tMemberAttribute::mcNone);
	RISSE_ASSERT(attrib.GetOverride() != tMemberAttribute::ocNone);
	RISSE_ASSERT(attrib.GetProperty() != tMemberAttribute::pcNone);
	RISSE_ASSERT(attrib.GetAccess() != tMemberAttribute::acNone);

	// プロパティアクセスの方法に従って情報を取得する
	switch(attrib.GetProperty())
	{
	case tMemberAttribute::pcNone: // あり得ない
		break;

	case tMemberAttribute::pcField: // 普通のメンバ
		// 単純に、結果に値をコピーする
		result = member->Value;
		break;

	case tMemberAttribute::pcProperty: // プロパティアクセス
		// member->Value を引数なしで関数呼び出しし、その結果を得る
		tRetValue rv = member->Value.Operate(GetRTTI()->GetScriptEngine(),
			ocFuncCall, &result, tString::GetEmptyString(),
			flags & ~tOperateFlags::ofUseClassMembersRule, tMethodArgument::Empty(),
			(DefaultMethodContext && !flags.Has(tOperateFlags::ofUseClassMembersRule)) ? *DefaultMethodContext:This);
		if(rv != rvNoError && rv != rvMemberNotFound) return rv; // rvMemberNotFound以外のなにかエラーがおこったらここで戻る
		break;
	}

	if(!flags.Has(tOperateFlags::ofUseClassMembersRule))
	{
		// コンテキストを設定する
		if(DefaultMethodContext)
			result.OverwriteContext(DefaultMethodContext);
	}

	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectBase::tRetValue tObjectBase::Write(const tString & name, tOperateFlags flags,
	const tVariant &value, const tVariant &This)
{
	volatile tSynchronizer sync(this); // sync

	if(flags.Has(tOperateFlags::ofUseClassMembersRule) && !MembersName.IsEmpty())
	{
		// ofUseClassMembersRule が指定されているのでそちらを見に行く
		tVariant members;
		tRetValue members_rv = Read(ss_members, tOperateFlags::ofInstanceMemberOnly, members, This);
		if(members_rv == rvNoError)
		{
			// 要求を members にリダイレクトする
			return members.OperateForMember(GetRTTI()->GetScriptEngine(), ocDSet, NULL, name,
				flags, tMethodArgument::New(value), This);
		}
	}

	tMemberData * member;

	// メンバに書き込む
	member = HashTable.Find(name);

	if(member)
	{
		// メンバが見つかったのでこれに上書きをする
		// そのまえに属性チェック
		tMemberAttribute attrib = member->Attribute;
		attrib.Overwrite(flags); // flags の指定を優先させる
		RISSE_ASSERT(attrib.GetMutability() != tMemberAttribute::mcNone);
		RISSE_ASSERT(attrib.GetOverride() != tMemberAttribute::ocNone);
		RISSE_ASSERT(attrib.GetProperty() != tMemberAttribute::pcNone);
		RISSE_ASSERT(attrib.GetAccess() != tMemberAttribute::acNone);

		if(flags.Has(tOperateFlags::ofFinalOnly))
		{
			switch(attrib.GetOverride())
			{
			case tMemberAttribute::ocNone: // あり得ない(上でassert)
				break;

			case tMemberAttribute::ocVirtual: // ふつうのやつ
				break;

			case tMemberAttribute::ocFinal: // final メンバ
				return rvMemberIsFinal;
			}
			return rvMemberNotFound; // 見つからなかったというのと同じ扱い
		}

		switch(attrib.GetMutability())
		{
		case tMemberAttribute::mcNone: // あり得ない
			RISSE_ASSERT(attrib.GetMutability() != tMemberAttribute::mcNone);
			break;

		case tMemberAttribute::mcVar: // ふつうのやつ
			// 下で処理
			break;

		case tMemberAttribute::mcConst: // 定数
			// プロパティの場合は書き込みが成功する可能性があるので注意。
			// もっとも、もちろん、プロパティの場合はメンバそのものに書き込みが
			// 行われるわけではなく、プロパティハンドラが起動される。
			if(attrib.GetProperty() != tMemberAttribute::pcProperty)
				return rvMemberIsReadOnly; // 書き込めません
			break; // 下で処理
		}

		switch(attrib.GetProperty())
		{
		case tMemberAttribute::pcNone: // あり得ない
			break;

		case tMemberAttribute::pcField: // 普通のメンバ
			member->Value = value;
			return rvNoError;

		case tMemberAttribute::pcProperty: // プロパティアクセス
			return member->Value.Operate(GetRTTI()->GetScriptEngine(), ocDSet, NULL,
				tString::GetEmptyString(),
				flags & ~tOperateFlags::ofUseClassMembersRule, tMethodArgument::New(value),
				(DefaultMethodContext && !flags.Has(tOperateFlags::ofUseClassMembersRule)) ?
						*DefaultMethodContext:This);
		}
	}

	// この時点でメンバはインスタンスには見つかっていない。
	if(flags.Has(tOperateFlags::ofInstanceMemberOnly))
	{
		// クラスを探さない場合は
		if(flags.Has(tOperateFlags::ofMemberEnsure))
		{
			// 新規作成フラグがある場合はメンバを新規作成する
			tMemberAttribute attrib = tMemberAttribute::GetDefault();
			attrib.Overwrite(flags);
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

	// モジュールやクラスのメンバを検索するときの基本的なフラグ
	// デフォルトのコンテキストを設定しないように(常にThisをコンテキストとして使うように)
	// またクラスの members の中をみるように
	// tOperateFlags::ofUseClassMembersRule もつける
	// ofFinalOnly をつけるのは flags に ofMemberEnsure がくっついているときだけ
	tOperateFlags class_search_base_flags =
			(flags & ~(tOperateFlags::ofFinalOnly)) |
			(
				flags.Has(tOperateFlags::ofMemberEnsure) ?
						(tOperateFlags::ofFinalOnly|tOperateFlags::ofUseClassMembersRule) :
						(                           tOperateFlags::ofUseClassMembersRule) 
			);

	// モジュールを探す
	// modules 配列の場合は members の中に modules 配列がある場合があるので
	// ofUseClassMembersRule フラグをつける
	tVariant modules;
	if(Read(ss_modules, tOperateFlags::ofInstanceMemberOnly|tOperateFlags::ofUseClassMembersRule, modules, This) == rvNoError)
	{
		// モジュール配列がある
		// モジュールは、モジュール配列をたどることで検索を行う
		risse_offset length =
			static_cast<risse_offset>(
				(risse_int64)modules.GetPropertyDirect(GetRTTI()->GetScriptEngine(), ss_length, 0, modules));
		for(risse_offset i = 0; i < length; i++)
		{
			tVariant index(static_cast<risse_int64>(i));
			tVariant module = modules.IGet(index);

			// モジュールに対してメンバ設定を行う
			// モジュールはモジュールインスタンスのクラス (Moduleクラス以上)
			// を検索したりしないように、tOperateFlags::ofInstanceMemberOnly をつけて
			// 検索を行う
			result = module.OperateForMember(GetRTTI()->GetScriptEngine(), ocDSet, NULL, name,
				flags|
					class_search_base_flags|
					tOperateFlags::ofInstanceMemberOnly ,
				tMethodArgument::New(value),
				(DefaultMethodContext && !flags.Has(tOperateFlags::ofUseClassMembersRule)) ? *DefaultMethodContext:This);
			if(result != rvMemberNotFound) return result; // 「メンバが見つからない」以外は戻る
		}
	}

	// クラスを探す
	tVariant Class;
	RaiseIfError(Read(PrototypeName, tOperateFlags::ofInstanceMemberOnly, Class, This), PrototypeName);
	if(!Class.IsNull())
	{
		// クラスを特定できた場合
		// クラスに対してメンバ設定を行う
		result = Class.OperateForMember(GetRTTI()->GetScriptEngine(), ocDSet, NULL, name,
							flags|
							class_search_base_flags ,
							tMethodArgument::New(value),
							(DefaultMethodContext && !flags.Has(tOperateFlags::ofUseClassMembersRule)) ? *DefaultMethodContext:This);
		// ちなみに見つかったのが定数で、書き込みに失敗した場合は
		// 例外が飛ぶので OperateForMember は戻ってこない。
		if(result != rvMemberNotFound) return result; // 「メンバが見つからない」以外は戻る
	}

	if(result == rvMemberNotFound && flags.Has(tOperateFlags::ofFinalOnly))
	{
		// 親クラスを見に行ったがメンバがなかった、あるいはそもそも親クラスが見つからなかった。
		// ofFinalOnly が指定されているのでこの
		// インスタンスにメンバを作成するわけにもいかない。
		return rvMemberNotFound; // メンバが見つからない
	}

	if(flags.Has(tOperateFlags::ofMemberEnsure))
	{
		// そのほかの場合、つまりクラスを特定できない場合や、
		// クラスにメンバが無かった場合、
		// クラスにメンバがあったがプロパティとして起動できなかった
		// 場合はこのインスタンスにメンバを作成する。
		tMemberAttribute attrib;
		attrib = tMemberAttribute::GetDefault();
		attrib.Overwrite(flags);
		HashTable.Add(name, tMemberData(tMemberData(value, attrib)));
		return rvNoError;
	}
	else
	{
		return rvMemberNotFound;
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectBase::tRetValue tObjectBase::Delete(const tString & name, tOperateFlags flags)
{
	volatile tSynchronizer sync(this); // sync

	if(flags.Has(tOperateFlags::ofUseClassMembersRule) && !MembersName.IsEmpty())
	{
		// ofUseClassMembersRule が指定されているのでそちらを見に行く
		tVariant members;
		tRetValue members_rv = Read(ss_members, tOperateFlags::ofInstanceMemberOnly, members, *tVariant::GetDynamicContext());
		if(members_rv == rvNoError)
		{
			// 要求を members にリダイレクトする
			return members.OperateForMember(GetRTTI()->GetScriptEngine(), ocDDelete, NULL, name,
				flags, tMethodArgument::Empty(), *tVariant::GetDynamicContext());
		}
	}

	if(!HashTable.Delete(name))
	{
		if(flags.Has(tOperateFlags::ofInstanceMemberOnly))
			return rvMemberNotFound; // クラスを探さない場合はここでかえる

		// TODO: モジュールに対する削除は？

		// クラスを探す
		tVariant Class;
		RaiseIfError(Read(PrototypeName, tOperateFlags::ofInstanceMemberOnly,
			Class, *tVariant::GetDynamicContext()), PrototypeName);
		if(Class.IsNull()) return rvMemberNotFound; // クラスが null

		// クラスに対して削除を行う
		tRetValue rv = Class.OperateForMember(GetRTTI()->GetScriptEngine(), ocDDelete, NULL, name, flags,
					tMethodArgument::Empty(), *tVariant::GetDynamicContext());
		return rv;
	}

	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectBase::tRetValue tObjectBase::FuncCall(
		tVariant * ret,
		const tString & name, risse_uint32 flags,
		const tMethodArgument & args,
		const tVariant & This)
{
	if(!name.IsEmpty())
	{

		// メンバを読み出す
		tVariant function_object;
		tRetValue rv = Read(name, flags, function_object, This);
		if(rv != rvNoError) return rv;

		// メンバに対して関数呼び出しを実行する
		function_object.FuncCall(GetRTTI()->GetScriptEngine(), ret, flags & ~tOperateFlags::ofUseClassMembersRule, args, This);
	}
	else
	{
		// このオブジェクトに対して FuncCall を実行する
		return FuncCall(ret, mnFuncCall, flags & ~tOperateFlags::ofUseClassMembersRule, args, This);
	}
	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectBase::tRetValue tObjectBase::New(
		tVariant * ret,
		const tString & name, risse_uint32 flags,
		const tMethodArgument & args,
		const tVariant & This)
{
	if(!name.IsEmpty())
	{
		// メンバを読み出す
		tVariant class_object;
		tRetValue rv = Read(name, flags, class_object, This);
		if(rv != rvNoError) return rv;

		// メンバに対しNewを実行する
		tVariant result = class_object.New(flags & ~tOperateFlags::ofUseClassMembersRule, args);
		if(ret) *ret = result;

		return rvNoError;
	}
	else
	{
		// このオブジェクトに対して Newを実行する
		return FuncCall(ret, mnNew, flags & ~tOperateFlags::ofUseClassMembersRule, args, This);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectBase::tRetValue tObjectBase::SetAttribute(
		const tString & name, tOperateFlags flags, const tVariant & This)
{
	volatile tSynchronizer sync(this); // sync

	if(flags.Has(tOperateFlags::ofUseClassMembersRule) && !MembersName.IsEmpty())
	{
		// ofUseClassMembersRule が指定されているのでそちらを見に行く
		tVariant members;
		tRetValue members_rv = Read(ss_members, tOperateFlags::ofInstanceMemberOnly, members, This);
		if(members_rv == rvNoError)
		{
			// 要求を members にリダイレクトする
			return members.OperateForMember(GetRTTI()->GetScriptEngine(), ocDSetAttrib, NULL, name,
				flags, tMethodArgument::Empty(), This);
		}
	}

	tMemberData * member = HashTable.Find(name);

	if(!member)
	{
		if(flags.Has(tOperateFlags::ofInstanceMemberOnly))
			return rvMemberNotFound; // クラスを探さない場合はここでかえる

		// TODO: モジュールに対する設定は？

		// クラスを探す
		tVariant Class;
		RaiseIfError(Read(PrototypeName, tOperateFlags::ofInstanceMemberOnly, Class, This), PrototypeName);
		if(Class.IsNull()) return rvMemberNotFound; // クラスが null

		// クラスに対して属性設定を行う
		tRetValue rv = Class.OperateForMember(GetRTTI()->GetScriptEngine(),
			ocDSetAttrib, NULL, name, flags, tMethodArgument::Empty(), This);
		return rv;
	}

	// 属性を設定する
	member->Attribute.Overwrite(flags);

	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tObjectBase::InstanceOf(
		const tVariant & RefClass, risse_uint32 flags,
		const tVariant & This
		)
{
	if(RefClass.GetType() != tVariant::vtObject) return false;// vt は vtObjectでなければならない

	if(this == RefClass.GetObjectInterface()) return true; // これそのものだ

	if(flags & tOperateFlags::ofInstanceMemberOnly)
		return false; // クラスやモジュールを探さない場合はここでかえる

	// モジュールを探す
	// modules 配列の場合は members の中に modules 配列がある場合があるので
	// ofUseClassMembersRule フラグをつける
	tVariant modules;
	tRetValue modules_rv = Read(ss_modules, tOperateFlags::ofInstanceMemberOnly|tOperateFlags::ofUseClassMembersRule, modules, This);
	if(modules_rv == rvNoError)
	{
		// モジュール配列がある
		// モジュールは、モジュール配列をたどることで検索を行う
		risse_offset length =
			static_cast<risse_offset>(
				(risse_int64)modules.GetPropertyDirect(GetRTTI()->GetScriptEngine(),
						ss_length, 0, modules));
		for(risse_offset i = 0; i < length; i++)
		{
			tVariant index(static_cast<risse_int64>(i));
			tVariant module = modules.IGet(index);

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
	tVariant SuperClass;
	RaiseIfError(Read(PrototypeName, tOperateFlags::ofInstanceMemberOnly, SuperClass, This), PrototypeName);
	if(SuperClass.IsNull()) return false; // クラスが null

	if(SuperClass.ObjectInterfaceMatch(RefClass)) return true; // マッチした

	// クラスに対して再帰
	tVariant result;
	SuperClass.GetObjectInterface()->Do(ocInstanceOf, &result, tString::GetEmptyString(),
		flags|tOperateFlags::ofUseClassMembersRule,
		tMethodArgument::New(RefClass),
		(DefaultMethodContext && (flags & tOperateFlags::ofUseClassMembersRule)) ? *DefaultMethodContext:This);

	return result.operator bool();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectBase::tRetValue tObjectBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
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

	case ocDDelete:
		// delete member
		// TODO: このオブジェクトそのもに対する操作への対応
		if(result) *result = tVariant(true); // 戻り値は true 固定
		return Delete(name, flags);

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
			DefaultMethodContext = new tVariant(args[0]);
		return rvNoError;

	default:
		; // TODO: unhandled operation code support
	}
	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectBase::RegisterNormalMember(const tString & name,
			const tVariant & value, tMemberAttribute attrib, bool ismembers)
{
	tOperateFlags access_flags =
		tOperateFlags::ofMemberEnsure|tOperateFlags::ofInstanceMemberOnly;
	if(ismembers) access_flags = access_flags | tOperateFlags::ofUseClassMembersRule;
	RaiseIfError(Write(name,
		tOperateFlags(tMemberAttribute::GetDefault()) |
							access_flags, value, tVariant(this)), name);
	if(attrib.HasAny())
		SetAttribute(name, access_flags|attrib);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectBase::ReadMember(const tString & name,
					risse_uint32 flags) const
{
	tVariant result;
	RaiseIfError(Read(name, flags, result, tVariant(const_cast<tObjectBase *>(this))), name);
	return result;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectBase::GetPropertyDirect(const tString & name, risse_uint32 flags)
{
	tVariant value;
	Do(ocDGet, &value, name, flags, tMethodArgument::Empty());
	return value;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectBase::SetPropertyDirect(const tString & name, risse_uint32 flags, const tVariant &val)
{
	Do(ocDSet, NULL, name, flags, tMethodArgument::New(val));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectBase::Invoke(const tString & membername)
{
	tVariant value;
	Do(ocFuncCall, &value, membername, 0, tMethodArgument::Empty());
	return value;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectBase::Invoke(
	const tString & membername,
	const tVariant & arg1)
{
	tVariant value;
	Do(ocFuncCall, &value, membername, 0, tMethodArgument::New(arg1));
	return value;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectBase::Invoke(
	const tString & membername,
	const tVariant & arg1,
	const tVariant & arg2)
{
	tVariant value;
	Do(ocFuncCall, &value, membername, 0, tMethodArgument::New(arg1, arg2));
	return value;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectBase::Invoke(
	const tString & membername,
	const tVariant & arg1,
	const tVariant & arg2,
	const tVariant & arg3)
{
	tVariant value;
	Do(ocFuncCall, &value, membername, 0, tMethodArgument::New(arg1, arg2, arg3));
	return value;
}
//---------------------------------------------------------------------------

} /* namespace Risse */

