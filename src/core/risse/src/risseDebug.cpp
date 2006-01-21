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
#include "risseCommHead.h"

#include <algorithm>
#include "risseDebug.h"
#include "risseHashSearch.h"
#include "risseInterCodeGen.h"
#include "risseGlobalStringMap.h"



namespace Risse
{
RISSE_DEFINE_SOURCE_ID(37721,57543,13976,19997,6330,1842,60033,38333);
//---------------------------------------------------------------------------
// ObjectHashMap : hash map to track object construction/destruction
//--------------------------------------------------------------------------
tRisseBinaryStream * RisseObjectHashMapLog = NULL;  // log file object
//---------------------------------------------------------------------------
enum tRisseObjectHashMapLogItemId
{
	liiEnd,     // 00 end of the log
	liiVersion, // 01 identify log structure version
	liiAdd,     // 02 add a object
	liiRemove,  // 03 remove a object
	liiSetType, // 04 set object type information
	liiSetFlag, // 05 set object flag
};
//---------------------------------------------------------------------------
template <typename T>
static void RisseStoreLog(const T &object)
{
	RisseObjectHashMapLog->Write(&object, sizeof(object));
}
//---------------------------------------------------------------------------
template <typename T>
static T RisseRestoreLog()
{
	T object;
	RisseObjectHashMapLog->Read(&object, sizeof(object));
	return object;
}
//---------------------------------------------------------------------------
template <>
static void RisseStoreLog<ttstr>(const ttstr & which)
{
	// store a string into log stream
	risse_int length = which.GetLen();
	RisseObjectHashMapLog->Write(&length, sizeof(length));
		// Note that this function does not care what the endian is.
	RisseObjectHashMapLog->Write(which.c_str(), length * sizeof(risse_char));
}
//---------------------------------------------------------------------------
template <>
static ttstr RisseRestoreLog<ttstr>()
{
	// restore a string from log stream
	risse_int length;
	RisseObjectHashMapLog->Read(&length, sizeof(length));
	ttstr ret((tRisseStringBufferLength)(length));
	risse_char *buf = ret.Independ();
	RisseObjectHashMapLog->Read(buf, length * sizeof(risse_char));
	buf[length] = 0;
	ret.FixLen();
	return ret;
}
//---------------------------------------------------------------------------
template <>
static void RisseStoreLog<tRisseObjectHashMapLogItemId>(const tRisseObjectHashMapLogItemId & id)
{
	// log item id
	char cid = id;
	RisseObjectHashMapLog->Write(&cid, sizeof(char));
}
//---------------------------------------------------------------------------
template <>
static tRisseObjectHashMapLogItemId RisseRestoreLog<tRisseObjectHashMapLogItemId>()
{
	// restore item id
	char cid;
	if(RisseObjectHashMapLog->Read(&cid, sizeof(char)) != sizeof(char))
		return liiEnd;
	return (tRisseObjectHashMapLogItemId)(cid);
}
//---------------------------------------------------------------------------
struct tRisseObjectHashMapRecord
{
	ttstr History; // call history where the object was created
	ttstr Where; // a label which indicates where the object was created
	ttstr Type; // object type ("class Array" etc)
	risse_uint32 Flags;

	tRisseObjectHashMapRecord() : Flags(RISSE_OHMF_EXIST) {;}

	void StoreLog()
	{
		// store the object into log stream
		RisseStoreLog(History);
		RisseStoreLog(Where);
		RisseStoreLog(Type);
		RisseStoreLog(Flags);
	}

	void RestoreLog()
	{
		// restore the object from log stream
		History = RisseRestoreLog<ttstr>();
		Where   = RisseRestoreLog<ttstr>();
		Type    = RisseRestoreLog<ttstr>();
		Flags   = RisseRestoreLog<risse_uint32>();
	}
};
//---------------------------------------------------------------------------
class tRisseObjectHashMapRecordComparator_HistoryAndType
{
public:
	bool operator () (const tRisseObjectHashMapRecord & lhs,
		const tRisseObjectHashMapRecord & rhs) const
	{
		if(lhs.Where == rhs.Where)
			return lhs.Type < rhs.Type;
		return lhs.History < rhs.History;
	}
};

class tRisseObjectHashMapRecordComparator_Type
{
public:
	bool operator () (const tRisseObjectHashMapRecord & lhs,
		const tRisseObjectHashMapRecord & rhs) const
	{
		return lhs.Type < rhs.Type;
	}
};
//---------------------------------------------------------------------------
class tRisseObjectHashMap;
tRisseObjectHashMap * RisseObjectHashMap;
class tRisseObjectHashMap
{
	typedef
		tRisseHashTable<void *, tRisseObjectHashMapRecord,
			tRisseHashFunc<void *>, 1024>
		tHash;
	tHash Hash;

