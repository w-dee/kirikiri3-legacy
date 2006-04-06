//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief バリアント型の実装
//---------------------------------------------------------------------------
#ifndef risseVariantH
#define risseVariantH

#include "gc_cpp.h"
#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseCxxObject.h"
#include "risseCxxString.h"
#include "risseCxxOctet.h"


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
class tRisseVariantBlock : public gc
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

	//! @brief bool ストレージ型
	struct tBool
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

	//! @brief Bool型への参照を取得 @return Bool型フィールドへの参照
	bool & AsBool() { return reinterpret_cast<tBool*>(Storage)->Value; }
	//! @brief Bool型へのconst参照を取得 @return Bool型フィールドへのconst参照
	const bool & AsBool() const { return reinterpret_cast<const tBool*>(Storage)->Value; }

	//! @brief String型への参照を取得 @return String型フィールドへの参照
	tRisseString & AsString() { return *reinterpret_cast<tRisseString*>(Storage); }
	//! @brief String型へのconst参照を取得 @return String型フィールドへのconst参照
	const tRisseString & AsString() const { return *reinterpret_cast<const tRisseString*>(Storage); }

	//! @brief Object型への参照を取得 @return Object型フィールドへの参照
	tRisseObject & AsObject() { return *reinterpret_cast<tRisseObject*>(Storage); }
	//! @brief Object型へのconst参照を取得 @return Object型フィールドへのconst参照
	const tRisseObject & AsObject() const { return *reinterpret_cast<const tRisseObject*>(Storage); }

	//! @brief Octet型への参照を取得 @return Octet型フィールドへの参照
	tRisseOctet & AsOctet() { return *reinterpret_cast<tRisseOctet*>(Storage); }
	//! @brief Octet型へのconst参照を取得 @return Octet型フィールドへのconst参照
	const tRisseOctet & AsOctet() const { return *reinterpret_cast<const tRisseOctet*>(Storage); }

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
			RV_SIZE_MAX(sizeof(tBool),
			RV_SIZE_MAX(sizeof(tRisseString),
			RV_SIZE_MAX(sizeof(tRisseObject),
			RV_SIZE_MAX(sizeof(tRisseOctet),
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
		vtBool			= 3,
		vtString		= 4 + 0,
		vtObject		= 4 + 1,
		vtOctet			= 4 + 2,
	//	vtReserved		= 4 + 3,
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
		case vtBool:		*this = ref.AsBool();		break;
		case vtString:		*this = ref.AsString();		break;
		case vtObject:		*this = ref.AsObject();		break;
		case vtOctet:		*this = ref.AsOctet();		break;
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
		Type = vtBool;
		AsBool() = ref;
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

public: // 演算子

	tRisseVariantBlock operator +(const tRisseVariantBlock & ref) const // incomplete
	{
		if(GetType() == vtString && ref.GetType() == vtString)
		{
			return tRisseVariantBlock(AsString() + AsString());
		}
		printf("fail: %d\n", sizeof(tRisseVariantBlock));
		return tRisseVariantBlock();
	}

	tRisseVariantBlock operator -() const // incomplete
	{
		switch(GetType())
		{
		case vtVoid:	return (risse_int64)0;					break; // -void => 0
		case vtInteger:	return -AsInteger();					break;
		case vtReal:	return -AsReal();						break;
		case vtBool:	return (risse_int64)(AsBool()?-1:0);	break;
/*
		not yet
		case vtString:		*this = 		break;
		case vtObject:		*this = ref.AsObject();		break;
		case vtOctet:		*this = ref.AsOctet();		break;
*/
		}
		return tRisseVariantBlock();
	}

public:
	void prtsizes()
	{
		printf("tRisseVariantBlock: %d\n", sizeof(tRisseVariantBlock));
		printf("Storage: %d\n", sizeof(Storage));
		printf("risse_ptruint: %d\n", sizeof(risse_ptruint));
		printf("tRisseString: %d\n", sizeof(tRisseString));
		printf("tRisseObject: %d\n", sizeof(tRisseObject));
		printf("tRisseOctet: %d\n", sizeof(tRisseOctet));
		printf("tVoid: %d\n", sizeof(tVoid));
		printf("tInteger: %d\n", sizeof(tInteger));
		printf("tReal: %d\n", sizeof(tReal));
		printf("tBool: %d\n", sizeof(tBool));
	}

};
//---------------------------------------------------------------------------
typedef tRisseVariantBlock tRisseVariant;
//---------------------------------------------------------------------------
} // namespace Risse
#endif

