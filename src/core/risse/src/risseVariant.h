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

#include "risseVariantData.h"
#include "risseMethod.h"
#include "risseOperateRetValue.h"
#include "risseObjectInterfaceArg.h"
#include "risseOpCodes.h"

namespace Risse
{
class tRisseScriptBlockBase;
//---------------------------------------------------------------------------
//! @brief	バリアント型
//! @note	tRisseVariantData よりも高度な動作をここで定義する
//---------------------------------------------------------------------------
class tRisseVariantBlock : public tRisseVariantData, public tRisseOperateRetValue
{
private:
	//! @brief		CannotCreateInstanceFromNonClassObjectException を投げる
	//! @note		本来 risseExceptionClass.h に書いてある物だが
	//!				ここの位置からは参照できないのでわざわざワンクッションを置く
	static void ThrowCannotCreateInstanceFromNonClassObjectException();

	//! @brief		BadContextException を投げる
	//! @note		本来 risseExceptionClass.h に書いてある物だが
	//!				ここの位置からは参照できないのでわざわざワンクッションを置く
	static void ThrowBadContextException();

	//! @brief		NoSuchMemberException を投げる
	//! @param		name		メンバ名
	//! @note		本来 risseExceptionClass.h に書いてある物だが
	//!				ここの位置からは参照できないのでわざわざワンクッションを置く
	static void ThrowNoSuchMemberException(const tRisseString & name);


private: // static オブジェクト
	//! @brief	null/void/などの特殊な値を表すstaticな領域
	struct tStaticPrimitive
	{
		risse_ptruint Type; //!< バリアントタイプ
		char Storage[RV_STORAGE_SIZE - sizeof(risse_ptruint)];
			//!< 残り(0で埋める) パディングは問題にならないはず
	};
	static tStaticPrimitive VoidObject;
	static tStaticPrimitive NullObject;

	//! @brief	DynamicContext などの特殊な値を表す static な領域
	struct tStaticObject
	{
		risse_ptruint Intf; //!< オブジェクトインターフェースへのポインタ
		const tRisseVariantBlock * Context; //!< (Intfがメソッドオブジェクトやプロパティオブジェクトを
						//!< 指しているとして)メソッドが動作するコンテキスト
		char Storage[RV_STORAGE_SIZE - sizeof(risse_ptruint) - sizeof(const tRisseVariantBlock *)];
			//!< 残り(0で埋める) パディングは問題にならないはず
	};
	static tStaticObject DynamicContext;

public: // static オブジェクト
	//! @brief		void オブジェクトを得る
	//! @return		void オブジェクトへのstaticなconst参照
	static const tRisseVariantBlock & GetVoidObject()
	{
		return *reinterpret_cast<tRisseVariantBlock*>(&VoidObject);
	}

	//! @brief		null オブジェクトを得る
	//! @return		null オブジェクトへのstaticなconst参照
	static const tRisseVariantBlock & GetNullObject()
	{
		return *reinterpret_cast<tRisseVariantBlock*>(&NullObject);
	}

