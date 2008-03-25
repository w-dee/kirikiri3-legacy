//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
class tScriptBlockInstance;
class tScriptEngine;
class tClassBase;
//---------------------------------------------------------------------------
//! @brief	バリアント型
//! @note	tVariantData よりも高度な動作をここで定義する
//! @note	スレッド保護無し
//---------------------------------------------------------------------------
class tVariantBlock : public tVariantData, public tOperateRetValue
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
	static void ThrowNoSuchMemberException(const tString & name);

	//! @brief		IllegalArgumentClassException を投げる
	//! @param		class_name			クラス名
	//! @note		本来 risseExceptionClass.h に書いてある物だが
	//!				ここの位置からは参照できないのでわざわざワンクッションを置く
	static void ThrowIllegalArgumentClassException(const tString & class_name);

	//! @brief		IllegalArgumentClassException を投げる
	//! @param		method_name			メソッド名
	//! @note		本来 risseExceptionClass.h に書いてある物だが
	//!				ここの位置からは参照できないのでわざわざワンクッションを置く
	void ThrowIllegalOperationMethod(const tString & method_name) const;

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
		const tVariantBlock * Context; //!< (Intfがメソッドオブジェクトやプロパティオブジェクトを
						//!< 指しているとして)メソッドが動作するコンテキスト
		char Storage[RV_STORAGE_SIZE - sizeof(risse_ptruint) - sizeof(const tVariantBlock *)];
			//!< 残り(0で埋める) パディングは問題にならないはず
	};
	static tStaticObject DynamicContext;

public: // static オブジェクト
	//! @brief		void オブジェクトを得る
	//! @return		void オブジェクトへのstaticなconst参照
	static const tVariantBlock & GetVoidObject()
	{
		return *reinterpret_cast<tVariantBlock*>(&VoidObject);
	}

	//! @brief		null オブジェクトを得る
	//! @return		null オブジェクトへのstaticなconst参照
	static const tVariantBlock & GetNullObject()
	{
		return *reinterpret_cast<tVariantBlock*>(&NullObject);
	}

	//! @brief		DynamicContext オブジェクトを得る
	static const tVariantBlock * GetDynamicContext()
	{
		return reinterpret_cast<tVariantBlock*>(&DynamicContext);
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

	//! @brief		バリアントのタイプをタグとして設定する
	//! @param		type		型
	//! @note		バリアントのタイプがこれによって設定されるが、
	//!				元の値は破棄され、値は各バリアントタイプの代表値になる
	//!				(例えば文字列ならば空文字列、数値ならば0)
	void SetTypeTag(tType type);

public: // GuessTypeXXXXX で使用されるもの
	//! @brief		GuessTypeXXXXX で使用される列挙型
	enum tGuessType
	{
		gtVoid		= vtVoid,
		gtInteger	= vtInteger,
		gtReal		= vtReal,
		gtNull		= vtNull,
		gtString	= vtString,
		gtOctet		= vtOctet,
		gtBoolean	= vtBoolean,
		gtObject	= vtObject,

		gtAny,		//!< 任意の型(GuessTypeXXXXX の入力として用いられた場合は
					//!< どのような型もあり得ることを表す。出力として得られた場合も
					//!< どのような型もあり得ることを表す)

		// 以降、出力のみ
		gtError		//!< (出力のみ) この演算の組み合わせは「必ず」エラーになる。
					//!< 「場合によってはエラーになる」場合は他の結果が
					//!< 得られる。
	};
	enum tGuessTypeFlags
	{
		gtTypeMask = 0x3f, //!< 型のマスク
		gtEffective = 0x40 //!< (出力のみ) 副作用を持っている場合にこれを組み合わせて使う。
	};

	//! @brief		tGuessType を文字列化する
	//! @param		type		タイプ
	//! @return		type		を文字列化した物
	static const risse_char * GetGuessTypeString(tGuessType type);

public: // コンストラクタ/代入演算子

	//! @brief デフォルトコンストラクタ(void型を作成)
	tVariantBlock()
	{
		Clear();
	}

	//! @brief		コピーコンストラクタ
	//! @param		ref		元となるオブジェクト
	tVariantBlock(const tVariantBlock & ref)
	{
		* this = ref;
	}

	//! @brief		単純代入
	//! @param		ref		元となるオブジェクト
	tVariantBlock & operator = (const tVariantBlock & ref)
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
		return *this;
	}

private:
	//! @brief		コンストラクタ(const void * から)
	//! @param		ref		値
	//! @note		tObjectInterface に変換できない任意のポインタを
	//!				ここで引っかけるためのコンストラクタ。ここで
	//!				引っかかった場合はコードを見直すこと。
	//!				おそらく、
	//!				 1. なにか関係のないポインタを tVariant に突っ込もうとした
	//!				 2. 確かに tObjectInterface 派生クラスだが
	//!				    必要な情報を include してないためにコンパイラがダウンキャスト
	//!				    に失敗している
	//!				 3. const な tObjectInterface へのポインタを渡そうとした
	//!				    (非const な tObjectInterface * しか受け付けない)
	tVariantBlock(const void * ref);

	//! @brief		代入演算子(const void * を代入)
	//! @param		ref		値
	//! @note		tObjectInterface に変換できない任意のポインタを
	//!				ここで引っかけるための代入演算子。
	//!				ここで引っかかったた場合はコードを見直すこと。
	//!				(tVariantBlock(const void * )も参照)
	void operator = (const void * ref);

