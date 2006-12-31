//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief コンパイル時の名前空間の管理
//---------------------------------------------------------------------------
#ifndef risseNamespaceH
#define risseNamespaceH

#include "risseVariant.h"
#include <vector>
using namespace std;

namespace Risse
{
//---------------------------------------------------------------------------
class iRisseDispatch;
//---------------------------------------------------------------------------
// tRisseLocalSymbolList
//---------------------------------------------------------------------------
struct tRisseLocalSymbol
{
	risse_char *Name;
};
//---------------------------------------------------------------------------
class tRisseLocalSymbolList
{
	vector<tRisseLocalSymbol *> List;
	risse_int LocalCountStart;
	risse_int *StartWriteAddr;
	risse_int *CountWriteAddr;

public:
	tRisseLocalSymbolList(risse_int LocalCount);
	~tRisseLocalSymbolList(void);

	void SetWriteAddr(risse_int *StartWriteAddr, risse_int *CountWriteAddr);

	void Add(const risse_char * name);
	risse_int Find(const risse_char * name);
	void Remove(const risse_char * name);

	risse_int GetCount(void) const  { return List.size(); }
		// this count includes variable holder that is marked as un-used
	risse_int GetLocalCountStart(void) const  { return LocalCountStart; }

	risse_int * GetStartWriteAddr(void) const { return StartWriteAddr; }
	risse_int * GetCountWriteAddr(void) const { return CountWriteAddr; }
};
//---------------------------------------------------------------------------
// tRisseLocalNamespace
//---------------------------------------------------------------------------
class tRisseLocalNamespace
{
	vector<tRisseLocalSymbolList *> Levels;
	risse_int MaxCount; // max count of local variables
	risse_int CurrentCount; // current local variable count
	risse_int * MaxCountWriteAddr;

public:
	tRisseLocalNamespace(void);
	~tRisseLocalNamespace(void);

	void SetMaxCountWriteAddr(risse_int *MaxCountWriteAddr);

	risse_int GetCount(void);
	risse_int GetMaxCount(void) const { return MaxCount; }
	risse_int Find(const risse_char * name);
	risse_int GetLevel(void);
	void Add(const risse_char * name);
	void Remove(const risse_char *name);
	void Commit(void);

	tRisseLocalSymbolList * GetTopSymbolList();

	void Push(void);
	void Pop(void);

	void Clear(void); // all clear
};
//---------------------------------------------------------------------------
// tRisseLocalNamespaceAutoPushPop
//---------------------------------------------------------------------------
class tRisseLocalNamespaceAutoPushPop
{
	tRisseLocalNamespace *Space;
public:
	tRisseLocalNamespaceAutoPushPop(tRisseLocalNamespace *space)
	{
		Space = space;
		Space->Push();
	}
	~tRisseLocalNamespaceAutoPushPop()
	{
		Space->Pop();
	}
};
//---------------------------------------------------------------------------
// tRisseLocalNamespaceAutoClass
//---------------------------------------------------------------------------
class tRisseLocalNamespaceAutoClass
{
	// create namespace if necessary
	tRisseLocalNamespace *Space;
	bool SpaceCreated;
public:
	tRisseLocalNamespaceAutoClass(tRisseLocalNamespace *space)
	{
		Space = space;
		if(Space == NULL)
		{
			Space = new tRisseLocalNamespace;
			SpaceCreated = true;
		}
		else
		{
			SpaceCreated = false;
		}
		Space->Push();
	}

	~tRisseLocalNamespaceAutoClass()
	{
		Space->Pop();
		if(SpaceCreated) delete Space;
	}

	tRisseLocalNamespace * GetNamespace(void){ return Space; }
};
//---------------------------------------------------------------------------

} // namespace Risse
#endif
