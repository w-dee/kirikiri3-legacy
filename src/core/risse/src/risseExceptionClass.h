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
//! @brief			RisseのReturnExceptionやBreakExceptionなどのインスタンス(暫定実装)
//---------------------------------------------------------------------------
class tRisseExitTryExceptionClass : public tRisseObjectInterface
{
	const void * Identifier; //!< tryブロックを識別するための情報
	risse_uint32 BranchTargetIndex; //!< 分岐ターゲットのインデックス
	const tRisseVariant * Value; //!< 例外とともに投げられる情報 (もしあれば)

public:
	//! @brief		コンストラクタ
	//! @param		identifier		tryブロックを識別するための情報
	//! @param		targ_idx		分岐ターゲットのインデックス
	//! @param		value			例外とともに投げられる情報 (無ければ NULL)
	tRisseExitTryExceptionClass(const void * id, risse_uint32 targ_idx,
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
class tRisseSourcePointClass : public tRisseClassBase, public tRisseClassSingleton<tRisseSourcePointClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseSourcePointClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"Throwable" クラス
//---------------------------------------------------------------------------
class tRisseThrowableClass : public tRisseClassBase, public tRisseClassSingleton<tRisseThrowableClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseThrowableClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"Error" クラス
//---------------------------------------------------------------------------
class tRisseErrorClass : public tRisseClassBase, public tRisseClassSingleton<tRisseErrorClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseErrorClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"BlockExitException" クラス
//---------------------------------------------------------------------------
class tRisseBlockExitExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseBlockExitExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseBlockExitExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"Exception" クラス
//---------------------------------------------------------------------------
class tRisseExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"IOException" クラス
//---------------------------------------------------------------------------
class tRisseIOExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseIOExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseIOExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		"CharConversionException" クラス
//---------------------------------------------------------------------------
class tRisseCharConversionExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseCharConversionExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseCharConversionExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		「無効なUTF-8文字列です」例外を発生
	static void ThrowInvalidUTF8String();
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		"RuntimeException" クラス
//---------------------------------------------------------------------------
class tRisseRuntimeExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseRuntimeExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseRuntimeExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		"ArgumentException" クラス
//---------------------------------------------------------------------------
class tRisseArgumentExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseArgumentExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseArgumentExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"IllegalArgumentException" クラス
//---------------------------------------------------------------------------
class tRisseIllegalArgumentExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseIllegalArgumentExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseIllegalArgumentExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"BadArgumentCountException" クラス
//---------------------------------------------------------------------------
class tRisseBadArgumentCountExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseBadArgumentCountExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseBadArgumentCountExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		普通の引数の数が期待した数でなかった場合の例外を発生
	//! @param		passed		渡された引数の数
	//! @param		expected	期待した数
	static void ThrowNormal(risse_size passed, risse_size expected);

	//! @brief		ブロック引数の数が期待した数でなかった場合の例外を発生
	//! @param		passed		渡された引数の数
	//! @param		expected	期待した数
	static void ThrowBlock(risse_size passed, risse_size expected);
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		"MemberAccessException" クラス
//---------------------------------------------------------------------------
class tRisseMemberAccessExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseMemberAccessExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseMemberAccessExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		"NoSuchMemberException" クラス
//---------------------------------------------------------------------------
class tRisseNoSuchMemberExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseNoSuchMemberExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseNoSuchMemberExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:
	//! @brief		例外を発生
	//! @param		name		メンバ名
	static void Throw(const tRisseString & name);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"IllegalMemberAccessException" クラス
//---------------------------------------------------------------------------
class tRisseIllegalMemberAccessExceptionClass : public tRisseClassBase, public tRisseClassSingleton<tRisseIllegalMemberAccessExceptionClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseIllegalMemberAccessExceptionClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

public:

	//! @brief		「読み出し専用メンバに上書きしようとした」例外を発生
	//! @param		name		メンバ名
	static void ThrowMemberIsReadOnly(const tRisseString & name);

	//! @brief		「このプロパティからは読み込むことができない」例外を発生
	//! @param		name		メンバ名
	static void ThrowPropertyCannotBeRead(const tRisseString & name);

	//! @brief		「このプロパティには書き込むことができない」例外を発生
	//! @param		name		メンバ名
	static void ThrowPropertyCannotBeWritten(const tRisseString & name);
};
//---------------------------------------------------------------------------







} // namespace Risse
//---------------------------------------------------------------------------


#endif // #ifndef risseExceptionH




