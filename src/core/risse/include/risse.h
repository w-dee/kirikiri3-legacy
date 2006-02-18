//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief "tRisse" スクリプト言語クラスの実装
//---------------------------------------------------------------------------

#ifndef risseH
#define risseH

#include <vector>
#include "risseConfig.h"
#include "risseVariant.h"
#include "risseInterface.h"
#include "risseString.h"
#include "risseMessage.h"


namespace Risse
{

//---------------------------------------------------------------------------
// Risse version
//---------------------------------------------------------------------------
extern const risse_int RisseVersionMajor;
extern const risse_int RisseVersionMinor;
extern const risse_int RisseVersionRelease;
extern const risse_int RisseVersionHex;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Console output callback interface
//---------------------------------------------------------------------------
class iRisseConsoleOutput
{
public:
	virtual void ExceptionPrint(const risse_char *msg) = 0;
	virtual void Print(const risse_char *msg) = 0;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Object Hash Size Limit ( must be larger than or equal to 0 )
//---------------------------------------------------------------------------
extern risse_int RisseObjectHashBitsLimit;



//---------------------------------------------------------------------------
// global options
//---------------------------------------------------------------------------
extern bool RisseEvalOperatorIsOnGlobal;
	// Post-! operator (evaluate expression) is to be executed on "this" context
	// since Risse2 2.4.1.
	// Turn this switch true makes post-! operator running on global context,
	// like Risse2 before 2.4.1.
extern bool RisseWarnOnNonGlobalEvalOperator;
	// Output warning against non-local post-! operator.
	// (For checking where the post-! operators are used)
extern bool RisseEnableDebugMode;
	// Enable Risse Debugging support. Enabling this may make the
	// program somewhat slower and using more memory.
	// Do not use this mode unless you want to debug the program.
extern bool RisseWarnOnExecutionOnDeletingObject;
	// Output warning against running code on context of
	// deleting-in-progress object. This is available only the Debug mode is
	// enabled.
extern bool RisseUnaryAsteriskIgnoresPropAccess;
	// Unary '*' operator means accessing property object directly without
	// normal property access, if this options is set true.
	// This is replaced with '&' operator since Risse2 2.4.15. Turn true for
	// gaining old compatibility.


//---------------------------------------------------------------------------
// tRisse class - "tRisse" Risse API Class
//---------------------------------------------------------------------------
class tRisseScriptBlock;
class tRissePPMap;
class tRisseCustomObject;
class tRisseScriptCache;
class tRisse
{
	friend class tRisseScriptBlock;
private:
	risse_uint RefCount; // reference count

public:
	tRisse();

protected:
	virtual ~tRisse();

public:
	void Cleanup();

	void AddRef();
	void Release();

	void Shutdown();

private:
	tRissePPMap * PPValues;

	std::vector<tRisseScriptBlock*> ScriptBlocks;

	iRisseConsoleOutput *ConsoleOutput;

	tRisseCustomObject * Global;

	tRisseScriptCache * Cache;

public:
	iRisseDispatch2 * GetGlobal();
	iRisseDispatch2 * GetGlobalNoAddRef() const;

private:
	void AddScriptBlock(tRisseScriptBlock *block);
	void RemoveScriptBlock(tRisseScriptBlock *block);

public:
	void SetConsoleOutput(iRisseConsoleOutput *console);
	iRisseConsoleOutput * GetConsoleOutput() const { return ConsoleOutput; };
	void OutputToConsole(const risse_char *msg) const;
	void OutputExceptionToConsole(const risse_char *msg) const;
	void OutputToConsoleWithCentering(const risse_char *msg, risse_uint width) const;
	void OutputToConsoleSeparator(const risse_char *text, risse_uint count) const;

	void Dump(risse_uint width = 80) const; // dumps all existing script block

	void ExecScript(const risse_char *script, tRisseVariant *result = NULL,
		iRisseDispatch2 *context = NULL,
		const risse_char *name = NULL, risse_int lineofs = 0);

	void ExecScript(const ttstr &script, tRisseVariant *result = NULL,
		iRisseDispatch2 *context = NULL,
		const ttstr *name = NULL, risse_int lineofs = 0);

	void EvalExpression(const risse_char *expression, tRisseVariant *result,
		iRisseDispatch2 *context = NULL,
		const risse_char *name = NULL, risse_int lineofs = 0);

	void EvalExpression(const ttstr &expression, tRisseVariant *result,
		iRisseDispatch2 *context = NULL,
		const ttstr *name = NULL, risse_int lineofs = 0);

	void SetPPValue(const risse_char *name, const risse_int32 value);
	risse_int32 GetPPValue(const risse_char *name);

	void DoGarbageCollection();

};
//---------------------------------------------------------------------------


/*[*/
//---------------------------------------------------------------------------
// iRisseTextStream - used by Array.save/load Dictionaty.save/load
//---------------------------------------------------------------------------
class tRisseString;
class iRisseTextReadStream
{
public:
	virtual risse_size Read(tRisseString & targ, risse_size size) = 0;
};
//---------------------------------------------------------------------------
class iRisseTextWriteStream
{
public:
	virtual void Write(const tRisseString & targ) = 0;
};
//---------------------------------------------------------------------------
extern iRisseTextReadStream * (*RisseCreateTextStreamForRead)(const tRisseString &name,
	const tRisseString &modestr);
extern iRisseTextWriteStream * (*RisseCreateTextStreamForWrite)(const tRisseString &name,
	const tRisseString &modestr);
//---------------------------------------------------------------------------




/*]*/
/*[*/
//---------------------------------------------------------------------------
// tRisseBinaryStream constants
//---------------------------------------------------------------------------
#define RISSE_BS_READ 1
#define RISSE_BS_WRITE 2
#define RISSE_BS_APPEND 7
#define RISSE_BS_UPDATE 3

#define RISSE_BS_ACCESS_MASK 0x03
#define RISSE_BS_ACCESS_READ_BIT 1
#define RISSE_BS_ACCESS_WRITE_BIT 2
#define RISSE_BS_ACCESS_APPEND_BIT 4

#define RISSE_BS_SEEK_SET 0
#define RISSE_BS_SEEK_CUR 1
#define RISSE_BS_SEEK_END 2
//---------------------------------------------------------------------------




/*]*/




//---------------------------------------------------------------------------
// tRisseBinaryStream base stream class
//---------------------------------------------------------------------------
class tRisseBinaryStream
{
private:
public:
	//-- must implement
	virtual risse_uint64 Seek(risse_int64 offset, risse_int whence) = 0;
		/* if error, position is not changed */

	//-- optionally to implement
	virtual risse_uint Read(void *buffer, risse_uint read_size) = 0;
		/* returns actually read size */

	virtual risse_uint Write(const void *buffer, risse_uint write_size) = 0;
		/* returns actually written size */

	virtual void SetEndOfStorage();
		// the default behavior is raising a exception
		/* if error, raises exception */

	//-- should re-implement for higher performance
	virtual risse_uint64 GetSize() = 0;

	virtual ~tRisseBinaryStream() {;}

	risse_uint64 GetPosition();

	void SetPosition(risse_uint64 pos);

	void ReadBuffer(void *buffer, risse_uint read_size);
	void WriteBuffer(const void *buffer, risse_uint write_size);

	risse_uint64 ReadI64LE(); // reads little-endian integers
	risse_uint32 ReadI32LE();
	risse_uint16 ReadI16LE();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

}
#endif


