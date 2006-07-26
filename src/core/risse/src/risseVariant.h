//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バリアント型の実装
//---------------------------------------------------------------------------
#ifndef risseVariantH
#define risseVariantH

#include "risseGC.h"
#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseCxxObject.h"
#include "risseCxxString.h"
#include "risseCxxOctet.h"
#include "risseException.h"


namespace Risse
{
//---------------------------------------------------------------------------
//! @brief	バリアント型
/*! @note

tRisseStringData, tRisseObjectData, tRisseOctetData の各先頭のメンバは必ず
何かのポインタである。それらはメンバ Type とストレージを共有する。これらは
実際にはそれぞれ tRisseString, tRisseObject, tRisseOctet として扱われるが、
データメンバのレイアウトは同一である。

各ポインタは4の倍数のアドレスにしか配置されないことがメモリアロケータの仕様
および risse_char のサイズにより保証されている。このため、下位2ビットは必ず遊
んでいることになる。また、0x10 未満のような極端に低い番地にこれらのポインタ
が配置されることはあり得ない。

そのため、tRisseVariant::GetType() を見ればわかるとおり、Type が 4 以上
ならば下位2ビット+4を Type とし、4 未満ならばそれをそのまま Type として返
している。ここら辺は ruby の実装からヒントを得た物。

問題はtRisseString, tRisseObject, tRisseOctet 内にある各ポインタのアライン
メントがあわないことだが、これは ~0x03 との bit and をとれば、元のポインタを
求めることができる。

tRisseString の内部ポインタが指し示している場所は、文字列を保持しているバッ
ファである。RisseではUTF-32文字列を対象とするため、このポインタが 32bit境界
にそろわないことはない。デバッガなどでそのまま UTF-32 文字列を表示したい用
途がある。tRisseString の Type の条件は 0x04 以上かつ下位2ビットが00
であるため、内部ポインタの値と本来指し示しているポインタは同じになる。これに
より、内部ポインタが指し示すポインタがそのまま文字列バッファのポインタとなり、
そのままデバッガなどで内容を表示できる。

とりあえず tRiseVariant のサイズを抑えたいがための苦肉の策。こんなことをしな
い方が速いかもしれないし、こうした方が速いかもしれない。 

tRisseVariant はパフォーマンスの関係上、ILP32 システムでは 3 * 32bit, LP64 シス
テムでは 2 * 64bit に収まるようにすること。
*/
//---------------------------------------------------------------------------
class tRisseVariantBlock : public tRisseCollectee
{
protected:

	//! @brief void ストレージ型
	struct tVoid
	{
		risse_ptruint Type; //!< バリアントタイプ: 0 固定
	};

	//! @brief integer ストレージ型
	struct tInteger
	{
		risse_ptruint Type; //!< バリアントタイプ: 1 固定
		risse_int64  Value; //!< 値
	};

	//! @brief real ストレージ型
	struct tReal
	{
		risse_ptruint Type; //!< バリアントタイプ: 2 固定
		double Value; //!< 値
	};

	//! @brief boolean ストレージ型
	struct tBoolean
	{
		risse_ptruint Type; //!< バリアントタイプ: 3 固定
		bool Value; //!< 値
	};

	//! @brief Integer型への参照を取得 @return Integer型フィールドへの参照
	risse_int64 & AsInteger() { return reinterpret_cast<tInteger*>(Storage)->Value; }
	//! @brief Integer型へのconst参照を取得 @return Integer型フィールドへのconst参照
	const risse_int64 & AsInteger() const { return reinterpret_cast<const tInteger*>(Storage)->Value; }

	//! @brief Real型への参照を取得 @return Real型フィールドへの参照
	double & AsReal() { return reinterpret_cast<tReal*>(Storage)->Value; }
	//! @brief Real型へのconst参照を取得 @return Real型フィールドへのconst参照
	const double & AsReal() const { return reinterpret_cast<const tReal*>(Storage)->Value; }

	//! @brief Boolean型への参照を取得 @return Boolean型フィールドへの参照
	bool & AsBoolean() { return reinterpret_cast<tBoolean*>(Storage)->Value; }
	//! @brief Boolean型へのconst参照を取得 @return Boolean型フィールドへのconst参照
	const bool & AsBoolean() const { return reinterpret_cast<const tBoolean*>(Storage)->Value; }

	//! @brief String型への参照を取得 @return String型フィールドへの参照
	tRisseString & AsString() { return *reinterpret_cast<tRisseString*>(Storage); }
	//! @brief String型へのconst参照を取得 @return String型フィールドへのconst参照
	const tRisseString & AsString() const { return *reinterpret_cast<const tRisseString*>(Storage); }

	//! @brief Octet型への参照を取得 @return Octet型フィールドへの参照
	tRisseOctet & AsOctet() { return *reinterpret_cast<tRisseOctet*>(Storage); }
	//! @brief Octet型へのconst参照を取得 @return Octet型フィールドへのconst参照
	const tRisseOctet & AsOctet() const { return *reinterpret_cast<const tRisseOctet*>(Storage); }

	//! @brief Object型への参照を取得 @return Object型フィールドへの参照
	tRisseObject & AsObject() { return *reinterpret_cast<tRisseObject*>(Storage); }
	//! @brief Object型へのconst参照を取得 @return Object型フィールドへのconst参照
	const tRisseObject & AsObject() const { return *reinterpret_cast<const tRisseObject*>(Storage); }

	#define RV_SIZE_MAX(a, b) ((a)>(b)?(a):(b))
	//! @brief 各バリアントの内部型の union
	union
	{
		risse_ptruint Type;	//!< バリアントタイプ

		//! @brief	データストレージ
		//! @note
		//! わざわざマクロで各構造体のサイズの最大値を得て、その要素数を
		//! もった char 配列を確保しているが、これは gcc などが
		//! union で構造体を配置する際に、望ましくないパッキングを
		//! 行ってしまう可能性があるため。
		char Storage[
			RV_SIZE_MAX(sizeof(risse_ptruint),
			RV_SIZE_MAX(sizeof(tVoid),
			RV_SIZE_MAX(sizeof(tInteger),
			RV_SIZE_MAX(sizeof(tReal),
			RV_SIZE_MAX(sizeof(tBoolean),
			RV_SIZE_MAX(sizeof(tRisseString),
			RV_SIZE_MAX(sizeof(tRisseOctet),
			RV_SIZE_MAX(sizeof(tRisseObject),
					4 /*ダミー*/
			 ))))))))
			];
	};
public:
	//! @brief バリアントのタイプ
	enum tType
	{
		vtVoid			= 0,
		vtInteger		= 1,
		vtReal			= 2,
		vtBoolean		= 3,
		vtString		= 4 + 0,
		vtOctet			= 4 + 1,
		vtObject		= 4 + 2,
	//	vtReserved		= 4 + 2,
	};

