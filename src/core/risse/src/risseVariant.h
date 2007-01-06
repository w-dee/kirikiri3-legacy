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
#ifndef risseVariantH
#define risseVariantH

#include "risseGC.h"
#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseString.h"
#include "risseOctet.h"
#include "risseException.h"
#include "risseMethod.h"
#include "risseOperateRetValue.h"
#include "risseObjectInterfaceArg.h"
#include "risseOpCodes.h"

namespace Risse
{


// 本来ならばtRisseStackFrameContextとtRisseMethodContextはrisseMethod.hにあるべき
// だがインクルード順の解決が難しいのでここに書く。
class tRisseVariantBlock;
typedef tRisseVariantBlock tRisseVariant;
//---------------------------------------------------------------------------
//! @brief		スタックフレームコンテキスト
//---------------------------------------------------------------------------
struct tRisseStackFrameContext
{
private:
	tRisseVariant *Frame; //!< スタックフレーム (NULL=スタックフレームを指定しない)
	tRisseVariant *Share; //!< 共有フレーム (NULL=共有フレームを指定しない)

public:
	//! @brief		デフォルトコンストラクタ
	tRisseStackFrameContext() { Frame = NULL; Share = NULL; }

	//! @brief		コンストラクタ(スタックフレームと共有フレームから)
	//! @param		frame		スタックフレーム
	//! @param		share		共有フレーム
	tRisseStackFrameContext(tRisseVariant * frame, tRisseVariant * share)
		: Frame(frame), Share(share) {;}

	//! @brief		スタックフレームを取得する
	//! @return		スタックフレーム
	tRisseVariant * GetFrame() const { return Frame; }

	//! @brief		スタックフレームを設定する
	//! @param		frame		スタックフレーム
	void SetFrame(tRisseVariant * frame) { Frame = frame; }

	//! @brief		共有フレームを取得する
	//! @return		共有フレーム
	tRisseVariant * GetShare() const { return Share; }

