//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief "System" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/system/SystemClass.h"
#include "base/exception/RisaException.h"
#include "base/system/StandardIO.h"





namespace Risa {
RISSE_DEFINE_SOURCE_ID(39928,10664,59105,17103,17283,64007,22298,6764);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tSystemClass::tSystemClass(tScriptEngine * engine) :
	tClassBase(tSS<'S','y','s','t','e','m'>(), engine->ObjectClass)
{
	RegisterMembers();

	// stdin, stdout, stderr を作成して登録する
	tClassBase * stdio_cls =
		tRisseClassRegisterer<tStandardIOStreamClass>::instance()->GetClassInstance();
	tVariant stdin_v  = stdio_cls->Invoke(ss_new, (risse_int64)0);
	tVariant stdout_v = stdio_cls->Invoke(ss_new, (risse_int64)1);
	tVariant stderr_v = stdio_cls->Invoke(ss_new, (risse_int64)2);

	RegisterFinalConstMember(tSS<'s','t','d','i','n'>(),     stdin_v,  true);
	RegisterFinalConstMember(tSS<'s','t','d','o','u','t'>(), stdout_v, true);
	RegisterFinalConstMember(tSS<'s','t','d','e','r','r'>(), stderr_v, true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSystemClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tSystemClass::ovulate);
	BindFunction(this, ss_construct, &tSystemClass::construct);
	BindFunction(this, ss_initialize, &tSystemClass::initialize);
	BindFunction(this, tSS<'c','o','n','f','i','r','m'>(), &tSystemClass::confirm);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tSystemClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tSystemClass::confirm(const tString & message, const tMethodArgument & args)
{
	// TODO: メインスレッドからの呼び出しのみに限定した方がいいのでは？
	// TODO: 親ウィンドウの指定は？
	wxString caption = args.HasArgument(1) ?
		args[1].operator tString().AsWxString() :
		wxString(_("Confirmation"));
	int result = ::wxMessageBox(message.AsWxString(), caption, wxYES_NO, NULL);
	if(result == wxNO)
		return false;
	else
		return true;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		System クラスレジストラ
template class tRisseClassRegisterer<tSystemClass>;
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa

