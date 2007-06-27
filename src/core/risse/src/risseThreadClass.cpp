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
	tRisseThreadInstance * Owner; //!< オーナー

protected:
	//! @brief		スレッドの実行ルーチン
	void Execute()
	{
		if(Owner)
		{
			tRisseScriptEngine * engine = Owner->GetRTTI()->GetScriptEngine();
			try
			{
				try
				{
					// Method を呼び出す
					// TODO: メソッドのコンテキストを正しく
					if(!Owner->Method.IsVoid())
						Owner->Method.FuncCall(engine, &Owner->Ret,
							tRisseString::GetEmptyString(), 0,
							tRisseMethodArgument::Empty(), Owner->Context);
				}
				catch(const tRisseTemporaryException * te)
				{
					te->ThrowConverted(engine);
				}
			}
			catch(const tRisseVariant * e)
			{
				// 例外を受け取った
				// TODO: より親切な表示
				engine->GetWarningOutput()->Output(
					tRisseString(
						RISSE_WS_TR("Unexpected thread abortion due to unhandled exception: %1"),
						e->operator tRisseString()));
			}
		}

		// スレッドが存在している間は、これらはスタック上などから参照可能な位置にあるが、
		// スレッドが無くなったとき (GCからも見えなくなったとき)、また他のポインタも
		// とぎれたとき、これらのポインタが残っていると、finalization が
		// 必要なオブジェクトを巻き込んだ循環参照に
		// はまることになる。それを避けるため、このオブジェクトが持っているポインタの類を
		// すべてここで無効にする。
		Owner = NULL;
	}

public:
	//! @brief	コンストラクタ
	tRisseScriptThread()
	{
		Owner = NULL;
	}

	//! @brief	スレッドの実行を開始する
	//! @param	owner		オーナーとなる tRisseThreadInstance のインスタンス
	void Start(tRisseThreadInstance * owner)
	{
		Owner = owner;
		Run(); // tRisseThread のメソッドのネーミングは Risse スクリプト上の物と違うので注意
	}

	//! @brief	スレッドの終了を待つ
	//! @return	スレッドメソッドの戻り値
	void Join()
	{
		Wait(); // tRisseThread のメソッドのネーミングは Risse スクリプト上の物と違うので注意
	}
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseThreadInstance::tRisseThreadInstance()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::construct()
{
	// 何もしない
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
		Method = info.args.GetBlockArgument(0);
	else
		Method = info.This.GetPropertyDirect_Object(ss_run);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::run(void) const
{
	// デフォルトの run は何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::start()
{
	// スレッドの実行を開始する
	{
		volatile tSynchronizer sync(this); // sync

		if(Thread) return; // already started
		Thread = new tRisseScriptThread();
	}

	Context = (tRisseObjectInterface*)this;

	Thread->Start(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseThreadInstance::join() const
{
	// スレッドの終了を待機する
	if(!Thread) return Ret; // no thread
	Thread->Join();
	return Ret;
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

