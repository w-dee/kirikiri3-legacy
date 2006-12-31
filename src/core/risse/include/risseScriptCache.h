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

#ifndef risseScriptCacheH
#define risseScriptCacheH

#include "risseConfig.h"
#include "risseHashSearch.h"
#include "risseUtils.h"


namespace Risse
{

//---------------------------------------------------------------------------
// tRisseScriptCache - a class to cache script blocks
//---------------------------------------------------------------------------
class tRisse;
class tRisseScriptCache
{
private:
	struct tScriptCacheData
	{
		ttstr Script;
		bool ExpressionMode;
		bool MustReturnResult;

		bool operator ==(const tScriptCacheData &rhs) const
		{
			return Script == rhs.Script && ExpressionMode == rhs.ExpressionMode &&
				MustReturnResult == rhs.MustReturnResult;
		}
	};

	class tScriptCacheHashFunc
	{
	public:
		static risse_uint32 Make(const tScriptCacheData &val)
		{
			risse_uint32 v = tRisseHashFunc<ttstr>::Make(val.Script);
			v ^= val.ExpressionMode;
			v ^= val.MustReturnResult;
			return v;
		}
	};

	tRisse *Owner;

	typedef tRisseRefHolder<tRisseScriptBlock> tScriptBlockHolder;

	typedef tRisseHashCache<tScriptCacheData, tScriptBlockHolder, tScriptCacheHashFunc>
		tCache;

	tCache Cache;

public:
	tRisseScriptCache(tRisse *owner);
	virtual ~tRisseScriptCache();


public:
	void ExecScript(const risse_char *script, tRisseVariant *result,
		iRisseDispatch2 * context,
		const risse_char *name, risse_int lineofs);

	void ExecScript(const ttstr &script, tRisseVariant *result,
		iRisseDispatch2 * context,
		const ttstr *name, risse_int lineofs);


public:

	void EvalExpression(const risse_char *expression, tRisseVariant *result,
		iRisseDispatch2 * context,
		const risse_char *name, risse_int lineofs);

	void EvalExpression(const ttstr &expression, tRisseVariant *result,
		iRisseDispatch2 * context,
		const ttstr *name, risse_int lineofs);
};
//---------------------------------------------------------------------------

}
#endif


