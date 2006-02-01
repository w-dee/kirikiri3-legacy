//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 文字列クラス tRisseString ( ttstr ) の実装
//---------------------------------------------------------------------------

#ifndef risseStringH
#define risseStringH

#include "risseConfig.h"
#ifdef RISSE_SUPPORT_VCL
	#include <vcl.h>
#endif
#include "risseVariantString.h"


namespace Risse
{
/*[*/
//---------------------------------------------------------------------------
// tRisseStringBufferLength
//---------------------------------------------------------------------------
#pragma pack(push, 4)
class tRisseStringBufferLength
{
public:
	risse_int n;
	tRisseStringBufferLength(risse_int n) {this->n = n;}
};
#pragma pack(pop)
/*]*/
//---------------------------------------------------------------------------





class tRisseVariant;
extern const risse_char *RisseNullStrPtr;

/*[*/
//---------------------------------------------------------------------------
// tRisseString
//---------------------------------------------------------------------------
#pragma pack(push, 4)
class tRisseVariantString;
struct tRisseString_S
{
	tRisseVariantString *Ptr;
};
#pragma pack(pop)
class tRisseString;
/*]*/

/*start-of-tRisseString*/
class tRisseString : protected tRisseString_S
{
public:
	//-------------------------------------------------------- constructor --
	tRisseString() { Ptr = NULL; }
	tRisseString(const tRisseString &rhs) { Ptr = rhs.Ptr; if(Ptr) Ptr->AddRef(); }
	tRisseString(tRisseVariantString *vstr)   { Ptr = vstr; if(Ptr) Ptr->AddRef(); }
	tRisseString(const risse_char *str) { Ptr = RisseAllocVariantString(str); }
#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	tRisseString(const wchar_t *str) { Ptr = RisseAllocVariantString(str); }
#endif
	tRisseString(const tRisseStringBufferLength len)
		{ Ptr = RisseAllocVariantStringBuffer(len.n); }

	tRisseString(const tRisseVariant & val);

	tRisseString(const tRisseString &str, int n) // construct with first n chars of str
		{ Ptr = RisseAllocVariantString(str.c_str(), n); }

	tRisseString(const risse_char *str, int n) // same as above except for str's type
		{ Ptr = RisseAllocVariantString(str, n); }
	tRisseString(risse_int n); // from int
	tRisseString(const tRisseString &str, const tRisseString &s1);
	tRisseString(const tRisseString &str, const tRisseString &s1, const tRisseString &s2);
	tRisseString(const tRisseString &str, const tRisseString &s1, const tRisseString &s2, const tRisseString &s3);
	tRisseString(const tRisseString &str, const tRisseString &s1, const tRisseString &s2,
		const tRisseString &s3, const tRisseString &s4);

	//--------------------------------------------------------- destructor --
	~tRisseString() { if(Ptr) Ptr->Release(); }

	//--------------------------------------------------------- conversion --
	const risse_char * c_str() const
		{ return Ptr?Ptr->operator const risse_char *():RisseNullStrPtr; }

#ifdef RISSE_SUPPORT_WX
	wxString AsWxString() const
	{
		if(!Ptr) return wxString();

		return RisseCharToWxString(Ptr->operator const risse_char *());
	}
#endif

	tRisseVariantString * AsVariantStringNoAddRef() const
	{
		return Ptr;
	}

	risse_int64 AsInteger() const;

	//------------------------------------------------------- substitution --
	tRisseString & operator =(const tRisseString & rhs)
	{
		if(rhs.Ptr) rhs.Ptr->AddRef();
		if(Ptr) Ptr->Release();
		Ptr = rhs.Ptr;
		return *this;
	}

	tRisseString & operator =(const risse_char * rhs)
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
			Ptr = RisseAllocVariantString(rhs);
		}
		return *this;
	}

#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	tRisseString & operator =(const wchar_t * rhs)
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
			Ptr = RisseAllocVariantString(rhs);
		}
		return *this;
	}
#endif

