//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 実行コンテキスト管理
//---------------------------------------------------------------------------
#ifndef risseExecutorContextH
#define risseExecutorContextH

#include "risseGC.h"
#include "risseCodeBlock.h"
#include "risseObject.h"

//---------------------------------------------------------------------------
namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		呼び出し情報
//---------------------------------------------------------------------------
struct tRisseCallInfo : public tRisseCollectee
{
	tRisseVariant Callee;					//!< 呼び出し先

	tRisseOpCode Code;						//!< オペレーションコード
	tRisseVariant * Result;					//!< 結果の格納先 (NULLの場合は結果が要らない場合)
	tRisseString Name;						//!< 操作を行うメンバ名
											//!< (空文字列の場合はこのオブジェクトそのものに対しての操作)
	risse_uint32 Flags;						//!< オペレーションフラグ
	tRisseMethodArgument Args;				//!< 引数
	tRisseMethodArgument BArgs;				//!< ブロック引数
	const tRisseVariant *This;				//!< メソッドが実行されるべき"Thisオブジェクト"
											//!< (NULL="Thisオブジェクト"を指定しない場合)
	const tRisseMethodClosure *Closure;		//!< メソッドが実行されるべきクロージャ

	//! @brief		コンストラクタ
	//! @param		callee	呼び出し先オブジェクト
	//! @param		code	オペレーションコード
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		name	操作を行うメンバ名
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		bargs	ブロック引数
	//! @param		This_	メソッドが実行されるべき"Thisオブジェクト"
	//! @param		closure	メソッドが実行されるべきクロージャ
	tRisseCallInfo(
		const tRisseVariant & callee,
		tRisseOpCode code,
		tRisseVariant * result = NULL,
		const tRisseString & name = tRisseString::GetEmptyString(),
		risse_uint32 flags = 0,
		const tRisseMethodArgument & args = tRisseMethodArgument::GetEmptyArgument(),
		const tRisseMethodArgument & bargs = tRisseMethodArgument::GetEmptyArgument(),
		const tRisseVariant *This_ = NULL,
		const tRisseMethodClosure * closure = NULL) :
			Callee(callee), Code(code), Result(result), Name(name), Flags(flags), Args(args),
			BArgs(bargs), This(This_), Closure(closure)
	{
		;
	}

	//! @brief		コンストラクタ
	//! @note		デフォルトコンストラクタはメンバを初期化しないので注意
//	tRisseCallInfo() {;}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		実行コンテキストのクラス
//---------------------------------------------------------------------------
class tRisseExecutorContext : public tRisseCollectee
{
public:
	//! @brief		スタックブロック(スタックの一つの環境を表す)
	struct tBlock
	{
		void * State; //!< Executor の状態
		tRisseCallInfo Info; //!< 呼び出し情報
		//! @brief		コンストラクタ
		//! @param		callee	呼び出し先オブジェクト
		//! @param		code	オペレーションコード
		//! @param		result	結果の格納先
		//! @param		name	操作を行うメンバ名
		//! @param		flags	オペレーションフラグ
		//! @param		args	引数
		//! @param		bargs	ブロック引数
		//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
		//! @param		closure	メソッドが実行されるべきクロージャ
		tBlock(
			const tRisseVariant & callee,
			tRisseOpCode code,
			tRisseVariant * result = NULL,
			const tRisseString & name = tRisseString::GetEmptyString(),
			risse_uint32 flags = 0,
			const tRisseMethodArgument & args = tRisseMethodArgument::GetEmptyArgument(),
			const tRisseMethodArgument & bargs = tRisseMethodArgument::GetEmptyArgument(),
			const tRisseVariant *This = NULL,
			const tRisseMethodClosure * closure = NULL) :
				State(NULL), 
				Info(callee, code, result, name, flags, args, bargs, This, closure)
		{;}
	};

protected:
	gc_vector<tBlock> Stack; //!< スタックブロックのスタック

public:
	//! @brief		コンストラクタ
	//! @note		Run() を呼び出す前に PushCallee で呼び出し先をpushしておくこと
	tRisseExecutorContext();

	//! @brief		スタックのトップにあるスタックブロックを得る
	tBlock & GetTop()
	{
		RISSE_ASSERT(Stack.size() > 0);
		return Stack.back();
	}

	//! @brief		スタックのトップにあるスタックブロックを得る
	const tBlock & GetTop() const
	{
		RISSE_ASSERT(Stack.size() > 0);
		return Stack.back();
	}

	//! @brief		呼び出しもとのスタックブロックを得る
	tBlock & GetCaller()
	{
		RISSE_ASSERT(Stack.size() > 1);
		return Stack[Stack.size() - 2];
	}

	//! @brief		呼び出しもとのスタックブロックを得る
	const tBlock & GetCaller() const
	{
		RISSE_ASSERT(Stack.size() > 1);
		return Stack[Stack.size() - 2];
	}

	//! @brief		呼び出し元は結果を必要とするかどうかを得る
	//! @return		呼び出し元は結果を必要とするかどうか
	bool DoesCallerReceiveResult() const
	{
		RISSE_ASSERT(Stack.size() >= 2); // callee と caller が必要
		return GetCaller().Info.Result != NULL;
	}

	//! @brief		呼び出し元に結果を返す
	//! @param		result_val		結果
	//! @return		呼び出し元は結果を必要としない場合は何もしない
	void SetResult(const tRisseVariant & result_val) const
	{
		RISSE_ASSERT(Stack.size() >= 2); // callee と caller が必要
		tRisseVariant * result = GetCaller().Info.Result;
		if(result) *result = result_val;
	}

	//! @brief		呼び出し結果の格納先を設定する
	//! @param		val		格納先(スタック上のデータを指定しないこと)
	void SetResultReceiveTarget(tRisseVariant * val)
	{
		GetTop().Info.Result = val;
	}

	//! @brief		呼び出し先をpushする
	//! @param		callee	呼び出し先オブジェクト
	//! @param		code	オペレーションコード
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		bargs	ブロック引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @param		closure	メソッドが実行されるべきクロージャ
	//!						(NULL=クロージャを指定しない場合)
	//!						NULLを指定した場合は tRisseVariant 内で tRisseVariant が
	//!						持っているクロージャの情報が上書きされる
	void PushCallee(
		const tRisseVariant & callee,
		tRisseOpCode code,
		const tRisseString & name = tRisseString::GetEmptyString(),
		risse_uint32 flags = 0,
		const tRisseMethodArgument & args = tRisseMethodArgument::GetEmptyArgument(),
		const tRisseMethodArgument & bargs = tRisseMethodArgument::GetEmptyArgument(),
		const tRisseVariant *This = NULL,
		const tRisseMethodClosure * closure = NULL)
	{
		Stack.push_back(tBlock(callee, code, NULL, name,
							flags, args, bargs, This, closure));
	}

	//! @brief		実行を行う
	void Run();

protected:

	//! @brief		スタックをpopする
	void Pop()
	{
		RISSE_ASSERT(Stack.size() > 0);
		Stack.pop_back();
	}



};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------


} // namespace Risse
#endif

