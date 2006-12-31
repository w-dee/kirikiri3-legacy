//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 広域文字列マップの実装
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include "risseGlobalStringMap.h"
#include "risseHashSearch.h"

/*
	Global String Map is a large string hash table, to share the string
	which is already 'known' by the hash table, using Risse string heap
	management mechanism. This will dramatically decrease string heap
	size which is used for constant strings (member names, string literals,
	etc ...).
*/

#define RISSE_GLOBAL_STRING_MAP_SIZE 5000



namespace Risse
{
RISSE_DEFINE_SOURCE_ID(3432,21654,59229,18534,14983,9197,55037,52986);
//---------------------------------------------------------------------------
// tRisseGlobalStringMap - hash map to keep constant strings shared
//---------------------------------------------------------------------------
class tRisseGlobalStringMap;
static tRisseGlobalStringMap * RisseGlobalStringMap = NULL;
struct tRisseEmptyClass {};
class tRisseGlobalStringMap
{
	tRisseHashCache<tRisseString, tRisseEmptyClass, tRisseHashFunc<ttstr>, 1024> Hash;

	risse_int RefCount;

public:
	tRisseGlobalStringMap()  : Hash (RISSE_GLOBAL_STRING_MAP_SIZE)
	{
		RefCount = 1;
		RisseGlobalStringMap = this;
	}

protected:
	~tRisseGlobalStringMap()
	{
		RisseGlobalStringMap = NULL;
	}

public:
	tRisseString _Map(const tRisseString & string)
	{
		// Search Hash, and return the string which to be shared

		const tRisseString * key;
		tRisseEmptyClass * v;

		risse_uint32 hash = tRisseHashFunc<ttstr>::Make(string);

		if(Hash.FindAndTouchWithHash(string, hash, key, v))
		{
			ttstr ret(*key);
			if(ret.GetHint()) *(ret.GetHint()) = hash;
			return ret;
		}
		else
		{
			Hash.AddWithHash(string, hash, tRisseEmptyClass());
			ttstr ret(string);
			if(ret.GetHint()) *(ret.GetHint()) = hash;
			return ret;
		}
	}

protected:
	void _AddRef() { RefCount ++; }
	void _Release() { if(RefCount == 1) delete this; else RefCount --; }

public:
	static void AddRef()
	{
		if(RisseGlobalStringMap)
			RisseGlobalStringMap->_AddRef();
		else
			new tRisseGlobalStringMap();
	}

	static void Release()
	{
		if(RisseGlobalStringMap)
			RisseGlobalStringMap->_Release();
	}

	static ttstr Map(const ttstr & string)
	{
		if(RisseGlobalStringMap)
			return RisseGlobalStringMap->_Map(string);
		else
			return string;
	}
};
//---------------------------------------------------------------------------
void RisseAddRefGlobalStringMap()
{
	tRisseGlobalStringMap::AddRef();
}
//---------------------------------------------------------------------------
void RisseReleaseGlobalStringMap()
{
	tRisseGlobalStringMap::Release();
}
//---------------------------------------------------------------------------
ttstr RisseMapGlobalStringMap(const ttstr & string)
{
	return tRisseGlobalStringMap::Map(string);
}
//---------------------------------------------------------------------------




} // namespace Risse

