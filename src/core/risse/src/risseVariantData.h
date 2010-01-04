//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
#include "risseOperateRetValue.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tObjectInterface;
class tPrimitiveClassBase;
class tClassBase;
class tVariant;
//---------------------------------------------------------------------------
/**
 * バリアント型のデータ部
 */
/*! @note

tStringData, tVariant::tObject, tOctetData の各先頭
のメンバは必ず何かのポインタである。それらはメンバ Type とストレージを共有
する。このメンバ Type と共有を行ってる各ポインタはこれらは実際にはそれぞれ
tString, tObject, tOctetとして扱われるが、データメンバのレイ
アウトは同一である。

各ポインタは4の倍数のアドレスにしか配置されないことがメモリアロケータの仕様
および risse_char のサイズにより保証されている。このため、下位2ビットは必ず遊
んでいることになる。また、0x100 未満のような極端に低い番地にこれらのポインタ
が配置されることはあり得ない。

そのため、tVariant::GetType() を見ればわかるとおり、ポインタの値が 9 未満な
らばそそれをそのまま Type として扱い、そうでなければ下位2ビットを Type とし
て扱っている。ここら辺は ruby の実装からヒントを得た物。

tString, tVariant::tObject, tOctet 内にある各の「本当の」ポインタを選る
には、~0x03 との bit and をとればよい。

tString の内部ポインタが指し示している場所は、文字列を保持しているバッ
ファである。RisseではUTF-32文字列を対象とするため、このポインタが 32bit境界
にそろわないことはない。デバッガなどでそのまま UTF-32 文字列を表示したい用
途がある。tString の Type の条件は 0x10 以上かつ下位2ビットが00
であるため、内部ポインタの値と本来指し示しているポインタは同じになる。これに
より、内部ポインタが指し示すポインタがそのまま文字列バッファのポインタとなり、
そのままデバッガなどで内容を表示できる。

とりあえず tRiseVariant のサイズを抑えたいがための苦肉の策。こんなことをしな
い方が速いかもしれないし、こうした方が速いかもしれない。

tVariant はパフォーマンスの関係上、ILP32 システムでは 4 * 32bit, LP64 シス
テムでは 2 * 64bit に収まるようにすること。

tVariantData::tVoid や tVariantData::tNull, tVariantData::tBoolean
の Ptr は、tObject においてポインタが入っている所である。
void や null や boolean にしたとき、ここにポインタが入っていると
GCによりマークし続けられてしまう。そのため、void や integer , boolean に
する際はここのポインタも破壊するようになっている。

スレッド保護はない。
*/
//---------------------------------------------------------------------------
class tVariantData : public tCollectee, public tOperateRetValue
{
protected:

	/**
	 * void ストレージ型
	 */
	struct tVoid
	{
		risse_ptruint Type; //!< バリアントタイプ: 4 固定
		risse_ptruint Ptr; //!< 何らかのポインタ
	};

	/**
	 * integer ストレージ型
	 */
	struct tInteger
	{
		risse_ptruint Type; //!< バリアントタイプ: 5 固定
		// ここに パディングが入っている場合アリ
		risse_int64  Value; //!< 値
	};

	/**
	 * real ストレージ型
	 */
	struct tReal
	{
		risse_ptruint Type; //!< バリアントタイプ: 6 固定
		// ここに パディングが入っている場合アリ
		risse_real Value; //!< 値
	};

	/**
	 * boolean ストレージ型
	 */
	struct tBoolean
	{
		risse_ptruint Type; //!< バリアントタイプ
		bool Value; //!< 値
	};

	/**
	 * null ストレージ型
	 */
	struct tNull
	{
		risse_ptruint Type; //!< バリアントタイプ: 7 固定
		risse_ptruint Ptr; //!< 何らかのポインタ
	};

	/**
	 * data ストレージ型
	 */
	struct tData
	{
		tPrimitiveClassBase * Class; //!< クラスインスタンスへのポインタ(下位の2ビットは常にDataPointerBias)
		void * Data; //!< データ(なんでもよい。何を表しているかは Class が決める)
		/**
		 * 厳密に同一かどうかを返す
		 */
		bool StrictEqual(const tData & rhs) const
		{	return Class == rhs.Class && Data == rhs.Data; }
	};

