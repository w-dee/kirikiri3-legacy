//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2005	 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Math.RandomGenerator クラス実装
//---------------------------------------------------------------------------
#include "risseCommHead.h"


#include <time.h>
#include "risseError.h"
#include "risseRandomGenerator.h"
#include "risseDictionary.h"
#include "risseLex.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(47172,40016,36657,19844,14753,2903,55023,7986);
//---------------------------------------------------------------------------
void (*RisseGetRandomBits128)(void *dest) = NULL;
	// retrives 128-bits (16bytes) random bits for random seed.
	// this can be override application-specified routine, otherwise
	// Risse uses current time as a random seed.
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseNI_RandomGenerator : risse Native Instance : RandomGenerator
//---------------------------------------------------------------------------
tRisseNI_RandomGenerator::tRisseNI_RandomGenerator()
{
	// C++ constructor
	Generator = NULL;
}
//---------------------------------------------------------------------------
tRisseNI_RandomGenerator::~tRisseNI_RandomGenerator()
{
	// C++ destructor
	if(Generator) delete Generator;
}
//---------------------------------------------------------------------------
void tRisseNI_RandomGenerator::Randomize(tRisseVariant ** param, risse_int numparams)
{
	if(numparams == 0)
	{
		// parametor not given
		if(RisseGetRandomBits128)
		{
			// another random generator is given
			risse_uint8 buf[32];
			unsigned long tmp[32];
			RisseGetRandomBits128(buf);
			RisseGetRandomBits128(buf+16);
			for(risse_int i = 0; i < 32; i++)
				tmp[i] = buf[i] + (buf[i] << 8) +
				(buf[1] << 16) + (buf[i] << 24);

			if(Generator) delete Generator, Generator = NULL;
			Generator = new tRisseMersenneTwister(tmp, 32);
		}
		else
		{
			time_t tm;

			if(Generator) delete Generator, Generator = NULL;
			Generator = new tRisseMersenneTwister((unsigned long)time(&tm));
		}
	}
	else if(numparams >= 1)
	{
		if(param[0]->Type() == tvtObject)
		{
			tRisseMersenneTwisterData *data = NULL;
			try
			{
				// may be a reconstructible information
				tRisseVariantClosure clo = param[0]->AsObjectClosureNoAddRef();
				if(!clo.Object) Risse_eRisseError(RisseNullAccess);


				ttstr state;
				tRisseVariant val;

				data = new tRisseMersenneTwisterData;

				// get state array
				RISSE_THROW_IF_ERROR(clo.PropGet(RISSE_MEMBERMUSTEXIST, RISSE_WS("state"),
					NULL, &val, NULL));

				state = val;
				if(state.GetLen() != RISSE_MT_N * 8)
					Risse_eRisseError(RisseNotReconstructiveRandomizeData);

				const risse_char *p = state.c_str();

				for(risse_int i = 0; i < RISSE_MT_N; i++)
				{
					risse_uint32 n = 0;
					risse_int tmp;

					for(risse_int j = 0; j < 8; j++)
					{
						tmp = RisseHexNum(p[j]);
						if(tmp == -1)
							Risse_eRisseError(RisseNotReconstructiveRandomizeData);
						else
							n <<= 4, n += tmp;
					}

					p += 8;
					data->state[i] = (unsigned long)(n);
				}

				// get other members
				RISSE_THROW_IF_ERROR(clo.PropGet(RISSE_MEMBERMUSTEXIST, RISSE_WS("left"), NULL,
					&val, NULL));
				data->left = (risse_int)val;

				RISSE_THROW_IF_ERROR(clo.PropGet(RISSE_MEMBERMUSTEXIST, RISSE_WS("next"), NULL,
					&val, NULL));
				data->next = (risse_int)val + data->state;

				if(Generator) delete Generator, Generator = NULL;
				Generator = new tRisseMersenneTwister(*data);
			}
			catch(...)
			{
				if(data) delete data;
				Risse_eRisseError(RisseNotReconstructiveRandomizeData);
			}
			delete data;
		}
		else
		{
			risse_uint64 n = (risse_int64)*param[0];
			unsigned long tmp[2];
			tmp[0] = (unsigned long)n;
			tmp[1] = (unsigned long)(n>>32);
  			if(Generator) delete Generator, Generator = NULL;
			Generator = new tRisseMersenneTwister(tmp, 2);
		}
	}

}
//---------------------------------------------------------------------------
iRisseDispatch2 * tRisseNI_RandomGenerator::Serialize()
{
	// create dictionary object which has reconstructible information
	// which can be passed into constructor or randomize method.
	if(!Generator) return NULL;


	iRisseDispatch2 * dic = NULL;
	tRisseVariant val;

	// retrive tRisseMersenneTwisterData
	const tRisseMersenneTwisterData & data = Generator->GetData();

	try
	{
		// create 'state' string
		ttstr state;
		risse_char *p = state.AllocBuffer(RISSE_MT_N * 8);
		for(risse_int i = 0; i < RISSE_MT_N; i++)
		{
			const risse_char* hex = RISSE_WS("0123456789abcdef");
			p[0] = hex[(data.state[i]  >> 28) & 0x000f];
			p[1] = hex[(data.state[i]  >> 24) & 0x000f];
			p[2] = hex[(data.state[i]  >> 20) & 0x000f];
			p[3] = hex[(data.state[i]  >> 16) & 0x000f];
			p[4] = hex[(data.state[i]  >> 12) & 0x000f];
			p[5] = hex[(data.state[i]  >>  8) & 0x000f];
			p[6] = hex[(data.state[i]  >>  4) & 0x000f];
			p[7] = hex[(data.state[i]  >>  0) & 0x000f];
			p += 8;
		}
		state.FixLen();

		// create dictionary and store information
		dic = RisseCreateDictionaryObject();

		val = state;
		dic->PropSet(RISSE_MEMBERENSURE, RISSE_WS("state"), NULL, &val, dic);

		val = (tTVInteger)data.left;
		dic->PropSet(RISSE_MEMBERENSURE, RISSE_WS("left"), NULL, &val, dic);

		val = (tTVInteger)(data.next - data.state);
		dic->PropSet(RISSE_MEMBERENSURE, RISSE_WS("next"), NULL, &val, dic);
	}
	catch(...)
	{
		if(dic) dic->Release();
		throw;
	}
	return dic;
}
//---------------------------------------------------------------------------
double tRisseNI_RandomGenerator::Random()
{
	// returns double precision random value x, x is in 0 <= x < 1
	if(!Generator) return 0;
	return Generator->rand_double();

}
//---------------------------------------------------------------------------
risse_uint32 tRisseNI_RandomGenerator::Random32()
{
	// returns 63 bit integer random value
	if(!Generator) return 0;

	return Generator->int32();;
}
//---------------------------------------------------------------------------
risse_int64 tRisseNI_RandomGenerator::Random63()
{
	// returns 63 bit integer random value
	if(!Generator) return 0;

	risse_uint64 v;
	((risse_uint32 *)&v)[0]   = Generator->int32();
	((risse_uint32 *)&v)[1]   = Generator->int32();

	return v & RISSE_UI64_VAL(0x7fffffffffffffff);
}
//---------------------------------------------------------------------------
risse_int64 tRisseNI_RandomGenerator::Random64()
{
	// returns 64 bit integer random value
	if(!Generator) return 0;

	risse_uint64 v;
	((risse_uint32 *)&v)[0]   = Generator->int32();
	((risse_uint32 *)&v)[1]   = Generator->int32();

	return v;
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
// tRisseNC_RandomGenerator : risse Native Class : RandomGenerator
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_RandomGenerator::ClassID = (risse_uint32)-1;
tRisseNC_RandomGenerator::tRisseNC_RandomGenerator() :
	tRisseNativeClass(RISSE_WS("RandomGenerator"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/RandomGenerator)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tRisseNI_RandomGenerator,
	/*Risse class name*/ RandomGenerator)
{
	_this->Randomize(param, numparams);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/RandomGenerator)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/randomize)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RandomGenerator);

	_this->Randomize(param, numparams);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/randomize)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/random)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RandomGenerator);

	// returns 53-bit precision random value x, x is in 0 <= x < 1

	if(result)
		*result = _this->Random();
	else
		_this->Random(); // discard result

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/random)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/random32)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RandomGenerator);

	// returns 32-bit precision integer value x, x is in 0 <= x <= 4294967295

	if(result)
		*result = (risse_int64)_this->Random32();
	else
		_this->Random32(); // discard result

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/random32)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/random63)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RandomGenerator);

	// returns 63-bit precision integer value x, x is in 0 <= x <= 9223372036854775807

	if(result)
		*result = _this->Random63();
	else
		_this->Random63(); // discard result

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/random63)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/random64)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RandomGenerator);

	// returns 64-bit precision integer value x, x is in
	// -9223372036854775808 <= x <= 9223372036854775807

	if(result)
		*result = _this->Random64();
	else
		_this->Random64(); // discard result

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/random64)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/serialize)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RandomGenerator);

	if(result)
	{
		iRisseDispatch2 * dsp = _this->Serialize();
		*result = tRisseVariant(dsp, dsp);
		dsp->Release();
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/serialize)
//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseNativeInstance *tRisseNC_RandomGenerator::CreateNativeInstance()
{
	return new tRisseNI_RandomGenerator();
}
//---------------------------------------------------------------------------
} // namespace Risse

//---------------------------------------------------------------------------

