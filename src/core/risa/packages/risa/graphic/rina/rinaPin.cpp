//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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
RISSE_IMPL_CLASS_BEGIN(tPinClass, (tSS<'P','i','n'>()), engine->ObjectClass)
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * Pin クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tPinClass>;
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
void tInputPinInstance::connect(const tVariant & output_pin)
{
	tOutputPinInstance * instance =
		output_pin.ExpectAndGetObjectInterface(
				tClassHolder<tOutputPinClass>::instance()->GetClass());
	Connect(instance);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tInputPinClass,
		(tSS<'I','n','p','u','t','P','i','n'>()),
		tClassHolder<tPinClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
	BindFunction(this, tSS<'c','o','n','n','e','c','t'>(), &tInputPinInstance::connect);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * InputPin クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tInputPinClass>;
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
		risse_size dist = it.GetValue().
			ExpectAndGetObjectInterface(
				tClassHolder<tInputPinClass>::instance()->GetClass())->
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
RISSE_IMPL_CLASS_BEGIN(tOutputPinClass,
		(tSS<'O','u','t','p','u','t','P','i','n'>()),
		tClassHolder<tPinClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * OutputPin クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tOutputPinClass>;
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
void tPinArrayInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPinArrayInstance::initialize(const tVariant & node, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	SetNodeInstance(node.ExpectAndGetObjectInterface(
		tClassHolder<tNodeClass>::instance()->GetClass()));

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPinArrayInstance::iget(risse_offset ofs_index)
{
	volatile tSynchronizer sync(this); // sync
	volatile tNodeInstance::tGraphLocker holder(NodeInstance);

	tPinInstance * instance = Get(ofs_index);
	if(instance == NULL) return tVariant();
	return instance;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tPinArrayInstance::get_length()
{
	volatile tSynchronizer sync(this); // sync
	volatile tNodeInstance::tGraphLocker holder(NodeInstance);

	return GetCount();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tPinArrayClass,
		(tSS<'P','i','n','A','r','r','a','y'>()),
		engine->ObjectClass)
	RISSE_BIND_CONSTRUCTORS
	BindFunction(this, mnIGet, &tPinArrayInstance::iget);
	BindProperty(this, ss_length, &tPinArrayInstance::get_length);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * PinArray クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tPinArrayClass>;
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

	info.InitializeSuperClass(tMethodArgument::New(node));
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tInputPinArrayClass,
		(tSS<'I','n','p','u','t','P','i','n','A','r','r','a','y'>()),
		tClassHolder<tPinArrayClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * InputPinArray クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tInputPinArrayClass>;
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
void tOneInputPinArrayInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOneInputPinArrayInstance::initialize(const tVariant & node, const tVariant & pin,
			const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	// 引数はそのまま渡す
	info.InitializeSuperClass(info.args);

	// ピンを設定する
	PinInstance = pin.ExpectAndGetObjectInterface(
			tClassHolder<tInputPinClass>::instance()->GetClass());
	PinInstance->SetNodeInstance(GetNodeInstance());

}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tOneInputPinArrayClass,
		(tSS<'O','n','e','I','n','p','u','t','P','i','n','A','r','r','a','y'>()),
		tClassHolder<tInputPinArrayClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * OneInputPinArray クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tOneInputPinArrayClass>;
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

	info.InitializeSuperClass(tMethodArgument::New(node));
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tOutputPinArrayClass,
		(tSS<'O','u','t','p','u','t','P','i','n','A','r','r','a','y'>()),
		tClassHolder<tPinArrayClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * OutputPinArray クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tOutputPinArrayClass>;
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
void tOneOutputPinArrayInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOneOutputPinArrayInstance::initialize(const tVariant & node, const tVariant & pin,
		const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	// 引数はそのまま渡す
	info.InitializeSuperClass(info.args);

	// ピンを設定する
	PinInstance = pin.ExpectAndGetObjectInterface(
			tClassHolder<tOutputPinClass>::instance()->GetClass());
	PinInstance->SetNodeInstance(GetNodeInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tOneOutputPinArrayClass,
		(tSS<'O','n','e','O','u','t','p','u','t','P','i','n','A','r','r','a','y'>()),
		tClassHolder<tOutputPinArrayClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * OneOutputPinArray クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tOneOutputPinArrayClass>;
//---------------------------------------------------------------------------




















//---------------------------------------------------------------------------
}
