//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief ユーティリティ
//---------------------------------------------------------------------------
#ifndef tjsUtilsH
#define tjsUtilsH

#include "tjsVariant.h"
#include "tjsString.h"

namespace TJS
{
//---------------------------------------------------------------------------
// クリティカルセクション
//---------------------------------------------------------------------------
#ifdef TJS_SUPPORT_WX
// wxWidgets サポートの場合
#include <wx/thread.h>

// wxWidgets の物を使う
typedef wxCriticalSection tTJSCriticalSection;

#elif
// Windows プラットフォームの場合

#include <Windows.h>

//! @brief クリティカルセクションの実装
class tTJSCriticalSection
{
	CRITICAL_SECTION CS; //!< Win32 クリティカルセクションオブジェクト
public:
	tTJSCriticalSection() { InitializeCriticalSection(&CS); } //!< コンストラクタ
	~tTJSCriticalSection() { DeleteCriticalSection(&CS); } //!< デストラクタ

	void Enter() { EnterCriticalSection(&CS); } //!< クリティカルセクションに入る
	void Leave() { LeaveCriticalSection(&CS); } //!< クリティカルセクションから出る
};
#else
// wxWidgets でも Windows でもない場合はサポートしない

class tTJSCriticalSection
{
public:
	tTJSCriticalSection() { ; }
	~tTJSCriticalSection() { ; }

	void Enter() { ; }
	void Leave() { ; }
};

#endif
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief クリティカルセクションを保持するクラス
//---------------------------------------------------------------------------
class tTJSCriticalSectionHolder
{
	tTJSCriticalSection *Section; //!< クリティカルセクションオブジェクト
public:

	//! @brief コンストラクタ
	//! @param クリティカルセクションオブジェクトへの参照
	tTJSCriticalSectionHolder(tTJSCriticalSection &cs)
	{
		Section = &cs;
		Section->Enter();
	}

	//! @brief デストラクタ
	~tTJSCriticalSectionHolder()
	{
		Section->Leave();
	}

};
typedef tTJSCriticalSectionHolder tTJSCSH;
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
// tTJSAtExit / tTJSAtStart
//---------------------------------------------------------------------------
class tTJSAtExit
{
	void (*Function)();
public:
	tTJSAtExit(void (*func)()) { Function = func; };
	~tTJSAtExit() { Function(); }
};
//---------------------------------------------------------------------------
class tTJSAtStart
{
public:
	tTJSAtStart(void (*func)()) { func(); };
};
//---------------------------------------------------------------------------
class iTJSDispatch2;
extern iTJSDispatch2 * TJSObjectTraceTarget;

#define TJS_DEBUG_REFERENCE_BREAK \
	if(TJSObjectTraceTarget == (iTJSDispatch2*)this) TJSNativeDebuggerBreak()
#define TJS_SET_REFERENCE_BREAK(x) TJSObjectTraceTarget=(x)
//---------------------------------------------------------------------------
const tjs_char *TJSVariantTypeToTypeString(tTJSVariantType type);
	// convert given variant type to type string ( "void", "int", "object" etc.)

tTJSString TJSVariantToReadableString(const tTJSVariant &val, tjs_int maxlen = 512);
	// convert given variant to human-readable string
	// ( eg. "(string)\"this is a\\nstring\"" )
tTJSString TJSVariantToExpressionString(const tTJSVariant &val);
	// convert given variant to string which can be interpret as an expression.
	// this function does not convert objects ( returns empty string )

//---------------------------------------------------------------------------



/*[*/
//---------------------------------------------------------------------------
// tTJSRefHolder : a object holder for classes that has AddRef and Release methods
//---------------------------------------------------------------------------
template <typename T>
class tTJSRefHolder
{
private:
	T *Object;
public:
	tTJSRefHolder(T * ref) { Object = ref; Object->AddRef(); }
	tTJSRefHolder(const tTJSRefHolder<T> &ref)
	{
		Object = ref.Object;
		Object->AddRef();
	}
	~tTJSRefHolder() { Object->Release(); }

	T* GetObject() { Object->AddRef(); return Object; }
	T* GetObjectNoAddRef() { return Object; }

	const tTJSRefHolder & operator = (const tTJSRefHolder & rhs)
	{
		if(rhs.Object != Object)
		{
			Object->Release();
			Object = rhs.Object;
			Object->AddRef();
		}
		return *this;
	}
};



/*]*/

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TJSAlignedAlloc : aligned memory allocater
//---------------------------------------------------------------------------
void *TJSAlignedAlloc(tjs_uint bytes, tjs_uint align_bits);
void TJSAlignedDealloc(void *ptr);
//---------------------------------------------------------------------------

/*[*/
//---------------------------------------------------------------------------
// floating-point class checker
//---------------------------------------------------------------------------
// constants used in TJSGetFPClass
#define TJS_FC_CLASS_MASK 7
#define TJS_FC_SIGN_MASK 8

#define TJS_FC_CLASS_NORMAL 0
#define TJS_FC_CLASS_NAN 1
#define TJS_FC_CLASS_INF 2

#define TJS_FC_IS_NORMAL(x)  (((x)&TJS_FC_CLASS_MASK) == TJS_FC_CLASS_NORMAL)
#define TJS_FC_IS_NAN(x)  (((x)&TJS_FC_CLASS_MASK) == TJS_FC_CLASS_NAN)
#define TJS_FC_IS_INF(x)  (((x)&TJS_FC_CLASS_MASK) == TJS_FC_CLASS_INF)

#define TJS_FC_IS_NEGATIVE(x) ((bool)((x) & TJS_FC_SIGN_MASK))
#define TJS_FC_IS_POSITIVE(x) (!TJS_FC_IS_NEGATIVE(x))


/*]*/
tjs_uint32 TJSGetFPClass(tjs_real r);
//---------------------------------------------------------------------------
}

#endif