	//! @brief		共有フレームを設定する
	//! @param		share		共有フレーム
	void SetShare(tRisseVariant * share) { Share = share; }

private:
	struct tNullContext
	{
		// この構造体のバイナリレイアウトは tRisseStackFrameContext と同一で
		// ある必要がある
		tRisseVariant * Frame;
		tRisseVariant * Share;
	};
	static tNullContext NullContext;

public:
	//! @brief		static な null スタックフレームコンテキストを返す
	static const tRisseStackFrameContext & GetNullContext()
	{
		// tRisseStackFrameContextData から tRisseStackFrameContext へは
		// 安全にキャストできるはず
		return *reinterpret_cast<tRisseStackFrameContext*>(&NullContext);
	}
};
//---------------------------------------------------------------------------











/* ここからが真打ち */





//---------------------------------------------------------------------------
class tRisseObjectInterface;
class tRisseMethodContext;
//---------------------------------------------------------------------------
//! @brief	バリアント型
/*! @note

tRisseStringData, tRisseVariantBlock::tObject, tRisseOctetData の各先頭
のメンバは必ず何かのポインタである。それらはメンバ Type とストレージを共有
する。このメンバ Type と共有を行ってる書くポインタはこれらは実際にはそれぞれ
tRisseString, tRisseObject, tRisseOctetとして扱われるが、データメンバのレイ
アウトは同一である。

各ポインタは4の倍数のアドレスにしか配置されないことがメモリアロケータの仕様
および risse_char のサイズにより保証されている。このため、下位2ビットは必ず遊
んでいることになる。また、0x10 未満のような極端に低い番地にこれらのポインタ
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
class tRisseVariantBlock : public tRisseCollectee, tRisseOperateRetValue
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

	//! @brief object ストレージ型
	struct tObject
	{
		tRisseObjectInterface * Intf; //!< オブジェクトインターフェースへのポインタ(下位の2ビットは常に10)
		const tRisseMethodContext * Context;
						//!< (Intfがメソッドオブジェクトやプロパティオブジェクトを
						//!< 指しているとして)メソッドが動作するコンテキスト
	};
	#define RISSE_OBJECT_NULL_PTR (reinterpret_cast<tRisseObjectInterface*>((risse_ptruint)0x10))

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
	tObject & AsObject() { return *reinterpret_cast<tObject*>(Storage); }
	//! @brief Object型へのconst参照を取得 @return Object型フィールドへのconst参照
	const tObject & AsObject() const { return *reinterpret_cast<const tObject*>(Storage); }

public:
	//! @brief tRisseObjectInterfaceへのポインタを取得 @return tRisseObjectInterfaceへのポインタ
	//! @note Intfをいじる場合は常にこのメソッドを使うこと
	tRisseObjectInterface * GetObjectInterface() const
	{
		RISSE_ASSERT(GetType() == vtObject);
		tRisseObjectInterface * ret = reinterpret_cast<tRisseObjectInterface*>(
			reinterpret_cast<risse_ptruint>(AsObject().Intf) - 2);
		// 2 = Intf の下位2ビットは常に10なので、これを元に戻す
		if(ret == RISSE_OBJECT_NULL_PTR) return NULL;
			// "null"が入っていた場合はRISSE_OBJECT_NULL_PTRが得られるのでちゃんとNULLを返す
		return ret;
	}

protected:
	//! @brief tRisseObjectInterfaceへのポインタを設定 @param intf tRisseObjectInterfaceへのポインタ
	//! @note Intfをいじる場合は常にこのメソッドを使うこと
	void SetObjectIntf(tRisseObjectInterface * intf)
	{
		RISSE_ASSERT(GetType() == vtObject);
		if(!intf) intf = RISSE_OBJECT_NULL_PTR;
			// "null"の代わりにRISSE_OBJECT_NULL_PTRを使う
		AsObject().Intf = reinterpret_cast<tRisseObjectInterface*>(
			reinterpret_cast<risse_ptruint>(intf) + 2);
		// 2 = Intf の下位2ビットは常に10なので、これをたす
	}



	#define RV_SIZE_MAX(a, b) ((a)>(b)?(a):(b))
	#define RV_STORAGE_SIZE \
			RV_SIZE_MAX(sizeof(risse_ptruint),\
			RV_SIZE_MAX(sizeof(tVoid),        \
			RV_SIZE_MAX(sizeof(tInteger),     \
			RV_SIZE_MAX(sizeof(tReal),        \
			RV_SIZE_MAX(sizeof(tBoolean),     \
			RV_SIZE_MAX(sizeof(tRisseString), \
			RV_SIZE_MAX(sizeof(tRisseOctet),  \
			RV_SIZE_MAX(sizeof(tObject),      \
					4                         \
			 ))))))))
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

	//! @brief null値を表すstaticな領域
	struct tNullObject
	{
		risse_ptruint Type; //!< バリアントタイプ(= RISSE_OBJECT_NULL_PTR固定)
		char Storage[RV_STORAGE_SIZE - sizeof(risse_ptruint)]; //!< 残り(0で埋める) パディングは問題にならないはず
	};
	static tNullObject NullObject;

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
		Clear();
	}

	//! @brief		コピーコンストラクタ
	//! @param		ref		元となるオブジェクト
	tRisseVariantBlock(const tRisseVariantBlock & ref)
	{
		switch(ref.GetType())
		{
		case vtVoid:		Clear();					break;
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

	//! @brief		コンストラクタ(tRisseObjectInterface*型より)
	//! @param		ref		元となるオブジェクト
	tRisseVariantBlock(tRisseObjectInterface * ref)
	{
		* this = ref;
	}

	//! @brief		コンストラクタ(tRisseObjectInterface*型とコンテキストを表すtRisseMethodContext*型より)
	//! @param		ref		元となるオブジェクト(メソッドオブジェクトかプロパティオブジェクトを表す)
	//! @param		context	そのメソッドやプロパティが実行されるべきコンテキストを表す
	tRisseVariantBlock(tRisseObjectInterface * ref, const tRisseMethodContext * context)
	{
		Type = vtObject;
		SetObjectIntf(ref);
		AsObject().Context = context;
	}

	//! @brief		代入演算子(tRisseObjectInterface*型を代入)
	//! @param		ref		元となるオブジェクト
	tRisseVariantBlock & operator = (tRisseObjectInterface * ref)
	{
		// これはちょっと特殊
		Type = vtObject;
		SetObjectIntf(ref);
		AsObject().Context = NULL; // this は null に設定
		return *this;
	}

	//! @brief		代入演算子(tObject型を代入)
	//! @param		ref		元となるオブジェクト
	tRisseVariantBlock & operator = (const tObject & ref)
	{
		Type = vtObject;
		AsObject() = ref;
		return *this;
	}

public: // デストラクタ
	//! @brief		デストラクタ
	//! @note		このデストラクタは通常は呼ばれないことに注意すること。
	//!				また、ここに内容の破壊以外に意味のあるコードをかけることを期待しないこと。
	//!				tRisseHashTable などは明示的にin-placeでデストラクタを
	//!				呼ぶことにより内容が破壊され、メンバが参照しているポインタが
	//!				破壊されることにより参照のリンクが切れることを期待するので、それに
	//!				対応する。
	~tRisseVariantBlock()
	{
		Clear();
		AsObject().Context = NULL;
			// いまのところ、Type 以外の場所にポインタを持っているのは
			// Object の Context だけである。
			// ここでは Object 型かどうかはチェックしないが、このフィールド
			// はクリアしておかなければならない。
	}

public: // 初期化
	//! @brief		内容を初期化する (void にする)
	//! @note		このメソッドは Type を vtVoid にすることにより
	//!				型を void にするだけである。内部のメンバが保持しているポインタなどを
	//!				破壊するわけではないので、参照はいまだ保持されたままになる可能性
	//!				があることに注意すること。
	void Clear()
	{
		Type = vtVoid;
	}

public: // Object関連
	//! @brief		コンテキストを設定する
	//! @param		context	そのメソッドやプロパティが実行されるべきコンテキストを表す
	//! @note		このメソッドは、vtがvtObjectで、そのオブジェクトがメソッドオブジェクトやプロパティ
	//!				オブジェクトを表している場合に用いる。このメソッドはvtがvtObjectかどうかを
	//!				チェックしないので注意すること
	void SetContext(const tRisseMethodContext * context)
	{
		AsObject().Context = context;
	}

	//! @brief		オブジェクトが null かどうかを得る
	//! @note		型がオブジェクトで無かった場合は false を返す
	bool IsNull() const
	{
		if(GetType() != vtObject) return false;
		return GetObjectInterface() == NULL;
	}

	//! @brief		null オブジェクトを得る
	//! @return		null オブジェクトへのstaticなconst参照
	static const tRisseVariantBlock & GetNullObject()
	{
		return *reinterpret_cast<tRisseVariantBlock*>(&NullObject);
	}

public: // operate
	//! @brief		オブジェクトに対して操作を行う(失敗した場合は例外を発生させる)
	//! @param		code	オペレーションコード
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		bargs	ブロック引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	//! @note		何か操作に失敗した場合は例外が発生する。このため、このメソッドに
	//!				エラーコードなどの戻り値はない
	void Do(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
	{
		tRetValue ret = Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
		if(ret != rvNoError) RaiseError(ret, name);
	}

	//! @brief		オブジェクトに対して操作を行う
	//! @param		code	オペレーションコード
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		bargs	ブロック引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	//! @return		エラーコード
	tRetValue
		Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
	{
		if(!name.IsEmpty())
		{
			// 名前指定がある場合
			return OperateForMember(RISSE_OBJECTINTERFACE_PASS_ARG);
		}

		switch(code)
		{
		case ocNoOperation		://!< なにもしない
			return rvNoError;

		case ocNew				://!< "new"





		case ocLogNot			://!< "!" logical not
			if(result)
				*result = LogNot();
			else
				BitNot(); // discard result
			return rvNoError;

		case ocBitNot			://!< "~" bit not
			if(result)
				*result = BitNot();
			else
				BitNot(); // discard result
			return rvNoError;

		case ocDecAssign		://!< "--" decrement
			Dec();
			if(result) *result = *this;
			return rvNoError;

		case ocIncAssign		://!< "++" increment
			Inc();
			if(result) *result = *this;
			return rvNoError;

		case ocPlus				://!< "+"
			if(result)
				*result = Plus();
			else
				Plus(); // discard result
			return rvNoError;

		case ocMinus			://!< "-"
			if(result)
				*result = Minus();
			else
				Minus(); // discard result
			return rvNoError;

#define RISSE_BIN_OP(func) \
			if(args.GetCount() != 1)                           \
				RisseThrowBadArgumentCount(args.GetCount(), 1);\
			if(result)                                         \
				*result = BitOr(args[0]);                      \
			else                                               \
				BitOr(args[0]); /* discard result */           \
			return rvNoError;

		case ocLogOr			://!< ||
			RISSE_BIN_OP(LogOr);

		case ocLogAnd			://!< &&
			RISSE_BIN_OP(LogAnd);

		case ocBitOr			://!< |
			RISSE_BIN_OP(BitOr);

		case ocBitXor			://!< ^
			RISSE_BIN_OP(BitXor);

		case ocBitAnd			://!< &
			RISSE_BIN_OP(BitAnd);

		case ocNotEqual			://!< !=
			RISSE_BIN_OP(NotEqual);

		case ocEqual			://!< ==
			RISSE_BIN_OP(Equal);

		case ocDiscNotEqual		://!< !==
			RISSE_BIN_OP(DiscNotEqual);

		case ocDiscEqual		://!< ===
			RISSE_BIN_OP(DiscEqual);

		case ocLesser			://!< <
			RISSE_BIN_OP(Lesser);

		case ocGreater			://!< >
			RISSE_BIN_OP(Greater);

		case ocLesserOrEqual	://!< <=
			RISSE_BIN_OP(LesserOrEqual);

		case ocGreaterOrEqual	://!< >=
			RISSE_BIN_OP(GreaterOrEqual);

		case ocRBitShift		://!< >>>
			RISSE_BIN_OP(RBitShift);

		case ocLShift			://!< <<
			RISSE_BIN_OP(LShift);

