//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief エラーメッセージなどの管理
//---------------------------------------------------------------------------
#ifndef risseMessageH
#define risseMessageH

#include "risseVariant.h"
#include "risseString.h"

namespace Risse
{
//---------------------------------------------------------------------------
// this class maps message and its object
//---------------------------------------------------------------------------
extern void RisseAddRefMessageMapper();
extern void RisseReleaseMessageMapper();
class tRisseMessageHolder;
extern void RisseRegisterMessageMap(const risse_char *name, tRisseMessageHolder *holder);
extern void RisseUnregisterMessageMap(const risse_char *name);
extern bool RisseAssignMessage(const risse_char *name, const risse_char *newmsg);
extern ttstr RisseCreateMessageMapString();
ttstr RisseGetMessageMapMessage(const risse_char *name);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// a simple class to hold message
// this holder should be created as a static object
//---------------------------------------------------------------------------
class tRisseMessageHolder
{
	const risse_char *Name;
	const risse_char *DefaultMessage;
	risse_char *AssignedMessage;

public:
	tRisseMessageHolder(const risse_char *name, const risse_char *defmsg, bool regist = true)
	{
		/* "name" and "defmsg" must point static area */
		AssignedMessage = NULL;
		Name = NULL;
		DefaultMessage = defmsg;
		RisseAddRefMessageMapper();
		if(regist)
		{
			Name = name;
			RisseRegisterMessageMap(Name, this);
		}
	}

	~tRisseMessageHolder()
	{
		if(Name) RisseUnregisterMessageMap(Name);
		if(AssignedMessage) delete [] AssignedMessage, AssignedMessage = NULL;
		RisseReleaseMessageMapper();
	}

	void AssignMessage(const risse_char *msg)
	{
		if(AssignedMessage) delete [] AssignedMessage, AssignedMessage = NULL;
		AssignedMessage = new risse_char[Risse_strlen(msg) + 1];
		Risse_strcpy(AssignedMessage, msg);
	}

	operator const risse_char * ()
		{ return AssignedMessage?AssignedMessage:DefaultMessage; }
		/* this function may called after destruction */
};
//---------------------------------------------------------------------------
}

#endif