public:
	//! @brief		コンストラクタ(integer型を作成)
	//! @param		ref		元となる整数
	tVariantBlock(const risse_int64 ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(integer型を代入)
	//! @param		ref		元となる整数
	tVariantBlock & operator = (const risse_int64 ref)
	{
		Type = vtInteger;
		AsInteger() = ref;
		return *this;
	}

	//! @brief		コンストラクタ(real型を作成)
	//! @param		ref		元となる実数
	tVariantBlock(const risse_real ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(real型を代入)
	//! @param		ref		元となる実数
	tVariantBlock & operator = (const risse_real ref)
	{
		Type = vtReal;
		AsReal() = ref;
		return *this;
	}

	//! @brief		コンストラクタ(bool型を作成)
	//! @param		ref		元となる真偽値
	tVariantBlock(const bool ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(bool型を代入)
	//! @param		ref		元となる真偽値
	tVariantBlock & operator = (const bool ref)
	{
		Type = ref?BooleanTrue:BooleanFalse;
		Ptr = 0;
		return *this;
	}

	//! @brief		コンストラクタ(string型を作成)
	//! @param		ref		元となる文字列
	tVariantBlock(const tString & ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(string型を代入)
	//! @param		ref		元となる文字列
	//! @return		このオブジェクトへの参照
	tVariantBlock & operator = (const tString & ref)
	{
		// Type の設定は必要なし
		AsString() = ref;
		return *this;
	}

	//! @brief		コンストラクタ(string型を作成)
	//! @param		ref		元となる文字列
	tVariantBlock(const risse_char * ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(string型を代入)
	//! @param		ref		元となる文字列
	//! @return		このオブジェクトへの参照
	tVariantBlock & operator = (const risse_char * ref)
	{
		// Type の設定は必要なし
		AsString() = tString(ref);
		return *this;
	}

	//! @brief		コンストラクタ(octet型を作成)
	//! @param		ref		元となるオクテット列
	tVariantBlock(const tOctet & ref)
	{
		* this = ref;
	}

	//! @brief		代入演算子(octet型を代入)
	//! @param		ref		元となるオクテット列
	tVariantBlock & operator = (const tOctet & ref)
	{
		// Type の設定は必要なし
		AsOctet() = ref;
		return *this;
	}

	//! @brief		コンストラクタ(tObjectInterface*型より)
	//! @param		ref		元となるオブジェクト
	tVariantBlock(tObjectInterface * ref)
	{
		* this = ref;
	}

	//! @brief		コンストラクタ(tObjectInterface*型とコンテキストを表すtVariant型より)
	//! @param		ref		元となるオブジェクト(メソッドオブジェクトかプロパティオブジェクトを表す)
	//! @param		context	そのメソッドやプロパティが実行されるべきコンテキストオブジェクトを表す
	tVariantBlock(tObjectInterface * ref, const tVariantBlock * context)
	{
		Type = vtObject;
		SetObjectIntf(ref);
		RISSE_ASSERT(context != NULL);
		AsObject().Context = context;
	}

	//! @brief		代入演算子(tObjectInterface*型を代入)
	//! @param		ref		元となるオブジェクト
	tVariantBlock & operator = (tObjectInterface * ref)
	{
		// これはちょっと特殊
		Type = vtObject;
		SetObjectIntf(ref);
		AsObject().Context = GetDynamicContext(); // this は DynamicContext に設定
		return *this;
	}

	//! @brief		代入演算子(tObject型を代入)
	//! @param		ref		元となるオブジェクト
	tVariantBlock & operator = (const tObject & ref)
	{
		Type = vtObject;
		AsObject() = ref;
		return *this;
	}

public: // デストラクタ
	//! @brief		デストラクタ(tHashTableがこれを呼ぶ)
	void Destruct()
	{
		// 少なくとも、メンバとして持っているポインタが破壊できればよい
		DestructPointer();
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
	//! @param		engine		スクリプトエンジンインスタンス
	tPrimitiveClassBase * GetPrimitiveClass(tScriptEngine * engine) const;

public: // Object関連
	//! @brief		オブジェクトインターフェースがマッチするかどうかを調べる
	//! @param		rhs			右辺値
	//! @return		マッチしたかどうか
	//! @note		このメソッドは、(1)thisとrhsが両方ともvtObjectかつ(2)オブジェクトインターフェース
	//!				ポインタが同一であるかどうかを調べる
	bool ObjectInterfaceMatch(const tVariant & rhs) const
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
	void SetContext(const tVariantBlock * context)
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
	//!				このメソッドは const tVariantBlock * context を引数に取る版とちがい、
	//!				context がどうやら dynamic コンテキストらしい場合は自動的に
	//!				GetDynamicContext() の戻りに変換する。そうでない場合は
	//!				tVariantBlock を new してそのポインタを SetContext() で設定する。
	void SetContext(const tVariantBlock &context);

	//! @brief		コンテキストを取得する
	//! @return		そのメソッドやプロパティが実行されるべきコンテキスト
	//! @note		このメソッドはvtがvtObjectかどうかを
	//!				チェックしないので注意すること
	const tVariantBlock * GetContext() const
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
	void OverwriteContext(const tVariantBlock * context)
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
	//!				ただし、フラグに ofUseClassMembersRule が指定されていた場合は常に This を返す
	const tVariantBlock & SelectContext(risse_uint32 flags, const tVariantBlock & This) const
	{
		RISSE_ASSERT(GetType() == vtObject); // チェックはしないとはいうものの一応ASSERTはする
		if(!(flags & tOperateFlags::ofUseClassMembersRule))
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
	//! @param		engine	スクリプトエンジンインスタンス
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
	void Do(tScriptEngine * engine, RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
	{
		tRetValue ret = Operate(engine, RISSE_OBJECTINTERFACE_PASS_ARG);
		if(ret != rvNoError) RaiseError(ret, name);
	}

	//! @brief		オブジェクトに対して操作を行う
	//! @param		engine	スクリプトエンジンインスタンス
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
		Operate(tScriptEngine * engine, RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
	{
		if(!name.IsEmpty())
		{
			// 名前指定がある場合
			return OperateForMember(engine, RISSE_OBJECTINTERFACE_PASS_ARG);
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
			args.ExpectArgumentCount(1);
			if(result)
				*result = InstanceOf(engine, args[0]);
			else
				InstanceOf(engine, args[0]); /* discard result */
			return rvNoError;

//		case ocDGet				://!< get .  
//			RISSE_BIN_OP(DGet);

		case ocIGet				://!< get [ ]
			RISSE_BIN_OP(IGet);

//		case ocDDelete			://!< delete .
//			RISSE_BIN_OP(DDelete);

		case ocIDelete			://!< delete [ ]
			RISSE_BIN_OP(IDelete);

//		case ocDSet				://!< set .
//			RISSE_BIN_OP(DSet);

		case ocISet				://!< set [ ]
			args.ExpectArgumentCount(2);
			ISet(args[0], args[1]);
			return rvNoError;

		case ocDSetAttrib		: //!< dseta
			args.ExpectArgumentCount(1);
			SetAttributeDirect(engine, args[0], flags);
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
			return OperateForMember(engine, RISSE_OBJECTINTERFACE_PASS_ARG);
				// OperateForMemberならば処理ができるかもしれない
			;
		}
		return  rvNoError;
	}

	//! @brief		オブジェクトのメンバに対して操作を行う
	//! @param		engine	スクリプトエンジンインスタンス
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
		OperateForMember(tScriptEngine * engine, RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

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
	tVariantBlock GetPropertyDirect(tScriptEngine * engine, const tString & name,
		risse_uint32 flags = 0,
		const tVariant & This = tVariant::GetNullObject()) const
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
			return GetPropertyDirect_Primitive(engine, name, flags, This);
		case vtObject:
			return GetPropertyDirect_Object   (        name, flags, This);
		}
		return tVariant();
	}

	tVariantBlock GetPropertyDirect_Primitive(tScriptEngine * engine, const tString & name, risse_uint32 flags = 0, const tVariant & This = tVariant::GetNullObject()) const ;
	tVariantBlock GetPropertyDirect_Object   (                             const tString & name, risse_uint32 flags = 0, const tVariant & This = tVariant::GetNullObject()) const ;

	//-----------------------------------------------------------------------
	//! @brief		直接プロパティ設定		SetPropertyDirect dset
	//! @param		engine	スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	//! @param		value		設定する値
	//! @param		flags		フラグ
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//-----------------------------------------------------------------------
	void SetPropertyDirect(tScriptEngine * engine, const tString & name, risse_uint32 flags,
		const tVariantBlock & value, const tVariant & This = tVariant::GetNullObject()) const
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
			SetPropertyDirect_Primitive(engine, name, flags, value, This); return;
		case vtObject:
			SetPropertyDirect_Object   (        name, flags, value, This); return;
		}
	}

	void SetPropertyDirect_Primitive(tScriptEngine * engine, const tString & name, risse_uint32 flags, const tVariantBlock & value, const tVariant & This = tVariant::GetNullObject()) const;
	void SetPropertyDirect_Object   (                             const tString & name, risse_uint32 flags, const tVariantBlock & value, const tVariant & This = tVariant::GetNullObject()) const;

	//-----------------------------------------------------------------------
	//! @brief		間接プロパティ取得		IGet iget
	//! @param		key		キー
	//! @return		プロパティ取得の結果
	//-----------------------------------------------------------------------
	tVariantBlock IGet(const tVariantBlock & key) const
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
		return tVariant();
	}

	tVariantBlock IGet_Void    (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IGet_Integer (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IGet_Real    (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IGet_Null    (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IGet_String  (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IGet_Octet   (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IGet_Boolean (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IGet_Object  (const tVariantBlock & key) const { return Invoke_Object(mnIGet, key); }

	//-----------------------------------------------------------------------
	//! @brief		間接削除		IDelete idel
	//! @param		key		キー
	//! @return		削除されたキーの値(削除できなかった場合は普通void)
	//-----------------------------------------------------------------------
	tVariantBlock IDelete(const tVariantBlock & key) const
	{
		switch(GetType())
		{
		case vtVoid:	return IDelete_Void     (key);
		case vtInteger:	return IDelete_Integer  (key);
		case vtReal:	return IDelete_Real     (key);
		case vtNull:	return IDelete_Null     (key);
		case vtString:	return IDelete_String   (key);
		case vtOctet:	return IDelete_Octet    (key);
		case vtBoolean:	return IDelete_Boolean  (key);
		case vtObject:	return IDelete_Object   (key);
		}
		return tVariant();
	}

	tVariantBlock IDelete_Void    (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IDelete_Integer (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IDelete_Real    (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IDelete_Null    (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IDelete_String  (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IDelete_Octet   (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IDelete_Boolean (const tVariantBlock & key) const { return tVariant(); /* incomplete */ }
	tVariantBlock IDelete_Object  (const tVariantBlock & key) const { return Invoke_Object(mnIDelete, key); }

	//-----------------------------------------------------------------------
	//! @brief		間接プロパティ設定		ISet iset
	//! @param		key			キー
	//! @param		value		設定する値
	//-----------------------------------------------------------------------
	void ISet(const tVariantBlock & key, const tVariantBlock & value) const
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

	void ISet_Void    (const tVariantBlock & key, const tVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Integer (const tVariantBlock & key, const tVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Real    (const tVariantBlock & key, const tVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Null    (const tVariantBlock & key, const tVariantBlock & value) const { return; /* incomplete */ }
	void ISet_String  (const tVariantBlock & key, const tVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Octet   (const tVariantBlock & key, const tVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Boolean (const tVariantBlock & key, const tVariantBlock & value) const { return; /* incomplete */ }
	void ISet_Object  (const tVariantBlock & key, const tVariantBlock & value) const { Invoke_Object(mnISet, value, key);
		/* 注意!!! ISet がメソッド呼び出しに変換される場合、value が先に来て key が後に来る。これは将来的に
		複数の key を使用可能にする可能性があるためである */ }

	//-----------------------------------------------------------------------
	//! @brief		直接プロパティ削除		DeletePropertyDirect ddelete
	//! @param		engine	スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	//! @param		flags		フラグ
	//-----------------------------------------------------------------------
	void DeletePropertyDirect(tScriptEngine * engine, const tString & name, risse_uint32 flags) const
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
			DeletePropertyDirect_Primitive(engine, name, flags); return;
		case vtObject:
			DeletePropertyDirect_Object   (        name, flags); return;
		}
	}

	void DeletePropertyDirect_Primitive(tScriptEngine * engine, const tString & name, risse_uint32 flags) const;
	void DeletePropertyDirect_Object   (                             const tString & name, risse_uint32 flags) const;

	//-----------------------------------------------------------------------
	//! @brief		属性の設定		DSetAttrib dseta
	//! @param		engine	スクリプトエンジンインスタンス
	//! @param		key			キー
	//! @param		attrib		設定する属性値
	//-----------------------------------------------------------------------
	void SetAttributeDirect(tScriptEngine * engine, const tVariantBlock & key, risse_uint32 attrib) const
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
			SetAttributeDirect_Primitive(engine, key, attrib); return;
		case vtObject:
			SetAttributeDirect_Object   (        key, attrib); return;
		}
	}

	void SetAttributeDirect_Primitive(tScriptEngine * engine, const tVariantBlock & key, risse_uint32 attrib) const { return; /* incomplete */ }
	void SetAttributeDirect_Object   (                        const tVariantBlock & key, risse_uint32 attrib) const;

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトに対する)関数呼び出し		FuncCall
	//! @param		engine	スクリプトエンジンインスタンス
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//-----------------------------------------------------------------------
	void FuncCall(tScriptEngine * engine, tVariantBlock * ret = NULL, risse_uint32 flags = 0,
		const tMethodArgument & args = tMethodArgument::Empty(),
		const tVariant & This = tVariant::GetNullObject()) const;

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトのメンバに対する)関数呼び出し		FuncCall
	//! @param		engine	スクリプトエンジンインスタンス
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		name		関数名
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @param		This		このメソッドが実行されるべき"Thisオブジェクト"
	//-----------------------------------------------------------------------
	void FuncCall(
		tScriptEngine * engine,
		tVariantBlock * ret,
		const tString & name, risse_uint32 flags = 0,
		const tMethodArgument & args = tMethodArgument::Empty(),
		const tVariant & This = tVariant::GetNullObject()) const
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
			FuncCall_Primitive(engine,     ret, name, flags, args, This); return;

		case vtObject:	FuncCall_Object   (ret, name, flags, args, This); return;
		}
	}

	void FuncCall_Primitive(tScriptEngine * engine, tVariantBlock * ret, const tString & name, risse_uint32 flags = 0, const tMethodArgument & args = tMethodArgument::Empty(), const tVariant & This = tVariant::GetNullObject()) const;
	void FuncCall_Object   (                             tVariantBlock * ret, const tString & name, risse_uint32 flags = 0, const tMethodArgument & args = tMethodArgument::Empty(), const tVariant & This = tVariant::GetNullObject()) const;

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトのメンバに対する)単純な関数呼び出し		Invoke
	//! @param		engine	スクリプトエンジンインスタンス
	//! @param		membername	メンバ名
	//! @return		戻り値
	//-----------------------------------------------------------------------
	tVariantBlock Invoke(tScriptEngine * engine,
		const tString & membername) const
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
			return Invoke_Primitive(engine, membername);
		case vtObject:
			return Invoke_Object   (        membername);
		}
		return tVariantBlock();
	}

	tVariantBlock Invoke_Primitive(tScriptEngine * engine, const tString & membername) const;
	tVariantBlock Invoke_Object   (                             const tString & membername) const;

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトのメンバに対する)単純な関数呼び出し		Invoke
	//! @param		engine	スクリプトエンジンインスタンス
	//! @param		membername	メンバ名
	//! @param		arg1		引数
	//! @return		戻り値
	//-----------------------------------------------------------------------
	tVariantBlock Invoke(
		tScriptEngine * engine,
		const tString & membername,
		const tVariant & arg1) const
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
			return Invoke_Primitive(engine, membername, arg1);
		case vtObject:
			return Invoke_Object   (        membername,arg1);
		}
		return tVariantBlock();
	}

	tVariantBlock Invoke_Primitive(tScriptEngine * engine, const tString & membername,const tVariant & arg1) const;
	tVariantBlock Invoke_Object   (                             const tString & membername,const tVariant & arg1) const;

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトのメンバに対する)単純な関数呼び出し		Invoke
	//! @param		engine	スクリプトエンジンインスタンス
	//! @param		membername	メンバ名
	//! @param		arg1		引数
	//! @param		arg2		引数
	//! @return		戻り値
	//-----------------------------------------------------------------------
	tVariantBlock Invoke(
		tScriptEngine * engine,
		const tString & membername,
		const tVariant & arg1,
		const tVariant & arg2
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
			return Invoke_Primitive(engine, membername,arg1,arg2);
		case vtObject:
			return Invoke_Object   (        membername,arg1,arg2);
		}
		return tVariantBlock();
	}

	tVariantBlock Invoke_Primitive(tScriptEngine * engine, const tString & membername,const tVariant & arg1,const tVariant & arg2) const;
	tVariantBlock Invoke_Object   (                             const tString & membername,const tVariant & arg1,const tVariant & arg2) const;

public:
	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトに対する)インスタンス作成		New
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @return		新しいインスタンス
	//-----------------------------------------------------------------------
	tVariantBlock New(risse_uint32 flags = 0,
		const tMethodArgument & args = tMethodArgument::Empty()) const // TODO: あれ、Thisは？
	{
		// Object 以外はクラスとしては機能しないため
		// すべて 例外を発生する
		switch(GetType())
		{
		case vtObject:	return New_Object   (tString::GetEmptyString(), flags, args);

		default:
			ThrowCannotCreateInstanceFromNonClassObjectException(); break;
		}
		return tVariantBlock();
	}

	//-----------------------------------------------------------------------
	//! @brief		(このオブジェクトのメンバに対する)インスタンス作成		New
	//! @param		name		関数名
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @return		新しいインスタンス
	//-----------------------------------------------------------------------
	tVariantBlock New(
		const tString & name, risse_uint32 flags = 0,
		const tMethodArgument & args = tMethodArgument::Empty()) const
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

	tVariantBlock New_Void    (const tString & name, risse_uint32 flags, const tMethodArgument & args) const { return tVariantBlock(); /* incomplete */ }
	tVariantBlock New_Integer (const tString & name, risse_uint32 flags, const tMethodArgument & args) const { return tVariantBlock(); /* incomplete */ }
	tVariantBlock New_Real    (const tString & name, risse_uint32 flags, const tMethodArgument & args) const { return tVariantBlock(); /* incomplete */ }
	tVariantBlock New_Null    (const tString & name, risse_uint32 flags, const tMethodArgument & args) const { return tVariantBlock(); /* incomplete */ }
	tVariantBlock New_String  (const tString & name, risse_uint32 flags, const tMethodArgument & args) const { return tVariantBlock(); /* incomplete */ }
	tVariantBlock New_Octet   (const tString & name, risse_uint32 flags, const tMethodArgument & args) const { return tVariantBlock(); /* incomplete */ }
	tVariantBlock New_Boolean (const tString & name, risse_uint32 flags, const tMethodArgument & args) const { return tVariantBlock(); /* incomplete */ }
	tVariantBlock New_Object  (const tString & name, risse_uint32 flags, const tMethodArgument & args) const;


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

	static int GuessTypeLogNot(tGuessType l)
	{
		return gtBoolean | (GuessTypeCastToBoolean(l) & gtEffective);
		// lに関わらず必ずbooleanになる
		// 副作用を持つかどうかはCastToBooleanが副作用を持つかどうかによる
	}

	//-----------------------------------------------------------------------
	//! @brief		単項 ~ 演算子		BitNot
	//! @return		演算結果(通常、integerへのキャストのビットを反転させた物)
	//-----------------------------------------------------------------------
	tVariantBlock BitNot() const
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
		return tVariantBlock();
	}

	tVariantBlock operator ~() const { return BitNot(); }

	// 基本的にビットを反転させた物を返す。

	risse_int64   BitNot_Void     () const { return ~CastToInteger_Void(); }
	risse_int64   BitNot_Integer  () const { return ~CastToInteger_Integer(); }
	risse_int64   BitNot_Real     () const { return ~CastToInteger_Real(); }
	risse_int64   BitNot_Null     () const { ThrowIllegalOperationMethod(mnBitNot); return 0; }
	risse_int64   BitNot_String   () const { ThrowIllegalOperationMethod(mnBitNot); return 0; }
	tOctet        BitNot_Octet    () const { return ~AsOctet(); }
	risse_int64   BitNot_Boolean  () const { ThrowIllegalOperationMethod(mnBitNot); return 0; }
	tVariantBlock BitNot_Object   () const { return Invoke_Object(mnBitNot); }

	static int GuessTypeBitNot(tGuessType l)
	{
		switch(l)
		{
		case gtVoid:		return gtInteger;
		case gtInteger:		return gtInteger;
		case gtReal:		return gtInteger;
		case gtNull:		return gtError		|gtEffective;	// 例外が発生するため
		case gtString:		return gtError		|gtEffective;	// 例外が発生するため
		case gtOctet:		return gtOctet;
		case gtBoolean:		return gtError		|gtEffective;	// 例外が発生するため
		case gtObject:		return gtAny		|gtEffective;	// 何が呼ばれるか分からない
		case gtAny:			return gtAny		|gtEffective;

		default:		return gtAny|gtEffective;
		}
	}

	//-----------------------------------------------------------------------
	//! @brief		++ 演算子			Inc
	//! @return		演算結果(通常、+1 をした数値)
	//-----------------------------------------------------------------------
	tVariantBlock & Inc()
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

	tVariantBlock & operator ++()    { /*前置*/ return Inc(); }
	tVariantBlock   operator ++(int) { /*後置*/ tVariantBlock t = *this; Inc(); return t;}

	tVariantBlock & Inc_Void     () { *this = (risse_int64)1; /* void は 整数の 1になる */ return *this; }
	tVariantBlock & Inc_Integer  () { *this = AsInteger() + 1; return *this; }
	tVariantBlock & Inc_Real     () { *this = AsReal() + 1.0; return *this; }
	tVariantBlock & Inc_Null     () { AddAssign((risse_int64)1); return *this; }
	tVariantBlock & Inc_String   () { AddAssign((risse_int64)1); return *this; }
	tVariantBlock & Inc_Octet    () { AddAssign((risse_int64)1); return *this; }
	tVariantBlock & Inc_Boolean  () { AddAssign((risse_int64)1); return *this; }
	tVariantBlock & Inc_Object   () { *this = Invoke_Object(mnAdd, tVariantBlock((risse_int64)1)); return *this; }

	static int GuessTypeInc(tGuessType l)
	{
		switch(l)
		{
		case gtVoid:		return gtInteger;
		case gtInteger:		return gtInteger;
		case gtReal:		return gtReal;
		case gtNull:		return GuessTypeAdd(l, gtInteger);
		case gtString:		return GuessTypeAdd(l, gtInteger);
		case gtOctet:		return GuessTypeAdd(l, gtInteger);
		case gtBoolean:		return GuessTypeAdd(l, gtInteger);
		case gtObject:		return gtAny	|gtEffective;	// 何が呼ばれるか分からない
		case gtAny:			return gtAny	|gtEffective;

		default:		return gtAny|gtEffective;
		}
	}

	//-----------------------------------------------------------------------
	//! @brief		-- 演算子			Dec
	//! @return		演算結果(通常、-1 をした数値)
	//-----------------------------------------------------------------------
	tVariantBlock & Dec()
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

	tVariantBlock & operator --()    { /*前置*/ return Dec(); }
	tVariantBlock   operator --(int) { /*後置*/ tVariantBlock t = *this; Dec(); return t;}

	tVariantBlock & Dec_Void     () { *this = (risse_int64)-1; /* void は 整数の -1になる */ return *this; }
	tVariantBlock & Dec_Integer  () { *this = AsInteger() - 1; return *this; }
	tVariantBlock & Dec_Real     () { *this = AsReal() - 1.0; return *this; }
	tVariantBlock & Dec_Null     () { SubAssign((risse_int64)1); return *this; }
	tVariantBlock & Dec_String   () { SubAssign((risse_int64)1); return *this; }
	tVariantBlock & Dec_Octet    () { SubAssign((risse_int64)1); return *this; }
	tVariantBlock & Dec_Boolean  () { SubAssign((risse_int64)1); return *this; }
	tVariantBlock & Dec_Object   () { *this = Invoke_Object(mnSub, tVariantBlock((risse_int64)1)); return *this; }

	static int GuessTypeDec(tGuessType l)
	{
		switch(l)
		{
		case gtVoid:		return gtInteger;
		case gtInteger:		return gtInteger;
		case gtReal:		return gtReal;
		case gtNull:		return GuessTypeSub(l, gtInteger);
		case gtString:		return GuessTypeSub(l, gtInteger);
		case gtOctet:		return GuessTypeSub(l, gtInteger);
		case gtBoolean:		return GuessTypeSub(l, gtInteger);
		case gtObject:		return gtAny	|gtEffective;	// 何が呼ばれるか分からない
		case gtAny:			return gtAny	|gtEffective;

		default:		return gtAny|gtEffective;
		}
	}

	//-----------------------------------------------------------------------
	//! @brief		単項 + 演算子		Plus
	//! @return		演算結果(通常、数値へのキャスト)
	//-----------------------------------------------------------------------
	tVariantBlock Plus() const
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
		return tVariantBlock();
	}

	tVariantBlock operator +() const { return Plus(); }

	tVariantBlock Plus_Void     () const { return (risse_int64)0; /* void は 整数の 0 */ }
	tVariantBlock Plus_Integer  () const { return *this; }
	tVariantBlock Plus_Real     () const { return *this; }
	tVariantBlock Plus_Null     () const { ThrowIllegalOperationMethod(mnPlus); return *this; }
	tVariantBlock Plus_String   () const;
	tVariantBlock Plus_Octet    () const { ThrowIllegalOperationMethod(mnPlus); return *this; }
	tVariantBlock Plus_Boolean  () const { return (risse_int64)(CastToBoolean_Boolean() != false);
	                                       /* boolean は 0 か 1 かに変換される */ }
	tVariantBlock Plus_Object   () const { return Invoke_Object(mnPlus); }

	static int GuessTypePlus(tGuessType l)
	{
		switch(l)
		{
		case gtVoid:		return gtInteger;
		case gtInteger:		return gtInteger;
		case gtReal:		return gtReal;
		case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
		case gtString:		return gtAny;	// Integer にも Real にもなりうる
		case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
		case gtBoolean:		return GuessTypeCastToBoolean(l);
		case gtObject:		return gtAny	|gtEffective;	// 何が呼ばれるか分からない
		case gtAny:			return gtAny	|gtEffective;

		default:		return gtAny|gtEffective;
		}
	}

	//-----------------------------------------------------------------------
	//! @brief		単項 - 演算子		Minus
	//! @return		演算結果(通常、符号が反転した物)
	//-----------------------------------------------------------------------
	tVariantBlock Minus() const
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
		return tVariantBlock();
	}

	tVariantBlock operator -() const { return Minus(); }

	tVariantBlock Minus_Void     () const { return (risse_int64)0; }
	tVariantBlock Minus_Integer  () const { return -AsInteger(); }
	tVariantBlock Minus_Real     () const { return -AsReal(); }
	tVariantBlock Minus_Null     () const { ThrowIllegalOperationMethod(mnMinus); return *this; }
	tVariantBlock Minus_String   () const { ThrowIllegalOperationMethod(mnMinus); return *this; }
	tVariantBlock Minus_Octet    () const { ThrowIllegalOperationMethod(mnMinus); return *this; }
	tVariantBlock Minus_Boolean  () const { ThrowIllegalOperationMethod(mnMinus); return *this; }
	tVariantBlock Minus_Object   () const { return Invoke_Object(mnMinus); }

	static int GuessTypeMinus(tGuessType l)
	{
		switch(l)
		{
		case gtVoid:		return gtInteger;
		case gtInteger:		return gtInteger;
		case gtReal:		return gtReal;
		case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
		case gtString:		return gtError	|gtEffective;	// 例外が発生するため
		case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
		case gtBoolean:		return gtError	|gtEffective;	// 例外が発生するため
		case gtObject:		return gtAny	|gtEffective;	// 何が呼ばれるか分からない
		case gtAny:			return gtAny	|gtEffective;

		default:		return gtAny|gtEffective;
		}
	}

	//-----------------------------------------------------------------------
	//! @brief		|| 演算子		LogOr
	//! @return		演算結果(通常、双方のbooleanキャストの論理和)
	//! @note		この演算子はショートカットを行う。すなわち、左辺が真ならば
	//!				右辺は評価されない
	//! @note		この演算子の戻り値は常に bool
	//-----------------------------------------------------------------------
	bool LogOr(const tVariantBlock & rhs) const
	{
		return (bool)*this || (bool)rhs; // 短絡を行う
	}

	bool operator ||(const tVariantBlock & rhs) const { return LogOr(rhs); }

	bool LogOr_Void     (const tVariantBlock & rhs) const { return rhs.operator bool(); }
	bool LogOr_Integer  (const tVariantBlock & rhs) const { return CastToBoolean_Integer() || rhs.operator bool(); }
	bool LogOr_Real     (const tVariantBlock & rhs) const { return CastToBoolean_Real   () || rhs.operator bool(); }
	bool LogOr_Null     (const tVariantBlock & rhs) const { return CastToBoolean_Null   () || rhs.operator bool(); }
	bool LogOr_String   (const tVariantBlock & rhs) const { return CastToBoolean_String () || rhs.operator bool(); }
	bool LogOr_Octet    (const tVariantBlock & rhs) const { return CastToBoolean_Octet  () || rhs.operator bool(); }
	bool LogOr_Boolean  (const tVariantBlock & rhs) const { return CastToBoolean_Boolean() || rhs.operator bool(); }
	bool LogOr_Object   (const tVariantBlock & rhs) const { return CastToBoolean_Object () || rhs.operator bool(); }

	//-----------------------------------------------------------------------
	//! @brief		||= 演算子		LogOrAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & LogOrAssign(const tVariantBlock & rhs)
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
	bool LogAnd(const tVariantBlock & rhs) const
	{
		return (bool)*this && (bool)rhs; // 短絡を行う
	}

	bool operator &&(const tVariantBlock & rhs) const { return LogAnd(rhs); }

	bool LogAnd_Void     (const tVariantBlock & rhs) const { return false; }
	bool LogAnd_Integer  (const tVariantBlock & rhs) const { return CastToBoolean_Integer() && rhs.operator bool(); }
	bool LogAnd_Real     (const tVariantBlock & rhs) const { return CastToBoolean_Real   () && rhs.operator bool(); }
	bool LogAnd_Null     (const tVariantBlock & rhs) const { return CastToBoolean_Null   () && rhs.operator bool(); }
	bool LogAnd_String   (const tVariantBlock & rhs) const { return CastToBoolean_String () && rhs.operator bool(); }
	bool LogAnd_Octet    (const tVariantBlock & rhs) const { return CastToBoolean_Octet  () && rhs.operator bool(); }
	bool LogAnd_Boolean  (const tVariantBlock & rhs) const { return CastToBoolean_Boolean() && rhs.operator bool(); }
	bool LogAnd_Object   (const tVariantBlock & rhs) const { return CastToBoolean_Object () && rhs.operator bool(); }

	//-----------------------------------------------------------------------
	//! @brief		&&= 演算子		LogAndAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & LogAndAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->LogAnd(rhs);
		return *this;
	}

	//-----------------------------------------------------------------------
	//! @brief		| 演算子		BitOr
	//! @return		演算結果(通常、双方のintegerキャストのビット和)
	//-----------------------------------------------------------------------
	tVariantBlock BitOr(const tVariantBlock & rhs) const
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

	tVariantBlock operator |(const tVariantBlock & rhs) const { return BitOr(rhs); }

	tVariantBlock BitOr_Void     (const tVariantBlock & rhs) const;
	tVariantBlock BitOr_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock BitOr_Real     (const tVariantBlock & rhs) const;
	tVariantBlock BitOr_Null     (const tVariantBlock & rhs) const;
	tVariantBlock BitOr_String   (const tVariantBlock & rhs) const;
	tVariantBlock BitOr_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock BitOr_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock BitOr_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnBitOr, rhs); }

	static int GuessTypeBitOr(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeBitOr_Void     (r);
		case gtInteger:		return GuessTypeBitOr_Integer  (r);
		case gtReal:		return GuessTypeBitOr_Real     (r);
		case gtNull:		return GuessTypeBitOr_Null     (r);
		case gtString:		return GuessTypeBitOr_String   (r);
		case gtOctet:		return GuessTypeBitOr_Octet    (r);
		case gtBoolean:		return GuessTypeBitOr_Boolean  (r);
		case gtObject:		return GuessTypeBitOr_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeBitOr_Void     (tGuessType r);
	static int GuessTypeBitOr_Integer  (tGuessType r);
	static int GuessTypeBitOr_Real     (tGuessType r);
	static int GuessTypeBitOr_Null     (tGuessType r);
	static int GuessTypeBitOr_String   (tGuessType r);
	static int GuessTypeBitOr_Octet    (tGuessType r);
	static int GuessTypeBitOr_Boolean  (tGuessType r);
	static int GuessTypeBitOr_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		|= 演算子		BitOrAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & BitOrAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->BitOr(rhs);
		return *this;
	}

	tVariantBlock & operator |=(const tVariantBlock & rhs) { return BitOrAssign(rhs); }

	//-----------------------------------------------------------------------
	//! @brief		^ 演算子		BitXor
	//! @return		演算結果(通常、双方のintegerキャストのビット排他的論理和)
	//-----------------------------------------------------------------------
	tVariantBlock BitXor(const tVariantBlock & rhs) const
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

	tVariantBlock operator ^(const tVariantBlock & rhs) const { return BitXor(rhs); }

	tVariantBlock BitXor_Void     (const tVariantBlock & rhs) const;
	tVariantBlock BitXor_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock BitXor_Real     (const tVariantBlock & rhs) const;
	tVariantBlock BitXor_Null     (const tVariantBlock & rhs) const;
	tVariantBlock BitXor_String   (const tVariantBlock & rhs) const;
	tVariantBlock BitXor_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock BitXor_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock BitXor_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnBitXor, rhs); }

	static int GuessTypeBitXor(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeBitXor_Void     (r);
		case gtInteger:		return GuessTypeBitXor_Integer  (r);
		case gtReal:		return GuessTypeBitXor_Real     (r);
		case gtNull:		return GuessTypeBitXor_Null     (r);
		case gtString:		return GuessTypeBitXor_String   (r);
		case gtOctet:		return GuessTypeBitXor_Octet    (r);
		case gtBoolean:		return GuessTypeBitXor_Boolean  (r);
		case gtObject:		return GuessTypeBitXor_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeBitXor_Void     (tGuessType r);
	static int GuessTypeBitXor_Integer  (tGuessType r);
	static int GuessTypeBitXor_Real     (tGuessType r);
	static int GuessTypeBitXor_Null     (tGuessType r);
	static int GuessTypeBitXor_String   (tGuessType r);
	static int GuessTypeBitXor_Octet    (tGuessType r);
	static int GuessTypeBitXor_Boolean  (tGuessType r);
	static int GuessTypeBitXor_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		^= 演算子		BitXorAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & BitXorAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->BitXor(rhs);
		return *this;
	}

	tVariantBlock & operator ^=(const tVariantBlock & rhs) { return BitXorAssign(rhs); }

	//-----------------------------------------------------------------------
	//! @brief		& 演算子		BitAnd
	//! @return		演算結果(通常、双方のintegerキャストのビット論理積)
	//-----------------------------------------------------------------------
	tVariantBlock BitAnd(const tVariantBlock & rhs) const
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

	tVariantBlock operator &(const tVariantBlock & rhs) const { return BitAnd(rhs); }

	tVariantBlock BitAnd_Void     (const tVariantBlock & rhs) const;
	tVariantBlock BitAnd_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock BitAnd_Real     (const tVariantBlock & rhs) const;
	tVariantBlock BitAnd_Null     (const tVariantBlock & rhs) const;
	tVariantBlock BitAnd_String   (const tVariantBlock & rhs) const;
	tVariantBlock BitAnd_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock BitAnd_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock BitAnd_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnBitAnd, rhs); }

	static int GuessTypeBitAnd(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeBitAnd_Void     (r);
		case gtInteger:		return GuessTypeBitAnd_Integer  (r);
		case gtReal:		return GuessTypeBitAnd_Real     (r);
		case gtNull:		return GuessTypeBitAnd_Null     (r);
		case gtString:		return GuessTypeBitAnd_String   (r);
		case gtOctet:		return GuessTypeBitAnd_Octet    (r);
		case gtBoolean:		return GuessTypeBitAnd_Boolean  (r);
		case gtObject:		return GuessTypeBitAnd_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeBitAnd_Void     (tGuessType r);
	static int GuessTypeBitAnd_Integer  (tGuessType r);
	static int GuessTypeBitAnd_Real     (tGuessType r);
	static int GuessTypeBitAnd_Null     (tGuessType r);
	static int GuessTypeBitAnd_String   (tGuessType r);
	static int GuessTypeBitAnd_Octet    (tGuessType r);
	static int GuessTypeBitAnd_Boolean  (tGuessType r);
	static int GuessTypeBitAnd_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		&= 演算子		BitAndAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & BitAndAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->BitAnd(rhs);
		return *this;
	}

	tVariantBlock & operator &=(const tVariantBlock & rhs) { return BitAndAssign(rhs); }

	//-----------------------------------------------------------------------
	//! @brief		!= 演算子		NotEqual
	//! @return		演算結果
	//! @note		この演算子の戻り値は常に bool
	//-----------------------------------------------------------------------
	bool NotEqual(const tVariantBlock & rhs) const
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

	bool operator !=(const tVariantBlock & rhs) const { return NotEqual(rhs); }

	bool NotEqual_Void     (const tVariantBlock & rhs) const { return !Equal_Void   (rhs); }
	bool NotEqual_Integer  (const tVariantBlock & rhs) const { return !Equal_Integer(rhs); }
	bool NotEqual_Real     (const tVariantBlock & rhs) const { return !Equal_Real   (rhs); }
	bool NotEqual_Null     (const tVariantBlock & rhs) const { return !Equal_Null   (rhs); }
	bool NotEqual_String   (const tVariantBlock & rhs) const { return !Equal_String (rhs); }
	bool NotEqual_Octet    (const tVariantBlock & rhs) const { return !Equal_Octet  (rhs); }
	bool NotEqual_Boolean  (const tVariantBlock & rhs) const { return !Equal_Boolean(rhs); }
	bool NotEqual_Object   (const tVariantBlock & rhs) const { return !Equal_Object (rhs); }

	static int GuessTypeNotEqual(tGuessType l, tGuessType r)
	{
		return GuessTypeEqual(l, r); // Equal と一緒
	}

	//-----------------------------------------------------------------------
	//! @brief		== 演算子		Equal
	//! @return		演算結果
	//! @note		この演算子の戻り値は常に bool
	//-----------------------------------------------------------------------
	tVariantBlock Equal(const tVariantBlock & rhs) const
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

	tVariantBlock operator ==(const tVariantBlock & rhs) const { return Equal(rhs); }

	bool Equal_Void     (const tVariantBlock & rhs) const;
	bool Equal_Integer  (const tVariantBlock & rhs) const;
	bool Equal_Real     (const tVariantBlock & rhs) const;
	bool Equal_Null     (const tVariantBlock & rhs) const;
	bool Equal_String   (const tVariantBlock & rhs) const;
	bool Equal_Octet    (const tVariantBlock & rhs) const;
	bool Equal_Boolean  (const tVariantBlock & rhs) const;
	bool Equal_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnEqual, rhs).CastToBoolean(); }

	static int GuessTypeEqual(tGuessType l, tGuessType r)
	{
		int effective = 0;
		(void)r;
		if(l == gtObject) effective = gtEffective;
		return gtBoolean | effective;
		// l,rに関わらず必ずbooleanになる
		// gtObject の場合は確実に副作用を持つ場合がある
	}

	//-----------------------------------------------------------------------
	//! @brief		!== 演算子		DiscNotEqual
	//! @return		演算結果
	//! @note		この演算子の戻り値は常に bool
	//-----------------------------------------------------------------------
	bool DiscNotEqual(const tVariantBlock & rhs) const
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

	bool DiscNotEqual_Void     (const tVariantBlock & rhs) const { return !DiscEqual_Void   (rhs); }
	bool DiscNotEqual_Integer  (const tVariantBlock & rhs) const { return !DiscEqual_Integer(rhs); }
	bool DiscNotEqual_Real     (const tVariantBlock & rhs) const { return !DiscEqual_Real   (rhs); }
	bool DiscNotEqual_Null     (const tVariantBlock & rhs) const { return !DiscEqual_Null   (rhs); }
	bool DiscNotEqual_String   (const tVariantBlock & rhs) const { return !DiscEqual_String (rhs); }
	bool DiscNotEqual_Octet    (const tVariantBlock & rhs) const { return !DiscEqual_Octet  (rhs); }
	bool DiscNotEqual_Boolean  (const tVariantBlock & rhs) const { return !DiscEqual_Boolean(rhs); }
	bool DiscNotEqual_Object   (const tVariantBlock & rhs) const { return !DiscEqual_Object (rhs); }

	static int GuessTypeDiscNotEqual(tGuessType l, tGuessType r)
	{
		return GuessTypeDiscEqual(l, r); // GuessTypeDiscEqual と一緒
	}

	//-----------------------------------------------------------------------
	//! @brief		識別 === 演算子		DiscEqual
	//! @param		rhs			右辺
	//! @return		識別の結果、同一ならば真、そうでなければ偽
	//-----------------------------------------------------------------------
	bool DiscEqual(const tVariantBlock & rhs) const
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

	bool DiscEqual_Void     (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtVoid; }
	bool DiscEqual_Integer  (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtInteger && rhs.AsInteger() == AsInteger(); }
	bool DiscEqual_Real     (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtReal && rhs.AsReal() == AsReal(); }
	bool DiscEqual_Null     (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtNull; }
	bool DiscEqual_String   (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtString && rhs.AsString() == AsString(); }
	bool DiscEqual_Octet    (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtOctet && rhs.AsOctet() == AsOctet(); }
	bool DiscEqual_Boolean  (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtBoolean && rhs.CastToBoolean_Boolean() == CastToBoolean_Boolean(); }
	bool DiscEqual_Object   (const tVariantBlock & rhs) const
			{ return Invoke_Object(mnDiscEqual, rhs).CastToBoolean(); }

	static int GuessTypeDiscEqual(tGuessType l, tGuessType r)
	{
		int effective = 0;
		(void)r;
		if(l == gtObject) effective = gtEffective;
		return gtBoolean | effective;
		// l,rに関わらず必ずbooleanになる
		// gtObject の場合は確実に副作用を持つ場合がある
	}

	//-----------------------------------------------------------------------
	//! @brief		厳密な識別演算子		StrictEqual
	//! @param		rhs			右辺
	//! @return		識別の結果、同一ならば真、そうでなければ偽。
	//!				=== 演算子と異なり、real の符合判断も厳密に行う。
	//!				オブジェクト型の場合、インスタンスのポインタが同一があることで
	//!				真とみなす。
	//-----------------------------------------------------------------------
	bool StrictEqual(const tVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return StrictEqual_Void     (rhs);
		case vtInteger:	return StrictEqual_Integer  (rhs);
		case vtReal:	return StrictEqual_Real     (rhs);
		case vtNull:	return StrictEqual_Null     (rhs);
		case vtString:	return StrictEqual_String   (rhs);
		case vtOctet:	return StrictEqual_Octet    (rhs);
		case vtBoolean:	return StrictEqual_Boolean  (rhs);
		case vtObject:	return StrictEqual_Object   (rhs);
		}
		return false;
	}

	bool StrictEqual_Void     (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtVoid; }
	bool StrictEqual_Integer  (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtInteger && rhs.AsInteger() == AsInteger(); }
	bool StrictEqual_Real     (const tVariantBlock & rhs) const;
	bool StrictEqual_Null     (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtNull; }
	bool StrictEqual_String   (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtString && rhs.AsString() == AsString(); }
	bool StrictEqual_Octet    (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtOctet && rhs.AsOctet() == AsOctet(); }
	bool StrictEqual_Boolean  (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtBoolean && rhs.CastToBoolean_Boolean() == CastToBoolean_Boolean(); }
	bool StrictEqual_Object   (const tVariantBlock & rhs) const
			{ return rhs.GetType() == vtObject && rhs.AsObject().StrictEqual(AsObject()); }

	//-----------------------------------------------------------------------
	//! @brief		同定演算子		Identify
	//! @param		rhs			右辺
	//! @return		同定の結果、同一ならば真、そうでなければ偽。
	//!				Dictionary のキーの比較に用いられる。
	//-----------------------------------------------------------------------
	bool Identify(const tVariantBlock & rhs) const
	{
		switch(GetType())
		{
		case vtVoid:	return Identify_Void     (rhs);
		case vtInteger:	return Identify_Integer  (rhs);
		case vtReal:	return Identify_Real     (rhs);
		case vtNull:	return Identify_Null     (rhs);
		case vtString:	return Identify_String   (rhs);
		case vtOctet:	return Identify_Octet    (rhs);
		case vtBoolean:	return Identify_Boolean  (rhs);
		case vtObject:	return Identify_Object   (rhs);
		}
		return false;
	}

	bool Identify_Void     (const tVariantBlock & rhs) const { return StrictEqual_Void     (rhs); }
	bool Identify_Integer  (const tVariantBlock & rhs) const { return StrictEqual_Integer  (rhs); }
	bool Identify_Real     (const tVariantBlock & rhs) const { return StrictEqual_Real     (rhs); }
	bool Identify_Null     (const tVariantBlock & rhs) const { return StrictEqual_Null     (rhs); }
	bool Identify_String   (const tVariantBlock & rhs) const { return StrictEqual_String   (rhs); }
	bool Identify_Octet    (const tVariantBlock & rhs) const { return StrictEqual_Octet    (rhs); }
	bool Identify_Boolean  (const tVariantBlock & rhs) const { return StrictEqual_Boolean  (rhs); }
	bool Identify_Object   (const tVariantBlock & rhs) const;

	//-----------------------------------------------------------------------
	//! @brief		< 演算子		Lesser
	//! @param		rhs			右辺
	//! @return		*this < rhs ならば真
	//-----------------------------------------------------------------------
	bool Lesser(const tVariantBlock & rhs) const
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

	bool operator < (const tVariantBlock & rhs) const { return Lesser(rhs); }

	bool Lesser_Void     (const tVariantBlock & rhs) const;
	bool Lesser_Integer  (const tVariantBlock & rhs) const;
	bool Lesser_Real     (const tVariantBlock & rhs) const;
	bool Lesser_Null     (const tVariantBlock & rhs) const;
	bool Lesser_String   (const tVariantBlock & rhs) const;
	bool Lesser_Octet    (const tVariantBlock & rhs) const;
	bool Lesser_Boolean  (const tVariantBlock & rhs) const;
	bool Lesser_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnLesser, rhs); }

	static int GuessTypeLesser(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeLesser_Void     (r);
		case gtInteger:		return GuessTypeLesser_Integer  (r);
		case gtReal:		return GuessTypeLesser_Real     (r);
		case gtNull:		return GuessTypeLesser_Null     (r);
		case gtString:		return GuessTypeLesser_String   (r);
		case gtOctet:		return GuessTypeLesser_Octet    (r);
		case gtBoolean:		return GuessTypeLesser_Boolean  (r);
		case gtObject:		return GuessTypeLesser_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeLesser_Void     (tGuessType r);
	static int GuessTypeLesser_Integer  (tGuessType r);
	static int GuessTypeLesser_Real     (tGuessType r);
	static int GuessTypeLesser_Null     (tGuessType r);
	static int GuessTypeLesser_String   (tGuessType r);
	static int GuessTypeLesser_Octet    (tGuessType r);
	static int GuessTypeLesser_Boolean  (tGuessType r);
	static int GuessTypeLesser_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		> 演算子		Greater
	//! @param		rhs			右辺
	//! @return		*this > rhs ならば真
	//-----------------------------------------------------------------------
	bool Greater(const tVariantBlock & rhs) const
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

	bool operator > (const tVariantBlock & rhs) const { return Greater(rhs); }

	bool Greater_Void     (const tVariantBlock & rhs) const;
	bool Greater_Integer  (const tVariantBlock & rhs) const;
	bool Greater_Real     (const tVariantBlock & rhs) const;
	bool Greater_Null     (const tVariantBlock & rhs) const;
	bool Greater_String   (const tVariantBlock & rhs) const;
	bool Greater_Octet    (const tVariantBlock & rhs) const;
	bool Greater_Boolean  (const tVariantBlock & rhs) const;
	bool Greater_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnGreater, rhs); }

	static int GuessTypeGreater(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeGreater_Void     (r);
		case gtInteger:		return GuessTypeGreater_Integer  (r);
		case gtReal:		return GuessTypeGreater_Real     (r);
		case gtNull:		return GuessTypeGreater_Null     (r);
		case gtString:		return GuessTypeGreater_String   (r);
		case gtOctet:		return GuessTypeGreater_Octet    (r);
		case gtBoolean:		return GuessTypeGreater_Boolean  (r);
		case gtObject:		return GuessTypeGreater_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeGreater_Void     (tGuessType r);
	static int GuessTypeGreater_Integer  (tGuessType r);
	static int GuessTypeGreater_Real     (tGuessType r);
	static int GuessTypeGreater_Null     (tGuessType r);
	static int GuessTypeGreater_String   (tGuessType r);
	static int GuessTypeGreater_Octet    (tGuessType r);
	static int GuessTypeGreater_Boolean  (tGuessType r);
	static int GuessTypeGreater_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		<= 演算子		LesserOrEqual
	//! @param		rhs			右辺
	//! @return		*this < rhs ならば真
	//-----------------------------------------------------------------------
	bool LesserOrEqual(const tVariantBlock & rhs) const
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

	bool operator <= (const tVariantBlock & rhs) const { return LesserOrEqual(rhs); }

	bool LesserOrEqual_Void     (const tVariantBlock & rhs) const;
	bool LesserOrEqual_Integer  (const tVariantBlock & rhs) const;
	bool LesserOrEqual_Real     (const tVariantBlock & rhs) const;
	bool LesserOrEqual_Null     (const tVariantBlock & rhs) const;
	bool LesserOrEqual_String   (const tVariantBlock & rhs) const;
	bool LesserOrEqual_Octet    (const tVariantBlock & rhs) const;
	bool LesserOrEqual_Boolean  (const tVariantBlock & rhs) const;
	bool LesserOrEqual_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnLesserOrEqual, rhs); }


	static int GuessTypeLesserOrEqual(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeLesserOrEqual_Void     (r);
		case gtInteger:		return GuessTypeLesserOrEqual_Integer  (r);
		case gtReal:		return GuessTypeLesserOrEqual_Real     (r);
		case gtNull:		return GuessTypeLesserOrEqual_Null     (r);
		case gtString:		return GuessTypeLesserOrEqual_String   (r);
		case gtOctet:		return GuessTypeLesserOrEqual_Octet    (r);
		case gtBoolean:		return GuessTypeLesserOrEqual_Boolean  (r);
		case gtObject:		return GuessTypeLesserOrEqual_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeLesserOrEqual_Void     (tGuessType r) { return GuessTypeGreater_Void     (r); }
	static int GuessTypeLesserOrEqual_Integer  (tGuessType r) { return GuessTypeGreater_Integer  (r); }
	static int GuessTypeLesserOrEqual_Real     (tGuessType r) { return GuessTypeGreater_Real     (r); }
	static int GuessTypeLesserOrEqual_Null     (tGuessType r) { return GuessTypeGreater_Null     (r); }
	static int GuessTypeLesserOrEqual_String   (tGuessType r) { return GuessTypeGreater_String   (r); }
	static int GuessTypeLesserOrEqual_Octet    (tGuessType r) { return GuessTypeGreater_Octet    (r); }
	static int GuessTypeLesserOrEqual_Boolean  (tGuessType r) { return GuessTypeGreater_Boolean  (r); }
	static int GuessTypeLesserOrEqual_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		>= 演算子		GreaterOrEqual
	//! @param		rhs			右辺
	//! @return		*this < rhs ならば真
	//-----------------------------------------------------------------------
	bool GreaterOrEqual(const tVariantBlock & rhs) const
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

	bool operator >= (const tVariantBlock & rhs) const { return GreaterOrEqual(rhs); }

	bool GreaterOrEqual_Void     (const tVariantBlock & rhs) const;
	bool GreaterOrEqual_Integer  (const tVariantBlock & rhs) const;
	bool GreaterOrEqual_Real     (const tVariantBlock & rhs) const;
	bool GreaterOrEqual_Null     (const tVariantBlock & rhs) const;
	bool GreaterOrEqual_String   (const tVariantBlock & rhs) const;
	bool GreaterOrEqual_Octet    (const tVariantBlock & rhs) const;
	bool GreaterOrEqual_Boolean  (const tVariantBlock & rhs) const;
	bool GreaterOrEqual_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnGreaterOrEqual, rhs); }

	static int GuessTypeGreaterOrEqual(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeGreaterOrEqual_Void     (r);
		case gtInteger:		return GuessTypeGreaterOrEqual_Integer  (r);
		case gtReal:		return GuessTypeGreaterOrEqual_Real     (r);
		case gtNull:		return GuessTypeGreaterOrEqual_Null     (r);
		case gtString:		return GuessTypeGreaterOrEqual_String   (r);
		case gtOctet:		return GuessTypeGreaterOrEqual_Octet    (r);
		case gtBoolean:		return GuessTypeGreaterOrEqual_Boolean  (r);
		case gtObject:		return GuessTypeGreaterOrEqual_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeGreaterOrEqual_Void     (tGuessType r) { return GuessTypeLesser_Void     (r); }
	static int GuessTypeGreaterOrEqual_Integer  (tGuessType r) { return GuessTypeLesser_Integer  (r); }
	static int GuessTypeGreaterOrEqual_Real     (tGuessType r) { return GuessTypeLesser_Real     (r); }
	static int GuessTypeGreaterOrEqual_Null     (tGuessType r) { return GuessTypeLesser_Null     (r); }
	static int GuessTypeGreaterOrEqual_String   (tGuessType r) { return GuessTypeLesser_String   (r); }
	static int GuessTypeGreaterOrEqual_Octet    (tGuessType r) { return GuessTypeLesser_Octet    (r); }
	static int GuessTypeGreaterOrEqual_Boolean  (tGuessType r) { return GuessTypeLesser_Boolean  (r); }
	static int GuessTypeGreaterOrEqual_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		>>> 演算子(符号なし右シフト)		RBitShift
	//! @param		rhs			右辺
	//! @return		通常、左辺をintegerにキャストした後右辺回数分シフトしたもの
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が integer だとは限らない
	//-----------------------------------------------------------------------
	tVariantBlock RBitShift(const tVariantBlock & rhs) const
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

	tVariantBlock RBitShift_Void     (const tVariantBlock & rhs) const;
	tVariantBlock RBitShift_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock RBitShift_Real     (const tVariantBlock & rhs) const;
	tVariantBlock RBitShift_Null     (const tVariantBlock & rhs) const;
	tVariantBlock RBitShift_String   (const tVariantBlock & rhs) const;
	tVariantBlock RBitShift_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock RBitShift_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock RBitShift_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnRBitShift, rhs); }

	static int GuessTypeRBitShift(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeRBitShift_Void     (r);
		case gtInteger:		return GuessTypeRBitShift_Integer  (r);
		case gtReal:		return GuessTypeRBitShift_Real     (r);
		case gtNull:		return GuessTypeRBitShift_Null     (r);
		case gtString:		return GuessTypeRBitShift_String   (r);
		case gtOctet:		return GuessTypeRBitShift_Octet    (r);
		case gtBoolean:		return GuessTypeRBitShift_Boolean  (r);
		case gtObject:		return GuessTypeRBitShift_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeRBitShift_Void     (tGuessType r);
	static int GuessTypeRBitShift_Integer  (tGuessType r);
	static int GuessTypeRBitShift_Real     (tGuessType r);
	static int GuessTypeRBitShift_Null     (tGuessType r);
	static int GuessTypeRBitShift_String   (tGuessType r);
	static int GuessTypeRBitShift_Octet    (tGuessType r);
	static int GuessTypeRBitShift_Boolean  (tGuessType r);
	static int GuessTypeRBitShift_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		>>>= 演算子		RBitShiftAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & RBitShiftAssign(const tVariantBlock & rhs)
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
	tVariantBlock LShift(const tVariantBlock & rhs) const
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

	tVariantBlock operator << (const tVariantBlock & rhs) const { return LShift(rhs); }

	tVariantBlock LShift_Void     (const tVariantBlock & rhs) const;
	tVariantBlock LShift_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock LShift_Real     (const tVariantBlock & rhs) const;
	tVariantBlock LShift_Null     (const tVariantBlock & rhs) const;
	tVariantBlock LShift_String   (const tVariantBlock & rhs) const;
	tVariantBlock LShift_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock LShift_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock LShift_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnLShift, rhs); }

	static int GuessTypeLShift(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeLShift_Void     (r);
		case gtInteger:		return GuessTypeLShift_Integer  (r);
		case gtReal:		return GuessTypeLShift_Real     (r);
		case gtNull:		return GuessTypeLShift_Null     (r);
		case gtString:		return GuessTypeLShift_String   (r);
		case gtOctet:		return GuessTypeLShift_Octet    (r);
		case gtBoolean:		return GuessTypeLShift_Boolean  (r);
		case gtObject:		return GuessTypeLShift_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeLShift_Void     (tGuessType r);
	static int GuessTypeLShift_Integer  (tGuessType r);
	static int GuessTypeLShift_Real     (tGuessType r);
	static int GuessTypeLShift_Null     (tGuessType r);
	static int GuessTypeLShift_String   (tGuessType r);
	static int GuessTypeLShift_Octet    (tGuessType r);
	static int GuessTypeLShift_Boolean  (tGuessType r);
	static int GuessTypeLShift_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		<<= 演算子		LShiftAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & LShiftAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->LShift(rhs);
		return *this;
	}

	tVariantBlock & operator <<=(const tVariantBlock & rhs) { return LShiftAssign(rhs); }

	//-----------------------------------------------------------------------
	//! @brief		>> 演算子(符号つき右シフト)		RShift
	//! @param		rhs			右辺
	//! @return		通常、左辺をintegerにキャストした後右辺回数分シフトしたもの
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が integer だとは限らない
	//-----------------------------------------------------------------------
	tVariantBlock RShift(const tVariantBlock & rhs) const
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

	tVariantBlock operator >> (const tVariantBlock & rhs) const { return RShift(rhs); }

	tVariantBlock RShift_Void     (const tVariantBlock & rhs) const;
	tVariantBlock RShift_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock RShift_Real     (const tVariantBlock & rhs) const;
	tVariantBlock RShift_Null     (const tVariantBlock & rhs) const;
	tVariantBlock RShift_String   (const tVariantBlock & rhs) const;
	tVariantBlock RShift_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock RShift_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock RShift_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnRShift, rhs); }

	static int GuessTypeRShift(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeRShift_Void     (r);
		case gtInteger:		return GuessTypeRShift_Integer  (r);
		case gtReal:		return GuessTypeRShift_Real     (r);
		case gtNull:		return GuessTypeRShift_Null     (r);
		case gtString:		return GuessTypeRShift_String   (r);
		case gtOctet:		return GuessTypeRShift_Octet    (r);
		case gtBoolean:		return GuessTypeRShift_Boolean  (r);
		case gtObject:		return GuessTypeRShift_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeRShift_Void     (tGuessType r);
	static int GuessTypeRShift_Integer  (tGuessType r);
	static int GuessTypeRShift_Real     (tGuessType r);
	static int GuessTypeRShift_Null     (tGuessType r);
	static int GuessTypeRShift_String   (tGuessType r);
	static int GuessTypeRShift_Octet    (tGuessType r);
	static int GuessTypeRShift_Boolean  (tGuessType r);
	static int GuessTypeRShift_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		>>= 演算子		RShiftAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & RShiftAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->RShift(rhs);
		return *this;
	}

	tVariantBlock & operator >>=(const tVariantBlock & rhs) { return RShiftAssign(rhs); }

	//-----------------------------------------------------------------------
	//! @brief		% 演算子(剰余)		Mod
	//! @param		rhs			右辺
	//! @return		通常、両方をintegerにキャストし、左辺を右辺で割ったあまり
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が integer だとは限らない
	//-----------------------------------------------------------------------
	tVariantBlock Mod(const tVariantBlock & rhs) const
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

	tVariantBlock operator % (const tVariantBlock & rhs) const { return Mod(rhs); }

	tVariantBlock Mod_Void     (const tVariantBlock & rhs) const;
	tVariantBlock Mod_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock Mod_Real     (const tVariantBlock & rhs) const;
	tVariantBlock Mod_Null     (const tVariantBlock & rhs) const;
	tVariantBlock Mod_String   (const tVariantBlock & rhs) const;
	tVariantBlock Mod_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock Mod_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock Mod_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnMod, rhs); }

	static int GuessTypeMod(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeMod_Void     (r);
		case gtInteger:		return GuessTypeMod_Integer  (r);
		case gtReal:		return GuessTypeMod_Real     (r);
		case gtNull:		return GuessTypeMod_Null     (r);
		case gtString:		return GuessTypeMod_String   (r);
		case gtOctet:		return GuessTypeMod_Octet    (r);
		case gtBoolean:		return GuessTypeMod_Boolean  (r);
		case gtObject:		return GuessTypeMod_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeMod_Void     (tGuessType r);
	static int GuessTypeMod_Integer  (tGuessType r);
	static int GuessTypeMod_Real     (tGuessType r);
	static int GuessTypeMod_Null     (tGuessType r);
	static int GuessTypeMod_String   (tGuessType r);
	static int GuessTypeMod_Octet    (tGuessType r);
	static int GuessTypeMod_Boolean  (tGuessType r);
	static int GuessTypeMod_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		%= 演算子		ModAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & ModAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Mod(rhs);
		return *this;
	}

	tVariantBlock & operator %=(const tVariantBlock & rhs) { return ModAssign(rhs); }

	//-----------------------------------------------------------------------
	//! @brief		/ 演算子(剰余)		Div
	//! @param		rhs			右辺
	//! @return		通常、両方を real にキャストし、左辺を右辺で割ったもの
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が real だとは限らない
	//-----------------------------------------------------------------------
	tVariantBlock Div(const tVariantBlock & rhs) const
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

	tVariantBlock operator / (const tVariantBlock & rhs) const { return Div(rhs); }

	tVariantBlock Div_Void     (const tVariantBlock & rhs) const;
	tVariantBlock Div_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock Div_Real     (const tVariantBlock & rhs) const;
	tVariantBlock Div_Null     (const tVariantBlock & rhs) const;
	tVariantBlock Div_String   (const tVariantBlock & rhs) const;
	tVariantBlock Div_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock Div_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock Div_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnDiv, rhs); }

	static int GuessTypeDiv(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeDiv_Void     (r);
		case gtInteger:		return GuessTypeDiv_Integer  (r);
		case gtReal:		return GuessTypeDiv_Real     (r);
		case gtNull:		return GuessTypeDiv_Null     (r);
		case gtString:		return GuessTypeDiv_String   (r);
		case gtOctet:		return GuessTypeDiv_Octet    (r);
		case gtBoolean:		return GuessTypeDiv_Boolean  (r);
		case gtObject:		return GuessTypeDiv_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeDiv_Void     (tGuessType r);
	static int GuessTypeDiv_Integer  (tGuessType r);
	static int GuessTypeDiv_Real     (tGuessType r);
	static int GuessTypeDiv_Null     (tGuessType r);
	static int GuessTypeDiv_String   (tGuessType r);
	static int GuessTypeDiv_Octet    (tGuessType r);
	static int GuessTypeDiv_Boolean  (tGuessType r);
	static int GuessTypeDiv_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		/= 演算子		DivAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & DivAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Div(rhs);
		return *this;
	}

	tVariantBlock & operator /=(const tVariantBlock & rhs) { return DivAssign(rhs); }

	//-----------------------------------------------------------------------
	//! @brief		\ 演算子(整数除算)		Idiv
	//! @param		rhs			右辺
	//! @return		通常、両方をintegerにキャストし、左辺を右辺で割ったもの
	//! @note		オブジェクトが演算子をオーバーロードしている可能性もあるので
	//!				戻り値が integer だとは限らない
	//-----------------------------------------------------------------------
	tVariantBlock Idiv(const tVariantBlock & rhs) const
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

	tVariantBlock Idiv_Void     (const tVariantBlock & rhs) const;
	tVariantBlock Idiv_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock Idiv_Real     (const tVariantBlock & rhs) const;
	tVariantBlock Idiv_Null     (const tVariantBlock & rhs) const;
	tVariantBlock Idiv_String   (const tVariantBlock & rhs) const;
	tVariantBlock Idiv_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock Idiv_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock Idiv_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnIdiv, rhs); }

	static int GuessTypeIdiv(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeIdiv_Void     (r);
		case gtInteger:		return GuessTypeIdiv_Integer  (r);
		case gtReal:		return GuessTypeIdiv_Real     (r);
		case gtNull:		return GuessTypeIdiv_Null     (r);
		case gtString:		return GuessTypeIdiv_String   (r);
		case gtOctet:		return GuessTypeIdiv_Octet    (r);
		case gtBoolean:		return GuessTypeIdiv_Boolean  (r);
		case gtObject:		return GuessTypeIdiv_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeIdiv_Void     (tGuessType r);
	static int GuessTypeIdiv_Integer  (tGuessType r);
	static int GuessTypeIdiv_Real     (tGuessType r);
	static int GuessTypeIdiv_Null     (tGuessType r);
	static int GuessTypeIdiv_String   (tGuessType r);
	static int GuessTypeIdiv_Octet    (tGuessType r);
	static int GuessTypeIdiv_Boolean  (tGuessType r);
	static int GuessTypeIdiv_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		\= 演算子		IdivAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & IdivAssign(const tVariantBlock & rhs)
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
	tVariantBlock Mul(const tVariantBlock & rhs) const
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

	tVariantBlock operator * (const tVariantBlock & rhs) const { return Mul(rhs); }

	tVariantBlock Mul_Void     (const tVariantBlock & rhs) const;
	tVariantBlock Mul_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock Mul_Real     (const tVariantBlock & rhs) const;
	tVariantBlock Mul_Null     (const tVariantBlock & rhs) const;
	tVariantBlock Mul_String   (const tVariantBlock & rhs) const;
	tVariantBlock Mul_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock Mul_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock Mul_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnMul, rhs); }

	static int GuessTypeMul(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeMul_Void     (r);
		case gtInteger:		return GuessTypeMul_Integer  (r);
		case gtReal:		return GuessTypeMul_Real     (r);
		case gtNull:		return GuessTypeMul_Null     (r);
		case gtString:		return GuessTypeMul_String   (r);
		case gtOctet:		return GuessTypeMul_Octet    (r);
		case gtBoolean:		return GuessTypeMul_Boolean  (r);
		case gtObject:		return GuessTypeMul_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeMul_Void     (tGuessType r);
	static int GuessTypeMul_Integer  (tGuessType r);
	static int GuessTypeMul_Real     (tGuessType r);
	static int GuessTypeMul_Null     (tGuessType r);
	static int GuessTypeMul_String   (tGuessType r);
	static int GuessTypeMul_Octet    (tGuessType r);
	static int GuessTypeMul_Boolean  (tGuessType r);
	static int GuessTypeMul_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		*= 演算子		MulAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & MulAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Mul(rhs);
		return *this;
	}

	tVariantBlock & operator *=(const tVariantBlock & rhs) { return MulAssign(rhs); }

	//-----------------------------------------------------------------------
	//! @brief		+ 演算子(加算)		Add
	//! @param		rhs			右辺
	//! @return		左辺に右辺を加算した物
	//-----------------------------------------------------------------------
	tVariantBlock Add(const tVariantBlock & rhs) const
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

	tVariantBlock operator + (const tVariantBlock & rhs) const { return Add(rhs); }

	tVariantBlock Add_Void     (const tVariantBlock & rhs) const;
	tVariantBlock Add_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock Add_Real     (const tVariantBlock & rhs) const;
	tVariantBlock Add_Null     (const tVariantBlock & rhs) const;
	tVariantBlock Add_String   (const tVariantBlock & rhs) const;
	tVariantBlock Add_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock Add_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock Add_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnAdd, rhs); }

	static int GuessTypeAdd(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeAdd_Void     (r);
		case gtInteger:		return GuessTypeAdd_Integer  (r);
		case gtReal:		return GuessTypeAdd_Real     (r);
		case gtNull:		return GuessTypeAdd_Null     (r);
		case gtString:		return GuessTypeAdd_String   (r);
		case gtOctet:		return GuessTypeAdd_Octet    (r);
		case gtBoolean:		return GuessTypeAdd_Boolean  (r);
		case gtObject:		return GuessTypeAdd_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeAdd_Void     (tGuessType r);
	static int GuessTypeAdd_Integer  (tGuessType r);
	static int GuessTypeAdd_Real     (tGuessType r);
	static int GuessTypeAdd_Null     (tGuessType r);
	static int GuessTypeAdd_String   (tGuessType r);
	static int GuessTypeAdd_Octet    (tGuessType r);
	static int GuessTypeAdd_Boolean  (tGuessType r);
	static int GuessTypeAdd_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		+= 演算子		AddAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & AddAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Add(rhs);
		return *this;
	}

	tVariantBlock & operator +=(const tVariantBlock & rhs) { return AddAssign(rhs); }

	//-----------------------------------------------------------------------
	//! @brief		- 演算子(減算)		Sub
	//! @param		rhs			右辺
	//! @return		左辺から右辺を減算した物
	//-----------------------------------------------------------------------
	tVariantBlock Sub(const tVariantBlock & rhs) const
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

	tVariantBlock operator - (const tVariantBlock & rhs) const { return Sub(rhs); }

	tVariantBlock Sub_Void     (const tVariantBlock & rhs) const;
	tVariantBlock Sub_Integer  (const tVariantBlock & rhs) const;
	tVariantBlock Sub_Real     (const tVariantBlock & rhs) const;
	tVariantBlock Sub_Null     (const tVariantBlock & rhs) const;
	tVariantBlock Sub_String   (const tVariantBlock & rhs) const;
	tVariantBlock Sub_Octet    (const tVariantBlock & rhs) const;
	tVariantBlock Sub_Boolean  (const tVariantBlock & rhs) const;
	tVariantBlock Sub_Object   (const tVariantBlock & rhs) const { return Invoke_Object(mnSub, rhs); }

	static int GuessTypeSub(tGuessType l, tGuessType r)
	{
		switch(l)
		{
		case gtVoid:		return GuessTypeSub_Void     (r);
		case gtInteger:		return GuessTypeSub_Integer  (r);
		case gtReal:		return GuessTypeSub_Real     (r);
		case gtNull:		return GuessTypeSub_Null     (r);
		case gtString:		return GuessTypeSub_String   (r);
		case gtOctet:		return GuessTypeSub_Octet    (r);
		case gtBoolean:		return GuessTypeSub_Boolean  (r);
		case gtObject:		return GuessTypeSub_Object   (r);
		case gtAny:			return gtAny;

		default:		return gtAny|gtEffective;
		}
	}

	static int GuessTypeSub_Void     (tGuessType r);
	static int GuessTypeSub_Integer  (tGuessType r);
	static int GuessTypeSub_Real     (tGuessType r);
	static int GuessTypeSub_Null     (tGuessType r);
	static int GuessTypeSub_String   (tGuessType r);
	static int GuessTypeSub_Octet    (tGuessType r);
	static int GuessTypeSub_Boolean  (tGuessType r);
	static int GuessTypeSub_Object   (tGuessType r) { return gtAny|gtEffective; }

	//-----------------------------------------------------------------------
	//! @brief		-= 演算子		SubAssign
	//! @return		演算後の*thisへの参照
	//-----------------------------------------------------------------------
	tVariantBlock & SubAssign(const tVariantBlock & rhs)
	{
		// TODO: より効率的な実装
		*this = this->Sub(rhs);
		return *this;
	}

	tVariantBlock & operator -=(const tVariantBlock & rhs) { return SubAssign(rhs); }

	//-----------------------------------------------------------------------
	//! @brief		instanceof 演算子(instanceof)		InstanceOf
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		rhs			右辺
	//! @return		左辺が右辺で示したクラスのインスタンスならば真
	//-----------------------------------------------------------------------
	bool InstanceOf(tScriptEngine * engine, const tVariantBlock & rhs) const;


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

	risse_int64 CastToInteger          () const { return operator risse_int64(); }

	risse_int64 CastToInteger_Void     () const { return false; /* void は 0 */}
	risse_int64 CastToInteger_Integer  () const { return AsInteger(); }
	risse_int64 CastToInteger_Real     () const { return (risse_int64)AsReal(); }
	risse_int64 CastToInteger_Null     () const { ThrowIllegalOperationMethod(mnInteger); return (risse_int64)0; }
	risse_int64 CastToInteger_String   () const;
	risse_int64 CastToInteger_Octet    () const { ThrowIllegalOperationMethod(mnInteger); return (risse_int64)0; }
	risse_int64 CastToInteger_Boolean  () const { return (risse_int64)CastToBoolean_Boolean(); }
	risse_int64 CastToInteger_Object   () const { return Invoke_Object(mnInteger).CastToInteger(); }

	static int GuessTypeCastToInteger(tGuessType l)
	{
		switch(l)
		{
		case gtVoid:		return gtInteger;
		case gtInteger:		return gtInteger;
		case gtReal:		return gtInteger;
		case gtNull:		return gtError		|gtEffective;
		case gtString:		return gtInteger;
		case gtOctet:		return gtError		|gtEffective;
		case gtBoolean:		return gtInteger;
		case gtObject:		return gtInteger	|gtEffective;	// 何が呼ばれるか分からないが強制的に String にキャストされる
		case gtAny:			return gtInteger	|gtEffective;

		default:		return gtAny|gtEffective;
		}
	}

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

	risse_real CastToReal          () const { return operator risse_real(); }

	risse_real CastToReal_Void     () const { return (risse_real)0.0; }
	risse_real CastToReal_Integer  () const { return AsInteger(); }
	risse_real CastToReal_Real     () const { return AsReal(); }
	risse_real CastToReal_Null     () const { ThrowIllegalOperationMethod(mnReal); return (risse_real)0.0; }
	risse_real CastToReal_String   () const { return (risse_real)Plus_String(); /* Plus_String の戻りを risse_real に再キャスト */ }
	risse_real CastToReal_Octet    () const { ThrowIllegalOperationMethod(mnReal); return (risse_real)0.0; }
	risse_real CastToReal_Boolean  () const { return (risse_real)(int)CastToBoolean_Boolean(); }
	risse_real CastToReal_Object   () const { return Invoke_Object(mnReal).CastToReal(); }

	static int GuessTypeCastToReal(tGuessType l)
	{
		switch(l)
		{
		case gtVoid:		return gtReal;
		case gtInteger:		return gtReal;
		case gtReal:		return gtReal;
		case gtNull:		return gtError	|gtEffective;	// 例外が発生するため
		case gtString:		return gtReal;
		case gtOctet:		return gtError	|gtEffective;	// 例外が発生するため
		case gtBoolean:		return gtReal;
		case gtObject:		return gtReal	|gtEffective;	// 何が呼ばれるか分からないが強制的に String にキャストされる
		case gtAny:			return gtReal	|gtEffective;

		default:		return gtAny|gtEffective;
		}
	}

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

	bool CastToBoolean          () const { return operator bool(); }

	bool CastToBoolean_Void     () const { return false; /* void は偽 */}
	bool CastToBoolean_Integer  () const { return AsInteger() != 0; }
	bool CastToBoolean_Real     () const { return AsReal() != 0.0; }
	bool CastToBoolean_Null     () const { return false; /* null は偽 */ }
	bool CastToBoolean_String   () const { return !AsString().IsEmpty(); }
	bool CastToBoolean_Octet    () const { return !AsOctet().IsEmpty(); }
	bool CastToBoolean_Boolean  () const { return Type == BooleanTrue; }
	bool CastToBoolean_Object   () const { return Invoke_Object(mnBoolean).CastToBoolean(); }

	static int GuessTypeCastToBoolean(tGuessType l)
	{
		switch(l)
		{
		case gtVoid:		return gtBoolean;
		case gtInteger:		return gtBoolean;
		case gtReal:		return gtBoolean;
		case gtNull:		return gtBoolean;
		case gtString:		return gtBoolean;
		case gtOctet:		return gtBoolean;
		case gtBoolean:		return gtBoolean;
		case gtObject:		return gtBoolean	|gtEffective;	// 何が呼ばれるか分からないが強制的に String にキャストされる
		case gtAny:			return gtBoolean	|gtEffective;

		default:		return gtAny|gtEffective;
		}
	}

	//-----------------------------------------------------------------------
	//! @brief		文字列に変換
	//! @return		文字列
	//-----------------------------------------------------------------------
	operator tString() const
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
		return tString();
	}

	tString CastToString          () const { return operator tString(); }

	tString CastToString_Void     () const { return tString(); }
	tString CastToString_Integer  () const;
	tString CastToString_Real     () const;
	tString CastToString_Null     () const { ThrowIllegalOperationMethod(mnString); return tString();  }
	tString CastToString_String   () const { return AsString(); }
	tString CastToString_Octet    () const { ThrowIllegalOperationMethod(mnString); return tString();  }
	tString CastToString_Boolean  () const;
	tString CastToString_Object   () const { return Invoke_Object(mnString).CastToString(); }

	static int GuessTypeCastToString(tGuessType l)
	{
		switch(l)
		{
		case gtVoid:		return gtString;
		case gtInteger:		return gtString;
		case gtReal:		return gtString;
		case gtNull:		return gtString;
		case gtString:		return gtString;
		case gtOctet:		return gtError		|gtEffective;	// 例外が発生するため
		case gtBoolean:		return gtString;
		case gtObject:		return gtString		|gtEffective;	// 何が呼ばれるか分からないが強制的に String にキャストされる
		case gtAny:			return gtString		|gtEffective;

		default:		return gtAny|gtEffective;
		}
	}

	//-----------------------------------------------------------------------
	//! @brief		オクテット列に変換
	//! @return		オクテット列
	//-----------------------------------------------------------------------
	operator tOctet() const
	{
		switch(GetType())
		{
		case vtVoid:	return CastToOctet_Void     ();
		case vtInteger:	return CastToOctet_Integer  ();
		case vtReal:	return CastToOctet_Real     ();
		case vtNull:	return CastToOctet_Null     ();
		case vtString:	return CastToOctet_String   ();
		case vtOctet:	return CastToOctet_Octet    ();
		case vtBoolean:	return CastToOctet_Boolean  ();
		case vtObject:	return CastToOctet_Object   ();
		}
		return tOctet();
	}

	tOctet CastToOctet          () const { return operator tOctet(); }

	tOctet CastToOctet_Void     () const { return tOctet(); }
	tOctet CastToOctet_Integer  () const { ThrowIllegalOperationMethod(mnOctet); return tOctet(); }
	tOctet CastToOctet_Real     () const { ThrowIllegalOperationMethod(mnOctet); return tOctet(); }
	tOctet CastToOctet_Null     () const { ThrowIllegalOperationMethod(mnOctet); return tOctet(); }
	tOctet CastToOctet_String   () const;
	tOctet CastToOctet_Octet    () const { return AsOctet();  }
	tOctet CastToOctet_Boolean  () const { ThrowIllegalOperationMethod(mnOctet); return tOctet(); }
	tOctet CastToOctet_Object   () const { return Invoke_Object(mnOctet).CastToOctet(); }

	static int GuessTypeCastToOctet(tGuessType l)
	{
		switch(l)
		{
		case gtVoid:		return gtOctet;
		case gtInteger:		return gtError		|gtEffective;	// 例外が発生するため
		case gtReal:		return gtError		|gtEffective;	// 例外が発生するため
		case gtNull:		return gtError		|gtEffective;	// 例外が発生するため
		case gtString:		return gtOctet;
		case gtOctet:		return gtOctet;
		case gtBoolean:		return gtError		|gtEffective;	// 例外が発生するため
		case gtObject:		return gtOctet		|gtEffective;	// 何が呼ばれるか分からないが強制的に Octet にキャストされる
		case gtAny:			return gtOctet		|gtEffective;

		default:		return gtAny|gtEffective;
		}
	}

