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
void tVariantBlock::ThrowCannotCreateInstanceFromNonClassObjectException()
{
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromNonClassObject();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::ThrowBadContextException()
{
	tBadContextExceptionClass::Throw();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::ThrowNoSuchMemberException(const tString &name)
{
	tNoSuchMemberExceptionClass::Throw(name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::ThrowIllegalArgumentClassException(const tString & class_name)
{
	tIllegalArgumentClassExceptionClass::ThrowSpecifyInstanceOfClass(class_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::ThrowIllegalOperationMethod(const tString & method_name) const
{
	tIllegalArgumentClassExceptionClass::ThrowIllegalOperationMethod(
			GetClassName() + ss_doubleColon + (method_name));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// voidオブジェクトへのconst参照を保持するオブジェクト
// これのバイナリレイアウトはtVariantBlockと同一でなければならない
tVariantBlock::tStaticPrimitive tVariantBlock::VoidObject = {
	tVariantBlock::vtVoid,
	{0}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// nullオブジェクトへのconst参照を保持するオブジェクト
// これのバイナリレイアウトはtVariantBlockと同一でなければならない
tVariantBlock::tStaticPrimitive tVariantBlock::NullObject = {
	tVariantBlock::vtNull,
	{0}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// DynamicContextオブジェクトへのconst参照を保持するオブジェクト
// これのバイナリレイアウトはtVariantBlockと同一でなければならない
static tIdentifyObject DynamicContextObject;
tVariantBlock::tStaticObject tVariantBlock::DynamicContext = {
	reinterpret_cast<risse_ptruint>(&DynamicContextObject) + tVariantBlock::ObjectPointerBias, NULL,
	{0}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const risse_char * tVariantBlock::GetTypeString(tType type)
{
	switch(type)
	{
	case vtVoid:		return RISSE_WS("void");
	case vtInteger:		return RISSE_WS("integer");
	case vtReal:		return RISSE_WS("real");
	case vtNull:		return RISSE_WS("null");
	case vtString:		return RISSE_WS("string");
	case vtObject:		return RISSE_WS("object");
	case vtBoolean:		return RISSE_WS("boolean");
	case vtOctet:		return RISSE_WS("octet");
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::SetTypeTag(tType type)
{
	switch(type)
	{
	case vtVoid:		Clear();							RISSE_ASSERT(GetType()==vtVoid);    return;
	case vtInteger:		*this = (risse_int64)0;				RISSE_ASSERT(GetType()==vtInteger); return;
	case vtReal:		*this = (risse_real)0.0;			RISSE_ASSERT(GetType()==vtReal);    return;
	case vtNull:		Nullize();							RISSE_ASSERT(GetType()==vtNull);    return;
	case vtString:		*this = tString::GetEmptyString();	RISSE_ASSERT(GetType()==vtString);  return;
	case vtObject:		*this = *GetDynamicContext();		RISSE_ASSERT(GetType()==vtObject);  return;
	case vtBoolean:		*this = false;						RISSE_ASSERT(GetType()==vtBoolean); return;
	case vtOctet:		*this = tOctet();					RISSE_ASSERT(GetType()==vtOctet);   return;
	}
	return;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const risse_char * tVariantBlock::GetGuessTypeString(tGuessType type)
{
	switch(type)
	{
	case gtVoid:		return RISSE_WS("Void");
	case gtInteger: 	return RISSE_WS("Integer");
	case gtReal:		return RISSE_WS("Real");
	case gtNull:		return RISSE_WS("Null");
	case gtString:		return RISSE_WS("String");
	case gtOctet:		return RISSE_WS("Object");
	case gtBoolean:		return RISSE_WS("Boolean");
	case gtObject:		return RISSE_WS("Octet");

	case gtAny:
		return RISSE_WS("any");
	case gtError:
		return RISSE_WS("error");
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
inline tPrimitiveClassBase * tVariantBlock::GetPrimitiveClass(tScriptEngine * engine) const
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
	default:		break;
	}
	RISSE_ASSERT(Class != NULL);
	return Class;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::SetContext(const tVariantBlock &context)
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
tVariantBlock::tRetValue
	tVariantBlock::OperateForMember(tScriptEngine * engine, RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	switch(GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtString:
	case vtOctet:
	case vtBoolean:
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
				// ならない。プリミティブ型は immutable とはいえ、内部実装は
				// 完全に mutable なので、この時点での実行結果を保存しておくために
				// new でオブジェクトを再確保し、固定する。
				// TODO: 返りのオブジェクトがコンテキストを持ってないと、毎回newが行われて
				// しまう。関数やプロパティ以外はコンテキストを伴う必要はないので
				// 努めて関数やプロパティ以外はダミーでもよいからコンテキストを
				// 設定するようにするべき。
				if(!result->HasContext() && !(flags & tOperateFlags::ofUseClassMembersRule))
					result->SetContext(new tVariantBlock(*this));
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
tVariantBlock tVariantBlock::GetPropertyDirect_Primitive(tScriptEngine * engine, const tString & name, risse_uint32 flags, const tVariant & This) const
{
	tVariantBlock result;
	GetPrimitiveClass(engine)->GetGateway().Do(engine, ocDGet, &result, name,
				flags | tOperateFlags::ofUseClassMembersRule,
					// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
					// また、ゲートウェイの members の中から探す
				tMethodArgument::Empty(), *this); // 動作コンテキストは常に *this
	// コンテキストを設定する
	// ここの長ったらしい説明は tVariantBlock::OperateForMember() 内を参照
	if(!result.HasContext() && !(flags & tOperateFlags::ofUseClassMembersRule))
		result.SetContext(new tVariantBlock(*this));
	return result;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::GetPropertyDirect_Object  (const tString & name, risse_uint32 flags, const tVariant & This) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	tVariantBlock ret;
	intf->Do(ocDGet, &ret, name,
		flags, tMethodArgument::Empty(),
		SelectContext(flags, This)
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::SetPropertyDirect_Primitive(tScriptEngine * engine,
	const tString & name, risse_uint32 flags, const tVariantBlock & value,
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
void tVariantBlock::SetPropertyDirect_Object  (const tString & name,
		risse_uint32 flags, const tVariantBlock & value,
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
void tVariantBlock::DeletePropertyDirect_Primitive(tScriptEngine * engine, const tString & name, risse_uint32 flags) const
{
	GetPrimitiveClass(engine)->GetGateway().Do(engine, ocDDelete, NULL, name,
				flags | tOperateFlags::ofUseClassMembersRule,
					// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
					// また、ゲートウェイの members の中から探す
				tMethodArgument::Empty(), *this); // 動作コンテキストは常に *this
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::DeletePropertyDirect_Object   (const tString & name, risse_uint32 flags) const
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
void tVariantBlock::SetAttributeDirect_Object  (const tString & key, risse_uint32 attrib) const
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
void tVariantBlock::FuncCall(tScriptEngine * engine, tVariantBlock * ret, risse_uint32 flags,
	const tMethodArgument & args,
	const tVariant & This) const
{
	switch(GetType())
	{
	case vtVoid:
	case vtInteger:
	case vtReal:
	case vtNull:
	case vtString:
	case vtOctet:
	case vtBoolean:
		FuncCall_Primitive(engine, ret, tString::GetEmptyString(), flags, args, This); return;
	case vtObject:
		FuncCall_Object   (        ret, tString::GetEmptyString(), flags, args, This); return;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::FuncCall_Primitive(tScriptEngine * engine, 
	tVariantBlock * ret, const tString & name,
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
void tVariantBlock::FuncCall_Object  (
	tVariantBlock * ret, const tString & name, risse_uint32 flags,
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
tVariantBlock tVariantBlock::Invoke_Primitive(tScriptEngine * engine, const tString & membername) const
{
	tVariantBlock ret;
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
tVariantBlock tVariantBlock::Invoke_Object   (const tString & membername) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	tVariantBlock ret;
	intf->Do(ocFuncCall, &ret, membername,
		0, 
		tMethodArgument::Empty(),
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Invoke_Primitive(tScriptEngine * engine, const tString & membername,const tVariant & arg1) const
{
	tVariantBlock ret;
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
tVariantBlock tVariantBlock::Invoke_Object   (const tString & membername,const tVariant & arg1) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	tVariantBlock ret;
	intf->Do(ocFuncCall, &ret, membername,
		0, 
		tMethodArgument::New(arg1),
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Invoke_Primitive(tScriptEngine * engine, const tString & membername,const tVariant & arg1,const tVariant & arg2) const
{
	tVariantBlock ret;
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
tVariantBlock tVariantBlock::Invoke_Object   (const tString & membername,const tVariant & arg1,const tVariant & arg2) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	tVariantBlock ret;
	intf->Do(ocFuncCall, &ret, membername,
		0, 
		tMethodArgument::New(arg1, arg2),
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::New_Object  (const tString & name,
	risse_uint32 flags, const tMethodArgument & args) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tObjectInterface * intf = GetObjectInterface();
	tVariantBlock ret;
	intf->Do(ocNew, &ret, name,
		flags,
		args,
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Plus_String   () const
{
	tVariantBlock val;
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
tVariantBlock tVariantBlock::BitOr_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0;
	case vtInteger:	return rhs.CastToInteger_Integer(); // void | integer
	case vtReal:	return rhs.CastToInteger_Real(); // void | real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitOr_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitOr_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger(); // integer | void
	case vtInteger:	return AsInteger() | rhs.CastToInteger_Integer(); // integer | integer
	case vtReal:	return AsInteger() | rhs.CastToInteger_Real(); // integer | real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitOr_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitOr_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToInteger_Real(); // real | void
	case vtInteger:	return CastToInteger_Real() | rhs.CastToInteger_Integer(); // real | integer
	case vtReal:	return CastToInteger_Real() | rhs.CastToInteger_Real(); // real | real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitOr_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitOr_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitOr_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitOr_String   (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitOr_String     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitOr_Octet    (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitOr_Octet     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitOr_Boolean  (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitOr_Boolean     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitXor_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0;
	case vtInteger:	return rhs.CastToInteger_Integer(); // void ^ integer
	case vtReal:	return rhs.CastToInteger_Real(); // void ^ real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitXor_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitXor_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger(); // integer ^ void
	case vtInteger:	return AsInteger() ^ rhs.CastToInteger_Integer(); // integer ^ integer
	case vtReal:	return AsInteger() ^ rhs.CastToInteger_Real(); // integer ^ real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitXor_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitXor_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToInteger_Real(); // real | void
	case vtInteger:	return CastToInteger_Real() ^ rhs.CastToInteger_Integer(); // real | integer
	case vtReal:	return CastToInteger_Real() ^ rhs.CastToInteger_Real(); // real | real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitXor_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitXor_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitXor_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitXor_String   (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitXor_String     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitXor_Octet    (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitXor_Octet     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitXor_Boolean  (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitXor_Boolean     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitAnd_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0;
	case vtInteger:	return (risse_int64)0; // void & integer
	case vtReal:	return (risse_int64)0; // void & real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitAnd_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitAnd_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // integer & void
	case vtInteger:	return AsInteger() & rhs.CastToInteger_Integer(); // integer & integer
	case vtReal:	return AsInteger() & rhs.CastToInteger_Real(); // integer & real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitAnd_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitAnd_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // real & void
	case vtInteger:	return CastToInteger_Real() & rhs.CastToInteger_Integer(); // real & integer
	case vtReal:	return CastToInteger_Real() & rhs.CastToInteger_Real(); // real & real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitAnd_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitAnd_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitAnd_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitAnd_String   (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitAnd_String     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitAnd_Octet    (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitAnd_Octet     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::BitAnd_Boolean  (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeBitAnd_Boolean     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Equal_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return true; // void == void
	case vtInteger:	return CastToInteger_Void() == rhs.AsInteger();
	case vtReal:	return CastToReal_Void()    == rhs.AsReal();
	case vtNull:	return false; // void == null
	case vtString:	return rhs.AsString().IsEmpty();
	case vtOctet:	return rhs.AsOctet().IsEmpty();
	case vtBoolean:	return CastToBoolean_Void() == rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Equal_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger() == 0;
	case vtInteger:	return AsInteger() == rhs.AsInteger();
	case vtReal:	return AsInteger() == rhs.AsReal();
	case vtNull:	return false; // 数値とnullの比較は常に偽
	case vtString:	return AsInteger() == rhs.CastToInteger_String();
	case vtOctet:	return false; // 数値とoctetの比較は常に偽
	case vtBoolean:	return CastToBoolean_Integer() == rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Equal_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsReal() == 0.0;
	case vtInteger:	return AsReal() == rhs.AsInteger();
	case vtReal:	return AsReal() == rhs.AsReal();
	case vtNull:	return false; // 数値とnullの比較は常に偽
	case vtString:	return AsReal() == rhs.CastToReal_String();
	case vtOctet:	return false; // 数値とoctetの比較は常に偽
	case vtBoolean:	return CastToBoolean_Real() == rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Equal_Null     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void == void
	case vtInteger:	return false;
	case vtReal:	return false;
	case vtNull:	return true;
	case vtString:	return false;
	case vtOctet:	return false;
	case vtBoolean:	return rhs.CastToBoolean_Boolean() == false; // 偽とnullの比較は真
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Equal_String   (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsString().IsEmpty();
	case vtInteger:	return CastToInteger_String() == rhs.AsInteger();
	case vtReal:	return CastToReal_String() == rhs.AsReal();
	case vtNull:	return false; // 文字列と null の比較は常に偽
	case vtString:	return AsString() == rhs.AsString();
	case vtOctet:	return false; // 文字列とoctetの比較は常に偽
	case vtBoolean:	return CastToBoolean_String() == rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Equal_Octet    (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsOctet().IsEmpty();
	case vtInteger:	return false; // オクテット列と integer の比較は常に偽
	case vtReal:	return false; // オクテット列と real の比較は常に偽
	case vtNull:	return false; // オクテット列と null の比較は常に偽
	case vtString:	return false; // オクテット列と string の比較は常に偽
	case vtOctet:	return AsOctet() == rhs.AsOctet();
	case vtBoolean:	return CastToBoolean_Octet() == rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Equal_Boolean  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToBoolean_Boolean() == 0.0;
	case vtInteger:	return CastToBoolean_Boolean() == rhs.AsInteger();
	case vtReal:	return CastToBoolean_Boolean() == rhs.AsReal();
	case vtNull:	return CastToBoolean_Boolean() == false; // false と null の比較ならば真
	case vtString:	return CastToBoolean_Boolean() == rhs.CastToBoolean_String();
	case vtOctet:	return CastToBoolean_Boolean() == rhs.CastToBoolean_Octet();
	case vtBoolean:	return CastToBoolean_Boolean() == rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::StrictEqual_Real     (const tVariantBlock & rhs) const
{
	// Real::identify メソッドの実装はこれがつかわれるので、
	// hash プロパティと齟齬が生じないようにすること。
	// このソースファイルの
	// tVariantBlock::GetHash_Real() も参照のこと。

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
bool tVariantBlock::Identify_Object   (const tVariantBlock & rhs) const
{
	return Invoke_Object(ss_identify, rhs).CastToBoolean();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Lesser_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void < void
	case vtInteger:	return CastToInteger_Void() < rhs.AsInteger();
	case vtReal:	return CastToReal_Void()    < rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	return !rhs.AsString().IsEmpty();
	case vtOctet:	return !rhs.AsOctet().IsEmpty();
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLesser_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtBoolean;
	case gtOctet:		return gtBoolean;
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Lesser_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger() < 0;
	case vtInteger:	return AsInteger() < rhs.AsInteger();
	case vtReal:	return AsInteger() < rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLesser_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Lesser_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsReal() < 0.0;
	case vtInteger:	return AsReal() < rhs.AsInteger();
	case vtReal:	return AsReal() < rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLesser_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Lesser_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLesser_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Lesser_String   (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void は空文字列と見なす
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	return AsString() < rhs.AsString();
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLesser_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtBoolean;
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Lesser_Octet    (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void は空オクテット列と見なす
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtOctet:	return AsOctet() < rhs.AsOctet();
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLesser_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtBoolean;
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Lesser_Boolean  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesser);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLesser_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Greater_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void > void
	case vtInteger:	return CastToInteger_Void() > rhs.AsInteger();
	case vtReal:	return CastToReal_Void()    > rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	return false;
	case vtOctet:	return false;
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeGreater_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtBoolean;
	case gtOctet:		return gtBoolean;
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Greater_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger() > 0;
	case vtInteger:	return AsInteger() > rhs.AsInteger();
	case vtReal:	return AsInteger() > rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeGreater_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Greater_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsReal() > 0.0;
	case vtInteger:	return AsReal() > rhs.AsInteger();
	case vtReal:	return AsReal() > rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeGreater_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtBoolean;
	case gtReal:		return gtBoolean;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Greater_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeGreater_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Greater_String   (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !AsString().IsEmpty();
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	return AsString() > rhs.AsString();
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeGreater_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtBoolean;
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Greater_Octet    (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !AsOctet().IsEmpty();
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtOctet:	return AsOctet() > rhs.AsOctet();
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeGreater_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtBoolean;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtBoolean;
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtBoolean|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::Greater_Boolean  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreater);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeGreater_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::LesserOrEqual_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void < void
	case vtInteger:	return !(CastToInteger_Void() > rhs.AsInteger());
	case vtReal:	return !(CastToReal_Void()    > rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	return !false;
	case vtOctet:	return !false;
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::LesserOrEqual_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsInteger() > 0);
	case vtInteger:	return !(AsInteger() > rhs.AsInteger());
	case vtReal:	return !(AsInteger() > rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::LesserOrEqual_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsReal() > 0.0);
	case vtInteger:	return !(AsReal() > rhs.AsInteger());
	case vtReal:	return !(AsReal() > rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::LesserOrEqual_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::LesserOrEqual_String   (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(!AsString().IsEmpty());
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	return !(AsString() > rhs.AsString());
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::LesserOrEqual_Octet    (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(!AsOctet().IsEmpty());
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtOctet:	return !(AsOctet() > rhs.AsOctet());
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::LesserOrEqual_Boolean  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnLesserOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::GreaterOrEqual_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void > void
	case vtInteger:	return !(CastToInteger_Void() < rhs.AsInteger());
	case vtReal:	return !(CastToReal_Void()    < rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	return !(!rhs.AsString().IsEmpty());
	case vtOctet:	return !(!rhs.AsOctet().IsEmpty());
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::GreaterOrEqual_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsInteger() < 0);
	case vtInteger:	return !(AsInteger() < rhs.AsInteger());
	case vtReal:	return !(AsInteger() < rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::GreaterOrEqual_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsReal() < 0.0);
	case vtInteger:	return !(AsReal() < rhs.AsInteger());
	case vtReal:	return !(AsReal() < rhs.AsReal());
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::GreaterOrEqual_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::GreaterOrEqual_String   (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void は空文字列と見なす
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	return !(AsString() < rhs.AsString());
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::GreaterOrEqual_Octet    (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void は空オクテット列と見なす
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtOctet:	return !(AsOctet() < rhs.AsOctet());
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::GreaterOrEqual_Boolean  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE_B(mnGreaterOrEqual);
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RBitShift_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // void >>> void
	case vtInteger:	return (risse_int64)0; // void >>> integer
	case vtReal:	return (risse_int64)0; // void >>> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRBitShift_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RBitShift_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger(); // integer >>> void
	case vtInteger:	return (risse_int64)((risse_uint64)AsInteger() >> rhs.CastToInteger_Integer()); // integer >>> integer
	case vtReal:	return (risse_int64)((risse_uint64)AsInteger() >> rhs.CastToInteger_Real()); // integer >>> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRBitShift_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RBitShift_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToInteger_Real(); // real >>> void
	case vtInteger:	return (risse_int64)((risse_uint64)CastToInteger_Real() >> rhs.CastToInteger_Integer()); // real >>> integer
	case vtReal:	return (risse_int64)((risse_uint64)CastToInteger_Real() >> rhs.CastToInteger_Real()); // real >>> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRBitShift_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RBitShift_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRBitShift_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RBitShift_String   (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRBitShift_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RBitShift_Octet    (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRBitShift_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RBitShift_Boolean  (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRBitShift_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::LShift_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // void << void
	case vtInteger:	return (risse_int64)0; // void << integer
	case vtReal:	return (risse_int64)0; // void << real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLShift_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::LShift_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger(); // integer << void
	case vtInteger:	return AsInteger() << rhs.CastToInteger_Integer(); // integer << integer
	case vtReal:	return AsInteger() << rhs.CastToInteger_Real(); // integer << real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLShift_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::LShift_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToInteger_Real(); // real << void
	case vtInteger:	return CastToInteger_Real() << rhs.CastToInteger_Integer(); // real << integer
	case vtReal:	return CastToInteger_Real() << rhs.CastToInteger_Real(); // real << real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLShift_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::LShift_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLShift_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::LShift_String   (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLShift_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::LShift_Octet    (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLShift_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::LShift_Boolean  (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeLShift_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RShift_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // void >> void
	case vtInteger:	return (risse_int64)0; // void >> integer
	case vtReal:	return (risse_int64)0; // void >> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRShift_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RShift_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger(); // integer >> void
	case vtInteger:	return AsInteger() >> rhs.CastToInteger_Integer(); // integer >> integer
	case vtReal:	return AsInteger() >> rhs.CastToInteger_Real(); // integer >> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRShift_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RShift_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return CastToInteger_Real(); // real >> void
	case vtInteger:	return CastToInteger_Real() >> rhs.CastToInteger_Integer(); // real >> integer
	case vtReal:	return CastToInteger_Real() >> rhs.CastToInteger_Real(); // real >> real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRShift_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RShift_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRShift_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RShift_String   (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRShift_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RShift_Octet    (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRShift_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::RShift_Boolean  (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeRShift_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mod_Void     (const tVariantBlock & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtInteger:	rhs_value = rhs.AsInteger();						goto do_div;
	case vtReal:	rhs_value = static_cast<risse_int64>(rhs.AsReal());	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariantBlock();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)0 % rhs_value; // void % integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMod_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger;
	case gtReal:		return gtInteger;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mod_Integer  (const tVariantBlock & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tArithmeticExceptionClass::ThrowDivideByZeroException(); return tVariantBlock();
	case vtInteger:	rhs_value = rhs.AsInteger();						goto do_div;
	case vtReal:	rhs_value = static_cast<risse_int64>(rhs.AsReal());	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariantBlock();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return AsInteger() % rhs_value; // integer % integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMod_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtReal:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mod_Real     (const tVariantBlock & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tArithmeticExceptionClass::ThrowDivideByZeroException(); return tVariantBlock();
	case vtInteger:	rhs_value = rhs.AsInteger();			goto do_div;
	case vtReal:	rhs_value = (risse_int64)rhs.AsReal();	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariantBlock();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)AsReal() % rhs_value; // real % integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMod_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtReal:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mod_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMod_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mod_String   (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMod_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mod_Octet    (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMod_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mod_Boolean  (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMod_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Div_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtInteger:	return (risse_real)0.0 / rhs.AsInteger(); // void / integer
	case vtReal:	return (risse_real)0.0 / rhs.AsReal(); // void / real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeDiv_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtReal|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Div_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_real)AsInteger() / rhs.CastToReal_Void(); // integer / void
	case vtInteger:	return (risse_real)AsInteger() / rhs.AsInteger(); // integer / integer
	case vtReal:	return (risse_real)AsInteger() / rhs.AsReal(); // integer / real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeDiv_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtReal;
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtReal	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Div_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsReal() / rhs.CastToReal_Void(); // real / void
	case vtInteger:	return AsReal() / rhs.AsInteger(); // real / integer
	case vtReal:	return AsReal() / rhs.AsReal(); // real / real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeDiv_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtReal;
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtReal	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Div_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeDiv_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Div_String   (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeDiv_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Div_Octet    (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeDiv_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Div_Boolean  (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeDiv_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Idiv_Void     (const tVariantBlock & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtInteger:	rhs_value = rhs.AsInteger();						goto do_div;
	case vtReal:	rhs_value = static_cast<risse_int64>(rhs.AsReal());	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariantBlock();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)0 / rhs_value; // void / integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeIdiv_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtReal:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Idiv_Integer  (const tVariantBlock & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tArithmeticExceptionClass::ThrowDivideByZeroException(); return tVariantBlock();
	case vtInteger:	rhs_value = rhs.AsInteger();						goto do_div;
	case vtReal:	rhs_value = static_cast<risse_int64>(rhs.AsReal());	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariantBlock();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return AsInteger() / rhs_value; // integer / integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeIdiv_Integer     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtReal:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Idiv_Real     (const tVariantBlock & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tArithmeticExceptionClass::ThrowDivideByZeroException(); return tVariantBlock();
	case vtInteger:	rhs_value = rhs.AsInteger();			goto do_div;
	case vtReal:	rhs_value = (risse_int64)rhs.AsReal();	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariantBlock();

do_div:
	if(rhs_value == 0) tArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)AsReal() / rhs_value; // real / integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeIdiv_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtReal:		return gtInteger|gtEffective;	// 例外が発生する可能性があるため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtInteger|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Idiv_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeIdiv_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Idiv_String   (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeIdiv_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Idiv_Octet    (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeIdiv_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Idiv_Boolean  (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeIdiv_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mul_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return tVariant::GetVoidObject(); // void * void
	case vtInteger:	return (risse_int64)0; // void * integer
	case vtReal:	return (risse_real)0.0;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMul_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtVoid;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mul_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // integer * void
	case vtInteger:	return AsInteger() * rhs.AsInteger(); // integer * integer
	case vtReal:	return AsInteger() * rhs.AsReal(); // integer * real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMul_Integer   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mul_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_real)0; // real * void
	case vtInteger:	return AsReal() * rhs.AsInteger(); // real * integer
	case vtReal:	return AsReal() * rhs.AsReal(); // real * real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMul_Real   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtReal;
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mul_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMul_Null   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mul_String   (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMul_String   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mul_Octet    (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMul_Octet   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Mul_Boolean  (const tVariantBlock & rhs) const
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
	case vtObject:
		RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeMul_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Add_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return tVariantBlock(); // void + void = void かなぁ
	case vtInteger:	return rhs; // void + integer
	case vtReal:	return rhs;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	return rhs;
	case vtOctet:	return rhs;
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeAdd_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtVoid;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtString;
	case gtOctet:		return gtOctet;
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Add_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this;
	case vtInteger:	return AsInteger() + rhs.AsInteger();
	case vtReal:	return AsInteger() + rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeAdd_Integer  (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtInteger;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Add_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this;
	case vtInteger:	return AsReal() + rhs.AsInteger();
	case vtReal:	return AsReal() + rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeAdd_Real     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtReal;
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Add_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeAdd_Null     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Add_String   (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this;
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	return AsString() + rhs.AsString();
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeAdd_String    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtString;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtString;
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Add_Octet    (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this;
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtOctet:	return AsOctet() + rhs.AsOctet();
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeAdd_Octet    (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtOctet;
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtOctet;
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Add_Boolean  (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeAdd_Boolean  (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Sub_Void     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return tVariant::GetVoidObject(); // void - void = void かなぁ
	case vtInteger:	return - rhs.AsInteger(); // void - integer
	case vtReal:	return - rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeSub_Void     (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtVoid;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Sub_Integer  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this; // integer - void
	case vtInteger:	return AsInteger() - rhs.AsInteger(); // integer - integer
	case vtReal:	return AsInteger() - rhs.AsReal(); // integer - real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeSub_Integer   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtVoid;
	case gtInteger:		return gtInteger;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Sub_Real     (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this; // real - void
	case vtInteger:	return AsReal() - rhs.AsInteger(); // real - integer
	case vtReal:	return AsReal() - rhs.AsReal(); // real - real
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeSub_Real   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtVoid;
	case gtInteger:		return gtReal;
	case gtReal:		return gtReal;
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtAny	|gtEffective;

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Sub_Null     (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeSub_Null   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtError|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Sub_Octet    (const tVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeSub_Octet   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Sub_String   (const tVariantBlock & rhs) const
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
			RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeSub_String   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock tVariantBlock::Sub_Boolean  (const tVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtInteger:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtReal:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tVariantBlock::GuessTypeSub_Boolean   (tGuessType r)
{
	switch(r)
	{
	case gtVoid:		return gtError	|gtEffective;	// 例外が発生するため
	case gtInteger:		return gtError	|gtEffective;	// 例外が発生するため
	case gtReal:		return gtError	|gtEffective;	// 例外が発生するため
	case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
	case gtString:		return gtError	|gtEffective;	// 例外が発生するため
	case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
	case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
	case gtObject:		return gtError	|gtEffective;	// 例外が発生するため
	case gtAny:			return gtError	|gtEffective;	// 必ず例外が発生する

	default:		return gtAny|gtEffective;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tVariantBlock::InstanceOf(tScriptEngine * engine, const tVariantBlock & rhs) const
{
	// this の class を得る
	tVariant Class = GetPropertyDirect(engine, ss_class, tOperateFlags::ofInstanceMemberOnly, *this);

	RISSE_ASSERT(Class.GetType() == vtObject);

	tVariantBlock ret;
	Class.GetObjectInterface()->Do(ocInstanceOf, &ret, tString::GetEmptyString(), 0,
			tMethodArgument::New(rhs), Class);
	return ret.operator bool();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int64 tVariantBlock::CastToInteger_String   () const
{
	tVariantBlock val;
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
tString tVariantBlock::CastToString_Integer  () const
{
	risse_char buf[40];
	::Risse::int64_to_str(AsInteger(), buf);
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVariantBlock::CastToString_Real     () const
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
tString tVariantBlock::CastToString_Boolean  () const
{
	return CastToBoolean_Boolean()?
		RISSE_WS("true"):
		RISSE_WS("false");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOctet tVariantBlock::CastToOctet_String   () const
{
	// String -> Octet 変換
	// 暗黙のこの変換においては、UTF-8 形式にて octet 列に変換を行う
	risse_size sz = 0;
	char * p = AsString().AsNarrowString(&sz);
	return tOctet(reinterpret_cast<risse_uint8*>(p), sz);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock::tSynchronizer::tSynchronizer(const tVariant & object)
{
	// この ASSERT は、 tObjectInterface::tSynchronizer のサイズが
	// Synchronizer のサイズ以下であることを保証する。
	// ヘッダの記述位置では tObjectInterface も tCriticalSection も
	// 定義済みかどうかが怪しい。とりあえず sizeof(void*) の分を確保しているが、
	// tCriticalSection::tLocker の実装を見る限りは大丈夫なはず。
	RISSE_ASSERT(sizeof(Synchronizer) >= sizeof(tObjectInterface::tSynchronizer));

	// 今のところ、ロックが出来るのは vtObject だけ。
	// vtObject 以外の場合はロックを行わない。
	// ロックオブジェクトを Synchronizer の場所に作成する
	tObjectInterface *intf;
	if(object.GetType() == tVariantBlock::vtObject)
		intf = object.GetObjectInterface();
	else
		intf = NULL;

	new (reinterpret_cast<tObjectInterface::tSynchronizer*>(Synchronizer))
			tObjectInterface::tSynchronizer(intf);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariantBlock::tSynchronizer::~tSynchronizer()
{
	// ロックオブジェクトを消滅させる
	(reinterpret_cast<tObjectInterface::tSynchronizer*>(Synchronizer))->~tSynchronizer();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tVariantBlock::GetHint_Object   () const
{
	return static_cast<risse_uint32>(GetPropertyDirect_Object(ss_hint).CastToInteger());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::SetHint_Object   (risse_uint32 hint) const
{
	SetPropertyDirect_Object(ss_hint, 0, (risse_int64)hint);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tVariantBlock::GetHash_Integer  () const
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
risse_uint32 tVariantBlock::GetHash_Real     () const
{
	// tVariantBlock::StrictEqual_Real() も参照すること。

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
risse_uint32 tVariantBlock::GetHash_Object   () const
{
	return static_cast<risse_uint32>(GetPropertyDirect_Object(ss_hash).CastToInteger());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVariantBlock::AsHumanReadable_Void     (risse_size maxlen) const
{
	return RISSE_WS("void");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVariantBlock::AsHumanReadable_Null     (risse_size maxlen) const
{
	return RISSE_WS("null");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::AddTrace(const tScriptBlockInstance * sb, risse_size pos) const
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
void tVariantBlock::PrependMessage(const tString & message) const
{
	RISSE_ASSERT(GetType() == vtObject);
	SetPropertyDirect_Object(ss_message, 0,
		tVariant(message + (tString)GetPropertyDirect_Object(ss_message)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVariantBlock::AssertClass(tClassBase * cls) const
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
tString tVariantBlock::GetClassName(bool * got) const
{
	// class.name を得る
	switch(GetType())
	{
	case vtVoid:		if(got) *got=true; return ss_Void;
	case vtInteger:		if(got) *got=true; return ss_Integer;
	case vtReal:		if(got) *got=true; return ss_Real;
	case vtNull:		if(got) *got=true; return ss_Null;
	case vtString:		if(got) *got=true; return ss_String;
	case vtBoolean:		if(got) *got=true; return ss_Boolean;
	case vtOctet:		if(got) *got=true; return ss_Octet;
	case vtObject:
		{
			tVariant name;
			try
			{
				tObjectInterface * intf = GetObjectInterface();
				// class を得る
				tVariant Class;
				if(intf->Operate(ocDGet, &Class, ss_class) !=
					tOperateRetValue::rvNoError)
				{
					if(got) *got = false;
					return tSS<'<','u','n','k','n','o','w','n','>'>();
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
void tVariantBlock::DebugDump() const
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
void tVariantBlock::RegisterMember(const tString & name, const tVariantBlock & value,
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
void tVariantBlock::RegisterFinalConstMember(const tString & name,
	const tVariantBlock & value, risse_uint32 flags) const
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
