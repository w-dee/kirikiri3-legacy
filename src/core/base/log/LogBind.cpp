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
class tLogClass : public tClassBase, depends_on<tLogger>
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tLogClass(tScriptEngine * engine) :
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

		BindFunction(this, ss_construct, &tLogClass::construct,
			tMemberAttribute(	tMemberAttribute(tMemberAttribute::vcConst)|
									tMemberAttribute(tMemberAttribute::ocFinal)) );
		BindFunction(this, tSS<'d','e','b','u','g'>(), &tLogClass::debug);
		BindFunction(this, tSS<'i','n','f','o'>(), &tLogClass::info);
		BindFunction(this, tSS<'n','o','t','i','c','e'>(), &tLogClass::notice);
		BindFunction(this, tSS<'w','a','r','n','i','n','g'>(), &tLogClass::warning);
		BindFunction(this, tSS<'e','r','r','o','r'>(), &tLogClass::error);
		BindFunction(this, tSS<'r','e','c','o','r','d'>(), &tLogClass::record);
		BindFunction(this, tSS<'c','r','i','t','i','c','a','l'>(), &tLogClass::critical);
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
		tLogger::instance()->Log(content, tLogger::llDebug);
	}

	static void info(const tString & content) //!< info メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llInfo);
	}

	static void notice(const tString & content) //!< notice メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llNotice);
	}

	static void warning(const tString & content) //!< warning メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llWarning);
	}

	static void error(const tString & content) //!< error メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llError);
	}

	static void record(const tString & content) //!< record メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llRecord);
	}

	static void critical(const tString & content) //!< critical メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llCritical);
	}

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		Log クラスレジストラ
template class tRisseClassRegisterer<tLogClass>;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa

