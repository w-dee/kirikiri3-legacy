//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 文字列クラス tTJSString ( ttstr ) の実装
//---------------------------------------------------------------------------

#ifndef tjsStringH
#define tjsStringH

#include "tjsConfig.h"
#ifdef TJS_SUPPORT_VCL
	#include <vcl.h>
#endif
#include "tjsVariantString.h"


namespace TJS
{
/*[*/
//---------------------------------------------------------------------------
// tTJSStringBufferLength
//---------------------------------------------------------------------------
#pragma pack(push, 4)
class tTJSStringBufferLength
{
public:
	tjs_int n;
	tTJSStringBufferLength(tjs_int n) {this->n = n;}
};
#pragma pack(pop)
/*]*/
//---------------------------------------------------------------------------





class tTJSVariant;
extern const tjs_char *TJSNullStrPtr;

/*[*/
//---------------------------------------------------------------------------
// tTJSString
//---------------------------------------------------------------------------
#pragma pack(push, 4)
class tTJSVariantString;
struct tTJSString_S
{
	tTJSVariantString *Ptr;
};
#pragma pack(pop)
class tTJSString;
/*]*/

/*start-of-tTJSString*/
class tTJSString : protected tTJSString_S
{
public:
	//-------------------------------------------------------- constructor --
	tTJSString() { Ptr = NULL; }
	tTJSString(const tTJSString &rhs) { Ptr = rhs.Ptr; if(Ptr) Ptr->AddRef(); }
	tTJSString(tTJSVariantString *vstr)   { Ptr = vstr; if(Ptr) Ptr->AddRef(); }
	tTJSString(const tjs_char *str) { Ptr = TJSAllocVariantString(str); }
#ifdef TJS_WCHAR_T_SIZE_IS_16BIT
	tTJSString(const wchar_t *str) { Ptr = TJSAllocVariantString(str); }
#endif
	tTJSString(const tTJSStringBufferLength len)
		{ Ptr = TJSAllocVariantStringBuffer(len.n); }

	tTJSString(const tTJSVariant & val);

	tTJSString(const tTJSString &str, int n) // construct with first n chars of str
		{ Ptr = TJSAllocVariantString(str.c_str(), n); }

	tTJSString(const tjs_char *str, int n) // same as above except for str's type
		{ Ptr = TJSAllocVariantString(str, n); }
	tTJSString(tjs_int n); // from int
	tTJSString(const tTJSString &str, const tTJSString &s1);
	tTJSString(const tTJSString &str, const tTJSString &s1, const tTJSString &s2);
	tTJSString(const tTJSString &str, const tTJSString &s1, const tTJSString &s2, const tTJSString &s3);
	tTJSString(const tTJSString &str, const tTJSString &s1, const tTJSString &s2,
		const tTJSString &s3, const tTJSString &s4);

	//--------------------------------------------------------- destructor --
	~tTJSString() { if(Ptr) Ptr->Release(); }

	//--------------------------------------------------------- conversion --
	const tjs_char * c_str() const
		{ return Ptr?Ptr->operator const tjs_char *():TJSNullStrPtr; }

#ifdef TJS_SUPPORT_WX
	wxString AsWxString() const
	{
		if(!Ptr) return wxString();

		return TJSCharToWxString(Ptr->operator const tjs_char *());
	}
#endif

	tTJSVariantString * AsVariantStringNoAddRef() const
	{
		return Ptr;
	}

	tjs_int64 AsInteger() const;

	//------------------------------------------------------- substitution --
	tTJSString & operator =(const tTJSString & rhs)
	{
		if(rhs.Ptr) rhs.Ptr->AddRef();
		if(Ptr) Ptr->Release();
		Ptr = rhs.Ptr;
		return *this;
	}

	tTJSString & operator =(const tjs_char * rhs)
	{
		if(Ptr)
		{
			Independ();
			if(rhs && rhs[0])
				Ptr->ResetString(rhs);
			else
				Ptr->Release(), Ptr = NULL;
		}
		else
		{
			Ptr = TJSAllocVariantString(rhs);
		}
		return *this;
	}

#ifdef TJS_WCHAR_T_SIZE_IS_16BIT
	tTJSString & operator =(const wchar_t * rhs)
	{
		if(Ptr)
		{
			Independ();
			if(rhs && rhs[0])
				Ptr->ResetString(rhs);
			else
				Ptr->Release(), Ptr = NULL;
		}
		else
		{
			Ptr = TJSAllocVariantString(rhs);
		}
		return *this;
	}
#endif

#ifdef TJS_SUPPORT_WX
	tTJSString & operator =(wxString &rhs)
	{
		if(Ptr) Ptr->Release();
		Ptr = TJSAllocVariantString(rhs.c_str());
		return *this;
	}
#endif

