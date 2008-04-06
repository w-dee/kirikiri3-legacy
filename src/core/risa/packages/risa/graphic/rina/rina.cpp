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
//! @brief RINA パッケージ
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/graphic/rina/rina.h"
#include "risa/packages/risa/graphic/rina/rinaGraph.h"
#include "risa/packages/risa/graphic/rina/rinaPin.h"
#include "risa/packages/risa/graphic/rina/rinaNode.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(7553,5046,18084,18092,45445,41845,31896,26896);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaGraphicRinaPackageInitializer::tRisaGraphicRinaPackageInitializer(tScriptEngine * engine) :
	tBuiltinPackageInitializer(
		tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>())
{
	GraphClass         = new tGraphClass        (engine);
	NodeClass          = new tNodeClass         (engine);
	PinClass           = new tPinClass          (engine);
	InputPinClass      = new tInputPinClass     (engine);
	OutputPinClass     = new tOutputPinClass    (engine);
	InputPinArrayClass = new tInputPinArrayClass(engine);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaGraphicRinaPackageInitializer::Initialize(tScriptEngine * engine,
	const tString & name, const tVariant & global)
{
	GraphClass        ->RegisterInstance(global);
	NodeClass         ->RegisterInstance(global);
	PinClass          ->RegisterInstance(global);
	InputPinClass     ->RegisterInstance(global);
	OutputPinClass    ->RegisterInstance(global);
	InputPinArrayClass->RegisterInstance(global);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		risa.fs パッケージイニシャライザレジストラ
template class tPackageInitializerRegisterer<tRisaGraphicRinaPackageInitializer>;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
}