		case ocRShift			://!< >>
			RISSE_BIN_OP(RShift);

		case ocMod				://!< %
			RISSE_BIN_OP(Mod);

		case ocDiv				://!< /
			RISSE_BIN_OP(Div);

		case ocIdiv				://!< \ (integer div)
			RISSE_BIN_OP(IDiv);

		case ocMul				://!< *
			RISSE_BIN_OP(Mul);

		case ocAdd				://!< +
			RISSE_BIN_OP(Add);

		case ocSub				://!< -
			RISSE_BIN_OP(Sub);

//		case ocDGet				://!< get .  
//			RISSE_BIN_OP(DGet);

//		case ocIGet				://!< get [ ]
//			RISSE_BIN_OP(IGet);

//		case ocDDelete			://!< delete .
//			RISSE_BIN_OP(DDelete);

//		case ocIDelete			://!< delete [ ]
//			RISSE_BIN_OP(IDelete);

//		case ocDSet				://!< set .
//			RISSE_BIN_OP(DSet);

//		case ocISet				://!< set [ ]
//			RISSE_BIN_OP(ISet);

#define RISSE_ASSIGN_OP(func) \
			if(args.GetCount() != 1)                           \
				RisseThrowBadArgumentCount(args.GetCount(), 1);\
			func(args[0]);                                     \
			if(result) *result = *this;                        \
			return rvNoError;

		case ocBitAndAssign		://!< &=
			RISSE_ASSIGN_OP(BitAndAssign);

		case ocBitOrAssign		://!< |=
			RISSE_ASSIGN_OP(BitOrAssign);

		case ocBitXorAssign		://!< ^=
			RISSE_ASSIGN_OP(BitXorAssign);

		case ocSubAssign		://!< -=
			RISSE_ASSIGN_OP(SubAssign);

		case ocAddAssign		://!< +=
			RISSE_ASSIGN_OP(AddAssign);

		case ocModAssign		://!< %=
			RISSE_ASSIGN_OP(ModAssign);

		case ocDivAssign		://!< /=
			RISSE_ASSIGN_OP(DivAssign);

		case ocIdivAssign		://!< \=
			RISSE_ASSIGN_OP(IdivAssign);

		case ocMulAssign		://!< *=
			RISSE_ASSIGN_OP(MulAssign);

		case ocLogOrAssign		://!< ||=
			RISSE_ASSIGN_OP(LogOrAssign)

		case ocLogAndAssign		://!< &&=
			RISSE_ASSIGN_OP(LogAndAssign)

		case ocRBitShiftAssign	://!< >>>=
			RISSE_ASSIGN_OP(RBitShiftAssign);

		case ocLShiftAssign		://!< <<=
			RISSE_ASSIGN_OP(LShiftAssign);

		case ocRShiftAssign		://!< >>=
			RISSE_ASSIGN_OP(RShiftAssign);

