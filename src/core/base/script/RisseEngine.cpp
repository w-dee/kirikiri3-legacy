//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseスクリプトエンジンの開始・終了・スクリプト実行などのインターフェース
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/script/RisseEngine.h"
#include "base/log/Log.h"
#include "risse/include/risseObjectBase.h"
#include "base/fs/common/FSManager.h"
#include "risse/include/risseExceptionClass.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(50344,48369,3431,18494,14208,60463,45295,19784);
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		"Script" クラス
//---------------------------------------------------------------------------
class tScriptClass : public tClassBase, depends_on<tRisseScriptEngine>
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tScriptClass(tScriptEngine * engine) :
		tClassBase(tSS<'S','c','r','i','p','t'>(), engine->ObjectClass)
	{
		RegisterMembers();
	}

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers()
	{
		// 親クラスの RegisterMembers を呼ぶ
		inherited::RegisterMembers();

		// クラスに必要なメソッドを登録する
		// このクラスのインスタンスは作られないのでinitializeメソッドはないが、
		// construct メソッドはある (finalであることを表す)

		BindFunction(this, ss_construct, &tScriptClass::construct,
			tMemberAttribute(	tMemberAttribute(tMemberAttribute::vcConst)|
									tMemberAttribute(tMemberAttribute::ocFinal)) );
		BindFunction(this, tSS<'r','e','q','u','i','r','e'>(), &tScriptClass::require);
	}

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tVariant CreateNewObjectBase()
	{
		// このクラスのインスタンスは作成できないので例外を投げる
		tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
		return tVariant();
	}

public: // Risse 用メソッドなど
	static void construct()
	{
		// 何もしない
	}

	static void require(const tString & filename)
	{
		// ファイルを読み込んで実行する
		tRisseScriptEngine::instance()->EvaluateFile(filename, NULL, NULL, false);
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Script クラスレジストラ
template class tRisseClassRegisterer<tScriptClass>;
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tRisseScriptEngine::tRisseScriptEngine()
{
	ScriptEngine = new tScriptEngine();
	DefaultRTTI = new tRTTI(ScriptEngine);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseScriptEngine::~tRisseScriptEngine()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::Shutdown()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::RegisterGlobalObject(const tString & name, const tVariant & object)
{
	if(!ScriptEngine) return;
	// グローバルオブジェクトは tObjectBase のはず・・・
	RISSE_ASSERT(dynamic_cast<tObjectBase *>(ScriptEngine->GetGlobalObject().GetObjectInterface()) != NULL);
	static_cast<tObjectBase *>(ScriptEngine->GetGlobalObject().GetObjectInterface())->RegisterNormalMember(name, object);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::EvaluateExpresisonAndPrintResultToConsole(const tString & expression)
{
	// execute the expression
	tString result_str;
	tVariant result;
	try
	{
		ScriptEngine->Evaluate(expression, RISSE_WS_TR("console"), 0, &result, NULL, true);
	}
	catch(const tVariant * e)
	{
		// An exception had been occured in console quick Risse expression evaluation
		result_str = tString(RISSE_WS_TR("(Console)")) + expression +
			tString(RISSE_WS_TR(" = (exception %1) "), e->GetClassName()) +
			e->operator tString();
		tLogger::Log(result_str, tLogger::llError);
		return;
	}
	catch(...)
	{
		throw;
	}

	// success in console quick Risse expression evaluation
	result_str = tString(RISSE_WS_TR("(Console) ")) + expression +
		tString(RISSE_WS_TR(" = ")) + result.AsHumanReadable();
	tLogger::Log(result_str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::Evaluate(const tString & script, const tString & name,
				risse_size lineofs,
				tVariant * result,
				const tBindingInfo * binding, bool is_expression)
{
	ScriptEngine->Evaluate(script, name, lineofs, result, binding, is_expression);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::EvaluateFile(const tString & filename,
				tVariant * result, const tBindingInfo * binding, bool is_expression)
{
	// ファイルを読み込む
	// TODO: ここは正規のテキスト読み込みルーチンを通じてファイルを読み込むべき
	tStreamAdapter stream(tFileSystemManager::instance()->Open(filename, tFileOpenModes::omRead));

	risse_uint64 size64 = stream.GetSize();
	risse_size size = static_cast<risse_size>(size64);

	if(size != size64) tIOExceptionClass::Throw(tString(RISSE_WS_TR("too large file: %1"), filename));

	unsigned char * buf = new (PointerFreeGC) unsigned char [size + 1];
	stream.ReadBuffer(buf, size);
	buf[size] = '\0';

	// 評価を行う
	Evaluate(reinterpret_cast<char*>(buf), filename, 0, result, binding, is_expression);
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
} // namespace Risa

