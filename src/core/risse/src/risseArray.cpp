//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 配列クラスの実装
//---------------------------------------------------------------------------


#include "risseCommHead.h"

#include <algorithm>
#include <functional>
#include "risseArray.h"
#include "risseDictionary.h"
#include "risseUtils.h"
#include "risseCharUtils.h"

#ifndef RISSE_NO_REGEXP
#include "risseRegExp.h"
#endif

// TODO: Check the deque's exception handling on deleting


#define RISSE_ARRAY_BASE_HASH_BITS 3
	/* hash bits for base "Object" hash */



namespace Risse
{
RISSE_DEFINE_SOURCE_ID(1385,7542,2130,16964,48293,12395,14019,40094);
//---------------------------------------------------------------------------
static risse_int32 ClassID_Array;
//---------------------------------------------------------------------------
static bool inline Risse_iswspace(risse_char ch)
{
	// the standard iswspace misses when non-zero page code
	if(ch&0xff00) return false; else return isspace(ch);
}
//---------------------------------------------------------------------------
static bool inline Risse_iswdigit(risse_char ch)
{
	// the standard iswdigit misses when non-zero page code
	if(ch&0xff00) return false; else return isdigit(ch);
}
//---------------------------------------------------------------------------
// Utility Function(s)
//---------------------------------------------------------------------------
static bool IsNumber(const risse_char *str, risse_int &result)
{
	// when str indicates a number, this function converts it to
	// number and put to result, and returns true.
	// otherwise returns false.
	if(!str) return false;
	const risse_char *orgstr = str;

	if(!*str) return false;
	while(*str && Risse_iswspace(*str)) str++;
	if(!*str) return false;
	if(*str == RISSE_WC('-')) str++; // sign
	if(!*str) return false;
	while(*str && Risse_iswspace(*str)) str++;
	if(!*str) return false;
	if(!Risse_iswdigit(*str)) return false;
	while(*str && Risse_iswdigit(*str)) str++;
	while(*str && Risse_iswspace(*str)) str++;
	if(*str == 0)
	{
		result = Risse_atoi(orgstr);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseStringAppender
//---------------------------------------------------------------------------
#define RISSE_STRINGAPPENDER_DATA_INC 8192
tRisseStringAppender::tRisseStringAppender()
{
	Data = NULL;
	DataLen = 0;
	DataCapacity = 0;
}
//---------------------------------------------------------------------------
tRisseStringAppender::~tRisseStringAppender()
{
	if(Data) RisseVS_free(Data);
}
//---------------------------------------------------------------------------
void tRisseStringAppender::Append(const risse_char *string, risse_int len)
{
	if(!Data)
	{
		if(len > 0)
		{
			Data = RisseVS_malloc(DataCapacity = len * sizeof(risse_char));
			memcpy(Data, string, DataCapacity);
			DataLen = len;
		}
	}
	else
	{
		if(DataLen + len > DataCapacity)
		{
			DataCapacity = DataLen + len + RISSE_STRINGAPPENDER_DATA_INC;
			Data = RisseVS_realloc(Data, DataCapacity * sizeof(risse_char));
		}
		memcpy(Data + DataLen, string, len * sizeof(risse_char));
		DataLen += len;
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// tRisseArraySortCompare  : a class for comarison operator
//---------------------------------------------------------------------------
class tRisseArraySortCompare_NormalAscending :
	public std::binary_function<const tRisseVariant &, const tRisseVariant &, bool>
{
public:
	result_type operator () (first_argument_type lhs, second_argument_type rhs) const
	{
		return lhs < rhs;
	}
};
class tRisseArraySortCompare_NormalDescending :
	public std::binary_function<const tRisseVariant &, const tRisseVariant &, bool>
{
public:
	result_type operator () (first_argument_type lhs, second_argument_type rhs) const
	{
		return lhs > rhs;
	}
};
class tRisseArraySortCompare_NumericAscending :
	public std::binary_function<const tRisseVariant &, const tRisseVariant &, bool>
{
public:
	result_type operator () (first_argument_type lhs, second_argument_type rhs) const
	{
		if(lhs.Type() == tvtString && rhs.Type() == tvtString)
		{
			tRisseVariant ltmp(lhs), rtmp(rhs);
			ltmp.tonumber();
			rtmp.tonumber();
			return ltmp < rtmp;
		}
		return lhs < rhs;
	}
};
class tRisseArraySortCompare_NumericDescending :
	public std::binary_function<const tRisseVariant &, const tRisseVariant &, bool>
{
public:
	result_type operator () (first_argument_type lhs, second_argument_type rhs) const
	{
		if(lhs.Type() == tvtString && rhs.Type() == tvtString)
		{
			tRisseVariant ltmp(lhs), rtmp(rhs);
			ltmp.tonumber();
			rtmp.tonumber();
			return ltmp > rtmp;
		}
		return lhs > rhs;
	}
};
class tRisseArraySortCompare_StringAscending :
	public std::binary_function<const tRisseVariant &, const tRisseVariant &, bool>
{
public:
	result_type operator () (first_argument_type lhs, second_argument_type rhs) const
	{
		if(lhs.Type() == tvtString && rhs.Type() == tvtString)
			return lhs < rhs;
		return (ttstr)lhs < (ttstr)rhs;
	}
};
class tRisseArraySortCompare_StringDescending :
	public std::binary_function<const tRisseVariant &, const tRisseVariant &, bool>
{
public:
	result_type operator () (first_argument_type lhs, second_argument_type rhs) const
	{
		if(lhs.Type() == tvtString && rhs.Type() == tvtString)
			return lhs > rhs;
		return (ttstr)lhs > (ttstr)rhs;
	}
};
class tRisseArraySortCompare_Functional :
	public std::binary_function<const tRisseVariant &, const tRisseVariant &, bool>
{
	tRisseVariantClosure Closure;
public:
	tRisseArraySortCompare_Functional(const tRisseVariantClosure &clo) :
		Closure(clo)
	{
	}

	result_type operator () (first_argument_type lhs, second_argument_type rhs) const
	{

		tRisseVariant result;

		risse_error hr;
		tRisseVariant *param[] = {
			const_cast<tRisseVariant *>(&lhs), // note that doing cast to non-const pointer
			const_cast<tRisseVariant *>(&rhs) };

		hr = Closure.FuncCall(0, NULL, NULL, &result, 2, param, NULL);

		if(RISSE_FAILED(hr))
			RisseThrowFrom_risse_error(hr);

		return result.operator bool();
	}
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tRisseArrayClass : tRisseArray class
//---------------------------------------------------------------------------
risse_uint32 tRisseArrayClass::ClassID = (risse_uint32)-1;
tRisseArrayClass::tRisseArrayClass() :
	tRisseNativeClass(RISSE_WS("Array"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/* Risse class name */Array)

//---------------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/* var. name */_this, /* var. type */tRisseArrayNI,
	/* Risse class name */ Array)
{
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/Array)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func. name */load)
{
	// loads a file into this array.
	// each a line becomes an array element.

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	ttstr name(*param[0]);
	ttstr mode;
	if(numparams >= 2 && param[1]->Type() != tvtVoid) mode =*param[1];

	iRisseTextReadStream * stream = RisseCreateTextStreamForRead(name, mode);
	try
	{
		ni->Items.clear();
		ttstr content;
		stream->Read(content, 0);
		const risse_char * p = content.c_str();
		const risse_char * sp = p;
		risse_uint l;

		// count lines
		risse_uint lines = 0;
		while(*p)
		{
			if(*p == RISSE_WC('\r') || *p == RISSE_WC('\n'))
			{
				p++;
				if(p[-1] == RISSE_WC('\r') && p[0] == RISSE_WC('\n')) p++;

				lines++;

				sp = p;
				continue;
			}
			p++;
		}

		l = p - sp;
		if(l)
		{
			lines++;
		}

		ni->Items.resize(lines);

		// split to each line
		p = content.c_str();
		sp = p;
		lines = 0;

		tRisseVariantString *vs;

		try
		{
			while(*p)
			{
				if(*p == RISSE_WC('\r') || *p == RISSE_WC('\n'))
				{
					risse_uint l = p - sp;

					p++;
					if(p[-1] == RISSE_WC('\r') && p[0] == RISSE_WC('\n')) p++;

					vs = RisseAllocVariantString(sp, l);
					ni->Items[lines++] = vs;
					if(vs) vs->Release(), vs = NULL;

					sp = p;
					continue;
				}
				p++;
			}

			l = p - sp;
			if(l)
			{
				vs = RisseAllocVariantString(sp, l);
				ni->Items[lines] = vs;
				if(vs) vs->Release(), vs = NULL;
			}
		}
		catch(...)
		{
			if(vs) vs->Release();
			throw;
		}


	}
	catch(...)
	{
		stream->Destruct();
		throw;
	}
	stream->Destruct();

	if(result) *result = tRisseVariant(objthis, objthis);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */load)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func. name */save)
{
	// saves the array into a file.
	// only string and number stuffs are stored.

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	ttstr name(*param[0]);
	ttstr mode;
	if(numparams >= 2 && param[1]->Type() != tvtVoid) mode = *param[1];
	iRisseTextWriteStream * stream = RisseCreateTextStreamForWrite(name, mode);
	try
	{
		tRisseArrayNI::tArrayItemIterator i = ni->Items.begin();
#ifdef RISSE_TEXT_OUT_CRLF
		const static ttstr cr(RISSE_WS("\r\n"));
#else
		const static ttstr cr(RISSE_WS("\n"));
#endif

		while( i != ni->Items.end())
		{
			tRisseVariantType type = i->Type();
			if(type == tvtString || type == tvtInteger || type == tvtReal)
			{
				stream->Write(*i);
			}
			stream->Write(cr);
			i++;
		}
	}
	catch(...)
	{
		stream->Destruct();
		throw;
	}
	stream->Destruct();

	if(result) *result = tRisseVariant(objthis, objthis);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */save)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func. name */saveStruct)
{
	// saves the array into a file, that can be interpret as an expression.

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	ttstr name(*param[0]);
	ttstr mode;
	if(numparams >= 2 && param[1]->Type() != tvtVoid) mode = *param[1];

	iRisseTextWriteStream * stream = RisseCreateTextStreamForWrite(name, mode);
	try
	{
		std::vector<iRisseDispatch2 *> stack;
		stack.push_back(objthis);
		tRisseStringAppender string;
		ni->SaveStructuredData(stack, string, RISSE_WS(""));
		stream->Write(ttstr(string.GetData(), string.GetLen()));
	}
	catch(...)
	{
		stream->Destruct();
		throw;
	}
	stream->Destruct();

	if(result) *result = tRisseVariant(objthis, objthis);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */saveStruct)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */split)
{
	// split string with given delimiters.

	// arguments are : <pattern/delimiter>, <string>, [<reserved>],
	// [<whether ignore empty element>]

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 2) return RISSE_E_BADPARAMCOUNT;

	ni->Items.resize(0);
	tRisseString string = *param[1];
	bool purgeempty = false;

	if(numparams >= 4 && param[3]->Type() != tvtVoid)
		purgeempty = param[3]->operator bool();

#ifndef RISSE_NO_REGEXP
	if(param[0]->Type() == tvtObject)
	{
		tRisseNI_RegExp * re = NULL;
		tRisseVariantClosure clo = param[0]->AsObjectClosureNoAddRef();
		if(clo.Object)
		{
			if(RISSE_SUCCEEDED(clo.Object->NativeInstanceSupport(
				RISSE_NIS_GETINSTANCE,
				tRisseNC_RegExp::ClassID, (iRisseNativeInstance**)&re)))
			{
				// param[0] is regexp
				iRisseDispatch2 *array = objthis;
				re->Split(&array, string, purgeempty);

				if(result) *result = tRisseVariant(objthis, objthis);

				return RISSE_S_OK;
			}
		}
	}
#endif

	tRisseString pattern = *param[0];


	// split with delimiter
	const risse_char *s = string.c_str();
	const risse_char *delim = pattern.c_str();
	const risse_char *sstart = s;
	while(*s)
	{
		if(Risse_strchr(delim, *s) != NULL)
		{
			// delimiter found
			if(!purgeempty || (purgeempty && (s-sstart)!=0) )
			{
				ni->Items.push_back(tRisseString(sstart, s-sstart));
			}
			s++;
			sstart = s;
		}
		else
		{
			s++;
		}
	}

	if(!purgeempty || (purgeempty && (s-sstart)!=0) )
	{
		ni->Items.push_back(tRisseString(sstart, s-sstart));
	}

	if(result) *result = tRisseVariant(objthis, objthis);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */split)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */join)
{
	// join string with given delimiters.

	// arguments are : <delimiter>, [<reserved>],
	// [<whether ignore empty element>]

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	tRisseString delimiter = *param[0];

	bool purgeempty = false;

	if(numparams >= 3 && param[2]->Type() != tvtVoid)
		purgeempty = param[2]->operator bool();

	// join with delimiter
	bool first = true;
	tRisseString out;
	tRisseArrayNI::tArrayItemIterator i;
	for(i = ni->Items.begin(); i != ni->Items.end(); i++)
	{
		if(purgeempty && i->Type() == tvtVoid) continue;

		if(!first) out += delimiter;
		first = false;
		out += (tRisseString)(*i);
	}

	if(result) *result = out;

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */join)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */sort)
{
	// sort array items.

	// arguments are : [<sort order/comparison function>], [<whether to do stable sort>]

	// sort order is one of:
	// '+' (default)   :  Normal ascending  (comparison by tRisseVariant::operator < )
	// '-'             :  Normal descending (comparison by tRisseVariant::operator < )
	// '0'             :  Numeric value ascending
	// '9'             :  Numeric value descending
	// 'a'             :  String ascending
	// 'z'             :  String descending

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	char method = '+';
	bool do_stable_sort = false;
	tRisseVariantClosure closure;

	if(numparams >= 1 && param[0]->Type() != tvtVoid)
	{
		// check first argument
		if(param[0]->Type() == tvtObject)
		{
			// comarison function object
			closure = param[0]->AsObjectClosureNoAddRef();
			method = 0;
		}
		else
		{
			// sort order letter
			ttstr me = *param[0];
			method = (risse_char)(me.c_str()[0]);

			switch(method)
			{
			case '+':
			case '-':
			case '0':
			case '9':
			case 'a':
			case 'z':
				break;
			default:
				method = '+';
				break;
			}
		}
	}

	if(numparams >= 2 && param[1]->Type() != tvtVoid)
	{
		// whether to do a stable sort
		do_stable_sort = param[1]->operator bool();
	}


	// sort
	switch(method)
	{
	case '+':
		if(do_stable_sort)
			std::stable_sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_NormalAscending());
		else
			std::sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_NormalAscending());
		break;
	case '-':
		if(do_stable_sort)
			std::stable_sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_NormalDescending());
		else
			std::sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_NormalDescending());
		break;
	case '0':
		if(do_stable_sort)
			std::stable_sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_NumericAscending());
		else
			std::sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_NumericAscending());
		break;
	case '9':
		if(do_stable_sort)
			std::stable_sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_NumericDescending());
		else
			std::sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_NumericDescending());
		break;
	case 'a':
		if(do_stable_sort)
			std::stable_sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_StringAscending());
		else
			std::sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_StringAscending());
		break;
	case 'z':
		if(do_stable_sort)
			std::stable_sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_StringDescending());
		else
			std::sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_StringDescending());
		break;
	case 0:
		if(do_stable_sort)
			std::stable_sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_Functional(closure));
		else
			std::sort(ni->Items.begin(), ni->Items.end(),
				tRisseArraySortCompare_Functional(closure));
		break;
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */sort)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */reverse)
{
	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	// reverse array
	std::reverse(ni->Items.begin(), ni->Items.end());

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */reverse)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */assign)
{
	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	((tRisseArrayObject*)objthis)->Clear(ni);

	tRisseVariantClosure clo = param[0]->AsObjectClosureNoAddRef();
	if(clo.ObjThis)
		ni->Assign(clo.ObjThis);
	else if(clo.Object)
		ni->Assign(clo.Object);
	else Risse_eRisseError(RisseNullAccess);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */assign)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */assignStruct)
{
	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	((tRisseArrayObject*)objthis)->Clear(ni);
	std::vector<iRisseDispatch2 *> stack;

	tRisseVariantClosure clo = param[0]->AsObjectClosureNoAddRef();
	if(clo.ObjThis)
		ni->AssignStructure(clo.ObjThis, stack);
	else if(clo.Object)
		ni->AssignStructure(clo.Object, stack);
	else Risse_eRisseError(RisseNullAccess);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */assignStruct)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */clear)
{
	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	((tRisseArrayObject*)objthis)->Clear(ni);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */clear)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */erase)
{
	// remove specified item number from the array

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	risse_int num = *param[0];

	((tRisseArrayObject*)objthis)->Erase(ni, num);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */erase)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */remove)
{
	// remove specified item from the array wchich appears first or all

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	bool eraseall;
	if(numparams >= 2)
		eraseall = param[1]->operator bool();
	else
		eraseall = true;

	tRisseVariant & val = *param[0];

	risse_size count = ((tRisseArrayObject*)objthis)->Remove(ni, val, eraseall);

	if(result) *result = (tTVInteger)count;

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */remove)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */insert)
{
	// insert item at specified position

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 2) return RISSE_E_BADPARAMCOUNT;

	risse_int num = *param[0];

	((tRisseArrayObject*)objthis)->Insert(ni, *param[1], num);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */insert)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */add)
{
	// add item at last

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	((tRisseArrayObject*)objthis)->Add(ni, *param[0]);

	if(result) *result = (tTVInteger)(ni->Items.size() -1);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */add)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/* func.name */find)
{
	// find item in the array,
	// return an index which points the item that appears first.

	RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	if(result)
	{
		tRisseVariant & val = *param[0];
		risse_offset start = 0;
		if(numparams >= 2) start = *param[1];
		if(start < 0) start += ni->Items.size();
		if(start < 0) start = 0;
		if((risse_size)start >= ni->Items.size()) { *result = (tTVInteger)-1; return RISSE_S_OK; }

		tRisseArrayNI::tArrayItemIterator i;
		for(i = ni->Items.begin() + start; i != ni->Items.end(); i++)
		{
			if(val.DiscernCompare(*i)) break;
		}
		if(i == ni->Items.end())
			*result = (tTVInteger)(-1);
		else
			*result = (tTVInteger)(i - ni->Items.begin());
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/* func.name */find)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(count)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);
		if(result) *result = (tTVInteger)(ni->Items.size());
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_BEGIN_NATIVE_PROP_SETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tRisseArrayNI);
		ni->Items.resize((risse_uint)(tTVInteger)*param);
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(count)
//----------------------------------------------------------------------

	ClassID_Array = RISSE_NCM_CLASSID;
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
tRisseArrayClass::~tRisseArrayClass()
{
}
//---------------------------------------------------------------------------
tRisseNativeInstance *tRisseArrayClass::CreateNativeInstance()
{
	return new tRisseArrayNI(); // return a native object instance.
}
//---------------------------------------------------------------------------
iRisseDispatch2 *tRisseArrayClass::CreateBaseRisseObject()
{
	return new tRisseArrayObject();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseArrayNI
//---------------------------------------------------------------------------
tRisseArrayNI::tRisseArrayNI()
{
	// constructor
}
//---------------------------------------------------------------------------
risse_error tRisseArrayNI::Construct(risse_int numparams, tRisseVariant **params,
	iRisseDispatch2 * risseobj)
{
	// called by risse constructor
	if(numparams!=0) return RISSE_E_BADPARAMCOUNT;
	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
void tRisseArrayNI::Assign(iRisseDispatch2 * dsp)
{
	// copy members from "dsp" to "Owner"

	// determin dsp's object type
	tRisseArrayNI *arrayni = NULL;
	if(RISSE_SUCCEEDED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
		ClassID_Array, (iRisseNativeInstance**)&arrayni)) )
	{
		// copy from array
		Items.clear();

		Items.assign(arrayni->Items.begin(), arrayni->Items.end());
	}
	else
	{
		// convert from dictionary or others
		Items.clear();
		tDictionaryEnumCallback callback;
		callback.Items = &Items;

		tRisseVariantClosure clo(&callback, NULL);
		dsp->EnumMembers(RISSE_IGNOREPROP, &clo, dsp);

	}
}
//---------------------------------------------------------------------------
risse_error tRisseArrayNI::tDictionaryEnumCallback::FuncCall(
	risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
	tRisseVariant *result, risse_int numparams, tRisseVariant **param,
	iRisseDispatch2 *objthis)
{
	// called from tRisseCustomObject::EnumMembers

	if(numparams < 3) return RISSE_E_BADPARAMCOUNT;

	// hidden members are not processed
	risse_uint32 flags = (risse_int)*param[1];
	if(flags & RISSE_HIDDENMEMBER)
	{
		if(result) *result = (tTVInteger)1;
		return RISSE_S_OK;
	}

	// push items

	Items->push_back(*param[0]);
	Items->push_back(*param[2]);

	if(result) *result = (tTVInteger)1;
	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
void tRisseArrayNI::SaveStructuredData(std::vector<iRisseDispatch2 *> &stack,
	tRisseStringAppender & string, const ttstr &indentstr)
{
#ifdef RISSE_TEXT_OUT_CRLF
	string += RISSE_WS("[\r\n");
#else
	string += RISSE_WS("[\n");
#endif

	ttstr indentstr2 = indentstr + RISSE_WS(" ");

	tArrayItemIterator i;
	risse_uint c = 0;
	for(i = Items.begin(); i != Items.end(); i++)
	{
		string += indentstr2;
		tRisseVariantType type = i->Type();
		if(type == tvtObject)
		{
			// object
			tRisseVariantClosure clo = i->AsObjectClosureNoAddRef();
			SaveStructuredDataForObject(clo.SelectObjectNoAddRef(),
				stack, string, indentstr2);
		}
		else
		{
			string += RisseVariantToExpressionString(*i);
		}
#ifdef RISSE_TEXT_OUT_CRLF
		if(c != Items.size() -1) // unless last
			string += RISSE_WS(",\r\n");
		else
			string += RISSE_WS("\r\n");
#else
		if(c != Items.size() -1) // unless last
			string += RISSE_WS(",\n");
		else
			string += RISSE_WS("\n");
#endif

		c++;
	}

	string += indentstr;
	string += RISSE_WS("]");
}
//---------------------------------------------------------------------------
void tRisseArrayNI::SaveStructuredDataForObject(iRisseDispatch2 *dsp,
		std::vector<iRisseDispatch2 *> &stack, tRisseStringAppender &string,
		const ttstr &indentstr)
{
	// check object recursion
	std::vector<iRisseDispatch2 *>::iterator i;
	for(i = stack.begin(); i!=stack.end(); i++)
	{
		if(*i == dsp)
		{
			// object recursion detected
			string += RISSE_WS("null /* object recursion detected */");
			return;
		}
	}

	// determin dsp's object type
	tRisseDictionaryNI *dicni = NULL;
	tRisseArrayNI *arrayni = NULL;
	if(dsp && RISSE_SUCCEEDED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
		RisseGetDictionaryClassID(), (iRisseNativeInstance**)&dicni)) )
	{
		// dictionary
		stack.push_back(dsp);
		dicni->SaveStructuredData(stack, string, indentstr);
		stack.pop_back();
	}
	else if(dsp && RISSE_SUCCEEDED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
		ClassID_Array, (iRisseNativeInstance**)&arrayni)) )
	{
		// array
		stack.push_back(dsp);
		arrayni->SaveStructuredData(stack, string, indentstr);
		stack.pop_back();
	}
	else if(dsp != NULL)
	{
		// other objects
		string += RISSE_WS("null /* (object) \""); // stored as a null
		tRisseVariant val(dsp,dsp);
		string += ttstr(val).EscapeC();
		string += RISSE_WS("\" */");
	}
	else
	{
		// null
		string += RISSE_WS("null");
	}
}
//---------------------------------------------------------------------------
void tRisseArrayNI::AssignStructure(iRisseDispatch2 * dsp,
	std::vector<iRisseDispatch2 *> &stack)
{
	// assign structured data from dsp
	tRisseArrayNI *arrayni = NULL;
	if(RISSE_SUCCEEDED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
		ClassID_Array, (iRisseNativeInstance**)&arrayni)) )
	{
		// copy from array
		stack.push_back(dsp);
		try
		{
			Items.clear();

			tArrayItemIterator i;
			for(i = arrayni->Items.begin(); i != arrayni->Items.end(); i++)
			{
				tRisseVariantType type = i->Type();
				if(type == tvtObject)
				{
					// object
					iRisseDispatch2 *dsp = i->AsObjectNoAddRef();
					// determin dsp's object type

					tRisseDictionaryNI *dicni = NULL;
					tRisseArrayNI *arrayni = NULL;

					if(dsp && RISSE_SUCCEEDED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
						RisseGetDictionaryClassID(), (iRisseNativeInstance**)&dicni)) )
					{
						// dictionary
						bool objrec = false;
						std::vector<iRisseDispatch2 *>::iterator i;
						for(i = stack.begin(); i!=stack.end(); i++)
						{
							if(*i == dsp)
							{
								// object recursion detected
								objrec = true;
								break;
							}
						}
						if(objrec)
						{
							Items.push_back(tRisseVariant((iRisseDispatch2*)NULL)); // becomes null
						}
						else
						{
							iRisseDispatch2 * newobj = RisseCreateDictionaryObject();
							Items.push_back(tRisseVariant(newobj, newobj));
							newobj->Release();
							tRisseDictionaryNI * newni = NULL;
							if(RISSE_SUCCEEDED(newobj->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
								RisseGetDictionaryClassID(), (iRisseNativeInstance**)&newni)) )
							{
								newni->AssignStructure(dsp, stack);
							}
						}
					}
					else if(dsp && RISSE_SUCCEEDED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
						RisseGetArrayClassID(), (iRisseNativeInstance**)&arrayni)) )
					{
						// array
						bool objrec = false;
						std::vector<iRisseDispatch2 *>::iterator i;
						for(i = stack.begin(); i!=stack.end(); i++)
						{
							if(*i == dsp)
							{
								// object recursion detected
								objrec = true;
								break;
							}
						}
						if(objrec)
						{
							Items.push_back(tRisseVariant((iRisseDispatch2*)NULL)); // becomes null
						}
						else
						{
							iRisseDispatch2 * newobj = RisseCreateArrayObject();
							Items.push_back(tRisseVariant(newobj, newobj));
							newobj->Release();
							tRisseArrayNI * newni = NULL;
							if(RISSE_SUCCEEDED(newobj->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
								RisseGetArrayClassID(), (iRisseNativeInstance**)&newni)) )
							{
								newni->AssignStructure(dsp, stack);
							}
						}
					}
					else
					{
						// other object types
						Items.push_back(*i);
					}
				}
				else
				{
					// others
					Items.push_back(*i);
				}
			}
		}
		catch(...)
		{
			stack.pop_back();
			throw;
		}
		stack.pop_back();
	}
	else
	{
		Risse_eRisseError(RisseSpecifyDicOrArray);
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseArrayObject
//---------------------------------------------------------------------------
tRisseArrayObject::tRisseArrayObject() : tRisseCustomObject(RISSE_ARRAY_BASE_HASH_BITS)
{
	CallFinalize = false;
}
//---------------------------------------------------------------------------
tRisseArrayObject::~tRisseArrayObject()
{
}
//---------------------------------------------------------------------------

#define ARRAY_GET_NI \
	tRisseArrayNI *ni; \
	if(RISSE_FAILED(objthis->NativeInstanceSupport(RISSE_NIS_GETINSTANCE, \
		ClassID_Array, (iRisseNativeInstance**)&ni))) \
			return RISSE_E_NATIVECLASSCRASH;
static tRisseVariant VoidValue;
#define ARRAY_GET_VAL \
	risse_int membercount = (risse_int)(ni->Items.size()); \
	if(num < 0) num = membercount + num; \
	if((flag & RISSE_MEMBERMUSTEXIST) && (num < 0 || membercount <= num)) \
		return RISSE_E_MEMBERNOTFOUND; \
	tRisseVariant val ( (membercount<=num || num < 0) ?VoidValue:ni->Items[num]);
		// Do not take reference of the element (because the element *might*
		// disappear in function call, property handler etc.) So here must be
		// an object copy, not reference.
//---------------------------------------------------------------------------
void tRisseArrayObject::Finalize()
{
	tRisseArrayNI *ni;
	if(RISSE_FAILED(NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
		ClassID_Array, (iRisseNativeInstance**)&ni)))
			Risse_eRisseError(RisseNativeClassCrash);
	Clear(ni);

	inherited::Finalize();
}
//---------------------------------------------------------------------------
void tRisseArrayObject::Clear(tRisseArrayNI * ni)
{
	// clear members

	std::vector<iRisseDispatch2*> vector;
	try
	{
		risse_uint i;
		for(i=0; i<ni->Items.size(); i++)
		{
			if(ni->Items[i].Type() == tvtObject)
			{
				CheckObjectClosureRemove(ni->Items[i]);
				tRisseVariantClosure clo =
					ni->Items[i].AsObjectClosureNoAddRef();
				clo.AddRef();
				if(clo.Object) vector.push_back(clo.Object);
				if(clo.ObjThis) vector.push_back(clo.ObjThis);
				ni->Items[i].Clear();
			}
		}
		ni->Items.clear();
	}
	catch(...)
	{
		std::vector<iRisseDispatch2*>::iterator i;
		for(i = vector.begin(); i != vector.end(); i++)
		{
			(*i)->Release();
		}

		throw;
	}

	// release all objects
	std::vector<iRisseDispatch2*>::iterator i;
	for(i = vector.begin(); i != vector.end(); i++)
	{
		(*i)->Release();
	}

}
//---------------------------------------------------------------------------
void tRisseArrayObject::Add(tRisseArrayNI * ni, const tRisseVariant &val)
{
	ni->Items.push_back(val);
	CheckObjectClosureAdd(ni->Items[ni->Items.size() -1]);
}
//---------------------------------------------------------------------------
risse_int tRisseArrayObject::Remove(tRisseArrayNI * ni, const tRisseVariant &ref, bool removeall)
{
	risse_int count = 0;
	std::vector<risse_int> todelete;
	risse_int num = 0;
	for(tRisseArrayNI::tArrayItemIterator i = ni->Items.begin();
		i != ni->Items.end(); i++)
	{
		if(ref.DiscernCompare(*i))
		{
			count ++;
			todelete.push_back(num);
			if(!removeall) break;
		}
		num++;
	}

	std::vector<iRisseDispatch2*> vector;
	try
	{
		// list objects up
		for(std::vector<risse_int>::iterator i = todelete.begin();
			i != todelete.end(); i++)
		{
			if(ni->Items[*i].Type() == tvtObject)
			{
				CheckObjectClosureRemove(ni->Items[*i]);
				tRisseVariantClosure clo =
					ni->Items[*i].AsObjectClosureNoAddRef();
				clo.AddRef();
				if(clo.Object) vector.push_back(clo.Object);
				if(clo.ObjThis) vector.push_back(clo.ObjThis);
				ni->Items[*i].Clear();
			}
		}

		// remove items found
		for(risse_int i = todelete.size() -1; i>=0; i--)
		{
			ni->Items.erase(ni->Items.begin() + todelete[i]);
		}
	}
	catch(...)
	{
		std::vector<iRisseDispatch2*>::iterator i;
		for(i = vector.begin(); i != vector.end(); i++)
		{
			(*i)->Release();
		}

		throw;
	}

	// release all objects
	std::vector<iRisseDispatch2*>::iterator i;
	for(i = vector.begin(); i != vector.end(); i++)
	{
		(*i)->Release();
	}

	return count;
}
//---------------------------------------------------------------------------
void tRisseArrayObject::Erase(tRisseArrayNI * ni, risse_int num)
{
	if(num < 0) num += ni->Items.size();
	if(num < 0) Risse_eRisseError(RisseRangeError);
	if((unsigned)num >= ni->Items.size()) Risse_eRisseError(RisseRangeError);

	CheckObjectClosureRemove(ni->Items[num]);
	ni->Items.erase(ni->Items.begin() + num);
}
//---------------------------------------------------------------------------
void tRisseArrayObject::Insert(tRisseArrayNI *ni, const tRisseVariant &val, risse_int num)
{
	if(num < 0) num += ni->Items.size();
	if(num < 0) Risse_eRisseError(RisseRangeError);
	risse_int count = (risse_int)ni->Items.size();
	if(num > count) Risse_eRisseError(RisseRangeError);

	if(num == count)
		ni->Items.push_back(val);
	else
		ni->Items.insert(ni->Items.begin() + num, val);
	CheckObjectClosureAdd(val);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::FuncCall(risse_uint32 flag, const risse_char * membername,
	risse_uint32 *hint,
		tRisseVariant *result, risse_int numparams, tRisseVariant **param,
		iRisseDispatch2 *objthis)
{
	risse_int idx;
	if(membername && IsNumber(membername, idx))
		return FuncCallByNum(flag, idx, result, numparams, param, objthis);

	return inherited::FuncCall(flag, membername, hint, result, numparams, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::FuncCallByNum(risse_uint32 flag, risse_int num, tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	ARRAY_GET_NI;
	ARRAY_GET_VAL;
	return RisseDefaultFuncCall(flag, val, result, numparams, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::PropGet(risse_uint32 flag, const risse_char * membername,
	risse_uint32*hint, tRisseVariant *result, iRisseDispatch2 *objthis)
{
	risse_int idx;
	if(membername && IsNumber(membername, idx))
		return PropGetByNum(flag, idx, result, objthis);
	return inherited::PropGet(flag, membername, hint, result, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::PropGetByNum(risse_uint32 flag, risse_int num,
		tRisseVariant *result, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	ARRAY_GET_NI;
	ARRAY_GET_VAL;
	return RisseDefaultPropGet(flag, val, result, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::PropSet(risse_uint32 flag, const risse_char *membername,
		risse_uint32 *hint,
		const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	risse_int idx;
	if(membername && IsNumber(membername, idx))
		return PropSetByNum(flag, idx, param, objthis);
	return inherited::PropSet(flag, membername, hint, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::PropSetByNum(risse_uint32 flag, risse_int num,
		const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	ARRAY_GET_NI;
	if(num < 0) num += ni->Items.size();
	if(num >= (risse_int)ni->Items.size())
	{
		if(flag & RISSE_MEMBERMUSTEXIST) return RISSE_E_MEMBERNOTFOUND;
		ni->Items.resize(num+1);
	}
	if(num < 0) return RISSE_E_MEMBERNOTFOUND;
	tRisseVariant &val = ni->Items[num];
	risse_error hr;
	CheckObjectClosureRemove(val);
	try
	{
		hr = RisseDefaultPropSet(flag, val, param, objthis);
	}
	catch(...)
	{
		CheckObjectClosureAdd(val);
		throw;
	}
	CheckObjectClosureAdd(val);
	return hr;
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::PropSetByVS(risse_uint32 flag, tRisseVariantString *membername,
		const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	risse_int idx;
	if(membername && IsNumber((const risse_char*)(*membername), idx))
		return PropSetByNum(flag, idx, param, objthis);
	return inherited::PropSetByVS(flag, membername, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::DeleteMember(risse_uint32 flag, const risse_char *membername,
	risse_uint32 *hint,
	iRisseDispatch2 *objthis)
{
	risse_int idx;
	if(membername && IsNumber(membername, idx))
		return DeleteMemberByNum(flag, idx, objthis);
	return inherited::DeleteMember(flag, membername, hint, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::DeleteMemberByNum(risse_uint32 flag, risse_int num,
	iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	ARRAY_GET_NI;
	if(num < 0) num += ni->Items.size();
	if(num < 0 || (risse_uint)num>=ni->Items.size()) return RISSE_E_MEMBERNOTFOUND;
	CheckObjectClosureRemove(ni->Items[num]);
	std::deque<tRisseVariant>::iterator i;
	ni->Items.erase(ni->Items.begin() + num);
	return RISSE_S_OK;
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::Invalidate(risse_uint32 flag, const risse_char *membername,
	risse_uint32 *hint,
	iRisseDispatch2 *objthis)
{
	risse_int idx;
	if(membername && IsNumber(membername, idx))
		return InvalidateByNum(flag, idx, objthis);
	return inherited::Invalidate(flag, membername, hint, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::InvalidateByNum(risse_uint32 flag, risse_int num,
		iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	ARRAY_GET_NI;
	ARRAY_GET_VAL;
	return RisseDefaultInvalidate(flag, val, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::IsValid(risse_uint32 flag, const risse_char *membername,
		risse_uint32 *hint,
		iRisseDispatch2 *objthis)
{
	risse_int idx;
	if(membername && IsNumber(membername, idx))
		return IsValidByNum(flag, idx, objthis);
	return inherited::IsValid(flag, membername, hint, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::IsValidByNum(risse_uint32 flag, risse_int num,
		iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	ARRAY_GET_NI;
	ARRAY_GET_VAL;
	return RisseDefaultIsValid(flag, val, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::CreateNew(risse_uint32 flag, const risse_char * membername,
		risse_uint32 *hint,
		iRisseDispatch2 **result, risse_int numparams, tRisseVariant **param,
		iRisseDispatch2 *objthis)
{
	risse_int idx;
	if(membername && IsNumber(membername, idx))
		return CreateNewByNum(flag, idx, result, numparams, param, objthis);
	return inherited::CreateNew(flag, membername, hint, result, numparams, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::CreateNewByNum(risse_uint32 flag, risse_int num, iRisseDispatch2 **result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	ARRAY_GET_NI;
	ARRAY_GET_VAL;
	return RisseDefaultCreateNew(flag, val, result, numparams, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::IsInstanceOf(risse_uint32 flag,const  risse_char *membername,
		risse_uint32 *hint,
		const risse_char *classname, iRisseDispatch2 *objthis)
{
	risse_int idx;
	if(membername && IsNumber(membername, idx))
		return IsInstanceOfByNum(flag, idx, classname, objthis);
	return inherited::IsInstanceOf(flag, membername, hint, classname, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::IsInstanceOfByNum(risse_uint32 flag, risse_int num,
		const risse_char *classname, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	ARRAY_GET_NI;
	ARRAY_GET_VAL;
	return RisseDefaultIsInstanceOf(flag, val, classname, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::Operation(risse_uint32 flag, const risse_char *membername,
		risse_uint32 *hint,
		tRisseVariant *result, const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	risse_int idx;
	if(membername && IsNumber(membername, idx))
		return OperationByNum(flag, idx, result, param, objthis);
	return inherited::Operation(flag, membername, hint, result, param, objthis);
}
//---------------------------------------------------------------------------
risse_error
	tRisseArrayObject::OperationByNum(risse_uint32 flag, risse_int num,
		tRisseVariant *result, const tRisseVariant *param, iRisseDispatch2 *objthis)
{
	if(!GetValidity())
		return RISSE_E_INVALIDOBJECT;

	ARRAY_GET_NI;
	if(num < 0) num += ni->Items.size();
	if(num >= (risse_int)ni->Items.size())
	{
		if(flag & RISSE_MEMBERMUSTEXIST) return RISSE_E_MEMBERNOTFOUND;
		ni->Items.resize(num+1);
	}
	if(num < 0) return RISSE_E_MEMBERNOTFOUND;
	risse_error hr;
	tRisseVariant &val = ni->Items[num];
	CheckObjectClosureRemove(val);
	try
	{
		hr=RisseDefaultOperation(flag, val, result, param, objthis);
	}
	catch(...)
	{
		CheckObjectClosureAdd(val);
		throw;
	}
	CheckObjectClosureAdd(val);
	return hr;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// RisseGetArrayClassID
//---------------------------------------------------------------------------
risse_int32 RisseGetArrayClassID()
{
	return ClassID_Array;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// RisseCreateArrayObject
//---------------------------------------------------------------------------
iRisseDispatch2 * RisseCreateArrayObject(iRisseDispatch2 **classout)
{
	// create an Array object
	struct tHolder
	{
		iRisseDispatch2 * Obj;
		tHolder() { Obj = new tRisseArrayClass(); }
		~tHolder() { Obj->Release(); }
	} static arrayclass;

	if(classout) *classout = arrayclass.Obj, arrayclass.Obj->AddRef();

	tRisseArrayObject *arrayobj;
	(arrayclass.Obj)->CreateNew(0, NULL, NULL, 
		(iRisseDispatch2**)&arrayobj, 0, NULL, arrayclass.Obj);
	return arrayobj;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Utility functions
//---------------------------------------------------------------------------
risse_int RisseGetArrayElementCount(iRisseDispatch2 * dsp)
{
	// returns array element count
	tRisseArrayNI *ni;
	if(RISSE_FAILED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
		ClassID_Array, (iRisseNativeInstance**)&ni)))
			Risse_eRisseError(RisseSpecifyArray);
	return ni->Items.size();
}
//---------------------------------------------------------------------------
risse_int RisseCopyArrayElementTo(iRisseDispatch2 * dsp,
	tRisseVariant *dest, risse_uint start, risse_int count)
{
	// copy array elements to specified variant array.
	// returns copied element count.
	tRisseArrayNI *ni;
	if(RISSE_FAILED(dsp->NativeInstanceSupport(RISSE_NIS_GETINSTANCE,
		ClassID_Array, (iRisseNativeInstance**)&ni)))
			Risse_eRisseError(RisseSpecifyArray);

	if(count < 0) count = ni->Items.size();

	if(start >= ni->Items.size()) return 0;

	risse_uint limit = start + count;

	for(risse_uint i = start; i < limit; i++)
		*(dest++) = ni->Items[i];

	return limit - start;
}
//---------------------------------------------------------------------------





} // namespace Risse


