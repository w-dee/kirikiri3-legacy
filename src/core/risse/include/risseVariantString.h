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
#ifndef risseVariantStringH
#define risseVariantStringH

#include "risseConfig.h"
#include <stdlib.h>
#include <string.h>
#include "risseCharUtils.h"

namespace Risse
{

// #define RISSE_DEBUG_UNRELEASED_STRING
// #define RISSE_DEBUG_CHECK_STRING_HEAP_INTEGRITY
// #define RISSE_DEBUG_DUMP_STRING

/*[*/
//---------------------------------------------------------------------------
// tRisseVariantString stuff
//---------------------------------------------------------------------------
#define RISSE_VS_SHORT_LEN 10
/*]*/
struct tRisseVariantString;
extern risse_int RisseGetShorterStrLen(const risse_char *str, risse_int max);
extern tRisseVariantString * RisseAllocStringHeap(void);
extern void RisseDeallocStringHeap(tRisseVariantString * vs);
extern void RisseThrowStringAllocError();
extern void RisseThrowNarrowToWideConversionError();
extern void RisseCompactStringHeap();
#ifdef RISSE_DEBUG_DUMP_STRING
extern void RisseDumpStringHeap(void);
#endif
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// base memory allocation functions for long string
//---------------------------------------------------------------------------
risse_char * RisseVS_malloc(risse_uint len);
risse_char * RisseVS_realloc(risse_char *buf, risse_uint len);
void RisseVS_free(risse_char *buf);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseVariantString
//---------------------------------------------------------------------------
class tRisseVariant;


struct tRisseVariantString_S
{
	risse_size RefCount; // reference count - 1
	risse_char *LongString;
	risse_char ShortString[RISSE_VS_SHORT_LEN +1];
	risse_size Length; // string length
	risse_uint32 HeapFlag;
	risse_uint32 Hint;
};


class tRisseVariantString : public tRisseVariantString_S
{
public:

	void AddRef()
	{
		RefCount++;
	}

	void Release();

	void SetString(const risse_char *ref, risse_int maxlen = -1)
	{
		if(LongString) RisseVS_free(LongString), LongString = NULL;
		risse_int len;
		if(maxlen != -1)
			len = RisseGetShorterStrLen(ref, maxlen);
		else
			len = Risse_strlen(ref);

		Length = len;
		if(len>RISSE_VS_SHORT_LEN)
		{
			LongString = RisseVS_malloc(len+1);
			Risse_strcpy_maxlen(LongString, ref, len);
		}
		else
		{
			Risse_strcpy_maxlen(ShortString, ref, len);
		}
	}

#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	//! @brief		文字列の設定 (wchar_t より)
	//! @param		ref 文字列
	void SetString(const wchar_t *ref)
	{
		risse_size len = 1;
		const wchar_t *p = ref;
		while(*p) len++, p++;  // 文字列長を取得
		risse_char *rissestr = new risse_char[len];
		try
		{
			RisseConvertUTF16ToRisseCharString(rissestr,
					reinterpret_cast<const risse_uint16 *>(ref)); // UTF16 を UTF32 に変換
			SetString(rissestr);
		}
		catch(...)
		{
			delete [] rissestr;
			throw;
		}
		delete [] rissestr;
	}
#endif

	void AllocBuffer(risse_uint len)
	{
		/* note that you must call FixLength if you allocate larger than the
			actual string size */

		if(LongString) RisseVS_free(LongString), LongString = NULL;

		Length = len;
		if(len>RISSE_VS_SHORT_LEN)
		{
			LongString = RisseVS_malloc(len+1);
			LongString[len] = 0;
		}
		else
		{
			ShortString[len] = 0;
		}
	}

	void ResetString(const risse_char *ref)
	{
		if(LongString) RisseVS_free(LongString), LongString = NULL;
		SetString(ref);
	}

#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	void ResetString(const wchar_t *ref)
	{
		if(LongString) RisseVS_free(LongString), LongString = NULL;
		SetString(ref);
	}
#endif


	void AppendBuffer(risse_uint applen)
	{
		/* note that you must call FixLength if you allocate larger than the
			actual string size */

		// assume this != NULL
		risse_int newlen = Length += applen;
		if(LongString)
		{
			// still long string
			LongString = RisseVS_realloc(LongString, newlen + 1);
			LongString[newlen] = 0;
			return;
		}
		else
		{
			if(newlen <= RISSE_VS_SHORT_LEN)
			{
				// still short string
				ShortString[newlen] = 0;
				return;
			}
			// becomes a long string
			risse_char *newbuf = RisseVS_malloc(newlen+1);
			Risse_strcpy(newbuf, ShortString);
			LongString = newbuf;
			LongString[newlen] = 0;
			return;
		}

	}

	void Append(const risse_char *str)
	{
		// assume this != NULL
		Append(str, Risse_strlen(str));
	}

	void Append(const risse_char *str, risse_int applen)
	{
		// assume this != NULL
		risse_int orglen = Length;
		risse_int newlen = Length += applen;
		if(LongString)
		{
			// still long string
			LongString = RisseVS_realloc(LongString, newlen + 1);
			Risse_strcpy(LongString+orglen, str);
			return;
		}
		else
		{
			if(newlen <= RISSE_VS_SHORT_LEN)
			{
				// still short string
				Risse_strcpy(ShortString + orglen, str);
				return;
			}
			// becomes a long string
			risse_char *newbuf = RisseVS_malloc(newlen+1);
			Risse_strcpy(newbuf, ShortString);
			Risse_strcpy(newbuf+orglen, str);
			LongString = newbuf;
			return;
		}
	}

	operator const risse_char *() const
	{
		return (!this)?(NULL):(LongString?LongString:ShortString);
	}

	operator risse_char *()
	{
		return (!this)?(NULL):(LongString?LongString:ShortString);
	}

	risse_int GetLength() const
	{
		if(!this) return 0;
		return Length;
	}

	tRisseVariantString *FixLength();

	risse_uint32 *GetHint() { return &Hint; }

	tTVInteger ToInteger() const;
	tTVReal ToReal() const;
	void ToNumber(tRisseVariant &dest) const;

	risse_int GetRefCount()
	{
		return RefCount;
	}

	risse_int QueryPersistSize() const
	{
		return sizeof(risse_uint) +
			GetLength() * sizeof(risse_char);
	}

	void Persist(risse_uint8 *dest) const
	{
		risse_uint size;
		const risse_char *ptr = LongString?LongString:ShortString;
		*(risse_uint*)dest = size = GetLength();
		dest += sizeof(risse_uint);
		while(size--)
		{
			*(risse_char*)dest = *ptr;
			dest += sizeof(risse_char);
			ptr++;
		}
	}
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tRisseVariantString *RisseAllocVariantString(const risse_char *ref1,
	const risse_char *ref2);
tRisseVariantString *RisseAllocVariantString(const risse_char *ref, risse_int n);
tRisseVariantString *RisseAllocVariantString(const risse_char *ref);
#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
tRisseVariantString *RisseAllocVariantString(const wchar_t *ref);
#endif
tRisseVariantString *RisseAllocVariantString(const risse_uint8 **src);
tRisseVariantString *RisseAllocVariantStringBuffer(risse_uint len);
tRisseVariantString *RisseAppendVariantString(tRisseVariantString *str,
	const risse_char *app);
tRisseVariantString *RisseAppendVariantString(tRisseVariantString *str,
	const tRisseVariantString *app);
tRisseVariantString *RisseFormatString(const risse_char *format, risse_uint numparams,
	tRisseVariant **params);

//---------------------------------------------------------------------------
} // namespace Risse
#endif
