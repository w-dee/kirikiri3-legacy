//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse 例外クラス
//---------------------------------------------------------------------------


#ifndef risseExceptionClassH
#define risseExceptionClassH

#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseString.h"
#include "risseGC.h"
#include "risseObject.h"
#include "risseClass.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief			仮例外用クラス
//! @note			例外クラスはスクリプトエンジンインスタンスに依存しているため、
//!					スクリプトエンジンインスタンスへのポインタが利用できない場合は
//!					この例外を仮になげておく。スクリプトエンジンインスタンスが利用
//!					可能な場所でこの例外を受け取った場合は、
//!					tRisseTemporaryException::ThrowConverted() メソッドで
//!					本来の例外に変換し、再び投げることができる。
//---------------------------------------------------------------------------
class tRisseTemporaryException : public tRisseCollectee
{
	tRisseString ExceptionClassName; //!< 例外クラス名
	gc_vector<tRisseVariant> Arguments; //!< 例外クラスのコンストラクタに渡すための引数
public:
	//! @brief		コンストラクタ
	//! @param		classname		クラス名
	tRisseTemporaryException(const tRisseString classname);

	//! @brief		コンストラクタ
	//! @param		classname		クラス名
	//! @param		arg1			引数1
	tRisseTemporaryException(const tRisseString classname, const tRisseVariant & arg1);

	//! @brief		コンストラクタ
	//! @param		classname		クラス名
	//! @param		arg1			引数1
	//! @param		arg2			引数2
	tRisseTemporaryException(const tRisseString classname, const tRisseVariant & arg1,
								const tRisseVariant & arg2);

	//! @brief		コンストラクタ
	//! @param		classname		クラス名
	//! @param		arg1			引数1
	//! @param		arg2			引数2
	//! @param		arg3			引数3
	tRisseTemporaryException(const tRisseString classname, const tRisseVariant & arg1,
								const tRisseVariant & arg2, const tRisseVariant & arg3);


	//! @brief		例外を本来の例外に変換する
	//! @param		engine		スクリプトエンジン
	//! @return		変換された例外オブジェクト
	tRisseVariant * Convert(tRisseScriptEngine * engine) const;

	//! @brief		例外を本来の例外に変換し、再び投げる
	//! @param		engine		スクリプトエンジン
	void ThrowConverted(tRisseScriptEngine * engine) const { throw Convert(engine); }
};
//---------------------------------------------------------------------------


class tRisseScriptBlockBase;
//---------------------------------------------------------------------------
//! @brief			RisseのReturnExceptionやBreakExceptionなどのインスタンス(暫定実装)
//---------------------------------------------------------------------------
class tRisseExitTryExceptionClass : public tRisseObjectInterface
{
	const void * Identifier; //!< tryブロックを識別するための情報
	risse_uint32 BranchTargetIndex; //!< 分岐ターゲットのインデックス
	const tRisseVariant * Value; //!< 例外とともに投げられる情報 (もしあれば)

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		id				tryブロックを識別するための情報
	//! @param		targ_idx		分岐ターゲットのインデックス
	//! @param		value			例外とともに投げられる情報 (無ければ NULL)
	tRisseExitTryExceptionClass(tRisseScriptEngine * engine,
					const void * id, risse_uint32 targ_idx,
						const tRisseVariant * value = NULL);

	// 暫定実装 TODO:ちゃんと実装
	const void * GetIdentifier() const { return Identifier; }
	risse_uint32 GetBranchTargetIndex() const { return BranchTargetIndex; }
	const tRisseVariant * GetValue() const { return Value; }


public: // tRisseObjectInterface メンバ

	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);


};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"SourcePoint" クラス
//! @note		ソースコード上の位置を表すクラス@n
//!				注意: このクラスの実装は将来的に変更される可能性がある。
//!				このクラスのインスタンスを自前で作成したり、インスタンスのメンバに
//!				書き込んだりしないこと。
//---------------------------------------------------------------------------
class tRisseSourcePointClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseSourcePointClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"Throwable" クラス
//---------------------------------------------------------------------------
class tRisseThrowableClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseThrowableClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"Error" クラス
//---------------------------------------------------------------------------
class tRisseErrorClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseErrorClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"AssertionError" クラス
//---------------------------------------------------------------------------
class tRisseAssertionErrorClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseAssertionErrorClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		例外を投げる
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		expression		ASSERTに失敗した式
	static void Throw(tRisseScriptEngine * engine, const tRisseString & expression);

	//! @brief		例外を投げる
	//! @param		expression		ASSERTに失敗した式
	static void Throw(const tRisseString & expression) { Throw(NULL, expression); }
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"BlockExitException" クラス
//---------------------------------------------------------------------------
class tRisseBlockExitExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseBlockExitExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"Exception" クラス
//---------------------------------------------------------------------------
class tRisseExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"InsufficientResourceException" クラス
//---------------------------------------------------------------------------
class tRisseInsufficientResourceExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseInsufficientResourceExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		「コルーチンを作成できない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCouldNotCreateCoroutine(tRisseScriptEngine * engine);
	//! @brief		「コルーチンを作成できない」例外を発生
	static void ThrowCouldNotCreateCoroutine() { ThrowCouldNotCreateCoroutine(NULL); }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"IOException" クラス
