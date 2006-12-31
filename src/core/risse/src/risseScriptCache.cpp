//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief スクリプトブロックキャッシュ
//---------------------------------------------------------------------------

#include "risseCommHead.h"

#include "risse.h"
#include "risseScriptCache.h"
#include "risseScriptBlock.h"

#define RISSE_SCRIPT_CACHE_MAX 64


// currently this object holds only anonymous, single-context expression.

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(47023,63987,11566,16417,14996,65219,54575,63680);
//---------------------------------------------------------------------------
// tRisseScriptCache - a class to cache script blocks
//---------------------------------------------------------------------------
tRisseScriptCache::tRisseScriptCache(tRisse *owner)
	: Cache(RISSE_SCRIPT_CACHE_MAX)
{
	Owner = owner;
}
//---------------------------------------------------------------------------
tRisseScriptCache::~tRisseScriptCache()
{
}
//---------------------------------------------------------------------------
void tRisseScriptCache::ExecScript(const risse_char *script, tRisseVariant *result,
		iRisseDispatch2 * context,
		const risse_char *name, risse_int lineofs)
{
	// currently this does nothing with normal script blocks.
	tRisseScriptBlock *blk = new tRisseScriptBlock(Owner);

	try
	{
		if(name) blk->SetName(name, lineofs);
		blk->SetText(result, script, context, false);
	}
	catch(...)
	{
		blk->Release();
		throw;
	}

	blk->Release();

}
//---------------------------------------------------------------------------
void tRisseScriptCache::ExecScript(const ttstr &script, tRisseVariant *result,
		iRisseDispatch2 * context,
		const ttstr *name, risse_int lineofs)
{
	tRisseScriptBlock *blk = new tRisseScriptBlock(Owner);

	try
	{
		if(name) blk->SetName(name->c_str(), lineofs);
		blk->SetText(result, script.c_str(), context, false);
	}
	catch(...)
	{
		blk->Release();
		throw;
	}

	blk->Release();
}
//---------------------------------------------------------------------------
void tRisseScriptCache::EvalExpression(const risse_char *expression,
	tRisseVariant *result, iRisseDispatch2 * context,
	const risse_char *name, risse_int lineofs)
{
	// currently this works only with anonymous script blocks.
	if(name)
	{
		tRisseScriptBlock *blk = new tRisseScriptBlock(Owner);

		try
		{
			blk->SetName(name, lineofs);
			blk->SetText(result, expression, context, true);
		}
		catch(...)
		{
			blk->Release();
			throw;
		}

		blk->Release();
		return;
	}

	// search through script block cache
	tScriptCacheData data;
	data.Script = expression;
	data.ExpressionMode = true;
	data.MustReturnResult = result != NULL;

	risse_uint32 hash = tScriptCacheHashFunc::Make(data);

	tScriptBlockHolder *holder = Cache.FindAndTouchWithHash(data, hash);

	if(holder)
	{
		// found in cache

		// execute script block in cache
		holder->GetObjectNoAddRef()->ExecuteTopLevelScript(result, context);
		return;
	}

	// not found in cache
	tRisseScriptBlock *blk = new tRisseScriptBlock(Owner);

	try
	{
		blk->SetText(result, expression, context, true);
	}
	catch(...)
	{
		blk->Release();
		throw;
	}

	// add to cache
	if(blk->IsReusable())
	{
		// currently only single-context script block is cached
		tScriptBlockHolder newholder(blk);
		Cache.AddWithHash(data, hash, newholder);
	}

	blk->Release();
	return;
}
//---------------------------------------------------------------------------
void tRisseScriptCache::EvalExpression(const ttstr &expression, tRisseVariant *result,
		iRisseDispatch2 * context,
		const ttstr *name, risse_int lineofs)
{
	// currently this works only with anonymous script blocks.

	// note that this function is basically the same as function above.

	if(name && !name->IsEmpty())
	{
		tRisseScriptBlock *blk = new tRisseScriptBlock(Owner);

		try
		{
			blk->SetName(name->c_str(), lineofs);
			blk->SetText(result, expression.c_str(), context, true);
		}
		catch(...)
		{
			blk->Release();
			throw;
		}

		blk->Release();
		return;
	}

	// search through script block cache
	tScriptCacheData data;
	data.Script = expression;
	data.ExpressionMode = true;
	data.MustReturnResult = result != NULL;

	risse_uint32 hash = tScriptCacheHashFunc::Make(data);

	tScriptBlockHolder *holder = Cache.FindAndTouchWithHash(data, hash);

	if(holder)
	{
		// found in cache

		// execute script block in cache
		holder->GetObjectNoAddRef()->ExecuteTopLevelScript(result, context);
		return;
	}

	// not found in cache
	tRisseScriptBlock *blk = new tRisseScriptBlock(Owner);

	try
	{
		blk->SetText(result, expression.c_str(), context, true);
	}
	catch(...)
	{
		blk->Release();
		throw;
	}

	// add to cache
	if(blk->IsReusable())
	{
		// currently only single-context script block is cached
		tScriptBlockHolder newholder(blk);
		Cache.AddWithHash(data, hash, newholder);
	}

	blk->Release();
	return;
}
//---------------------------------------------------------------------------


} // namespace Risse