		default:
			// invalid opcode
			;
		}
		return  rvNoError;
	}

	//! @brief		オブジェクトのメンバに対して操作を行う
	//! @param		code	オペレーションコード
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		bargs	ブロック引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		stack	メソッドが実行されるべきスタックフレームコンテキスト
	//!						(NULL=スタックフレームコンテキストを指定しない場合)
	//! @note		Operate() メソッドがname付きで呼ばれた場合にこのメソッドが呼ばれる
	//! @return		エラーコード
	tRetValue
		OperateForMember(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

public: // 演算子

	/*
		演算子群は各型ごとに固有の計算方法があるため、組み合わせが
		膨大になる。ある程度をまとめる事はできるが、基本的にはしらみ
		つぶしにそれぞれを記述している。
	*/

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトに対する)関数呼び出し		FuncCall
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		args		引数
	//! @param		bargs		ブロック引数
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//-----------------------------------------------------------------------
	void FuncCall(tRisseVariantBlock * ret,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant & This)
	{
		// Object 以外は関数(メソッド)としては機能しないため
		// すべて 例外を発生する
		switch(GetType())
		{
		case vtObject:	FuncCall_Object   (ret, args, bargs, This); return;

		default:
			RisseThrowCannotCallNonFunctionObjectException(); break;
		}
	}

	void FuncCall_Object   (tRisseVariantBlock * ret,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant & This);

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトのメンバに対する)単純な関数呼び出し		Invoke
	//! @param		membername	メンバ名
	//! @return		戻り値
	//-----------------------------------------------------------------------
	tRisseVariantBlock Invoke(
		const tRisseString & membername) const
	{
		switch(GetType())
		{
		case vtVoid:	return Invoke_Void     (membername);
		case vtInteger:	return Invoke_Integer  (membername);
		case vtReal:	return Invoke_Real     (membername);
		case vtBoolean:	return Invoke_Boolean  (membername);
		case vtString:	return Invoke_String   (membername);
		case vtOctet:	return Invoke_Octet    (membername);
		case vtObject:	return Invoke_Object   (membername);
		}
		return tRisseVariantBlock();
	}

	tRisseVariantBlock Invoke_Void     (const tRisseString & membername) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_Integer  (const tRisseString & membername) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_Real     (const tRisseString & membername) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_Boolean  (const tRisseString & membername) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_String   (const tRisseString & membername) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_Octet    (const tRisseString & membername) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_Object   (const tRisseString & membername) const;

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトのメンバに対する)単純な関数呼び出し		Invoke
	//! @param		membername	メンバ名
	//! @param		arg1		引数
	//! @return		戻り値
	//-----------------------------------------------------------------------
	tRisseVariantBlock Invoke(
		const tRisseString & membername,
		const tRisseVariant & arg1) const
	{
		switch(GetType())
		{
		case vtVoid:	return Invoke_Void     (membername,arg1);
		case vtInteger:	return Invoke_Integer  (membername,arg1);
		case vtReal:	return Invoke_Real     (membername,arg1);
		case vtBoolean:	return Invoke_Boolean  (membername,arg1);
		case vtString:	return Invoke_String   (membername,arg1);
		case vtOctet:	return Invoke_Octet    (membername,arg1);
		case vtObject:	return Invoke_Object   (membername,arg1);
		}
		return tRisseVariantBlock();
	}

	tRisseVariantBlock Invoke_Void     (const tRisseString & membername,const tRisseVariant & arg1) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_Integer  (const tRisseString & membername,const tRisseVariant & arg1) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_Real     (const tRisseString & membername,const tRisseVariant & arg1) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_Boolean  (const tRisseString & membername,const tRisseVariant & arg1) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_String   (const tRisseString & membername,const tRisseVariant & arg1) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_Octet    (const tRisseString & membername,const tRisseVariant & arg1) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock Invoke_Object   (const tRisseString & membername,const tRisseVariant & arg1) const;

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
	tRisseVariantBlock BitNot_Object   () const { return Invoke(mnBitNot); }

	//-----------------------------------------------------------------------
	//! @brief		++ 演算子			Inc
	//! @return		演算結果(通常、+1 をした数値)
	//-----------------------------------------------------------------------
	tRisseVariantBlock & Inc()
	{
		switch(GetType())
		{
		case vtVoid:	return Inc_Void     ();
		case vtInteger:	return Inc_Integer  ();
		case vtReal:	return Inc_Real     ();
		case vtBoolean:	return Inc_Boolean  ();
		case vtString:	return Inc_String   ();
		case vtOctet:	return Inc_Octet    ();
		case vtObject:	return Inc_Object   ();
		}
		return *this;
	}

	tRisseVariantBlock & operator ++()    { /*前置*/ return Inc(); }
	tRisseVariantBlock   operator ++(int) { /*後置*/ tRisseVariantBlock t = *this; Inc(); return t;}

	tRisseVariantBlock & Inc_Void     () { *this = (risse_int64)1; /* void は 整数の 1になる */ return *this; }
	tRisseVariantBlock & Inc_Integer  () { *this = AsInteger() + 1; return *this; }
	tRisseVariantBlock & Inc_Real     () { *this = AsReal() + 1.0; return *this; }
	tRisseVariantBlock & Inc_Boolean  () { *this = (risse_int64)((int)AsBoolean() + 1); return *this; }
	tRisseVariantBlock & Inc_String   () { *this = tRisseVariantBlock((risse_int64)1).Add_Integer(Plus_String()); return *this; }
	tRisseVariantBlock & Inc_Octet    () { *this = (risse_int64)0; return *this; /* incomplete */; }
	tRisseVariantBlock & Inc_Object   () { *this = Invoke(mnAdd, tRisseVariantBlock((risse_int64)1)); return *this; }

	//-----------------------------------------------------------------------
	//! @brief		-- 演算子			Dec
	//! @return		演算結果(通常、-1 をした数値)
	//-----------------------------------------------------------------------
	tRisseVariantBlock & Dec()
	{
		switch(GetType())
		{
		case vtVoid:	return Dec_Void     ();
		case vtInteger:	return Dec_Integer  ();
		case vtReal:	return Dec_Real     ();
		case vtBoolean:	return Dec_Boolean  ();
		case vtString:	return Dec_String   ();
		case vtOctet:	return Dec_Octet    ();
		case vtObject:	return Dec_Object   ();
		}
		return *this;
	}

	tRisseVariantBlock & operator --()    { /*前置*/ return Dec(); }
	tRisseVariantBlock   operator --(int) { /*後置*/ tRisseVariantBlock t = *this; Dec(); return t;}

	tRisseVariantBlock & Dec_Void     () { *this = (risse_int64)-1; /* void は 整数の -1になる */ return *this; }
	tRisseVariantBlock & Dec_Integer  () { *this = AsInteger() - 1; return *this; }
	tRisseVariantBlock & Dec_Real     () { *this = AsReal() - 1.0; return *this; }
	tRisseVariantBlock & Dec_Boolean  () { *this = (risse_int64)((int)AsBoolean() - 1); return *this; }
	tRisseVariantBlock & Dec_String   () { *this = tRisseVariantBlock((risse_int64)-1).Add_Integer(Plus_String()); return *this; }
	tRisseVariantBlock & Dec_Octet    () { *this = (risse_int64)0; return *this; /* incomplete */; }
	tRisseVariantBlock & Dec_Object   () { *this = Invoke(mnSub, tRisseVariantBlock((risse_int64)1)); return *this; }


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
	tRisseVariantBlock Plus_Object   () const { return Invoke(mnPlus); }

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
	tRisseVariantBlock Minus_Object   () const { return Invoke(mnMinus); }

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
	//! @brief		||= 演算子		LogOrAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & LogOrAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->LogOr(rhs);
		return *this;
	}

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
	//! @brief		&&= 演算子		LogAndAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & LogAndAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->LogAnd(rhs);
		return *this;
	}

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
	tRisseVariantBlock BitOr_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnBitOr, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		|= 演算子		BitOrAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & BitOrAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->BitOr(rhs);
		return *this;
	}

	tRisseVariantBlock & operator |=(const tRisseVariantBlock & rhs) { return BitOrAssign(rhs); }

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
	tRisseVariantBlock BitXor_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnBitXor, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		^= 演算子		BitXorAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & BitXorAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->BitXor(rhs);
		return *this;
	}

	tRisseVariantBlock & operator ^=(const tRisseVariantBlock & rhs) { return BitXorAssign(rhs); }

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
	tRisseVariantBlock BitAnd_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnBitAnd, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		&= 演算子		BitAndAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & BitAndAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->BitAnd(rhs);
		return *this;
	}

	tRisseVariantBlock & operator &=(const tRisseVariantBlock & rhs) { return BitAndAssign(rhs); }

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
	bool NotEqual_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnNotEqual, rhs);  }

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

	bool Equal_Void     (const tRisseVariantBlock & rhs) const;
	bool Equal_Integer  (const tRisseVariantBlock & rhs) const;
	bool Equal_Real     (const tRisseVariantBlock & rhs) const;
	bool Equal_Boolean  (const tRisseVariantBlock & rhs) const;
	bool Equal_String   (const tRisseVariantBlock & rhs) const;
	bool Equal_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Equal_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnEqual, rhs); }

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
	bool DiscNotEqual_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnDiscNotEqual, rhs);  }

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
			{ return Invoke(mnDiscEqual, rhs); }

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

	bool Lesser_Void     (const tRisseVariantBlock & rhs) const;
	bool Lesser_Integer  (const tRisseVariantBlock & rhs) const;
	bool Lesser_Real     (const tRisseVariantBlock & rhs) const;
	bool Lesser_Boolean  (const tRisseVariantBlock & rhs) const;
	bool Lesser_String   (const tRisseVariantBlock & rhs) const;
	bool Lesser_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Lesser_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnLesser, rhs); }

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

	bool Greater_Void     (const tRisseVariantBlock & rhs) const;
	bool Greater_Integer  (const tRisseVariantBlock & rhs) const;
	bool Greater_Real     (const tRisseVariantBlock & rhs) const;
	bool Greater_Boolean  (const tRisseVariantBlock & rhs) const;
	bool Greater_String   (const tRisseVariantBlock & rhs) const;
	bool Greater_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Greater_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnGreater, rhs); }

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

	bool LesserOrEqual_Void     (const tRisseVariantBlock & rhs) const;
	bool LesserOrEqual_Integer  (const tRisseVariantBlock & rhs) const;
	bool LesserOrEqual_Real     (const tRisseVariantBlock & rhs) const;
	bool LesserOrEqual_Boolean  (const tRisseVariantBlock & rhs) const;
	bool LesserOrEqual_String   (const tRisseVariantBlock & rhs) const;
	bool LesserOrEqual_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool LesserOrEqual_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnLesserOrEqual, rhs); }


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

	bool GreaterOrEqual_Void     (const tRisseVariantBlock & rhs) const;
	bool GreaterOrEqual_Integer  (const tRisseVariantBlock & rhs) const;
	bool GreaterOrEqual_Real     (const tRisseVariantBlock & rhs) const;
	bool GreaterOrEqual_Boolean  (const tRisseVariantBlock & rhs) const;
	bool GreaterOrEqual_String   (const tRisseVariantBlock & rhs) const;
	bool GreaterOrEqual_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool GreaterOrEqual_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnGreaterOrEqual, rhs); }

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
	tRisseVariantBlock RBitShift_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnRBitShift, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		>>>= 演算子		RBitShiftAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & RBitShiftAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->RBitShift(rhs);
		return *this;
	}

	// 対応する C++ 演算子は無い

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
	tRisseVariantBlock LShift_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnLShift, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		<<= 演算子		LShiftAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & LShiftAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->LShift(rhs);
		return *this;
	}

	tRisseVariantBlock & operator <<=(const tRisseVariantBlock & rhs) { return LShiftAssign(rhs); }

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
	tRisseVariantBlock RShift_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnRShift, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		>>= 演算子		RShiftAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & RShiftAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->RShift(rhs);
		return *this;
	}

	tRisseVariantBlock & operator >>=(const tRisseVariantBlock & rhs) { return RShiftAssign(rhs); }

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
	tRisseVariantBlock Mod_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnMod, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		%= 演算子		ModAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & ModAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Mod(rhs);
		return *this;
	}

	tRisseVariantBlock & operator %=(const tRisseVariantBlock & rhs) { return ModAssign(rhs); }

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
	tRisseVariantBlock Div_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnDiv, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		/= 演算子		DivAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & DivAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Div(rhs);
		return *this;
	}

	tRisseVariantBlock & operator /=(const tRisseVariantBlock & rhs) { return DivAssign(rhs); }

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
	tRisseVariantBlock Idiv_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnIdiv, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		\= 演算子		IdivAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & IdivAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Idiv(rhs);
		return *this;
	}

	// 対応する C++ 演算子は無い

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

	tRisseVariantBlock Mul_Void     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Mul_Integer  (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Mul_Real     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Mul_Boolean  (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Mul_String   (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Mul_Octet    (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Mul_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnMul, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		*= 演算子		MulAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & MulAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Mul(rhs);
		return *this;
	}

	tRisseVariantBlock & operator *=(const tRisseVariantBlock & rhs) { return MulAssign(rhs); }

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

	tRisseVariantBlock Add_Void     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Add_Integer  (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Add_Real     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Add_Boolean  (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Add_String   (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Add_Octet    (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Add_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnAdd, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		+= 演算子		AddAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & AddAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Add(rhs);
		return *this;
	}

	tRisseVariantBlock & operator +=(const tRisseVariantBlock & rhs) { return AddAssign(rhs); }

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

	tRisseVariantBlock Sub_Void     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Sub_Integer  (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Sub_Real     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Sub_Boolean  (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Sub_String   (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Sub_Octet    (const tRisseVariantBlock & rhs) const { return (double)0; /* incomplete */ }
	tRisseVariantBlock Sub_Object   (const tRisseVariantBlock & rhs) const { return Invoke(mnSub, rhs); }

	//-----------------------------------------------------------------------
	//! @brief		-= 演算子		SubAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tRisseVariantBlock & SubAssign(const tRisseVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Sub(rhs);
		return *this;
	}

	tRisseVariantBlock & operator -=(const tRisseVariantBlock & rhs) { return SubAssign(rhs); }


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
	risse_int64 CastToInteger_Object   () const { return Invoke(mnInteger); }

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
	double CastToReal_Object   () const { return Invoke(mnReal); }

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
	bool CastToBoolean_Object   () const { return Invoke(mnBoolean); }

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
	tRisseString CastToString_Object   () const { return Invoke(mnString); }

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
		fprintf(stderr, "tRisseVariantBlock: %d\n", sizeof(tRisseVariantBlock));
		fprintf(stderr, "Storage: %d\n", sizeof(Storage));
		fprintf(stderr, "risse_ptruint: %d\n", sizeof(risse_ptruint));
		fprintf(stderr, "tRisseString: %d\n", sizeof(tRisseString));
		fprintf(stderr, "tRisseOctet: %d\n", sizeof(tRisseOctet));
		fprintf(stderr, "tObject: %d\n", sizeof(tObject));
		fprintf(stderr, "tVoid: %d\n", sizeof(tVoid));
		fprintf(stderr, "tInteger: %d\n", sizeof(tInteger));
		fprintf(stderr, "tReal: %d\n", sizeof(tReal));
		fprintf(stderr, "tBoolean: %d\n", sizeof(tBoolean));
	}

};
//---------------------------------------------------------------------------
typedef tRisseVariantBlock tRisseVariant;
//---------------------------------------------------------------------------












// 本来ならばtRisseStackFrameContextとtRisseMethodContextはrisseMethod.hにあるべき
// だがインクルード順の解決が難しいのでここに書く。

//---------------------------------------------------------------------------
//! @brief		メソッドのコンテキスト
//---------------------------------------------------------------------------
class tRisseMethodContext
{
private:
	tRisseVariant This; //!< "Thisオブジェクト" (NULL=Thisオブジェクトを指定しない)
	tRisseStackFrameContext Stack; //!< スタックフレームコンテキスト

public:
	//! @brief		コンストラクタ("Thisオブジェクト"から)
	//! @param		_This		"Thisオブジェクト"
	tRisseMethodContext(const tRisseVariant & _This) : This(_This) {;}

	//! @brief		コンストラクタ(スタックフレームコンテキストから)
	//! @brief		stack	スタックフレームコンテキスト
	tRisseMethodContext(const tRisseStackFrameContext & stack) : Stack(stack) {;}

	//! @brief		コンストラクタ("Thisオブジェクト"とスタックフレームと共有フレームから)
	//! @param		_This		"Thisオブジェクト"
	//! @brief		stack	スタックフレームコンテキスト
	tRisseMethodContext(const tRisseVariant & _This,
		const tRisseStackFrameContext & stack) : This(_This), Stack(stack) {;}

	//! @brief		"Thisオブジェクト" を得る
	const tRisseVariant & GetThis() const
	{
		return This;
	}

	//! @brief		"Thisオブジェクト" を得る
	//! @param		alt		"Thisオブジェクト" が null だった場合に使用される別のオブジェクト
	const tRisseVariant & GetThis(const tRisseVariant &alt) const
	{
		if(This.IsNull()) return alt;
		return This;
	}

	//! @brief		スタックフレームコンテキストを得る	
	//! @return		スタックフレームコンテキスト
	const tRisseStackFrameContext & GetStack() const { return Stack; }
};
//---------------------------------------------------------------------------





} // namespace Risse
#endif