//---------------------------------------------------------------------------
class tRisseIOExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseIOExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		"CharConversionException" クラス
//---------------------------------------------------------------------------
class tRisseCharConversionExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseCharConversionExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		「無効なUTF-8文字列です」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowInvalidUTF8String(tRisseScriptEngine * engine);
	//! @brief		「無効なUTF-8文字列です」例外を発生
	static void ThrowInvalidUTF8String() { ThrowInvalidUTF8String(NULL); }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"RuntimeException" クラス
//---------------------------------------------------------------------------
class tRisseRuntimeExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseRuntimeExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"CompileException" クラス
//---------------------------------------------------------------------------
class tRisseCompileExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseCompileExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		「コンパイルエラー」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		reason		例外の理由
	//! @param		sb			スクリプトブロック
	//! @param		pos			スクリプト上の位置
	static void Throw(tRisseScriptEngine * engine, const tRisseString & reason,
		const tRisseScriptBlockBase * sb = NULL, risse_size pos = risse_size_max);
	//! @brief		「コンパイルエラー」例外を発生
	//! @param		reason		例外の理由
	static void Throw(const tRisseString & reason)
		{ Throw(NULL, reason); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"ClassDefinitionException" クラス
//---------------------------------------------------------------------------
class tRisseClassDefinitionExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseClassDefinitionExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		「extensibleでないクラスのサブクラスを作成しようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCannotCreateSubClassOfNonExtensibleClass(tRisseScriptEngine * engine);
	//! @brief		「extensibleでないクラスのサブクラスを作成しようとした」例外を発生
	static void ThrowCannotCreateSubClassOfNonExtensibleClass()
		{ ThrowCannotCreateSubClassOfNonExtensibleClass(NULL); }

	//! @brief		「スーパークラスはクラスではない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowSuperClassIsNotAClass(tRisseScriptEngine * engine);
	//! @brief		「スーパークラスはクラスではない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowSuperClassIsNotAClass()
		{ ThrowSuperClassIsNotAClass(NULL); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"InstantiationException" クラス
//---------------------------------------------------------------------------
class tRisseInstantiationExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseInstantiationExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		「クラスでない物からインスタンスを生成しようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCannotCreateInstanceFromNonClassObject(tRisseScriptEngine * engine);
	//! @brief		「クラスでない物からインスタンスを生成しようとした」例外を発生
	static void ThrowCannotCreateInstanceFromNonClassObject()
		{ ThrowCannotCreateInstanceFromNonClassObject(NULL); }

	//! @brief		「このクラスからインスタンスは作成できません」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCannotCreateInstanceFromThisClass(tRisseScriptEngine * engine);
	//! @brief		「このクラスからインスタンスは作成できません」例外を発生
	static void ThrowCannotCreateInstanceFromThisClass()
		{ ThrowCannotCreateInstanceFromThisClass(NULL); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"BadContextException" クラス
//---------------------------------------------------------------------------
class tRisseBadContextExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseBadContextExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		「異なるクラスのコンテキストです」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void Throw(tRisseScriptEngine * engine);
	//! @brief		「異なるクラスのコンテキストです」例外を発生
	static void Throw() { Throw(NULL); }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"UnsupportedOperationException" クラス
//---------------------------------------------------------------------------
class tRisseUnsupportedOperationExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseUnsupportedOperationExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		「関数でない物を呼び出そうとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCannotCallNonFunctionObjectException(tRisseScriptEngine * engine);
	//! @brief		「関数でない物を呼び出そうとした」例外を発生
	static void ThrowCannotCallNonFunctionObjectException()
		{ ThrowCannotCallNonFunctionObjectException(NULL); }

	//! @brief		「機能が実装されていません」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowOperationIsNotImplemented(tRisseScriptEngine * engine);
	//! @brief		「機能が実装されていません」例外を発生
	static void ThrowOperationIsNotImplemented()
		{ ThrowOperationIsNotImplemented(NULL); }
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		"ArgumentException" クラス
//---------------------------------------------------------------------------
class tRisseArgumentExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseArgumentExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"NullObjectException" クラス
//---------------------------------------------------------------------------
class tRisseNullObjectExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseNullObjectExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		「異なるクラスのコンテキストです」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void Throw(tRisseScriptEngine * engine);
	//! @brief		「異なるクラスのコンテキストです」例外を発生
	static void Throw() { Throw(NULL); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"IllegalArgumentException" クラス
//---------------------------------------------------------------------------
class tRisseIllegalArgumentExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseIllegalArgumentExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"BadArgumentCountException" クラス
//---------------------------------------------------------------------------
class tRisseBadArgumentCountExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseBadArgumentCountExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		普通の引数の数が期待した数でなかった場合の例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		passed		渡された引数の数
	//! @param		expected	期待した数
	static void ThrowNormal(tRisseScriptEngine * engine, risse_size passed, risse_size expected);
	//! @brief		普通の引数の数が期待した数でなかった場合の例外を発生
	//! @param		passed		渡された引数の数
	//! @param		expected	期待した数
	static void ThrowNormal(risse_size passed, risse_size expected)
		{ ThrowNormal(NULL, passed, expected); }

	//! @brief		ブロック引数の数が期待した数でなかった場合の例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		passed		渡された引数の数
	//! @param		expected	期待した数
	static void ThrowBlock(tRisseScriptEngine * engine, risse_size passed, risse_size expected);
	//! @brief		ブロック引数の数が期待した数でなかった場合の例外を発生
	//! @param		passed		渡された引数の数
	//! @param		expected	期待した数
	static void ThrowBlock(risse_size passed, risse_size expected)
		{ ThrowBlock(NULL, passed, expected); }
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		"MemberAccessException" クラス
//---------------------------------------------------------------------------
class tRisseMemberAccessExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseMemberAccessExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		"NoSuchMemberException" クラス
//---------------------------------------------------------------------------
class tRisseNoSuchMemberExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseNoSuchMemberExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void Throw(tRisseScriptEngine * engine, const tRisseString & name);
	//! @brief		例外を発生
	//! @param		name		メンバ名
	static void Throw(const tRisseString & name) { Throw(NULL, name); }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"IllegalMemberAccessException" クラス
//---------------------------------------------------------------------------
class tRisseIllegalMemberAccessExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseIllegalMemberAccessExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:

	//! @brief		「読み出し専用メンバに上書きしようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowMemberIsReadOnly(tRisseScriptEngine * engine, const tRisseString & name);
	//! @brief		「読み出し専用メンバに上書きしようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowMemberIsReadOnly(const tRisseString & name)
		{ ThrowMemberIsReadOnly(NULL, name); }

	//! @brief		「finalメンバをオーバーライドしようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowMemberIsFinal(tRisseScriptEngine * engine, const tRisseString & name);
	//! @brief		「finalメンバをオーバーライドしようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowMemberIsFinal(const tRisseString & name)
		{ ThrowMemberIsFinal(NULL, name); }

	//! @brief		「このプロパティからは読み込むことができない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowPropertyCannotBeRead(tRisseScriptEngine * engine, const tRisseString & name);
	//! @brief		「このプロパティからは読み込むことができない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowPropertyCannotBeRead(const tRisseString & name)
		{ ThrowPropertyCannotBeRead(NULL, name); }

	//! @brief		「このプロパティには書き込むことができない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowPropertyCannotBeWritten(tRisseScriptEngine * engine, const tRisseString & name);
	//! @brief		「このプロパティには書き込むことができない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowPropertyCannotBeWritten(const tRisseString & name)
		{ ThrowPropertyCannotBeWritten(NULL, name); }
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"CoroutineException" クラス
//---------------------------------------------------------------------------
class tRisseCoroutineExceptionClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseCoroutineExceptionClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		「コルーチンは既に終了している」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineHasAlreadyExited(tRisseScriptEngine * engine);
	//! @brief		「コルーチンは既に終了している」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineHasAlreadyExited()
		{ ThrowCoroutineHasAlreadyExited(NULL); }

	//! @brief		「コルーチンはまだ開始していない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineHasNotStartedYet(tRisseScriptEngine * engine);
	//! @brief		「コルーチンはまだ開始していない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineHasNotStartedYet()
		{ ThrowCoroutineHasNotStartedYet(NULL); }

	//! @brief		「コルーチンは実行中でない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineIsNotRunning(tRisseScriptEngine * engine);
	//! @brief		「コルーチンは実行中でない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineIsNotRunning()
		{ ThrowCoroutineIsNotRunning(NULL); }

	//! @brief		「コルーチンは実行中」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineIsRunning(tRisseScriptEngine * engine);
	//! @brief		「コルーチンは実行中」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineIsRunning()
		{ ThrowCoroutineIsRunning(NULL); }

};
//---------------------------------------------------------------------------






} // namespace Risse
//---------------------------------------------------------------------------


#endif // #ifndef risseExceptionH




