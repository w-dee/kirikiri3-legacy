//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief "tRisse" スクリプト言語クラスの実装
//---------------------------------------------------------------------------

#include "risseCommHead.h"

#include <map>
#include "risse.h"
#include "risseScriptBlock.h"
#include "risseArray.h"
#include "risseDictionary.h"
#include "risseDate.h"
#include "risseMath.h"
#include "risseException.h"
#include "risseInterCodeExec.h"
#include "risseScriptCache.h"
#include "risseLex.h"
#include "risseHashSearch.h"
#include "risseRandomGenerator.h"
#include "risseGlobalStringMap.h"
#include "risseDebug.h"




namespace Risse
{
RISSE_DEFINE_SOURCE_ID(47591,10463,24139,18796,8358,56876,22568,35252);


#ifndef RISSE_NO_REGEXP
extern iRisseDispatch2 * RisseCreateRegExpClass();
	// to avoid to include large regexp library header
#endif
//---------------------------------------------------------------------------
// risse Version
//---------------------------------------------------------------------------
const risse_int RisseVersionMajor   = 3;
const risse_int RisseVersionMinor   = 0;
const risse_int RisseVersionRelease = 0;
const risse_int RisseVersionHex =
	RisseVersionMajor * 0x1000000 + RisseVersionMinor * 0x10000 + RisseVersionRelease;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// global options
//---------------------------------------------------------------------------
bool RisseEvalOperatorIsOnGlobal = false;
	// Post-! operator (evaluate expression) is to be executed on "this" context
	// since Risse2 version 2.4.1
bool RisseWarnOnNonGlobalEvalOperator = false;
	// Output warning against non-local post-! operator.
bool RisseEnableDebugMode = false;
	// Enable Risse Debugging support. Enabling this may make the
	// program somewhat slower and using more memory.
	// Do not use this mode unless you want to debug the program.
bool RisseWarnOnExecutionOnDeletingObject = false;
	// Output warning against running code on context of
	// deleting-in-progress object. This is available only the Debug mode is
	// enabled.
bool RisseUnaryAsteriskIgnoresPropAccess = false;
	// Unary '*' operator means accessing property object directly without
	// normal property access, if this options is set true.
	// This is replaced with '&' operator since Risse2 2.4.15. Turn true for
	// gaining old compatibility.
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRissePPMap : value container for pre-processor
//---------------------------------------------------------------------------
class tRissePPMap
{
public:
	tRisseHashTable<ttstr, risse_int32> Values;
};
//---------------------------------------------------------------------------




#define RISSE_GLOBAL_HASH_BITS 7

//---------------------------------------------------------------------------
// tRisse
//---------------------------------------------------------------------------
tRisse::tRisse()
{
	// tRisse constructor
	RefCount = 1;
	ConsoleOutput = NULL;
	PPValues = NULL;

	// ensure variant array stack for function stack
	RisseVariantArrayStackAddRef();

	// ensure hash table for reserved words
	RisseReservedWordsHashAddRef();

	// AddRef create global string map object
	RisseAddRefGlobalStringMap();

	// Create debugging-related objects
	if(RisseEnableDebugMode)
	{
		RisseAddRefObjectHashMap();
		RisseAddRefStackTracer();
	}

	// create script cache object
	Cache = new tRisseScriptCache(this);


	try
	{

		// push version value to pp value
		PPValues = new tRissePPMap();
		PPValues->Values.Add(ttstr(RISSE_WS("version")), RisseVersionHex);

		// create the GLOBAL object
		Global = new tRisseCustomObject(RISSE_GLOBAL_HASH_BITS);

		if(RisseObjectHashMapEnabled())
			RisseObjectHashSetType(Global, ttstr(RISSE_WS("the global object")));

		// register some default classes to the GLOBAL
		iRisseDispatch2 *dsp;
		tRisseVariant val;

		// Array
		dsp = new tRisseArrayClass(); //RisseCreateArrayClass();
		val = tRisseVariant(dsp, NULL);
		dsp->Release();
		Global->PropSet(RISSE_MEMBERENSURE, RISSE_WS("Array"), NULL, &val, Global);

		// Dictionary
		dsp = new tRisseDictionaryClass();
		val = tRisseVariant(dsp, NULL);
		dsp->Release();
		Global->PropSet(RISSE_MEMBERENSURE, RISSE_WS("Dictionary"), NULL, &val, Global);

		// Date
		dsp = new tRisseNC_Date();
		val = tRisseVariant(dsp, NULL);
		dsp->Release();
		Global->PropSet(RISSE_MEMBERENSURE, RISSE_WS("Date"), NULL, &val, Global);

		// Math
		{
			iRisseDispatch2 * math;

			dsp = math = new tRisseNC_Math();
			val = tRisseVariant(dsp, NULL);
			dsp->Release();
			Global->PropSet(RISSE_MEMBERENSURE, RISSE_WS("Math"), NULL, &val, Global);

			// Math.RandomGenerator
			dsp = new tRisseNC_RandomGenerator();
			val = tRisseVariant(dsp, NULL);
			dsp->Release();
			math->PropSet(RISSE_MEMBERENSURE, RISSE_WS("RandomGenerator"), NULL, &val, math);
		}

		// Exception
		dsp = new tRisseNC_Exception();
		val = tRisseVariant(dsp, NULL);
		dsp->Release();
		Global->PropSet(RISSE_MEMBERENSURE, RISSE_WS("Exception"), NULL, &val, Global);
#ifndef RISSE_NO_REGEXP
		// RegExp
		dsp = RisseCreateRegExpClass(); // the body is implemented in risseRegExp.cpp
		val = tRisseVariant(dsp, NULL);
		dsp->Release();
		Global->PropSet(RISSE_MEMBERENSURE, RISSE_WS("RegExp"), NULL, &val, Global);
#endif
	}
	catch(...)
	{
		Cleanup();

		throw;
	}
}
//---------------------------------------------------------------------------
tRisse::~tRisse()
{
	// tRisse destructor
	Cleanup();
}
//---------------------------------------------------------------------------
void tRisse::Cleanup()
{
	RisseVariantArrayStackCompactNow();
	RisseVariantArrayStackRelease();

	if(Global) Global->Release(), Global = NULL;

	if(PPValues) delete PPValues;
	if(Cache) delete Cache;

	RisseReservedWordsHashRelease();

	RisseReleaseGlobalStringMap();

	if(RisseEnableDebugMode)
	{
		RisseReleaseStackTracer();
		RisseReleaseObjectHashMap();
	}
}
//---------------------------------------------------------------------------
void tRisse::AddRef()
{
	RefCount ++;
}
//---------------------------------------------------------------------------
void tRisse::Release()
{
	if(RefCount == 1)
	{
		delete this;
	}
	else
	{
		RefCount --;
	}
}
//---------------------------------------------------------------------------
void tRisse::Shutdown()
{
	RisseVariantArrayStackCompactNow();
	Global->Clear();
	if(Global) Global->Release(), Global = NULL;
	if(Cache) delete Cache, Cache = NULL;
}
//---------------------------------------------------------------------------
iRisseDispatch2 * tRisse::GetGlobal()
{
	Global->AddRef();
	return Global;
}
//---------------------------------------------------------------------------
iRisseDispatch2 * tRisse::GetGlobalNoAddRef() const
{
	return Global;
}
//---------------------------------------------------------------------------
void tRisse::AddScriptBlock(tRisseScriptBlock *block)
{
	ScriptBlocks.push_back(block);
}
//---------------------------------------------------------------------------
void tRisse::RemoveScriptBlock(tRisseScriptBlock *block)
{
	std::vector<tRisseScriptBlock*>::iterator i = ScriptBlocks.begin();
	while(i != ScriptBlocks.end())
	{
		if(*i == block)
		{
			ScriptBlocks.erase(i);
			break;
		}
		i++;
	}
}
//---------------------------------------------------------------------------
void tRisse::SetConsoleOutput(iRisseConsoleOutput* console)
{
	// set a console output callback routine
	ConsoleOutput = console;
}
//---------------------------------------------------------------------------
void tRisse::OutputToConsole(const risse_char *msg) const
{
	if(ConsoleOutput)
	{
		ConsoleOutput->Print(msg);
	}
}
//---------------------------------------------------------------------------
void tRisse::OutputExceptionToConsole(const risse_char *msg) const
{
	if(ConsoleOutput)
	{
		ConsoleOutput->Print(msg);
	}
}
//---------------------------------------------------------------------------
void tRisse::OutputToConsoleWithCentering(const risse_char *msg, risse_uint width) const
{
	// this function does not matter whether msg includes ZENKAKU characters ...
	if(!msg) return;
	risse_int len = Risse_strlen(msg);
	risse_int ns = ((risse_int)width - len)/2;
	if(ns<=0)
	{
		OutputToConsole(msg);
	}
	else
	{
		risse_char *outbuf = new risse_char[ns + len +1];
		risse_char *p = outbuf;
		while(ns--) *(p++)= RISSE_WC(' ');
		Risse_strcpy(p, msg);
		try
		{
			OutputToConsole(outbuf);
		}
		catch(...)
		{
			delete [] outbuf;
			throw;
		}

		delete [] outbuf;
	}
}
//---------------------------------------------------------------------------
void tRisse::OutputToConsoleSeparator(const risse_char *text, risse_uint count) const
{
	risse_int len = Risse_strlen(text);
	risse_char *outbuf = new risse_char [ len * count + 1];
	risse_char *p = outbuf;
	while(count--)
	{
		Risse_strcpy(p, text);
		p += len;
	}

	try
	{
		OutputToConsole(outbuf);
	}
	catch(...)
	{
		delete [] outbuf;
		throw;
	}

	delete [] outbuf;
}
//---------------------------------------------------------------------------
void tRisse::Dump(risse_uint width) const
{
#if 0
	// dumps all existing script block
	risse_char version[100];
	Risse_sprintf(version, RISSE_WS("Risse version %d.%d.%d"), RisseVersionMajor,
		RisseVersionMinor, RisseVersionRelease);

	OutputToConsoleSeparator(RISSE_W("#"), width);
	OutputToConsoleWithCentering(RISSE_W("Risse Context Dump"), width);
	OutputToConsoleSeparator(RISSE_W("#"), width);
	OutputToConsole(version);
	OutputToConsole(RISSE_W(""));

	if(ScriptBlocks.size())
	{
		std::vector<tRisseScriptBlock*>::const_iterator i;

		risse_char buf[1024];
		Risse_sprintf(buf, RISSE_W("Total %d script block(s)"), ScriptBlocks.size());
		OutputToConsole(buf);
		OutputToConsole(RISSE_W(""));

		risse_uint totalcontexts = 0;
		risse_uint totalcodesize = 0;
		risse_uint totaldatasize = 0;

		for(i = ScriptBlocks.begin(); i != ScriptBlocks.end(); i++)
		{
			risse_uint n;
			const risse_char * name = (*i)-> GetName();

			ttstr title;
			if(name)
				title = (*i)-> GetNameInfo();
			else
				title = RISSE_W("(no-named script block)");

			risse_char ptr[256];
			Risse_sprintf(ptr, RISSE_W(" 0x%p"), (*i));

			title += ptr;

			OutputToConsole(title.c_str());

			n = (*i)->GetContextCount();
			totalcontexts += n;
			Risse_sprintf(buf, RISSE_W("\tCount of contexts      : %d"), n);
			OutputToConsole(buf);

			n = (*i)->GetTotalVMCodeSize();
			totalcodesize += n;
			Risse_sprintf(buf, RISSE_W("\tVM code area size      : %d words"), n);
			OutputToConsole(buf);

			n = (*i)->GetTotalVMDataSize();
			totaldatasize += n;
			Risse_sprintf(buf, RISSE_W("\tVM constant data count : %d"), n);
			OutputToConsole(buf);

			OutputToConsole(RISSE_W(""));
		}

		Risse_sprintf(buf, RISSE_W("Total count of contexts      : %d"), totalcontexts);
		OutputToConsole(buf);
		Risse_sprintf(buf, RISSE_W("Total VM code area size      : %d words"), totalcodesize);
		OutputToConsole(buf);
		Risse_sprintf(buf, RISSE_W("Total VM constant data count : %d"), totaldatasize);
		OutputToConsole(buf);

		OutputToConsole(RISSE_W(""));


		for(i = ScriptBlocks.begin(); i != ScriptBlocks.end(); i++)
		{

			OutputToConsoleSeparator(RISSE_W("-"), width);
			const risse_char * name = (*i)-> GetName();

			ttstr title;
			if(name)
				title = (*i)-> GetNameInfo();
			else
				title = RISSE_W("(no-named script block)");

			risse_char ptr[256];
			Risse_sprintf(ptr, RISSE_W(" 0x%p"), (*i));

			title += ptr;

			OutputToConsoleWithCentering(title.c_str(), width);

			OutputToConsoleSeparator(RISSE_W("-"), width);

			(*i)->Dump();

			OutputToConsole(RISSE_W(""));
			OutputToConsole(RISSE_W(""));
		}
	}
	else
	{
		OutputToConsole(RISSE_W(""));
		OutputToConsole(RISSE_W("There are no script blocks in the system."));
	}
#endif
}
//---------------------------------------------------------------------------
void tRisse::ExecScript(const risse_char *script, tRisseVariant *result,
	iRisseDispatch2 *context,
	const risse_char *name, risse_int lineofs)
{
	RisseSetFPUE();
	if(Cache) Cache->ExecScript(script, result, context, name, lineofs);
}
//---------------------------------------------------------------------------
void tRisse::ExecScript(const ttstr &script, tRisseVariant *result,
	iRisseDispatch2 *context,
	const ttstr *name, risse_int lineofs)
{
	RisseSetFPUE();
	if(Cache) Cache->ExecScript(script, result, context, name, lineofs);
}
//---------------------------------------------------------------------------
void tRisse::EvalExpression(const risse_char *expression, tRisseVariant *result,
	iRisseDispatch2 *context,
	const risse_char *name, risse_int lineofs)
{
	RisseSetFPUE();
	if(Cache) Cache->EvalExpression(expression, result, context, name, lineofs);
}
//---------------------------------------------------------------------------
void tRisse::EvalExpression(const ttstr &expression, tRisseVariant *result,
	iRisseDispatch2 *context,
	const ttstr *name, risse_int lineofs)
{
	RisseSetFPUE();
	if(Cache) Cache->EvalExpression(expression, result, context, name, lineofs);
}
//---------------------------------------------------------------------------
void tRisse::SetPPValue(const risse_char *name, risse_int32 value)
{
	PPValues->Values.Add(ttstr(name), value);
}
//---------------------------------------------------------------------------
risse_int32 tRisse::GetPPValue(const risse_char *name)
{
	risse_int32 *f = PPValues->Values.Find(ttstr(name));
	if(!f) return 0;
	return *f;
}
//---------------------------------------------------------------------------
void tRisse::DoGarbageCollection()
{
	// do garbage collection
	RisseVariantArrayStackCompactNow();
	RisseCompactStringHeap();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// TextStream Creation
//---------------------------------------------------------------------------
iRisseTextReadStream * RisseDefCreateTextStreamForRead(const tRisseString &name,
	const tRisseString &mode)
{ return NULL; }
iRisseTextWriteStream * RisseDefCreateTextStreamForWrite(const tRisseString &name,
	const tRisseString &mode)
{ return NULL; }
//---------------------------------------------------------------------------
iRisseTextReadStream * (*RisseCreateTextStreamForRead)(const tRisseString &name,
	const tRisseString &mode) =
	RisseDefCreateTextStreamForRead;
iRisseTextWriteStream * (*RisseCreateTextStreamForWrite)(const tRisseString &name,
	const tRisseString &mode) =
	RisseDefCreateTextStreamForWrite;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tRisseBinaryStream
//---------------------------------------------------------------------------

void tRisseBinaryStream::SetEndOfStorage()
{
	Risse_eRisseError(RisseWriteError);
}
//---------------------------------------------------------------------------
risse_uint64 tRisseBinaryStream::GetSize()
{
	risse_uint64 orgpos = GetPosition();
	risse_uint64 size = Seek(0, RISSE_BS_SEEK_END);
	Seek(orgpos, SEEK_SET);
	return size;
}
//---------------------------------------------------------------------------
risse_uint64 tRisseBinaryStream::GetPosition()
{
	return Seek(0, SEEK_CUR);
}
//---------------------------------------------------------------------------
void tRisseBinaryStream::SetPosition(risse_uint64 pos)
{
	if(pos != Seek(pos, RISSE_BS_SEEK_SET))
		Risse_eRisseError(RisseSeekError);
}
//---------------------------------------------------------------------------
void tRisseBinaryStream::ReadBuffer(void *buffer, risse_uint read_size)
{
	if(Read(buffer, read_size) != read_size)
		Risse_eRisseError(RisseReadError);
}
//---------------------------------------------------------------------------
void tRisseBinaryStream::WriteBuffer(const void *buffer, risse_uint write_size)
{
	if(Write(buffer, write_size) != write_size)
		Risse_eRisseError(RisseWriteError);
}
//---------------------------------------------------------------------------
risse_uint64 tRisseBinaryStream::ReadI64LE()
{
#if RISSE_HOST_IS_BIG_ENDIAN
	risse_uint8 buffer[8];
	ReadBuffer(buffer, 8);
	risse_uint64 ret = 0;
	for(risse_int i=0; i<8; i++)
		ret += (risse_uint64)buffer[i]<<(i*8);
	return ret;
#else
	risse_uint64 temp;
	ReadBuffer(&temp, 8);
	return temp;
#endif
}
//---------------------------------------------------------------------------
risse_uint32 tRisseBinaryStream::ReadI32LE()
{
#if RISSE_HOST_IS_BIG_ENDIAN
	risse_uint8 buffer[4];
	ReadBuffer(buffer, 4);
	risse_uint32 ret = 0;
	for(risse_int i=0; i<4; i++)
		ret += (risse_uint32)buffer[i]<<(i*8);
	return ret;
#else
	risse_uint32 temp;
	ReadBuffer(&temp, 4);
	return temp;
#endif
}
//---------------------------------------------------------------------------
risse_uint16 tRisseBinaryStream::ReadI16LE()
{
#if RISSE_HOST_IS_BIG_ENDIAN
	risse_uint8 buffer[2];
	ReadBuffer(buffer, 2);
	risse_uint16 ret = 0;
	for(risse_int i=0; i<2; i++)
		ret += (risse_uint16)buffer[i]<<(i*8);
	return ret;
#else
	risse_uint16 temp;
	ReadBuffer(&temp, 2);
	return temp;
#endif
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------

} // namespace Risse
