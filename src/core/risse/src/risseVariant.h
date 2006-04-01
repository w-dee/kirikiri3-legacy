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

tRisseString, tRisseObject, tRisseOctet の各先頭のメンバは必ず何かのポインタ
である。それらはメンバ Type とストレージを共有する。

少なくともそれらのポインタは4の倍数のアドレスにしか配置されないので、下位2
ビットは必ず遊んでいることになる。また、0x10 未満のような極端に低い番地にこ
れらのポインタが配置されることはあり得ない。

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

tRisseVariant はパフォーマンスの関係上、IL32 システムでは 3 * 32bit, LP64 シス
テムでは 2 * 64bit に収まるようにすること。
*/
//---------------------------------------------------------------------------
class tRisseVariant : public gc
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

	//! @beief bool ストレージ型
	struct tBool
	{
		risse_ptruint Type; //!< バリアントタイプ: 3 固定
		bool Value; //!< 値
	};

	//! @beief string ストレージ型
	struct tString
	{
		//! @brief Type と Value の共用体
		union
		{
			risse_ptruint Type; //!< バリアントタイプ: (Type & 0x03) == 0
			tRisseString Value; //!< 値
		};
	};

	//! @beief object ストレージ型
	struct tObject
	{
		//! @brief Type と Value の共用体
		union
		{
			risse_ptruint Type; //!< バリアントタイプ: (Type & 0x03) == 1
			tRisseObject Value; //!< 値
		};
	};

	//! @beief octet ストレージ型
	struct tOctet
	{
		//! @brief Type と Value の共用体
		union
		{
			risse_ptruint Type; //!< バリアントタイプ: (Type & 0x03) == 2
			tRisseOctet Value; //!< 値
		};
	};

	//! @brief 各バリアントの内部型の union
	union
	{
		risse_ptruint Type;	//!< バリアントタイプ
		tString String;		//!< string型
		tObject Object;		//!< object型
		tOctet Octet;		//!< octet型
		tVoid Void;			//!< void型
		tInteger Integer;	//!< integer型
		tReal Real;			//!< real型
		tBool Bool;			//!< bool型
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
		vtReserved		= 4 + 3,
	};

	//! @brief バリアントのタイプを得る
	//! @return バリアントのタイプ
	tType GetType() const
	{
		return (Type & 3) + ( (Type >= 4) << 2 );

		// 上記の行は以下の2行と同じ
		//	if(Type >= 4) return static_cast<tType>(Type & 3) + 4;
		//	return static_cast<tType>(Type);
	}

public: //コンストラクタ

	//! @brief デフォルトコンストラクタ(void型を作成)
	tRisseVariant()
	{
		Type = vtVoid;
	}

	//! @brief		コンストラクタ(string型を作成)
	//! @param		ref		元となる文字列
	tRisseVariant(const tRisseString & ref)
	{
		// Type の設定は必要なし
		String = ref;
	}

	//! @brief		コンストラクタ(object型を作成)
	//! @param		ref		元となるオブジェクト
	tRisseVariant(const tRisseObject & ref)
	{
		// Type の設定は必要なし
		Object = ref;
	}

	//! @brief		コンストラクタ(octet型を作成)
	//! @param		ref		元となるオクテット列
	tRisseVariant(const tRisseOctet & ref)
	{
		// Type の設定は必要なし
		Octet = ref;
	}

	//! @brief		コンストラクタ(integer型を作成)
	//! @param		ref		元となる整数
	tRisseVariant(const risse_int64 ref)
	{
		Type = vtInteger;
		Integer.Value = ref;
	}

	//! @brief		コンストラクタ(real型を作成)
	//! @param		ref		元となる実数
	tRisseVariant(const double ref)
	{
		Type = vtReal;
		Real.Value = ref;
	}

	//! @brief		コンストラクタ(bool型を作成)
	//! @param		ref		元となる真偽値
	tRisseVariant(const bool ref)
	{
		Type = vtBool;
		Bool.Value = ref;
	}

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#endif