	//! @brief バリアントのタイプを得る
	//! @return バリアントのタイプ
	tType GetType() const
	{
		return static_cast<tType>((Type & 3) + ( (Type >= 4) << 2 ));

		// 上記の行は以下の2行と同じ
		//	if(Type >= 4) return static_cast<tType>((Type & 3) + 4);
		//	return static_cast<tType>(Type);
	}

	//! @brief		バリアントのタイプを文字列化する
	//! @param		type タイプ
	//! @return		型を表す文字列
	static const risse_char * GetTypeString(tType type);

	//! @brief		バリアントのタイプを文字列化する
	//! @param		type タイプ
	//! @return		型を表す文字列
	const risse_char * GetTypeString() const { return GetTypeString(GetType()); }

public: // コンストラクタ/代入演算子

	//! @brief デフォルトコンストラクタ(void型を作成)
	tRisseVariantBlock()
	{
		Type = vtVoid;
	}

	//! @brief		コピーコンストラクタ
	//! @param		ref		元となるオブジェクト
	tRisseVariantBlock(const tRisseVariantBlock & ref)
	{
		switch(ref.GetType())
		{
		case vtVoid:		Type = vtVoid;				break;
		case vtInteger:		*this = ref.AsInteger();	break;
		case vtReal:		*this = ref.AsReal();		break;
		case vtBoolean:		*this = ref.AsBoolean();	break;
		case vtString:		*this = ref.AsString();		break;
		case vtOctet:		*this = ref.AsOctet();		break;
		case vtObject:		*this = ref.AsObject();		break;
		}
	}

