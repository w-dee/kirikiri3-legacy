//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "risseStaticStrings.h"
#include "risseExceptionClass.h"
#include "risseScriptBlockClass.h"
#include "risseScriptEngine.h"
#include "risseStringTemplate.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(8265,43737,22162,17503,41631,46790,57901,27164);



//---------------------------------------------------------------------------
// tRisseIllegalArgumentTypeExceptionClass を投げるためのマクロ
// この中では this と rhs を使い、tRisseVariant を帰す。
#define RISSE_THROW_ILLEGAL_ARG_TYPE(method_name) \
	tRisseIllegalArgumentTypeExceptionClass::ThrowNonAcceptableType( \
			GetClassName() + ss_doubleColon + (method_name), rhs.GetClassName()); \
			return tRisseVariant::GetVoidObject();
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::ThrowCannotCreateInstanceFromNonClassObjectException()
{
	tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromNonClassObject();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::ThrowBadContextException()
{
	tRisseBadContextExceptionClass::Throw();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::ThrowNoSuchMemberException(const tRisseString &name)
{
	tRisseNoSuchMemberExceptionClass::Throw(name);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// voidオブジェクトへのconst参照を保持するオブジェクト
// これのバイナリレイアウトはtRisseVariantBlockと同一でなければならない
tRisseVariantBlock::tStaticPrimitive tRisseVariantBlock::VoidObject = {
	tRisseVariantBlock::vtVoid,
	{0}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// nullオブジェクトへのconst参照を保持するオブジェクト
// これのバイナリレイアウトはtRisseVariantBlockと同一でなければならない
tRisseVariantBlock::tStaticPrimitive tRisseVariantBlock::NullObject = {
	tRisseVariantBlock::vtNull,
	{0}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// DynamicContextオブジェクトへのconst参照を保持するオブジェクト
// これのバイナリレイアウトはtRisseVariantBlockと同一でなければならない
static tRisseIdentifyObject RisseDynamicContextObject;
tRisseVariantBlock::tStaticObject tRisseVariantBlock::DynamicContext = {
	reinterpret_cast<risse_ptruint>(&RisseDynamicContextObject) + tRisseVariantBlock::ObjectPointerBias, NULL,
	{0}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const risse_char * tRisseVariantBlock::GetTypeString(tType type)
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
inline tRissePrimitiveClassBase * tRisseVariantBlock::GetPrimitiveClass(tRisseScriptEngine * engine) const
{
	tRissePrimitiveClassBase * Class = NULL;
	switch(GetType())
	{
	case vtVoid:	Class = engine->VoidClass; break;
	case vtInteger:	Class = engine->IntegerClass; break;
	case vtReal:	Class = engine->RealClass; break;
	case vtBoolean:	Class = engine->BooleanClass; break;
	case vtString:	Class = engine->StringClass; break;
	case vtOctet:	break;
	default:		break;
	}
	RISSE_ASSERT(Class != NULL);
	return Class;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::SetContext(const tRisseVariantBlock &context)
{
	const tRisseVariant * context_ptr;
	if(context.GetType() == tRisseVariant::vtObject)
	{
		if(context.IsDynamicContext())
			context_ptr = tRisseVariant::GetDynamicContext();
		else
			context_ptr = new tRisseVariant(context);
	}
	else
	{
		context_ptr = new tRisseVariant(context);
	}
	SetContext(context_ptr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock::tRetValue
	tRisseVariantBlock::OperateForMember(tRisseScriptEngine * engine, RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
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
				flags | tRisseOperateFlags::ofUseClassMembersRule,
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
				if(!result->HasContext() && !(flags & tRisseOperateFlags::ofUseClassMembersRule))
					result->SetContext(new tRisseVariantBlock(*this));
			}
			return rv;
		}

	case vtObject:
		{
			tRisseObjectInterface * intf = GetObjectInterface();
			return intf->Operate(code, result, name, flags, args,
				SelectContext(flags, This)
				);
		}
	}
	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::GetPropertyDirect_Primitive(tRisseScriptEngine * engine, const tRisseString & name, risse_uint32 flags, const tRisseVariant & This) const
{
	tRisseVariantBlock result;
	GetPrimitiveClass(engine)->GetGateway().Do(engine, ocDGet, &result, name,
				flags | tRisseOperateFlags::ofUseClassMembersRule,
					// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
					// また、ゲートウェイの members の中から探す
				tRisseMethodArgument::Empty(), *this); // 動作コンテキストは常に *this
	// コンテキストを設定する
	// ここの長ったらしい説明は tRisseVariantBlock::OperateForMember() 内を参照
	if(!result.HasContext() && !(flags & tRisseOperateFlags::ofUseClassMembersRule))
		result.SetContext(new tRisseVariantBlock(*this));
	return result;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::GetPropertyDirect_Object  (const tRisseString & name, risse_uint32 flags, const tRisseVariant & This) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tRisseObjectInterface * intf = GetObjectInterface();
	tRisseVariantBlock ret;
	intf->Do(ocDGet, &ret, name,
		flags, tRisseMethodArgument::Empty(),
		SelectContext(flags, This)
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::SetPropertyDirect_Primitive(tRisseScriptEngine * engine,
	const tRisseString & name, risse_uint32 flags, const tRisseVariantBlock & value,
	const tRisseVariant & This) const
{
	GetPrimitiveClass(engine)->GetGateway().Do(engine, ocDSet, NULL, name,
				flags | tRisseOperateFlags::ofUseClassMembersRule,
					// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
					// また、ゲートウェイの members の中から探す
				tRisseMethodArgument::New(value), *this); // 動作コンテキストは常に *this
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::SetPropertyDirect_Object  (const tRisseString & name,
		risse_uint32 flags, const tRisseVariantBlock & value,
		const tRisseVariant & This) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tRisseObjectInterface * intf = GetObjectInterface();
	intf->Do(ocDSet, NULL, name,
		flags, tRisseMethodArgument::New(value),
		SelectContext(flags, This)
		);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::DeletePropertyDirect_Primitive(tRisseScriptEngine * engine, const tRisseString & name, risse_uint32 flags) const
{
	GetPrimitiveClass(engine)->GetGateway().Do(engine, ocDDelete, NULL, name,
				flags | tRisseOperateFlags::ofUseClassMembersRule,
					// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
					// また、ゲートウェイの members の中から探す
				tRisseMethodArgument::Empty(), *this); // 動作コンテキストは常に *this
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::DeletePropertyDirect_Object   (const tRisseString & name, risse_uint32 flags) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tRisseObjectInterface * intf = GetObjectInterface();
	intf->Do(ocDDelete, NULL, name,
		flags, tRisseMethodArgument::Empty(),
		*this // 動作コンテキストは無視されるが一応 this を指定しておく
		);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::FuncCall(tRisseScriptEngine * engine, tRisseVariantBlock * ret, risse_uint32 flags,
	const tRisseMethodArgument & args,
	const tRisseVariant & This) const
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
		FuncCall_Primitive(engine, ret, tRisseString::GetEmptyString(), flags, args, This); return;
	case vtObject:
		FuncCall_Object   (        ret, tRisseString::GetEmptyString(), flags, args, This); return;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::FuncCall_Primitive(tRisseScriptEngine * engine, 
	tRisseVariantBlock * ret, const tRisseString & name,
	risse_uint32 flags, const tRisseMethodArgument & args,
	const tRisseVariant & This) const
{
	GetPrimitiveClass(engine)->GetGateway().
		Do(engine, ocFuncCall, ret, name,
		flags |tRisseOperateFlags::ofUseClassMembersRule,
		// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
		// また、ゲートウェイの members の中から探す
		args, *this); // 動作コンテキストは常に *this
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::FuncCall_Object  (
	tRisseVariantBlock * ret, const tRisseString & name, risse_uint32 flags,
	const tRisseMethodArgument & args, const tRisseVariant & This) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tRisseObjectInterface * intf = GetObjectInterface();
	intf->Do(ocFuncCall, ret, name,
		flags, args,
		SelectContext(flags, This)
		);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Invoke_Primitive(tRisseScriptEngine * engine, const tRisseString & membername) const
{
	tRisseVariantBlock ret;
	GetPrimitiveClass(engine)->GetGateway().
		Do(engine, ocFuncCall, &ret, membername,
		tRisseOperateFlags::ofUseClassMembersRule,
		// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
		// また、ゲートウェイの members の中から探す
		tRisseMethodArgument::Empty(), *this); // 動作コンテキストは常に *this
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Invoke_Object   (const tRisseString & membername) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tRisseObjectInterface * intf = GetObjectInterface();
	tRisseVariantBlock ret;
	intf->Do(ocFuncCall, &ret, membername,
		0, 
		tRisseMethodArgument::Empty(),
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Invoke_Primitive(tRisseScriptEngine * engine, const tRisseString & membername,const tRisseVariant & arg1) const
{
	tRisseVariantBlock ret;
	GetPrimitiveClass(engine)->GetGateway().
		Do(engine, ocFuncCall, &ret, membername,
		tRisseOperateFlags::ofUseClassMembersRule,
		// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
		// また、ゲートウェイの members の中から探す
		tRisseMethodArgument::New(arg1), *this); // 動作コンテキストは常に *this
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Invoke_Object   (const tRisseString & membername,const tRisseVariant & arg1) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tRisseObjectInterface * intf = GetObjectInterface();
	tRisseVariantBlock ret;
	intf->Do(ocFuncCall, &ret, membername,
		0, 
		tRisseMethodArgument::New(arg1),
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Invoke_Primitive(tRisseScriptEngine * engine, const tRisseString & membername,const tRisseVariant & arg1,const tRisseVariant & arg2) const
{
	tRisseVariantBlock ret;
	GetPrimitiveClass(engine)->GetGateway().
		Do(engine, ocFuncCall, &ret, membername,
		tRisseOperateFlags::ofUseClassMembersRule,
		// ↑動作コンテキストは常に *this なのでゲートウェイのコンテキストは用いない
		// また、ゲートウェイの members の中から探す
		tRisseMethodArgument::New(arg1, arg2), *this); // 動作コンテキストは常に *this
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Invoke_Object   (const tRisseString & membername,const tRisseVariant & arg1,const tRisseVariant & arg2) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tRisseObjectInterface * intf = GetObjectInterface();
	tRisseVariantBlock ret;
	intf->Do(ocFuncCall, &ret, membername,
		0, 
		tRisseMethodArgument::New(arg1, arg2),
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::New_Object  (const tRisseString & name,
	risse_uint32 flags, const tRisseMethodArgument & args) const
{
	RISSE_ASSERT(GetType() == vtObject);
	tRisseObjectInterface * intf = GetObjectInterface();
	tRisseVariantBlock ret;
	intf->Do(ocNew, &ret, name,
		flags,
		args,
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Plus_String   () const
{
	tRisseVariantBlock val;
	const risse_char *p = AsString().c_str();
	if(tRisseLexerUtility::ParseNumber(p, val))
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
tRisseVariantBlock tRisseVariantBlock::BitOr_Void     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitOr_Integer  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitOr_Real     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitOr_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitOr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitOr_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitOr_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitOr_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitXor_Void     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitXor_Integer  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitXor_Real     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitXor_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitXor);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitXor_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitXor_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitXor_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitAnd_Void     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitAnd_Integer  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitAnd_Real     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitAnd_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnBitAnd);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitAnd_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitAnd_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::BitAnd_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Equal_Void     (const tRisseVariantBlock & rhs) const
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
bool tRisseVariantBlock::Equal_Integer  (const tRisseVariantBlock & rhs) const
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
bool tRisseVariantBlock::Equal_Real     (const tRisseVariantBlock & rhs) const
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
bool tRisseVariantBlock::Equal_Null     (const tRisseVariantBlock & rhs) const
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
bool tRisseVariantBlock::Equal_String   (const tRisseVariantBlock & rhs) const
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
bool tRisseVariantBlock::Equal_Octet    (const tRisseVariantBlock & rhs) const
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
bool tRisseVariantBlock::Equal_Boolean  (const tRisseVariantBlock & rhs) const
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
bool tRisseVariantBlock::Lesser_Void     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void < void
	case vtInteger:	return CastToInteger_Void() < rhs.AsInteger();
	case vtReal:	return CastToReal_Void()    < rhs.AsReal();
	case vtNull:	return false; // 常に偽
	case vtString:	return rhs.AsString().IsEmpty();
	case vtOctet:	return rhs.AsOctet().IsEmpty();
	case vtBoolean:	return CastToBoolean_Void() < rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Lesser_Integer  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger() < 0;
	case vtInteger:	return AsInteger() < rhs.AsInteger();
	case vtReal:	return AsInteger() < rhs.AsReal();
	case vtNull:	return false; // 常に偽
	case vtString:	return AsInteger() < rhs.CastToInteger_String();
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return CastToBoolean_Integer() < rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Lesser_Real     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsReal() < 0.0;
	case vtInteger:	return AsReal() < rhs.AsInteger();
	case vtReal:	return AsReal() < rhs.AsReal();
	case vtNull:	return false; // 常に偽
	case vtString:	return AsReal() < rhs.CastToReal_String();
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return CastToBoolean_Real() < rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Lesser_String   (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void は空文字列と見なす
	case vtInteger:	return CastToInteger_String() < rhs.AsInteger();
	case vtReal:	return CastToReal_String() < rhs.AsReal();
	case vtNull:	return false; // 常に偽
	case vtString:	return AsString() < rhs.AsString();
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return CastToBoolean_String() < rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Lesser_Octet    (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void は空オクテット列と見なす
	case vtInteger:	return false; // 常に偽
	case vtReal:	return false; // 常に偽
	case vtNull:	return false; // 常に偽
	case vtString:	return false; // 常に偽
	case vtOctet:	return AsOctet() < rhs.AsOctet();
	case vtBoolean:	return CastToBoolean_Octet() < rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Lesser_Boolean  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // boolean が void より小さくなることはない
	case vtInteger:	return CastToBoolean_Boolean() < rhs.AsInteger();
	case vtReal:	return CastToBoolean_Boolean() < rhs.AsReal();
	case vtNull:	return false; // 常に偽
	case vtString:	return CastToBoolean_Boolean() < rhs.CastToBoolean_String();
	case vtOctet:	return CastToBoolean_Boolean() < rhs.CastToBoolean_Octet();
	case vtBoolean:	return CastToBoolean_Boolean() < rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Greater_Void     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void > void
	case vtInteger:	return CastToInteger_Void() > rhs.AsInteger();
	case vtReal:	return CastToReal_Void()    > rhs.AsReal();
	case vtNull:	return false; // 常に偽
	case vtString:	return !rhs.AsString().IsEmpty();
	case vtOctet:	return !rhs.AsOctet().IsEmpty();
	case vtBoolean:	return CastToBoolean_Void() > rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Greater_Integer  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsInteger() > 0;
	case vtInteger:	return AsInteger() > rhs.AsInteger();
	case vtReal:	return AsInteger() > rhs.AsReal();
	case vtNull:	return false; // 常に偽
	case vtString:	return AsInteger() > rhs.CastToInteger_String();
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return CastToBoolean_Integer() > rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Greater_Real     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return AsReal() > 0.0;
	case vtInteger:	return AsReal() > rhs.AsInteger();
	case vtReal:	return AsReal() > rhs.AsReal();
	case vtNull:	return false; // 常に偽
	case vtString:	return AsReal() > rhs.CastToReal_String();
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return CastToBoolean_Real() > rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Greater_String   (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void は空文字列と見なす
	case vtInteger:	return CastToInteger_String() > rhs.AsInteger();
	case vtReal:	return CastToReal_String() > rhs.AsReal();
	case vtNull:	return false; // 常に偽
	case vtString:	return AsString() > rhs.AsString();
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return CastToBoolean_String() > rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Greater_Octet    (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void は空オクテット列と見なす
	case vtInteger:	return false; // 常に偽
	case vtReal:	return false; // 常に偽
	case vtNull:	return false; // 常に偽
	case vtString:	return false; // 常に偽
	case vtOctet:	return AsOctet() > rhs.AsOctet();
	case vtBoolean:	return CastToBoolean_Octet() > rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::Greater_Boolean  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // boolean が void より小さくなることはない
	case vtInteger:	return CastToBoolean_Boolean() > rhs.AsInteger();
	case vtReal:	return CastToBoolean_Boolean() > rhs.AsReal();
	case vtNull:	return false; // 常に偽
	case vtString:	return CastToBoolean_Boolean() > rhs.CastToBoolean_String();
	case vtOctet:	return CastToBoolean_Boolean() > rhs.CastToBoolean_Octet();
	case vtBoolean:	return CastToBoolean_Boolean() > rhs.CastToBoolean_Boolean();
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::LesserOrEqual_Void     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void <= void
	case vtInteger:	return !(CastToInteger_Void() > rhs.AsInteger());
	case vtReal:	return !(CastToReal_Void()    > rhs.AsReal());
	case vtNull:	return false; // 常に偽
	case vtString:	return !(!rhs.AsString().IsEmpty());
	case vtOctet:	return !(!rhs.AsOctet().IsEmpty());
	case vtBoolean:	return !(CastToBoolean_Void() > rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::LesserOrEqual_Integer  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsInteger() > 0);
	case vtInteger:	return !(AsInteger() > rhs.AsInteger());
	case vtReal:	return !(AsInteger() > rhs.AsReal());
	case vtNull:	return false; // 常に偽
	case vtString:	return !(AsInteger() > rhs.CastToInteger_String());
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return !(CastToBoolean_Integer() > rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::LesserOrEqual_Real     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsReal() > 0.0);
	case vtInteger:	return !(AsReal() > rhs.AsInteger());
	case vtReal:	return !(AsReal() > rhs.AsReal());
	case vtNull:	return false; // 常に偽
	case vtString:	return !(AsReal() > rhs.CastToReal_String());
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return !(CastToBoolean_Real() > rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::LesserOrEqual_String   (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void は空文字列と見なす
	case vtInteger:	return !(CastToInteger_String() > rhs.AsInteger());
	case vtReal:	return !(CastToReal_String() > rhs.AsReal());
	case vtNull:	return false; // 常に偽
	case vtString:	return !(AsString() > rhs.AsString());
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return !(CastToBoolean_String() > rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::LesserOrEqual_Octet    (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void は空オクテット列と見なす
	case vtInteger:	return false; // 常に偽
	case vtReal:	return false; // 常に偽
	case vtNull:	return false; // 常に偽
	case vtString:	return false; // 常に偽
	case vtOctet:	return !(AsOctet() > rhs.AsOctet());
	case vtBoolean:	return !(CastToBoolean_Octet() > rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::LesserOrEqual_Boolean  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // boolean が void より小さくなることはない
	case vtInteger:	return !(CastToBoolean_Boolean() > rhs.AsInteger());
	case vtReal:	return !(CastToBoolean_Boolean() > rhs.AsReal());
	case vtNull:	return CastToBoolean_Boolean() == false; // 偽ならば真
	case vtString:	return !(CastToBoolean_Boolean() > rhs.CastToBoolean_String());
	case vtOctet:	return !(CastToBoolean_Boolean() > rhs.CastToBoolean_Octet());
	case vtBoolean:	return !(CastToBoolean_Boolean() > rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::GreaterOrEqual_Void     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // void >= void
	case vtInteger:	return !(CastToInteger_Void() < rhs.AsInteger());
	case vtReal:	return !(CastToReal_Void()    < rhs.AsReal());
	case vtNull:	return false; // 常に偽
	case vtString:	return !(rhs.AsString().IsEmpty());
	case vtOctet:	return !(rhs.AsOctet().IsEmpty());
	case vtBoolean:	return !(CastToBoolean_Void() < rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::GreaterOrEqual_Integer  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsInteger() < 0);
	case vtInteger:	return !(AsInteger() < rhs.AsInteger());
	case vtReal:	return !(AsInteger() < rhs.AsReal());
	case vtNull:	return false; // 常に偽
	case vtString:	return !(AsInteger() < rhs.CastToInteger_String());
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return !(CastToBoolean_Integer() < rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::GreaterOrEqual_Real     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !(AsReal() < 0.0);
	case vtInteger:	return !(AsReal() < rhs.AsInteger());
	case vtReal:	return !(AsReal() < rhs.AsReal());
	case vtNull:	return false; // 常に偽
	case vtString:	return !(AsReal() < rhs.CastToReal_String());
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return !(CastToBoolean_Real() < rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::GreaterOrEqual_String   (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // !(void は空文字列と見なす)
	case vtInteger:	return !(CastToInteger_String() < rhs.AsInteger());
	case vtReal:	return !(CastToReal_String() < rhs.AsReal());
	case vtNull:	return false; // 常に偽
	case vtString:	return !(AsString() < rhs.AsString());
	case vtOctet:	return false; // 常に偽
	case vtBoolean:	return !(CastToBoolean_String() < rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::GreaterOrEqual_Octet    (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // void は空オクテット列と見なす
	case vtInteger:	return false; // 常に偽
	case vtReal:	return false; // 常に偽
	case vtNull:	return false; // 常に偽
	case vtString:	return false; // 常に偽
	case vtOctet:	return !(AsOctet() < rhs.AsOctet());
	case vtBoolean:	return !(CastToBoolean_Octet() < rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::GreaterOrEqual_Boolean  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return !false; // !(boolean が void より小さくなることはない)
	case vtInteger:	return !(CastToBoolean_Boolean() < rhs.AsInteger());
	case vtReal:	return !(CastToBoolean_Boolean() < rhs.AsReal());
	case vtNull:	return CastToBoolean_Boolean() == false; // 偽ならば真
	case vtString:	return !(CastToBoolean_Boolean() < rhs.CastToBoolean_String());
	case vtOctet:	return !(CastToBoolean_Boolean() < rhs.CastToBoolean_Octet());
	case vtBoolean:	return !(CastToBoolean_Boolean() < rhs.CastToBoolean_Boolean());
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RBitShift_Void     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RBitShift_Integer  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RBitShift_Real     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RBitShift_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRBitShift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RBitShift_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RBitShift_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RBitShift_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::LShift_Void     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::LShift_Integer  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::LShift_Real     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::LShift_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnLShift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::LShift_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::LShift_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::LShift_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RShift_Void     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RShift_Integer  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RShift_Real     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RShift_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnRShift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RShift_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RShift_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::RShift_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mod_Void     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();

do_div:
	if(rhs_value == 0) tRisseArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)0 % rhs_value; // void % integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mod_Integer  (const tRisseVariantBlock & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tRisseArithmeticExceptionClass::ThrowDivideByZeroException(); return tRisseVariantBlock();
	case vtInteger:	rhs_value = rhs.AsInteger();						goto do_div;
	case vtReal:	rhs_value = static_cast<risse_int64>(rhs.AsReal());	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tRisseVariantBlock();

do_div:
	if(rhs_value == 0) tRisseArithmeticExceptionClass::ThrowDivideByZeroException();
	return AsInteger() % rhs_value; // integer % integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mod_Real     (const tRisseVariantBlock & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tRisseArithmeticExceptionClass::ThrowDivideByZeroException(); return tRisseVariantBlock();
	case vtInteger:	rhs_value = rhs.AsInteger();			goto do_div;
	case vtReal:	rhs_value = (risse_int64)rhs.AsReal();	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
	}
	return tRisseVariantBlock();

do_div:
	if(rhs_value == 0) tRisseArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)AsReal() % rhs_value; // real % integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mod_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMod);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mod_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mod_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mod_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Div_Void     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Div_Integer  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Div_Real     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Div_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnDiv);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Div_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Div_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Div_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Idiv_Void     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();

do_div:
	if(rhs_value == 0) tRisseArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)0 / rhs_value; // void / integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Idiv_Integer  (const tRisseVariantBlock & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tRisseArithmeticExceptionClass::ThrowDivideByZeroException(); return tRisseVariantBlock();
	case vtInteger:	rhs_value = rhs.AsInteger();						goto do_div;
	case vtReal:	rhs_value = static_cast<risse_int64>(rhs.AsReal());	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tRisseVariantBlock();

do_div:
	if(rhs_value == 0) tRisseArithmeticExceptionClass::ThrowDivideByZeroException();
	return AsInteger() / rhs_value; // integer / integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Idiv_Real     (const tRisseVariantBlock & rhs) const
{
	risse_int64 rhs_value;
	switch(rhs.GetType())
	{
	case vtVoid:	tRisseArithmeticExceptionClass::ThrowDivideByZeroException(); return tRisseVariantBlock();
	case vtInteger:	rhs_value = rhs.AsInteger();			goto do_div;
	case vtReal:	rhs_value = (risse_int64)rhs.AsReal();	goto do_div;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
	}
	return tRisseVariantBlock();

do_div:
	if(rhs_value == 0) tRisseArithmeticExceptionClass::ThrowDivideByZeroException();
	return (risse_int64)AsReal() / rhs_value; // real / integer
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Idiv_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnIdiv);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Idiv_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Idiv_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Idiv_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mul_Void     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return tRisseVariant::GetVoidObject(); // void * void
	case vtInteger:	return (risse_int64)0; // void * integer
	case vtReal:	return (risse_real)0.0;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
	}
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mul_Integer  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mul_Real     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mul_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnMul);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mul_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mul_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mul_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Add_Void     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return tRisseVariantBlock(); // void + void = void かなぁ
	case vtInteger:	return rhs; // void + integer
	case vtReal:	return rhs;
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtString:	return rhs;
	case vtOctet:	return rhs;
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
	}
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Add_Integer  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Add_Real     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Add_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Add_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Add_Octet    (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Add_Boolean  (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnAdd);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Sub_Void     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return tRisseVariant::GetVoidObject(); // void - void = void かなぁ
	case vtInteger:	return - rhs.AsInteger(); // void - integer
	case vtReal:	return - rhs.AsReal();
	case vtNull:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtString:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtOctet:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtBoolean:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	case vtObject:	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
	}
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Sub_Integer  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Sub_Real     (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Sub_Null     (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Sub_Octet    (const tRisseVariantBlock & rhs) const
{
	RISSE_THROW_ILLEGAL_ARG_TYPE(mnSub);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Sub_String   (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Sub_Boolean  (const tRisseVariantBlock & rhs) const
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
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseVariantBlock::InstanceOf(tRisseScriptEngine * engine, const tRisseVariantBlock & rhs) const
{
	// this の class を得る
	tRisseVariant Class = GetPropertyDirect(engine, ss_class, tRisseOperateFlags::ofInstanceMemberOnly, *this);

	RISSE_ASSERT(Class.GetType() == vtObject);

	tRisseVariantBlock ret;
	Class.GetObjectInterface()->Do(ocInstanceOf, &ret, tRisseString::GetEmptyString(), 0,
			tRisseMethodArgument::New(rhs), Class);
	return ret.operator bool();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int64 tRisseVariantBlock::CastToInteger_String   () const
{
	tRisseVariantBlock val;
	const risse_char *p = AsString().c_str();
	if(tRisseLexerUtility::ParseNumber(p, val))
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
tRisseString tRisseVariantBlock::CastToString_Integer  () const
{
	risse_char buf[40];
	Risse_int64_to_str(AsInteger(), buf);
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVariantBlock::CastToString_Real     () const
{
	risse_int32 cls = RisseGetFPClass(AsReal());

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
			return RISSE_WS("+0.0");
	}

	risse_char buf[25];
	Risse_real_to_str(AsReal(), buf);
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock::tSynchronizer::tSynchronizer(const tRisseVariant & object)
{
	// この ASSERT は、 tRisseObjectInterface::tSynchronizer のサイズが
	// Synchronizer のサイズ以下であることを保証する。
	// ヘッダの記述位置では tRisseObjectInterface も tRisseCriticalSection も
	// 定義済みかどうかが怪しい。とりあえず sizeof(void*) の分を確保しているが、
	// tRisseCriticalSection::tLocker の実装を見る限りは大丈夫なはず。
	RISSE_ASSERT(sizeof(Synchronizer) >= sizeof(tRisseObjectInterface::tSynchronizer));

	// 今のところ、ロックが出来るのは vtObject だけ。
	// vtObject 以外の場合はロックを行わない。
	// ロックオブジェクトを Synchronizer の場所に作成する
	tRisseObjectInterface *intf;
	if(object.GetType() == tRisseVariantBlock::vtObject)
		intf = object.GetObjectInterface();
	else
		intf = NULL;

	new (reinterpret_cast<tRisseObjectInterface::tSynchronizer*>(Synchronizer))
			tRisseObjectInterface::tSynchronizer(intf);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock::tSynchronizer::~tSynchronizer()
{
	// ロックオブジェクトを消滅させる
	(reinterpret_cast<tRisseObjectInterface::tSynchronizer*>(Synchronizer))->~tSynchronizer();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVariantBlock::CastToString_Null     () const
{
	ThrowNoSuchMemberException(mnString);
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVariantBlock::CastToString_Boolean  () const
{
	return CastToBoolean_Boolean()?
		RISSE_WS("true"):
		RISSE_WS("false");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVariantBlock::AsHumanReadable_Void     (risse_size maxlen) const
{
	return RISSE_WS("void");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVariantBlock::AsHumanReadable_Null     (risse_size maxlen) const
{
	return RISSE_WS("null");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::AddTrace(const tRisseScriptBlockInstance * sb, risse_size pos) const
{
	if(sb != NULL && pos != risse_size_max)
	{
		RISSE_ASSERT(GetType() == vtObject);
		tRisseScriptEngine * engine = this->GetObjectInterface()->GetRTTI()->GetScriptEngine();
		tRisseVariant source_point = tRisseVariant(engine->SourcePointClass).New(0,
			tRisseMethodArgument::New(
				tRisseVariant(const_cast<tRisseScriptBlockInstance *>(sb)),
				(risse_int64)pos));
		Invoke_Object(ss_addTrace, source_point);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVariantBlock::GetClassName(bool * got) const
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
			tRisseVariant name;
			try
			{
				tRisseObjectInterface * intf = GetObjectInterface();
				// class を得る
				tRisseVariant Class;
				if(intf->Operate(ocDGet, &Class, ss_class) !=
					tRisseOperateRetValue::rvNoError)
				{
					if(got) *got = false;
					return tRisseSS<'<','u','n','k','n','o','w','n','>'>();
				}
				// class.name を得る
				if(Class.GetType() != vtObject)
				{
					if(got) *got = false;
					return tRisseSS<'<','u','n','k','n','o','w','n','>'>();
				}
				intf = Class.GetObjectInterface();
				if(intf->Operate(ocDGet, &name, ss_name) !=
					tRisseOperateRetValue::rvNoError)
				{
					if(got) *got = false;
					return tRisseSS<'<','u','n','k','n','o','w','n','>'>();
				}
			}
			catch(...)
			{
				// 例外が発生
				// 型名は不明であると判断する
				if(got) *got = false;
				return tRisseSS<'<','u','n','k','n','o','w','n','>'>();
			}
			tRisseString str = name.operator tRisseString();
			if(str.IsEmpty()) str = tRisseSS<'<','a','n','o','n','y','m','o','u','s','>'>();
			return str;
		}
	}
	return tRisseSS<'<','u','n','k','n','o','w','n','>'>();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::DebugDump() const
{
	if(GetType() == tRisseVariant::vtObject)
	{
		risse_char buf[40];
		Risse_pointer_to_str(GetObjectInterface(), buf);
		RisseFPrint(stderr, (tRisseString(RISSE_WS("Object@")) + buf).c_str());
		const tRisseVariant * context = GetContext();
		if(context)
		{
			if(context->GetType() == tRisseVariant::vtObject)
			{
				if(context == GetDynamicContext())
				{
					RisseFPrint(stderr, RISSE_WS(":dynamic"));
				}
				else
				{
					Risse_pointer_to_str(context->GetObjectInterface(), buf);
					RisseFPrint(stderr, (tRisseString(RISSE_WS(":")) + buf).c_str());
				}
			}
			else
			{
				RisseFPrint(stderr, (context->AsHumanReadable()).c_str());
			}
		}
	}
	else
	{
		RisseFPrint(stderr, AsHumanReadable().c_str());
	}

	RisseFPrint(stderr, RISSE_WS("\n"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
