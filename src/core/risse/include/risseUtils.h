//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief ユーティリティ
//---------------------------------------------------------------------------
#ifndef risseUtilsH
#define risseUtilsH

#include "risseVariant.h"
#include "risseString.h"

namespace Risse
{
//---------------------------------------------------------------------------
// クリティカルセクション
//---------------------------------------------------------------------------
#ifdef RISSE_SUPPORT_WX
// wxWidgets サポートの場合
#include <wx/thread.h>

// wxWidgets の物を使う
typedef wxCriticalSection tRisseCriticalSection;

#elif _WIN32
// Windows プラットフォームの場合

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//! @brief クリティカルセクションの実装
class tRisseCriticalSection
{
	CRITICAL_SECTION CS; //!< Win32 クリティカルセクションオブジェクト
public:
	tRisseCriticalSection() { InitializeCriticalSection(&CS); } //!< コンストラクタ
	~tRisseCriticalSection() { DeleteCriticalSection(&CS); } //!< デストラクタ

	void Enter() { EnterCriticalSection(&CS); } //!< クリティカルセクションに入る
	void Leave() { LeaveCriticalSection(&CS); } //!< クリティカルセクションから出る
};
#else
// wxWidgets でも Windows でもない場合はサポートしない

class tRisseCriticalSection
{
public:
	tRisseCriticalSection() { ; }
	~tRisseCriticalSection() { ; }

	void Enter() { ; }
	void Leave() { ; }
};

#endif
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief クリティカルセクションを保持するクラス
//---------------------------------------------------------------------------
class tRisseCriticalSectionHolder
{
	tRisseCriticalSection *Section; //!< クリティカルセクションオブジェクト
public:

	//! @brief コンストラクタ
	//! @param クリティカルセクションオブジェクトへの参照
	tRisseCriticalSectionHolder(tRisseCriticalSection &cs)
	{
		Section = &cs;
		Section->Enter();
	}

	//! @brief デストラクタ
	~tRisseCriticalSectionHolder()
	{
		Section->Leave();
	}

};
typedef tRisseCriticalSectionHolder tRisseCSH;
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
// tRisseAtExit / tRisseAtStart
//---------------------------------------------------------------------------
class tRisseAtExit
{
	void (*Function)();
public:
	tRisseAtExit(void (*func)()) { Function = func; };
	~tRisseAtExit() { Function(); }
};
//---------------------------------------------------------------------------
class tRisseAtStart
{
public:
	tRisseAtStart(void (*func)()) { func(); };
};
//---------------------------------------------------------------------------
class iRisseDispatch2;
extern iRisseDispatch2 * RisseObjectTraceTarget;

#define RISSE_DEBUG_REFERENCE_BREAK \
	if(RisseObjectTraceTarget == (iRisseDispatch2*)this) RisseNativeDebuggerBreak()
#define RISSE_SET_REFERENCE_BREAK(x) RisseObjectTraceTarget=(x)
//---------------------------------------------------------------------------
const risse_char *RisseVariantTypeToTypeString(tRisseVariantType type);
	// convert given variant type to type string ( "void", "int", "object" etc.)

tRisseString RisseVariantToReadableString(const tRisseVariant &val, risse_int maxlen = 512);
	// convert given variant to human-readable string
	// ( eg. "(string)\"this is a\\nstring\"" )
tRisseString RisseVariantToExpressionString(const tRisseVariant &val);
	// convert given variant to string which can be interpret as an expression.
	// this function does not convert objects ( returns empty string )

//---------------------------------------------------------------------------



/*[*/
//---------------------------------------------------------------------------
// tRisseRefHolder : a object holder for classes that has AddRef and Release methods
//---------------------------------------------------------------------------
template <typename T>
class tRisseRefHolder
{
private:
	T *Object;
public:
	tRisseRefHolder(T * ref) { Object = ref; Object->AddRef(); }
	tRisseRefHolder(const tRisseRefHolder<T> &ref)
	{
		Object = ref.Object;
		Object->AddRef();
	}
	~tRisseRefHolder() { Object->Release(); }

	T* GetObject() { Object->AddRef(); return Object; }
	T* GetObjectNoAddRef() { return Object; }

	const tRisseRefHolder & operator = (const tRisseRefHolder & rhs)
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
// RisseAlignedAlloc : aligned memory allocater
//---------------------------------------------------------------------------
void *RisseAlignedAlloc(risse_uint bytes, risse_uint align_bits);
void RisseAlignedDealloc(void *ptr);
//---------------------------------------------------------------------------

/*[*/
//---------------------------------------------------------------------------
// floating-point class checker
//---------------------------------------------------------------------------
// constants used in RisseGetFPClass
#define RISSE_FC_CLASS_MASK 7
#define RISSE_FC_SIGN_MASK 8

#define RISSE_FC_CLASS_NORMAL 0
#define RISSE_FC_CLASS_NAN 1
#define RISSE_FC_CLASS_INF 2

#define RISSE_FC_IS_NORMAL(x)  (((x)&RISSE_FC_CLASS_MASK) == RISSE_FC_CLASS_NORMAL)
#define RISSE_FC_IS_NAN(x)  (((x)&RISSE_FC_CLASS_MASK) == RISSE_FC_CLASS_NAN)
#define RISSE_FC_IS_INF(x)  (((x)&RISSE_FC_CLASS_MASK) == RISSE_FC_CLASS_INF)

#define RISSE_FC_IS_NEGATIVE(x) ((bool)((x) & RISSE_FC_SIGN_MASK))
#define RISSE_FC_IS_POSITIVE(x) (!RISSE_FC_IS_NEGATIVE(x))


/*]*/
risse_uint32 RisseGetFPClass(risse_real r);
//---------------------------------------------------------------------------
}

#endif




