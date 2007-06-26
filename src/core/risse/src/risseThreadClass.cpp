/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Thread" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseThreadClass.h"
#include "risseThread.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"
#include "risseExceptionClass.h"

/*
	Risseスクリプトから見える"Thread" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(18367,25403,3163,16422,1923,3893,57628,424);


//---------------------------------------------------------------------------
//! @brief		Risseスクリプト用スレッドを実装するクラス
//---------------------------------------------------------------------------
class tRisseScriptThread : protected tRisseThread
{
	//TODO: 正しくスレッドの状態をハンドリングすること

	tRisseScriptEngine * ScriptEngine; //!< スクリプトエンジン
	tRisseVariant Method; //!< 実行するメソッド
	tRisseVariant Context; //!< 実行するメソッドのコンテキスト
	tRisseVariant Ret; //!< Execute メソッドの戻り値

protected:
	//! @brief		スレッドの実行ルーチン
	void Execute()
	{
		if(ScriptEngine)
		{
			try
			{
				try
				{
					// Method を呼び出す
					// TODO: メソッドのコンテキストを正しく
					if(!Method.IsVoid())
						Method.FuncCall(ScriptEngine, &Ret, tRisseString::GetEmptyString(), 0,
							tRisseMethodArgument::Empty(), Context);
				}
				catch(const tRisseTemporaryException * te)
				{
					te->ThrowConverted(ScriptEngine);
				}
			}
			catch(const tRisseVariant * e)
			{
				// 例外を受け取った
				// TODO: より親切な表示
				ScriptEngine->GetWarningOutput()->Output(
					tRisseString(
						RISSE_WS_TR("Unexpected thread abortion due to unhandled exception: %1"),
						e->operator tRisseString()));
			}
		}
	}

public:
	//! @brief	コンストラクタ
	tRisseScriptThread()
	{
		ScriptEngine = NULL;
	}

	//! @brief	スクリプトエンジンインスタンスを設定する
	//! @param	engine		スクリプトエンジンインスタンス
	void SetScriptEngine(tRisseScriptEngine * engine) { ScriptEngine = engine ; }

	//! @brief	実行するメソッドを設定する
	//! @param	method		実行するメソッド
	void SetMethod(const tRisseVariant & method) {Method=method;}

	//! @brief	スレッドの実行を開始する
	//! @param	context		デフォルトのコンテキスト
	void Start(const tRisseVariant & context)
	{
		Context = context;
		Run(); // tRisseThread のメソッドのネーミングは Risse スクリプト上の物と違うので注意
	}

	//! @brief	スレッドの終了を待つ
	//! @return	スレッドメソッドの戻り値
	const tRisseVariant & Join()
	{
		Wait(); // tRisseThread のメソッドのネーミングは Risse スクリプト上の物と違うので注意
		return Ret;
	}
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseThreadInstance::tRisseThreadInstance()
{
	Thread = new tRisseScriptThread();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::construct()
{
	volatile tSynchronizer sync(this); // sync

	// スレッドの実装オブジェクトに情報を設定
	Thread->SetScriptEngine(GetRTTI()->GetScriptEngine());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::initialize(const tRisseNativeCallInfo & info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// ブロック引数があればそれを実行、無ければ run メソッドを実行するように設定する
	if(info.args.GetBlockArgumentCount() >= 1)
	{
		Thread->SetMethod(info.args.GetBlockArgument(0));
	}
	else
	{
		Thread->SetMethod(info.This.GetPropertyDirect_Object(ss_run));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::run(void) const
{
	// デフォルトの run は何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::start() const
{
	// スレッドの実行を開始する
	Thread->Start(tRisseVariant((tRisseObjectInterface*)this));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseThreadInstance::join() const
{
	// スレッドの終了を待機する
	return Thread->Join();
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tRisseThreadClass::tRisseThreadClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_ovulate, &tRisseThreadClass::ovulate);
	RisseBindFunction(this, ss_construct, &tRisseThreadInstance::construct);
	RisseBindFunction(this, ss_initialize, &tRisseThreadInstance::initialize);
	RisseBindFunction(this, ss_run, &tRisseThreadInstance::run);
	RisseBindFunction(this, ss_start, &tRisseThreadInstance::start);
	RisseBindFunction(this, ss_join, &tRisseThreadInstance::join);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseThreadClass::ovulate()
{
	return tRisseVariant(new tRisseThreadInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

