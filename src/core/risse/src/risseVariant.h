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
		return !(bool)*this);
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
	risse_int64 CastToInteger_Octet    () const { return (risse_int64)0; /*incomplete */ }
	risse_int64 CastToInteger_Object   () const { return (risse_int64)0; /* incomplete */ }

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