	risse_int RefCount;


public:
	tRisseObjectHashMap()
	{
		RefCount = 1;
		RisseObjectHashMap = this;
	}

protected:
	~tRisseObjectHashMap()
	{
		RisseObjectHashMap = NULL;
	}

protected:
	void _AddRef() { RefCount ++; }
	void _Release() { if(RefCount == 1) delete this; else RefCount --; }


public:
	static void AddRef()
	{
		if(RisseObjectHashMap)
			RisseObjectHashMap->_AddRef();
		else
			new tRisseObjectHashMap();
	}

	static void Release()
	{
		if(RisseObjectHashMap)
			RisseObjectHashMap->_Release();
	}


	void Add(void * object,
		const tRisseObjectHashMapRecord & record)
	{
		Hash.Add(object, record);
	}

	void SetType(void * object, const ttstr & info)
	{
		tRisseObjectHashMapRecord * rec;
		rec = Hash.Find(object);
		if(!rec) return;
		rec->Type = RisseMapGlobalStringMap(info);
	}

	ttstr GetType(void * object)
	{
		tRisseObjectHashMapRecord * rec;
		rec = Hash.Find(object);
		if(!rec) return ttstr();
		return rec->Type;
	}

	void SetFlag(void * object, risse_uint32 flags_to_change, risse_uint32 bits)
	{
		tRisseObjectHashMapRecord * rec;
		rec = Hash.Find(object);
		if(!rec) return;
		rec->Flags &=  (~flags_to_change);
		rec->Flags |=  (bits & flags_to_change);
	}

	risse_uint32 GetFlag(void * object)
	{
		tRisseObjectHashMapRecord * rec;
		rec = Hash.Find(object);
		if(!rec) return 0;
		return rec->Flags;
	}

	void Remove(void * object)
	{
		Hash.Delete(object);
	}

	void WarnIfObjectIsDeleting(iRisseConsoleOutput * output, void * object)
	{
		tRisseObjectHashMapRecord * rec;
		rec = Hash.Find(object);
		if(!rec) return;

		if(rec->Flags & RISSE_OHMF_DELETING)
		{
			// warn running code on deleting-in-progress object
			ttstr warn(RisseWarning);
			risse_char tmp[64];
			tmp[0] = RISSE_WC('0');
			tmp[1] = RISSE_WC('x');
			Risse_pointer_to_str(object, tmp + 2);

			ttstr info(RisseWarnRunningCodeOnDeletingObject);
			info.Replace(RISSE_WS("%1"), tmp);
			info.Replace(RISSE_WS("%2"), rec->Type);
			info.Replace(RISSE_WS("%3"), rec->Where);
			info.Replace(RISSE_WS("%4"), RisseGetStackTraceString(1));

			output->Print((warn + info).c_str());
		}
	}