#ifdef RISSE_SUPPORT_WX
	tRisseString & operator =(wxString &rhs)
	{
		if(Ptr) Ptr->Release();
		Ptr = RisseAllocVariantString(rhs.c_str());
		return *this;
	}
#endif

	//------------------------------------------------------------ compare --
	bool operator ==(const tRisseString & ref) const
	{
		if(Ptr == ref.Ptr) return true; // both empty or the same pointer
		if(!Ptr && ref.Ptr) return false;
		if(Ptr && !ref.Ptr) return false;
		if(Ptr->Length != ref.Ptr->Length) return false;
		return !Risse_strcmp(*Ptr, *ref.Ptr);
	}

	bool operator !=(const tRisseString &ref) const
	{
		return ! this->operator == (ref);
	}

	risse_int CompareAsciiIC(const tRisseString & ref) const
	{
		if(!Ptr && !ref.Ptr) return true; // both empty string
		if(!Ptr && ref.Ptr) return false;
		if(Ptr && !ref.Ptr) return false;
		return Risse_straicmp(*Ptr, *ref.Ptr);
	}

	bool operator ==(const risse_char * ref) const
	{
		bool rnemp = ref && ref[0];
		if(!Ptr && !rnemp) return true; // both empty string
		if(!Ptr && rnemp) return false;
		if(Ptr && !rnemp) return false;
		return !Risse_strcmp(*Ptr, ref);
	}

	bool operator !=(const risse_char * ref) const
	{
		return ! this->operator == (ref);
	}

	risse_int CompareAsciiIC(const risse_char * ref) const
	{
		bool rnemp = ref && ref[0];
		if(!Ptr && !rnemp) return true; // both empty string
		if(!Ptr && rnemp) return false;
		if(Ptr && !rnemp) return false;
		return Risse_straicmp(*Ptr, ref);
	}

	bool operator < (const tRisseString &ref) const
	{
		if(!Ptr && !ref.Ptr) return false;
		if(!Ptr && ref.Ptr) return true;
		if(Ptr && !ref.Ptr) return false;
		return Risse_strcmp(*Ptr, *ref.Ptr)<0;
	}

	bool operator > (const tRisseString &ref) const
	{
		if(!Ptr && !ref.Ptr) return false;
		if(!Ptr && ref.Ptr) return false;
		if(Ptr && !ref.Ptr) return true;
		return Risse_strcmp(*Ptr, *ref.Ptr)>0;
	}

	//---------------------------------------------------------- operation --
	void operator +=(const tRisseString &ref)
	{
		if(!ref.Ptr) return;
		Independ();
		Ptr = RisseAppendVariantString(Ptr, *ref.Ptr);
	}

	void operator +=(const tRisseVariantString *ref)
	{
		if(!ref) return;
		Independ();
		Ptr = RisseAppendVariantString(Ptr, ref);
	}

	void operator +=(const risse_char *ref)
	{
		if(!ref) return;
		Independ();
		Ptr = RisseAppendVariantString(Ptr, ref);
	}

	void operator +=(risse_char rch)
	{
		Independ();
		risse_char ch[2];
		ch[0] = rch;
		ch[1] = 0;
		Ptr = RisseAppendVariantString(Ptr, ch);
	}

	tRisseString operator +(const tRisseString &ref) const
	{
		if(!ref.Ptr && !Ptr) return tRisseString();
		if(!ref.Ptr) return *this;
		if(!Ptr) return ref;

		tRisseString newstr;
		newstr.Ptr = RisseAllocVariantString(*this->Ptr, *ref.Ptr);
		return newstr;
	}

	tRisseString operator +(const risse_char *ref) const
	{
		if(!ref && !Ptr) return tRisseString();
		if(!ref) return *this;
		if(!Ptr) return tRisseString(ref);

		tRisseString newstr;
		newstr.Ptr = RisseAllocVariantString(*this->Ptr, ref);
		return newstr;
	}

	tRisseString operator +(risse_char rch) const
	{
		if(!Ptr) return tRisseString(rch);
		risse_char ch[2];
		ch[0] = rch;
		ch[1] = 0;
		tRisseString newstr;
		newstr.Ptr = RisseAllocVariantString(*this->Ptr, ch);
		return newstr;
	}

	friend tRisseString operator + (const risse_char *lhs, const tRisseString &rhs);

	risse_char operator [](risse_uint i) const
	{
		// returns character at i. this function does not check the range.
		if(!Ptr) return 0;
		return Ptr->operator const risse_char *() [i];
	}

	void Clear()
	{
		if(Ptr) Ptr->Release(), Ptr = NULL;
	}

	risse_char * AllocBuffer(risse_uint len)
	{
		/* you must call FixLen when you allocate larger buffer than actual string length */

		if(Ptr) Ptr->Release();
		Ptr = RisseAllocVariantStringBuffer(len);
		return const_cast<risse_char*>(Ptr->operator const risse_char *());
	}

	risse_char * AppendBuffer(risse_uint len)
	{
		/* you must call FixLen when you allocate larger buffer than actual string length */

		if(!Ptr) return AllocBuffer(len);
		Independ();
		Ptr->AppendBuffer(len);
		return const_cast<risse_char *>(Ptr->operator const risse_char *());
	}


	void FixLen()
	{
		Independ();
		if(Ptr) Ptr = Ptr->FixLength();
	}

	void Replace
		(const tRisseString &from, const tRisseString &to, bool forall = true);

	tRisseString AsLowerCase() const;
	tRisseString AsUpperCase() const;

	void AsLowerCase(tRisseString &dest) const { dest = AsLowerCase(); }
	void AsUpperCase(tRisseString &dest) const { dest = AsUpperCase(); }

	void ToLowerCase();
	void ToUppserCase();

	risse_int Risse_cdecl printf(const risse_char *format, ...);

	tRisseString EscapeC() const;   // c-style string escape/unescaep
	tRisseString UnescapeC() const;

	void EscapeC(tRisseString &dest) const { dest = EscapeC(); }
	void UnescapeC(tRisseString &dest) const { dest = UnescapeC(); }

	bool StartsWith(const risse_char *string) const;
	bool StartsWith(const tRisseString & string) const
		{ return StartsWith(string.c_str()); }
	bool StartsWith(risse_char ch) const
		{
			if(GetLen() == 0)
				return ch == 0;
			return c_str()[0] == ch;
		}

	bool EndsWith(const risse_char *string) const;
	bool EndsWith(const tRisseString & string) const
		{ return StartsWith(string.c_str()); }
	bool EndsWith(risse_char ch) const
		{
			if(GetLen() == 0)
				return ch == 0;
			return c_str()[GetLen() - 1] == ch;
		}

	risse_uint32 * GetHint() const { if(!Ptr) return NULL; return Ptr->GetHint(); }

	//------------------------------------------------------------- others --
	bool IsEmpty() const { return Ptr==NULL; }

private:
	risse_char * InternalIndepend();

public:
	risse_char * Independ()
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
			return const_cast<risse_char *>(Ptr->operator const risse_char *());
		}
		return InternalIndepend();
	}


	risse_int GetLen() const
	{
#ifdef __CODEGUARD__
		if(!Ptr) return 0; // tRisseVariantString::GetLength can return zero if 'this' is NULL
#endif
		return Ptr->GetLength();
	}

	risse_int length() const { return GetLen(); }

	risse_char GetLastChar() const
	{
		if(!Ptr) return (risse_char)0;
		const risse_char * p = Ptr->operator const risse_char*();
		return p[Ptr->GetLength() - 1];
	}
};
/*end-of-tRisseString*/
tRisseString operator +(const risse_char *lhs, const tRisseString &rhs);


//---------------------------------------------------------------------------
tRisseString RisseInt32ToHex(risse_uint32 num, int zeropad = 8);
//---------------------------------------------------------------------------
/*[*/
typedef tRisseString ttstr;
/*]*/
extern tRisseString RisseEmptyString; // holds an empty string
//---------------------------------------------------------------------------
} // namespace Risse
#endif
