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
	//! @param		object		オブジェクト
	static void Throw(const tRisseString & name);
};
//---------------------------------------------------------------------------







} // namespace Risse
//---------------------------------------------------------------------------


#endif // #ifndef risseExceptionH