	/**
	 * object ストレージ型
	 */
	struct tObject
	{
		tObjectInterface * Intf; //!< オブジェクトインターフェースへのポインタ(下位の2ビットは常にObjectPointerBias)
		const tVariant * Context;
						//!< (Intfがメソッドオブジェクトやプロパティオブジェクトを
						//!< 指しているとして)メソッドが動作するコンテキスト
		/**
		 * 厳密に同一かどうかを返す
		 */
		bool StrictEqual(const tObject & rhs) const
		{	return Intf == rhs.Intf && Context == rhs.Context; }
	};
public:
	static const int DataPointerBias = 2; //!< Variantに格納される際の Data 型のポインタのバイアス値
	static const int ObjectPointerBias = 3; //!< Variantに格納される際の Object 型のポインタのバイアス値

protected:
	/**
	 * Integer型への参照を取得 @return Integer型フィールドへの参照
	 */
	risse_int64 & AsInteger() { return reinterpret_cast<tInteger*>(Storage)->Value; }
	/**
	 * Integer型へのconst参照を取得 @return Integer型フィールドへのconst参照
	 */
	const risse_int64 & AsInteger() const { RISSE_ASSERT(GetType() == vtInteger); return reinterpret_cast<const tInteger*>(Storage)->Value; }

	/**
	 * Real型への参照を取得 @return Real型フィールドへの参照
	 */
	risse_real & AsReal() { return reinterpret_cast<tReal*>(Storage)->Value; }
	/**
	 * Real型へのconst参照を取得 @return Real型フィールドへのconst参照
	 */
	const risse_real & AsReal() const { RISSE_ASSERT(GetType() == vtReal); return reinterpret_cast<const tReal*>(Storage)->Value; }

	/**
	 * String型への参照を取得 @return String型フィールドへの参照
	 */
	tString & AsString() { return *reinterpret_cast<tString*>(Storage); }
	/**
	 * String型へのconst参照を取得 @return String型フィールドへのconst参照
	 */
	const tString & AsString() const { RISSE_ASSERT(GetType() == vtString); return *reinterpret_cast<const tString*>(Storage); }

	/**
	 * Octet型への参照を取得 @return Octet型フィールドへの参照
	 */
	tOctet & AsOctet() { return *reinterpret_cast<tOctet*>(Storage); }
	/**
	 * Octet型へのconst参照を取得 @return Octet型フィールドへのconst参照
	 */
	const tOctet & AsOctet() const { RISSE_ASSERT(GetType() == vtOctet); return *reinterpret_cast<const tOctet*>(Storage); }

	/**
	 * Data型への参照を取得 @return Data型フィールドへの参照
	 */
	tData & AsData() { return *reinterpret_cast<tData*>(Storage); }
	/**
	 * Data型へのconst参照を取得 @return Data型フィールドへのconst参照
	 */
	const tData & AsData() const { RISSE_ASSERT(GetType() == vtData); return *reinterpret_cast<const tData*>(Storage); }

	/**
	 * Object型への参照を取得 @return Object型フィールドへの参照
	 */
	tObject & AsObject() { return *reinterpret_cast<tObject*>(Storage); }
	/**
	 * Object型へのconst参照を取得 @return Object型フィールドへのconst参照
	 */
	const tObject & AsObject() const { RISSE_ASSERT(GetType() == vtObject); return *reinterpret_cast<const tObject*>(Storage); }

