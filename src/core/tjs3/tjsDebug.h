//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS Debugging support
//---------------------------------------------------------------------------
#ifndef tjsDebugH
#define tjsDebugH

#include "tjsString.h"

namespace TJS
{
//---------------------------------------------------------------------------
// ObjectHashMap : hash map to track object construction/destruction
//---------------------------------------------------------------------------
// object hash map flags
#define TJS_OHMF_EXIST        1  // The object is in object hash map
#define TJS_OHMF_INVALIDATED  2  // The object had been invalidated  // currently not used
#define TJS_OHMF_DELETING     4  // The object is now being deleted
#define TJS_OHMF_SET          (~0)
#define TJS_OHMF_UNSET        (0)
//---------------------------------------------------------------------------
class tTJSScriptBlock;
struct tTJSObjectHashMapRecord;

class tTJSObjectHashMap;
extern tTJSObjectHashMap * TJSObjectHashMap;
extern tTJSBinaryStream * TJSObjectHashMapLog;
extern void TJSAddRefObjectHashMap();
extern void TJSReleaseObjectHashMap();
extern void TJSAddObjectHashRecord(void * object);
extern void TJSRemoveObjectHashRecord(void * object);
extern void TJSObjectHashSetType(void * object, const ttstr &type);
extern void TJSSetObjectHashFlag(void * object, tjs_uint32 flags_to_change, tjs_uint32 bits);
extern void TJSReportAllUnfreedObjects(iTJSConsoleOutput * output);
extern bool TJSObjectHashAnyUnfreed();
extern void TJSObjectHashMapSetLog(tTJSBinaryStream * stream);
extern void TJSWriteAllUnfreedObjectsToLog();
extern void TJSWarnIfObjectIsDeleting(iTJSConsoleOutput * output, void * object);
extern void TJSReplayObjectHashMapLog();
static inline bool TJSObjectHashMapEnabled() { return TJSObjectHashMap || TJSObjectHashMapLog; }
extern inline bool TJSObjectTypeInfoEnabled() { return TJSObjectHashMap; }
extern inline bool TJSObjectFlagEnabled() { return TJSObjectHashMap; }
extern ttstr TJSGetObjectTypeInfo(void * object);
extern tjs_uint32 TJSGetObjectHashCheckFlag(void * object);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// StackTracer : stack to trace function call trace
//---------------------------------------------------------------------------
class tTJSStackTracer;
class tTJSInterCodeContext;
extern tTJSStackTracer * TJSStackTracer;
extern void TJSAddRefStackTracer();
extern void TJSReleaseStackTracer();
extern void TJSStackTracerPush(tTJSInterCodeContext *context, bool in_try);
extern void TJSStackTracerSetCodePointer(const tjs_int32 * codebase, tjs_int32 * const * codeptr);
extern void TJSStackTracerPop();
extern ttstr TJSGetStackTraceString(tjs_int limit = 0, const tjs_char *delimiter = NULL);
static inline bool TJSStackTracerEnabled() { return TJSStackTracer; }
//---------------------------------------------------------------------------

} // namespace TJS

#endif