	void ReportAllUnfreedObjects(iRisseConsoleOutput * output)
	{
		{
			ttstr msg = (const risse_char *)RisseNObjectsWasNotFreed;
			msg.Replace(RISSE_WS("%1"), ttstr((risse_int)Hash.GetCount()));
			output->Print(msg.c_str());
		}

		// list all unfreed objects
		tHash::tIterator i;
		for(i = Hash.GetFirst(); !i.IsNull(); i++)
		{
			risse_char addr[65];
			addr[0] = RISSE_WC('0');
			addr[1] = RISSE_WC('x');
			Risse_pointer_to_str(i.GetKey(), addr + 2);
			ttstr info = (const risse_char *)RisseObjectWasNotFreed;
			info.Replace(RISSE_WS("%1"), addr);
			info.Replace(RISSE_WS("%2"), i.GetValue().Type);
			info.Replace(RISSE_WS("%3"), i.GetValue().History);
			output->Print(info.c_str());
		}

		// group by the history and object type
		output->Print(RISSE_WS("---"));
		output->Print((const risse_char *)RisseGroupByObjectTypeAndHistory);
		std::vector<tRisseObjectHashMapRecord> items;
		for(i = Hash.GetFirst(); !i.IsNull(); i++)
			items.push_back(i.GetValue());

		std::stable_sort(items.begin(), items.end(),
			tRisseObjectHashMapRecordComparator_HistoryAndType());

		ttstr history, type;
		risse_int count = 0;
		if(items.size() > 0)
		{
			for(std::vector<tRisseObjectHashMapRecord>::iterator i = items.begin();
				; i++)
			{
				if(i != items.begin() &&
					(i == items.end() || history != i->History || type != i->Type))
				{
					ttstr info = (const risse_char *)RisseObjectCountingMessageGroupByObjectTypeAndHistory;
					info.Replace(RISSE_WS("%1"), ttstr(count));
					info.Replace(RISSE_WS("%2"), type);
					info.Replace(RISSE_WS("%3"), history);
					output->Print(info.c_str());

					if(i == items.end()) break;

					count = 0;
				}

				history = i->History;
				type = i->Type;
				count ++;
			}
		}

		// group by object type
		output->Print(RISSE_WS("---"));
		output->Print((const risse_char *)RisseGroupByObjectType);
		std::stable_sort(items.begin(), items.end(),
			tRisseObjectHashMapRecordComparator_Type());

		type.Clear();
		count = 0;
		if(items.size() > 0)
		{
			for(std::vector<tRisseObjectHashMapRecord>::iterator i = items.begin();
				; i++)
			{
				if(i != items.begin() &&
					(i == items.end() || type != i->Type))
				{
					ttstr info = (const risse_char *)RisseObjectCountingMessageRisseGroupByObjectType;
					info.Replace(RISSE_WS("%1"), ttstr(count));
					info.Replace(RISSE_WS("%2"), type);
					output->Print(info.c_str());

					if(i == items.end()) break;

					count = 0;
				}

				type = i->Type;
				count ++;
			}
		}
	}

	bool AnyUnfreed()
	{
		return Hash.GetCount() != 0;
	}

	void WriteAllUnfreedObjectsToLog()
	{
		if(!RisseObjectHashMapLog) return;
			
		tHash::tIterator i;
		for(i = Hash.GetFirst(); !i.IsNull(); i++)
		{
			RisseStoreLog(liiAdd);
			RisseStoreLog(i.GetKey());
			i.GetValue().StoreLog();
		}
	}

