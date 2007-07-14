//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログ機構のRisseバインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/log/Log.h"
#include "base/exception/RisaException.h"
#include "base/script/RisseEngine.h"
#include "risse/include/risseStringTemplate.h"
#include "risse/include/risseStaticStrings.h"
#include "risse/include/risseObjectClass.h"
#include "risse/include/risseExceptionClass.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(17391,44513,60617,19121,16540,39031,65356,19696);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		"Log" クラス
//---------------------------------------------------------------------------
class tRisaLogClass : public tClassBase, depends_on<tRisaLogger>
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisaLogClass(tScriptEngine * engine) :
		tClassBase(tSS<'L','o','g'>(), engine->ObjectClass)
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

		BindFunction(this, ss_construct, &tRisaLogClass::construct,
			tMemberAttribute(	tMemberAttribute(tMemberAttribute::vcConst)|
									tMemberAttribute(tMemberAttribute::ocFinal)) );
		BindFunction(this, tSS<'d','e','b','u','g'>(), &tRisaLogClass::debug);
		BindFunction(this, tSS<'i','n','f','o'>(), &tRisaLogClass::info);
		BindFunction(this, tSS<'n','o','t','i','c','e'>(), &tRisaLogClass::notice);
		BindFunction(this, tSS<'w','a','r','n','i','n','g'>(), &tRisaLogClass::warning);
		BindFunction(this, tSS<'e','r','r','o','r'>(), &tRisaLogClass::error);
		BindFunction(this, tSS<'r','e','c','o','r','d'>(), &tRisaLogClass::record);
		BindFunction(this, tSS<'c','r','i','t','i','c','a','l'>(), &tRisaLogClass::critical);
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

	static void debug(const tString & content) //!< debug メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llDebug);
	}

	static void info(const tString & content) //!< info メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llInfo);
	}

	static void notice(const tString & content) //!< notice メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llNotice);
	}

	static void warning(const tString & content) //!< warning メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llWarning);
	}

	static void error(const tString & content) //!< error メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llError);
	}

	static void record(const tString & content) //!< record メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llRecord);
	}

	static void critical(const tString & content) //!< critical メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llCritical);
	}

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		Log クラスレジストラ
template class tRisaRisseClassRegisterer<tRisaLogClass>;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa

