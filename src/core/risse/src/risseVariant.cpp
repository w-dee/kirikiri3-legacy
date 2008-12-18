//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バリアント型の実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseVariant.h"
#include "risseLexerUtils.h"
#include "risseObject.h"
#include "risseStringClass.h"
#include "risseIntegerClass.h"
#include "risseRealClass.h"
#include "risseBooleanClass.h"
#include "risseVoidClass.h"
#include "risseNullClass.h"
#include "risseOctetClass.h"
#include "risseStaticStrings.h"
#include "risseExceptionClass.h"
#include "risseScriptBlockClass.h"
#include "risseScriptEngine.h"
#include "risseStringTemplate.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(8265,43737,22162,17503,41631,46790,57901,27164);



//---------------------------------------------------------------------------
// tIllegalArgumentClassExceptionClass を投げるためのマクロ
// この中では this と rhs を使い、tVariant を帰す。
#define RISSE_THROW_ILLEGAL_ARG_TYPE(method_name) \
	tIllegalArgumentClassExceptionClass::ThrowNonAcceptableClass( \
			GetClassName() + ss_doubleColon + (method_name), rhs.GetClassName()); \
			return tVariant::GetVoidObject();

// 上のマクロとは違い false を返す。
#define RISSE_THROW_ILLEGAL_ARG_TYPE_B(method_name) \
	tIllegalArgumentClassExceptionClass::ThrowNonAcceptableClass( \
			GetClassName() + ss_doubleColon + (method_name), rhs.GetClassName()); \
			return false;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::ThrowCannotCreateInstanceFromNonClassObjectException()
{
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromNonClassObject();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::ThrowBadContextException()
{
	tBadContextExceptionClass::Throw();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::ThrowNoSuchMemberException(const tString &name)
{
	tNoSuchMemberExceptionClass::Throw(name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::ThrowIllegalArgumentClassException(const tString & class_name)
{
	tIllegalArgumentClassExceptionClass::ThrowSpecifyInstanceOfClass(class_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::ThrowIllegalOperationMethod(const tString & method_name) const
{
	tIllegalArgumentClassExceptionClass::ThrowIllegalOperationMethod(
			GetClassName() + ss_doubleColon + (method_name));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// voidオブジェクトへのconst参照を保持するオブジェクト
// これのバイナリレイアウトはtVariantと同一でなければならない
tVariant::tStaticPrimitive tVariant::VoidObject = {
	tVariant::vtVoid,
	{}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// nullオブジェクトへのconst参照を保持するオブジェクト
// これのバイナリレイアウトはtVariantと同一でなければならない
tVariant::tStaticPrimitive tVariant::NullObject = {
	tVariant::vtNull,
	{}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// DynamicContextオブジェクトへのconst参照を保持するオブジェクト
// これのバイナリレイアウトはtVariantと同一でなければならない
static tIdentifyObject DynamicContextObject;
tVariant::tStaticObject tVariant::DynamicContext = {
	reinterpret_cast<risse_ptruint>(&DynamicContextObject) + tVariant::ObjectPointerBias, NULL,
	{}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// SetTypeTag で用いる vtData 用の tIdentifyObject インスタンス
static tIdentifyObject DummyDataClass;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tScriptEngine * tVariant::GetScriptEngine_Data() const
{
	RISSE_ASSERT(GetType() == vtData);
	return GetDataClassInstance()->GetRTTI()->GetScriptEngine();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tScriptEngine * tVariant::GetScriptEngine_Object() const
{
	RISSE_ASSERT(GetType() == vtObject);
	return GetObjectInterface()->GetRTTI()->GetScriptEngine();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const risse_char * tVariant::GetTypeString(tType type)
{
	switch(type)
	{
	case vtVoid:		return ss_void.c_str();
	case vtInteger:		return ss_integer.c_str();
	case vtReal:		return ss_real.c_str();
	case vtNull:		return ss_null.c_str();
	case vtBoolean:		return ss_boolean.c_str();
	case vtString:		return ss_string.c_str();
	case vtObject:		return ss_object.c_str();
	case vtData:		return ss_data.c_str();
	case vtOctet:		return ss_octet.c_str();
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::SetTypeTag(tType type)
{
	switch(type)
	{
	case vtVoid:		Clear();							RISSE_ASSERT(GetType()==vtVoid);    return;
	case vtInteger:		*this = (risse_int64)0;				RISSE_ASSERT(GetType()==vtInteger); return;
	case vtReal:		*this = (risse_real)0.0;			RISSE_ASSERT(GetType()==vtReal);    return;
	case vtNull:		Nullize();							RISSE_ASSERT(GetType()==vtNull);    return;
	case vtBoolean:		*this = false;						RISSE_ASSERT(GetType()==vtBoolean); return;
	case vtString:		*this = tString::GetEmptyString();	RISSE_ASSERT(GetType()==vtString);  return;
	case vtOctet:		*this = tOctet();					RISSE_ASSERT(GetType()==vtOctet);   return;
	case vtData:		*this =
		tVariant(static_cast<tPrimitiveClassBase*>(static_cast<void*>(&DummyDataClass)), (void*)NULL);
															RISSE_ASSERT(GetType()==vtData);    return;
	case vtObject:		*this = *GetDynamicContext();		RISSE_ASSERT(GetType()==vtObject);  return;
	}
	return;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const risse_char * tVariant::GetGuessTypeString(tGuessType type)
{
	switch(type)
	{
	case gtVoid:		return ss_Void.c_str();
	case gtInteger: 	return ss_Integer.c_str();
	case gtReal:		return ss_Real.c_str();
	case gtNull:		return ss_Null.c_str();
	case gtBoolean:		return ss_Boolean.c_str();
	case gtString:		return ss_String.c_str();
	case gtOctet:		return ss_Object.c_str();
	case gtData:		return ss_Data.c_str();
	case gtObject:		return ss_Octet.c_str();

	case gtAny:
		return RISSE_WS("any");
	case gtError:
		return RISSE_WS("error");
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
inline tPrimitiveClassBase * tVariant::GetPrimitiveClass(tScriptEngine * engine) const
{
	tPrimitiveClassBase * Class = NULL;
	switch(GetType())
	{
	case vtVoid:	Class = engine->VoidClass; break;
	case vtInteger:	Class = engine->IntegerClass; break;
	case vtReal:	Class = engine->RealClass; break;
	case vtNull:	Class = engine->NullClass; break;
	case vtBoolean:	Class = engine->BooleanClass; break;
	case vtString:	Class = engine->StringClass; break;
	case vtOctet:	Class = engine->OctetClass; break;
	case vtData:	Class = GetDataClassInstance(); break;
	default:		break;
	}
	RISSE_ASSERT(Class != NULL);
	return Class;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::SetContext(const tVariant &context)
{
	const tVariant * context_ptr;
	if(context.GetType() == tVariant::vtObject)
	{
		if(context.IsDynamicContext())
			context_ptr = tVariant::GetDynamicContext();
		else
			context_ptr = new tVariant(context);
	}
	else
	{
		context_ptr = new tVariant(context);
	}
	SetContext(context_ptr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant::tRetValue
	tVariant::OperateForMember(tScriptEngine * engine, RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	switch(GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
		// プリミティブ型に対する処理
		{
			tRetValue rv = GetPrimitiveClass(engine)->GetGateway().
				Operate(engine, code, result, name,
				flags | tOperateFlags::ofUseClassMembersRule,
					// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
					// また、ゲートウェイの members の中から探す
				args, *this); // 動作コンテキストは常に *this
			if(rv == rvNoError && result) // TODO: code が ocDGet かどうかをチェックすべきじゃないのか？
			{
				// コンテキストを設定する
				// コンテキストはこの操作が実行された時の状態を保っていなければ
				// ならない。たいていのプリミティブ型は immutable とはいえ、内部実装は
				// 完全に mutable なので、この時点での実行結果を保存しておくために
				// new でオブジェクトを再確保し、固定する。
				// TODO: 返りのオブジェクトがコンテキストを持ってないと、毎回newが行われて
				// しまう。関数やプロパティ以外はコンテキストを伴う必要はないので
				// 努めて関数やプロパティ以外はダミーでもよいからコンテキストを
				// 設定するようにするべき。
				// TODO: Data 型の場合ってこれ必要？Data型の場合は Object と同じく mutable
				// だけれども………
				if(!result->HasContext() && !(flags & tOperateFlags::ofUseClassMembersRule))
					result->SetContext(new tVariant(*this));
			}
			return rv;
		}

	case vtObject:
		{
			tObjectInterface * intf = GetObjectInterface();
			return intf->Operate(code, result, name, flags, args,
				SelectContext(flags, This)
				);
		}
	}
	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::GetPropertyDirect_Primitive(tScriptEngine * engine, const tString & name, risse_uint32 flags, const tVariant & This) const
{
	tVariant result;
	GetPrimitiveClass(engine)->GetGateway().Do(engine, ocDGet, &result, name,
				flags | tOperateFlags::ofUseClassMembersRule,
					// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
					// また、ゲートウェイの members の中から探す
				tMethodArgument::Empty(), *this); // 動作コンテキストは常に *this
	// コンテキストを設定する
	// ここの長ったらしい説明は tVariant::OperateForMember() 内を参照
	if(!result.HasContext() && !(flags & tOperateFlags::ofUseClassMembersRule))
		result.SetContext(new tVariant(*this));
	return result;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::GetPropertyDirect_Object  (const tString & name, risse_uint32 flags, const tVariant & This) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	tVariant ret;
	intf->Do(ocDGet, &ret, name,
		flags, tMethodArgument::Empty(),
		SelectContext(flags, This)
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::SetPropertyDirect_Primitive(tScriptEngine * engine,
	const tString & name, risse_uint32 flags, const tVariant & value,
	const tVariant & This) const
{
	GetPrimitiveClass(engine)->GetGateway().Do(engine, ocDSet, NULL, name,
				flags | tOperateFlags::ofUseClassMembersRule,
					// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
					// また、ゲートウェイの members の中から探す
				tMethodArgument::New(value), *this); // 動作コンテキストは常に *this
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::SetPropertyDirect_Object  (const tString & name,
		risse_uint32 flags, const tVariant & value,
		const tVariant & This) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	intf->Do(ocDSet, NULL, name,
		flags, tMethodArgument::New(value),
		SelectContext(flags, This)
		);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::DeletePropertyDirect_Primitive(tScriptEngine * engine, const tString & name, risse_uint32 flags) const
{
	GetPrimitiveClass(engine)->GetGateway().Do(engine, ocDDelete, NULL, name,
				flags | tOperateFlags::ofUseClassMembersRule,
					// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
					// また、ゲートウェイの members の中から探す
				tMethodArgument::Empty(), *this); // 動作コンテキストは常に *this
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::DeletePropertyDirect_Object   (const tString & name, risse_uint32 flags) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	intf->Do(ocDDelete, NULL, name,
		flags, tMethodArgument::Empty(),
		*this // 動作コンテキストは無視されるが一応 this を指定しておく
		);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::SetAttributeDirect_Object  (const tString & key, risse_uint32 attrib) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	intf->Do(ocDSetAttrib, NULL, key,
		attrib, tMethodArgument::Empty(),
		*this // 動作コンテキストは無視されるが一応 this を指定しておく
		);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::FuncCall(tScriptEngine * engine, tVariant * ret, risse_uint32 flags,
	const tMethodArgument & args,
	const tVariant & This) const
{
	switch(GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
		FuncCall_Primitive(engine, ret, tString::GetEmptyString(), flags, args, This); return;
	case vtObject:
		FuncCall_Object   (        ret, tString::GetEmptyString(), flags, args, This); return;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::FuncCall_Primitive(tScriptEngine * engine,
	tVariant * ret, const tString & name,
	risse_uint32 flags, const tMethodArgument & args,
	const tVariant & This) const
{
	GetPrimitiveClass(engine)->GetGateway().
		Do(engine, ocFuncCall, ret, name,
		flags |tOperateFlags::ofUseClassMembersRule,
		// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
		// また、ゲートウェイの members の中から探す
		args, *this); // 動作コンテキストは常に *this
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::FuncCall_Object  (
	tVariant * ret, const tString & name, risse_uint32 flags,
	const tMethodArgument & args, const tVariant & This) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	intf->Do(ocFuncCall, ret, name,
		flags, args,
		SelectContext(flags, This)
		);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Invoke_Primitive(tScriptEngine * engine, const tString & membername) const
{
	tVariant ret;
	GetPrimitiveClass(engine)->GetGateway().
		Do(engine, ocFuncCall, &ret, membername,
		tOperateFlags::ofUseClassMembersRule,
		// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
		// また、ゲートウェイの members の中から探す
		tMethodArgument::Empty(), *this); // 動作コンテキストは常に *this
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Invoke_Object   (const tString & membername) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	tVariant ret;
	intf->Do(ocFuncCall, &ret, membername,
		0,
		tMethodArgument::Empty(),
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Invoke_Primitive(tScriptEngine * engine, const tString & membername,const tVariant & arg1) const
{
	tVariant ret;
	GetPrimitiveClass(engine)->GetGateway().
		Do(engine, ocFuncCall, &ret, membername,
		tOperateFlags::ofUseClassMembersRule,
		// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
		// また、ゲートウェイの members の中から探す
		tMethodArgument::New(arg1), *this); // 動作コンテキストは常に *this
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Invoke_Object   (const tString & membername,const tVariant & arg1) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	tVariant ret;
	intf->Do(ocFuncCall, &ret, membername,
		0,
		tMethodArgument::New(arg1),
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Invoke_Primitive(tScriptEngine * engine, const tString & membername,const tVariant & arg1,const tVariant & arg2) const
{
	tVariant ret;
	GetPrimitiveClass(engine)->GetGateway().
		Do(engine, ocFuncCall, &ret, membername,
		tOperateFlags::ofUseClassMembersRule,
		// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
		// また、ゲートウェイの members の中から探す
		tMethodArgument::New(arg1, arg2), *this); // 動作コンテキストは常に *this
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Invoke_Object   (const tString & membername,const tVariant & arg1,const tVariant & arg2) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	tVariant ret;
	intf->Do(ocFuncCall, &ret, membername,
		0,
		tMethodArgument::New(arg1, arg2),
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::New_Object  (const tString & name,
	risse_uint32 flags, const tMethodArgument & args) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	tVariant ret;
	intf->Do(ocNew, &ret, name,
		flags,
		args,
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Plus_String   () const
{
	tVariant val;
	const risse_char *p = AsString().c_str();
	if(tLexerUtility::ParseNumber(p, val))
	{
		// 解析に成功
		return val;
	}
	else
	{
		// 解析に失敗
		return (risse_int64)0;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitOr_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0;
	case vtInteger:	return rhs.CastToInteger_Integer(); // void | integer
	case vtReal:	return rhs.CastToInteger_Real(); // void | real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitOr_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitOr_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger(); // integer | void
	case vtInteger:	return AsInteger() | rhs.CastToInteger_Integer(); // integer | integer
	case vtReal:	return AsInteger() | rhs.CastToInteger_Real(); // integer | real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitOr_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitOr_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToInteger_Real(); // real | void
	case vtInteger:	return CastToInteger_Real() | rhs.CastToInteger_Integer(); // real | integer
	case vtReal:	return CastToInteger_Real() | rhs.CastToInteger_Real(); // real | real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitOr_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitOr_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitOr_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitOr_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitOr_String     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitOr_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitOr_Octet     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitOr_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitOr_Boolean     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitXor_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0;
	case vtInteger:	return rhs.CastToInteger_Integer(); // void ^ integer
	case vtReal:	return rhs.CastToInteger_Real(); // void ^ real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitXor_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitXor_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger(); // integer ^ void
	case vtInteger:	return AsInteger() ^ rhs.CastToInteger_Integer(); // integer ^ integer
	case vtReal:	return AsInteger() ^ rhs.CastToInteger_Real(); // integer ^ real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitXor_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitXor_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToInteger_Real(); // real | void
	case vtInteger:	return CastToInteger_Real() ^ rhs.CastToInteger_Integer(); // real | integer
	case vtReal:	return CastToInteger_Real() ^ rhs.CastToInteger_Real(); // real | real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitXor_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitXor_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitXor_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitXor_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitXor_String     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitXor_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitXor_Octet     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitXor_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitXor_Boolean     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitAnd_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0;
	case vtInteger:	return (risse_int64)0; // void & integer
	case vtReal:	return (risse_int64)0; // void & real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitAnd_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitAnd_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // integer & void
	case vtInteger:	return AsInteger() & rhs.CastToInteger_Integer(); // integer & integer
	case vtReal:	return AsInteger() & rhs.CastToInteger_Real(); // integer & real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitAnd_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitAnd_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // real & void
	case vtInteger:	return CastToInteger_Real() & rhs.CastToInteger_Integer(); // real & integer
	case vtReal:	return CastToInteger_Real() & rhs.CastToInteger_Real(); // real & real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitAnd_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitAnd_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitAnd_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitAnd_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitAnd_String     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitAnd_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtString:
	case vtBoolean:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitAnd_Octet     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::BitAnd_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeBitAnd_Boolean     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Equal_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return true; // void == void
	case vtInteger:	return CastToInteger_Void() == rhs.AsInteger();
	case vtReal:	return CastToReal_Void()    == rhs.AsReal();
	case vtNull:	return false; // void == null
	case vtBoolean:	return CastToBoolean_Void() == rhs.CastToBoolean_Boolean();
	case vtString:	return rhs.AsString().IsEmpty();
	case vtOctet:	return rhs.AsOctet().IsEmpty();
	case vtData:	return false; // incomplete; 交換法則を成り立たせるかも
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Equal_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger() == 0;
	case vtInteger:	return AsInteger() == rhs.AsInteger();
	case vtReal:	return AsInteger() == rhs.AsReal();
	case vtNull:	return false; // 数値とnullの比較は常に偽
	case vtBoolean:	return CastToBoolean_Integer() == rhs.CastToBoolean_Boolean();
	case vtString:	return AsInteger() == rhs.CastToInteger_String();
	case vtOctet:	return false; // 数値とoctetの比較は常に偽
	case vtData:	return false; // incomplete; 交換法則を成り立たせるかも
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Equal_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsReal() == 0.0;
	case vtInteger:	return AsReal() == rhs.AsInteger();
	case vtReal:	return AsReal() == rhs.AsReal();
	case vtNull:	return false; // 数値とnullの比較は常に偽
	case vtBoolean:	return CastToBoolean_Real() == rhs.CastToBoolean_Boolean();
	case vtString:	return AsReal() == rhs.CastToReal_String();
	case vtOctet:	return false; // 数値とoctetの比較は常に偽
	case vtData:	return false; // incomplete; 交換法則を成り立たせるかも
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Equal_Null     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void == void
	case vtInteger:	return false;
	case vtReal:	return false;
	case vtNull:	return true;
	case vtBoolean:	return rhs.CastToBoolean_Boolean() == false; // 偽とnullの比較は真
	case vtString:	return false;
	case vtOctet:	return false;
	case vtData:	return false; // incomplete; 交換法則を成り立たせるかも
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Equal_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsString().IsEmpty();
	case vtInteger:	return CastToInteger_String() == rhs.AsInteger();
	case vtReal:	return CastToReal_String() == rhs.AsReal();
	case vtNull:	return false; // 文字列と null の比較は常に偽
	case vtBoolean:	return CastToBoolean_String() == rhs.CastToBoolean_Boolean();
	case vtString:	return AsString() == rhs.AsString();
	case vtOctet:	return false; // 文字列とoctetの比較は常に偽
	case vtData:	return false; // incomplete; 交換法則を成り立たせるかも
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Equal_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsOctet().IsEmpty();
	case vtInteger:	return false; // オクテット列と integer の比較は常に偽
	case vtReal:	return false; // オクテット列と real の比較は常に偽
	case vtNull:	return false; // オクテット列と null の比較は常に偽
	case vtBoolean:	return CastToBoolean_Octet() == rhs.CastToBoolean_Boolean();
	case vtString:	return false; // オクテット列と string の比較は常に偽
	case vtOctet:	return AsOctet() == rhs.AsOctet();
	case vtData:	return false; // incomplete; 交換法則を成り立たせるかも
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Equal_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToBoolean_Boolean() == 0.0;
	case vtInteger:	return CastToBoolean_Boolean() == rhs.AsInteger();
	case vtReal:	return CastToBoolean_Boolean() == rhs.AsReal();
	case vtNull:	return CastToBoolean_Boolean() == false; // false と null の比較ならば真
	case vtBoolean:	return CastToBoolean_Boolean() == rhs.CastToBoolean_Boolean();
	case vtString:	return CastToBoolean_Boolean() == rhs.CastToBoolean_String();
	case vtOctet:	return CastToBoolean_Boolean() == rhs.CastToBoolean_Octet();
	case vtData:	return false; // incomplete; 交換法則を成り立たせるかも
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::StrictEqual_Real     (const tVariant & rhs) const
{
	// Real::identify メソッドの実装はこれがつかわれるので、
	// hash プロパティと齟齬が生じないようにすること。
	// このソースファイルの
	// tVariant::GetHash_Real() も参照のこと。

	// Real の符合、クラスも厳密に同じかどうかを見る
	if(rhs.GetType() != vtReal) return false;

	risse_real lv =     AsReal();
	risse_real rv = rhs.AsReal();
	risse_int32 l_cls = GetFPClass(lv);
	risse_int32 r_cls = GetFPClass(rv);

	// NaN の場合は符合はチェックしない
	if(RISSE_FC_IS_NAN(l_cls))
		return RISSE_FC_IS_NAN(r_cls);

	// クラスと値が同じであるかどうかを返す
	return l_cls == r_cls && lv == rv;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Identify_Data   (const tVariant & rhs) const
{
	return Invoke_Primitive(GetScriptEngine_Data(), ss_identify, rhs).CastToBoolean();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Identify_Object   (const tVariant & rhs) const
{
	return Invoke_Object(ss_identify, rhs).CastToBoolean();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Lesser_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void < void
	case vtInteger:	return CastToInteger_Void() < rhs.AsInteger();
	case vtReal:	return CastToReal_Void()    < rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	return !rhs.AsString().IsEmpty();
	case vtOctet:	return !rhs.AsOctet().IsEmpty();
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLesser_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtBoolean;
	case gtOctet:		return gtBoolean;
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Lesser_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger() < 0;
	case vtInteger:	return AsInteger() < rhs.AsInteger();
	case vtReal:	return AsInteger() < rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLesser_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Lesser_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsReal() < 0.0;
	case vtInteger:	return AsReal() < rhs.AsInteger();
	case vtReal:	return AsReal() < rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLesser_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Lesser_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLesser_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Lesser_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void は空文字列と見なす
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	return AsString() < rhs.AsString();
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLesser_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtBoolean;
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Lesser_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void は空オクテット列と見なす
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtOctet:	return AsOctet() < rhs.AsOctet();
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLesser_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtBoolean;
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Lesser_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLesser_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Greater_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void > void
	case vtInteger:	return CastToInteger_Void() > rhs.AsInteger();
	case vtReal:	return CastToReal_Void()    > rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	return false;
	case vtOctet:	return false;
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeGreater_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtBoolean;
	case gtOctet:		return gtBoolean;
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Greater_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger() > 0;
	case vtInteger:	return AsInteger() > rhs.AsInteger();
	case vtReal:	return AsInteger() > rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeGreater_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Greater_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsReal() > 0.0;
	case vtInteger:	return AsReal() > rhs.AsInteger();
	case vtReal:	return AsReal() > rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeGreater_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Greater_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeGreater_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Greater_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !AsString().IsEmpty();
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	return AsString() > rhs.AsString();
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeGreater_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtBoolean;
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Greater_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !AsOctet().IsEmpty();
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtOctet:	return AsOctet() > rhs.AsOctet();
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeGreater_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtBoolean;
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::Greater_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeGreater_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::LesserOrEqual_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void < void
	case vtInteger:	return !(CastToInteger_Void() > rhs.AsInteger());
	case vtReal:	return !(CastToReal_Void()    > rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	return !false;
	case vtOctet:	return !false;
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::LesserOrEqual_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsInteger() > 0);
	case vtInteger:	return !(AsInteger() > rhs.AsInteger());
	case vtReal:	return !(AsInteger() > rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::LesserOrEqual_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsReal() > 0.0);
	case vtInteger:	return !(AsReal() > rhs.AsInteger());
	case vtReal:	return !(AsReal() > rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::LesserOrEqual_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::LesserOrEqual_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(!AsString().IsEmpty());
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	return !(AsString() > rhs.AsString());
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::LesserOrEqual_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(!AsOctet().IsEmpty());
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtOctet:	return !(AsOctet() > rhs.AsOctet());
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::LesserOrEqual_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::GreaterOrEqual_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void > void
	case vtInteger:	return !(CastToInteger_Void() < rhs.AsInteger());
	case vtReal:	return !(CastToReal_Void()    < rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	return !(!rhs.AsString().IsEmpty());
	case vtOctet:	return !(!rhs.AsOctet().IsEmpty());
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::GreaterOrEqual_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsInteger() < 0);
	case vtInteger:	return !(AsInteger() < rhs.AsInteger());
	case vtReal:	return !(AsInteger() < rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::GreaterOrEqual_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsReal() < 0.0);
	case vtInteger:	return !(AsReal() < rhs.AsInteger());
	case vtReal:	return !(AsReal() < rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::GreaterOrEqual_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::GreaterOrEqual_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void は空文字列と見なす
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	return !(AsString() < rhs.AsString());
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::GreaterOrEqual_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void は空オクテット列と見なす
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtOctet:	return !(AsOctet() < rhs.AsOctet());
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::GreaterOrEqual_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RBitShift_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // void >>> void
	case vtInteger:	return (risse_int64)0; // void >>> integer
	case vtReal:	return (risse_int64)0; // void >>> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRBitShift_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RBitShift_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger(); // integer >>> void
	case vtInteger:	return (risse_int64)((risse_uint64)AsInteger() >> rhs.CastToInteger_Integer()); // integer >>> integer
	case vtReal:	return (risse_int64)((risse_uint64)AsInteger() >> rhs.CastToInteger_Real()); // integer >>> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRBitShift_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RBitShift_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToInteger_Real(); // real >>> void
	case vtInteger:	return (risse_int64)((risse_uint64)CastToInteger_Real() >> rhs.CastToInteger_Integer()); // real >>> integer
	case vtReal:	return (risse_int64)((risse_uint64)CastToInteger_Real() >> rhs.CastToInteger_Real()); // real >>> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRBitShift_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RBitShift_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRBitShift_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RBitShift_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRBitShift_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RBitShift_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRBitShift_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RBitShift_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRBitShift_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::LShift_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // void << void
	case vtInteger:	return (risse_int64)0; // void << integer
	case vtReal:	return (risse_int64)0; // void << real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLShift_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::LShift_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger(); // integer << void
	case vtInteger:	return AsInteger() << rhs.CastToInteger_Integer(); // integer << integer
	case vtReal:	return AsInteger() << rhs.CastToInteger_Real(); // integer << real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLShift_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::LShift_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToInteger_Real(); // real << void
	case vtInteger:	return CastToInteger_Real() << rhs.CastToInteger_Integer(); // real << integer
	case vtReal:	return CastToInteger_Real() << rhs.CastToInteger_Real(); // real << real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLShift_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::LShift_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLShift_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::LShift_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLShift_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::LShift_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLShift_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::LShift_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeLShift_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RShift_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // void >> void
	case vtInteger:	return (risse_int64)0; // void >> integer
	case vtReal:	return (risse_int64)0; // void >> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRShift_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RShift_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger(); // integer >> void
	case vtInteger:	return AsInteger() >> rhs.CastToInteger_Integer(); // integer >> integer
	case vtReal:	return AsInteger() >> rhs.CastToInteger_Real(); // integer >> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRShift_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RShift_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToInteger_Real(); // real >> void
	case vtInteger:	return CastToInteger_Real() >> rhs.CastToInteger_Integer(); // real >> integer
	case vtReal:	return CastToInteger_Real() >> rhs.CastToInteger_Real(); // real >> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRShift_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RShift_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRShift_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RShift_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRShift_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RShift_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRShift_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::RShift_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeRShift_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mod_Void     (const tVariant & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtInteger:	rhs_value = rhs.AsInteger();						goto do_div;
	case vtReal:	rhs_value = static_cast<risse_int64>(rhs.AsReal());	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariant();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)0 % rhs_value; // void % integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMod_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mod_Integer  (const tVariant & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tArithmeticExceptionClass::ThrowDivideByZeroException(); return tVariant();
	case vtInteger:	rhs_value = rhs.AsInteger();						goto do_div;
	case vtReal:	rhs_value = static_cast<risse_int64>(rhs.AsReal());	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariant();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return AsInteger() % rhs_value; // integer % integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMod_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtReal:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mod_Real     (const tVariant & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tArithmeticExceptionClass::ThrowDivideByZeroException(); return tVariant();
	case vtInteger:	rhs_value = rhs.AsInteger();			goto do_div;
	case vtReal:	rhs_value = (risse_int64)rhs.AsReal();	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariant();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)AsReal() % rhs_value; // real % integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMod_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtReal:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mod_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMod_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mod_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMod_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mod_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMod_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mod_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMod_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Div_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtInteger:	return (risse_real)0.0 / rhs.AsInteger(); // void / integer
	case vtReal:	return (risse_real)0.0 / rhs.AsReal(); // void / real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeDiv_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtReal|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Div_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_real)AsInteger() / rhs.CastToReal_Void(); // integer / void
	case vtInteger:	return (risse_real)AsInteger() / rhs.AsInteger(); // integer / integer
	case vtReal:	return (risse_real)AsInteger() / rhs.AsReal(); // integer / real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeDiv_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtReal;
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtReal	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Div_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsReal() / rhs.CastToReal_Void(); // real / void
	case vtInteger:	return AsReal() / rhs.AsInteger(); // real / integer
	case vtReal:	return AsReal() / rhs.AsReal(); // real / real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeDiv_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtReal;
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtReal	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Div_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeDiv_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Div_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeDiv_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Div_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeDiv_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Div_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeDiv_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Idiv_Void     (const tVariant & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtInteger:	rhs_value = rhs.AsInteger();						goto do_div;
	case vtReal:	rhs_value = static_cast<risse_int64>(rhs.AsReal());	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariant();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)0 / rhs_value; // void / integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeIdiv_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtReal:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Idiv_Integer  (const tVariant & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tArithmeticExceptionClass::ThrowDivideByZeroException(); return tVariant();
	case vtInteger:	rhs_value = rhs.AsInteger();						goto do_div;
	case vtReal:	rhs_value = static_cast<risse_int64>(rhs.AsReal());	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariant();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return AsInteger() / rhs_value; // integer / integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeIdiv_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtReal:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Idiv_Real     (const tVariant & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tArithmeticExceptionClass::ThrowDivideByZeroException(); return tVariant();
	case vtInteger:	rhs_value = rhs.AsInteger();			goto do_div;
	case vtReal:	rhs_value = (risse_int64)rhs.AsReal();	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariant();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)AsReal() / rhs_value; // real / integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeIdiv_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtReal:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Idiv_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeIdiv_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Idiv_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeIdiv_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Idiv_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeIdiv_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Idiv_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeIdiv_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mul_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return tVariant::GetVoidObject(); // void * void
	case vtInteger:	return (risse_int64)0; // void * integer
	case vtReal:	return (risse_real)0.0;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMul_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtVoid;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mul_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // integer * void
	case vtInteger:	return AsInteger() * rhs.AsInteger(); // integer * integer
	case vtReal:	return AsInteger() * rhs.AsReal(); // integer * real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMul_Integer   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mul_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_real)0; // real * void
	case vtInteger:	return AsReal() * rhs.AsInteger(); // real * integer
	case vtReal:	return AsReal() * rhs.AsReal(); // real * real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMul_Real   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtReal;
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mul_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMul_Null   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mul_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMul_String   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mul_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMul_Octet   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Mul_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
	case vtData:
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeMul_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Add_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return tVariant(); // void + void = void かなぁ
	case vtInteger:	return rhs; // void + integer
	case vtReal:	return rhs;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	return rhs;
	case vtOctet:	return rhs;
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeAdd_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtVoid;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtString;
	case gtOctet:		return gtOctet;
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Add_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this;
	case vtInteger:	return AsInteger() + rhs.AsInteger();
	case vtReal:	return AsInteger() + rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeAdd_Integer  (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Add_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this;
	case vtInteger:	return AsReal() + rhs.AsInteger();
	case vtReal:	return AsReal() + rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeAdd_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtReal;
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Add_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeAdd_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Add_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this;
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	return AsString() + rhs.AsString();
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeAdd_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtString;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtString;
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Add_Octet    (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this;
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtOctet:	return AsOctet() + rhs.AsOctet();
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeAdd_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtOctet;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtOctet;
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Add_Boolean  (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeAdd_Boolean  (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Sub_Void     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return tVariant::GetVoidObject(); // void - void = void かなぁ
	case vtInteger:	return - rhs.AsInteger(); // void - integer
	case vtReal:	return - rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeSub_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtVoid;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Sub_Integer  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this; // integer - void
	case vtInteger:	return AsInteger() - rhs.AsInteger(); // integer - integer
	case vtReal:	return AsInteger() - rhs.AsReal(); // integer - real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeSub_Integer   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtVoid;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Sub_Real     (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this; // real - void
	case vtInteger:	return AsReal() - rhs.AsInteger(); // real - integer
	case vtReal:	return AsReal() - rhs.AsReal(); // real - real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeSub_Real   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtVoid;
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Sub_Null     (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeSub_Null   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Sub_Octet    (const tVariant & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeSub_Octet   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Sub_String   (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtBoolean:
	case vtString:
	case vtOctet:
			RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeSub_String   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVariant::Sub_Boolean  (const tVariant & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtData:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariant::GuessTypeSub_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtData:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariant::InstanceOf(tScriptEngine * engine, const tVariant & rhs) const
{
	// this の class を得る
	tVariant Class = GetPropertyDirect(engine, ss_class, tOperateFlags::ofInstanceMemberOnly, *this);

	RISSE_ASSERT(Class.GetType() == vtObject);

	tVariant ret;
	Class.GetObjectInterface()->Do(ocInstanceOf, &ret, tString::GetEmptyString(), 0,
			tMethodArgument::New(rhs), Class);
	return ret.operator bool();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int64 tVariant::CastToInteger_String   () const
{
	tVariant val;
	const risse_char *p = AsString().c_str();
	if(tLexerUtility::ParseNumber(p, val))
	{
		// 解析に成功
		return (risse_int64)val; // val は文字列にはならないので再帰はしないはず
	}
	else
	{
		// 解析に失敗
		return (risse_int64)0;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVariant::CastToString_Integer  () const
{
	risse_char buf[40];
	::Risse::int64_to_str(AsInteger(), buf);
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVariant::CastToString_Real     () const
{
	risse_int32 cls = GetFPClass(AsReal());

	if(RISSE_FC_IS_NAN(cls))
	{
		return RISSE_WS("NaN");
	}
	if(RISSE_FC_IS_INF(cls))
	{
		if(RISSE_FC_IS_NEGATIVE(cls))
			return RISSE_WS("-Infinity");
		else
			return RISSE_WS("+Infinity");
	}
	if(AsReal() == 0.0)
	{
		if(RISSE_FC_IS_NEGATIVE(cls))
			return RISSE_WS("-0.0");
		else
			return RISSE_WS("0.0");
	}

	risse_char buf[27];
	::Risse::real_to_str(AsReal(), buf);
	if(!::Risse::strchr(buf, RISSE_WC('.')))
	{
		size_t len = ::Risse::strlen(buf);
		buf[len  ] = RISSE_WC('.');
		buf[len+1] = RISSE_WC('0');
		buf[len+2] = RISSE_WC('\0');
	}
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVariant::CastToString_Boolean  () const
{
	return CastToBoolean_Boolean()?
		RISSE_WS("true"):
		RISSE_WS("false");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOctet tVariant::CastToOctet_String   () const
{
	// String -> Octet 変換
	// 暗黙のこの変換においては、UTF-8 形式にて octet 列に変換を行う
	risse_size sz = 0;
	char * p = AsString().AsNarrowString(&sz);
	return tOctet(reinterpret_cast<risse_uint8*>(p), sz);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant::tSynchronizer::tSynchronizer(const tVariant & object)
{
	// この ASSERT は、 tObjectInterface::tSynchronizer のサイズが
	// Synchronizer のサイズ以下であることを保証する。
	// ヘッダの記述位置では tObjectInterface が
	// 定義済みかどうかが怪しい。とりあえず必要な分を確保しているはず。
	RISSE_ASSERT(sizeof(Synchronizer) >= sizeof(tObjectInterface::tSynchronizer));

	// 今のところ、ロックが出来るのは vtObject だけ。
	// vtObject 以外の場合はロックを行わない。
	// ロックオブジェクトを Synchronizer の場所に作成する
	tObjectInterface *intf;
	if(object.GetType() == tVariant::vtObject)
		intf = object.GetObjectInterface();
	else
		intf = NULL;

	new (reinterpret_cast<tObjectInterface::tSynchronizer*>(Synchronizer))
			tObjectInterface::tSynchronizer(intf);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant::tSynchronizer::~tSynchronizer()
{
	// ロックオブジェクトを消滅させる
	(reinterpret_cast<tObjectInterface::tSynchronizer*>(Synchronizer))->~tSynchronizer();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tVariant::GetHint_Data     () const
{
	return static_cast<risse_uint32>(
		GetPropertyDirect_Primitive(GetScriptEngine_Data(), ss_hint).CastToInteger());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tVariant::GetHint_Object   () const
{
	return static_cast<risse_uint32>(GetPropertyDirect_Object(ss_hint).CastToInteger());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::SetHint_Data     (risse_uint32 hint) const
{
	SetPropertyDirect_Primitive(GetScriptEngine_Data(), ss_hint, 0, (risse_int64)hint);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::SetHint_Object   (risse_uint32 hint) const
{
	SetPropertyDirect_Object(ss_hint, 0, (risse_int64)hint);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tVariant::GetHash_Integer  () const
{
	// ビット列を適当にハッシュして返す
	risse_uint64 iv = static_cast<risse_uint64>(AsInteger());
	risse_uint32 hash = static_cast<risse_uint32>(iv);
	hash += (hash << 10);
	hash ^= (hash >> 6);
	hash += static_cast<risse_uint32>(iv >> 11);
	hash += (hash << 10);
	hash ^= (hash >> 6);
	hash += static_cast<risse_uint32>(iv >> 32);
	hash += (hash << 10);
	hash ^= (hash >> 6);
	hash += (hash << 3);
	hash ^= (hash >> 10);
	hash += (hash << 15);
	return hash;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tVariant::GetHash_Real     () const
{
	// tVariant::StrictEqual_Real() も参照すること。

	risse_real v =     AsReal();
	risse_uint32 cls = GetFPClass(v);

	switch(cls & RISSE_FC_CLASS_MASK)
	{
	case RISSE_FC_CLASS_NORMAL:
		// 普通の数値
		{
			// ビット列を適当にハッシュして返す
			risse_uint64 iv = *reinterpret_cast<risse_uint64*>(&v);
			risse_uint32 hash = static_cast<risse_uint32>(iv);
			hash += (hash << 10);
			hash ^= (hash >> 6);
			hash += static_cast<risse_uint32>(iv >> 11);
			hash += (hash << 10);
			hash ^= (hash >> 6);
			hash += static_cast<risse_uint32>(iv >> 32);
			hash += (hash << 10);
			hash ^= (hash >> 6);
			hash += (hash << 3);
			hash ^= (hash >> 10);
			hash += (hash << 15);
			return hash;
		}

	case RISSE_FC_CLASS_NAN:
		// NaN の場合は符号は無視する
		return RISSE_FC_CLASS_NAN;

	case RISSE_FC_CLASS_INF:
		return RISSE_FC_IS_NEGATIVE(cls) ? 3 : 2;
	}

	return ~static_cast<risse_uint32>(0);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tVariant::GetHash_Data     () const
{
	return static_cast<risse_uint32>(
		GetPropertyDirect_Primitive(GetScriptEngine_Data(), ss_hash).CastToInteger());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tVariant::GetHash_Object   () const
{
	return static_cast<risse_uint32>(GetPropertyDirect_Object(ss_hash).CastToInteger());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVariant::AsHumanReadable_Void     (risse_size maxlen) const
{
	return RISSE_WS("void");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVariant::AsHumanReadable_Null     (risse_size maxlen) const
{
	return RISSE_WS("null");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::AddTrace(const tScriptBlockInstance * sb, risse_size pos) const
{
	if(sb != NULL && pos != risse_size_max)
	{
		RISSE_ASSERT(GetType() == vtObject);
		tScriptEngine * engine = this->GetObjectInterface()->GetRTTI()->GetScriptEngine();
		tVariant source_point = tVariant(engine->SourcePointClass).New(0,
			tMethodArgument::New(
				tVariant(const_cast<tScriptBlockInstance *>(sb)),
				(risse_int64)pos));
		Invoke_Object(ss_addTrace, source_point);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::PrependMessage(const tString & message) const
{
	RISSE_ASSERT(GetType() == vtObject);
	SetPropertyDirect_Object(ss_message, 0,
		tVariant(message + (tString)GetPropertyDirect_Object(ss_message)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::AssertClass(tClassBase * cls) const
{
	// CheckClass も同じ構造をしているので、修正の際はよく見比べること。
	if(GetType() == vtObject)
	{
		if(!cls->GetRTTIMatcher().Match(GetObjectInterface()->GetRTTI()))
			ThrowIllegalArgumentClassException(cls->GetPropertyDirect(ss_name));
	}
	else
	{
		if(!InstanceOf(cls->GetRTTI()->GetScriptEngine(), tVariant((tObjectInterface*)cls)))
			ThrowIllegalArgumentClassException(cls->GetPropertyDirect(ss_name));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVariant::GetClassName(bool * got) const
{
	// class.name を得る
	tType type = GetType();
	switch(type)
	{
	case vtVoid:		if(got) *got=true; return ss_Void;
	case vtInteger:		if(got) *got=true; return ss_Integer;
	case vtReal:		if(got) *got=true; return ss_Real;
	case vtNull:		if(got) *got=true; return ss_Null;
	case vtString:		if(got) *got=true; return ss_String;
	case vtBoolean:		if(got) *got=true; return ss_Boolean;
	case vtOctet:		if(got) *got=true; return ss_Octet;
	case vtData:
	case vtObject:
		{
			tVariant name;
			try
			{
				tVariant Class;
				tObjectInterface * intf;
				if(type == vtObject)
				{
					intf = GetObjectInterface();
					// class を得る
					if(intf->Operate(ocDGet, &Class, ss_class) !=
						tOperateRetValue::rvNoError)
					{
						if(got) *got = false;
						return tSS<'<','u','n','k','n','o','w','n','>'>();
					}
				}
				else if(type == vtData)
				{
					Class = GetDataClassInstance();
				}

				// class.name を得る
				if(Class.GetType() != vtObject)
				{
					if(got) *got = false;
					return tSS<'<','u','n','k','n','o','w','n','>'>();
				}
				intf = Class.GetObjectInterface();
				if(intf->Operate(ocDGet, &name, ss_name) !=
					tOperateRetValue::rvNoError)
				{
					if(got) *got = false;
					return tSS<'<','u','n','k','n','o','w','n','>'>();
				}
			}
			catch(...)
			{
				// 例外が発生
				// 型名は不明であると判断する
				if(got) *got = false;
				return tSS<'<','u','n','k','n','o','w','n','>'>();
			}
			tString str = name.operator tString();
			if(str.IsEmpty()) str = tSS<'<','a','n','o','n','y','m','o','u','s','>'>();
			return str;
		}
	}
	return tSS<'<','u','n','k','n','o','w','n','>'>();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::DebugDump() const
{
	if(GetType() == tVariant::vtObject)
	{
		risse_char buf[40];
		::Risse::pointer_to_str(GetObjectInterface(), buf);
		FPrint(stderr, (tString(RISSE_WS("Object@")) + buf).c_str());
		const tVariant * context = GetContext();
		if(context)
		{
			if(context->GetType() == tVariant::vtObject)
			{
				if(context == GetDynamicContext())
				{
					FPrint(stderr, RISSE_WS(":dynamic"));
				}
				else
				{
					::Risse::pointer_to_str(context->GetObjectInterface(), buf);
					FPrint(stderr, (tString(RISSE_WS(":")) + buf).c_str());
				}
			}
			else
			{
				FPrint(stderr, (context->AsHumanReadable()).c_str());
			}
		}
	}
	else
	{
		FPrint(stderr, AsHumanReadable().c_str());
	}

	FPrint(stderr, RISSE_WS("\n"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::RegisterMember(const tString & name, const tVariant & value,
	tMemberAttribute attrib, risse_uint32 flags) const
{
	RISSE_ASSERT(GetType() == vtObject);
	SetPropertyDirect_Object(name,
		flags|tOperateFlags::ofMemberEnsure|
			(risse_uint32)(tMemberAttribute::GetDefault()),
		value);
	SetAttributeDirect_Object(name, (risse_uint32)attrib);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariant::RegisterFinalConstMember(const tString & name,
	const tVariant & value, risse_uint32 flags) const
{
	RegisterMember(name, value,
		tMemberAttribute::GetDefault()
			.Set(tMemberAttribute::mcConst)
			.Set(tMemberAttribute::ocFinal),
		flags);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
