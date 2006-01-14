//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief デバッグのサポート
//---------------------------------------------------------------------------
#ifndef risseDebugH
#define risseDebugH

#include "risse.h"
#include "risseString.h"

namespace Risse
{
//---------------------------------------------------------------------------
// ObjectHashMap : hash map to track object construction/destruction
//---------------------------------------------------------------------------
// object hash map flags
#define RISSE_OHMF_EXIST        1  // The object is in object hash map
#define RISSE_OHMF_INVALIDATED  2  // The object had been invalidated  // currently not used
#define RISSE_OHMF_DELETING     4  // The object is now being deleted
#define RISSE_OHMF_SET          (~0)
#define RISSE_OHMF_UNSET        (0)
//---------------------------------------------------------------------------
class tRisseScriptBlock;
struct tRisseObjectHashMapRecord;

class tRisseObjectHashMap;
extern tRisseObjectHashMap * RisseObjectHashMap;
extern tRisseBinaryStream * RisseObjectHashMapLog;
extern void RisseAddRefObjectHashMap();
extern void RisseReleaseObjectHashMap();
extern void RisseAddObjectHashRecord(void * object);
extern void RisseRemoveObjectHashRecord(void * object);
extern void RisseObjectHashSetType(void * object, const ttstr &type);
extern void RisseSetObjectHashFlag(void * object, risse_uint32 flags_to_change, risse_uint32 bits);
extern void RisseReportAllUnfreedObjects(iRisseConsoleOutput * output);
extern bool RisseObjectHashAnyUnfreed();
extern void RisseObjectHashMapSetLog(tRisseBinaryStream * stream);
extern void RisseWriteAllUnfreedObjectsToLog();
extern void RisseWarnIfObjectIsDeleting(iRisseConsoleOutput * output, void * object);
extern void RisseReplayObjectHashMapLog();
static inline bool RisseObjectHashMapEnabled() { return RisseObjectHashMap || RisseObjectHashMapLog; }
extern inline bool RisseObjectTypeInfoEnabled() { return RisseObjectHashMap; }
extern inline bool RisseObjectFlagEnabled() { return RisseObjectHashMap; }
extern ttstr RisseGetObjectTypeInfo(void * object);
extern risse_uint32 RisseGetObjectHashCheckFlag(void * object);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// StackTracer : stack to trace function call trace
//---------------------------------------------------------------------------
class tRisseStackTracer;
class tRisseInterCodeContext;
extern tRisseStackTracer * RisseStackTracer;
extern void RisseAddRefStackTracer();
extern void RisseReleaseStackTracer();
extern void RisseStackTracerPush(tRisseInterCodeContext *context, bool in_try);
extern void RisseStackTracerSetCodePointer(const risse_int32 * codebase, risse_int32 * const * codeptr);
extern void RisseStackTracerPop();
extern ttstr RisseGetStackTraceString(risse_int limit = 0, const risse_char *delimiter = NULL);
static inline bool RisseStackTracerEnabled() { return RisseStackTracer; }
//---------------------------------------------------------------------------

} // namespace Risse

#endif