	//! @brief		DynamicContext オブジェクトを得る
	static const tRisseVariantBlock * GetDynamicContext()
	{
		return reinterpret_cast<tRisseVariantBlock*>(&DynamicContext);
	}

public: // バリアントタイプ
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
		case vtNull:		Nullize();					break;
		case vtString:		*this = ref.AsString();		break;
		case vtOctet:		*this = ref.AsOctet();		break;
		case vtBoolean:		Type = ref.Type;			break;
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
	tRisseVariantBlock(const risse_real ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(real型を代入)
	//! @param		ref		元となる実数
	tRisseVariantBlock & operator = (const risse_real ref)
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
		Type = ref?BooleanTrue:BooleanFalse;
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

	//! @brief		コンストラクタ(string型を作成)
	//! @param		ref		元となる文字列
	tRisseVariantBlock(const risse_char * ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(string型を代入)
	//! @param		ref		元となる文字列
	//! @return		このオブジェクトへの参照
	tRisseVariantBlock & operator = (const risse_char * ref)
	{
		// Type の設定は必要なし
		AsString() = tRisseString(ref);
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

	//! @brief		コンストラクタ(tRisseObjectInterface*型とコンテキストを表すtRisseVariant型より)
	//! @param		ref		元となるオブジェクト(メソッドオブジェクトかプロパティオブジェクトを表す)
	//! @param		context	そのメソッドやプロパティが実行されるべきコンテキストオブジェクトを表す
	tRisseVariantBlock(tRisseObjectInterface * ref, const tRisseVariantBlock * context)
	{
		Type = vtObject;
		SetObjectIntf(ref);
		RISSE_ASSERT(context != NULL);
		AsObject().Context = context;
	}

	//! @brief		代入演算子(tRisseObjectInterface*型を代入)
	//! @param		ref		元となるオブジェクト
	tRisseVariantBlock & operator = (tRisseObjectInterface * ref)
	{
		// これはちょっと特殊
		Type = vtObject;
		SetObjectIntf(ref);
		AsObject().Context = GetDynamicContext(); // this は DynamicContext に設定
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
		// TODO: デストラクタがあるとスタック巻き戻しのための例外処理コードが追加
		//		されてしまうのであまりよくない。tRisseHashTable などが明示的に
		//		何らかのクリアコードを呼ぶように改造すべき。このデストラクタは
		//		なくなるべき。
		Clear();
		AsObject().Context = NULL;
			// いまのところ、Type 以外の場所にポインタを持っているのは
			// Object の Context だけである。
			// ここでは Object 型かどうかはチェックしないが、このフィールド
			// はクリアしておかなければならない。
	}

public: // String関連
	//! @brief		文字列が空文字列かどうかを得る
	//! @note		文字列が空の場合に真が帰るが、vt が vtString 以外の場合は
	//!				(vtVoidでも) 偽を返すので注意
	bool IsEmptyString() const
	{
		return GetType() == vtString && AsString().IsEmpty();
	}

public: // Primitive関連
	//! @brief		プリミティブ型に即してプリミティブ型クラスを得る
	tRissePrimitiveClassBase * GetPrimitiveClass() const;

public: // Object関連
	//! @brief		オブジェクトインターフェースがマッチするかどうかを調べる
	//! @param		rhs			右辺値
	//! @return		マッチしたかどうか
	//! @note		このメソッドは、(1)thisとrhsが両方ともvtObjectかつ(2)オブジェクトインターフェース
	//!				ポインタが同一であるかどうかを調べる
	bool ObjectInterfaceMatch(const tRisseVariant & rhs) const
	{
		if(GetType() != vtObject) return false;
		if(rhs.GetType() != vtObject) return false;
		return AsObject().Intf == rhs.AsObject().Intf;
	}

	//! @brief		コンテキストを設定する
	//! @param		context	そのメソッドやプロパティが実行されるべきコンテキストを表す
	//! @note		このメソッドは、vtがvtObjectで、そのオブジェクトがメソッドオブジェクトやプロパティ
	//!				オブジェクトを表している場合に用いる。このメソッドはvtがvtObjectかどうかを
	//!				チェックしないので注意すること。@n
	//!				DynamicContextを指定する場合はGetDynamicContext()の戻りを指定すること。
	void SetContext(const tRisseVariantBlock * context)
	{
		RISSE_ASSERT(GetType() == vtObject); // チェックはしないとはいうものの一応ASSERTはする
		RISSE_ASSERT(context != NULL);
		AsObject().Context = context;
	}

	//! @brief		コンテキストを設定する
	//! @param		context	そのメソッドやプロパティが実行されるべきコンテキストを表す
	//! @note		このメソッドは、vtがvtObjectで、そのオブジェクトがメソッドオブジェクトやプロパティ
	//!				オブジェクトを表している場合に用いる。このメソッドはvtがvtObjectかどうかを
	//!				チェックしないので注意すること。@n
	//!				DynamicContextを指定する場合はGetDynamicContext()の戻りを指定すること。@n
	//!				このメソッドは const tRisseVariantBlock * context を引数に取る版とちがい、
	//!				context がどうやら dynamic コンテキストらしい場合は自動的に
	//!				GetDynamicContext() の戻りに変換する。そうでない場合は
	//!				tRisseVariantBlock を new してそのポインタを SetContext() で設定する。
	void SetContext(const tRisseVariantBlock &context);

	//! @brief		コンテキストを取得する
	//! @return		そのメソッドやプロパティが実行されるべきコンテキスト
	//! @note		このメソッドはvtがvtObjectかどうかを
	//!				チェックしないので注意すること
	const tRisseVariantBlock * GetContext() const
	{
		RISSE_ASSERT(GetType() == vtObject); // チェックはしないとはいうものの一応ASSERTはする
		return AsObject().Context;
	}

	//! @brief		コンテキストを持っているかどうかを得る
	//! @note		このメソッドはvtがvtObject以外の場合はtrueを返す。
	//!				コンテキストを持っている(コンテキストが dynamic でない)場合に真を返す
	bool HasContext() const
	{
		if(GetType() != vtObject) return true;
		const tObject & obj = AsObject();
		RISSE_ASSERT(obj.Context != NULL);
		return obj.Context != GetDynamicContext();
	}

	//! @brief		このオブジェクトが DynamicContext かどうかを返す
	//! @return		このオブジェクトが DynamicContext かどうか
	bool IsDynamicContext() const
	{
		if(GetType() != vtObject) return false;
		// オブジェクトインターフェースのポインタが同一かどうかを得る
		return AsObject().Intf == GetDynamicContext()->AsObject().Intf;
	}

	//! @brief		コンテキストを上書きする
	//! @param		context	上書きするコンテキスト
	//! @note		このメソッドはvtがvtObject以外の場合はなにもしない。コンテキストの上書きは、
	//!				このオブジェクトのコンテキストが設定されいていない場合のみに発生する。
	void OverwriteContext(const tRisseVariantBlock * context)
	{
		RISSE_ASSERT(context != NULL);
		if(!HasContext()) SetContext(context);
	}

	//! @brief		コンテキストを選択する
	//! @param		flags		フラグ
	//! @param		This		コンテキストを持っていなかった場合に返されるコンテキスト
	//! @return		選択されたコンテキスト
	//! @note		このメソッドはvtがvtObjectかどうかをチェックしないので注意。
	//!				この値がコンテキストを持っていればそのコンテキストを返すが、そうでない場合は This を返す。
	//!				ただし、フラグに ofUseThisAsContext が指定されていた場合は常に This を返す
	const tRisseVariantBlock & SelectContext(risse_uint32 flags, const tRisseVariantBlock & This) const
	{
		RISSE_ASSERT(GetType() == vtObject); // チェックはしないとはいうものの一応ASSERTはする
		if(!(flags & tRisseOperateFlags::ofUseThisAsContext))
		{
			const tObject & obj = AsObject();
			RISSE_ASSERT(obj.Context != NULL);
			if(obj.Context != GetDynamicContext())
				return *obj.Context;
		}
		return This;
	}

public: // operate
	//! @brief		オブジェクトに対して操作を行う(失敗した場合は例外を発生させる)
	//! @param		code	オペレーションコード
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
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
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
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
			args.ExpectArgumentCount(1);                              \
			if(result)                                                \
				*result = func(args[0]);                              \
			else                                                      \
				func(args[0]); /* discard result */                   \
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
			RISSE_BIN_OP(Idiv);

		case ocMul				://!< *
			RISSE_BIN_OP(Mul);

		case ocAdd				://!< +
			RISSE_BIN_OP(Add);

		case ocSub				://!< -
			RISSE_BIN_OP(Sub);

		case ocInstanceOf		://!< instanceof
			RISSE_BIN_OP(InstanceOf);

//		case ocDGet				://!< get .  
//			RISSE_BIN_OP(DGet);

		case ocIGet				://!< get [ ]
			RISSE_BIN_OP(IGet);

//		case ocDDelete			://!< delete .
//			RISSE_BIN_OP(DDelete);

//		case ocIDelete			://!< delete [ ]
//			RISSE_BIN_OP(IDelete);

//		case ocDSet				://!< set .
//			RISSE_BIN_OP(DSet);

		case ocISet				://!< set [ ]
			args.ExpectArgumentCount(2);
			ISet(args[0], args[1]);
			return rvNoError;

#define RISSE_ASSIGN_OP(func) \
			args.ExpectArgumentCount(1);                       \
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
			// invalid or unknown opcode
			return OperateForMember(RISSE_OBJECTINTERFACE_PASS_ARG);
				// OperateForMemberならば処理ができるかもしれない
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
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
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
	//! @brief		直接プロパティ取得		GetPropertyDirect dget
	//! @param		name		メンバ名
	//! @param		flags		フラグ
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//! @return		プロパティ取得の結果
	//-----------------------------------------------------------------------
	tRisseVariantBlock GetPropertyDirect(const tRisseString & name,
		risse_uint32 flags = 0,
		const tRisseVariant & This = tRisseVariant::GetNullObject()) const
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
			return GetPropertyDirect_Primitive(name, flags, This);
		case vtObject:
			return GetPropertyDirect_Object   (name, flags, This);
		}
		return tRisseVariant();
	}

	tRisseVariantBlock GetPropertyDirect_Primitive(const tRisseString & name, risse_uint32 flags, const tRisseVariant & This) const ;
	tRisseVariantBlock GetPropertyDirect_Object   (const tRisseString & name, risse_uint32 flags, const tRisseVariant & This) const ;

	//-----------------------------------------------------------------------
	//! @brief		直接プロパティ設定		SetPropertyDirect dset
	//! @param		name		メンバ名
	//! @param		value		設定する値
	//! @param		flags		フラグ
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//-----------------------------------------------------------------------
	void SetPropertyDirect(const tRisseString & name, risse_uint32 flags,
		const tRisseVariantBlock & value, const tRisseVariant & This = tRisseVariant::GetNullObject()) const
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
			SetPropertyDirect_Primitive(name, flags, value, This); return;
		case vtObject:
			SetPropertyDirect_Object   (name, flags, value, This); return;
		}
	}

