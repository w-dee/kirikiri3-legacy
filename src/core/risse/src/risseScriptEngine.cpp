//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトエンジン管理
//---------------------------------------------------------------------------

#include "prec.h"
#include "risseScriptEngine.h"
#include "risseCoroutine.h"
#include "risseStaticStrings.h"
#include "risse_parser/risseRisseScriptBlockClass.h"

// 各クラスの new に必要なインクルードファイルをインクルードする
#define RISSE_INTERNALCLASSES_INCLUDE
#include "risseInternalClasses.inc"
#undef RISSE_INTERNALCLASSES_INCLUDE

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(4659,21296,13745,17188,45721,42457,47629,47295);


//---------------------------------------------------------------------------
bool tScriptEngine::CommonObjectsInitialized = false;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tScriptEngine::tScriptEngine()
{
	// 各クラスのインスタンスをいったん NULL に設定
	// ここでは StartSentinel と EndSentinel の間が連続して各オブジェクトのインスタンス
	// へのポインタの領域として割り当たっていることを仮定する(構造体というかクラスのレイアウト
	// はそうなっている訳だしコンパイラもそう割り当てるはずだが...)
	EndSentinel = NULL;
	for(void ** p = &StartSentinel; p < &EndSentinel; p++) *p = NULL;

	// フィールドの初期化
	WarningOutput = NULL;

	// 共通に初期化しなくてはならない部分は初期化されているか
	if(!CommonObjectsInitialized)
	{
		CommonObjectsInitialized = true;
		// 共通初期化
		GC_init();
		InitCoroutine();
	}

	// 各クラスのインスタンスを作成する
	#define RISSE_INTERNALCLASSES_CLASS(X) X##Class = new t##X##Class(this);
	#include "risseInternalClasses.inc"
	#undef RISSE_INTERNALCLASSES_CLASS


	// Object, Module, Class の各クラスは Class の作成以前に
	// 作成されており、Class クラスの RTTI が正常に登録されていないはずなので
	// 登録する
	ObjectClass->SetClassClassRTTI(this);
	ModuleClass->SetClassClassRTTI(this);
	ClassClass->SetClassClassRTTI(this);

	// グローバルオブジェクトを "Object" クラスから作成する
	GlobalObject = tVariant(ObjectClass).New();

	// 各クラスをグローバルオブジェクトに登録する
	#define RISSE_INTERNALCLASSES_CLASS(X) X##Class->RegisterClassInstance(GlobalObject);
	#include "risseInternalClasses.inc"
	#undef RISSE_INTERNALCLASSES_CLASS


	// 各クラスのメンバを正式な物に登録し直すためにもう一度RegisterMembersを呼ぶ
	// 上記の状態では メンバとして仮のものが登録されている可能性がある
	// (たとえばArray.modulesはArrayクラスの初期化が終了しないと
	//  登録されないし、各メソッドは Function クラスの初期化が終了しないと
	//  登録できない)。
	// このため、各クラスの RegisterMembers メソッドをもう一度呼び、メンバを
	// 登録し治す。上記ですべてクラスの初期化は終了しているため、
	// もう一度このメソッドを呼べば、正しくメソッドが登録されるはずである。
	#define RISSE_INTERNALCLASSES_CLASS(X) X##Class->RegisterMembers();
	#include "risseInternalClasses.inc"
	#undef RISSE_INTERNALCLASSES_CLASS

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptEngine::Evaluate(const tString & script, const tString & name,
					risse_size lineofs,
					tVariant * result, const tBindingInfo * binding,
					bool is_expression)
{
	try
	{
		// 暫定実装
		// スクリプトブロックを作成(コンパイル)
		tRisseScriptBlockInstance * block;

		tVariant sb =
			tVariant(RisseScriptBlockClass).
				New(0, tMethodArgument::New(script, name, (risse_int64)lineofs));

		block =
			sb.CheckAndGetObjectInterafce<tRisseScriptBlockInstance, tRisseScriptBlockClass>(
																			RisseScriptBlockClass);

		// スクリプトを実行
		block->Evaluate(binding == NULL ? (tBindingInfo(GlobalObject)) : *binding,
								result, is_expression);
	}
	catch(const tTemporaryException * te)
	{
		te->ThrowConverted(this);
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse



