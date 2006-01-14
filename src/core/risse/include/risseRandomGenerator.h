//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Math.RandomGenerator クラス実装
//---------------------------------------------------------------------------

#ifndef risseRandomGeneratorH
#define risseRandomGeneratorH

#include <time.h>
#include "risseNative.h"
#include "risseMT19937ar-cok.h"

namespace Risse
{
//---------------------------------------------------------------------------
extern void (*RisseGetRandomBits128)(void *dest);
    // retrives 128-bits (16bytes) random bits for random seed.
    // this can be override application-specified routine, otherwise
    // Risse uses current time as a random seed.
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
class tRisseNI_RandomGenerator : public tRisseNativeInstance
{
public:
	tRisseNI_RandomGenerator();
    ~tRisseNI_RandomGenerator();
private:
	tRisseMersenneTwister *Generator;

public:
	iRisseDispatch2 * Serialize();

	void Randomize(tRisseVariant ** param, risse_int numparams);
	double Random();
	risse_uint32 Random32();
	risse_int64 Random63();
	risse_int64 Random64();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
class tRisseNC_RandomGenerator : public tRisseNativeClass
{
public:
	tRisseNC_RandomGenerator();

	static risse_uint32 ClassID;

private:
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif
