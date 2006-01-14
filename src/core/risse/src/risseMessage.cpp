//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief エラーメッセージなどの管理
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include "risseMessage.h"
#include "risseHashSearch.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(1021);
//---------------------------------------------------------------------------
// tRisseMessageMapper class
//---------------------------------------------------------------------------
class tRisseMessageMapper
{
	tRisseHashTable<ttstr, tRisseMessageHolder*> Hash;
	risse_uint RefCount;

public:
	tRisseMessageMapper() {;}
	~tRisseMessageMapper() {;}

	void Register(const risse_char *name, tRisseMessageHolder *holder)
	{
		Hash.Add(ttstr(name), holder);
	}

	void Unregister(const risse_char *name)
	{
		Hash.Delete(ttstr(name));
	}

	bool AssignMessage(const risse_char *name, const risse_char *newmsg)
	{
		tRisseMessageHolder **holder = Hash.Find(ttstr(name));
		if(holder)
		{
			(*holder)->AssignMessage(newmsg);
			return true;
		}
		return false;
	}

	bool Get(const risse_char *name, ttstr &str)
	{
		tRisseMessageHolder **holder = Hash.Find(ttstr(name));
		if(holder)
		{
			str = (const risse_char *)(**holder);
			return true;
		}
		return false;
	}

	ttstr CreateMessageMapString();

} static * RisseMessageMapper = NULL;
static int RisseMessageMapperRefCount = 0;
//---------------------------------------------------------------------------
ttstr tRisseMessageMapper::CreateMessageMapString()
{
	ttstr script;
	tRisseHashTable<ttstr, tRisseMessageHolder*>::tIterator i;
	for(i = Hash.GetLast(); !i.IsNull(); i--)
	{
		ttstr name = i.GetKey();
		tRisseMessageHolder *holder = i.GetValue();
		script += RISSE_WS("\tr(\"");
		script += name.EscapeC();
		script += RISSE_WS("\", \"");
		script += ttstr((const risse_char *)(*holder)).EscapeC();
#ifdef RISSE_TEXT_OUT_CRLF
		script += RISSE_WS("\");\r\n");
#else
		script += RISSE_WS("\");\n");
#endif
	}
	return script;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void RisseAddRefMessageMapper()
{
	if(RisseMessageMapper)
	{
		RisseMessageMapperRefCount++;
	}
	else
	{
		RisseMessageMapper = new tRisseMessageMapper;
		RisseMessageMapperRefCount = 1;
	}
}
//---------------------------------------------------------------------------
void RisseReleaseMessageMapper()
{
	if(RisseMessageMapper)
	{
		RisseMessageMapperRefCount--;
		if(RisseMessageMapperRefCount == 0)
		{
			delete RisseMessageMapper;
			RisseMessageMapper = NULL;
		}
	}
}
//---------------------------------------------------------------------------
void RisseRegisterMessageMap(const risse_char *name, tRisseMessageHolder *holder)
{
	if(RisseMessageMapper) RisseMessageMapper->Register(name, holder);
}
//---------------------------------------------------------------------------
void RisseUnregisterMessageMap(const risse_char *name)
{
	if(RisseMessageMapper) RisseMessageMapper->Unregister(name);
}
//---------------------------------------------------------------------------
bool RisseAssignMessage(const risse_char *name, const risse_char *newmsg)
{
	if(RisseMessageMapper) return RisseMessageMapper->AssignMessage(name, newmsg);
	return false;
}
//---------------------------------------------------------------------------
ttstr RisseCreateMessageMapString()
{
	if(RisseMessageMapper) return RisseMessageMapper->CreateMessageMapString();
	return RISSE_WS("");
}
//---------------------------------------------------------------------------
ttstr RisseGetMessageMapMessage(const risse_char *name)
{
	if(RisseMessageMapper)
	{
		ttstr ret;
		if(RisseMessageMapper->Get(name, ret)) return ret;
		return ttstr();
	}
	return ttstr();
}
//---------------------------------------------------------------------------
}

