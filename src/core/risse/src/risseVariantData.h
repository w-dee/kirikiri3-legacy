//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バリアント型のデータ部分の実装
//---------------------------------------------------------------------------
#ifndef risseVariantDataH
#define risseVariantDataH

#include "risseGC.h"
#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseString.h"
#include "risseOctet.h"


namespace Risse
{
//---------------------------------------------------------------------------
class tRisseObjectInterface;
class tRissePrimitiveClassBase;
class tRisseVariantBlock;
//---------------------------------------------------------------------------
//! @brief	バリアント型のデータ部
/*! @note

tRisseStringData, tRisseVariantBlock::tObject, tRisseOctetData の各先頭
のメンバは必ず何かのポインタである。それらはメンバ Type とストレージを共有
する。このメンバ Type と共有を行ってる書くポインタはこれらは実際にはそれぞれ
tRisseString, tRisseObject, tRisseOctetとして扱われるが、データメンバのレイ
アウトは同一である。

各ポインタは4の倍数のアドレスにしか配置されないことがメモリアロケータの仕様
および risse_char のサイズにより保証されている。このため、下位2ビットは必ず遊
んでいることになる。また、0x100 未満のような極端に低い番地にこれらのポインタ
が配置されることはあり得ない。

そのため、tRisseVariant::GetType() を見ればわかるとおり、Type が 4 以上
ならば下位2ビット+4を Type とし、4 未満ならばそれをそのまま Type として返
している。ここら辺は ruby の実装からヒントを得た物。

tRisseString, tRisseVariantBlock::tObject, tRisseOctet 内にある各の「本当
の」ポインタを選るには、~0x03 との bit and をとればよい。

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
class tRisseVariantData : public tRisseCollectee
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
		risse_real Value; //!< 値
	};

	//! @brief boolean ストレージ型
	struct tBoolean
	{
		risse_ptruint Type; //!< バリアントタイプ: 0x42 = false, 0x72 = true
	};

	//! @brief boolean ストレージがfalseの時のTypeの値
	//! @note  この数字は最下位の2ビットが10である以外は特に意味はない
	static const risse_ptruint BooleanFalse = 0x42;
	//! @brief boolean ストレージがtrueの時のTypeの値
	static const risse_ptruint BooleanTrue = 0x72;

	//! @brief null ストレージ型
	struct tNull
	{
		risse_ptruint Type; //!< バリアントタイプ: 3 固定
	};

	//! @brief object ストレージ型
	struct tObject
	{
		tRisseObjectInterface * Intf; //!< オブジェクトインターフェースへのポインタ(下位の2ビットは常にObjectPointerBias)
		const tRisseVariantBlock * Context;
						//!< (Intfがメソッドオブジェクトやプロパティオブジェクトを
						//!< 指しているとして)メソッドが動作するコンテキスト
	};
public:
	static const int ObjectPointerBias = 3; //!< Variantに格納される際の Object 型のポインタのバイアス値

protected:
	//! @brief Integer型への参照を取得 @return Integer型フィールドへの参照
	risse_int64 & AsInteger() { return reinterpret_cast<tInteger*>(Storage)->Value; }
	//! @brief Integer型へのconst参照を取得 @return Integer型フィールドへのconst参照
	const risse_int64 & AsInteger() const { RISSE_ASSERT(GetType() == vtInteger); return reinterpret_cast<const tInteger*>(Storage)->Value; }

	//! @brief Real型への参照を取得 @return Real型フィールドへの参照
	risse_real & AsReal() { return reinterpret_cast<tReal*>(Storage)->Value; }
	//! @brief Real型へのconst参照を取得 @return Real型フィールドへのconst参照
	const risse_real & AsReal() const { RISSE_ASSERT(GetType() == vtReal); return reinterpret_cast<const tReal*>(Storage)->Value; }

	//! @brief String型への参照を取得 @return String型フィールドへの参照
	tRisseString & AsString() { return *reinterpret_cast<tRisseString*>(Storage); }
	//! @brief String型へのconst参照を取得 @return String型フィールドへのconst参照
	const tRisseString & AsString() const { RISSE_ASSERT(GetType() == vtString); return *reinterpret_cast<const tRisseString*>(Storage); }

	//! @brief Octet型への参照を取得 @return Octet型フィールドへの参照
	tRisseOctet & AsOctet() { return *reinterpret_cast<tRisseOctet*>(Storage); }
	//! @brief Octet型へのconst参照を取得 @return Octet型フィールドへのconst参照
	const tRisseOctet & AsOctet() const { RISSE_ASSERT(GetType() == vtOctet); return *reinterpret_cast<const tRisseOctet*>(Storage); }

	//! @brief Object型への参照を取得 @return Object型フィールドへの参照
	tObject & AsObject() { return *reinterpret_cast<tObject*>(Storage); }
	//! @brief Object型へのconst参照を取得 @return Object型フィールドへのconst参照
	const tObject & AsObject() const { RISSE_ASSERT(GetType() == vtObject); return *reinterpret_cast<const tObject*>(Storage); }

	//! @brief Boolean型への参照を取得 @return Boolean型フィールドへの参照
	tBoolean & AsBoolean() { return *reinterpret_cast<tBoolean*>(Storage); }
	//! @brief Boolean型へのconst参照を取得 @return Boolean型フィールドへのconst参照
	const tBoolean & AsBoolean() const { RISSE_ASSERT(GetType() == vtBoolean); return *reinterpret_cast<const tBoolean*>(Storage); }


public:
	//! @brief tRisseObjectInterfaceへのポインタを取得 @return tRisseObjectInterfaceへのポインタ
	//! @note Intfをいじる場合は常にこのメソッドを使うこと
	tRisseObjectInterface * GetObjectInterface() const
	{
		RISSE_ASSERT(GetType() == vtObject);
		tRisseObjectInterface * ret = reinterpret_cast<tRisseObjectInterface*>(
			reinterpret_cast<risse_ptruint>(AsObject().Intf) - ObjectPointerBias);
		// 2 = Intf の下位2ビットは常にObjectPointerBiasなので、これを元に戻す
		RISSE_ASSERT(ret != NULL);
		return ret;
	}

protected:
	//! @brief tRisseObjectInterfaceへのポインタを設定 @param intf tRisseObjectInterfaceへのポインタ
	//! @note Intfをいじる場合は常にこのメソッドを使うこと
	void SetObjectIntf(tRisseObjectInterface * intf)
	{
		RISSE_ASSERT(GetType() == vtObject);
		AsObject().Intf = reinterpret_cast<tRisseObjectInterface*>(
			reinterpret_cast<risse_ptruint>(intf) + ObjectPointerBias);
		// 2 = Intf の下位2ビットは常にObjectPointerBiasなので、これをたす
	}


protected:
	#define RV_SIZE_MAX(a, b) ((a)>(b)?(a):(b))
	#define RV_STORAGE_SIZE \
			RV_SIZE_MAX(sizeof(risse_ptruint),\
			RV_SIZE_MAX(sizeof(tVoid),        \
			RV_SIZE_MAX(sizeof(tInteger),     \
			RV_SIZE_MAX(sizeof(tReal),        \
			RV_SIZE_MAX(sizeof(tNull),        \
			RV_SIZE_MAX(sizeof(tRisseString), \
			RV_SIZE_MAX(sizeof(tRisseOctet),  \
			RV_SIZE_MAX(sizeof(tBoolean),     \
			RV_SIZE_MAX(sizeof(tObject),      \
					4                         \
			 )))))))))
			// ↑ 4 はダミー


	//! @brief 各バリアントの内部型の union
	union
	{
		risse_ptruint Type;	//!< バリアントタイプ

		//! @brief	データストレージ
		//! @note
		//! わざわざマクロで各構造体のサイズの最大値を得て、その要素数を
		//! もった char 配列を確保しているが(RV_STORAGE_SIZEの宣言を参照)、
		//! これは gcc など union で構造体を配置する際に、望ましくないパッキングを
		//! 行ってしまう可能性があるため。
		char Storage[RV_STORAGE_SIZE];
	};

public:
	//! @brief バリアントのタイプ
	enum tType
	{
		vtVoid			= 0,
		vtInteger		= 1,
		vtReal			= 2,
		vtNull			= 3,
		vtString		= 4 + 0,
		vtOctet			= 4 + 1,
		vtBoolean		= 4 + 2,
		vtObject		= 4 + 3,
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

	//! @brief バリアントがvoidかどうかを得る
	//! @return バリアントがvoidかどうか
	bool IsVoid() const
	{
		return Type == static_cast<risse_ptruint>(vtVoid);
		// 上記の行は以下の行と同じ
		// return GetType() == vtVoid;
	}

	//! @brief バリアントがnullかどうかを得る
	//! @return バリアントがnullかどうか
	bool IsNull() const
	{
		return Type == static_cast<risse_ptruint>(vtNull);
		// 上記の行は以下の行と同じ
		// return GetType() == vtNull;
	}

	//! @brief		内容を初期化する (void にする)
	//! @note		このメソッドは Type を vtVoid にすることにより
	//!				型を void にするだけである。内部のメンバが保持しているポインタなどを
	//!				破壊するわけではないので、参照はいまだ保持されたままになる可能性
	//!				があることに注意すること。
	void Clear()
	{
		Type = vtVoid;
	}

	//! @brief		内容をNULLにする
	//! @note		このメソッドは Type を vtNull にすることにより
	//!				型を null にするだけである。内部のメンバが保持しているポインタなどを
	//!				破壊するわけではないので、参照はいまだ保持されたままになる可能性
	//!				があることに注意すること。
	void Nullize()
	{
		Type = vtNull;
	}
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
