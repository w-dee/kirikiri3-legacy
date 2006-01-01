//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief tTJSVariant や tTJSString で使われる文字列の管理
//---------------------------------------------------------------------------
#ifndef tjsVariantStringH
#define tjsVariantStringH

#include "tjsConfig.h"
#include <stdlib.h>
#include <string.h>
#include "tjsCharUtils.h"

namespace TJS
{

// #define TJS_DEBUG_UNRELEASED_STRING
// #define TJS_DEBUG_CHECK_STRING_HEAP_INTEGRITY
// #define TJS_DEBUG_DUMP_STRING

/*[*/
//---------------------------------------------------------------------------
// tTJSVariantString stuff
//---------------------------------------------------------------------------
#define TJS_VS_SHORT_LEN 10
/*]*/
struct tTJSVariantString;
extern tjs_int TJSGetShorterStrLen(const tjs_char *str, tjs_int max);
extern tTJSVariantString * TJSAllocStringHeap(void);
extern void TJSDeallocStringHeap(tTJSVariantString * vs);
extern void TJSThrowStringAllocError();
extern void TJSThrowNarrowToWideConversionError();
extern void TJSCompactStringHeap();
#ifdef TJS_DEBUG_DUMP_STRING
extern void TJSDumpStringHeap(void);
#endif
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// base memory allocation functions for long string
//---------------------------------------------------------------------------
tjs_char * TJSVS_malloc(tjs_uint len);
tjs_char * TJSVS_realloc(tjs_char *buf, tjs_uint len);
void TJSVS_free(tjs_char *buf);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTJSVariantString
//---------------------------------------------------------------------------
class tTJSVariant;


struct tTJSVariantString_S
{
	tjs_size RefCount; // reference count - 1
	tjs_char *LongString;
	tjs_char ShortString[TJS_VS_SHORT_LEN +1];
	tjs_size Length; // string length
	tjs_uint32 HeapFlag;
	tjs_uint32 Hint;
};


class tTJSVariantString : public tTJSVariantString_S
{
public:

	void AddRef()
	{
		RefCount++;
	}

	void Release();

	void SetString(const tjs_char *ref, tjs_int maxlen = -1)
	{
		if(LongString) TJSVS_free(LongString), LongString = NULL;
		tjs_int len;
		if(maxlen != -1)
			len = TJSGetShorterStrLen(ref, maxlen);
		else
			len = TJS_strlen(ref);

		Length = len;
		if(len>TJS_VS_SHORT_LEN)
		{
			LongString = TJSVS_malloc(len+1);
			TJS_strcpy_maxlen(LongString, ref, len);
		}
		else
		{
			TJS_strcpy_maxlen(ShortString, ref, len);
		}
	}

#ifdef TJS_WCHAR_T_SIZE_IS_16BIT
	//! @brief		文字列の設定 (wchar_t より)
	//! @param		ref 文字列
	void SetString(const wchar_t *ref)
	{
		tjs_size len = 1;
		const wchar_t *p = ref;
		while(*p) len++, p++;  // 文字列長を取得
		tjs_char *tjsstr = new tjs_char[len];
		try
		{
			TJSConvertUTF16ToTJSCharString(tjsstr,
					reinterpret_cast<const tjs_uint16 *>(ref)); // UTF16 を UTF32 に変換
			SetString(tjsstr);
		}
		catch(...)
		{
			delete [] tjsstr;
			throw;
		}
		delete [] tjsstr;
	}
#endif

	void AllocBuffer(tjs_uint len)
	{
		/* note that you must call FixLength if you allocate larger than the
			actual string size */

		if(LongString) TJSVS_free(LongString), LongString = NULL;

		Length = len;
		if(len>TJS_VS_SHORT_LEN)
		{
			LongString = TJSVS_malloc(len+1);
			LongString[len] = 0;
		}
		else
		{
			ShortString[len] = 0;
		}
	}

