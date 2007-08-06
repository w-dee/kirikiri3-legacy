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
//!					tTemporaryException::ThrowConverted() メソッドで
//!					本来の例外に変換し、再び投げることができる。
//---------------------------------------------------------------------------
class tTemporaryException : public tCollectee
{
	tString ExceptionClassName; //!< 例外クラス名
	gc_vector<tVariant> Arguments; //!< 例外クラスのコンストラクタに渡すための引数
public:
	//! @brief		コンストラクタ
	//! @param		classname		クラス名
	tTemporaryException(const tString classname);

	//! @brief		コンストラクタ
	//! @param		classname		クラス名
	//! @param		arg1			引数1
	tTemporaryException(const tString classname, const tVariant & arg1);

	//! @brief		コンストラクタ
	//! @param		classname		クラス名
	//! @param		arg1			引数1
	//! @param		arg2			引数2
	tTemporaryException(const tString classname, const tVariant & arg1,
								const tVariant & arg2);

	//! @brief		コンストラクタ
	//! @param		classname		クラス名
	//! @param		arg1			引数1
	//! @param		arg2			引数2
	//! @param		arg3			引数3
	tTemporaryException(const tString classname, const tVariant & arg1,
								const tVariant & arg2, const tVariant & arg3);


	//! @brief		例外を本来の例外に変換する
	//! @param		engine		スクリプトエンジン
	//! @return		変換された例外オブジェクト
	tVariant * Convert(tScriptEngine * engine) const;

	//! @brief		例外を本来の例外に変換し、再び投げる
	//! @param		engine		スクリプトエンジン
	void ThrowConverted(tScriptEngine * engine) const { throw Convert(engine); }

	//! @brief		内容を標準エラー出力にダンプする
	void Dump() const;
};
//---------------------------------------------------------------------------








class tScriptBlockInstance;
//---------------------------------------------------------------------------
//! @brief			RisseのReturnExceptionやBreakExceptionなどのインスタンス(暫定実装)
//---------------------------------------------------------------------------
class tExitTryExceptionClass : public tObjectInterface
{
	const void * Identifier; //!< tryブロックを識別するための情報
	risse_uint32 BranchTargetIndex; //!< 分岐ターゲットのインデックス
	const tVariant * Value; //!< 例外とともに投げられる情報 (もしあれば)

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		id				tryブロックを識別するための情報
	//! @param		targ_idx		分岐ターゲットのインデックス
	//! @param		value			例外とともに投げられる情報 (無ければ NULL)
	tExitTryExceptionClass(tScriptEngine * engine,
					const void * id, risse_uint32 targ_idx,
						const tVariant * value = NULL);

	// 暫定実装 TODO:ちゃんと実装
	const void * GetIdentifier() const { return Identifier; }
	risse_uint32 GetBranchTargetIndex() const { return BranchTargetIndex; }
	const tVariant * GetValue() const { return Value; }


public: // tObjectInterface メンバ

	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);


};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		"SourcePoint" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tSourcePointInstance : public tObjectBase
{
public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo & info);
	tString toString();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"SourcePoint" クラス
