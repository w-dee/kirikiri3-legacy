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
class tObjectInterface;
class tPrimitiveClassBase;
class tVariantBlock;
//---------------------------------------------------------------------------
//! @brief	バリアント型のデータ部
/*! @note

tStringData, tVariantBlock::tObject, tOctetData の各先頭
のメンバは必ず何かのポインタである。それらはメンバ Type とストレージを共有
する。このメンバ Type と共有を行ってる書くポインタはこれらは実際にはそれぞれ
tString, tObject, tOctetとして扱われるが、データメンバのレイ
アウトは同一である。

各ポインタは4の倍数のアドレスにしか配置されないことがメモリアロケータの仕様
および risse_char のサイズにより保証されている。このため、下位2ビットは必ず遊
んでいることになる。また、0x100 未満のような極端に低い番地にこれらのポインタ
が配置されることはあり得ない。

そのため、tVariant::GetType() を見ればわかるとおり、Type が 4 以上
ならば下位2ビット+4を Type とし、4 未満ならばそれをそのまま Type として返
している。ここら辺は ruby の実装からヒントを得た物。

tString, tVariantBlock::tObject, tOctet 内にある各の「本当
の」ポインタを選るには、~0x03 との bit and をとればよい。

tString の内部ポインタが指し示している場所は、文字列を保持しているバッ
ファである。RisseではUTF-32文字列を対象とするため、このポインタが 32bit境界
にそろわないことはない。デバッガなどでそのまま UTF-32 文字列を表示したい用
途がある。tString の Type の条件は 0x04 以上かつ下位2ビットが00
であるため、内部ポインタの値と本来指し示しているポインタは同じになる。これに
より、内部ポインタが指し示すポインタがそのまま文字列バッファのポインタとなり、
そのままデバッガなどで内容を表示できる。

とりあえず tRiseVariant のサイズを抑えたいがための苦肉の策。こんなことをしな
い方が速いかもしれないし、こうした方が速いかもしれない。 

tVariant はパフォーマンスの関係上、ILP32 システムでは 3 * 32bit, LP64 シス
テムでは 2 * 64bit に収まるようにすること。

tVariantData::tVoid や tVariantData::tNull, tVariantData::tBoolean
の Ptr は、tObject においてポインタが入っている所である。
void や null や boolean にしたとき、ここにポインタが入っていると
GCによりマークし続けられてしまう。そのため、void や integer , boolean に
する際はここのポインタも破壊するようになっている。

スレッド保護はない。
*/
//---------------------------------------------------------------------------
class tVariantData : public tCollectee
{
protected:

	//! @brief void ストレージ型
	struct tVoid
	{
		risse_ptruint Type; //!< バリアントタイプ: 0 固定
		risse_ptruint Ptr; //!< 何らかのポインタ
	};

	//! @brief integer ストレージ型
	struct tInteger
	{
		risse_ptruint Type; //!< バリアントタイプ: 1 固定
		// ここに パディングが入っている場合アリ
		risse_int64  Value; //!< 値
	};

	//! @brief real ストレージ型
	struct tReal
	{
		risse_ptruint Type; //!< バリアントタイプ: 2 固定
		// ここに パディングが入っている場合アリ
		risse_real Value; //!< 値
	};

	//! @brief boolean ストレージ型
	struct tBoolean
	{
		risse_ptruint Type; //!< バリアントタイプ: 0x42 = false, 0x72 = true
		risse_ptruint Ptr; //!< 何らかのポインタ
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
		risse_ptruint Ptr; //!< 何らかのポインタ
	};

	//! @brief object ストレージ型
	struct tObject
	{
		tObjectInterface * Intf; //!< オブジェクトインターフェースへのポインタ(下位の2ビットは常にObjectPointerBias)
		const tVariantBlock * Context;
						//!< (Intfがメソッドオブジェクトやプロパティオブジェクトを
						//!< 指しているとして)メソッドが動作するコンテキスト
		//! @brief		厳密に同一かどうかを返す
		bool StrictEqual(const tObject & rhs) const
		{	return Intf == rhs.Intf && Context == rhs.Context; }
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
	tString & AsString() { return *reinterpret_cast<tString*>(Storage); }
	//! @brief String型へのconst参照を取得 @return String型フィールドへのconst参照
	const tString & AsString() const { RISSE_ASSERT(GetType() == vtString); return *reinterpret_cast<const tString*>(Storage); }

	//! @brief Octet型への参照を取得 @return Octet型フィールドへの参照
	tOctet & AsOctet() { return *reinterpret_cast<tOctet*>(Storage); }
	//! @brief Octet型へのconst参照を取得 @return Octet型フィールドへのconst参照
	const tOctet & AsOctet() const { RISSE_ASSERT(GetType() == vtOctet); return *reinterpret_cast<const tOctet*>(Storage); }

	//! @brief Object型への参照を取得 @return Object型フィールドへの参照
	tObject & AsObject() { return *reinterpret_cast<tObject*>(Storage); }
	//! @brief Object型へのconst参照を取得 @return Object型フィールドへのconst参照
	const tObject & AsObject() const { RISSE_ASSERT(GetType() == vtObject); return *reinterpret_cast<const tObject*>(Storage); }

	//! @brief Boolean型への参照を取得 @return Boolean型フィールドへの参照
	tBoolean & AsBoolean() { return *reinterpret_cast<tBoolean*>(Storage); }
	//! @brief Boolean型へのconst参照を取得 @return Boolean型フィールドへのconst参照
	const tBoolean & AsBoolean() const { RISSE_ASSERT(GetType() == vtBoolean); return *reinterpret_cast<const tBoolean*>(Storage); }


public:
	//! @brief tObjectInterfaceへのポインタを取得 @return tObjectInterfaceへのポインタ
	//! @note Intfをいじる場合は常にこのメソッドを使うこと
	tObjectInterface * GetObjectInterface() const
	{
		RISSE_ASSERT(GetType() == vtObject);
		tObjectInterface * ret = reinterpret_cast<tObjectInterface*>(
			reinterpret_cast<risse_ptruint>(AsObject().Intf) - ObjectPointerBias);
		// 2 = Intf の下位2ビットは常にObjectPointerBiasなので、これを元に戻す
		RISSE_ASSERT(ret != NULL);
		return ret;
	}

protected:
	//! @brief tObjectInterfaceへのポインタを設定 @param intf tObjectInterfaceへのポインタ
	//! @note Intfをいじる場合は常にこのメソッドを使うこと
	void SetObjectIntf(tObjectInterface * intf)
	{
		RISSE_ASSERT(GetType() == vtObject);
		AsObject().Intf = reinterpret_cast<tObjectInterface*>(
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
			RV_SIZE_MAX(sizeof(tString), \
			RV_SIZE_MAX(sizeof(tOctet),  \
			RV_SIZE_MAX(sizeof(tBoolean),     \
			RV_SIZE_MAX(sizeof(tObject),      \
					4                         \
			 )))))))))
			// ↑ 4 はダミー


	//! @brief 各バリアントの内部型の union
	union
	{
		struct
		{
			risse_ptruint Type;	//!< バリアントタイプ
			risse_ptruint Ptr; //!< なんらかのポインタ
		};

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

	//! @brief		ポインタ部分を破壊する
	void DestructPointer()
	{
		Ptr = 0;
	}

	//! @brief		内容を初期化する (void にする)
	void Clear()
	{
		Type = vtVoid;
		Ptr = 0; // ポインタをクリア
	}

	//! @brief		内容をNULLにする
	void Nullize()
	{
		Type = vtNull;
		Ptr = 0; // ポインタをクリア
	}
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