public: // スレッド同期

	//! @brief		オブジェクトをロックする為のクラス
	class tSynchronizer
	{
		tSynchronizer(const tSynchronizer &); //!< コピー不可です
		void operator = (const tSynchronizer &); //!< コピー不可です
		void * operator new(size_t); //!< ヒープ上に置かないでください
		void * operator new [] (size_t); //!< ヒープ上に置かないでください
		char Synchronizer[sizeof(void*)+sizeof(void*)]; //!< tObjectInterface::tSynchronizer を作成する先
	public:
		//! @brief		コンストラクタ
		tSynchronizer(const tVariant & object);

		//! @brief		デストラクタ
		~tSynchronizer();
	};

public: // ハッシュ/ヒント

	//-----------------------------------------------------------------------
	//! @brief		ヒントを得る
	//! @return		ヒント(0=ヒント無効)
	//-----------------------------------------------------------------------
	risse_uint32 GetHint() const
	{
		switch(GetType())
		{
		case vtVoid:	return GetHint_Void     ();
		case vtInteger:	return GetHint_Integer  ();
		case vtReal:	return GetHint_Real     ();
		case vtNull:	return GetHint_Null     ();
		case vtString:	return GetHint_String   ();
		case vtOctet:	return GetHint_Octet    ();
		case vtBoolean:	return GetHint_Boolean  ();
		case vtObject:	return GetHint_Object   ();
		}
		return 0;
	}

	risse_uint32 GetHint_Void     () const { return GetHash_Void(); }
	risse_uint32 GetHint_Integer  () const { return GetHash_Integer(); }
	risse_uint32 GetHint_Real     () const { return GetHash_Real(); }
	risse_uint32 GetHint_Null     () const { return GetHash_Null(); }
	risse_uint32 GetHint_String   () const { return AsString().GetHint(); }
	risse_uint32 GetHint_Octet    () const { return AsOctet().GetHint(); }
	risse_uint32 GetHint_Boolean  () const { return GetHash_Boolean(); }
	risse_uint32 GetHint_Object   () const;

	//-----------------------------------------------------------------------
	//! @brief		ヒントを設定する
	//! @param		hint		ヒント
	//-----------------------------------------------------------------------
	void SetHint(risse_uint32 hint) const
	{
		switch(GetType())
		{
		case vtVoid:	SetHint_Void     (hint); return;
		case vtInteger:	SetHint_Integer  (hint); return;
		case vtReal:	SetHint_Real     (hint); return;
		case vtNull:	SetHint_Null     (hint); return;
		case vtString:	SetHint_String   (hint); return;
		case vtOctet:	SetHint_Octet    (hint); return;
		case vtBoolean:	SetHint_Boolean  (hint); return;
		case vtObject:	SetHint_Object   (hint); return;
		}
	}

	void SetHint_Void     (risse_uint32 hint) const { (void)hint; return; /* ヒントを格納する場所がない */ }
	void SetHint_Integer  (risse_uint32 hint) const { (void)hint; return; /* ヒントを格納する場所がない */ }
	void SetHint_Real     (risse_uint32 hint) const { (void)hint; return; /* ヒントを格納する場所がない */ }
	void SetHint_Null     (risse_uint32 hint) const { (void)hint; return; /* ヒントを格納する場所がない */ }
	void SetHint_String   (risse_uint32 hint) const { AsString().SetHint(hint); }
	void SetHint_Octet    (risse_uint32 hint) const { AsOctet().SetHint(hint); }
	void SetHint_Boolean  (risse_uint32 hint) const { (void)hint; return; /* ヒントを格納する場所がない */ }
	void SetHint_Object   (risse_uint32 hint) const;

	//-----------------------------------------------------------------------
	//! @brief		ハッシュを得る
	//! @return		ハッシュ値
	//-----------------------------------------------------------------------
	risse_uint32 GetHash() const
	{
		switch(GetType())
		{
		case vtVoid:	return GetHash_Void     ();
		case vtInteger:	return GetHash_Integer  ();
		case vtReal:	return GetHash_Real     ();
		case vtNull:	return GetHash_Null     ();
		case vtString:	return GetHash_String   ();
		case vtOctet:	return GetHash_Octet    ();
		case vtBoolean:	return GetHash_Boolean  ();
		case vtObject:	return GetHash_Object   ();
		}
		return ~static_cast<risse_uint32>(0);
	}

	/* ここで返されている 0x8f84b331のような マジックナンバーに得に意味はない。
	   たんに、他と重ならなさそうな値が適当に選ばれているだけである。 */

	risse_uint32 GetHash_Void     () const { return 0x8f84b331; /* 唯一のインスタンス */ }
	risse_uint32 GetHash_Integer  () const;
	risse_uint32 GetHash_Real     () const;
	risse_uint32 GetHash_Null     () const { return 0x9b371c72; /* 唯一のインスタンス */ }
	risse_uint32 GetHash_String   () const
					{ return AsString().GetHash(); }
	risse_uint32 GetHash_Octet    () const
					{ return AsOctet().GetHash(); }
	risse_uint32 GetHash_Boolean  () const
					{ return CastToBoolean_Boolean()?0xab2ed843:0xbd8b88c4; }
	risse_uint32 GetHash_Object   () const;