	//------------------------------------------------------------ compare --
	bool operator ==(const tTJSString & ref) const
	{
		if(Ptr == ref.Ptr) return true; // both empty or the same pointer
		if(!Ptr && ref.Ptr) return false;
		if(Ptr && !ref.Ptr) return false;
		if(Ptr->Length != ref.Ptr->Length) return false;
		return !TJS_strcmp(*Ptr, *ref.Ptr);
	}

	bool operator !=(const tTJSString &ref) const
	{
		return ! this->operator == (ref);
	}

	tjs_int CompareAsciiIC(const tTJSString & ref) const
	{
		if(!Ptr && !ref.Ptr) return true; // both empty string
		if(!Ptr && ref.Ptr) return false;
		if(Ptr && !ref.Ptr) return false;
		return TJS_straicmp(*Ptr, *ref.Ptr);
	}

	bool operator ==(const tjs_char * ref) const
	{
		bool rnemp = ref && ref[0];
		if(!Ptr && !rnemp) return true; // both empty string
		if(!Ptr && rnemp) return false;
		if(Ptr && !rnemp) return false;
		return !TJS_strcmp(*Ptr, ref);
	}

	bool operator !=(const tjs_char * ref) const
	{
		return ! this->operator == (ref);
	}

	tjs_int CompareAsciiIC(const tjs_char * ref) const
	{
		bool rnemp = ref && ref[0];
		if(!Ptr && !rnemp) return true; // both empty string
		if(!Ptr && rnemp) return false;
		if(Ptr && !rnemp) return false;
		return TJS_straicmp(*Ptr, ref);
	}

	bool operator < (const tTJSString &ref) const
	{
		if(!Ptr && !ref.Ptr) return false;
		if(!Ptr && ref.Ptr) return true;
		if(Ptr && !ref.Ptr) return false;
		return TJS_strcmp(*Ptr, *ref.Ptr)<0;
	}

	bool operator > (const tTJSString &ref) const
	{
		if(!Ptr && !ref.Ptr) return false;
		if(!Ptr && ref.Ptr) return false;
		if(Ptr && !ref.Ptr) return true;
		return TJS_strcmp(*Ptr, *ref.Ptr)>0;
	}

	//---------------------------------------------------------- operation --
	void operator +=(const tTJSString &ref)
	{
		if(!ref.Ptr) return;
		Independ();
		Ptr = TJSAppendVariantString(Ptr, *ref.Ptr);
	}

	void operator +=(const tTJSVariantString *ref)
	{
		if(!ref) return;
		Independ();
		Ptr = TJSAppendVariantString(Ptr, ref);
	}

	void operator +=(const tjs_char *ref)
	{
		if(!ref) return;
		Independ();
		Ptr = TJSAppendVariantString(Ptr, ref);
	}

	void operator +=(tjs_char rch)
	{
		Independ();
		tjs_char ch[2];
		ch[0] = rch;
		ch[1] = 0;
		Ptr = TJSAppendVariantString(Ptr, ch);
	}

	tTJSString operator +(const tTJSString &ref) const
	{
		if(!ref.Ptr && !Ptr) return tTJSString();
		if(!ref.Ptr) return *this;
		if(!Ptr) return ref;

		tTJSString newstr;
		newstr.Ptr = TJSAllocVariantString(*this->Ptr, *ref.Ptr);
		return newstr;
	}

	tTJSString operator +(const tjs_char *ref) const
	{
		if(!ref && !Ptr) return tTJSString();
		if(!ref) return *this;
		if(!Ptr) return tTJSString(ref);

		tTJSString newstr;
		newstr.Ptr = TJSAllocVariantString(*this->Ptr, ref);
		return newstr;
	}

	tTJSString operator +(tjs_char rch) const
	{
		if(!Ptr) return tTJSString(rch);
		tjs_char ch[2];
		ch[0] = rch;
		ch[1] = 0;
		tTJSString newstr;
		newstr.Ptr = TJSAllocVariantString(*this->Ptr, ch);
		return newstr;
	}