	/**
	 * Boolean型への参照を取得 @return Boolean型フィールドへの参照
	 */
	tBoolean & AsBoolean() { return *reinterpret_cast<tBoolean*>(Storage); }
	/**
	 * Boolean型へのconst参照を取得 @return Boolean型フィールドへのconst参照
	 */
	const tBoolean & AsBoolean() const { RISSE_ASSERT(GetType() == vtBoolean); return *reinterpret_cast<const tBoolean*>(Storage); }


public:
	/**
	 * vtDataのクラスインスタンスへのポインタを取得 @return クラスインスタンスへのポインタ
	 * @note	Classをいじる場合は常にこのメソッドを使うこと
	 */
	tPrimitiveClassBase * GetDataClassInstance() const
	{
		RISSE_ASSERT(GetType() == vtData);
		tPrimitiveClassBase * ret = reinterpret_cast<tPrimitiveClassBase*>(
			reinterpret_cast<risse_ptruint>(AsData().Class) - DataPointerBias);
		// Intf の下位2ビットは常にDataPointerBiasなので、これを元に戻す
		RISSE_ASSERT(ret != NULL);
		return ret;
	}

protected:
	/**
	 * vtDataのクラスインスタンスへのポインタを設定 @param Class vtDataのクラスインスタンスへのポインタ
	 * @note	Classをいじる場合は常にこのメソッドを使うこと
	 */
	void SetDataClassInstance(tPrimitiveClassBase * Class)
	{
		RISSE_ASSERT(GetType() == vtData);
		RISSE_ASSERT(Class != NULL);
		AsData().Class = reinterpret_cast<tPrimitiveClassBase*>(
			reinterpret_cast<risse_ptruint>(Class) + DataPointerBias);
		// Intf の下位2ビットは常にDataPointerBiasなので、これをたす
	}

public:
	/**
	 * tObjectInterfaceへのポインタを取得 @return tObjectInterfaceへのポインタ
	 * @note	Intfをいじる場合は常にこのメソッドを使うこと
	 */
	tObjectInterface * GetObjectInterface() const
	{
		RISSE_ASSERT(GetType() == vtObject);
		tObjectInterface * ret = reinterpret_cast<tObjectInterface*>(
			reinterpret_cast<risse_ptruint>(AsObject().Intf) - ObjectPointerBias);
		// Intf の下位2ビットは常にObjectPointerBiasなので、これを元に戻す
		RISSE_ASSERT(ret != NULL);
		return ret;
	}

protected:
	/**
	 * tObjectInterfaceへのポインタを設定 @param intf tObjectInterfaceへのポインタ
	 * @note	Intfをいじる場合は常にこのメソッドを使うこと
	 */
	void SetObjectIntf(tObjectInterface * intf)
	{
		RISSE_ASSERT(GetType() == vtObject);
		RISSE_ASSERT(intf != NULL);
		AsObject().Intf = reinterpret_cast<tObjectInterface*>(
			reinterpret_cast<risse_ptruint>(intf) + ObjectPointerBias);
		// Intf の下位2ビットは常にObjectPointerBiasなので、これをたす
	}



protected:
	#define RV_SIZE_MAX(a, b) ((a)>(b)?(a):(b))
	enum ___dummy
	{
		RV_S0 = RV_SIZE_MAX(0,		sizeof(risse_ptruint)),
		RV_S1 = RV_SIZE_MAX(RV_S0,	sizeof(tVoid)),
		RV_S2 = RV_SIZE_MAX(RV_S1,	sizeof(tInteger)),
		RV_S3 = RV_SIZE_MAX(RV_S2,	sizeof(tReal)),
		RV_S4 = RV_SIZE_MAX(RV_S3,	sizeof(tNull)),
		RV_S5 = RV_SIZE_MAX(RV_S4,	sizeof(tBoolean)),
		RV_S6 = RV_SIZE_MAX(RV_S5,	sizeof(tString)),
		RV_S7 = RV_SIZE_MAX(RV_S6,	sizeof(tOctet)),
		RV_S8 = RV_SIZE_MAX(RV_S7,	sizeof(tData)),
		RV_S9 = RV_SIZE_MAX(RV_S8,	sizeof(tObject)),
		RV_STORAGE_SIZE = RV_S9
	};


	/**
	 * 各バリアントの内部型の union
	 */
	union
	{
		struct
		{
			risse_ptruint Type;	//!< バリアントタイプ
			risse_ptruint Ptr; //!< なんらかのポインタ
		};

		/**
		 * データストレージ
		 * わざわざマクロで各構造体のサイズの最大値を得て、その要素数を
		 * もった char 配列を確保しているが(RV_STORAGE_SIZEの宣言を参照)、
		 * これは gcc など union で構造体を配置する際に、望ましくないパッキングを
		 * 行ってしまう可能性があるため。
		 */
		char Storage[RV_STORAGE_SIZE];
	};

public:
	/**
	 * バリアントのタイプ
	 */
	enum tType
	{
		vtVoid			= 4,
		vtInteger		= 5,
		vtReal			= 6,
		vtNull			= 7,
		vtBoolean		= 8,
		vtString		= 0,
		vtOctet			= 1,
		vtData			= 2,
		vtObject		= 3,
	};

	/**
	 * バリアントのタイプを得る
	 * @return	バリアントのタイプ
	 */
	tType GetType() const
	{
		return static_cast<tType>(Type < 9 ? Type : (Type & 3));
	}

	/**
	 * バリアントがvoidかどうかを得る
	 * @return	バリアントがvoidかどうか
	 */
	bool IsVoid() const
	{
		return Type == static_cast<risse_ptruint>(vtVoid);
		// 上記の行は以下の行と同じ
		// return GetType() == vtVoid;
	}

	/**
	 * バリアントがnullかどうかを得る
	 * @return	バリアントがnullかどうか
	 */
	bool IsNull() const
	{
		return Type == static_cast<risse_ptruint>(vtNull);
		// 上記の行は以下の行と同じ
		// return GetType() == vtNull;
	}

	/**
	 * ポインタ部分を破壊する
	 * @note	GCがポインタに指された先を回収しないと困る場合は、
	 *			これを呼んでポインタ部分を破壊し、参照を破棄する
	 */
	void DestructPointer()
	{
		Ptr = 0;
	}

	/**
	 * 内容を初期化する (void にする)
	 */
	void Clear()
	{
		Type = vtVoid;
		Ptr = 0; // ポインタをクリア
	}

	/**
	 * 内容をNULLにする
	 */
	void Nullize()
	{
		Type = vtNull;
		Ptr = 0; // ポインタをクリア
	}
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