//! @note		ソースコード上の位置を表すクラス@n
//!				注意: このクラスの実装は将来的に変更される可能性がある。
//!				このクラスのインスタンスを自前で作成したり、インスタンスのメンバに
//!				書き込んだりしないこと。
//---------------------------------------------------------------------------
class tSourcePointClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tSourcePointClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		"Throwable" クラス
//! @note		このクラスでは専用のインスタンス用 C++クラスを用いない
//!				(tObjectBase を直接使う)。そのため BindFunction に渡す
//!				C++メソッドはすべてstatic宣言になり、あまりNativeBinderの恩恵が
//!				無いが、しかたない。
//---------------------------------------------------------------------------
class tThrowableClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tThrowableClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct(const tNativeCallInfo & info);
	static void initialize(const tNativeCallInfo & info);
	static tString toString(const tNativeCallInfo & info);
	static void addTrace(const tVariant & point,
		const tNativeCallInfo & info);
	static tVariant toException(const tNativeCallInfo & info);
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		"Error" クラス
//---------------------------------------------------------------------------
class tErrorClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tErrorClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);
};
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//! @brief		"AssertionError" クラス
//---------------------------------------------------------------------------
class tAssertionErrorClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tAssertionErrorClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct(const tNativeCallInfo & info);
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		例外を投げる
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		expression		ASSERTに失敗した式
	static void Throw(tScriptEngine * engine, const tString & expression);

	//! @brief		例外を投げる
	//! @param		expression		ASSERTに失敗した式
	static void Throw(const tString & expression) { Throw(NULL, expression); }
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		"BlockExitException" クラス
//---------------------------------------------------------------------------
class tBlockExitExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tBlockExitExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct(const tNativeCallInfo & info);
	static void initialize(const tNativeCallInfo & info);
};
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//! @brief		"Exception" クラス
//---------------------------------------------------------------------------
class tExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);
};
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
//! @brief		"InsufficientResourceException" クラス
//---------------------------------------------------------------------------
class tInsufficientResourceExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tInsufficientResourceExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「コルーチンを作成できない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCouldNotCreateCoroutine(tScriptEngine * engine);
	//! @brief		「コルーチンを作成できない」例外を発生
	static void ThrowCouldNotCreateCoroutine() { ThrowCouldNotCreateCoroutine(NULL); }
};
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//! @brief		"IOException" クラス
//---------------------------------------------------------------------------
class tIOExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tIOExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		msg			例外メッセージ
	static void Throw(tScriptEngine * engine, const tString & message);
	//! @brief		「読み込みエラーが発生した」例外を発生
	static void Throw(const tString & message)
		{ Throw(NULL, message); }

	//! @brief		「読み込みエラーが発生した」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		ストリームなどの名前
	static void ThrowReadError(tScriptEngine * engine,
		const tString & name = tString::GetEmptyString());
	//! @brief		「読み込みエラーが発生した」例外を発生
	static void ThrowReadError(const tString & name =
		tString::GetEmptyString())
		{ ThrowReadError(NULL, name); }

	//! @brief		「書き込みエラーが発生した」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		ストリームなどの名前
	static void ThrowWriteError(tScriptEngine * engine,
		const tString & name = tString::GetEmptyString());
	//! @brief		「書き込みエラーが発生した」例外を発生
	static void ThrowWriteError(const tString & name =
		tString::GetEmptyString()) { ThrowWriteError(NULL, name); }

	//! @brief		「シークエラーが発生した」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		ストリームなどの名前
	//! @param		pos			シークしようとした位置
	static void ThrowSeekError(tScriptEngine * engine,
		const tString & name = tString::GetEmptyString(),
		risse_size pos = risse_size_max);
	//! @brief		「シークエラーが発生した」例外を発生
	static void ThrowSeekError(const tString & name =
		tString::GetEmptyString(), risse_size pos = risse_size_max)
		{ ThrowSeekError(NULL, name, pos); }

	//! @brief		「ファイルの切りつめに失敗した」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		ストリームなどの名前
	//! @param		pos			切りつめようとした位置
	static void ThrowTruncateError(tScriptEngine * engine,
		const tString & name = tString::GetEmptyString(),
		risse_size pos = risse_size_max);
	//! @brief		「ファイルの切りつめに失敗した」例外を発生
	static void ThrowTruncateError(const tString & name =
		tString::GetEmptyString(), risse_size pos = risse_size_max)
		{ ThrowTruncateError(NULL, name, pos); }

	//! @brief		「ストリームは閉じられている」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		ストリームなどの名前
	static void ThrowStreamIsClosed(tScriptEngine * engine,
		const tString & name = tString::GetEmptyString());
	//! @brief		「ストリームは閉じられている」例外を発生
	static void ThrowStreamIsClosed(const tString & name =
		tString::GetEmptyString())
		{ ThrowStreamIsClosed(NULL, name); }

};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		"CharConversionException" クラス
//---------------------------------------------------------------------------
class tCharConversionExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tCharConversionExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「無効なUTF-8文字列です」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowInvalidUTF8String(tScriptEngine * engine);
	//! @brief		「無効なUTF-8文字列です」例外を発生
	static void ThrowInvalidUTF8String() { ThrowInvalidUTF8String(NULL); }
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		"RuntimeException" クラス
//---------------------------------------------------------------------------
class tRuntimeExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRuntimeExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		"CompileException" クラス
//---------------------------------------------------------------------------
class tCompileExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tCompileExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「コンパイルエラー」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		reason		例外の理由
	//! @param		sb			スクリプトブロック
	//! @param		pos			スクリプト上の位置
	static void Throw(tScriptEngine * engine, const tString & reason,
		const tScriptBlockInstance * sb = NULL, risse_size pos = risse_size_max);
	//! @brief		「コンパイルエラー」例外を発生
	//! @param		reason		例外の理由
	static void Throw(const tString & reason)
		{ Throw(NULL, reason); }
};
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//! @brief		"ClassDefinitionException" クラス
//---------------------------------------------------------------------------
class tClassDefinitionExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tClassDefinitionExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「extensibleでないクラスのサブクラスを作成しようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCannotCreateSubClassOfNonExtensibleClass(tScriptEngine * engine);
	//! @brief		「extensibleでないクラスのサブクラスを作成しようとした」例外を発生
	static void ThrowCannotCreateSubClassOfNonExtensibleClass()
		{ ThrowCannotCreateSubClassOfNonExtensibleClass(NULL); }

	//! @brief		「スーパークラスはクラスではない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowSuperClassIsNotAClass(tScriptEngine * engine);
	//! @brief		「スーパークラスはクラスではない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowSuperClassIsNotAClass()
		{ ThrowSuperClassIsNotAClass(NULL); }
};
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
//! @brief		"InstantiationException" クラス
//---------------------------------------------------------------------------
class tInstantiationExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tInstantiationExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「クラスでない物からインスタンスを生成しようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCannotCreateInstanceFromNonClassObject(tScriptEngine * engine);
	//! @brief		「クラスでない物からインスタンスを生成しようとした」例外を発生
	static void ThrowCannotCreateInstanceFromNonClassObject()
		{ ThrowCannotCreateInstanceFromNonClassObject(NULL); }

	//! @brief		「このクラスからインスタンスは作成できません」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCannotCreateInstanceFromThisClass(tScriptEngine * engine);
	//! @brief		「このクラスからインスタンスは作成できません」例外を発生
	static void ThrowCannotCreateInstanceFromThisClass()
		{ ThrowCannotCreateInstanceFromThisClass(NULL); }
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		"BadContextException" クラス
//---------------------------------------------------------------------------
class tBadContextExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tBadContextExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「異なるクラスのコンテキストです」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void Throw(tScriptEngine * engine);
	//! @brief		「異なるクラスのコンテキストです」例外を発生
	static void Throw() { Throw(NULL); }
};
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
//! @brief		"UnsupportedOperationException" クラス
//---------------------------------------------------------------------------
class tUnsupportedOperationExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tUnsupportedOperationExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「関数でない物を呼び出そうとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCannotCallNonFunctionObjectException(tScriptEngine * engine);
	//! @brief		「関数でない物を呼び出そうとした」例外を発生
	static void ThrowCannotCallNonFunctionObjectException()
		{ ThrowCannotCallNonFunctionObjectException(NULL); }

	//! @brief		「機能が実装されていません」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowOperationIsNotImplemented(tScriptEngine * engine);
	//! @brief		「機能が実装されていません」例外を発生
	static void ThrowOperationIsNotImplemented()
		{ ThrowOperationIsNotImplemented(NULL); }
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		"ArithmeticException" クラス
//---------------------------------------------------------------------------
class tArithmeticExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tArithmeticExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「0で除算をしようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowDivideByZeroException(tScriptEngine * engine);
	//! @brief		「0で除算をしようとした」例外を発生
	static void ThrowDivideByZeroException()
		{ ThrowDivideByZeroException(NULL); }
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		"ArgumentException" クラス
//---------------------------------------------------------------------------
class tArgumentExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tArgumentExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);
};
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
//! @brief		"NullObjectException" クラス
//---------------------------------------------------------------------------
class tNullObjectExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tNullObjectExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「nullオブジェクトにアクセスしようとしました」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void Throw(tScriptEngine * engine);
	//! @brief		「nullオブジェクトにアクセスしようとしました」例外を発生
	static void Throw() { Throw(NULL); }
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		"IllegalArgumentClassException" クラス
//---------------------------------------------------------------------------
class tIllegalArgumentClassExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tIllegalArgumentClassExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「method_nameにクラスclass_nameは受け入れられない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		method_name	メソッド名
	//! @param		type_name	クラス名
	static void ThrowNonAcceptableClass(tScriptEngine * engine,
		const tString & method_name, const tString & class_name);
	//! @brief		「method_nameにクラスclass_nameは受け入れられない」例外を発生
	//! @param		method_name	メソッド名
	//! @param		type_name	型名
	static void ThrowNonAcceptableClass(const tString & method_name, const tString & class_name)
		{ ThrowNonAcceptableClass(NULL, method_name, class_name); }

	//! @brief		「クラスclass_nameのインスタンスを指定すべし」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		class_name	クラス名
	static void ThrowSpecifyInstanceOfClass(tScriptEngine * engine,
		const tString & class_name);
	//! @brief		「クラスclass_nameのインスタンスを指定すべし」例外を発生
	//! @param		method_name	メソッド名
	//! @param		class_name	クラス名
	static void ThrowSpecifyInstanceOfClass(const tString & class_name)
		{ ThrowSpecifyInstanceOfClass(NULL, class_name); }


};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		"IllegalArgumentException" クラス
//---------------------------------------------------------------------------
class tIllegalArgumentExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tIllegalArgumentExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		例外を投げる
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		message		メッセージ
	static void Throw(tScriptEngine * engine, const tString & message);
	//! @brief		例外を投げる
	//! @param		message		メッセージ
	static void Throw(const tString & message)
		{ Throw(NULL, message); }

	//! @brief		「無効な日付文字列」例外を投げる
	//! @param		engine		スクリプトエンジンインスタンス	
	static void ThrowInvalidDateString(tScriptEngine * engine);
	//! @brief		「無効な日付文字列」例外を投げる
	static void ThrowInvalidDateString() { ThrowInvalidDateString(NULL); }
};
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
//! @brief		"BadArgumentCountException" クラス
//---------------------------------------------------------------------------
class tBadArgumentCountExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tBadArgumentCountExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		普通の引数の数が期待した数でなかった場合の例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		passed		渡された引数の数
	//! @param		expected	期待した数
	static void ThrowNormal(tScriptEngine * engine, risse_size passed, risse_size expected);
	//! @brief		普通の引数の数が期待した数でなかった場合の例外を発生
	//! @param		passed		渡された引数の数
	//! @param		expected	期待した数
	static void ThrowNormal(risse_size passed, risse_size expected)
		{ ThrowNormal(NULL, passed, expected); }

	//! @brief		ブロック引数の数が期待した数でなかった場合の例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		passed		渡された引数の数
	//! @param		expected	期待した数
	static void ThrowBlock(tScriptEngine * engine, risse_size passed, risse_size expected);
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
class tMemberAccessExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tMemberAccessExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct(const tNativeCallInfo & info);
	static void initialize(const tNativeCallInfo & info);
};
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//! @brief		"NoSuchMemberException" クラス
//---------------------------------------------------------------------------
class tNoSuchMemberExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tNoSuchMemberExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void Throw(tScriptEngine * engine, const tString & name);
	//! @brief		例外を発生
	//! @param		name		メンバ名
	static void Throw(const tString & name) { Throw(NULL, name); }
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		"IllegalMemberAccessException" クラス
//---------------------------------------------------------------------------
class tIllegalMemberAccessExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tIllegalMemberAccessExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「読み出し専用メンバに上書きしようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowMemberIsReadOnly(tScriptEngine * engine, const tString & name);
	//! @brief		「読み出し専用メンバに上書きしようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowMemberIsReadOnly(const tString & name)
		{ ThrowMemberIsReadOnly(NULL, name); }

	//! @brief		「finalメンバをオーバーライドしようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowMemberIsFinal(tScriptEngine * engine, const tString & name);
	//! @brief		「finalメンバをオーバーライドしようとした」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowMemberIsFinal(const tString & name)
		{ ThrowMemberIsFinal(NULL, name); }

	//! @brief		「このプロパティからは読み込むことができない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowPropertyCannotBeRead(tScriptEngine * engine, const tString & name);
	//! @brief		「このプロパティからは読み込むことができない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowPropertyCannotBeRead(const tString & name)
		{ ThrowPropertyCannotBeRead(NULL, name); }

	//! @brief		「このプロパティには書き込むことができない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowPropertyCannotBeWritten(tScriptEngine * engine, const tString & name);
	//! @brief		「このプロパティには書き込むことができない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		メンバ名
	static void ThrowPropertyCannotBeWritten(const tString & name)
		{ ThrowPropertyCannotBeWritten(NULL, name); }
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		"CoroutineException" クラス
//---------------------------------------------------------------------------
class tCoroutineExceptionClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tCoroutineExceptionClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public: // Risse用メソッドなど
	static void construct();
	static void initialize(const tNativeCallInfo & info);

public:
	//! @brief		「コルーチンは既に終了している」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineHasAlreadyExited(tScriptEngine * engine);
	//! @brief		「コルーチンは既に終了している」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineHasAlreadyExited()
		{ ThrowCoroutineHasAlreadyExited(NULL); }

	//! @brief		「コルーチンはまだ開始していない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineHasNotStartedYet(tScriptEngine * engine);
	//! @brief		「コルーチンはまだ開始していない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineHasNotStartedYet()
		{ ThrowCoroutineHasNotStartedYet(NULL); }

	//! @brief		「コルーチンは実行中でない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineIsNotRunning(tScriptEngine * engine);
	//! @brief		「コルーチンは実行中でない」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineIsNotRunning()
		{ ThrowCoroutineIsNotRunning(NULL); }

	//! @brief		「コルーチンは実行中」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineIsRunning(tScriptEngine * engine);
	//! @brief		「コルーチンは実行中」例外を発生
	//! @param		engine		スクリプトエンジンインスタンス
	static void ThrowCoroutineIsRunning()
		{ ThrowCoroutineIsRunning(NULL); }

};
//---------------------------------------------------------------------------






} // namespace Risse
//---------------------------------------------------------------------------


#endif // #ifndef risseExceptionH