	friend tTJSString operator + (const tjs_char *lhs, const tTJSString &rhs);

	tjs_char operator [](tjs_uint i) const
	{
		// returns character at i. this function does not check the range.
		if(!Ptr) return 0;
		return Ptr->operator const tjs_char *() [i];
	}

	void Clear()
	{
		if(Ptr) Ptr->Release(), Ptr = NULL;
	}

	tjs_char * AllocBuffer(tjs_uint len)
	{
		/* you must call FixLen when you allocate larger buffer than actual string length */

		if(Ptr) Ptr->Release();
		Ptr = TJSAllocVariantStringBuffer(len);
		return const_cast<tjs_char*>(Ptr->operator const tjs_char *());
	}

	tjs_char * AppendBuffer(tjs_uint len)
	{
		/* you must call FixLen when you allocate larger buffer than actual string length */

		if(!Ptr) return AllocBuffer(len);
		Independ();
		Ptr->AppendBuffer(len);
		return const_cast<tjs_char *>(Ptr->operator const tjs_char *());
	}


	void FixLen()
	{
		Independ();
		if(Ptr) Ptr = Ptr->FixLength();
	}

	void Replace
		(const tTJSString &from, const tTJSString &to, bool forall = true);

	tTJSString AsLowerCase() const;
	tTJSString AsUpperCase() const;

	void AsLowerCase(tTJSString &dest) const { dest = AsLowerCase(); }
	void AsUpperCase(tTJSString &dest) const { dest = AsUpperCase(); }

	void ToLowerCase();
	void ToUppserCase();

	tjs_int TJS_cdecl printf(const tjs_char *format, ...);

	tTJSString EscapeC() const;   // c-style string escape/unescaep
	tTJSString UnescapeC() const;

	void EscapeC(tTJSString &dest) const { dest = EscapeC(); }
	void UnescapeC(tTJSString &dest) const { dest = UnescapeC(); }

	bool StartsWith(const tjs_char *string) const;
	bool StartsWith(const tTJSString & string) const
		{ return StartsWith(string.c_str()); }
	bool StartsWith(tjs_char ch) const
		{
			if(GetLen() == 0)
				return ch == 0;
			return c_str()[0] == ch;
		}

	bool EndsWith(const tjs_char *string) const;
	bool EndsWith(const tTJSString & string) const
		{ return StartsWith(string.c_str()); }
	bool EndsWith(tjs_char ch) const
		{
			if(GetLen() == 0)
				return ch == 0;
			return c_str()[GetLen() - 1] == ch;
		}

	tjs_uint32 * GetHint() const { if(!Ptr) return NULL; return Ptr->GetHint(); }

	//------------------------------------------------------------- others --
	bool IsEmpty() const { return Ptr==NULL; }

private:
	tjs_char * InternalIndepend();

public:
	tjs_char * Independ()
	{
		// severs sharing of the string instance
		// and returns independent internal buffer

		// note that you must call FixLen after making modification of the buffer
		// if you shorten the string using this method's return value.
		// USING THIS METHOD'S RETURN VALUE AND MODIFYING THE INTERNAL
		// BUFFER IS VERY DANGER.

		if(!Ptr) return NULL;

		if(Ptr->GetRefCount() == 0)
		{
			// already indepentent
			return const_cast<tjs_char *>(Ptr->operator const tjs_char *());
		}
		return InternalIndepend();
	}


	tjs_int GetLen() const
	{
#ifdef __CODEGUARD__
		if(!Ptr) return 0; // tTJSVariantString::GetLength can return zero if 'this' is NULL
#endif
		return Ptr->GetLength();
	}

	tjs_int length() const { return GetLen(); }

	tjs_char GetLastChar() const
	{
		if(!Ptr) return (tjs_char)0;
		const tjs_char * p = Ptr->operator const tjs_char*();
		return p[Ptr->GetLength() - 1];
	}
};
/*end-of-tTJSString*/
tTJSString operator +(const tjs_char *lhs, const tTJSString &rhs);


//---------------------------------------------------------------------------
tTJSString TJSInt32ToHex(tjs_uint32 num, int zeropad = 8);
//---------------------------------------------------------------------------
/*[*/
typedef tTJSString ttstr;
/*]*/
//---------------------------------------------------------------------------
} // namespace TJS
#endif
