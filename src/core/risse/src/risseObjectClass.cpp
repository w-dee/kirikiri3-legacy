/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Object" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseObjectClass.h"
#include "risseStaticStrings.h"
#include "risseExceptionClass.h"
#include "risseScriptEngine.h"
#include "risseBindingInfo.h"
#include "risseArrayClass.h"
#include "rissePackage.h"

/*
	Risseスクリプトから見える"Object" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(41134,45186,9497,17812,19604,2796,36426,21671);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectClass::tObjectClass(tScriptEngine * engine) :
	tClassBase(engine)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_construct, &tObjectClass::construct);
	BindFunction(this, ss_initialize, &tObjectClass::initialize);
	BindFunction(this, ss_import, &tObjectClass::import);
	BindFunction(this, mnDiscEqual, &tObjectClass::DiscEqual);
	BindFunction(this, ss_identify, &tObjectClass::identify);
	BindProperty(this, ss_hint, &tObjectClass::get_hint, &tObjectClass::set_hint);
	BindProperty(this, ss_hash, &tObjectClass::get_hash);
	BindFunction(this, ss_isA, &tObjectClass::isA, 
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_getInstanceMember, &tObjectClass::getInstanceMember);
	BindFunction(this, ss_setInstanceMember, &tObjectClass::setInstanceMember);
	BindFunction(this, ss_getPublicMembers, &tObjectClass::getPublicMembers);
	BindFunction(this, ss_toException, &tObjectClass::toException);
	BindFunction(this, ss_p, &tObjectClass::p);
	BindFunction(this, mnBoolean, &tObjectClass::toBoolean);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::initialize()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::import(const tVariant & global,
	const tVariant & packages, const tNativeCallInfo & info)
{
	if(info.args.HasArgument(2))
	{
		// 引数が3個あるぜ
		info.engine->GetPackageManager()->
			DoImport(global, const_cast<tVariant&>(info.This), packages, info.args[2]);
	}
	else
	{
		// 引数はインポート先パッケージのみ
		info.engine->GetPackageManager()->
			DoImport(global, const_cast<tVariant&>(info.This), packages);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tObjectClass::DiscEqual(const tNativeCallInfo & info, const tVariant &rhs)
{
	// === 演算子
	if(info.This.GetType() == tVariant::vtObject)
	{
		if(rhs.GetType() != tVariant::vtObject) return false;
		return info.This.GetObjectInterface() == rhs.GetObjectInterface();
	}
	// プリミティブ型
	// TODO: 各サブクラスでオーバーライドされているかのように実装すべき？
	return info.This.DiscEqual(rhs);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tObjectClass::identify(const tNativeCallInfo & info, const tVariant &rhs)
{
	return info.This.StrictEqual(rhs); // デフォルトは StrictEqual と一緒
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::get_hint(const tNativePropGetInfo & info)
{
	if(info.This.GetType() == tVariant::vtObject)
	{
		// デフォルトでは hash プロパティを読み出す
		if(info.result)
			*info.result = info.This.GetPropertyDirect_Object(ss_hash);
		else
			info.This.GetPropertyDirect_Object(ss_hash); // dummy read
		return;
	}

	// プリミティブ型
	// TODO: 各サブクラスでオーバーライドされているかのように実装すべき？
	if(info.result)
		*info.result = static_cast<risse_int64>(info.This.GetHint());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::set_hint(const tNativePropSetInfo & info)
{
	if(info.This.GetType() == tVariant::vtObject)
	{
		return; // 何もできず
	}

	// プリミティブ型
	// TODO: 各サブクラスでオーバーライドされているかのように実装すべき？
	info.This.SetHint(static_cast<risse_uint32>(info.value.operator risse_int64()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::get_hash(const tNativePropGetInfo & info)
{
	if(info.This.GetType() == tVariant::vtObject)
	{
		// 適当にハッシュを作って返す。
		// 値はアドレスを整数にキャストしたものを元にするが、
		// 値がポインタに似てると(GCがその先にあるオブジェクトをマークしてしまうので)
		// やっかいなので bit not をとる。
		// またオブジェクトは 8byte とか 16byte にアラインメントされた位置に確保されるが
		// その影響を少なくするために 4 回右シフトした結果を xor する。

		// tObjectClass::get_hint() も参照のこと
		if(info.result)
		{
			risse_uint32 ptr = ~static_cast<risse_int32>(
				reinterpret_cast<risse_ptruint>(info.This.GetObjectInterface()));
			*info.result = static_cast<risse_int64>(ptr ^ (ptr >> 4));
		}
		return;
	}

	// プリミティブ型
	// TODO: 各サブクラスでオーバーライドされているかのように実装すべき？
	if(info.result)
		*info.result = static_cast<risse_int64>(info.This.GetHash());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tObjectClass::isA(const tVariant & Class,
						const tNativeCallInfo & info)
{
	// 自身が引数(=クラス) のインスタンスかどうかを得る
	return info.This.InstanceOf(info.engine, Class);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectClass::getInstanceMember(
	const tNativeCallInfo & info,
	const tString & membername)
{
	// This のインスタンスメンバを取得する
	return info.This.GetPropertyDirect(info.engine, membername,
			tMemberAttribute::GetDefault()|
			tOperateFlags::ofInstanceMemberOnly,
			info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::setInstanceMember(
	const tNativeCallInfo & info,
	const tString & membername, const tVariant & value)
{
	// This のインスタンスメンバを設定する
	info.This.SetPropertyDirect(info.engine, membername,
		tMemberAttribute::GetDefault()|
		tOperateFlags::ofInstanceMemberOnly|
		tOperateFlags::ofMemberEnsure,
					value, info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectClass::getPublicMembers(
				const tNativeCallInfo & info)
{
	bool recursive = true;
	if(info.args.HasArgument(0))
		recursive = info.args[0].operator bool();

	if(info.This.GetType() == tVariant::vtObject)
	{
		if(recursive)
		{
			// TODO: 再帰的な(親クラスも見に行く)enumuration
			RISSE_ASSERT(!"recursive enumuration is not implemented yet");
		}
		else
		{
			// メンバ名を配列にして返す
			tObjectBase * object =
				static_cast<tObjectBase*>(
					info.This.GetObjectInterface());
			tVariant array =
				tVariant(object->GetRTTI()->GetScriptEngine()->ArrayClass).
																New();
			class callback : public tObjectBase::tEnumMemberCallback
			{
				tVariant array;
			public:
				callback(const tVariant & array_) : array(array_) {}
				bool OnEnum(const tString & name,
					const tObjectBase::tMemberData & data)
				{
					RISSE_ASSERT(data.Attribute.GetAccess() != tMemberAttribute::acNone);
					if(data.Attribute.GetAccess() == tMemberAttribute::acPublic)
						array.Invoke_Object(ss_push, name);
					return true;
				}
			} cb(array);
			object->Enumurate(&cb);
			return array;
		}
	}
	else
	{
		// TODO: プリミティブ型での実装
		RISSE_ASSERT(!"enumuration on primitive types is not implemented yet");
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectClass::toException(const tNativeCallInfo & info)
{
	// デフォルトの動作は、This を文字列化してそれを RuntimeException.new 
	// に渡し、その結果を返す
	// TODO: global.RuntimeException を見に行かずに直接クラスを見に行っちゃっていいの？
	return tVariant(info.engine->RuntimeExceptionClass).
			New(0, tMethodArgument::New((tString)info.This));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::p(const tMethodArgument & args)
{
	for(risse_size i = 0; i < args.GetArgumentCount(); i++)
	{
		const tVariant & v = args[i];

		if(v.GetType() == tVariant::vtObject)
		{
			risse_char buf[40];
			::Risse::pointer_to_str(v.GetObjectInterface(), buf);
			FPrint(stdout, (tString(RISSE_WS("Object@")) + buf).c_str());
			const tVariant * context = v.GetContext();
			if(context)
			{
				if(context->GetType() == tVariant::vtObject)
				{
					if(context == tVariant::GetDynamicContext())
					{
						FPrint(stdout, RISSE_WS(":dynamic"));
					}
					else
					{
						::Risse::pointer_to_str(context->GetObjectInterface(), buf);
						FPrint(stdout, (tString(RISSE_WS(":")) + buf).c_str());
					}
				}
				else
				{
					FPrint(stdout, (context->AsHumanReadable()).c_str());
				}
			}
		}
		else
		{
			FPrint(stdout, (v.operator tString()).c_str());
		}
	}
	FPrint(stdout, RISSE_WS("\n"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::Bless(tObjectInterface * intf)
{
	// intf の RTTI にこのクラスの ClassRTTI を設定する
	intf->SetRTTI(&GetClassRTTI());

	// intf に class を登録し、Object クラスを指すようにする
	intf->Do(ocDSet, NULL, ss_class,
		tOperateFlags(tMemberAttribute::GetDefault()) |
		tOperateFlags::ofMemberEnsure|tOperateFlags::ofInstanceMemberOnly,
		tMethodArgument::New(tVariant((tObjectInterface *)this)) );
	intf->Do(ocDSetAttrib, NULL, ss_class,
		(risse_uint32)tMemberAttribute(tMemberAttribute::acInternal) |
			(risse_uint32)tOperateFlags(tOperateFlags::ofInstanceMemberOnly),
		tMethodArgument::New(tVariant((tObjectInterface *)this)) );
}
//---------------------------------------------------------------------------

} /* namespace Risse */

