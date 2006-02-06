//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief tRisseVariant や tRisseString で使われる文字列の管理
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include "risseVariantString.h"
#include "risseError.h"
#include "risseUtils.h"
#include "risseLex.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(34213,3201,39834,18417,3227,47787,4432,48477);

//---------------------------------------------------------------------------
// 例外を送出するための関数群
//---------------------------------------------------------------------------
void RisseThrowStringDeallocError()
{
	Risse_eRisseVariantError(RisseStringDeallocError);
}
//---------------------------------------------------------------------------
void RisseThrowStringAllocError()
{
	Risse_eRisseVariantError(RisseStringAllocError);
}
//---------------------------------------------------------------------------
void RisseThrowNarrowToWideConversionError()
{
	Risse_eRisseVariantError(RisseNarrowToWideConversionError);
}
//---------------------------------------------------------------------------
risse_int RisseGetShorterStrLen(const risse_char *str, risse_int max)
{
	// select shorter length over strlen(str) and max
	if(!str) return 0;
	const risse_char *p = str;
	max++;
	while(*p && --max) p++;
	return p - str;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 長い文字列用の文字列確保ルーチン
//---------------------------------------------------------------------------
#define RisseVS_ALLOC_INC_SIZE_S 16
	// additional space for long string heap under RisseVS_ALLOC_DOUBLE_LIMIT
#define RisseVS_ALLOC_INC_SIZE_L 4000000
	// additional space for long string heap over RisseVS_ALLOC_DOUBLE_LIMIT
#define RisseVS_ALLOC_DOUBLE_LIMIT 4000000
	// switching value of double-sizing or incremental-sizing
//---------------------------------------------------------------------------
/*static inline*/ risse_char *RisseVS_malloc(risse_uint len)
{
	char *ret = (char*)malloc(
		(len = (len + RisseVS_ALLOC_INC_SIZE_S))*sizeof(risse_char) + sizeof(size_t));
	if(!ret) RisseThrowStringAllocError();
	*(size_t *)ret = len; // embed size
	return (risse_char*)(ret + sizeof(size_t));
}
//---------------------------------------------------------------------------
/*static inline*/ risse_char *RisseVS_realloc(risse_char *buf, risse_uint len)
{
	if(!buf) return RisseVS_malloc(len);

	// compare embeded size
	size_t * ptr = (size_t *)((char*)buf - sizeof(size_t));
	if(*ptr >= len) return buf; // still adequate

//	char *ret = (char*)realloc(ptr,
//		(len = (len + RisseVS_ALLOC_INC_SIZE))*sizeof(risse_char) + sizeof(size_t));
	if(len < RisseVS_ALLOC_DOUBLE_LIMIT)
		len = len * 2;
	else
		len = len + RisseVS_ALLOC_INC_SIZE_L;

	char *ret = (char*)realloc(ptr,
		len*sizeof(risse_char) + sizeof(size_t));
	if(!ret) RisseThrowStringAllocError();
	*(size_t *)ret = len; // embed size
	return (risse_char*)(ret + sizeof(size_t));
}
//---------------------------------------------------------------------------
/*static inline*/ void RisseVS_free(risse_char *buf)
{
	// free buffer
	free((char*)buf - sizeof(size_t));
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// StringHeap
//---------------------------------------------------------------------------
//#define RISSE_VS_USE_SYSTEM_NEW

#define HEAP_FLAG_USING 0x01
#define HEAP_FLAG_DELETE 0x02
#define HEAP_CAPACITY_INC 4096
static tRisseCriticalSection *RisseStringHeapCS = NULL;
static std::vector<tRisseVariantString*> *RisseStringHeapList = NULL;
static tRisseVariantString ** RisseStringHeapFreeCellList = NULL;
//static risse_uint RisseStringHeapFreeCellListCapacity = 0;
static risse_uint RisseStringHeapFreeCellListPointer = 0;
static risse_uint RisseStringHeapAllocCount = 0;

static risse_uint RisseStringHeapLastCheckedFreeBlock = 0;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void RisseAddStringHeapBlock(void)
{
	// allocate StringHeapBlock
	tRisseVariantString *h;
	h = new tRisseVariantString[HEAP_CAPACITY_INC];
	memset(h, 0, sizeof(tRisseVariantString) * HEAP_CAPACITY_INC);
	RisseStringHeapList->push_back(h);

	// re-allocate RisseFreeCellList
	if(RisseStringHeapFreeCellList) delete [] RisseStringHeapFreeCellList;
	RisseStringHeapFreeCellList =
		new tRisseVariantString *[RisseStringHeapList->size() * HEAP_CAPACITY_INC];

	// prepare free list
	for(risse_int i = HEAP_CAPACITY_INC - 1; i >= 0; i--)
		RisseStringHeapFreeCellList[i] = h + i;
	RisseStringHeapFreeCellListPointer = HEAP_CAPACITY_INC;
}
//---------------------------------------------------------------------------
static void RisseInitStringHeap()
{
	RisseStringHeapCS = new tRisseCriticalSection();
	RisseStringHeapList = new std::vector<tRisseVariantString*>();
	RisseAddStringHeapBlock(); // initial block
}
//---------------------------------------------------------------------------
static void RisseUninitStringHeap(void)
{
	if(!RisseStringHeapList) return;
	if(!RisseStringHeapList->empty())
	{
		std::vector<tRisseVariantString*>::iterator c;
		for(c = RisseStringHeapList->end()-1; c >= RisseStringHeapList->begin(); c--)
		{
			tRisseVariantString *h = *c;
			for(risse_int i = 0; i < HEAP_CAPACITY_INC; i++)
			{
				if(h[i].HeapFlag & HEAP_FLAG_USING)
				{
					// using cell
					if(h[i].LongString) RisseVS_free(h[i].LongString);
				}
			}
			delete [] h;
		}
	}

	delete RisseStringHeapCS;
	RisseStringHeapCS = NULL;

	delete RisseStringHeapList;
	RisseStringHeapList = NULL;

	if(RisseStringHeapFreeCellList)
	{
		delete [] RisseStringHeapFreeCellList;
		RisseStringHeapFreeCellList = NULL;
	}
}
//---------------------------------------------------------------------------
#ifdef RISSE_DEBUG_UNRELEASED_STRING
static void RisseUninitStringHeap(void)
{
	if(!RisseStringHeapList) return;
	if(!RisseStringHeapList->empty())
	{
		std::vector<tRisseVariantString*>::iterator c;
		for(c = RisseStringHeapList->end()-1; c >= RisseStringHeapList->begin(); c--)
		{
			tRisseVariantString *h = *c;
			for(risse_int i = 0; i < HEAP_CAPACITY_INC; i++)
			{
				if(h[i].HeapFlag & HEAP_FLAG_USING)
				{
					// using cell
					char buf[1024];
					sprintf(buf, "%p:%ls", h + i,
						h[i].LongString?h[i].LongString:h[i].ShortString);
					OutputDebugString(buf);
				}
			}
		}
	}
}
#pragma exit RisseReportUnreleasedStringHeap 1

#endif

//---------------------------------------------------------------------------
#ifdef RISSE_DEBUG_DUMP_STRING
void RisseDumpStringHeap(void)
{
	if(!RisseStringHeapList) return;
	if(!RisseStringHeapList->empty())
	{
		FILE *f = fopen("string.txt", "wt");
		if(!f) return;
		std::vector<tRisseVariantString*>::iterator c;
		for(c = RisseStringHeapList->end()-1; c >= RisseStringHeapList->begin(); c--)
		{
			tRisseVariantString *h = *c;
			for(risse_int i = 0; i < HEAP_CAPACITY_INC; i++)
			{
				if(h[i].HeapFlag & HEAP_FLAG_USING)
				{
					// using cell
					tRisseNarrowStringHolder narrow(h[i].LongString?h[i].LongString:h[i].ShortString);

					fprintf(f, "%s\n", (const char*)narrow);
				}
			}
		}
		fclose(f);
	}
}
#endif
//---------------------------------------------------------------------------
static int RisseStringHeapSortFunction(const void *a, const void *b)
{
	return *(const tRisseVariantString **)b - *(const tRisseVariantString **)a;
}
//---------------------------------------------------------------------------
void RisseCompactStringHeap()
{
#ifdef RISSE_DEBUG_CHECK_STRING_HEAP_INTEGRITY

	if(!RisseStringHeapList) return;
	if(!RisseStringHeapList->empty())
	{
		std::vector<tRisseVariantString*>::iterator c;
		for(c = RisseStringHeapList->end()-1; c >= RisseStringHeapList->begin(); c--)
		{
			tRisseVariantString *h = *c;
			for(risse_int i = 0; i < HEAP_CAPACITY_INC; i++)
			{
				if(h[i].HeapFlag & HEAP_FLAG_USING)
				{
					// using cell
					const wchar_t * ptr = h[i].operator const risse_char *();
					if(ptr[0] == 0)
					{
						OutputDebugString("empty string cell found");
					}
					if(Risse_strlen(ptr) != h[i].GetLength())
					{
						OutputDebugString("invalid string length cell found");
					}
				}
			}
		}
	}
#endif

#define RISSE_SORT_STR_CELL_MAX 1000
#define RISSE_CHECK_FREE_BLOCK_MAX 50

	// sort free cell list by address
	if(!RisseStringHeapFreeCellList) return;
	if(!RisseStringHeapList) return;

	{	// thread-protected
		tRisseCriticalSection::tLocker csh(*RisseStringHeapCS);

#ifndef __CODEGUARD__
	// may be very slow when used with codeguard
		// sort RisseStringHeapFreeCellList
		tRisseVariantString ** start;
		risse_int count;
		if(RisseStringHeapFreeCellListPointer > RISSE_SORT_STR_CELL_MAX)
		{
			// too large; sort last RISSE_SORT_STR_CELL_MAX items
			start = RisseStringHeapFreeCellList +
				RisseStringHeapFreeCellListPointer - RISSE_SORT_STR_CELL_MAX;
			count = RISSE_SORT_STR_CELL_MAX;
		}
		else
		{
			start = RisseStringHeapFreeCellList;
			count = RisseStringHeapFreeCellListPointer;
		}

		qsort(start,
			count,
			sizeof(tRisseVariantString *),
			RisseStringHeapSortFunction);
#endif

		// delete all-freed heap block

		// - mark all-freed heap block
		risse_int free_block_count = 0;

		if(RisseStringHeapList)
		{
			if(RisseStringHeapLastCheckedFreeBlock >= RisseStringHeapList->size())
				RisseStringHeapLastCheckedFreeBlock = 0;
			risse_uint block_ind = RisseStringHeapLastCheckedFreeBlock;
			risse_int count = 0;

			do
			{
				risse_int freecount = 0;
				tRisseVariantString * block = (*RisseStringHeapList)[block_ind];
				for(risse_int i = 0; i < HEAP_CAPACITY_INC; i++)
				{
					if(!(block[i].HeapFlag & HEAP_FLAG_USING))
						freecount ++;
				}

				if(freecount == HEAP_CAPACITY_INC)
				{
					// all freed
					free_block_count ++;
					for(risse_int i = 0; i < HEAP_CAPACITY_INC; i++)
					{
						block[i].HeapFlag = HEAP_FLAG_DELETE;
					}
				}

				block_ind ++;
				if(block_ind >= RisseStringHeapList->size()) block_ind = 0;
				count++;
				
			} while(count < RISSE_CHECK_FREE_BLOCK_MAX &&
				block_ind != RisseStringHeapLastCheckedFreeBlock);

			RisseStringHeapLastCheckedFreeBlock = block_ind;
		}

		// - delete all marked cell from RisseStringHeapFreeCellList
		if(free_block_count)
		{
			tRisseVariantString ** newlist =
				new tRisseVariantString *
					[(RisseStringHeapList->size() - free_block_count) *
					  HEAP_CAPACITY_INC];

			risse_uint wp = 0;
			risse_uint rp;
			for(rp = 0; rp < RisseStringHeapFreeCellListPointer; rp++)
			{
				if(RisseStringHeapFreeCellList[rp]->HeapFlag != HEAP_FLAG_DELETE)
					newlist[wp++] =
						RisseStringHeapFreeCellList[rp];
			}

			RisseStringHeapFreeCellListPointer = wp;

			delete [] RisseStringHeapFreeCellList;
			RisseStringHeapFreeCellList = newlist;
		}

		// - delete all marked block
		if(free_block_count)
		{
			std::vector<tRisseVariantString*>::iterator i;
			for(i = RisseStringHeapList->begin(); i != RisseStringHeapList->end();)
			{
				if((*i)[0].HeapFlag == HEAP_FLAG_DELETE)
				{
					// to be deleted
					delete [] (*i);
					i = RisseStringHeapList->erase(i);
				}
				else
				{
					i++;
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
tRisseVariantString * RisseAllocStringHeap(void)
{
	if(RisseStringHeapAllocCount == 0)
	{
		// first string to alloc
		// here must be called in main thread...
		RisseInitStringHeap(); // initial block
	}

	{	// thread-protected
		tRisseCriticalSection::tLocker csh(*RisseStringHeapCS);

#ifdef RISSE_VS_USE_SYSTEM_NEW
		tRisseVariantString *ret = new tRisseVariantString();
#else
		if(RisseStringHeapFreeCellListPointer == 0)
			RisseAddStringHeapBlock();

		tRisseVariantString *ret =
			RisseStringHeapFreeCellList[--RisseStringHeapFreeCellListPointer];
#endif

		ret->RefCount = 0;
		ret->Length = 0;
		ret->LongString = NULL;
		ret->HeapFlag = HEAP_FLAG_USING;
		ret->Hint = 0;

		RisseStringHeapAllocCount ++;

		return ret;

	}	// end-of-thread-protected
}
//---------------------------------------------------------------------------
void RisseDeallocStringHeap(tRisseVariantString * vs)
{
	// free vs

	{	// thread-pretected
		tRisseCriticalSection::tLocker csh(*RisseStringHeapCS);

#ifdef RISSE_DEBUG_CHECK_STRING_HEAP_INTEGRITY
		{
			const wchar_t * ptr = vs->operator const risse_char *();
			if(ptr[0] == 0)
			{
				OutputDebugString("empty string cell found");
			}
			if(Risse_strlen(ptr) != vs->GetLength())
			{
				OutputDebugString("invalid string length cell found");
			}
		}
#endif


		if(vs->LongString) RisseVS_free(vs->LongString);

		vs->HeapFlag = 0;

#ifdef RISSE_VS_USE_SYSTEM_NEW
		delete vs;
#else
		RisseStringHeapFreeCellList[RisseStringHeapFreeCellListPointer++] = vs;
#endif

		RisseStringHeapAllocCount--;
	}	// end-of-thread-protected

	if(RisseStringHeapAllocCount == 0)
	{
		// last string was freed
		// here must be called in main thread...
		RisseUninitStringHeap();
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tRisseVariantString
//---------------------------------------------------------------------------
void tRisseVariantString::Release()
{
/*
	if(!this) return;
		// this is not a REAL remedy, but enough to buster careless misses...
		// (NULL->Release() problem)
*/

	if(RefCount==0)
	{
		RisseDeallocStringHeap(this);
		return;
	}
	RefCount--;
}
//---------------------------------------------------------------------------
tTVInteger tRisseVariantString::ToInteger() const
{
	if(!this) return 0;

	tRisseVariant val;
	const risse_char *ptr = this->operator const risse_char *();
	if(RisseParseNumber(val, &ptr)) 	return val.AsInteger();
	return 0;
}
//---------------------------------------------------------------------------
tTVReal tRisseVariantString::ToReal() const
{
	if(!this) return 0;

	tRisseVariant val;
	const risse_char *ptr = this->operator const risse_char *();
	if(RisseParseNumber(val, &ptr)) return val.AsReal();
	return 0;
}
//---------------------------------------------------------------------------
void tRisseVariantString::ToNumber(tRisseVariant &dest) const
{
	if(!this) { dest = (tTVInteger)0; return; }

	const risse_char *ptr = this->operator const risse_char *();
	if(RisseParseNumber(dest, &ptr)) return;

	dest = (tTVInteger)0;
}
//---------------------------------------------------------------------------
tRisseVariantString * tRisseVariantString::FixLength()
{
	if(!this) return NULL;

	if(RefCount != 0) RisseThrowStringDeallocError();
	Length = Risse_strlen(this->operator const risse_char*());
	if(!Length)
	{
		RisseDeallocStringHeap(this);
		return NULL;
	}
	return this;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// other allocation functions
//---------------------------------------------------------------------------
tRisseVariantString * RisseAllocVariantString(const risse_char *ref1, const risse_char *ref2)
{
	if(!ref1 && !ref2) return NULL;

	if(ref1)
	{
		if(ref1[0]==0)
		{
			if(ref2)
			{
				if(ref2[0]==0) return NULL;
			}
		}
	}

	risse_int len1 = ref1?Risse_strlen(ref1):0;
	risse_int len2 = ref2?Risse_strlen(ref2):0;

	tRisseVariantString *ret = RisseAllocStringHeap();

	if(len1+len2>RISSE_VS_SHORT_LEN)
	{
		ret->LongString = RisseVS_malloc(len1+len2+1);
		if(ref1) Risse_strcpy(ret->LongString , ref1);
		if(ref2) Risse_strcpy(ret->LongString + len1, ref2);
	}
	else
	{
		if(ref1) Risse_strcpy(ret->ShortString, ref1);
		if(ref2) Risse_strcpy(ret->ShortString + len1, ref2);
	}
	ret->Length = len1+len2;
	return ret;
}
//---------------------------------------------------------------------------
tRisseVariantString * RisseAllocVariantString(const risse_char *ref)
{
	if(!ref) return NULL;
	if(ref[0]==0) return NULL;
	tRisseVariantString *ret = RisseAllocStringHeap();
	ret->SetString(ref);
	return ret;
}
//---------------------------------------------------------------------------


#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
//---------------------------------------------------------------------------
//! @brief		文字列管理ブロックを確保(wchar_tより)
//! @param		ref     コピー元文字列
//! @return		確保された文字列管理ブロック
//---------------------------------------------------------------------------
tRisseVariantString *RisseAllocVariantString(const wchar_t *ref)
{
	if(!ref) return NULL;
	if(ref[0]==0) return NULL;
	tRisseVariantString *ret = RisseAllocStringHeap();
	ret->SetString(ref);
	return ret;
}
//---------------------------------------------------------------------------
#endif


//---------------------------------------------------------------------------
tRisseVariantString * RisseAllocVariantString(const risse_char *ref, risse_int n)
{
	if(n==0) return NULL;
	if(!ref) return NULL;
	if(ref[0]==0) return NULL;
	tRisseVariantString *ret = RisseAllocStringHeap();
	ret->SetString(ref, n);
	return ret;
}
//---------------------------------------------------------------------------
tRisseVariantString * RisseAllocVariantString(const risse_uint8 **src)
{
	risse_uint size = *(const risse_uint *)(*src);
	*src += sizeof(risse_uint);
	if(!size) return 0;
	*src += sizeof(risse_uint);
	tRisseVariantString *ret = RisseAllocStringHeap();
	ret->SetString((const risse_char *)src, size);
	*src += sizeof(risse_char) * size;
	return ret;
}
//---------------------------------------------------------------------------
tRisseVariantString * RisseAllocVariantStringBuffer(risse_uint len)
{
	/* note that you must call FixLength if you allocate larger than the
	actual string size */


	tRisseVariantString *ret = RisseAllocStringHeap();
	ret->AllocBuffer(len);
	return ret;
}

//---------------------------------------------------------------------------
tRisseVariantString * RisseAppendVariantString(tRisseVariantString *str,
	const risse_char *app)
{
	if(!app) return str;
	if(!str)
	{
		str = RisseAllocVariantString(app);
		return str;
	}
	str->Append(app);
	return str;
}
//---------------------------------------------------------------------------
tRisseVariantString * RisseAppendVariantString(tRisseVariantString *str,
	const tRisseVariantString *app)
{
	if(!app) return str;
	if(!str)
	{
		str = RisseAllocVariantString(app->operator const risse_char *());
		return str;
	}
	str->Append(app->operator const risse_char *(), app->GetLength());
	return str;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
class tRisaVariantStringHolder
{
	// this class keeps one Variant String from program start to program end,
	// to ensure heap system are alive during program's lifetime.
	tRisseVariantString * String;
public:
	tRisaVariantStringHolder()
	{ String = RisseAllocVariantString(RISSE_WS("This is a dummy.")); }
	~tRisaVariantStringHolder()
	{ String->Release(); }
} static RisaVariantStringHolder;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define RISSE_VS_FS_OUT_INC_SIZE 32
tRisseVariantString * RisseFormatString(const risse_char *format, risse_uint numparams,
	tRisseVariant **params)
{
	return NULL;
#if 0
	// TODO: more reliable implementation

	// format the string with the format illustrated as "format"
	// this is similar to C sprintf format, but this support only following:
	// % [flags] [width] [.prec] type_char
	// flags : '-'|'+'|' '|'#'
	// width : n|'0'n|*|
	// .prec : '.0'|'.n'|'.*'
	// type_char : [diouxXfegEGcs]
	// [diouxX] as an integer
	// [fegEG] as a real
	// [cs] as a string
	// and, this is safe for output buffer memory or inadequate parameters.

	const risse_char *f = format;
	if(!f) return NULL;

	RisseSetFPUE();
	tRisseVariantString *ret = RisseAllocVariantStringBuffer(RISSE_VS_FS_OUT_INC_SIZE);
	risse_uint allocsize = RISSE_VS_FS_OUT_INC_SIZE;
	risse_char *o = const_cast<risse_char*>(ret->operator const risse_char*());
	risse_uint s = 0;

	risse_uint in = 0;

#define check_alloc \
			if(s >= allocsize) \
			{ \
				ret->AppendBuffer(RISSE_VS_FS_OUT_INC_SIZE); \
				o = const_cast<risse_char*>(ret->operator const risse_char*()); \
				allocsize += RISSE_VS_FS_OUT_INC_SIZE; \
			}

	for(;*f;)
	{
		if(*f!=RISSE_WC('%'))
		{
			check_alloc;
			o[s++] = *(f++);
			continue;
		}

		// following are only format-checking, actual processing is in sprintf.

		risse_char flag = 0;
		risse_uint width = 0;
//		bool zeropad = false;
		bool width_ind = false;
		risse_uint prec = 0;
//		bool precspec = false;
		bool prec_ind = false;
		const risse_char *fst = f;

		f++;
		if(!*f) goto error;

	// flags
		switch(*f)
		{
		case RISSE_WC('-'): flag = RISSE_WC('-'); break;
		case RISSE_WC('+'): flag = RISSE_WC('+'); break;
		case RISSE_WC('#'): flag = RISSE_WC('#'); break;
		default: goto width;
		}

		f++;
		if(!*f) goto error;

	width:
		switch(*f)
		{
		case RISSE_WC('0'): /*zeropad = true;*/ break;
		default: goto width_2;
		}

		f++;
		if(!*f) goto error;

	width_2:
		switch(*f)
		{
		case RISSE_WC('*'): width_ind = true; break;
		default: goto width_3;
		}

		f++;
		if(!*f) goto error;

		goto prec;

	width_3:
		while(true)
		{
			if(*f >= RISSE_WC('0') && *f <= RISSE_WC('9'))
				width = width *10 + (*f - RISSE_WC('0'));
			else
				break;
			f++;
		}

		if(!*f) goto error;

	prec:
		if(*f == RISSE_WC('.'))
		{
			f++;
			if(!*f) goto error;
			if(*f == RISSE_WC('*'))
			{
				prec_ind = true;
				f++;
				if(!*f) goto error;
				goto type_char;
			}
			if(*f < RISSE_WC('0') || *f > RISSE_WC('9')) goto error;
			/*precspec = true;*/
			do
			{
				prec = prec * 10 + (*f-RISSE_WC('0'));
				f++;
			} while(*f >= RISSE_WC('0') && *f <= RISSE_WC('9'));
		}

	type_char:
		switch(*f)
		{
		case RISSE_WC('%'):
			// literal '%'
			check_alloc;
			o[s++] = '%';
			f++;
			continue;


		case RISSE_WC('c'):
		case RISSE_WC('s'):
		  {
			if(width_ind)
			{
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				width = (risse_int)(params[in++])->AsInteger();
			}
			if(prec_ind)
			{
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				prec = (risse_int)(params[in++])->AsInteger();
			}
			if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
			tRisseVariantString *str = (params[in++])->AsString();
			if(str)
			{
				risse_uint slen = str->GetLength();
				if(!prec) prec = slen;
				if(*f == RISSE_WC('c') && prec > 1 ) prec = 1;
				if(width < prec) width = prec;
				if(s + width > allocsize)
				{
					try
					{
						risse_uint inc_size;
						ret->AppendBuffer(inc_size = s+width-allocsize+RISSE_VS_FS_OUT_INC_SIZE);
						o = const_cast<risse_char*>(ret->operator const risse_char*());
						allocsize += inc_size;
					}
					catch(...)
					{
						str->Release();
						throw;
					}
				}
				risse_uint pad = width - prec;
				if(pad)
				{
					if(flag == RISSE_WC('-'))
					{
						// left align
						if(str) Risse_strncpy(o+s, *str, prec);
						risse_char * p = o + s + prec;
						while(pad--) 0[p++] = RISSE_WC(' ');
					}
					else
					{
						// right align
						if(str) Risse_strncpy(o+s+pad, *str, prec);
						risse_char *p = o + s;
						while(pad--) 0[p++] = RISSE_WC(' ');
					}
					s += width;
				}
				else
				{
					if(str) Risse_strncpy(o+s, *str, prec);
					s += prec;
				}
				str->Release();
			}
			f++;
			continue;
		  }
		case RISSE_WC('d'): case RISSE_WC('i'): case RISSE_WC('o'): case RISSE_WC('u'):
		case RISSE_WC('x'): case RISSE_WC('X'):
		  {
			// integers
			if(width+prec > 900) goto error;// too long
			risse_char buf[1024];
			risse_char *p;
			risse_char fmt[70];
			risse_uint fmtlen = f - fst;
			if(fmtlen > 67) goto error;  // too long
			Risse_strncpy(fmt, fst, fmtlen);
			fmt[fmtlen] = RISSE_WC('L'); //// CHECK!! 'L' must indicate a 64bit integer
			fmt[fmtlen+1] = *f;
			fmt[fmtlen+2] = 0;
			int ind[2];
			if(!width_ind && !prec_ind)
			{
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				tTVInteger integer = (params[in++])->AsInteger();
				Risse_sprintf(buf, fmt, integer);
			}
			else if((!width_ind && prec_ind) || (width_ind && !prec_ind))
			{
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				ind[0] = (params[in++])->AsInteger();
				if(width_ind && ind[0] + prec > 900) goto error;
				if(prec_ind && ind[0] + width > 900) goto error;
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				tTVInteger integer = (params[in++])->AsInteger();
				Risse_sprintf(buf, fmt, ind[0], integer);
			}
			else
			{
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				ind[0] = (params[in++])->AsInteger();
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				ind[1] = (params[in++])->AsInteger();
				if(ind[0] + ind[1] > 900) goto error;
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				tTVInteger integer = (params[in++])->AsInteger();
				Risse_sprintf(buf, fmt, ind[0], ind[1], integer);
			}

			risse_uint size = Risse_strlen(buf);
			risse_uint inc_size;
			if(s+size > allocsize)
			{
				ret->AppendBuffer(inc_size = s+size-allocsize+RISSE_VS_FS_OUT_INC_SIZE);
				o = const_cast<risse_char*>(ret->operator const risse_char*());
				allocsize += inc_size;
			}
			Risse_strcpy(o+s, buf);
			s += size;

			f++;
			continue;
		  }
		case RISSE_WC('f'): case RISSE_WC('e'): case RISSE_WC('g'): case RISSE_WC('E'):
		case RISSE_WC('G'):
		  {
			// reals
 			if(width+prec > 900) goto error;// too long
			risse_char buf[1024];
			risse_char fmt[70];
			risse_uint fmtlen = f - fst;
			if(fmtlen > 67) goto error;  // too long
			Risse_strncpy(fmt, fst, fmtlen);
			fmt[fmtlen] = RISSE_WC('l'); //// CHECK!! 'l' must indicate a 64bit real
			fmt[fmtlen+1] = *f;
			fmt[fmtlen+2] = 0;
			int ind[2];
			if(!width_ind && !prec_ind)
			{
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				tTVReal real = (params[in++])->AsReal();
				Risse_sprintf(buf, fmt, real);
			}
			else if((!width_ind && prec_ind) || (width_ind && !prec_ind))
			{
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				ind[0] = (params[in++])->AsInteger();
 				if(width_ind && ind[0] + prec > 900) goto error;
				if(prec_ind && ind[0] + width > 900) goto error;
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				tTVReal real = (params[in++])->AsReal();
				Risse_sprintf(buf, fmt, ind[0], real);
			}
			else
			{
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				ind[0] = (params[in++])->AsInteger();
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				ind[1] = (params[in++])->AsInteger();
				if(ind[0] + ind[1] > 900) goto error;
				if(in>=numparams) Risse_eRisseVariantError(RisseBadParamCount);
				tTVReal real = (params[in++])->AsReal();
				Risse_sprintf(buf, fmt, ind[0], ind[1], real);
			}

			risse_uint size = Risse_strlen(buf);
			risse_uint inc_size;
			if(s+size > allocsize)
			{
				ret->AppendBuffer(inc_size = s+size-allocsize+RISSE_VS_FS_OUT_INC_SIZE);
				o = const_cast<risse_char*>(ret->operator const risse_char*());
				allocsize += inc_size;
			}
			Risse_strcpy(o+s, buf);
			s += size;
			f++;
			continue;
		  }
		}

	}

	o[s] = 0;

	ret = ret->FixLength();

	return ret;

error:
	Risse_eRisseVariantError(RisseInvalidFormatString);
	return NULL; // not reached
#endif
}
//---------------------------------------------------------------------------

} // namespace Risse