	void SetPropertyDirect_Primitive(const tRisseString & name, risse_uint32 flags, const tRisseVariantBlock & value, const tRisseVariant & This) const;
	void SetPropertyDirect_Object   (const tRisseString & name, risse_uint32 flags, const tRisseVariantBlock & value, const tRisseVariant & This) const;

	//-----------------------------------------------------------------------
	//! @brief		間接プロパティ取得		IGet iget
	//! @param		key		キー
	//! @return		プロパティ取得の結果
	//-----------------------------------------------------------------------
	tRisseVariantBlock IGet(const tRisseVariantBlock & key) const
	{
		switch(GetType())
		{
		case vtVoid:	return IGet_Void     (key);
		case vtInteger:	return IGet_Integer  (key);
		case vtReal:	return IGet_Real     (key);
		case vtNull:	return IGet_Null     (key);
		case vtString:	return IGet_String   (key);
		case vtOctet:	return IGet_Octet    (key);
		case vtBoolean:	return IGet_Boolean  (key);
		case vtObject:	return IGet_Object   (key);
		}
		return tRisseVariant();
	}

	tRisseVariantBlock IGet_Void    (const tRisseVariantBlock & key) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock IGet_Integer (const tRisseVariantBlock & key) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock IGet_Real    (const tRisseVariantBlock & key) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock IGet_Null    (const tRisseVariantBlock & key) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock IGet_String  (const tRisseVariantBlock & key) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock IGet_Octet   (const tRisseVariantBlock & key) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock IGet_Boolean (const tRisseVariantBlock & key) const { return tRisseVariant(); /* incomplete */ }
	tRisseVariantBlock IGet_Object  (const tRisseVariantBlock & key) const { return Invoke(mnIGet, key); }

	//-----------------------------------------------------------------------
	//! @brief		間接プロパティ設定		ISet iset
	//! @param		key			キー
	//! @param		value		設定する値
	//-----------------------------------------------------------------------
	void ISet(const tRisseVariantBlock & key, const tRisseVariantBlock & value) const
	{
		switch(GetType())
		{
		case vtVoid:	ISet_Void     (key, value); return;
		case vtInteger:	ISet_Integer  (key, value); return;
		case vtReal:	ISet_Real     (key, value); return;
		case vtNull:	ISet_Null     (key, value); return;
		case vtString:	ISet_String   (key, value); return;
		case vtOctet:	ISet_Octet    (key, value); return;
		case vtBoolean:	ISet_Boolean  (key, value); return;
		case vtObject:	ISet_Object   (key, value); return;
		}
	}

	void ISet_Void    (const tRisseVariantBlock & key, const tRisseVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Integer (const tRisseVariantBlock & key, const tRisseVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Real    (const tRisseVariantBlock & key, const tRisseVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Null    (const tRisseVariantBlock & key, const tRisseVariantBlock & value) const { return; /* incomplete */ }
	void ISet_String  (const tRisseVariantBlock & key, const tRisseVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Octet   (const tRisseVariantBlock & key, const tRisseVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Boolean (const tRisseVariantBlock & key, const tRisseVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Object  (const tRisseVariantBlock & key, const tRisseVariantBlock & value) const { Invoke(mnISet, value, key);
		/* 注意!!! ISet がメソッド呼び出しに変換される場合、value が先に来て key が後に来る。これは将来的に
		複数の key を使用可能にする可能性があるためである */ }

	//-----------------------------------------------------------------------
	//! @brief		直接プロパティ削除		DeletePropertyDirect ddelete
	//! @param		name		メンバ名
	//! @param		flags		フラグ
	//-----------------------------------------------------------------------
	void DeletePropertyDirect(const tRisseString & name, risse_uint32 flags) const
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
			DeletePropertyDirect_Primitive(name, flags); return;
		case vtObject:
			DeletePropertyDirect_Object   (name, flags); return;
		}
	}

	void DeletePropertyDirect_Primitive(const tRisseString & name, risse_uint32 flags) const;
	void DeletePropertyDirect_Object   (const tRisseString & name, risse_uint32 flags) const;

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトに対する)関数呼び出し		FuncCall
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//-----------------------------------------------------------------------
	void FuncCall(tRisseVariantBlock * ret = NULL, risse_uint32 flags = 0,
		const tRisseMethodArgument & args = tRisseMethodArgument::Empty(),
		const tRisseVariant & This = tRisseVariant::GetNullObject()) const;

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトのメンバに対する)関数呼び出し		FuncCall
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		name		関数名
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//-----------------------------------------------------------------------
	void FuncCall(
		tRisseVariantBlock * ret,
		const tRisseString & name, risse_uint32 flags = 0,
		const tRisseMethodArgument & args = tRisseMethodArgument::Empty(),
		const tRisseVariant & This = tRisseVariant::GetNullObject()) const
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
			FuncCall_Primitive(ret, name, flags, args, This); return;

		case vtObject:	FuncCall_Object   (ret, name, flags, args, This); return;
		}
	}

