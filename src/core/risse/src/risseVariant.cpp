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

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(8265,43737,22162,17503,41631,46790,57901,27164);



//---------------------------------------------------------------------------
// nullオブジェクトへのconst参照を保持するオブジェクト
// これのバイナリレイアウトはtRisseVariantBlockと同一でなければならない
tRisseVariantBlock::tNullObject tRisseVariantBlock::NullObject = {
	reinterpret_cast<risse_ptruint>(RISSE_OBJECT_NULL_PTR)+2, // +2 = Object Mark
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
	case vtBoolean:		return RISSE_WS("boolean");
	case vtString:		return RISSE_WS("string");
	case vtObject:		return RISSE_WS("object");
	case vtOctet:		return RISSE_WS("octet");
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock::tRetValue
	tRisseVariantBlock::OperateForMember(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	switch(GetType())
	{
	case vtInteger:
		{
			tRetValue rv = tRisseIntegerClass::GetPointer()->GetGateway().
				Operate(code, result, name, flags, args, *this); // 動作コンテキストは常に *this
			if(rv == rvNoError && result)
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
				if(!result->HasContext())
					result->SetContext(new tRisseVariantBlock(*this));
			}
			return rv;
		}

	case vtString:
		{
			tRetValue rv = tRisseStringClass::GetPointer()->GetGateway().
				Operate(code, result, name, flags, args, *this); // 動作コンテキストは常に *this
			if(rv == rvNoError && result)
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
				if(!result->HasContext())
					result->SetContext(new tRisseVariantBlock(*this));
			}
			return rv;
		}

	case vtObject:
		tRisseObjectInterface * intf = GetObjectInterface();
		const tRisseVariantBlock * this_context = AsObject().Context;
		return intf->Operate(code, result, name, flags, args,
			this_context?*this_context:This
			);

	default:
		break; // TODO: これ以外のvtに対する処理
	}
	return rvNoError;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::GetPropertyDirect_Object  (const tRisseString & name, risse_uint32 flags, const tRisseVariant & This) const
{
	tRisseObjectInterface * intf = GetObjectInterface();
	const tRisseVariantBlock * this_context = AsObject().Context;
	if(!intf) { /* TODO: null check */; }
	tRisseVariantBlock ret;
	intf->Do(ocDGet, &ret, name,
		flags, tRisseMethodArgument::Empty(),
		this_context?*this_context:This
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::SetPropertyDirect_Object  (const tRisseString & name,
		risse_uint32 flags, const tRisseVariantBlock & value,
		const tRisseVariant & This) const
{
	tRisseObjectInterface * intf = GetObjectInterface();
	const tRisseVariantBlock * this_context = AsObject().Context;
	if(!intf) { /* TODO: null check */; }
	intf->Do(ocDSet, NULL, name,
		flags, tRisseMethodArgument::New(value),
		this_context?*this_context:This
		);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::FuncCall_Integer  (tRisseVariantBlock * ret,
		const tRisseString & name, risse_uint32 flags,
		const tRisseMethodArgument & args, const tRisseVariant & This) const
{
	// vtInteger への要求は Integer クラスにリダイレクトする
	tRisseIntegerClass::GetPointer()->GetGateway().
		Do(ocFuncCall, NULL, name, flags, args, *this); // 動作コンテキストは常に *this
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::FuncCall_String  (tRisseVariantBlock * ret,
		const tRisseString & name, risse_uint32 flags,
		const tRisseMethodArgument & args, const tRisseVariant & This) const
{
	// vtString への要求は String クラスにリダイレクトする
	tRisseStringClass::GetPointer()->GetGateway().
		Do(ocFuncCall, NULL, name, flags, args, *this); // 動作コンテキストは常に *this
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseVariantBlock::FuncCall_Object  (
	tRisseVariantBlock * ret, const tRisseString & name, risse_uint32 flags,
	const tRisseMethodArgument & args, const tRisseVariant & This) const
{
	tRisseObjectInterface * intf = GetObjectInterface();
	const tRisseVariantBlock * this_context = AsObject().Context;
	if(!intf) { /* TODO: null check */; }
	intf->Do(ocFuncCall, ret, name,
		flags, args,
		this_context?*this_context:This
		);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Invoke_Object   (const tRisseString & membername) const
{
	tRisseObjectInterface * intf = GetObjectInterface();
	if(!intf) { /* TODO: null check */; }
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
tRisseVariantBlock tRisseVariantBlock::Invoke_Object   (const tRisseString & membername,const tRisseVariant & arg1) const
{
	tRisseObjectInterface * intf = GetObjectInterface();
	if(!intf) { /* TODO: null check */; }
	tRisseVariantBlock ret;
	intf->Do(ocFuncCall, &ret, membername,
		0, 
		tRisseMethodArgument::New(&arg1),
		*this
		);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::New_Object  (const tRisseString & name,
	risse_uint32 flags, const tRisseMethodArgument & args) const
{
	tRisseObjectInterface * intf = GetObjectInterface();
	if(!intf) { /* TODO: null check */; }
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
bool tRisseVariantBlock::Equal_Void     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return true; // void == void
	case vtInteger:	return CastToInteger_Void() == rhs.AsInteger();
	case vtReal:	return CastToReal_Void()    == rhs.AsReal();
	case vtBoolean:	return CastToBoolean_Void() == rhs.AsBoolean();
	case vtString:	return rhs.AsString().IsEmpty();
	case vtOctet:	return rhs.AsOctet().IsEmpty();
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
	case vtBoolean:	return CastToBoolean_Integer() == rhs.AsBoolean();
	case vtString:	return AsInteger() == rhs.CastToInteger_String();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return CastToBoolean_Real() == rhs.AsBoolean();
	case vtString:	return AsReal() == rhs.CastToReal_String();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtVoid:	return AsBoolean() == 0.0;
	case vtInteger:	return AsBoolean() == rhs.AsInteger();
	case vtReal:	return AsBoolean() == rhs.AsReal();
	case vtBoolean:	return AsBoolean() == rhs.AsBoolean();
	case vtString:	return AsBoolean() == rhs.CastToBoolean_String();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return CastToBoolean_String() == rhs.AsBoolean();
	case vtString:	return AsString() == rhs.AsString();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return CastToBoolean_Void() < rhs.AsBoolean();
	case vtString:	return rhs.AsString().IsEmpty();
	case vtOctet:	return rhs.AsOctet().IsEmpty();
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
	case vtBoolean:	return CastToBoolean_Integer() < rhs.AsBoolean();
	case vtString:	return AsInteger() < rhs.CastToInteger_String();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return CastToBoolean_Real() < rhs.AsBoolean();
	case vtString:	return AsReal() < rhs.CastToReal_String();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtInteger:	return AsBoolean() < rhs.AsInteger();
	case vtReal:	return AsBoolean() < rhs.AsReal();
	case vtBoolean:	return AsBoolean() < rhs.AsBoolean();
	case vtString:	return AsBoolean() < rhs.CastToBoolean_String();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return CastToBoolean_String() < rhs.AsBoolean();
	case vtString:	return AsString() < rhs.AsString();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return CastToBoolean_Void() > rhs.AsBoolean();
	case vtString:	return !rhs.AsString().IsEmpty();
	case vtOctet:	return !rhs.AsOctet().IsEmpty();
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
	case vtBoolean:	return CastToBoolean_Integer() > rhs.AsBoolean();
	case vtString:	return AsInteger() > rhs.CastToInteger_String();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return CastToBoolean_Real() > rhs.AsBoolean();
	case vtString:	return AsReal() > rhs.CastToReal_String();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtInteger:	return AsBoolean() > rhs.AsInteger();
	case vtReal:	return AsBoolean() > rhs.AsReal();
	case vtBoolean:	return AsBoolean() > rhs.AsBoolean();
	case vtString:	return AsBoolean() > rhs.CastToBoolean_String();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return CastToBoolean_String() > rhs.AsBoolean();
	case vtString:	return AsString() > rhs.AsString();
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return !(CastToBoolean_Void() > rhs.AsBoolean());
	case vtString:	return !(!rhs.AsString().IsEmpty());
	case vtOctet:	return !(!rhs.AsOctet().IsEmpty());
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
	case vtBoolean:	return !(CastToBoolean_Integer() > rhs.AsBoolean());
	case vtString:	return !(AsInteger() > rhs.CastToInteger_String());
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return !(CastToBoolean_Real() > rhs.AsBoolean());
	case vtString:	return !(AsReal() > rhs.CastToReal_String());
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtInteger:	return !(AsBoolean() > rhs.AsInteger());
	case vtReal:	return !(AsBoolean() > rhs.AsReal());
	case vtBoolean:	return !(AsBoolean() > rhs.AsBoolean());
	case vtString:	return !(AsBoolean() > rhs.CastToBoolean_String());
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return !(CastToBoolean_String() > rhs.AsBoolean());
	case vtString:	return !(AsString() > rhs.AsString());
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return !(CastToBoolean_Void() < rhs.AsBoolean());
	case vtString:	return !(rhs.AsString().IsEmpty());
	case vtOctet:	return !(rhs.AsOctet().IsEmpty());
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
	case vtBoolean:	return !(CastToBoolean_Integer() < rhs.AsBoolean());
	case vtString:	return !(AsInteger() < rhs.CastToInteger_String());
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return !(CastToBoolean_Real() < rhs.AsBoolean());
	case vtString:	return !(AsReal() < rhs.CastToReal_String());
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtInteger:	return !(AsBoolean() < rhs.AsInteger());
	case vtReal:	return !(AsBoolean() < rhs.AsReal());
	case vtBoolean:	return !(AsBoolean() < rhs.AsBoolean());
	case vtString:	return !(AsBoolean() < rhs.CastToBoolean_String());
	case vtOctet:	return false; // incomplete; どうしよう
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
	case vtBoolean:	return !(CastToBoolean_String() < rhs.AsBoolean());
	case vtString:	return !(AsString() < rhs.AsString());
	case vtOctet:	return false; // incomplete; どうしよう
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mul_Void     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // void * void
	case vtInteger:	return (risse_int64)0; // void * integer
	case vtReal:	return (risse_real)0.0;
	case vtBoolean:	return (bool)false;
	case vtString:	return Mul_Void(rhs.Plus_String()); // Plus_String の戻りは integer か real
	case vtOctet:	return (risse_int64)0; // incomplete
	case vtObject:	return (risse_int64)0; // incomplete
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
	case vtBoolean:	return AsInteger() * (int)rhs.AsBoolean(); // integer * boolean
	case vtString:	return Mul_Integer(rhs.Plus_String()); // Plus_String の戻りは integer か real
	case vtOctet:	return (risse_int64)0; // incomplete
	case vtObject:	return (risse_int64)0; // incomplete
	}
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mul_Real     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // real * void
	case vtInteger:	return AsReal() * rhs.AsInteger(); // real * integer
	case vtReal:	return AsReal() * rhs.AsReal(); // real * real
	case vtBoolean:	return AsReal() * (int)rhs.AsBoolean(); // real * boolean
	case vtString:	return Mul_Real(rhs.Plus_String()); // Plus_String の戻りは integer か real
	case vtOctet:	return (risse_int64)0; // incomplete
	case vtObject:	return (risse_int64)0; // incomplete
	}
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Mul_Boolean  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return false; // bool * void
	case vtInteger:	return AsBoolean() * rhs.AsInteger(); // bool * integer
	case vtReal:	return AsBoolean() * rhs.AsReal(); // bool * real
	case vtBoolean:	return (risse_int64)(AsBoolean() * rhs.AsBoolean()); // bool * boolean
	case vtString:	return Mul_Boolean(rhs.Plus_String()); // Plus_String の戻りは integer か real
	case vtOctet:	return (risse_int64)0; // incomplete
	case vtObject:	return (risse_int64)0; // incomplete
	}
	return tRisseVariantBlock();
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
	case vtBoolean:
	case vtString:
		return Plus_String() * rhs; // Plus_String は integer か real になる
	case vtOctet:	return (risse_int64)0; // incomplete
	case vtObject:	return (risse_int64)0; // incomplete
	}
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Sub_Void     (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return (risse_int64)0; // void - void = 0 かなぁ
	case vtInteger:	return - rhs.AsInteger(); // void - integer
	case vtReal:	return - rhs.AsReal();
	case vtBoolean:	return (risse_int64)(- (int)rhs.AsBoolean());
	case vtString:	return - rhs.Plus_String();
	case vtOctet:	return false; // incomplete; どうしよう
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
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
	case vtBoolean:	return AsInteger() - (int)rhs.AsBoolean(); // integer - bool
	case vtString:	return Sub_Integer(rhs.Plus_String()); // Plus_String の戻りは integer か real
	case vtOctet:	return false; // incomplete; どうしよう
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
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
	case vtBoolean:	return AsReal() - (int)rhs.AsBoolean(); // real - bool
	case vtString:	return Sub_Real(rhs.Plus_String()); // Plus_String の戻りは integer か real
	case vtOctet:	return false; // incomplete; どうしよう
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Sub_Boolean  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this; // bool - void
	case vtInteger:	return (int)AsBoolean() - rhs.AsInteger(); // bool - integer
	case vtReal:	return (int)AsBoolean() - rhs.AsReal(); // bool - real
	case vtBoolean:	return (risse_int64)((int)AsBoolean() - (int)rhs.AsBoolean()); // bool - bool
	case vtString:	return Sub_Boolean(rhs.Plus_String()); // Plus_String の戻りは integer か real
	case vtOctet:	return false; // incomplete; どうしよう
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return tRisseVariantBlock();
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
	case vtBoolean:
	case vtString:
		return Plus_String() - rhs; // Plus_String の戻りは integer か real
	case vtOctet:	return false; // incomplete; どうしよう
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
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
	case vtBoolean:	return rhs;
	case vtString:	return rhs;
	case vtOctet:	return rhs;
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
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
	case vtBoolean:	return AsInteger() + (int)rhs.AsBoolean();
	case vtString:	return CastToString_Integer() + rhs.AsString();
	case vtOctet:	return false; // incomplete; どうしよう
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
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
	case vtBoolean:	return AsReal() + (int)rhs.AsBoolean();
	case vtString:	return CastToString_Real() + rhs.AsString();
	case vtOctet:	return false; // incomplete; どうしよう
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Add_Boolean  (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this;
	case vtInteger:	return (int)AsBoolean() + rhs.AsInteger();
	case vtReal:	return (int)AsBoolean() + rhs.AsReal();
	case vtBoolean:	return (risse_int64)((int)AsBoolean() + (int)rhs.AsBoolean());
	case vtString:	return CastToString_Real() + rhs.AsString();
	case vtOctet:	return false; // incomplete; どうしよう
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return tRisseVariantBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariantBlock tRisseVariantBlock::Add_String   (const tRisseVariantBlock & rhs) const
{
	switch(rhs.GetType())
	{
	case vtVoid:	return *this;
	case vtInteger:	return AsString() + rhs.CastToString_Integer();
	case vtReal:	return AsString() + rhs.CastToString_Real();
	case vtBoolean:	return AsString() + rhs.CastToString_Boolean();
	case vtString:	return AsString() + rhs.AsString();
	case vtOctet:	return false; // incomplete; どうしよう
	case vtObject:	return false; // incomplete; 交換法則を成り立たせるかも
	}
	return tRisseVariantBlock();
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
tRisseString tRisseVariantBlock::CastToString_Boolean  () const
{
	return AsBoolean()?
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
} // namespace Risse
