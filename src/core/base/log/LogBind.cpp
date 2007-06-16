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

RISSE_DEFINE_SOURCE_ID(17391,44513,60617,19121,16540,39031,65356,19696);

//---------------------------------------------------------------------------
//! @brief		"Log" クラス
//---------------------------------------------------------------------------
class tRisaLogClass : public tRisseClassBase, depends_on<tRisaLogger>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisaLogClass(tRisseScriptEngine * engine) :
		tRisseClassBase(engine->ObjectClass)
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

		RisseBindFunction(this, ss_construct, &tRisaLogClass::construct,
			tRisseMemberAttribute(	tRisseMemberAttribute(tRisseMemberAttribute::vcConst)|
									tRisseMemberAttribute(tRisseMemberAttribute::ocFinal)) );
		RisseBindFunction(this, tRisseSS<'d','e','b','u','g'>(), &tRisaLogClass::debug);
		RisseBindFunction(this, tRisseSS<'i','n','f','o'>(), &tRisaLogClass::info);
		RisseBindFunction(this, tRisseSS<'n','o','t','i','c','e'>(), &tRisaLogClass::notice);
		RisseBindFunction(this, tRisseSS<'w','a','r','n','i','n','g'>(), &tRisaLogClass::warning);
		RisseBindFunction(this, tRisseSS<'e','r','r','o','r'>(), &tRisaLogClass::error);
		RisseBindFunction(this, tRisseSS<'r','e','c','o','r','d'>(), &tRisaLogClass::record);
		RisseBindFunction(this, tRisseSS<'c','r','i','t','i','c','a','l'>(), &tRisaLogClass::critical);
	}

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase()
	{
		// このクラスのインスタンスは作成できないので例外を投げる
		tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
		return tRisseVariant();
	}

public: // Risse 用メソッドなど
	static void construct()
	{
		// 何もしない
	}

	static void debug(const tRisseString & content) //!< debug メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llDebug);
	}

	static void info(const tRisseString & content) //!< info メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llInfo);
	}

	static void notice(const tRisseString & content) //!< notice メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llNotice);
	}

	static void warning(const tRisseString & content) //!< warning メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llWarning);
	}

	static void error(const tRisseString & content) //!< error メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llError);
	}

	static void record(const tRisseString & content) //!< record メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llRecord);
	}

	static void critical(const tRisseString & content) //!< critical メッセージ出力
	{
		tRisaLogger::instance()->Log(content, tRisaLogger::llCritical);
	}

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		Log クラスレジストラ
template class tRisaRisseClassRegisterer<tRisaLogClass, tRisseSS<'L','o','g'> >;
//---------------------------------------------------------------------------