	void FuncCall_Primitive(tRisseVariantBlock * ret, const tRisseString & name, risse_uint32 flags, const tRisseMethodArgument & args, const tRisseVariant & This) const;
	void FuncCall_Object   (tRisseVariantBlock * ret, const tRisseString & name, risse_uint32 flags, const tRisseMethodArgument & args, const tRisseVariant & This) const;

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
		case vtVoid:
		case vtInteger:
		case vtReal:
		case vtNull:
		case vtString:
		case vtOctet:
		case vtBoolean:
			return Invoke_Primitive(membername);
		case vtObject:
			return Invoke_Object   (membername);
		}
		return tRisseVariantBlock();
	}

	tRisseVariantBlock Invoke_Primitive(const tRisseString & membername) const;
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
		case vtVoid:
		case vtInteger:
		case vtReal:
		case vtNull:
		case vtString:
		case vtOctet:
		case vtBoolean:
			return Invoke_Primitive(membername, arg1);
		case vtObject:
			return Invoke_Object   (membername,arg1);
		}
		return tRisseVariantBlock();
	}

	tRisseVariantBlock Invoke_Primitive(const tRisseString & membername,const tRisseVariant & arg1) const;
	tRisseVariantBlock Invoke_Object   (const tRisseString & membername,const tRisseVariant & arg1) const;

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトのメンバに対する)単純な関数呼び出し		Invoke
	//! @param		membername	メンバ名
	//! @param		arg1		引数
	//! @param		arg2		引数
	//! @return		戻り値
	//-----------------------------------------------------------------------
	tRisseVariantBlock Invoke(
		const tRisseString & membername,
		const tRisseVariant & arg1,
		const tRisseVariant & arg2
		) const
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
			return Invoke_Primitive(membername,arg1,arg2);
		case vtObject:
			return Invoke_Object   (membername,arg1,arg2);
		}
		return tRisseVariantBlock();
	}

	tRisseVariantBlock Invoke_Primitive(const tRisseString & membername,const tRisseVariant & arg1,const tRisseVariant & arg2) const;
	tRisseVariantBlock Invoke_Object   (const tRisseString & membername,const tRisseVariant & arg1,const tRisseVariant & arg2) const;

