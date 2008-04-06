//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA ピン管理
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/graphic/rina/rinaPin.h"
#include "risa/common/RisseEngine.h"
#include "risa/packages/risa/graphic/rina/rina.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(31046,8618,37955,16475,39331,46428,7001,14742);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tPinInstance::tPinInstance()
{
	NodeInstance = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tPinInstance::SuggestType(tPinInstance * pin, bool * strong_suggest)
{
	const gc_vector<risse_uint32> & this_types = GetSupportedTypes();
	const gc_vector<risse_uint32> &  his_types = pin->GetSupportedTypes();

	// こちらの対応するタイプを基準にあちらのタイプと一致している物が
	// あるかどうかを見ていく。タイプはそれほど多くならないと考えられるので
	// 非常に原始的なアルゴリズムでマッチングを行う
	for(gc_vector<risse_uint32>::const_iterator ti = this_types.begin();
		ti != this_types.end(); ti++)
	{
		for(gc_vector<risse_uint32>::const_iterator hi = his_types.begin();
			hi != his_types.end(); hi++)
			if(*ti == *hi)
			{
				if(strong_suggest) *strong_suggest = false;
				// このメソッドをオーバーライドしない限り
				// *strong_suggest が真になることはない ...
				return *ti;
			}
	}
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPinInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPinInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tPinClass::tPinClass(tScriptEngine * engine) :
	inherited(tSS<'P','i','n'>(), engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPinClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tPinClass::ovulate);
	BindFunction(this, ss_construct, &tPinInstance::construct);
	BindFunction(this, ss_initialize, &tPinInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPinClass::ovulate()
{
	// このクラスのインスタンスは生成できない
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------




















//---------------------------------------------------------------------------
tInputPinInstance::tInputPinInstance()
{
	OutputPinInstance = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tInputPinInstance::Negotiate(tOutputPinInstance * output_pin)
{
	bool this_strong_suggestion;
	risse_uint32 this_suggested_type = SuggestType(output_pin, &this_strong_suggestion);
	bool  his_strong_suggestion;
	risse_uint32  his_suggested_type = output_pin->SuggestType(this, & his_strong_suggestion);

	// どちらかが 0 を返した場合は同意に至らなかったと見なす
	if(this_suggested_type == 0 || his_suggested_type == 0) return 0;

	// どちらかが strong suggestion を示した場合はそちらを採用する
	if(this_strong_suggestion && !his_strong_suggestion) return this_suggested_type;
	if(!this_strong_suggestion && his_strong_suggestion) return  his_suggested_type;

	// どちらも strong suggestion では無かった場合や どちらも strong suggestion だった場合は
	// 入力ピン側の結果を優先する
	return this_suggested_type;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tInputPinInstance::InternalConnect(tOutputPinInstance * output_pin)
{
	AgreedType = Negotiate(output_pin);
//	if(AgreedType == 0) { /* TODO: 例外 */ }
	RISSE_ASSERT(AgreedType != 0);
	if(OutputPinInstance)
	{
		OutputPinInstance->Disconnect(this);
		OutputPinInstance->GetNodeInstance()->CalcLongestDistance();
	}
	OutputPinInstance = output_pin;
	output_pin->Connect(this);
	OutputPinInstance->GetNodeInstance()->CalcLongestDistance();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInputPinInstance::Connect(tOutputPinInstance * output_pin)
{
	volatile tGraphLocker holder(this);
	InternalConnect(output_pin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tInputPinInstance::GetAgreedType() const
{
	volatile tGraphLocker holder(this);
	return AgreedType;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInputPinInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInputPinInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tInputPinClass::tInputPinClass(tScriptEngine * engine) :
	inherited(tSS<'I','n','p','u','t','P','i','n'>(),
	tPackageInitializerRegisterer<tRisaGraphicRinaPackageInitializer>::instance()->GetInitializer()->PinClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInputPinClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tInputPinClass::ovulate);
	BindFunction(this, ss_construct, &tInputPinInstance::construct);
	BindFunction(this, ss_initialize, &tInputPinInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tInputPinClass::ovulate()
{
	// このクラスのインスタンスは生成できない
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
tOutputPinInstance::tOutputPinInstance()
{
	InputPins = static_cast<tArrayInstance*>(
		tVariant(tRisseScriptEngine::instance()->GetScriptEngine()->ArrayClass).New().GetObjectInterface());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tOutputPinInstance::GetLongestDistance() const
{
	risse_size longest = risse_size_max;
	tEnumerableIterator it(InputPins);
	while(it.Next())
	{
		// 識別子が長げえなぁ
		risse_size dist = it.GetValue().
			ExpectAndGetObjectInterafce<tInputPinInstance>(
				tPackageInitializerRegisterer<tRisaGraphicRinaPackageInitializer>::instance()->GetInitializer()->InputPinClass)->
			GetNodeInstance()->GetLongestDistance();
		if(longest == risse_size_max || longest < dist)
			longest = dist;
	}
	return longest;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOutputPinInstance::Connect(tInputPinInstance * input_pin)
{
	bool already_connected =
		InputPins->Invoke(ss_has, tVariant(input_pin)).operator bool();
	if(already_connected)
	{
		// すでに input_pin が配列中にあった………
		RISSE_ASSERT(!"already connected at tOutputPinInstance::Connect");
		return;
	}

	InputPins->Invoke(ss_push, input_pin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOutputPinInstance::Disconnect(tInputPinInstance * input_pin)
{
	if(InputPins->Invoke(ss_remove, tVariant(input_pin)).IsVoid())
	{
		// input_ping が配列中になかった
		RISSE_ASSERT(!"input_pin is not connected at tOutputPinInstance::Disconnect");
	}

}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void tOutputPinInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOutputPinInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tOutputPinClass::tOutputPinClass(tScriptEngine * engine) :
	inherited(tSS<'O','u','t','p','u','t','P','i','n'>(),
	tPackageInitializerRegisterer<tRisaGraphicRinaPackageInitializer>::instance()->GetInitializer()->PinClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOutputPinClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tOutputPinClass::ovulate);
	BindFunction(this, ss_construct, &tOutputPinInstance::construct);
	BindFunction(this, ss_initialize, &tOutputPinInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tOutputPinClass::ovulate()
{
	// このクラスのインスタンスは生成できない
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void tInputPinArrayInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInputPinArrayInstance::initialize(const tVariant & node, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	SetNodeInstance(node.ExpectAndGetObjectInterafce<tNodeInstance>(
		tPackageInitializerRegisterer<tRisaGraphicRinaPackageInitializer>::instance()->GetInitializer()->NodeClass));

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tInputPinArrayClass::tInputPinArrayClass(tScriptEngine * engine) :
	inherited(tSS<'I','n','p','u','t','P','i','n','A','r','r','a','y'>(),
	engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tInputPinArrayClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tInputPinArrayClass::ovulate);
	BindFunction(this, ss_construct, &tInputPinArrayInstance::construct);
	BindFunction(this, ss_initialize, &tInputPinArrayInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tInputPinArrayClass::ovulate()
{
	return tVariant(new tInputPinArrayInstance());
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void tOutputPinArrayInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOutputPinArrayInstance::initialize(const tVariant & node, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	SetNodeInstance(node.ExpectAndGetObjectInterafce<tNodeInstance>(
		tPackageInitializerRegisterer<tRisaGraphicRinaPackageInitializer>::instance()->GetInitializer()->NodeClass));

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tOutputPinArrayClass::tOutputPinArrayClass(tScriptEngine * engine) :
	inherited(tSS<'O','u','t','p','u','t','P','i','n','A','r','r','a','y'>(),
	engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOutputPinArrayClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tOutputPinArrayClass::ovulate);
	BindFunction(this, ss_construct, &tOutputPinArrayInstance::construct);
	BindFunction(this, ss_initialize, &tOutputPinArrayInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tOutputPinArrayClass::ovulate()
{
	return tVariant(new tOutputPinArrayInstance());
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
}