	void ResetString(const tjs_char *ref)
	{
		if(LongString) TJSVS_free(LongString), LongString = NULL;
		SetString(ref);
	}

#ifdef TJS_WCHAR_T_SIZE_IS_16BIT
	void ResetString(const wchar_t *ref)
	{
		if(LongString) TJSVS_free(LongString), LongString = NULL;
		SetString(ref);
	}
#endif


	void AppendBuffer(tjs_uint applen)
	{
		/* note that you must call FixLength if you allocate larger than the
			actual string size */

		// assume this != NULL
		tjs_int newlen = Length += applen;
		if(LongString)
		{
			// still long string
			LongString = TJSVS_realloc(LongString, newlen + 1);
			LongString[newlen] = 0;
			return;
		}
		else
		{
			if(newlen <= TJS_VS_SHORT_LEN)
			{
				// still short string
				ShortString[newlen] = 0;
				return;
			}
			// becomes a long string
			tjs_char *newbuf = TJSVS_malloc(newlen+1);
			TJS_strcpy(newbuf, ShortString);
			LongString = newbuf;
			LongString[newlen] = 0;
			return;
		}

	}

	void Append(const tjs_char *str)
	{
		// assume this != NULL
		Append(str, TJS_strlen(str));
	}

	void Append(const tjs_char *str, tjs_int applen)
	{
		// assume this != NULL
		tjs_int orglen = Length;
		tjs_int newlen = Length += applen;
		if(LongString)
		{
			// still long string
			LongString = TJSVS_realloc(LongString, newlen + 1);
			TJS_strcpy(LongString+orglen, str);
			return;
		}
		else
		{
			if(newlen <= TJS_VS_SHORT_LEN)
			{
				// still short string
				TJS_strcpy(ShortString + orglen, str);
				return;
			}
			// becomes a long string
			tjs_char *newbuf = TJSVS_malloc(newlen+1);
			TJS_strcpy(newbuf, ShortString);
			TJS_strcpy(newbuf+orglen, str);
			LongString = newbuf;
			return;
		}
	}

	operator const tjs_char *() const
	{
		return (!this)?(NULL):(LongString?LongString:ShortString);
	}

	operator tjs_char *()
	{
		return (!this)?(NULL):(LongString?LongString:ShortString);
	}

	tjs_int GetLength() const
	{
		if(!this) return 0;
		return Length;
	}

	tTJSVariantString *FixLength();

	tjs_uint32 *GetHint() { return &Hint; }

	tTVInteger ToInteger() const;
	tTVReal ToReal() const;
	void ToNumber(tTJSVariant &dest) const;

	tjs_int GetRefCount()
	{
		return RefCount;
	}

	tjs_int QueryPersistSize() const
	{
		return sizeof(tjs_uint) +
			GetLength() * sizeof(tjs_char);
	}

	void Persist(tjs_uint8 *dest) const
	{
		tjs_uint size;
		const tjs_char *ptr = LongString?LongString:ShortString;
		*(tjs_uint*)dest = size = GetLength();
		dest += sizeof(tjs_uint);
		while(size--)
		{
			*(tjs_char*)dest = *ptr;
			dest += sizeof(tjs_char);
			ptr++;
		}
	}
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tTJSVariantString *TJSAllocVariantString(const tjs_char *ref1,
	const tjs_char *ref2);
tTJSVariantString *TJSAllocVariantString(const tjs_char *ref, tjs_int n);
tTJSVariantString *TJSAllocVariantString(const tjs_char *ref);
#ifdef TJS_WCHAR_T_SIZE_IS_16BIT
tTJSVariantString *TJSAllocVariantString(const wchar_t *ref);
#endif
tTJSVariantString *TJSAllocVariantString(const tjs_uint8 **src);
tTJSVariantString *TJSAllocVariantStringBuffer(tjs_uint len);
tTJSVariantString *TJSAppendVariantString(tTJSVariantString *str,
	const tjs_char *app);
tTJSVariantString *TJSAppendVariantString(tTJSVariantString *str,
	const tTJSVariantString *app);
tTJSVariantString *TJSFormatString(const tjs_char *format, tjs_uint numparams,
	tTJSVariant **params);

//---------------------------------------------------------------------------
} // namespace TJS
#endif