public:
	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトに対する)インスタンス作成		New
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @return		新しいインスタンス
	//-----------------------------------------------------------------------
	tRisseVariantBlock New(risse_uint32 flags = 0,
		const tRisseMethodArgument & args = tRisseMethodArgument::Empty()) const // TODO: あれ、Thisは？
	{
		// Object 以外はクラスとしては機能しないため
		// すべて 例外を発生する
		switch(GetType())
		{
		case vtObject:	return New_Object   (tRisseString::GetEmptyString(), flags, args);

		default:
			ThrowCannotCreateInstanceFromNonClassObjectException(); break;
		}
		return tRisseVariantBlock();
	}

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトのメンバに対する)インスタンス作成		New
	//! @param		name		関数名
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @return		新しいインスタンス
	//-----------------------------------------------------------------------
	tRisseVariantBlock New(
		const tRisseString & name, risse_uint32 flags = 0,
		const tRisseMethodArgument & args = tRisseMethodArgument::Empty()) const
	{
		switch(GetType())
		{
		case vtVoid:	return New_Void     (name, flags, args);
		case vtInteger:	return New_Integer  (name, flags, args);
		case vtReal:	return New_Real     (name, flags, args);
		case vtNull:	return New_Null     (name, flags, args);
		case vtString:	return New_String   (name, flags, args);
		case vtOctet:	return New_Octet    (name, flags, args);
		case vtBoolean:	return New_Boolean  (name, flags, args);
		case vtObject:	return New_Object   (name, flags, args);
		}
	}

	tRisseVariantBlock New_Void    (const tRisseString & name, risse_uint32 flags, const tRisseMethodArgument & args) const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock New_Integer (const tRisseString & name, risse_uint32 flags, const tRisseMethodArgument & args) const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock New_Real    (const tRisseString & name, risse_uint32 flags, const tRisseMethodArgument & args) const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock New_Null    (const tRisseString & name, risse_uint32 flags, const tRisseMethodArgument & args) const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock New_String  (const tRisseString & name, risse_uint32 flags, const tRisseMethodArgument & args) const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock New_Octet   (const tRisseString & name, risse_uint32 flags, const tRisseMethodArgument & args) const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock New_Boolean (const tRisseString & name, risse_uint32 flags, const tRisseMethodArgument & args) const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock New_Object  (const tRisseString & name, risse_uint32 flags, const tRisseMethodArgument & args) const;


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
	bool LogNot_Null     () const { return !CastToBoolean_Null(); }
	bool LogNot_String   () const { return !CastToBoolean_String(); }
	bool LogNot_Octet    () const { return !CastToBoolean_Octet(); }
	bool LogNot_Boolean  () const { return !CastToBoolean_Boolean(); }
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
		case vtNull:	return BitNot_Null     ();
		case vtString:	return BitNot_String   ();
		case vtOctet:	return BitNot_Octet    ();
		case vtBoolean:	return BitNot_Boolean  ();
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
	risse_int64        BitNot_Null     () const { return ~CastToInteger_Null(); }
	risse_int64        BitNot_String   () const { return ~CastToInteger_String(); }
	risse_int64        BitNot_Octet    () const { return ~CastToInteger_Octet(); }
	risse_int64        BitNot_Boolean  () const { return ~CastToInteger_Boolean(); }
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
		case vtNull:	return Inc_Null     ();
		case vtString:	return Inc_String   ();
		case vtOctet:	return Inc_Octet    ();
		case vtBoolean:	return Inc_Boolean  ();
		case vtObject:	return Inc_Object   ();
		}
		return *this;
	}

	tRisseVariantBlock & operator ++()    { /*前置*/ return Inc(); }
	tRisseVariantBlock   operator ++(int) { /*後置*/ tRisseVariantBlock t = *this; Inc(); return t;}

	tRisseVariantBlock & Inc_Void     () { *this = (risse_int64)1; /* void は 整数の 1になる */ return *this; }
	tRisseVariantBlock & Inc_Integer  () { *this = AsInteger() + 1; return *this; }
	tRisseVariantBlock & Inc_Real     () { *this = AsReal() + 1.0; return *this; }
	tRisseVariantBlock & Inc_Null     () { ThrowNoSuchMemberException(mnAdd); return *this; }
	tRisseVariantBlock & Inc_String   () { *this = tRisseVariantBlock((risse_int64)1).Add_Integer(Plus_String()); return *this; }
	tRisseVariantBlock & Inc_Octet    () { *this = (risse_int64)0; return *this; /* incomplete */; }
	tRisseVariantBlock & Inc_Boolean  () { *this = (risse_int64)((int)CastToBoolean_Boolean() + 1); return *this; }
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
		case vtNull:	return Dec_Null     ();
		case vtString:	return Dec_String   ();
		case vtOctet:	return Dec_Octet    ();
		case vtBoolean:	return Dec_Boolean  ();
		case vtObject:	return Dec_Object   ();
		}
		return *this;
	}

	tRisseVariantBlock & operator --()    { /*前置*/ return Dec(); }
	tRisseVariantBlock   operator --(int) { /*後置*/ tRisseVariantBlock t = *this; Dec(); return t;}

	tRisseVariantBlock & Dec_Void     () { *this = (risse_int64)-1; /* void は 整数の -1になる */ return *this; }
	tRisseVariantBlock & Dec_Integer  () { *this = AsInteger() - 1; return *this; }
	tRisseVariantBlock & Dec_Real     () { *this = AsReal() - 1.0; return *this; }
	tRisseVariantBlock & Dec_Null     () { ThrowNoSuchMemberException(mnSub); return *this; }
	tRisseVariantBlock & Dec_String   () { *this = tRisseVariantBlock((risse_int64)-1).Add_Integer(Plus_String()); return *this; }
	tRisseVariantBlock & Dec_Octet    () { *this = (risse_int64)0; return *this; /* incomplete */; }
	tRisseVariantBlock & Dec_Boolean  () { *this = (risse_int64)((int)CastToBoolean_Boolean() - 1); return *this; }
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
		case vtNull:	return Plus_Null     ();
		case vtString:	return Plus_String   ();
		case vtOctet:	return Plus_Octet    ();
		case vtBoolean:	return Plus_Boolean  ();
		case vtObject:	return Plus_Object   ();
		}
		return tRisseVariantBlock();
	}

	tRisseVariantBlock operator +() const { return Plus(); }

	tRisseVariantBlock Plus_Void     () const { return (risse_int64)0; /* void は 整数の 0 */ }
	tRisseVariantBlock Plus_Integer  () const { return *this; }
	tRisseVariantBlock Plus_Real     () const { return *this; }
	tRisseVariantBlock Plus_Null     () const { ThrowNoSuchMemberException(mnPlus); return *this; }
	tRisseVariantBlock Plus_String   () const;
	tRisseVariantBlock Plus_Octet    () const { return (risse_int64)0; /* incomplete */; }
	tRisseVariantBlock Plus_Boolean  () const { return CastToBoolean_Boolean() != false; /* boolean は 0 か 1 かに変換される */ }
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
		case vtNull:	return Minus_Null     ();
		case vtString:	return Minus_String   ();
		case vtOctet:	return Minus_Octet    ();
		case vtBoolean:	return Minus_Boolean  ();
		case vtObject:	return Minus_Object   ();
		}
		return tRisseVariantBlock();
	}

	tRisseVariantBlock operator -() const { return Minus(); }

	tRisseVariantBlock Minus_Void     () const { return (risse_int64)0; }
	tRisseVariantBlock Minus_Integer  () const { return -AsInteger(); }
	tRisseVariantBlock Minus_Real     () const { return -AsReal(); }
	tRisseVariantBlock Minus_Null     () const { ThrowNoSuchMemberException(mnMinus); return *this; }
	tRisseVariantBlock Minus_String   () const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock Minus_Octet    () const { return tRisseVariantBlock(); /* incomplete */ }
	tRisseVariantBlock Minus_Boolean  () const { return (risse_int64)(CastToBoolean_Boolean()?-1:0); }
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
	bool LogOr_Null     (const tRisseVariantBlock & rhs) const { return CastToBoolean_Null   () || rhs.operator bool(); }
	bool LogOr_String   (const tRisseVariantBlock & rhs) const { return CastToBoolean_String () || rhs.operator bool(); }
	bool LogOr_Octet    (const tRisseVariantBlock & rhs) const { return CastToBoolean_Octet  () || rhs.operator bool(); }
	bool LogOr_Boolean  (const tRisseVariantBlock & rhs) const { return CastToBoolean_Boolean() || rhs.operator bool(); }
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
	bool LogAnd_Null     (const tRisseVariantBlock & rhs) const { return CastToBoolean_Null   () && rhs.operator bool(); }
	bool LogAnd_String   (const tRisseVariantBlock & rhs) const { return CastToBoolean_String () && rhs.operator bool(); }
	bool LogAnd_Octet    (const tRisseVariantBlock & rhs) const { return CastToBoolean_Octet  () && rhs.operator bool(); }
	bool LogAnd_Boolean  (const tRisseVariantBlock & rhs) const { return CastToBoolean_Boolean() && rhs.operator bool(); }
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
		case vtNull:	return BitOr_Null     (rhs);
		case vtString:	return BitOr_String   (rhs);
		case vtOctet:	return BitOr_Octet    (rhs);
		case vtBoolean:	return BitOr_Boolean  (rhs);
		case vtObject:	return BitOr_Object   (rhs);
		}
		return (risse_int64)0;
	}

	tRisseVariantBlock operator |(const tRisseVariantBlock & rhs) const { return BitOr(rhs); }

	risse_int64        BitOr_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)rhs; }
	risse_int64        BitOr_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
	risse_int64        BitOr_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
	risse_int64        BitOr_Null     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
	risse_int64        BitOr_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
	risse_int64        BitOr_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
	risse_int64        BitOr_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) | (risse_int64)rhs; }
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
		case vtNull:	return BitXor_Null     (rhs);
		case vtString:	return BitXor_String   (rhs);
		case vtOctet:	return BitXor_Octet    (rhs);
		case vtBoolean:	return BitXor_Boolean  (rhs);
		case vtObject:	return BitXor_Object   (rhs);
		}
		return (risse_int64)0;
	}

	tRisseVariantBlock operator ^(const tRisseVariantBlock & rhs) const { return BitXor(rhs); }

	risse_int64        BitXor_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)rhs; }
	risse_int64        BitXor_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
	risse_int64        BitXor_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
	risse_int64        BitXor_Null     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
	risse_int64        BitXor_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
	risse_int64        BitXor_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
	risse_int64        BitXor_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) ^ (risse_int64)rhs; }
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
		case vtNull:	return BitAnd_Null     (rhs);
		case vtString:	return BitAnd_String   (rhs);
		case vtOctet:	return BitAnd_Octet    (rhs);
		case vtBoolean:	return BitAnd_Boolean  (rhs);
		case vtObject:	return BitAnd_Object   (rhs);
		}
		return (risse_int64)0;
	}

	tRisseVariantBlock operator &(const tRisseVariantBlock & rhs) const { return BitAnd(rhs); }

	risse_int64        BitAnd_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)0; }
	risse_int64        BitAnd_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
	risse_int64        BitAnd_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
	risse_int64        BitAnd_Null     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
	risse_int64        BitAnd_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
	risse_int64        BitAnd_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
	risse_int64        BitAnd_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) & (risse_int64)rhs; }
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
	bool NotEqual_Null     (const tRisseVariantBlock & rhs) const { return !Equal_Null   (rhs); }
	bool NotEqual_String   (const tRisseVariantBlock & rhs) const { return !Equal_String (rhs); }
	bool NotEqual_Octet    (const tRisseVariantBlock & rhs) const { return !Equal_Octet  (rhs); }
	bool NotEqual_Boolean  (const tRisseVariantBlock & rhs) const { return !Equal_Boolean(rhs); }
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
		case vtNull:	return Equal_Null     (rhs);
		case vtString:	return Equal_String   (rhs);
		case vtOctet:	return Equal_Octet    (rhs);
		case vtBoolean:	return Equal_Boolean  (rhs);
		case vtObject:	return Equal_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator ==(const tRisseVariantBlock & rhs) const { return Equal(rhs); }

	bool Equal_Void     (const tRisseVariantBlock & rhs) const;
	bool Equal_Integer  (const tRisseVariantBlock & rhs) const;
	bool Equal_Real     (const tRisseVariantBlock & rhs) const;
	bool Equal_Null     (const tRisseVariantBlock & rhs) const;
	bool Equal_String   (const tRisseVariantBlock & rhs) const;
	bool Equal_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Equal_Boolean  (const tRisseVariantBlock & rhs) const;
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
	bool DiscNotEqual_Null     (const tRisseVariantBlock & rhs) const { return !DiscEqual_Null   (rhs); }
	bool DiscNotEqual_String   (const tRisseVariantBlock & rhs) const { return !DiscEqual_String (rhs); }
	bool DiscNotEqual_Octet    (const tRisseVariantBlock & rhs) const { return !DiscEqual_Octet  (rhs); }
	bool DiscNotEqual_Boolean  (const tRisseVariantBlock & rhs) const { return !DiscEqual_Boolean(rhs); }
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
		case vtNull:	return DiscEqual_Null     (rhs);
		case vtString:	return DiscEqual_String   (rhs);
		case vtOctet:	return DiscEqual_Octet    (rhs);
		case vtBoolean:	return DiscEqual_Boolean  (rhs);
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
	bool DiscEqual_Null     (const tRisseVariantBlock & rhs) const
			{ return rhs.GetType() == vtNull; }
	bool DiscEqual_String   (const tRisseVariantBlock & rhs) const
			{ return rhs.GetType() == vtString && rhs.AsString() == AsString(); /* incomplete */ }
	bool DiscEqual_Octet    (const tRisseVariantBlock & rhs) const
			{ return false; /* incomplete */ }
	bool DiscEqual_Boolean  (const tRisseVariantBlock & rhs) const
			{ return rhs.GetType() == vtBoolean && rhs.CastToBoolean_Boolean() == CastToBoolean_Boolean(); }
	bool DiscEqual_Object   (const tRisseVariantBlock & rhs) const
			{ if(IsNull()) { return rhs.IsNull(); } return Invoke(mnDiscEqual, rhs); }

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
		case vtNull:	return Lesser_Null     (rhs);
		case vtString:	return Lesser_String   (rhs);
		case vtOctet:	return Lesser_Octet    (rhs);
		case vtBoolean:	return Lesser_Boolean  (rhs);
		case vtObject:	return Lesser_Object   (rhs);
		}
		return false;
	}

	bool operator < (const tRisseVariantBlock & rhs) const { return Lesser(rhs); }

	bool Lesser_Void     (const tRisseVariantBlock & rhs) const;
	bool Lesser_Integer  (const tRisseVariantBlock & rhs) const;
	bool Lesser_Real     (const tRisseVariantBlock & rhs) const;
	bool Lesser_Null     (const tRisseVariantBlock & rhs) const { ThrowNoSuchMemberException(mnLesser); return false; }
	bool Lesser_String   (const tRisseVariantBlock & rhs) const;
	bool Lesser_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Lesser_Boolean  (const tRisseVariantBlock & rhs) const;
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
		case vtNull:	return Greater_Null     (rhs);
		case vtString:	return Greater_String   (rhs);
		case vtOctet:	return Greater_Octet    (rhs);
		case vtBoolean:	return Greater_Boolean  (rhs);
		case vtObject:	return Greater_Object   (rhs);
		}
		return false;
	}

	bool operator > (const tRisseVariantBlock & rhs) const { return Greater(rhs); }

	bool Greater_Void     (const tRisseVariantBlock & rhs) const;
	bool Greater_Integer  (const tRisseVariantBlock & rhs) const;
	bool Greater_Real     (const tRisseVariantBlock & rhs) const;
	bool Greater_Null     (const tRisseVariantBlock & rhs) const { ThrowNoSuchMemberException(mnGreater); return false; }
	bool Greater_String   (const tRisseVariantBlock & rhs) const;
	bool Greater_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool Greater_Boolean  (const tRisseVariantBlock & rhs) const;
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
		case vtNull:	return LesserOrEqual_Null     (rhs);
		case vtString:	return LesserOrEqual_String   (rhs);
		case vtOctet:	return LesserOrEqual_Octet    (rhs);
		case vtBoolean:	return LesserOrEqual_Boolean  (rhs);
		case vtObject:	return LesserOrEqual_Object   (rhs);
		}
		return false;
	}

	bool operator <= (const tRisseVariantBlock & rhs) const { return LesserOrEqual(rhs); }

	bool LesserOrEqual_Void     (const tRisseVariantBlock & rhs) const;
	bool LesserOrEqual_Integer  (const tRisseVariantBlock & rhs) const;
	bool LesserOrEqual_Real     (const tRisseVariantBlock & rhs) const;
	bool LesserOrEqual_Null     (const tRisseVariantBlock & rhs) const { ThrowNoSuchMemberException(mnLesserOrEqual); return false; }
	bool LesserOrEqual_String   (const tRisseVariantBlock & rhs) const;
	bool LesserOrEqual_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool LesserOrEqual_Boolean  (const tRisseVariantBlock & rhs) const;
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
		case vtNull:	return GreaterOrEqual_Null     (rhs);
		case vtString:	return GreaterOrEqual_String   (rhs);
		case vtOctet:	return GreaterOrEqual_Octet    (rhs);
		case vtBoolean:	return GreaterOrEqual_Boolean  (rhs);
		case vtObject:	return GreaterOrEqual_Object   (rhs);
		}
		return false;
	}

	bool operator >= (const tRisseVariantBlock & rhs) const { return GreaterOrEqual(rhs); }

	bool GreaterOrEqual_Void     (const tRisseVariantBlock & rhs) const;
	bool GreaterOrEqual_Integer  (const tRisseVariantBlock & rhs) const;
	bool GreaterOrEqual_Real     (const tRisseVariantBlock & rhs) const;
	bool GreaterOrEqual_Null     (const tRisseVariantBlock & rhs) const { ThrowNoSuchMemberException(mnGreaterOrEqual); return false; }
	bool GreaterOrEqual_String   (const tRisseVariantBlock & rhs) const;
	bool GreaterOrEqual_Octet    (const tRisseVariantBlock & rhs) const { return false; /* incomplete */ }
	bool GreaterOrEqual_Boolean  (const tRisseVariantBlock & rhs) const;
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
		case vtNull:	return RBitShift_Null     (rhs);
		case vtString:	return RBitShift_String   (rhs);
		case vtOctet:	return RBitShift_Octet    (rhs);
		case vtBoolean:	return RBitShift_Boolean  (rhs);
		case vtObject:	return RBitShift_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock RBitShift_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_Null     (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
	tRisseVariantBlock RBitShift_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)((risse_uint64)(risse_int64)(*this) >> (risse_int64)rhs); }
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
		case vtNull:	return LShift_Null     (rhs);
		case vtString:	return LShift_String   (rhs);
		case vtOctet:	return LShift_Octet    (rhs);
		case vtBoolean:	return LShift_Boolean  (rhs);
		case vtObject:	return LShift_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator << (const tRisseVariantBlock & rhs) const { return LShift(rhs); }

	tRisseVariantBlock LShift_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_Null     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
	tRisseVariantBlock LShift_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) << (risse_int64)rhs; }
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
		case vtNull:	return RShift_Null     (rhs);
		case vtString:	return RShift_String   (rhs);
		case vtOctet:	return RShift_Octet    (rhs);
		case vtBoolean:	return RShift_Boolean  (rhs);
		case vtObject:	return RShift_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator >> (const tRisseVariantBlock & rhs) const { return RShift(rhs); }

	tRisseVariantBlock RShift_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_Null     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
	tRisseVariantBlock RShift_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) >> (risse_int64)rhs; }
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
		case vtNull:	return Mod_Null     (rhs);
		case vtString:	return Mod_String   (rhs);
		case vtOctet:	return Mod_Octet    (rhs);
		case vtBoolean:	return Mod_Boolean  (rhs);
		case vtObject:	return Mod_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator % (const tRisseVariantBlock & rhs) const { return Mod(rhs); }

	tRisseVariantBlock Mod_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_Null     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
	tRisseVariantBlock Mod_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) % (risse_int64)rhs; }
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
		case vtNull:	return Div_Null     (rhs);
		case vtString:	return Div_String   (rhs);
		case vtOctet:	return Div_Octet    (rhs);
		case vtBoolean:	return Div_Boolean  (rhs);
		case vtObject:	return Div_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator / (const tRisseVariantBlock & rhs) const { return Div(rhs); }

	tRisseVariantBlock Div_Void     (const tRisseVariantBlock & rhs) const { return (risse_real)(*this) / (risse_real)rhs; }
	tRisseVariantBlock Div_Integer  (const tRisseVariantBlock & rhs) const { return (risse_real)(*this) / (risse_real)rhs; }
	tRisseVariantBlock Div_Real     (const tRisseVariantBlock & rhs) const { return (risse_real)(*this) / (risse_real)rhs; }
	tRisseVariantBlock Div_Null     (const tRisseVariantBlock & rhs) const { return (risse_real)(*this) / (risse_real)rhs; }
	tRisseVariantBlock Div_String   (const tRisseVariantBlock & rhs) const { return (risse_real)(*this) / (risse_real)rhs; }
	tRisseVariantBlock Div_Octet    (const tRisseVariantBlock & rhs) const { return (risse_real)(*this) / (risse_real)rhs; }
	tRisseVariantBlock Div_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_real)(*this) / (risse_real)rhs; }
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
		case vtNull:	return Idiv_Null     (rhs);
		case vtString:	return Idiv_String   (rhs);
		case vtOctet:	return Idiv_Octet    (rhs);
		case vtBoolean:	return Idiv_Boolean  (rhs);
		case vtObject:	return Idiv_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock Idiv_Void     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_Integer  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_Real     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_Null     (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_String   (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_Octet    (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
	tRisseVariantBlock Idiv_Boolean  (const tRisseVariantBlock & rhs) const { return (risse_int64)(*this) / (risse_int64)rhs; }
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
		case vtNull:	return Mul_Null     (rhs);
		case vtString:	return Mul_String   (rhs);
		case vtOctet:	return Mul_Octet    (rhs);
		case vtBoolean:	return Mul_Boolean  (rhs);
		case vtObject:	return Mul_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator * (const tRisseVariantBlock & rhs) const { return Mul(rhs); }

	tRisseVariantBlock Mul_Void     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Mul_Integer  (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Mul_Real     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Mul_Null     (const tRisseVariantBlock & rhs) const { ThrowNoSuchMemberException(mnMul); return *this; }
	tRisseVariantBlock Mul_String   (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Mul_Octet    (const tRisseVariantBlock & rhs) const { return (risse_real)0; /* incomplete */ }
	tRisseVariantBlock Mul_Boolean  (const tRisseVariantBlock & rhs) const;
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
		case vtNull:	return Add_Null     (rhs);
		case vtString:	return Add_String   (rhs);
		case vtOctet:	return Add_Octet    (rhs);
		case vtBoolean:	return Add_Boolean  (rhs);
		case vtObject:	return Add_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator + (const tRisseVariantBlock & rhs) const { return Add(rhs); }

	tRisseVariantBlock Add_Void     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Add_Integer  (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Add_Real     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Add_Null     (const tRisseVariantBlock & rhs) const { ThrowNoSuchMemberException(mnAdd); return *this; }
	tRisseVariantBlock Add_String   (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Add_Octet    (const tRisseVariantBlock & rhs) const { return (risse_real)0; /* incomplete */ }
	tRisseVariantBlock Add_Boolean  (const tRisseVariantBlock & rhs) const;
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
		case vtNull:	return Sub_Null     (rhs);
		case vtString:	return Sub_String   (rhs);
		case vtOctet:	return Sub_Octet    (rhs);
		case vtBoolean:	return Sub_Boolean  (rhs);
		case vtObject:	return Sub_Object   (rhs);
		}
		return false;
	}

	tRisseVariantBlock operator - (const tRisseVariantBlock & rhs) const { return Sub(rhs); }

	tRisseVariantBlock Sub_Void     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Sub_Integer  (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Sub_Real     (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Sub_Null     (const tRisseVariantBlock & rhs) const { ThrowNoSuchMemberException(mnSub); return *this; }
	tRisseVariantBlock Sub_String   (const tRisseVariantBlock & rhs) const;
	tRisseVariantBlock Sub_Octet    (const tRisseVariantBlock & rhs) const { return (risse_real)0; /* incomplete */ }
	tRisseVariantBlock Sub_Boolean  (const tRisseVariantBlock & rhs) const;
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

	//-----------------------------------------------------------------------
	//! @brief		instanceof 演算子(instanceof)		InstanceOf
	//! @param		rhs			右辺
	//! @return		左辺が右辺で示したクラスのインスタンスならば真
	//-----------------------------------------------------------------------
	bool InstanceOf(const tRisseVariantBlock & rhs) const;


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
		case vtNull:	return CastToInteger_Null     ();
		case vtString:	return CastToInteger_String   ();
		case vtOctet:	return CastToInteger_Octet    ();
		case vtBoolean:	return CastToInteger_Boolean  ();
		case vtObject:	return CastToInteger_Object   ();
		}
		return (risse_int64)0;
	}

	risse_int64 CastToInteger_Void     () const { return false; /* void は 0 */}
	risse_int64 CastToInteger_Integer  () const { return AsInteger(); }
	risse_int64 CastToInteger_Real     () const { return (risse_int64)AsReal(); }
	risse_int64 CastToInteger_Null     () const { ThrowNoSuchMemberException(mnInteger); return (risse_int64)0; }
	risse_int64 CastToInteger_String   () const;
	risse_int64 CastToInteger_Octet    () const { return (risse_int64)0; /* incomplete */ }
	risse_int64 CastToInteger_Boolean  () const { return (risse_int64)CastToBoolean_Boolean(); }
	risse_int64 CastToInteger_Object   () const { return Invoke(mnInteger); }

	//-----------------------------------------------------------------------
	//! @brief		realに変換
	//! @return		real
	//-----------------------------------------------------------------------
	operator risse_real() const
	{
		switch(GetType())
		{
		case vtVoid:	return CastToReal_Void     ();
		case vtInteger:	return CastToReal_Integer  ();
		case vtReal:	return CastToReal_Real     ();
		case vtNull:	return CastToReal_Null     ();
		case vtString:	return CastToReal_String   ();
		case vtOctet:	return CastToReal_Octet    ();
		case vtBoolean:	return CastToReal_Boolean  ();
		case vtObject:	return CastToReal_Object   ();
		}
		return false;
	}

	risse_real CastToReal_Void     () const { return (risse_real)0.0; }
	risse_real CastToReal_Integer  () const { return AsInteger(); }
	risse_real CastToReal_Real     () const { return AsReal(); }
	risse_real CastToReal_Null     () const { ThrowNoSuchMemberException(mnReal); return (risse_real)0.0; }
	risse_real CastToReal_String   () const { return (risse_real)Plus_String(); /* Plus_String の戻りを risse_real に再キャスト */ }
	risse_real CastToReal_Octet    () const { return (risse_real)0.0; /* incomplete */ }
	risse_real CastToReal_Boolean  () const { return (risse_real)(int)CastToBoolean_Boolean(); }
	risse_real CastToReal_Object   () const { return Invoke(mnReal); }

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
		case vtNull:	return CastToBoolean_Null     ();
		case vtString:	return CastToBoolean_String   ();
		case vtOctet:	return CastToBoolean_Octet    ();
		case vtBoolean:	return CastToBoolean_Boolean  ();
		case vtObject:	return CastToBoolean_Object   ();
		}
		return false;
	}

	bool CastToBoolean_Void     () const { return false; /* void は偽 */}
	bool CastToBoolean_Integer  () const { return AsInteger() != 0; }
	bool CastToBoolean_Real     () const { return AsReal() != 0.0; }
	bool CastToBoolean_Null     () const { return false; /* null は偽 */ }
	bool CastToBoolean_String   () const { return !AsString().IsEmpty(); }
	bool CastToBoolean_Octet    () const { return !AsOctet().IsEmpty(); }
	bool CastToBoolean_Boolean  () const { return Type == BooleanTrue; }
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
		case vtNull:	return CastToString_Null     ();
		case vtString:	return CastToString_String   ();
		case vtOctet:	return CastToString_Octet    ();
		case vtBoolean:	return CastToString_Boolean  ();
		case vtObject:	return CastToString_Object   ();
		}
		return tRisseString();
	}

	tRisseString CastToString_Void     () const { return tRisseString(); }
	tRisseString CastToString_Integer  () const;
	tRisseString CastToString_Real     () const;
	tRisseString CastToString_Null     () const;
	tRisseString CastToString_String   () const { return AsString(); }
	tRisseString CastToString_Octet    () const { return AsOctet().AsHumanReadable();  }
	tRisseString CastToString_Boolean  () const;
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
		case vtNull:	return AsHumanReadable_Null     (maxlen);
		case vtString:	return AsHumanReadable_String   (maxlen);
		case vtOctet:	return AsHumanReadable_Octet    (maxlen);
		case vtBoolean:	return AsHumanReadable_Boolean  (maxlen);
		case vtObject:	return AsHumanReadable_Object   (maxlen);
		}
		return tRisseString();
	}

	tRisseString AsHumanReadable_Void     (risse_size maxlen) const;
	tRisseString AsHumanReadable_Integer  (risse_size maxlen) const
					{ return CastToString_Integer(); }
	tRisseString AsHumanReadable_Real     (risse_size maxlen) const
					{ return CastToString_Real(); }
	tRisseString AsHumanReadable_Null     (risse_size maxlen) const;
	tRisseString AsHumanReadable_String   (risse_size maxlen) const
					{ return AsString().AsHumanReadable(maxlen); }
	tRisseString AsHumanReadable_Octet    (risse_size maxlen) const
					{ return AsOctet().AsHumanReadable(maxlen); }
	tRisseString AsHumanReadable_Boolean  (risse_size maxlen) const
					{ return CastToString_Boolean(); }
	tRisseString AsHumanReadable_Object   (risse_size maxlen) const
					{ return tRisseString(); /* incomplete */ }

	//! @brief		Object型に対するtypeチェック
	//! @note		バリアントが期待したタイプであるかどうかをチェックし
	//!				またそのオブジェクトインターフェースを得る。
	//!				期待した値でなければ「"期待したクラスではありません"」例外を発生する
	//!				テンプレートパラメータのObjectTはtRisseObjectBaseの派生クラス、
	//!				ClassTにはtRisseSingletonの派生クラスかつtRisseClassBaseの派生クラスを指定すること。
	template <typename ObjectT, typename ClassT>
	ObjectT * CheckAndGetObjectInterafce() const
	{
		if(GetType() != vtObject) ThrowBadContextException();
		ObjectT * intf = reinterpret_cast<ObjectT*>(GetObjectInterface());
		if(!ClassT::GetPointer()->GetRTTIMatcher().Match(intf->GetRTTI()))
			ThrowBadContextException();
		return intf;
	}

	//! @brief		トレースを追加する
	//! @param		sb			スクリプトブロック
	//! @param		pos			スクリプト上の位置
	//! @note		この variant に Throwable クラスのインスタンスが入っているとみなし、
	//!				指定されたトレースを追加する。sb が null の場合は
	//!				トレースは追加されない。
	void AddTrace(const tRisseScriptBlockBase * sb, risse_size pos) const;

	//! @brief		デバッグ用ダンプ(標準出力に出力する)
	void DebugDump() const;

	//! @brief		デバッグ用各種構造体サイズ表示
	void prtsizes() const
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
		fprintf(stderr, "tNull: %d\n", sizeof(tNull));
	}

};
//---------------------------------------------------------------------------
typedef tRisseVariantBlock tRisseVariant;
//---------------------------------------------------------------------------



} // namespace Risse
#endif

