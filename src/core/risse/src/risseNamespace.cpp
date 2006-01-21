//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief コンパイル時の名前空間の管理
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include "risseInterface.h"
#include "risseNamespace.h"
namespace Risse
{
RISSE_DEFINE_SOURCE_ID(16765,6763,44155,19343,49075,36506,42875,57529);

//---------------------------------------------------------------------------
// tRisseLocalSymbolList
//---------------------------------------------------------------------------
/*
	symbol list class for compile-time local variables look-up
*/
tRisseLocalSymbolList::tRisseLocalSymbolList(risse_int LocalCountStart)
{
	this->LocalCountStart=LocalCountStart;
	StartWriteAddr = NULL;
	CountWriteAddr = NULL;
}
//---------------------------------------------------------------------------
tRisseLocalSymbolList::~tRisseLocalSymbolList(void)
{
	if(StartWriteAddr)
	{
//		RisseMessageObserver(AnsiString("Start : ") + AnsiString(FLocalCountStart));
		*StartWriteAddr = LocalCountStart;
	}

	if(CountWriteAddr)
	{
		risse_int num = GetCount();
//		RisseMessageObserver(AnsiString("Count : ") + AnsiString(num));
		*CountWriteAddr = num;
	}

	size_t i;
	for(i=0;i<List.size();i++)
	{
		if(List[i]) delete [] List[i]->Name;
		delete List[i];
	}
}
//---------------------------------------------------------------------------
void tRisseLocalSymbolList::SetWriteAddr(risse_int *StartWriteAddr, risse_int *CountWriteAddr)
{
	this->StartWriteAddr = StartWriteAddr;
	this->CountWriteAddr = CountWriteAddr;
}
//---------------------------------------------------------------------------
void tRisseLocalSymbolList::Add(const risse_char * name)
{
	if(Find(name)==-1)
	{
		tRisseLocalSymbol *newsym=new tRisseLocalSymbol;
		newsym->Name=new risse_char[Risse_strlen(name)+1];
		Risse_strcpy(newsym->Name,name);
		size_t i;
		for(i=0;i<List.size();i++)
		{
			tRisseLocalSymbol *sym=List[i];
			if(sym==NULL)
			{
				List[i]=newsym;
				return;
			}
		}
		List.push_back(newsym);
	}
}
//---------------------------------------------------------------------------
risse_int tRisseLocalSymbolList::Find(const risse_char *name)
{
	size_t i;
	for(i=0;i<List.size();i++)
	{
		tRisseLocalSymbol *sym=List[i];
		if(sym)
		{
			if(!Risse_strcmp(sym->Name,name))
				return i;
		}
	}
	return -1;
}
//---------------------------------------------------------------------------
void tRisseLocalSymbolList::Remove(const risse_char *name)
{
	risse_int idx=Find(name);
	if(idx!=-1)
	{
		tRisseLocalSymbol *sym=List[idx];
		delete [] sym->Name;
		delete sym;
		List[idx]=NULL;  // un-used
	}
}
//---------------------------------------------------------------------------
// tRisseLocalNamespace
//---------------------------------------------------------------------------
/*
縲a class for compile-time local variables look-up
*/
#ifdef RISSE_DEBUG_PROFILE_TIME
risse_uint time_ns_Push = 0;
risse_uint time_ns_Pop = 0;
risse_uint time_ns_Find = 0;
risse_uint time_ns_Add = 0;
risse_uint time_ns_Remove = 0;
risse_uint time_ns_Commit = 0;
#endif

//---------------------------------------------------------------------------
tRisseLocalNamespace::tRisseLocalNamespace(void)
{
	MaxCount=0;
	CurrentCount=0;
	MaxCountWriteAddr = NULL;
}
//---------------------------------------------------------------------------
tRisseLocalNamespace::~tRisseLocalNamespace(void)
{
	if(MaxCountWriteAddr)
	{
//		RisseMessageObserver(AnsiString("Max count : ")+AnsiString(FMaxCount));
		*MaxCountWriteAddr = MaxCount;
	}
}
//---------------------------------------------------------------------------
void tRisseLocalNamespace::SetMaxCountWriteAddr(risse_int * MaxCountWriteAddr)
{
	this->MaxCountWriteAddr = MaxCountWriteAddr;
}
//---------------------------------------------------------------------------
risse_int tRisseLocalNamespace::GetCount(void)
{
	risse_int count=0;
	size_t i;
	for(i=0;i<Levels.size();i++)
	{
		tRisseLocalSymbolList * list= Levels[i];
		count+= list->GetCount();
	}
	return count;
}
//---------------------------------------------------------------------------
void tRisseLocalNamespace::Push()
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_ns_Push);
#endif
	CurrentCount=GetCount();
	tRisseLocalSymbolList * list=new tRisseLocalSymbolList(CurrentCount);
	Levels.push_back(list);
}
//---------------------------------------------------------------------------
void tRisseLocalNamespace::Pop(void)
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_ns_Pop);
#endif
	tRisseLocalSymbolList * list= Levels[Levels.size()-1];

	Commit();

	CurrentCount= list->GetLocalCountStart();

	Levels.pop_back();

	delete list;
}
//---------------------------------------------------------------------------
risse_int tRisseLocalNamespace::Find(const risse_char *name)
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_ns_Find);
#endif
	// search "name"
	risse_int i;  /* signed */
	for(i=Levels.size()-1; i>=0; i--)
	{
		tRisseLocalSymbolList* list = Levels[i];
		risse_int lidx = list->Find(name);
		if(lidx!=-1)
		{
			lidx += list->GetLocalCountStart();
			return lidx;
		}
	}
	return -1;
}
//---------------------------------------------------------------------------
risse_int tRisseLocalNamespace::GetLevel(void)
{
	// gets current namespace depth
	return Levels.size();
}
//---------------------------------------------------------------------------
void tRisseLocalNamespace::Add(const risse_char * name)
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_ns_Add);
#endif
	// adds "name" to namespace
	tRisseLocalSymbolList * top = GetTopSymbolList();
	if(!top) return; // this is global
	top->Add(name);
}
//---------------------------------------------------------------------------
void tRisseLocalNamespace::Remove(const risse_char *name)
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_ns_Remove);
#endif
	risse_int i;
	for(i=Levels.size()-1; i>=0; i--)
	{
		tRisseLocalSymbolList* list = Levels[i];
		risse_int lidx = list->Find(name);
		if(lidx!=-1)
		{
			list->Remove(name);
			return;
		}
	}
}
//---------------------------------------------------------------------------
void tRisseLocalNamespace::Commit(void)
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_ns_Commit);
#endif
	// compute MaxCount
	risse_int i;
	risse_int count = 0;
	for(i=Levels.size()-1; i>=0; i--)
	{
		tRisseLocalSymbolList* list = Levels[i];
		count += list->GetCount();
	}
	if(MaxCount < count) MaxCount = count;
}
//---------------------------------------------------------------------------
tRisseLocalSymbolList * tRisseLocalNamespace::GetTopSymbolList()
{
	// returns top symbol list
	if(Levels.size() == 0) return NULL;
	return (tRisseLocalSymbolList *)(Levels[Levels.size()-1]);
}
//---------------------------------------------------------------------------
void tRisseLocalNamespace::Clear(void)
{
	// all clear
	while(Levels.size()) Pop();
}
//---------------------------------------------------------------------------

} // namespace Risse