	//! @brief		コンストラクタ(integer型を作成)
	//! @param		ref		元となる整数
	tRisseVariantBlock(const risse_int64 ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(integer型を代入)
	//! @param		ref		元となる整数
	tRisseVariantBlock & operator = (const risse_int64 ref)
	{
		Type = vtInteger;
		AsInteger() = ref;
		return *this;
	}

	//! @brief		コンストラクタ(real型を作成)
	//! @param		ref		元となる実数
	tRisseVariantBlock(const double ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(real型を代入)
	//! @param		ref		元となる実数
	tRisseVariantBlock & operator = (const double ref)
	{
		Type = vtReal;
		AsReal() = ref;
		return *this;
	}

	//! @brief		コンストラクタ(bool型を作成)
	//! @param		ref		元となる真偽値
	tRisseVariantBlock(const bool ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(bool型を代入)
	//! @param		ref		元となる真偽値
	tRisseVariantBlock & operator = (const bool ref)
	{
		Type = vtBoolean;
		AsBoolean() = ref;
		return *this;
	}

	//! @brief		コンストラクタ(string型を作成)
	//! @param		ref		元となる文字列
	tRisseVariantBlock(const tRisseString & ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(string型を代入)
	//! @param		ref		元となる文字列
	//! @return		このオブジェクトへの参照
	tRisseVariantBlock & operator = (const tRisseString & ref)
	{
		// Type の設定は必要なし
		AsString() = ref;
		return *this;
	}

	//! @brief		コンストラクタ(octet型を作成)
	//! @param		ref		元となるオクテット列
	tRisseVariantBlock(const tRisseOctet & ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(octet型を代入)
	//! @param		ref		元となるオクテット列
	tRisseVariantBlock & operator = (const tRisseOctet & ref)
	{
		// Type の設定は必要なし
		AsOctet() = ref;
		return *this;
	}

	//! @brief		コンストラクタ(object型を作成)
	//! @param		ref		元となるオブジェクト
	tRisseVariantBlock(const tRisseObject & ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(object型を代入)
	//! @param		ref		元となるオブジェクト
	tRisseVariantBlock & operator = (const tRisseObject & ref)
	{
		// Type の設定は必要なし
		AsObject() = ref;
		return *this;
	}

public: // 演算子

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトに対する)関数呼び出し		FuncCall
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		argc		引数の数
	//! @param		argv		引数へのポインタの配列
	//-----------------------------------------------------------------------
	void FuncCall(tRisseVariantBlock * ret, risse_size argc, tRisseVariantBlock *argv[])
	{
		// Object 以外は関数(メソッド)としては機能しないため
		// すべて 例外を発生する
		switch(GetType())
		{
		case vtObject:	FuncCall_Object   (ret, argc, argv); return;

		default:
			RisseThrowCannotCallNonFunctionObjectException(); break;
		}
	}

	void FuncCall_Object   (tRisseVariantBlock * ret, risse_size argc, tRisseVariantBlock *argv[])
		{ return ; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトをクラスと見なした)インスタンス作成	New
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		argc		引数の数
	//! @param		argv		引数へのポインタの配列
	//-----------------------------------------------------------------------
	void New(tRisseVariantBlock * ret, risse_size argc, tRisseVariantBlock *argv[])
	{
		// Object 以外はクラスとしては機能しないため
		// すべて 例外を発生する
		switch(GetType())
		{
		case vtObject:	New_Object   (ret, argc, argv); return;

		default:
			RisseThrowCannotCreateInstanceFromNonClassObjectException(); break;
		}
	}

	void New_Object   (tRisseVariantBlock * ret, risse_size argc, tRisseVariantBlock *argv[])
		{ return ; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトに対する)ブロック付き関数呼び出し		FuncCallBlock
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		argc		引数の数
	//! @param		argv		引数へのポインタの配列
	//! @param		bargc		ブロック引数の数
	//! @param		bargv		ブロック引数へのポインタの配列
	//-----------------------------------------------------------------------
	void FuncCall(tRisseVariantBlock * ret, risse_size argc, tRisseVariantBlock *argv[],
		risse_size bargc, tRisseVariantBlock *bargv[])
	{
		// Object 以外は関数(メソッド)としては機能しないため
		// すべて 例外を発生する
		switch(GetType())
		{
		case vtObject:	FuncCallBlock_Object   (ret, argc, argv, bargc, bargv); return;

		default:
			RisseThrowCannotCallNonFunctionObjectException(); break;
		}
	}

	void FuncCallBlock_Object   (tRisseVariantBlock * ret, risse_size argc, tRisseVariantBlock *argv[],
		risse_size bargc, tRisseVariantBlock *bargv[])
		{ return ; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		単項 ! 演算子		LogNot
	//! @return		演算結果(booleanへのキャストの真偽を反転させた物)
	//! @note		この演算子の戻り値は常に bool
	//-----------------------------------------------------------------------
	bool LogNot() const
	{
		return !(bool)(*this);
	}

	bool operator !() const { return LogNot(); }

	bool LogNot_Void     () const { return !CastToBoolean_Void(); }
	bool LogNot_Integer  () const { return !CastToBoolean_Integer(); }
	bool LogNot_Real     () const { return !CastToBoolean_Real(); }
	bool LogNot_Boolean  () const { return !CastToBoolean_Boolean(); }
	bool LogNot_String   () const { return !CastToBoolean_String(); }
	bool LogNot_Octet    () const { return !CastToBoolean_Octet(); }
	bool LogNot_Object   () const { return !CastToBoolean_Object(); }

	//-----------------------------------------------------------------------
	//! @brief		単項 ~ 演算子		BitNot
	//! @return		演算結果(通常、integerへのキャストのビットを反転させた物)
	//-----------------------------------------------------------------------
	tRisseVariantBlock BitNot() const
	{
		switch(GetType())
		{
		case vtVoid:	return BitNot_Void     ();
		case vtInteger:	return BitNot_Integer  ();
		case vtReal:	return BitNot_Real     ();
		case vtBoolean:	return BitNot_Boolean  ();
		case vtString:	return BitNot_String   ();
		case vtOctet:	return BitNot_Octet    ();
		case vtObject:	return BitNot_Object   ();
		}
		return tRisseVariantBlock();
	}

	tRisseVariantBlock operator ~() const { return BitNot(); }

	// vtObject 以外は常に integer へのキャストのビットを反転させた物を返す。
	// vtObject に関してはオブジェクトによっては演算子をオーバーロードしている可能性が
	// あるため、別途処理を行う。
	// vtObject の戻り値は integer ではないかもしれない。

	risse_int64        BitNot_Void     () const { return ~CastToInteger_Void(); }
	risse_int64        BitNot_Integer  () const { return ~CastToInteger_Integer(); }
	risse_int64        BitNot_Real     () const { return ~CastToInteger_Real(); }
	risse_int64        BitNot_Boolean  () const { return ~CastToInteger_Boolean(); }
	risse_int64        BitNot_String   () const { return ~CastToInteger_String(); }
	risse_int64        BitNot_Octet    () const { return ~CastToInteger_Octet(); }
	tRisseVariantBlock BitNot_Object   () const { return (risse_int64)0; /* incomplete */; }

	//-----------------------------------------------------------------------
	//! @brief		単項 + 演算子		Plus
	//! @return		演算結果(通常、数値へのキャスト)
	//-----------------------------------------------------------------------
	tRisseVariantBlock Plus() const
	{
		switch(GetType())
		{
		case vtVoid:	return Plus_Void     ();
		case vtInteger:	return Plus_Integer  ();
		case vtReal:	return Plus_Real     ();
		case vtBoolean:	return Plus_Boolean  ();
		case vtString:	return Plus_String   ();
		case vtOctet:	return Plus_Octet    ();
		case vtObject:	return Plus_Object   ();
		}
		return tRisseVariantBlock();
	}

	tRisseVariantBlock operator +() const { return Plus(); }

	tRisseVariantBlock Plus_Void     () const { return (risse_int64)0; /* void は 整数の 0 */ }
	tRisseVariantBlock Plus_Integer  () const { return *this; }
	tRisseVariantBlock Plus_Real     () const { return *this; }
	tRisseVariantBlock Plus_Boolean  () const { return AsBoolean() != false; /* boolean は 0 か 1 かに変換される */ }
	tRisseVariantBlock Plus_String   () const;
	tRisseVariantBlock Plus_Octet    () const { return (risse_int64)0; /* incomplete */; }
	tRisseVariantBlock Plus_Object   () const { return (risse_int64)0; /* incomplete */; }

	//-----------------------------------------------------------------------
	//! @brief		単項 - 演算子		Minus
	//! @return		演算結果(通常、符号が反転した物)
	//-----------------------------------------------------------------------
	tRisseVariantBlock Minus() const
	{
		switch(GetType())
		{
		case vtVoid:	return Minus_Void     ();
		case vtInteger:	return Minus_Integer  ();
		case vtReal:	return Minus_Real     ();
		case vtBoolean:	return Minus_Boolean  ();
		case vtString:	return Minus_String   ();
		case vtOctet:	return Minus_Octet    ();
		case vtObject:	return Minus_Object   ();
		}
		return tRisseVariantBlock();
	}

	tRisseVariantBlock operator -() const { return Minus(); }

	tRisseVariantBlock Minus_Void     () const { return (risse_int64)0; }
	tRisseVariantBlock Minus_Integer  () const { return -AsInteger(); }
	tRisseVariantBlock Minus_Real     () const { return -AsReal(); }
	tRisseVariantBlock Minus_Boolean  () const { return (risse_int64)(AsBoolean()?-1:0); }
	tRisseVariantBlock Minus_String   () const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock Minus_Octet    () const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock Minus_Object   () const { return tRisseVariantBlock(); /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		|| 演算子		LogOr
	//! @return		演算結果(通常、双方のbooleanキャストの論理和)
	//! @note		この演算子はショートカットを行う。すなわち、左辺が真ならば
	//!				右辺は評価されない
	//! @note		この演算子の戻り値は常に bool
	//-----------------------------------------------------------------------
	bool LogOr(const tRisseVariantBlock & rhs) const
	{
		return (bool)*this || (bool)rhs; // 短絡を行う
	}

	bool operator ||(const tRisseVariantBlock & rhs) const { return LogOr(rhs); }

	bool LogOr_Void     (const tRisseVariantBlock & rhs) const { return rhs.operator bool(); }
	bool LogOr_Integer  (const tRisseVariantBlock & rhs) const { return CastToBoolean_Integer() || rhs.operator bool(); }
	bool LogOr_Real     (const tRisseVariantBlock & rhs) const { return CastToBoolean_Real   () || rhs.operator bool(); }
	bool LogOr_Boolean  (const tRisseVariantBlock & rhs) const { return CastToBoolean_Boolean() || rhs.operator bool(); }
	bool LogOr_String   (const tRisseVariantBlock & rhs) const { return CastToBoolean_String () || rhs.operator bool(); }
	bool LogOr_Octet    (const tRisseVariantBlock & rhs) const { return CastToBoolean_Octet  () || rhs.operator bool(); }
	bool LogOr_Object   (const tRisseVariantBlock & rhs) const { return CastToBoolean_Object () || rhs.operator bool(); }

	//-----------------------------------------------------------------------
	//! @brief		&& 演算子		LogAnd
	//! @return		演算結果(通常、双方のbooleanキャストの論理積)
	//! @note		この演算子はショートカットを行う。すなわち、左辺が偽ならば
	//!				右辺は評価されない
	//! @note		この演算子の戻り値は常に bool
	//-----------------------------------------------------------------------
	bool LogAnd(const tRisseVariantBlock & rhs) const
	{
		return (bool)*this && (bool)rhs; // 短絡を行う
	}

	bool operator &&(const tRisseVariantBlock & rhs) const { return LogAnd(rhs); }

	bool LogAnd_Void     (const tRisseVariantBlock & rhs) const { return false; }
	bool LogAnd_Integer  (const tRisseVariantBlock & rhs) const { return CastToBoolean_Integer() && rhs.operator bool(); }
	bool LogAnd_Real     (const tRisseVariantBlock & rhs) const { return CastToBoolean_Real   () && rhs.operator bool(); }
	bool LogAnd_Boolean  (const tRisseVariantBlock & rhs) const { return CastToBoolean_Boolean() && rhs.operator bool(); }
	bool LogAnd_String   (const tRisseVariantBlock & rhs) const { return CastToBoolean_String () && rhs.operator bool(); }
	bool LogAnd_Octet    (const tRisseVariantBlock & rhs) const { return CastToBoolean_Octet  () && rhs.operator bool(); }
	bool LogAnd_Object   (const tRisseVariantBlock & rhs) const { return CastToBoolean_Object () && rhs.operator bool(); }

	//-----------------------------------------------------------------------
	//! @brief		| 演算子		BitOr
	//! @return		演算結果(通常、双方のintegerキャストのビット和)
	//-----------------------------------------------------------------------
	tRisseVariantBlock BitOr(const tRisseVariantBlock & rhs) const
	{
		// vtObject の場合は演算子がオーバーロードされている可能性があるため、
		// 戻り値は integer ではないかもしれない。
		switch(GetType())
		{
		case vtVoid:	return BitOr_Void     (rhs);
		case vtInteger:	return BitOr_Integer  (rhs);
		case vtReal:	return BitOr_Real     (rhs);
		case vtBoolean:	return BitOr_Boolean  (rhs);
		case vtString:	return BitOr_String   (rhs);
		case vtOctet:	return BitOr_Octet    (rhs);
		case vtObject:	return BitOr_Object   (rhs);
		}
		return (risse_int64)0;
	}

	tRisseVariantBlock operator |(const tRisseVariantBlock & rhs) const { return BitOr(rhs); }

	risse_int64        BitOr_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)rhs; }
	risse_int64        BitOr_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
	risse_int64        BitOr_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
	risse_int64        BitOr_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
	risse_int64        BitOr_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
	risse_int64        BitOr_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
	tRisseVariantBlock BitOr_Object   (const tRisseVariantBlock & rhs) const { return (risse_int64)0; /* incomplete */; }

	//-----------------------------------------------------------------------
	//! @brief		^ 演算子		BitXor
	//! @return		演算結果(通常、双方のintegerキャストのビット排他的論理和)
	//-----------------------------------------------------------------------
	tRisseVariantBlock BitXor(const tRisseVariantBlock & rhs) const
	{
		// vtObject の場合は演算子がオーバーロードされている可能性があるため、
		// 戻り値は integer ではないかもしれない。
		switch(GetType())
		{
		case vtVoid:	return BitXor_Void     (rhs);
		case vtInteger:	return BitXor_Integer  (rhs);
		case vtReal:	return BitXor_Real     (rhs);
		case vtBoolean:	return BitXor_Boolean  (rhs);
		case vtString:	return BitXor_String   (rhs);
		case vtOctet:	return BitXor_Octet    (rhs);
		case vtObject:	return BitXor_Object   (rhs);
		}
		return (risse_int64)0;
	}

	tRisseVariantBlock operator ^(const tRisseVariantBlock & rhs) const { return BitXor(rhs); }

	risse_int64        BitXor_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)rhs; }
	risse_int64        BitXor_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
	risse_int64        BitXor_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
	risse_int64        BitXor_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
	risse_int64        BitXor_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
	risse_int64        BitXor_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
	tRisseVariantBlock BitXor_Object   (const tRisseVariantBlock & rhs) const { return (risse_int64)0; /* incomplete */; }

	//-----------------------------------------------------------------------
	//! @brief		& 演算子		BitAnd
	//! @return		演算結果(通常、双方のintegerキャストのビット論理積)
	//-----------------------------------------------------------------------
	tRisseVariantBlock BitAnd(const tRisseVariantBlock & rhs) const
	{
		// vtObject の場合は演算子がオーバーロードされている可能性があるため、
		// 戻り値は integer ではないかもしれない。
		switch(GetType())
		{
		case vtVoid:	return BitAnd_Void     (rhs);
		case vtInteger:	return BitAnd_Integer  (rhs);
		case vtReal:	return BitAnd_Real     (rhs);
		case vtBoolean:	return BitAnd_Boolean  (rhs);
		case vtString:	return BitAnd_String   (rhs);
		case vtOctet:	return BitAnd_Octet    (rhs);
		case vtObject:	return BitAnd_Object   (rhs);
		}
		return (risse_int64)0;
	}

	tRisseVariantBlock operator &(const tRisseVariantBlock & rhs) const { return BitAnd(rhs); }

	risse_int64        BitAnd_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)0; }
	risse_int64        BitAnd_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
	risse_int64        BitAnd_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
	risse_int64        BitAnd_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
	risse_int64        BitAnd_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
	risse_int64        BitAnd_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
	tRisseVariantBlock BitAnd_Object   (const tRisseVariantBlock & rhs) const { return (risse_int64)0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		!= 演算子		NotEqual
	//! @return		演算結果
	//! @note		この演算子の戻り値は常に bool
	//-----------------------------------------------------------------------
	bool NotEqual(const tRisseVariantBlock & rhs) const
	{
		// vtObject 以外は == 演算子の真偽を逆にした物である
		// vtObject の場合はオブジェクトによって振る舞いが異なる(ように定義できる)
		switch(GetType())
		{
		case vtObject:	return NotEqual_Object   (rhs);
		default:
			return !Equal(rhs);
		}
	}

	bool operator !=(const tRisseVariantBlock & rhs) const { return NotEqual(rhs); }

	bool NotEqual_Void     (const tRisseVariantBlock & rhs) const { return !Equal_Void   (rhs); }
	bool NotEqual_Integer  (const tRisseVariantBlock & rhs) const { return !Equal_Integer(rhs); }
	bool NotEqual_Real     (const tRisseVariantBlock & rhs) const { return !Equal_Real   (rhs); }
	bool NotEqual_Boolean  (const tRisseVariantBlock & rhs) const { return !Equal_Boolean(rhs); }
	bool NotEqual_String   (const tRisseVariantBlock & rhs) const { return !Equal_String (rhs); }
	bool NotEqual_Octet    (const tRisseVariantBlock & rhs) const { return !Equal_Octet  (rhs); }
	bool NotEqual_Object   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		== 演算子		Equal
	//! @return		演算結果
	//! @note		この演算子の戻り値は常に bool
	//-----------------------------------------------------------------------
	tRisseVariantBlock Equal(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return Equal_Void     (rhs);
		case vtInteger:	return Equal_Integer  (rhs);
		case vtReal:	return Equal_Real     (rhs);
		case vtBoolean:	return Equal_Boolean  (rhs);
		case vtString:	return Equal_String   (rhs);
		case vtOctet:	return Equal_Octet    (rhs);
		case vtObject:	return Equal_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator ==(const tRisseVariantBlock & rhs) const { return Equal(rhs); }

	bool Equal_Void     (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Equal_Integer  (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Equal_Real     (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Equal_Boolean  (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Equal_String   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Equal_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Equal_Object   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		!== 演算子		DiscNotEqual
	//! @return		演算結果
	//! @note		この演算子の戻り値は常に bool
	//-----------------------------------------------------------------------
	bool DiscNotEqual(const tRisseVariantBlock & rhs) const
	{
		// vtObject 以外は === 演算子の真偽を逆にした物である
		// vtObject の場合はオブジェクトによって振る舞いが異なる(ように定義できる)
		switch(GetType())
		{
		case vtObject:	return DiscNotEqual_Object   (rhs);
		default:
			return !DiscEqual(rhs);
		}
	}

	bool DiscNotEqual_Void     (const tRisseVariantBlock & rhs) const { return !DiscEqual_Void   (rhs); }
	bool DiscNotEqual_Integer  (const tRisseVariantBlock & rhs) const { return !DiscEqual_Integer(rhs); }
	bool DiscNotEqual_Real     (const tRisseVariantBlock & rhs) const { return !DiscEqual_Real   (rhs); }
	bool DiscNotEqual_Boolean  (const tRisseVariantBlock & rhs) const { return !DiscEqual_Boolean(rhs); }
	bool DiscNotEqual_String   (const tRisseVariantBlock & rhs) const { return !DiscEqual_String (rhs); }
	bool DiscNotEqual_Octet    (const tRisseVariantBlock & rhs) const { return !DiscEqual_Octet  (rhs); }
	bool DiscNotEqual_Object   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		識別 === 演算子		DiscEqual
	//! @param		rhs			右辺
	//! @return		識別の結果、同一ならば真、そうでなければ偽
	//-----------------------------------------------------------------------
	bool DiscEqual(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return DiscEqual_Void     (rhs);
		case vtInteger:	return DiscEqual_Integer  (rhs);
		case vtReal:	return DiscEqual_Real     (rhs);
		case vtBoolean:	return DiscEqual_Boolean  (rhs);
		case vtString:	return DiscEqual_String   (rhs);
		case vtOctet:	return DiscEqual_Octet    (rhs);
		case vtObject:	return DiscEqual_Object   (rhs);
		}
		return false;
	}

	bool DiscEqual_Void     (const tRisseVariantBlock & rhs) const
			{ return rhs.GetType() == vtVoid; }
	bool DiscEqual_Integer  (const tRisseVariantBlock & rhs) const
			{ return rhs.GetType() == vtInteger && rhs.AsInteger() == AsInteger(); }
	bool DiscEqual_Real     (const tRisseVariantBlock & rhs) const
			{ return rhs.GetType() == vtReal && rhs.AsReal() == AsReal(); }
	bool DiscEqual_Boolean  (const tRisseVariantBlock & rhs) const
			{ return rhs.GetType() == vtBoolean && rhs.AsBoolean() == AsBoolean(); }
	bool DiscEqual_String   (const tRisseVariantBlock & rhs) const
			{ return rhs.GetType() == vtString && rhs.AsString() == AsString(); /* incomplete */ }
	bool DiscEqual_Octet    (const tRisseVariantBlock & rhs) const
			{ return false; /* incomplete */ }
	bool DiscEqual_Object   (const tRisseVariantBlock & rhs) const
			{ return false; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		< 演算子		Lesser
	//! @param		rhs			右辺
	//! @return		*this < rhs ならば真
	//-----------------------------------------------------------------------
	bool Lesser(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return Lesser_Void     (rhs);
		case vtInteger:	return Lesser_Integer  (rhs);
		case vtReal:	return Lesser_Real     (rhs);
		case vtBoolean:	return Lesser_Boolean  (rhs);
		case vtString:	return Lesser_String   (rhs);
		case vtOctet:	return Lesser_Octet    (rhs);
		case vtObject:	return Lesser_Object   (rhs);
		}
		return false;
	}

	bool operator < (const tRisseVariantBlock & rhs) const { return Lesser(rhs); }

	bool Lesser_Void     (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Lesser_Integer  (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Lesser_Real     (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Lesser_Boolean  (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Lesser_String   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Lesser_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Lesser_Object   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		> 演算子		Greater
	//! @param		rhs			右辺
	//! @return		*this > rhs ならば真
	//-----------------------------------------------------------------------
	bool Greater(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return Greater_Void     (rhs);
		case vtInteger:	return Greater_Integer  (rhs);
		case vtReal:	return Greater_Real     (rhs);
		case vtBoolean:	return Greater_Boolean  (rhs);
		case vtString:	return Greater_String   (rhs);
		case vtOctet:	return Greater_Octet    (rhs);
		case vtObject:	return Greater_Object   (rhs);
		}
		return false;
	}

	bool operator > (const tRisseVariantBlock & rhs) const { return Greater(rhs); }

	bool Greater_Void     (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Greater_Integer  (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Greater_Real     (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Greater_Boolean  (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Greater_String   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Greater_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Greater_Object   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		<= 演算子		LesserOrEqual
	//! @param		rhs			右辺
	//! @return		*this < rhs ならば真
	//-----------------------------------------------------------------------
	bool LesserOrEqual(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return LesserOrEqual_Void     (rhs);
		case vtInteger:	return LesserOrEqual_Integer  (rhs);
		case vtReal:	return LesserOrEqual_Real     (rhs);
		case vtBoolean:	return LesserOrEqual_Boolean  (rhs);
		case vtString:	return LesserOrEqual_String   (rhs);
		case vtOctet:	return LesserOrEqual_Octet    (rhs);
		case vtObject:	return LesserOrEqual_Object   (rhs);
		}
		return false;
	}

	bool operator <= (const tRisseVariantBlock & rhs) const { return LesserOrEqual(rhs); }

	bool LesserOrEqual_Void     (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool LesserOrEqual_Integer  (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool LesserOrEqual_Real     (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool LesserOrEqual_Boolean  (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool LesserOrEqual_String   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool LesserOrEqual_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool LesserOrEqual_Object   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }


	//-----------------------------------------------------------------------
	//! @brief		>= 演算子		GreaterOrEqual
	//! @param		rhs			右辺
	//! @return		*this < rhs ならば真
	//-----------------------------------------------------------------------
	bool GreaterOrEqual(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return GreaterOrEqual_Void     (rhs);
		case vtInteger:	return GreaterOrEqual_Integer  (rhs);
		case vtReal:	return GreaterOrEqual_Real     (rhs);
		case vtBoolean:	return GreaterOrEqual_Boolean  (rhs);
		case vtString:	return GreaterOrEqual_String   (rhs);
		case vtOctet:	return GreaterOrEqual_Octet    (rhs);
		case vtObject:	return GreaterOrEqual_Object   (rhs);
		}
		return false;
	}

	bool operator >= (const tRisseVariantBlock & rhs) const { return GreaterOrEqual(rhs); }

	bool GreaterOrEqual_Void     (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool GreaterOrEqual_Integer  (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool GreaterOrEqual_Real     (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool GreaterOrEqual_Boolean  (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool GreaterOrEqual_String   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool GreaterOrEqual_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool GreaterOrEqual_Object   (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		>>> 演算子(符号なし右シフト)		RBitShift
	//! @param		rhs			右辺
	//! @return		通常、左辺をintegerにキャストした後右辺回数分シフトしたもの
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が integer だとは限らない
	//-----------------------------------------------------------------------
	tRisseVariantBlock RBitShift(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return RBitShift_Void     (rhs);
		case vtInteger:	return RBitShift_Integer  (rhs);
		case vtReal:	return RBitShift_Real     (rhs);
		case vtBoolean:	return RBitShift_Boolean  (rhs);
		case vtString:	return RBitShift_String   (rhs);
		case vtOctet:	return RBitShift_Octet    (rhs);
		case vtObject:	return RBitShift_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock RBitShift_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_Object   (const tRisseVariantBlock & rhs) const { return (risse_int64)0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		<< 演算子(符号つき左シフト)		LShift
	//! @param		rhs			右辺
	//! @return		通常、左辺をintegerにキャストした後右辺辺回数分シフトしたもの
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が integer だとは限らない
	//-----------------------------------------------------------------------
	tRisseVariantBlock LShift(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return LShift_Void     (rhs);
		case vtInteger:	return LShift_Integer  (rhs);
		case vtReal:	return LShift_Real     (rhs);
		case vtBoolean:	return LShift_Boolean  (rhs);
		case vtString:	return LShift_String   (rhs);
		case vtOctet:	return LShift_Octet    (rhs);
		case vtObject:	return LShift_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator << (const tRisseVariantBlock & rhs) const { return LShift(rhs); }

	tRisseVariantBlock LShift_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_Object   (const tRisseVariantBlock & rhs) const { return (risse_int64)0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		>> 演算子(符号つき右シフト)		RShift
	//! @param		rhs			右辺
	//! @return		通常、左辺をintegerにキャストした後右辺回数分シフトしたもの
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が integer だとは限らない
	//-----------------------------------------------------------------------
	tRisseVariantBlock RShift(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return RShift_Void     (rhs);
		case vtInteger:	return RShift_Integer  (rhs);
		case vtReal:	return RShift_Real     (rhs);
		case vtBoolean:	return RShift_Boolean  (rhs);
		case vtString:	return RShift_String   (rhs);
		case vtOctet:	return RShift_Octet    (rhs);
		case vtObject:	return RShift_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator >> (const tRisseVariantBlock & rhs) const { return RShift(rhs); }

	tRisseVariantBlock RShift_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_Object   (const tRisseVariantBlock & rhs) const { return (risse_int64)0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		% 演算子(剰余)		Mod
	//! @param		rhs			右辺
	//! @return		通常、両方をintegerにキャストし、左辺を右辺で割ったあまり
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が integer だとは限らない
	//-----------------------------------------------------------------------
	tRisseVariantBlock Mod(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return Mod_Void     (rhs);
		case vtInteger:	return Mod_Integer  (rhs);
		case vtReal:	return Mod_Real     (rhs);
		case vtBoolean:	return Mod_Boolean  (rhs);
		case vtString:	return Mod_String   (rhs);
		case vtOctet:	return Mod_Octet    (rhs);
		case vtObject:	return Mod_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator % (const tRisseVariantBlock & rhs) const { return Mod(rhs); }

	tRisseVariantBlock Mod_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_Object   (const tRisseVariantBlock & rhs) const { return (risse_int64)0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		/ 演算子(剰余)		Div
	//! @param		rhs			右辺
	//! @return		通常、両方を real にキャストし、左辺を右辺で割ったもの
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が real だとは限らない
	//-----------------------------------------------------------------------
	tRisseVariantBlock Div(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return Div_Void     (rhs);
		case vtInteger:	return Div_Integer  (rhs);
		case vtReal:	return Div_Real     (rhs);
		case vtBoolean:	return Div_Boolean  (rhs);
		case vtString:	return Div_String   (rhs);
		case vtOctet:	return Div_Octet    (rhs);
		case vtObject:	return Div_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator / (const tRisseVariantBlock & rhs) const { return Div(rhs); }

	tRisseVariantBlock Div_Void     (const tRisseVariantBlock & rhs) const { return (double)(*this) / (double)rhs; }
	tRisseVariantBlock Div_Integer  (const tRisseVariantBlock & rhs) const { return (double)(*this) / (double)rhs; }
	tRisseVariantBlock Div_Real     (const tRisseVariantBlock & rhs) const { return (double)(*this) / (double)rhs; }
	tRisseVariantBlock Div_Boolean  (const tRisseVariantBlock & rhs) const { return (double)(*this) / (double)rhs; }
	tRisseVariantBlock Div_String   (const tRisseVariantBlock & rhs) const { return (double)(*this) / (double)rhs; }
	tRisseVariantBlock Div_Octet    (const tRisseVariantBlock & rhs) const { return (double)(*this) / (double)rhs; }
	tRisseVariantBlock Div_Object   (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		\ 演算子(整数除算)		Idiv
	//! @param		rhs			右辺
	//! @return		通常、両方をintegerにキャストし、左辺を右辺で割ったもの
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が integer だとは限らない
	//-----------------------------------------------------------------------
	tRisseVariantBlock Idiv(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return Idiv_Void     (rhs);
		case vtInteger:	return Idiv_Integer  (rhs);
		case vtReal:	return Idiv_Real     (rhs);
		case vtBoolean:	return Idiv_Boolean  (rhs);
		case vtString:	return Idiv_String   (rhs);
		case vtOctet:	return Idiv_Octet    (rhs);
		case vtObject:	return Idiv_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock Idiv_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_Object   (const tRisseVariantBlock & rhs) const { return (risse_int64)0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		* 演算子(除算)		Mul
	//! @param		rhs			右辺
	//! @return		通常、両方を 数値 にキャストし、左辺と右辺を乗算した物。
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が integerやreal だとは限らない
	//-----------------------------------------------------------------------
	tRisseVariantBlock Mul(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return Mul_Void     (rhs);
		case vtInteger:	return Mul_Integer  (rhs);
		case vtReal:	return Mul_Real     (rhs);
		case vtBoolean:	return Mul_Boolean  (rhs);
		case vtString:	return Mul_String   (rhs);
		case vtOctet:	return Mul_Octet    (rhs);
		case vtObject:	return Mul_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator * (const tRisseVariantBlock & rhs) const { return Mul(rhs); }

	tRisseVariantBlock Mul_Void     (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Mul_Integer  (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Mul_Real     (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Mul_Boolean  (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Mul_String   (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Mul_Octet    (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Mul_Object   (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		+ 演算子(加算)		Add
	//! @param		rhs			右辺
	//! @return		左辺に右辺を加算した物
	//-----------------------------------------------------------------------
	tRisseVariantBlock Add(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return Add_Void     (rhs);
		case vtInteger:	return Add_Integer  (rhs);
		case vtReal:	return Add_Real     (rhs);
		case vtBoolean:	return Add_Boolean  (rhs);
		case vtString:	return Add_String   (rhs);
		case vtOctet:	return Add_Octet    (rhs);
		case vtObject:	return Add_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator + (const tRisseVariantBlock & rhs) const { return Add(rhs); }

	tRisseVariantBlock Add_Void     (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Add_Integer  (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Add_Real     (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Add_Boolean  (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Add_String   (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Add_Octet    (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Add_Object   (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		- 演算子(減算)		Sub
	//! @param		rhs			右辺
	//! @return		左辺から右辺を減算した物
	//-----------------------------------------------------------------------
	tRisseVariantBlock Sub(const tRisseVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return Sub_Void     (rhs);
		case vtInteger:	return Sub_Integer  (rhs);
		case vtReal:	return Sub_Real     (rhs);
		case vtBoolean:	return Sub_Boolean  (rhs);
		case vtString:	return Sub_String   (rhs);
		case vtOctet:	return Sub_Octet    (rhs);
		case vtObject:	return Sub_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator - (const tRisseVariantBlock & rhs) const { return Sub(rhs); }

	tRisseVariantBlock Sub_Void     (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Sub_Integer  (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Sub_Real     (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Sub_Boolean  (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Sub_String   (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Sub_Octet    (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Sub_Object   (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }

public: // キャスト
	//-----------------------------------------------------------------------
	//! @brief		integerに変換
	//! @return		integer
	//-----------------------------------------------------------------------
	operator risse_int64() const
	{
		switch(GetType())
		{
		case vtVoid:	return CastToInteger_Void     ();
		case vtInteger:	return CastToInteger_Integer  ();
		case vtReal:	return CastToInteger_Real     ();
		case vtBoolean:	return CastToInteger_Boolean  ();
		case vtString:	return CastToInteger_String   ();
		case vtOctet:	return CastToInteger_Octet    ();
		case vtObject:	return CastToInteger_Object   ();
		}
		return (risse_int64)0;
	}

	risse_int64 CastToInteger_Void     () const { return false; /* void は 0 */}
	risse_int64 CastToInteger_Integer  () const { return AsInteger(); }
	risse_int64 CastToInteger_Real     () const { return (risse_int64)AsReal(); }
	risse_int64 CastToInteger_Boolean  () const { return (risse_int64)AsBoolean(); }
	risse_int64 CastToInteger_String   () const;
	risse_int64 CastToInteger_Octet    () const { return (risse_int64)0; /* incomplete */ }
	risse_int64 CastToInteger_Object   () const { return (risse_int64)0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		realに変換
	//! @return		real
	//-----------------------------------------------------------------------
	operator double() const
	{
		switch(GetType())
		{
		case vtVoid:	return CastToReal_Void     ();
		case vtInteger:	return CastToReal_Integer  ();
		case vtReal:	return CastToReal_Real     ();
		case vtBoolean:	return CastToReal_Boolean  ();
		case vtString:	return CastToReal_String   ();
		case vtOctet:	return CastToReal_Octet    ();
		case vtObject:	return CastToReal_Object   ();
		}
		return false;
	}

	double CastToReal_Void     () const { return (double)0.0; }
	double CastToReal_Integer  () const { return AsInteger(); }
	double CastToReal_Real     () const { return AsReal(); }
	double CastToReal_Boolean  () const { return (double)(int)AsBoolean(); }
	double CastToReal_String   () const { return (double)Plus_String(); /* Plus_String の戻りを double に再キャスト */ }
	double CastToReal_Octet    () const { return (double)0.0; /* incomplete */ }
	double CastToReal_Object   () const { return (double)0.0; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		boolに変換
	//! @return		bool
	//-----------------------------------------------------------------------
	operator bool() const
	{
		switch(GetType())
		{
		case vtVoid:	return CastToBoolean_Void     ();
		case vtInteger:	return CastToBoolean_Integer  ();
		case vtReal:	return CastToBoolean_Real     ();
		case vtBoolean:	return CastToBoolean_Boolean  ();
		case vtString:	return CastToBoolean_String   ();
		case vtOctet:	return CastToBoolean_Octet    ();
		case vtObject:	return CastToBoolean_Object   ();
		}
		return false;
	}

	bool CastToBoolean_Void     () const { return false; /* void は偽 */}
	bool CastToBoolean_Integer  () const { return AsInteger() != 0; }
	bool CastToBoolean_Real     () const { return AsReal() != 0.0; }
	bool CastToBoolean_Boolean  () const { return AsBoolean(); }
	bool CastToBoolean_String   () const { return !AsString().IsEmpty(); }
	bool CastToBoolean_Octet    () const { return !AsOctet().IsEmpty(); }
	bool CastToBoolean_Object   () const { return true; /* incomplete */ }

	//-----------------------------------------------------------------------
	//! @brief		文字列に変換
	//! @return		文字列
	//-----------------------------------------------------------------------
	operator tRisseString() const
	{
		switch(GetType())
		{
		case vtVoid:	return CastToString_Void     ();
		case vtInteger:	return CastToString_Integer  ();
		case vtReal:	return CastToString_Real     ();
		case vtBoolean:	return CastToString_Boolean  ();
		case vtString:	return CastToString_String   ();
		case vtOctet:	return CastToString_Octet    ();
		case vtObject:	return CastToString_Object   ();
		}
		return tRisseString();
	}

	tRisseString CastToString_Void     () const { return tRisseString(); }
	tRisseString CastToString_Integer  () const;
	tRisseString CastToString_Real     () const;
	tRisseString CastToString_Boolean  () const;
	tRisseString CastToString_String   () const { return AsString(); }
	tRisseString CastToString_Octet    () const { return AsOctet().AsHumanReadable();  }
	tRisseString CastToString_Object   () const { return tRisseString(); /* incomplete */ }

public: // ユーティリティ
	//-----------------------------------------------------------------------
	//! @brief		人間が可読な形式に変換
	//! @param		maxlen		出力最大コードポイント数(目安)<br>
	//!							あくまで目安。無視されたり、ぴったりのコード
	//!							ポイント数にならなかったりする。risse_size_max
	//!							を指定すると制限なし
	//! @return		人間が可読な文字列
	//-----------------------------------------------------------------------
	tRisseString AsHumanReadable(risse_size maxlen = risse_size_max) const
	{
		switch(GetType())
		{
		case vtVoid:	return AsHumanReadable_Void     (maxlen);
		case vtInteger:	return AsHumanReadable_Integer  (maxlen);
		case vtReal:	return AsHumanReadable_Real     (maxlen);
		case vtBoolean:	return AsHumanReadable_Boolean  (maxlen);
		case vtString:	return AsHumanReadable_String   (maxlen);
		case vtOctet:	return AsHumanReadable_Octet    (maxlen);
		case vtObject:	return AsHumanReadable_Object   (maxlen);
		}
		return tRisseString();
	}

	tRisseString AsHumanReadable_Void     (risse_size maxlen) const;
	tRisseString AsHumanReadable_Integer  (risse_size maxlen) const
					{ return CastToString_Integer(); }
	tRisseString AsHumanReadable_Real     (risse_size maxlen) const
					{ return CastToString_Real(); }
	tRisseString AsHumanReadable_Boolean  (risse_size maxlen) const
					{ return CastToString_Boolean(); }
	tRisseString AsHumanReadable_String   (risse_size maxlen) const
					{ return AsString().AsHumanReadable(maxlen); }
	tRisseString AsHumanReadable_Octet    (risse_size maxlen) const
					{ return AsOctet().AsHumanReadable(maxlen); }
	tRisseString AsHumanReadable_Object   (risse_size maxlen) const
					{ return tRisseString(); /* incomplete */ }

	//! @brief		デバッグ用各種構造体サイズ表示
	void prtsizes()
	{
		printf("tRisseVariantBlock: %d\n", sizeof(tRisseVariantBlock));
		printf("Storage: %d\n", sizeof(Storage));
		printf("risse_ptruint: %d\n", sizeof(risse_ptruint));
		printf("tRisseString: %d\n", sizeof(tRisseString));
		printf("tRisseOctet: %d\n", sizeof(tRisseOctet));
		printf("tRisseObject: %d\n", sizeof(tRisseObject));
		printf("tVoid: %d\n", sizeof(tVoid));
		printf("tInteger: %d\n", sizeof(tInteger));
		printf("tReal: %d\n", sizeof(tReal));
		printf("tBoolean: %d\n", sizeof(tBoolean));
	}

};
//---------------------------------------------------------------------------
typedef tRisseVariantBlock tRisseVariant;
//---------------------------------------------------------------------------
} // namespace Risse
#endif