	void ReplayLog()
	{
		// replay recorded log
		while(true)
		{
			tRisseObjectHashMapLogItemId id = RisseRestoreLog<tRisseObjectHashMapLogItemId>();
			if(id == liiEnd)          // 00 end of the log
			{
				break;
			}
			else if(id == liiVersion) // 01 identify log structure version
			{
				risse_int v = RisseRestoreLog<risse_int>();
				if(v != RisseVersionHex)
					Risse_eRisseError(RISSE_WS("Object Hash Map log version mismatch"));
			}
			else if(id == liiAdd)     // 02 add a object
			{
				void * object = RisseRestoreLog<void*>();
				tRisseObjectHashMapRecord rec;
				rec.RestoreLog();
				Add(object, rec);
			}
			else if(id == liiRemove)  // 03 remove a object
			{
				void * object = RisseRestoreLog<void*>();
				Remove(object);
			}
			else if(id == liiSetType) // 04 set object type information
			{
				void * object = RisseRestoreLog<void*>();
				ttstr type = RisseRestoreLog<ttstr>();
				SetType(object, type);
			}
			else if(id == liiSetFlag) // 05 set object flag
			{
				void * object = RisseRestoreLog<void*>();
				risse_uint32 flags_to_change = RisseRestoreLog<risse_uint32>();
				risse_uint32 bits = RisseRestoreLog<risse_uint32>();
				SetFlag(object, flags_to_change, bits);
			}
			else
			{
				Risse_eRisseError(RISSE_WS("Currupted Object Hash Map log"));
			}
		}
	}

};
//---------------------------------------------------------------------------
void RisseAddRefObjectHashMap()
{
	tRisseObjectHashMap::AddRef();
}
//---------------------------------------------------------------------------
void RisseReleaseObjectHashMap()
{
	tRisseObjectHashMap::Release();
}
//---------------------------------------------------------------------------
void RisseAddObjectHashRecord(void * object)
{
	if(!RisseObjectHashMap && !RisseObjectHashMapLog) return;

	// create object record and log
	tRisseObjectHashMapRecord rec;
	ttstr hist(RisseGetStackTraceString(4, (const risse_char *)(RisseObjectCreationHistoryDelimiter)));
	if(hist.IsEmpty())
		hist = RisseMapGlobalStringMap((const risse_char *)RisseCallHistoryIsFromOutOfRisseScript);
	rec.History = hist;
	ttstr where(RisseGetStackTraceString(1));
	if(where.IsEmpty())
		where = RisseMapGlobalStringMap((const risse_char *)RisseCallHistoryIsFromOutOfRisseScript);
	rec.Where = where;
	static ttstr InitialType(RISSE_WS("unknown type"));
	rec.Type = InitialType;

	if(RisseObjectHashMap)
	{
		RisseObjectHashMap->Add(object, rec);
	}
	else if(RisseObjectHashMapLog)
	{
		RisseStoreLog(liiAdd);
		RisseStoreLog(object);
		rec.StoreLog();
	}
}
//---------------------------------------------------------------------------
void RisseRemoveObjectHashRecord(void * object)
{
	if(RisseObjectHashMap)
	{
		RisseObjectHashMap->Remove(object);
	}
	else if(RisseObjectHashMapLog)
	{
		RisseStoreLog(liiRemove);
		RisseStoreLog(object);
	}
}
//---------------------------------------------------------------------------
void RisseObjectHashSetType(void * object, const ttstr &type)
{
	if(RisseObjectHashMap)
	{
		RisseObjectHashMap->SetType(object, type);
	}
	else if(RisseObjectHashMapLog)
	{
		RisseStoreLog(liiSetType);
		RisseStoreLog(object);
		RisseStoreLog(type);
	}
}
//---------------------------------------------------------------------------
void RisseSetObjectHashFlag(void *object, risse_uint32 flags_to_change, risse_uint32 bits)
{
	if(RisseObjectHashMap)
	{
		RisseObjectHashMap->SetFlag(object, flags_to_change, bits);
	}
	else if(RisseObjectHashMapLog)
	{
		RisseStoreLog(liiSetFlag);
		RisseStoreLog(object);
		RisseStoreLog(flags_to_change);
		RisseStoreLog(bits);
	}
}
//---------------------------------------------------------------------------
void RisseReportAllUnfreedObjects(iRisseConsoleOutput * output)
{
	if(RisseObjectHashMap) RisseObjectHashMap->ReportAllUnfreedObjects(output);
}
//---------------------------------------------------------------------------
bool RisseObjectHashAnyUnfreed()
{
	if(RisseObjectHashMap) return RisseObjectHashMap->AnyUnfreed();
	return false;
}
//---------------------------------------------------------------------------
void RisseObjectHashMapSetLog(tRisseBinaryStream * stream)
{
	// Set log object. The log file object should not freed until
	// the program (the program is the Process, not RTL nor Risse framework).
	RisseObjectHashMapLog = stream;
	RisseStoreLog(liiVersion);
	RisseStoreLog(RisseVersionHex);
}
//---------------------------------------------------------------------------
void RisseWriteAllUnfreedObjectsToLog()
{
	if(RisseObjectHashMap && RisseObjectHashMapLog) RisseObjectHashMap->WriteAllUnfreedObjectsToLog();
}
//---------------------------------------------------------------------------
void RisseWarnIfObjectIsDeleting(iRisseConsoleOutput * output, void * object)
{
	if(RisseObjectHashMap) RisseObjectHashMap->WarnIfObjectIsDeleting(output, object);
}
//---------------------------------------------------------------------------
void RisseReplayObjectHashMapLog()
{
	if(RisseObjectHashMap && RisseObjectHashMapLog) RisseObjectHashMap->ReplayLog();
}
//---------------------------------------------------------------------------
ttstr RisseGetObjectTypeInfo(void * object)
{
	if(RisseObjectHashMap) return RisseObjectHashMap->GetType(object);
	return ttstr();
}
//---------------------------------------------------------------------------
risse_uint32 RisseGetObjectHashCheckFlag(void * object)
{
	if(RisseObjectHashMap) return RisseObjectHashMap->GetFlag(object);
	return 0;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// StackTracer : stack to trace function call trace
//---------------------------------------------------------------------------
class tRisseStackTracer;
tRisseStackTracer * RisseStackTracer;
//---------------------------------------------------------------------------
struct tRisseStackRecord
{
	tRisseInterCodeContext * Context;
	const risse_int32 * CodeBase;
	risse_int32 * const * CodePtr;
	bool InTry;

	tRisseStackRecord(tRisseInterCodeContext * context, bool in_try)
	{
		CodeBase = NULL;
		CodePtr = NULL;
		InTry = in_try;
		Context = context;
		if(Context) Context->AddRef();
	}

	~tRisseStackRecord()
	{
		if(Context) Context->Release();
	}

	tRisseStackRecord(const tRisseStackRecord & rhs)
	{
		Context = NULL;
		*this = rhs;
	}

	void operator = (const tRisseStackRecord & rhs)
	{
		if(Context != rhs.Context)
		{
			if(Context) Context->Release(), Context = NULL;
			Context = rhs.Context;
			if(Context) Context->AddRef();
		}
		CodeBase = rhs.CodeBase;
		CodePtr = rhs.CodePtr;
		InTry = rhs.InTry;
	}
};

//---------------------------------------------------------------------------
class tRisseStackTracer
{
	std::vector<tRisseStackRecord> Stack;

	risse_int RefCount;

public:
	tRisseStackTracer()
	{
		RefCount = 1;
		RisseStackTracer = this;
	}

protected:
	~tRisseStackTracer()
	{
		RisseStackTracer = NULL;
	}

protected:
	void _AddRef() { RefCount ++; }
	void _Release() { if(RefCount == 1) delete this; else RefCount --; }

public:
	static void AddRef()
	{
		if(RisseStackTracer)
			RisseStackTracer->_AddRef();
		else
			new tRisseStackTracer();
	}

	static void Release()
	{
		if(RisseStackTracer)
			RisseStackTracer->_Release();
	}

	void Push(tRisseInterCodeContext *context, bool in_try)
	{
		Stack.push_back(tRisseStackRecord(context, in_try));
	}

	void Pop()
	{
		Stack.pop_back();
	}

	void SetCodePointer(const risse_int32 * codebase, risse_int32 * const * codeptr)
	{
		risse_uint size = Stack.size();
		if(size < 1) return;
		risse_uint top = size - 1;
		Stack[top].CodeBase = codebase;
		Stack[top].CodePtr = codeptr;
	}

	ttstr GetTraceString(risse_int limit, const risse_char * delimiter)
	{
		// get stack trace string
		if(delimiter == NULL) delimiter = RISSE_WS(" <-- ");

		ttstr ret;
		risse_int top = (risse_int)(Stack.size() - 1);
		while(top >= 0)
		{
			if(!ret.IsEmpty()) ret += delimiter;

			const tRisseStackRecord & rec = Stack[top];
			ttstr str;
			if(rec.CodeBase && rec.CodePtr)
			{
				str = rec.Context->GetPositionDescriptionString(
					*rec.CodePtr - rec.CodeBase);
			}
			else
			{
				str = rec.Context->GetPositionDescriptionString(0);
			}

			ret += str;

			// skip try block stack.
			// 'try { } catch' blocks are implemented as sub-functions
			// in a parent function.
			while(top >= 0 && Stack[top].InTry) top--;

			// check limit
			if(limit)
			{
				limit --;
				if(limit <= 0) break;
			}

			top --;
		}

		return ret;
	}
};
//---------------------------------------------------------------------------
void RisseAddRefStackTracer()
{
	tRisseStackTracer::AddRef();
}
//---------------------------------------------------------------------------
void RisseReleaseStackTracer()
{
	tRisseStackTracer::Release();
}
//---------------------------------------------------------------------------
void RisseStackTracerPush(tRisseInterCodeContext *context, bool in_try)
{
	if(RisseStackTracer)
		RisseStackTracer->Push(context, in_try);
}
//---------------------------------------------------------------------------
void RisseStackTracerSetCodePointer(const risse_int32 * codebase,
	risse_int32 * const * codeptr)
{
	if(RisseStackTracer)
		RisseStackTracer->SetCodePointer(codebase, codeptr);
}
//---------------------------------------------------------------------------
void RisseStackTracerPop()
{
	if(RisseStackTracer)
		RisseStackTracer->Pop();
}
//---------------------------------------------------------------------------
ttstr RisseGetStackTraceString(risse_int limit, const risse_char *delimiter)
{
	if(RisseStackTracer)
		return RisseStackTracer->GetTraceString(limit, delimiter);
	else
		return ttstr();
}
//---------------------------------------------------------------------------





} // namespace Risse