public: // ユーティリティ
	//-----------------------------------------------------------------------
	//! @brief		人間が可読な形式に変換
	//! @param		maxlen		出力最大コードポイント数(目安)<br>
	//!							あくまで目安。無視されたり、ぴったりのコード
	//!							ポイント数にならなかったりする。risse_size_max
	//!							を指定すると制限なし
	//! @return		人間が可読な文字列
	//-----------------------------------------------------------------------
	tString AsHumanReadable(risse_size maxlen = risse_size_max) const
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
		return tString();
	}

	tString AsHumanReadable_Void     (risse_size maxlen) const;
	tString AsHumanReadable_Integer  (risse_size maxlen) const
					{ return CastToString_Integer(); }
	tString AsHumanReadable_Real     (risse_size maxlen) const
					{ return CastToString_Real(); }
	tString AsHumanReadable_Null     (risse_size maxlen) const;
	tString AsHumanReadable_String   (risse_size maxlen) const
					{ return AsString().AsHumanReadable(maxlen); }
	tString AsHumanReadable_Octet    (risse_size maxlen) const
					{ return AsOctet().AsHumanReadable(maxlen); }
	tString AsHumanReadable_Boolean  (risse_size maxlen) const
					{ return CastToString_Boolean(); }
	tString AsHumanReadable_Object   (risse_size maxlen) const
					{ return tString(); /* incomplete */ }


	//! @brief		Object型に対するtypeチェック(コンテキストチェック用)
	//! @param		cls		クラスオブジェクトインスタンス
	//! @note		AssertAndGetObjectInterafce() に似るが、チェックに失敗した場合は
	//!				例外が発生せずに NULL が帰る。
	template <typename ObjectT>
	ObjectT * CheckAndGetObjectInterafce(typename ObjectT::tClassBase * cls) const
	{
		if(GetType() != vtObject) return NULL;
		ObjectT * intf = static_cast<ObjectT*>(GetObjectInterface());
		if(!cls->GetRTTIMatcher().Match(intf->GetRTTI()))
			return NULL;
		return intf;
	}

	//! @brief		Object型に対するtypeチェック(コンテキストチェック用)
	//! @param		cls		クラスオブジェクトインスタンス
	//! @note		バリアントが期待したタイプであるかどうかをチェックし
	//!				またそのオブジェクトインターフェースを得る。
	//!				期待した値でなければ「"期待したクラスではありません"」例外を発生する。
	//!				テンプレートパラメータのObjectTはtObjectBaseの派生クラス、
	//!				clsにはtClassBaseの派生クラスのインスタンスを指定すること。
	//!				ObjectT::tClassBase というのは tObjectBase の定義を見れば分かるが
	//!				単に tClassBase の typedef で、このテンプレート内には tClassBase の
	//!				完全な定義がないと解釈できない部分があるが、その解釈を実際の
	//!				実体化の時まで遅らせるための処置。
	//!				ちなみに InstanceOf はモジュールもチェックするが、
	//!				これはモジュールまではチェックしないというかモジュールはチェックのしようがない。
	template <typename ObjectT>
	ObjectT * AssertAndGetObjectInterafce(typename ObjectT::tClassBase * cls) const
	{
		if(GetType() != vtObject) ThrowBadContextException();
		ObjectT * intf = static_cast<ObjectT*>(GetObjectInterface());
		if(!cls->GetRTTIMatcher().Match(intf->GetRTTI()))
			ThrowBadContextException();
		return intf;
	}

	//! @brief		インスタンスチェック(指定されたクラスのインスタンスかどうかをチェック)
	//! @param		cls		クラスオブジェクトインスタンス
	//! @note		指定されたクラスのインスタンスでないばあいは IllegalArgumentClassException 例外が発生する)
	void AssertClass(tClassBase * cls) const;

	//! @brief		Object型に対するtypeチェック(引数チェック用)
	//! @param		cls		クラスオブジェクトインスタンス
	//! @note		AssertAndGetObjectInterafce に似ているが、型が違ったときの
	//!				発生する例外が違う。
	template <typename ObjectT>
	ObjectT * ExpectAndGetObjectInterafce(tClassBase * cls) const
	{
		AssertClass(cls);
		return static_cast<ObjectT*>(GetObjectInterface());
	}

	//! @brief		型名を得る
	//! @param		got		型名が正常に取得できた場合に 真、
	//!						型名が不明だったり取得に失敗した場合に 偽が書き込まれる。
	//!						この情報が必要ない場合は NULL を指定する。
	//! @return		型名(文字列)   型名が不明な場合は <unknown>, 匿名クラスの場合は <anomymous> が帰る
	//! @note		この値の型名を得る。たとえば整数ならば "Integer" など
	tString GetClassName(bool * got = NULL) const;

	//! @brief		トレースを追加する
	//! @param		sb			スクリプトブロック
	//! @param		pos			スクリプト上の位置
	//! @note		この variant に Throwable クラスのインスタンスが入っているとみなし、
	//!				指定されたトレースを追加する。sb が null の場合は
	//!				トレースは追加されない。
	void AddTrace(const tScriptBlockInstance * sb, risse_size pos) const;

	//! @brief		メッセージにプレフィックスを追加する
	//! @param		sb			スクリプトブロック
	//! @param		pos			スクリプト上の位置
	//! @note		この variant に Throwable クラスのインスタンスが入っているとみなし、
	//!				message プロパティの前に指定されたメッセージを追加する。
	void PrependMessage(const tString & message) const;

	//! @brief		デバッグ用ダンプ(標準出力に出力する)
	void DebugDump() const;

	//! @brief		デバッグ用各種構造体サイズ表示
	void prtsizes() const
	{
		fprintf(stderr, "tVariantBlock: %d\n", sizeof(tVariantBlock));
		fprintf(stderr, "Storage: %d\n", sizeof(Storage));
		fprintf(stderr, "risse_ptruint: %d\n", sizeof(risse_ptruint));
		fprintf(stderr, "tString: %d\n", sizeof(tString));
		fprintf(stderr, "tOctet: %d\n", sizeof(tOctet));
		fprintf(stderr, "tObject: %d\n", sizeof(tObject));
		fprintf(stderr, "tVoid: %d\n", sizeof(tVoid));
		fprintf(stderr, "tInteger: %d\n", sizeof(tInteger));
		fprintf(stderr, "tReal: %d\n", sizeof(tReal));
		fprintf(stderr, "tBoolean: %d\n", sizeof(tBoolean));
		fprintf(stderr, "tNull: %d\n", sizeof(tNull));
	}

};
//---------------------------------------------------------------------------
typedef tVariantBlock tVariant;
//---------------------------------------------------------------------------



} // namespace Risse
#endif

