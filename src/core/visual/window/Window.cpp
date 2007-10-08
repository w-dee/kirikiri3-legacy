//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief サウンドクラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/window/Window.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(4811,32114,33460,19785,53925,19531,59339,65072);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tWindowInstance::tWindowInstance()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::construct()
{
	// 特にやること無し
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
//! @brief		"Window" クラス
//---------------------------------------------------------------------------
class tWindowClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tWindowClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tWindowClass::tWindowClass(tScriptEngine * engine) :
	tClassBase(tSS<'W','i','n','d','o','w'>(),
	tRisseClassRegisterer<tEventSourceClass>::instance()->GetClassInstance())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWindowClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tWindowClass::ovulate);
	BindFunction(this, ss_construct, &tWindowInstance::construct);
	BindFunction(this, ss_initialize, &tWindowInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tWindowClass::ovulate()
{
	return tVariant(new tWindowInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Window クラスレジストラ
template class tRisseClassRegisterer<tWindowClass>;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risa


